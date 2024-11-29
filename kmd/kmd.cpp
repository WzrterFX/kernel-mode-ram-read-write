/*
	File Kmd.cpp
	By WzrterFX
*/

#include "Kmd.h"

namespace Kmd {
	NTSTATUS Kmd::Create(PDRIVER_OBJECT driverObject) {
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		UNICODE_STRING deviceName { };
		RtlInitUnicodeString(&deviceName, L"\\Device\\Kmd");

		status = IoCreateDevice(
			driverObject, NULL, &deviceName,
			FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &_deviceObject
		);
		if (!NT_SUCCESS(status)) {
			DbgPrintEx(0, 0,
				"Fatal, failed to create driver device.\n"
			);

			return status;
		}

		UNICODE_STRING symbolicLink { };
		RtlInitUnicodeString(&symbolicLink, L"\\DosDevices\\Kmd");

		status = IoCreateSymbolicLink(&symbolicLink, &deviceName);
		if (!NT_SUCCESS(status)) {
			DbgPrintEx(0, 0,
				"Fatal, failed to establish driver link.\n"
			);

			IoDeleteDevice(_deviceObject);

			return status;
		}

		SetFlag(_deviceObject->Flags, DO_BUFFERED_IO);

		driverObject->MajorFunction[IRP_MJ_CREATE] = [](PDEVICE_OBJECT, PIRP io) -> NTSTATUS {
				IoCompleteRequest(io, IO_NO_INCREMENT);

				return io->IoStatus.Status;
			};

		driverObject->MajorFunction[IRP_MJ_CLOSE] = [](PDEVICE_OBJECT, PIRP io) -> NTSTATUS {
				IoCompleteRequest(io, IO_NO_INCREMENT);

				return io->IoStatus.Status;
			};

		driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = &this->KmdControl;

		ClearFlag(_deviceObject->Flags, DO_DEVICE_INITIALIZING);

		return STATUS_SUCCESS;
	}

	NTSTATUS Kmd::KmdControl(PDEVICE_OBJECT, PIRP io) {
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(io);
		if (!stack) {
			IoCompleteRequest(io, IO_NO_INCREMENT);

			DbgPrintEx(0, 0,
				"Fatal, missing driver stack location.\n"
			);

			return status;
		}

		PKmdRequest request = reinterpret_cast<PKmdRequest>(io->AssociatedIrp.SystemBuffer);
		if (!request) {
			IoCompleteRequest(io, IO_NO_INCREMENT);

			DbgPrintEx(0, 0,
				"Fatal, missing driver associated request.\n"
			);

			return status;
		}

		static PEPROCESS process { };
		static SIZE_T reserved { };

		switch (stack->Parameters.DeviceIoControl.IoControlCode) {
			case ::Kmd::_IoCtls::attach: {
				status = ::Kmd::_NtifsApi::PsLookupProcessByProcessId(
					request->attachRequest.process, &process
				);

				break;
			}

			case ::Kmd::_IoCtls::read: {
				if (process) {
					status = ::Kmd::_NtifsApi::MmCopyVirtualMemory(
						process, request->copyMemoryRequest.from, PsGetCurrentProcess(), request->copyMemoryRequest.to,
						request->copyMemoryRequest.requested,

						MODE::KernelMode, &reserved
					);
				}

				break;
			}

			case ::Kmd::_IoCtls::write: {
				if (process) {
					status = ::Kmd::_NtifsApi::MmCopyVirtualMemory(
						PsGetCurrentProcess(), request->copyMemoryRequest.to, process, request->copyMemoryRequest.from,
						request->copyMemoryRequest.requested,

						MODE::KernelMode, &reserved
					);
				}

				break;
			}

			default: {
				status = STATUS_INVALID_DEVICE_REQUEST;

				break;
			}
		}

		io->IoStatus.Status = status;
		io->IoStatus.Information = sizeof(KmdRequest);

		IoCompleteRequest(io, IO_NO_INCREMENT);

		return status;
	}
}