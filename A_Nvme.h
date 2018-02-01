#ifndef __A_NVME_H__
#define __A_NVME_H__

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NvmExpressPassthru.h>

EFI_STATUS EFIAPI Nvme_Init(IN EFI_HANDLE ImageHandle, EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  **gPassthru);
EFI_STATUS EFIAPI Nvme_UnInit(IN EFI_HANDLE ImageHandle);



#endif
