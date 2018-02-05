#ifndef __A_SMARTINFO_H__
#define __A_SMARTINFO_H__

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NvmExpressPassthru.h>
#include "Test_Nvme_A.h"

//Get Smart/Health information log using Passthru protocol
VOID 
GetLogPage(
	IN EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL		*NvmePassThru,
	IN UINT8									LogIdentifier,				//0x02
	IN OUT UINT8								**TransferBuffer
);


#endif