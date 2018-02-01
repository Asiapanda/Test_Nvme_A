#ifndef __TEST_NVME_A__H_
#define __TEST_NVME_A__H_

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NvmExpressPassthru.h>

typedef struct {
    UINT64      ActiveFirmwareInfo;
    UINT8       FirmwareRevSlot1[8];
    UINT8       FirmwareRevSlot2[8];
    UINT8       FirmwareRevSlot3[8];
    UINT8       FirmwareRevSlot4[8];
    UINT8       FirmwareRevSlot5[8];
    UINT8       FirmwareRevSlot6[8];
    UINT8       FirmwareRevSlot7[8];
    UINT8       Reserved[448];   
}NVME_FIRMWARE_SLOT_INFO;

#endif