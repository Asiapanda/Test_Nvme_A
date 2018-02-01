#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NvmExpressPassthru.h>


#include "A_Nvme.h"
#include "Dbg.h"


//gEfiNvmExpressPassThruProtocolGuid
static EFI_HANDLE *gHandles = NULL;
static INTN gHandle_num;


EFI_STATUS
EFIAPI
Nvme_Init(
	IN EFI_HANDLE		ImageHandle,
	EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  **gPassthru
	)
{
	EFI_STATUS			Status;
	UINTN				Index;

 
	if (*gPassthru != NULL)
		return EFI_SUCCESS;

	Index			= 0;
	gHandle_num		= 0;
	gHandles		= NULL;

	Status = gBS->LocateHandle (
			ByProtocol,
			&gEfiNvmExpressPassThruProtocolGuid,
			(void **)gPassthru,
			&gHandle_num,
			gHandles
			);

	if (Status != EFI_BUFFER_TOO_SMALL) {
		Print(L"There are no Nvme to the system.\n");
		Dbg_Print("There are no Nvme to the system.\n");
		return EFI_SUCCESS;
	}
	
	gHandles = AllocateZeroPool (sizeof (EFI_HANDLE) * gHandle_num);
	if (gHandles == NULL) {
		Print(L"Out of memory\n");
		Dbg_Print("Out of memory\n");
		return EFI_SUCCESS;
	}

	Status = gBS->LocateHandle (
			ByProtocol,
			&gEfiNvmExpressPassThruProtocolGuid,
			(void **)gPassthru,
			&gHandle_num,
			gHandles
			);

	Index = 0;
	Status = gBS->OpenProtocol (
			gHandles[Index],
			&gEfiNvmExpressPassThruProtocolGuid,
			(void **)gPassthru,
			ImageHandle,
			NULL,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
			);
	if (EFI_ERROR (Status)) {
		Print(L"Unexpected error accessing protocol\n");
		Dbg_Print("Unexpected error accessing protocol\n");
		return EFI_SUCCESS;
	}

	return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
Nvme_UnInit (
	IN EFI_HANDLE			ImageHandle
	)
{
	INTN i;

	if (gHandles != NULL)
	{
		for (i = 0; i < gHandle_num; i++)
		{
			gBS->CloseProtocol (
					gHandles[i],
					&gEfiNvmExpressPassThruProtocolGuid,
					ImageHandle,
					NULL
					);
		}

		FreePool (gHandles);
		gHandles = NULL;
	}
	return EFI_SUCCESS;
}
