/*
    File Uma.h
    By WzrterFX
*/

#pragma once

#ifndef UMA_H
#define UMA_H

#include <memory>
#include <stdexcept>

#include <windows.h>
#include <tlhelp32.h>

namespace Uma {
    namespace _IoCtls {
        constexpr std::uint32_t attach = CTL_CODE(FILE_DEVICE_UNKNOWN,
            0x01, METHOD_BUFFERED, FILE_SPECIAL_ACCESS
        );

        constexpr std::uint32_t read = CTL_CODE(FILE_DEVICE_UNKNOWN,
            0x02, METHOD_BUFFERED, FILE_SPECIAL_ACCESS
        );

        constexpr std::uint32_t write = CTL_CODE(FILE_DEVICE_UNKNOWN,
            0x03, METHOD_BUFFERED, FILE_SPECIAL_ACCESS
        );
    }

    class Uma {
    private:
        std::unique_ptr<void, decltype(&CloseHandle)> _kmdHandle;
        std::uint32_t _processId;

        typedef struct __AttachRequest {
            HANDLE process;
        } AttachRequest, * PAttachRequest;

        typedef struct __CopyMemoryRequest {
            PVOID from; PVOID to; SIZE_T requested;
        } CopyMemoryRequest, * PCopyMemoryRequest;

        typedef struct _KmdRequest {
            AttachRequest attachRequest;
            CopyMemoryRequest copyMemoryRequest;
        } KmdRequest, * PKmdRequest;

    public:
        Uma();

        void Attach(const std::wstring_view& name);

        std::uintptr_t ReadModule(const std::wstring_view& name) const;

        template<typename T>
        T ReadMemory(const std::uintptr_t address);

        template<typename T>
        void WriteMemory(const std::uintptr_t address, const T& variable);
    };

    template<typename T>
    T Uma::ReadMemory(const std::uintptr_t address) {
        T result { };

        KmdRequest request;

        request.copyMemoryRequest.from = reinterpret_cast<PVOID>(address);
        request.copyMemoryRequest.to = &result;
        request.copyMemoryRequest.requested = sizeof(result);

        if (!DeviceIoControl(
            _kmdHandle.get(), ::Uma::_IoCtls::read,
            &request, sizeof(request), &request, sizeof(request), nullptr, nullptr
        )) return NULL;

        return result;
    }

    template<typename T>
    void Uma::WriteMemory(const std::uintptr_t address, const T& variable) {
        KmdRequest request;

        request.copyMemoryRequest.from = reinterpret_cast<PVOID>(address);
        request.copyMemoryRequest.to = (PVOID)&variable;
        request.copyMemoryRequest.requested = sizeof(variable);

        DeviceIoControl(
            _kmdHandle.get(), ::Uma::_IoCtls::write,
            &request, sizeof(request), &request, sizeof(request), nullptr, nullptr
        );
    }
}

#endif /* !UMA_H */