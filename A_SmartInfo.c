#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NvmExpressPassthru.h>
#include "A_SmartInfo.h"
#include "Dbg.h"

//Get Smart/Health information log using Passthru protocol
VOID 
GetLogPage(
	IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL		*NvmePassThru,
	IN UINT8									LogIdentifier,				//0x02
	IN OUT UINT8								**TransferBuffer
)
{
	EFI_STATUS		Status;
	UINT32			TransferLength = 512;
	UINTN i = 0;
	UINTN j = 0;
	
	ResetNvmeCommandPacket();
	
	Status = gBS->AllocatePool(EfiBootServicesData,
								512,
								(VOID**)TransferBuffer);
	if(EFI_ERROR(Status))
	{
		Print(L"GetLogPage:gBS->AllocatePool\n");
		goto END;
	}
	
	gBS->SetMem(*TransferBuffer, TransferLength, 0);
	
	gNvmeCmd.Nsid 					= 0xFFFFFFFF;
	gNvmeCmd.Cdw0.Opcode 			= 0x02;
	gNvmeCmd.Cdw0.FusedOperation 	= 0;
	
	gNvmeCmd.Cdw10 = (TransferLength >> 2) - 1;
	gNvmeCmd.Cdw10 = gNvmeCmd.Cdw10 << 16 | LogIdentifier;		//0x02 for Smart/Health log    0xE0 for vendor specific E0H log page data
	
	gNvmeCmd.Flags |= CDW10_VALID;
	
	gNvmeCommandPacket.CommandTimeout 	= 10000000;
	gNvmeCommandPacket.NvmeCmd			= &gNvmeCmd;
	gNvmeCommandPacket.QueueType		= 0;
	gNvmeCommandPacket.NvmeCompletion	= &gNvmeCompletion;
	gNvmeCommandPacket.TransferBuffer	= *TransferBuffer;
	gNvmeCommandPacket.TransferLength	= TransferLength;
	
	//Send Command through Passthru API
	Status = NvmePassThru->PassThru(NvmePassThru, 
								0xFFFFFFFF,
								&gNvmeCommandPacket,
								NULL
								);
	if (EFI_ERROR(Status))
	{
		Print(L"GetLogPage:NvmePassThru->PassThru\n");
		goto DELETE;
	}
	
	Dbg_Print("GetLogPage:Begin\n");
	for(i = 0; i < 512; i++)
	{
		if(i % 16 == 0)
		{
			Dbg_Print("\n");
			Dbg_Print("%04x :", j);
			
			j += 1;
		}
		Dbg_Print("%02x ", (*TransferBuffer)[i]);
	}
	Dbg_Print("\n");
	Dbg_Print("GetLogPage:End\n");
	
DELETE:
	if(*TransferBuffer != NULL)
	{
		gBS->FreePool(*TransferBuffer);
		*TransferBuffer = NULL;
	}
END:
	;
}