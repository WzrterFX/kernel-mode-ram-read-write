/*
    File Uma.cpp
    By WzrterFX
*/

#include "Uma.h"

namespace Uma {
    Uma::Uma() : _kmdHandle(nullptr, &CloseHandle), _processId(NULL) {
        HANDLE kmdHandle = CreateFile(
            "\\\\.\\Kmd", GENERIC_READ, NULL, nullptr,

            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr
        );
        if (kmdHandle == INVALID_HANDLE_VALUE)
            throw std::runtime_error(
                "Fatal, failed to connect to driver make sure it is available."
            );

        _kmdHandle.reset(kmdHandle);
    }

    void Uma::Attach(const std::wstring_view& name) {
        HANDLE snapShot = CreateToolhelp32Snapshot(
            TH32CS_SNAPPROCESS, NULL
        );
        if (snapShot == INVALID_HANDLE_VALUE)
            throw std::runtime_error(
                "Fatal, failed to create process list."
            );

        PROCESSENTRY32W process{ sizeof(process) };

        if (Process32FirstW(snapShot, &process)) {
            do {
                if (!_wcsicmp(process.szExeFile, name.data())) {
                    _processId = process.th32ProcessID;

                    break;
                }
            } while (Process32NextW(snapShot, &process));
        }

        CloseHandle(snapShot);

        if (_processId == NULL)
            throw std::runtime_error(
                "Fatal, missing process check if process available."
            );

        KmdRequest request;
        request.attachRequest.process = reinterpret_cast<HANDLE>(_processId);

        if (!DeviceIoControl(
            _kmdHandle.get(), ::Uma::_IoCtls::attach,
            &request, sizeof(request), &request, sizeof(request), nullptr, nullptr
        )) throw std::runtime_error("Fatal, driver attach request failed.");
    }

    std::uintptr_t Uma::ReadModule(const std::wstring_view& name) const {
        HANDLE snapShot = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _processId
        );

        MODULEENTRY32W module { sizeof(module) };

        if (Module32FirstW(snapShot, &module)) {
            do {
                if (!_wcsicmp(module.szModule, name.data())) {
                    CloseHandle(snapShot);

                    return reinterpret_cast<std::uintptr_t>(module.modBaseAddr);
                }
            } while (Module32NextW(snapShot, &module));
        }

        CloseHandle(snapShot);
        return NULL;
    }
}