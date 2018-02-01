#ifndef _DBG_H_
#define _DBG_H_


EFI_STATUS EFIAPI Dbg_Init (IN EFI_HANDLE ImageHandle);

EFI_STATUS EFIAPI Dbg_UnInit (IN EFI_HANDLE ImageHandle);

void Dbg_Print(CHAR8  *Format, ... );

#endif

