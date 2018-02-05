#ifndef __A_FIRMWAREINFO_H__
#define __A_FIRMWAREINFO_H__

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NvmExpressPassthru.h>
#include "Test_Nvme_A.h"

//Gets the Firmware Slot information    
VOID
GetFirmwareSlotInfo(
	IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL		*NvmePassThru,
	IN OUT NVME_FIRMWARE_SLOT_INFO				**TransferBuffer
);

#endif
