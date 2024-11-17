#include "efi.h"
#include "efilib.h"
#include "efiapi.h"


void* memset(void* dest, int value, size_t size) {
    unsigned char* ptr = dest;
    while (size--) {
        *ptr++ = (unsigned char)value;
    }
    return dest;
}

typedef struct {
    VOID* framebuffer_base;
    UINT32 framebuffer_width;
    UINT32 framebuffer_height;
    UINT32 framebuffer_pitch;
    UINT32 framebuffer_bpp; // Bits per pixel
} FramebufferInfo;

FramebufferInfo gop_init(EFI_SYSTEM_TABLE* SystemTable) {
    FramebufferInfo fb_info = {0};
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS Status;

    Status = SystemTable->BootServices->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (VOID**)&gop
    );
    if (EFI_ERROR(Status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,L"Failed to locate GOP.\n");
        return fb_info; // Halt the system if GOP is unavailable
    }

    fb_info.framebuffer_base = (VOID*)gop->Mode->FrameBufferBase;
    fb_info.framebuffer_width = gop->Mode->Info->HorizontalResolution;
    fb_info.framebuffer_height = gop->Mode->Info->VerticalResolution;
    fb_info.framebuffer_pitch = gop->Mode->Info->PixelsPerScanLine * 4; // Assuming 32-bpp
    fb_info.framebuffer_bpp = 32;

    return fb_info;
}



VOID* load_kernel(EFI_SYSTEM_TABLE *st,EFI_FILE_PROTOCOL *root,CHAR16 *path){
    EFI_STATUS status = EFI_SUCCESS;
    VOID* file_buffer = NULL;

    //open file pointer to kernel
    EFI_FILE_PROTOCOL *kernel;
    status = root->Open(root,&kernel,path,EFI_FILE_MODE_READ,0);
    if(EFI_ERROR(status)){
        st->ConOut->OutputString(st->ConOut,L"Error opening kernel file\r\n");
        return NULL;
    }

    //get kernel file info
    EFI_FILE_INFO *kernel_info = NULL;
    UINTN kernel_size = 0;
    EFI_GUID info_guid = EFI_FILE_INFO_ID;

    // First call to GetInfo to determine the required buffer size
    status = kernel->GetInfo(kernel, &info_guid, &kernel_size, NULL);
    if (status == EFI_BUFFER_TOO_SMALL) {
        status = st->BootServices->AllocatePool(EfiLoaderData,kernel_size,(VOID **)&kernel_info);
        if(EFI_ERROR(status)){
            st->ConOut->OutputString(st->ConOut,L"Error allocating pool for kernel info \r\n");
            return NULL;
        }

        status = kernel->GetInfo(kernel,&info_guid,&kernel_size,kernel_info);
        if(EFI_ERROR(status)){
            st->ConOut->OutputString(st->ConOut,L"Error reading kernel info \r\n");
            return NULL;
        }

    } else if (EFI_ERROR(status)) {
        // Handle other errors
        st->ConOut->OutputString(st->ConOut, L"Error getting kernel file info\r\n");
        return NULL;
    }

    //allocate pool for kernel file
    kernel_size = kernel_info->FileSize;
    status = st->BootServices->AllocatePool(EfiLoaderData,kernel_size,&file_buffer);
    if(EFI_ERROR(status)){
        st->ConOut->OutputString(st->ConOut,L"Error allocating memory for kernel file\r\n");
        return NULL;
    }

    //read the kernel file into file buffer
    status = kernel->Read(kernel,&kernel_size,file_buffer);
    if(EFI_ERROR(status)){
        st->ConOut->OutputString(st->ConOut,L"Error reading kernel file into buffer");
        return NULL;
    }

    

    
    
    return file_buffer;
}


EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    EFI_STATUS status = EFI_SUCCESS;
    EFI_INPUT_KEY Key;

    /* Store the system table for future use in other functions */
    EFI_SYSTEM_TABLE *st = systab;

    // invoke the loaded image protocol to get the current device handle
    EFI_GUID lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *lip;
    status = st->BootServices->OpenProtocol(image,&lip_guid,(VOID **)&lip, image, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if(EFI_ERROR(status)){
        st->ConOut->OutputString(st->ConOut,L"Error loading LIP \r\n");
        return status;
    }

    //invoke the Simple File System Protocol on the retrieved root device handle
    EFI_GUID sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp;
    status = st->BootServices->OpenProtocol(lip->DeviceHandle,&sfsp_guid,(VOID **)&sfsp, image, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if(EFI_ERROR(status)){
        st->ConOut->OutputString(st->ConOut,L"Error initializing Simple File System Protocol\r\n");
        return status;
    }

    //Open root volume using Simple File System Protocol
    EFI_FILE_PROTOCOL* root;
    status = sfsp->OpenVolume(sfsp,&root);
    if(EFI_ERROR(status)){
        st->ConOut->OutputString(st->ConOut,L"Error Opening root volume \r\n");
        return status;
    }

    VOID* kernel_buffer = load_kernel(st,root,L"\\kernel.bin");
    if(kernel_buffer == NULL){
        st->ConOut->OutputString(st->ConOut,L"Error loading kernel\r\n");
        return status;
    }

    // EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    // EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    // status = st->BootServices->LocateProtocol(&gop_guid,NULL,(VOID **)&gop);
    // if(EFI_ERROR(status)){
    //     st->ConOut->OutputString(st->ConOut,L"Error locating GOP \r\n");
    //     return status;
    // }
    // EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    // UINTN SizeOfInfo, numModes, nativeMode;

    // status = gop->QueryMode(gop,gop->Mode==NULL?0:gop->Mode->Mode,&SizeOfInfo,&info);
    // if(EFI_ERROR(status)){
    //     st->ConOut->OutputString(st->ConOut,L"Error querying GOP mode \r\n");
    //     return status;
    // }

    // if (status == EFI_NOT_STARTED)
    //     status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
    // if(EFI_ERROR(status)) {
    //     st->ConOut->OutputString(st->ConOut,L"Unable to get native mode \r\n");
    //     return status;
    // }else {
    //     nativeMode = gop->Mode->Mode;
    //     numModes = gop->Mode->MaxMode;
    // }
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    EFI_GUID GopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    status = st->BootServices->LocateProtocol(&GopGuid, NULL, (void **)&Gop);
    if (EFI_ERROR(status)) {
        st->ConOut->OutputString(st->ConOut, L"Error: Unable to locate Graphics Output Protocol.\r\n");
        return status;
    }

    UINTN bestMode = 0;
    UINT32 maxWidth = 0, maxHeight = 0;

    for (UINTN i = 0; i < Gop->Mode->MaxMode; ++i) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
        UINTN infoSize;
        if (Gop->QueryMode(Gop, i, &infoSize, &info) == EFI_SUCCESS) {
            if (info->HorizontalResolution > maxWidth && info->VerticalResolution > maxHeight) {
                maxWidth = info->HorizontalResolution;
                maxHeight = info->VerticalResolution;
                bestMode = i;
            }
        }
    }
    Gop->SetMode(Gop, bestMode);


  

    typedef struct {
    UINT64 FrameBufferBase;
    UINT32 FrameBufferWidth;
    UINT32 FrameBufferHeight;
    UINT32 FrameBufferPitch;
    } BootInfo;

    BootInfo *bootInfo;
    status = st->BootServices->AllocatePool(EfiLoaderData, sizeof(BootInfo), (void **)&bootInfo);
    if(EFI_ERROR(status)){
        st->ConOut->OutputString(st->ConOut,L"Unable to allocate page for GOP\r\n");
    }

    bootInfo->FrameBufferBase = Gop->Mode->FrameBufferBase;
    bootInfo->FrameBufferWidth = Gop->Mode->Info->HorizontalResolution;
    bootInfo->FrameBufferHeight = Gop->Mode->Info->VerticalResolution;
    bootInfo->FrameBufferPitch = Gop->Mode->Info->PixelsPerScanLine;

    void (*KernelEntry)(BootInfo *) = (void (*)(BootInfo *))kernel_buffer;
    KernelEntry(bootInfo);


    // Jump to the kernel
    // void (*kernel_entry)(FramebufferInfo*) = (void (*)(FramebufferInfo*))kernel_buffer;
    // kernel_entry(&fb_info);

    /* Say hi */
    status = st->ConOut->OutputString(st->ConOut, L"Hello World\r\n"); // EFI Applications use Unicode and CRLF, a la Windows
    if (EFI_ERROR(status))
        return status;

    
    status = st->ConIn->Reset(st->ConIn, FALSE);
    if (EFI_ERROR(status))
        return status;

    while ((status = st->ConIn->ReadKeyStroke(st->ConIn, &Key)) == EFI_NOT_READY) ;

    return status;
}