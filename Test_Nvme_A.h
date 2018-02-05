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



EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  *gPassthru;
EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET gNvmeCommandPacket;
EFI_NVM_EXPRESS_COMMAND	gNvmeCmd;
EFI_NVM_EXPRESS_COMPLETION gNvmeCompletion;


VOID
ResetNvmeCommandPacket();

#pragma pack(1)

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

// Figure 83 NVM Express 1.1 Spec
typedef struct {
    UINT16      MP;                 // Bits 15:0
    UINT8       Reserved1;          // Bits 23:16
    UINT8       MPS : 1;            // Bits 24
    UINT8       NOPS : 1;           // Bits 25
    UINT8       Reserved2 : 6;      // Bits 31:26
    UINT32      ENLAT;              // Bits 63:32
    UINT32      EXLAT;              // Bits 95:64
    UINT8       RRT : 5;            // Bits 100:96
    UINT8       Reserved3 : 3;      // Bits 103:101
    UINT8       RRL : 5;            // Bits 108:104
    UINT8       Reserved4 : 3;      // Bits 111:109
    UINT8       RWT : 5;            // Bits 116:112
    UINT8       Reserved5 : 3;      // Bits 119:117
    UINT8       RWL : 5;            // Bits 124:120
    UINT8       Reserved6 : 3;      // Bits 127:125
    UINT16      IDLP;               // Bits 143:128
    UINT8       Reserved7 : 6;      // Bits 149:144
    UINT8       IPS :2;             // Bits 151:150
    UINT8       Reserved8;          // Bits 159:152
    UINT16      ACTP;               // Bits 175:160
    UINT8       APW : 3;            // Bits 178:176
    UINT8       Reserved9 : 3;      // Bits 181:179
    UINT8       APS : 2;            // Bits 183:182
    UINT8       Reserved10[9];      // Bits 255:184
} POWER_STATE_DESCRIPTOR;

// Figure 90 NVM Express 1.2 spec
typedef struct {
    UINT16                  VID;                    // Offset 01:00   
    UINT16                  SSVID;                  // Offset 03:02
    UINT8                   SerialNumber[20];       // Offset 23:04
    UINT8                   ModelNumber[40];        // Offset 63:24
    UINT8                   FirmwareRevision[8];    // Offset 71:64
    UINT8                   ArbitrationBurst;       // Offset 72
    UINT8                   IEEEOUIIdentifier[3];   // Offset 75:73
    UINT8                   CMIC;                   // Offset 76
    UINT8                   MDTS;                   // Offset 77
    UINT16                  ControllerID;           // Offset 79:78
    UINT32                  VER;                    // Offset 83:80
    UINT32                  RTD3R;                  // Offset 87:84
    UINT32                  RTD3E;                  // Offset 91:88
    UINT32                  OAES;                   // Offset 95:92
    UINT8                   Reserved1[160];         // Offset 255:96
    UINT16                  OACS;                   // Offset 257:256
    UINT8                   ACL;                    // Offset 258
    UINT8                   AERL;                   // Offset 259
    UINT8                   FRMW;                   // Offset 260
    UINT8                   LPA;                    // Offset 261
    UINT8                   ELPE;                   // Offset 262
    UINT8                   NPSS;                   // Offset 263
    UINT8                   AVSCC;                  // Offset 264
    UINT8                   APSTA;                  // Offset 265
    UINT16                  WCTEMP;                 // Offset 267:266
    UINT16                  CCTEMP;                 // Offset 269:268
    UINT16                  MTFA;                   // Offset 271:270
    UINT32                  HMPRE;                  // Offset 275:272
    UINT32                  HMMIN;                  // Offset 279:276
    UINT8                   TNVMCAP[16];            // Offset 295:280
    UINT8                   UNVMCAP[16];            // Offset 311:296
    UINT32                  RPMBS;                  // Offset 315:312
    UINT8                   Reserved2[196];         // Offset 511:316
    UINT8                   SQES;                   // Offset 512
    UINT8                   CQES;                   // Offset 513
    UINT16                  Reserved3;              // Offset 515:514
    UINT32                  NN;                     // Offset 519:516
    UINT16                  ONCS;                   // Offset 521:520
    UINT16                  FUSES;                  // Offset 523:522
    UINT8                   FNA;                    // Offset 524
    UINT8                   VWC;                    // Offset 525
    UINT16                  AWUN;                   // Offset 527:526
    UINT16                  AWUPF;                  // Offset 529:528
    UINT8                   NVSCC;                  // Offset 530
    UINT8                   Reserved4;              // Offset 531
    UINT16                  ACWU;                   // Offset 533:532
    UINT16                  Reserved5;              // Offset 535:536
    UINT32                  SGLS;                   // Offset 539:536
    UINT8                   Reserved6[164];         // Offset 703:540
    UINT8                   Reserved7[1344];        // Offset 2047:704
    POWER_STATE_DESCRIPTOR  psdd[32];               // Offset 3071:2048
    UINT8                   VS[1024];               // Offset 4095:3072
} IDENTIFY_CONTROLLER_DATA;

#pragma pack()


#endif