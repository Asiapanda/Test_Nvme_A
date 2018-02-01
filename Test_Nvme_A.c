//SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello man.\n welcome to UEFI world\n"); //可以将信息打印在UEFI Shell上面
#include "Test_Nvme_A.h"
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NvmExpressPassthru.h>

//-----------------------------------
#include "Dbg.h"
#include "A_Nvme.h"

EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  *gPassthru = NULL;
EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET gNvmeCommandPacket;
EFI_NVM_EXPRESS_COMMAND	gNvmeCmd;
EFI_NVM_EXPRESS_COMPLETION gNvmeCompletion;

VOID
ResetNvmeCommandPacket()
{
    
    // Clear the Global gNvmeCommandPacket structure to Zero
    gBS->SetMem(&gNvmeCommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET), 0 );

    // Clear Global Nvm Express Admin Command to Zero
    gBS->SetMem(&gNvmeCmd, sizeof(EFI_NVM_EXPRESS_COMMAND), 0);
    
    // Clear Global Nvm Express completion structure to Zero
    gBS->SetMem(&gNvmeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION), 0);
}

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
	
	Status = gPassthru->PassThru(gPassthru,
								0xFFFFFFFF,
								&gNvmeCommandPacket,
								NULL);
	//Print(L"Status %r\n", Status);
	if (EFI_ERROR(Status)){
		Print(L"GetFirmwareSlotInfo:gPassthru->PassThru\n");
		goto DELETE;
	}
	
	CurrentActiveSlot = (*TransferBuffer)->ActiveFirmwareInfo & 0x07;
	Print(L"GetFirmwareSlotInfo:Begin\n");
	Print(L"CurrentActiveSlot %X\n", CurrentActiveSlot);
	Print(L"GetFirmwareSlotInfo:End\n");
DELETE:
	if(*TransferBuffer != NULL)
	{
		gBS->FreePool(*TransferBuffer);
		*TransferBuffer = NULL;
	}
END:
	;
}

//Gets Identify Controller Data
VOID
GetIdentifyData(
	IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL		*NvmePassThru,
	IN OUT UINT8								**TransferBuffer
)	
{
	
}

//Get Smart/Health information log using Passthru protocol
VOID 
GetLogPage(
	IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL		*NvmePassThru,
	IN UINT8									LogIdentifier,
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
	Status = gPassthru->PassThru(gPassthru, 
								0xFFFFFFFF,
								&gNvmeCommandPacket,
								NULL
								);
	if (EFI_ERROR(Status))
	{
		Print(L"GetLogPage:gPassthru->PassThru\n");
		goto DELETE;
	}
	
	Print(L"GetLogPage:Begin\n");
	for(i = 0; i < 512; i++)
	{
		if(i % 16 == 0)
		{
			Print(L"\n");
			Print(L"%04x :", j);
			
			j += 1;
		}
		Print(L"%02x ", (*TransferBuffer)[i]);
	}
	Print(L"\n");
	Print(L"GetLogPage:End\n");
	
DELETE:
	if(*TransferBuffer != NULL)
	{
		gBS->FreePool(*TransferBuffer);
		*TransferBuffer = NULL;
	}
END:
	;
}

EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{

	//EFI_STATUS  Status;
	NVME_FIRMWARE_SLOT_INFO		*Nvme_firmware_slot_info_TransferBuffer = NULL;
	UINT8						*Uint8_TransferBuffer = NULL;
	Dbg_Init(ImageHandle);
	Nvme_Init(ImageHandle, &gPassthru);
	
	if(gPassthru == NULL)
	{
		SystemTable->ConOut->OutputString(SystemTable->ConOut, L"gPassthru is NULL\n"); 
		goto END;
	}
	
	//Gets the Firmware Slot information
	GetFirmwareSlotInfo(gPassthru, &Nvme_firmware_slot_info_TransferBuffer);
	
	//Gets Identify Controller Data
	//GetIdentifyData(gPassthru);
	
	//Get Smart/Health information log using Passthru protocol
	GetLogPage(gPassthru, 0x02, &Uint8_TransferBuffer);

END:
	Nvme_UnInit(ImageHandle);
	Dbg_UnInit(ImageHandle);
	return EFI_SUCCESS;
}











	/*
	UINT8		*E0LogPageBuffer;
	
	UINTN i = 0;
	UINTN j = 0;
	 Status = gBS->AllocatePool( EfiBootServicesData,
							512,
							(VOID**)&E0LogPageBuffer);
	if (EFI_ERROR(Status)){
		SystemTable->ConOut->OutputString(SystemTable->ConOut, L"gBS->AllocatePool\n"); 
		goto END;
	}
	
	gNvmeCmd.Cdw0.Opcode			= 0x02;
	gNvmeCmd.Cdw0.FusedOperation	= 0;
	gNvmeCmd.Cdw0.Reserved		= 0;
	
	
	gNvmeCmd.Flags		|= CDW10_VALID;
	gNvmeCmd.Nsid		= 0xFFFFFFFF;
	gNvmeCmd.Cdw2		= 0;
	gNvmeCmd.Cdw3		= 0;
	gNvmeCmd.Cdw10		= (512 >> 2) - 1;
	gNvmeCmd.Cdw10		= gNvmeCmd.Cdw10 << 16 | 0xE0;
	gNvmeCmd.Cdw11		= 0;
	gNvmeCmd.Cdw12		= 0;
	gNvmeCmd.Cdw13		= 0;
	gNvmeCmd.Cdw14		= 0;
	gNvmeCmd.Cdw15		= 0;

	gNvmeCompletion.DW0		= 0;
	gNvmeCompletion.DW1		= 0;
	gNvmeCompletion.DW2		= 0;
	gNvmeCompletion.DW3		= 0;
	
	
	gNvmeCommandPacket.CommandTimeout 	= 10000000; 
	gNvmeCommandPacket.TransferBuffer 	= E0LogPageBuffer;
	gNvmeCommandPacket.TransferLength 	= 512;
	gNvmeCommandPacket.MetadataBuffer 	= NULL;
	gNvmeCommandPacket.MetadataLength 	= 0;
	gNvmeCommandPacket.QueueType 		= 0;
	gNvmeCommandPacket.NvmeCmd 		= &gNvmeCmd;
	gNvmeCommandPacket.NvmeCompletion 	= &gNvmeCompletion;
	
	gBS->SetMem(gNvmeCommandPacket.TransferBuffer, gNvmeCommandPacket.TransferLength, 0);
	Status = gPassthru->PassThru(gPassthru, 	//This. in the c++ this *
					0xFFFFFFFF, 	//0 is nvme controller, 0xff is all namespace
					&gNvmeCommandPacket, 		//packet
					NULL);			
	if(EFI_ERROR(Status)){
		SystemTable->ConOut->OutputString(SystemTable->ConOut, L"gPassthru->PassThru\n"); 
		goto DELETE;
	}
	
	if(E0LogPageBuffer == NULL) {
		SystemTable->ConOut->OutputString(SystemTable->ConOut, L"E0LogPageBuffer == NULL\n"); 
		goto DELETE;
	}
	
	for(i = 0; i < 512; i++)
	{
		if(i % 16 == 0)
		{
			Print(L"\n");
			Print(L"%04x :", j);
			
			j += 1;
		}
		Print(L"%02x ", E0LogPageBuffer[i]);
	}
	Print(L"\n");
	
DELETE:
	if(E0LogPageBuffer != NULL)
	{
		gBS->FreePool(E0LogPageBuffer);
		E0LogPageBuffer = NULL;
	} */

