/** @file

**/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <../PciSioSerialDxe/Serial.h>
#include "dbg.h"

EFI_SERIAL_IO_PROTOCOL *gSerial = NULL;


static EFI_HANDLE *gHandles = NULL;
static INTN gHandle_num;


/**
  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
Dbg_Init (
  IN EFI_HANDLE        ImageHandle
  )
{
  EFI_STATUS             Status;
  UINTN                  Index;

  if(gSerial != NULL)
    return EFI_SUCCESS;

  Print (L"Serial Test Application\n");
  Print (L"Copyright 2012 (c) Ashley DeSimone\n");
  Print (L"Portions Copyright (c) 2009 - 2013, Intel Corporation. All rights reserved.\n");
  Print (L"\n");

  //
  // Figure out how big of an array is needed.
  //
  Index      = 0;
  gHandle_num = 0;
  gHandles     = NULL;

  gSerial = NULL;

  Status = gBS->LocateHandle (
                  ByProtocol,
                  &gEfiSerialIoProtocolGuid,
                  NULL,
                  &gHandle_num,
                  gHandles
                  );

  //
  // Check to make sure there is at least 1 handle in the system that implements
  // our protocol
  //
  if(Status != EFI_BUFFER_TOO_SMALL) {
    Print(L"There are no serial ports attached to the system.\n");
    return EFI_SUCCESS;
  }

  Print (L"Found %d serial ports\n", (gHandle_num / sizeof (EFI_HANDLE)));

  gHandles = AllocateZeroPool (sizeof (EFI_HANDLE) * gHandle_num);
  if (gHandles == NULL) {
    Print (L"Out of memory\n");
    return EFI_SUCCESS;
  }

  Status = gBS->LocateHandle (
                  ByProtocol,
                  &gEfiSerialIoProtocolGuid,
                  NULL,
                  &gHandle_num,
                  gHandles
                  );

  Print (L"Select Serial Port to Open 0x%x:\n", gHandles[0]);

  //
  // Get the users response and if valid open the selected port.
  //
  Index = 0;
  Status = gBS->OpenProtocol (
                  gHandles[Index],
                  &gEfiSerialIoProtocolGuid,
                  (void**)&gSerial,
                  ImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    Print (L"Unexpected error accessing protocol\n");
    return EFI_SUCCESS;
  }


  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Dbg_UnInit (
  IN EFI_HANDLE        ImageHandle
  )
{
  INTN  i;

  if(gHandles != NULL)
  {
    for(i = 0; i < gHandle_num; i++)
    {
      gBS->CloseProtocol ( 
             gHandles[i],
             &gEfiSerialIoProtocolGuid,
             ImageHandle,
             NULL
             );
    }
     
    FreePool (gHandles);
    gHandles = NULL;
  }
  
  return EFI_SUCCESS;
}



void Dbg_Print(CHAR8  *Format, ... )
{
  VA_LIST Marker;
  UINTN   Return;
  CHAR8      *Buffer;
  UINTN       BufferSize;

  if(gSerial==NULL)
    return;

  VA_START (Marker, Format);


  BufferSize = (512) * sizeof (CHAR8);

  Buffer = (CHAR8 *) AllocatePool(BufferSize);

  if(Buffer == NULL)
    return;

  //Return = UnicodeVSPrint (Buffer, BufferSize, Format, Marker);
  Return = AsciiVSPrint(Buffer, BufferSize, Format, Marker);
  //BufferSize = strlen(Buffer);

  if (Return > 0) {
    //
    // To be extra safe make sure Console has been initialized
    //
    gSerial->Write (gSerial, &Return, Buffer);
    Print(L"%s", Buffer);
  }

  FreePool (Buffer);

  VA_END (Marker);

  return;
}

void Dbg_Rd(
  IN CONST CHAR16  *Format,
  ...
  )
{

  return;
}

