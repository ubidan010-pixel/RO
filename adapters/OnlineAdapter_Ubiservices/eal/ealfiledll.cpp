////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealfiledll.cpp  DLL wrapper implementation for the EAL (file).
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "ealfile.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL Side

static eal_file_dll_interface        gEalFileDllInterface = {};

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalFileDllInitInterface(const eal_file_dll_interface& Interface)
{
    gEalFileDllInterface = Interface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalFileLibInit(eal_u32 Version)
{
    return (*gEalFileDllInterface.pEalFileLibInit)(Version);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalFileLibShutdown()
{
    (*gEalFileDllInterface.pEalFileLibShutdown)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file* EalFileOpen(eal_id Tag, const char* pFileName, eal_file_flags OpenFlags, eal_file_status& Status)
{
    return (*gEalFileDllInterface.pEalFileOpen)(Tag, pFileName, OpenFlags, Status);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalFileClose(eal_file* pFile)
{
    (*gEalFileDllInterface.pEalFileClose)(pFile);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalFileExists(eal_id Tag, const char* pPath)
{
    return (*gEalFileDllInterface.pEalFileExists)(Tag, pPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileRead(eal_file*           pFile,
                            void*               pBuffer,
                            const eal_u64&      Position,
                            eal_u32             BufferSize,
                            eal_u32&            SizeRead,
                            eal_file_priority   Priority)
{
    return (*gEalFileDllInterface.pEalFileRead)(pFile, pBuffer, Position, BufferSize, SizeRead, Priority);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileAsyncRead(eal_file*          pFile,
                                 void*              pBuffer,
                                 eal_u32            Size,
                                 const eal_u64&     Pos,
                                 eal_async_cb       Callback,
                                 void*              pCookie,
                                 eal_file_priority  Priority)
{
    return (*gEalFileDllInterface.pEalFileAsyncRead)(pFile, pBuffer, Size, Pos, Callback, pCookie, Priority);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileWrite(eal_file*      pFile,
                             const void*    pBuffer,
                             const eal_u64& Position,
                             eal_u32        BufferSize,
                             eal_u32&       SizeWritten)
{
    return (*gEalFileDllInterface.pEalFileWrite)(pFile, pBuffer, Position, BufferSize, SizeWritten);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u64 EalFileGetSize(eal_file* pFile)
{
    return (*gEalFileDllInterface.pEalFileGetSize)(pFile);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileFlush(eal_file* pFile)
{
    return (*gEalFileDllInterface.pEalFileFlush)(pFile);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalFileGetBufferAlignment(eal_file* pFile)
{
    return (*gEalFileDllInterface.pEalFileGetBufferAlignment)(pFile);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalFileGetSectorSize(eal_file* pFile)
{
    return (*gEalFileDllInterface.pEalFileGetSectorSize)(pFile);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

char EalFileGetSeparator()
{
    return (*gEalFileDllInterface.pEalFileGetSeparator)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// New since version 3.0

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileExistsWithStatus(eal_id      Tag,
                                        const char* pPath)
{
    return (*gEalFileDllInterface.pEalFileExistsWithStatus)(Tag, pPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileAsyncWrite(eal_file*         pFile,
                                  const void*       pBuffer,
                                  eal_u32           Size,
                                  const eal_u64&    Pos,
                                  eal_async_cb      Callback,
                                  void*             pCookie,
                                  eal_file_priority Priority)
{
    return (*gEalFileDllInterface.pEalFileAsyncWrite)(pFile, pBuffer, Size, Pos, Callback, pCookie, Priority);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileGetSizeWithStatus(eal_file* pFile,
                                         eal_u64&  FileSize)
{
    return (*gEalFileDllInterface.pEalFileGetSizeWithStatus)(pFile, FileSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// New since version 4.0

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileDelete(eal_id Tag,
                              const char* pPath)
{
    return (*gEalFileDllInterface.pEalFileDelete)(Tag, pPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileCreateDirectory(eal_id Tag,
                                       const char* pPath)
{
    return (*gEalFileDllInterface.pEalFileCreateDirectory)(Tag, pPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileRemoveDirectory(eal_id Tag,
                                       const char* pPath)
{
    return (*gEalFileDllInterface.pEalFileRemoveDirectory)(Tag, pPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// New since version 5.0

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_file_status EalFileVisitDirectoryEntries(eal_id Tag,
                                             const char* pPath,
                                             eal_u32 OptionFlags,
                                             eal_u32 HintFlags,
                                             eal_directory_entry_cb Callback,
                                             void* pCookie)
{
    return (*gEalFileDllInterface.pEalFileVisitDirectoryEntries)(Tag, pPath, OptionFlags, HintFlags, Callback, pCookie);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#else

////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine Side

void EalFileDllPopulateInterface(eal_file_dll_interface& Interface)
{
    Interface.pEalFileLibInit               = EalFileLibInit;
    Interface.pEalFileLibShutdown           = EalFileLibShutdown;
    Interface.pEalFileOpen                  = EalFileOpen;
    Interface.pEalFileClose                 = EalFileClose;
    Interface.pEalFileRead                  = EalFileRead;
    Interface.pEalFileAsyncRead             = EalFileAsyncRead;
    Interface.pEalFileWrite                 = EalFileWrite;
    Interface.pEalFileFlush                 = EalFileFlush;
    Interface.pEalFileGetBufferAlignment    = EalFileGetBufferAlignment;
    Interface.pEalFileGetSectorSize         = EalFileGetSectorSize;
    Interface.pEalFileGetSeparator          = EalFileGetSeparator;

    // BEGIN DEPRECATED in V501
#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable:4995 4996)
#elif defined(_GCC)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    Interface.pEalFileExists                = EalFileExists;
    Interface.pEalFileGetSize               = EalFileGetSize;
#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(_GCC)
#   pragma GCC diagnostic pop
#endif
    // END DEPRECATED

    // New since version 3.0
    Interface.pEalFileExistsWithStatus      = EalFileExistsWithStatus;
    Interface.pEalFileAsyncWrite            = EalFileAsyncWrite;
    Interface.pEalFileGetSizeWithStatus     = EalFileGetSizeWithStatus;

    // New since version 4.0
    Interface.pEalFileDelete                = EalFileDelete;
    Interface.pEalFileCreateDirectory       = EalFileCreateDirectory;
    Interface.pEalFileRemoveDirectory       = EalFileRemoveDirectory;

    // New since version 5.0
    Interface.pEalFileVisitDirectoryEntries = EalFileVisitDirectoryEntries;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
