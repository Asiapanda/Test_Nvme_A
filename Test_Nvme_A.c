#include "Test_Nvme_A.h"
#include <Uefi.h>
#include <Base.h>


EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello man.\n welcome to UEFI world\n");
	return EFI_SUCCESS;
}
