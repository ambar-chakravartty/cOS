// UEFI From Scratch - ThatOSDev ( 2021 - 2022 )
// https://codeberg.org/ThatOSDev/UEFI-Tuts

#include "efi.h"
#include "efilibs.h"

EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE *ST)
{
    ImageHandle = IH;
    SystemTable = ST;
	
	InitializeSystem();
	
	SetColor(EFI_WHITE);
	
	SetTextPosition(10, 1);
    Print(L"Loading ThatOS64 ...\r\n\r\n");
    
	readFile(u"ThatOS64\\loader.bin");


	UINT8* loader = (UINT8*)OSBuffer_Handle;
	
	// TODO : Setup and grab address of memory MAP
	
    UINTN                  MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR  *MemoryMap;
    UINTN                  MapKey;
    UINTN                  DescriptorSize;
    UINT32                 DescriptorVersion;
    
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    MemoryMapSize += 2 * DescriptorSize;
    SystemTable->BootServices->AllocatePool(2, MemoryMapSize, (void **)&MemoryMap);
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
	
    void (*KernelBinFile)(int, BLOCKINFO*) = ((__attribute__((ms_abi)) void (*)(int, BLOCKINFO*) ) &loader[262]);
	
    KernelBinFile(0, &bi);
    
    // We should not make it to this point.
    return EFI_SUCCESS;
}
