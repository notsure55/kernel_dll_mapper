#include "driver.hpp"
#include "requests/requests.hpp"

VOID UnloadDriver(PDRIVER_OBJECT DriverObject);
NTSTATUS OnCreateClose(PDEVICE_OBJECT, PIRP Irp);
NTSTATUS OnDeviceControl(PDEVICE_OBJECT, PIRP Irp);

extern "C" {
    NTKERNELAPI NTSTATUS IoCreateDriver(
        PUNICODE_STRING DriverName,
        PDRIVER_INITIALIZE InitializationFunction
    );
}

NTSTATUS real_main(PDRIVER_OBJECT DriverObject, PUNICODE_STRING registry_path) {
    UNREFERENCED_PARAMETER(registry_path);
    // functions are driver supports
    DriverObject->DriverUnload = UnloadDriver;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = OnCreateClose; // create file
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = OnCreateClose; // close handle
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = OnDeviceControl;

    UNICODE_STRING dev_name;
    RtlInitUnicodeString(&dev_name, L"\\Device\\meatdriver");
    PDEVICE_OBJECT dev_obj;

    auto status = IoCreateDevice(DriverObject, 0, &dev_name,
        FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev_obj);

    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to create device (0x%X)\n", status);
        return status;
    }

    // create symbolic link to expose device to user mode
    UNICODE_STRING sym_link;
    RtlInitUnicodeString(&sym_link, L"\\DosDevices\\meatdriver");
    status = IoCreateSymbolicLink(&sym_link, &dev_name);

    SetFlag(dev_obj->Flags, DO_BUFFERED_IO);

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(dev_obj);
        return status;
    }

    ClearFlag(dev_obj->Flags, DO_DEVICE_INITIALIZING);

    return STATUS_SUCCESS;
}

extern "C" NTSTATUS DriverEntry() {
    UNICODE_STRING  drv_name;
    RtlInitUnicodeString(&drv_name, L"\\Driver\\meatdriver");
    return IoCreateDriver(&drv_name, &real_main); //so it's kdmapper-able
}

VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{
    DbgPrint("MeatDriver unload!");

    // cleanup
    // delete sym link
    UNICODE_STRING sym_link;
    RtlInitUnicodeString(&sym_link, L"\\DosDevices\\meatdriver");
    IoDeleteSymbolicLink(&sym_link);
    // delete device objectxs
    IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS OnCreateClose(PDEVICE_OBJECT, PIRP Irp)
{
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, 0);

    return STATUS_SUCCESS;
}

NTSTATUS OnDeviceControl(PDEVICE_OBJECT, PIRP Irp)
{
    // process attached too
    static PEPROCESS process;
    // get i/o request stack location so we can access params
    const auto irp_sp{ IoGetCurrentIrpStackLocation(Irp) };

    // grab device io control param
    const auto& dic{ irp_sp->Parameters.DeviceIoControl };

    // init status
    auto status{ STATUS_INVALID_DEVICE_REQUEST };

    if (Irp->AssociatedIrp.SystemBuffer == nullptr) {
        status = STATUS_INVALID_PARAMETER;
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, 0);
        return status;
    }

    switch (dic.IoControlCode) {
	case IOCTL_OPEN_PROCESS: {
        status = Requests::attach_process(Irp->AssociatedIrp.SystemBuffer);
        Irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_READ_PROCESS: {
        Driver::ReadResult res{};
        status = Requests::read_memory(Irp->AssociatedIrp.SystemBuffer, &res);
        if (NT_SUCCESS(status)) {
            memcpy(Irp->AssociatedIrp.SystemBuffer, &res, sizeof(Driver::ReadResult));
            Irp->IoStatus.Information = sizeof(Driver::ReadResult);
        }

        break;
    }
    case IOCTL_WRITE_PROCESS: {
        Driver::WriteResult res{};
        status = Requests::write_memory(Irp->AssociatedIrp.SystemBuffer, &res);
        
        if (NT_SUCCESS(status)) {
            memcpy(Irp->AssociatedIrp.SystemBuffer, &res, sizeof(Driver::WriteResult));
            Irp->IoStatus.Information = sizeof(Driver::WriteResult);
        }
        break;
    }
    case IOCTL_ALLOC_PROCESS: {
        Driver::AllocResult res{};
        status = Requests::alloc_memory(Irp->AssociatedIrp.SystemBuffer, &res);

        if (NT_SUCCESS(status)) {
            memcpy(Irp->AssociatedIrp.SystemBuffer, &res, sizeof(Driver::AllocResult));
            Irp->IoStatus.Information = sizeof(Driver::AllocResult);
        }
        break;
    }
    case IOCTL_CREATE_THREAD_PROCESS: {
        Driver::ThreadResult res{};
        status = Requests::create_thread(Irp->AssociatedIrp.SystemBuffer, &res);

        if (NT_SUCCESS(status)) {
            memcpy(Irp->AssociatedIrp.SystemBuffer, &res, sizeof(Driver::ThreadResult));
            Irp->IoStatus.Information = sizeof(Driver::ThreadResult);
        }
        break;
    }
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, 0);
    return status;
}
