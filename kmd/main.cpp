#include "kmd.h"

NTSTATUS KmdMain(PDRIVER_OBJECT DriverObject, PUNICODE_STRING) {
	Kmd::Kmd kmd;

	return kmd.Create(DriverObject);
}

NTSTATUS KmdLoad() {
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	UNICODE_STRING driverName { };
	RtlInitUnicodeString(&driverName, L"\\Driver\\Kmd");

	status = ::Kmd::_NtifsApi::IoCreateDriver(&driverName, KmdMain);
	if (!NT_SUCCESS(status))
		DbgPrintEx(0, 0,
			"Fatal, failed create driver, service is running or loaded incorrect.\n"
		);

	return status;
}