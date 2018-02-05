#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NvmExpressPassthru.h>

#include "A_FirmwareInfo.h"
#include "Dbg.h"

//Gets the Firmware Slot information    
VOID
GetFirmwareSlotInfo(
	IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL		*NvmePassThru,
	IN OUT NVME_FIRMWARE_SLOT_INFO				**TransferBuffer
)
{
	EFI_STATUS  Status;
	UINT32						TransferLength = sizeof(NVME_FIRMWARE_SLOT_INFO);
	UINT8						CurrentActiveSlot;
	CHAR16						*FirmwareVersion = NULL;
	UINTN i = 0;
	UINTN j = 0;
	
	Status = gBS->AllocatePool(EfiBootServicesData, sizeof(NVME_FIRMWARE_SLOT_INFO), (VOID**)TransferBuffer);
	if(EFI_ERROR(Status)){
		Print(L"GetFirmwareSlotInfo:gBS->ALlocatePool\n");
		goto END;
	}

	gBS->SetMem(*TransferBuffer, sizeof(NVME_FIRMWARE_SLOT_INFO), 0);
	
	ResetNvmeCommandPacket();
	
	gNvmeCmd.Cdw0.Opcode = 0x02;
	gNvmeCmd.Cdw0.FusedOperation = 0;
	gNvmeCmd.Nsid = 0xFFFFFFFF;
	
	gNvmeCmd.Cdw10 = (TransferLength >> 2) - 1;
	gNvmeCmd.Cdw10 = gNvmeCmd.Cdw10 << 16 | 0x3;
	gNvmeCmd.Flags |= CDW10_VALID;
	
	gNvmeCommandPacket.CommandTimeout = 10000000;
	gNvmeCommandPacket.NvmeCmd = &gNvmeCmd;
	gNvmeCommandPacket.QueueType = 0;
	gNvmeCommandPacket.NvmeCompletion = &gNvmeCompletion;
	gNvmeCommandPacket.TransferBuffer = *TransferBuffer;
	gNvmeCommandPacket.TransferLength = TransferLength;
	
	Status = NvmePassThru->PassThru(NvmePassThru,
								0xFFFFFFFF,
								&gNvmeCommandPacket,
								NULL);
	//Print(L"Status %r\n", Status);
	if (EFI_ERROR(Status)){
		Print(L"GetFirmwareSlotInfo:NvmePassThru->PassThru\n");
		goto DELETE;
	}
	
	Status = gBS->AllocatePool(EfiBootServicesData, 0x20, &FirmwareVersion);
	if (EFI_ERROR(Status))
	{
		Print(L"GetFirmwareSlotInfo:gBS->AllocatePool\n");
		goto DELETE;
	}
	gBS->SetMem(FirmwareVersion, (UINTN)0x20, 0);
	
	CurrentActiveSlot = (*TransferBuffer)->ActiveFirmwareInfo & 0x07;
	gBS->CopyMem(FirmwareVersion, ((UINT64*)*TransferBuffer + CurrentActiveSlot), 8);
	//Print(L"FirmwareSlotCount: %S\n", (*TransferBuffer)->FirmwareSlotCount);
	
	Dbg_Print("GetFirmwareSlotInfo:Begin\n");
	Dbg_Print("FirmwareVersion: %S\n", FirmwareVersion);
	Dbg_Print("CurrentActiveSlot: %X\n", CurrentActiveSlot);
	Dbg_Print("GetFirmwareSlotInfo:End\n");
	
	
	Dbg_Print("GetFirmwareSlotInfo:Begin\n");
	for(i = 0; i < TransferLength; i++)
	{
		if(i % 16 == 0)
		{
			Dbg_Print("\n");
			Dbg_Print("%04x :", j);
			
			j += 1;
		}
		Dbg_Print("%02x ", ((UINT8*)(*TransferBuffer))[i]);
	}
	Dbg_Print("\n");
	Dbg_Print("GetFirmwareSlotInfo:End\n");
DELETE:
	if(*TransferBuffer != NULL)
	{
		gBS->FreePool(*TransferBuffer);
		*TransferBuffer = NULL;
	}
	if(FirmwareVersion != NULL)
	{
		gBS->FreePool(FirmwareVersion);
		FirmwareVersion = NULL;
	}
END:
	;
}
