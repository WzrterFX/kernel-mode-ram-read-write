/*
    File Kmd.h
    By WzrterFX
*/

#pragma once

#ifndef KMD_H
#define KMD_H

#include <cstdint>

#include <ntifs.h>

namespace Kmd {
    namespace _NtifsApi {
        extern "C" __declspec(dllimport)
            NTSTATUS __stdcall
            IoCreateDriver(
                IN PUNICODE_STRING     DriverName,
                IN PDRIVER_INITIALIZE  InitializationFunction
            );

        extern "C" __declspec(dllimport)
            NTSTATUS __stdcall
            PsLookupProcessByProcessId(
                IN  HANDLE      ProcessId,
                OUT PEPROCESS* Process
            );

        extern "C" __declspec(dllimport)
            NTSTATUS __stdcall
            MmCopyVirtualMemory(
                IN PEPROCESS        FromProcess,
                IN PVOID            FromAddress,
                IN PEPROCESS        ToProcess,
                IN PVOID            ToAddress,
                IN SIZE_T           BufferSize,
                IN KPROCESSOR_MODE  PreviousMode,
                OUT PSIZE_T         NumberOfBytesCopied
            );
    }

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

    class Kmd {
    private:
        PDEVICE_OBJECT _deviceObject;

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

        static NTSTATUS KmdControl(PDEVICE_OBJECT deviceObject, PIRP io);

    public:
        NTSTATUS Create(PDRIVER_OBJECT driverObject);
    };
}

#endif /* !KMD_H */