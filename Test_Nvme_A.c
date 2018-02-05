//SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello man.\n welcome to UEFI world\n"); //可以将信息打印在UEFI Shell上面
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
#include "A_SmartInfo.h"
#include "A_FirmwareInfo.h"
#include "Test_Nvme_A.h"

//Gets Identify Controller Data

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


VOID
GetIdentifyData(
	IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL		*NvmePassThru,
	IN OUT IDENTIFY_CONTROLLER_DATA				**TransferBuffer,
	IN UINT32									ControllerNameSpaceStructure,   // 1
	IN UINT32									NameSpaceID						// 0
)	
{
	EFI_STATUS		Status;
	UINTN 			i = 0;
	UINTN 			j = 0;
	UINT32			TransferLength = sizeof(IDENTIFY_CONTROLLER_DATA);
	
	Status = gBS->AllocatePool (EfiBootServicesData,
								sizeof(IDENTIFY_CONTROLLER_DATA),
								(VOID**)TransferBuffer);
	if (EFI_ERROR(Status))
	{
		Print(L"GetIdentifyData:gBS->AllocatePool\n");
		goto END;
	}
	ResetNvmeCommandPacket();
	gBS->SetMem(*TransferBuffer, sizeof(IDENTIFY_CONTROLLER_DATA), 0);
	
	gNvmeCmd.Nsid 					=  NameSpaceID;
	gNvmeCmd.Cdw0.Opcode 			=  0x06;
	gNvmeCmd.Cdw0.FusedOperation	=  0;
	gNvmeCmd.Cdw10 					=  ControllerNameSpaceStructure;
	gNvmeCmd.Flags				   |= CDW10_VALID;
	
	gNvmeCommandPacket.CommandTimeout	= 10000000;
	gNvmeCommandPacket.NvmeCmd			= &gNvmeCmd;
	gNvmeCommandPacket.QueueType		= 0;
	gNvmeCommandPacket.NvmeCompletion	= &gNvmeCompletion;
	gNvmeCommandPacket.TransferBuffer	= (VOID*)*TransferBuffer;
	gNvmeCommandPacket.TransferLength	= TransferLength;
	
	// Send Command through Passthru API
	Status = NvmePassThru->PassThru(NvmePassThru,
									NameSpaceID,
									&gNvmeCommandPacket,
									NULL);
	if (EFI_ERROR(Status))
	{
		Print(L"GetIdentifyData:NvmePassThru->PassThru\n");
		goto DELETE;
	}
	
	Dbg_Print("GetIdentifyData:Begin\n");
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
	Dbg_Print("GetIdentifyData:End\n");
	
DELETE:
	if (*TransferBuffer != NULL)
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
	IDENTIFY_CONTROLLER_DATA	*Identify_controller_data_TransferBuffer = NULL;
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
	GetIdentifyData(gPassthru, &Identify_controller_data_TransferBuffer, 1, 0);
	
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

