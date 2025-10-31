#include "precompiled_SystemAdapter_x360.h"

#ifndef _ITF_SYSTEMADAPTER_X360_H_
#include "adapters/SystemAdapter_x360/SystemAdapter_x360.h"
#endif //_ITF_SYSTEMADAPTER_X360_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef ITF_FINAL
    #include <xbdm.h>
#endif

namespace ITF
{

#define WRITE_AS_BINARY

#ifndef ITF_FINAL
    const CHAR* SystemAdapter_X360::g_filename = "d:\\callstacks.dat";
    BOOL SystemAdapter_X360::s_initialized = FALSE;
    BOOL SystemAdapter_X360::s_initTried = FALSE;
    HANDLE SystemAdapter_X360::hFile = INVALID_HANDLE_VALUE;
    DWORD SystemAdapter_X360::writeCount = 0;

    template <typename T> void ByteReverse( T& data )
    {
        BYTE* pData = ( BYTE* )&data;
        for( int i = 0; i < sizeof( data ) / 2; ++i )
        {
            std::swap( pData[i], pData[sizeof( data ) - 1 - i] );
        }
    }

    void SystemAdapter_X360::createDumpFile()
    {
    }
    
    void SystemAdapter_X360::WriteAddresses( ULONG numEntries, VOID** pAddresses, const void* _userData, size_t _userDataSize)
    {
    static BOOL s_initialized = FALSE;
    static BOOL s_initTried = FALSE;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD writeCount = 0;
    if( !s_initialized )
    {
        // If we've already tried to initialize and failed, just exit.
        if( s_initTried )
            return;
        s_initTried = TRUE;

        // Allow writing to e:
        DmMapDevkitDrive();
        // Create the file to store the callstack data.
        hFile = CreateFile( g_filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );

        if( hFile == INVALID_HANDLE_VALUE )
            return; // Failure

#ifdef  WRITE_AS_BINARY
        // Write a marker to identify this as a binary file.
        const char* moduleComment = "BINR";
        WriteFile( hFile, moduleComment, strlen(moduleComment), &writeCount, 0 );
#else
        // Write a comment for the reader.
        const char* moduleComment = "; List of loaded modules\r\n";
        WriteFile( hFile, moduleComment, strlen( moduleComment ), &writeCount, 0 );
#endif

        // Walk the list of loaded code modules.
        HRESULT error;
        PDM_WALK_MODULES pWalkMod = NULL;
        DMN_MODLOAD modLoad;

        while( XBDM_NOERR == ( error = DmWalkLoadedModules( &pWalkMod, &modLoad ) ) )
        {
            // Find the signature that describes the PDB file of the current module.
            DM_PDB_SIGNATURE signature = {0};
            DmFindPdbSignature( modLoad.BaseAddress, &signature );

#ifdef  WRITE_AS_BINARY
            // Each module is prefixed by a magic number, and the list of modules is
            // terminated with a DWORD zero.
            DWORD modulePrefixID = 0xABABCDCD;
            WriteFile( hFile, &modulePrefixID, sizeof(modulePrefixID), &writeCount, 0 );

            // Write the data that describes this module.
            WriteFile( hFile, &modLoad.BaseAddress, sizeof(modLoad.BaseAddress), &writeCount, 0 );
            WriteFile( hFile, &modLoad.Size, sizeof(modLoad.Size), &writeCount, 0 );
            WriteFile( hFile, &modLoad.TimeStamp, sizeof(modLoad.TimeStamp), &writeCount, 0 );
            WriteFile( hFile, &modLoad.Name, sizeof(modLoad.Name), &writeCount, 0 );
            WriteFile( hFile, &signature, sizeof(signature), &writeCount, 0 );
#else
            // Write the information about the current module as three lines of text.
            // The first line is the module file name, the second line is the full path to
            // the PDB file, and the third line is the load address, size, timestamp, GUID,
            // and PDB age.
            char buffer[1000];

            // Write the module name on its own line.
            sprintf_s( buffer, "%s\r\n", modLoad.Name );
            WriteFile( hFile, buffer, strlen( buffer ), &writeCount, 0 );

            // Write the pdb path on its own line.
            sprintf_s( buffer, "%s\r\n", signature.Path );
            WriteFile( hFile, buffer, strlen( buffer ), &writeCount, 0 );

            // Write the module address, size, and timestamp.
            sprintf_s( buffer, "%p, %08X, %08X, ", modLoad.BaseAddress, modLoad.Size,
                       modLoad.TimeStamp );
            WriteFile( hFile, buffer, strlen( buffer ), &writeCount, 0 );

            // ByteReverse the necessary elements of the GUID so it prints properly.
            ByteReverse( signature.Guid.Data1 );
            ByteReverse( signature.Guid.Data2 );
            ByteReverse( signature.Guid.Data3 );
            ByteReverse( signature.Age );
            // Print out the GUID and age in the standard format (as used by dumpbin /headers)
            // except with spaces between the bytes of Guid.Data4, to make for easier parsing.
            sprintf_s( buffer, "{%08X-%04X-%04X-%02X %02X-%02X %02X %02X %02X %02X %02X}, %d\r\n",
                       signature.Guid.Data1, signature.Guid.Data2, signature.Guid.Data3,
                       signature.Guid.Data4[0], signature.Guid.Data4[1],
                       signature.Guid.Data4[2], signature.Guid.Data4[3],
                       signature.Guid.Data4[4], signature.Guid.Data4[5],
                       signature.Guid.Data4[6], signature.Guid.Data4[7],
                       signature.Age );
            WriteFile( hFile, buffer, strlen( buffer ), &writeCount, 0 );
#endif
        }

        if( error != XBDM_ENDOFLIST )
        {
            // Handle errors here...
        }
        DmCloseLoadedModules( pWalkMod );

#ifdef  WRITE_AS_BINARY
        // Mark the end of the module list.
        DWORD endId = 0;
        WriteFile( hFile, &endId, sizeof(endId), &writeCount, 0 );
#else
        const char* moduleEndMarker = "ModuleEnd\r\n\r\n; CallStacks:\r\n";
        WriteFile( hFile, moduleEndMarker, strlen( moduleEndMarker ), &writeCount, 0 );
#endif

        // We have successfully initialized our stack tracing system.
        s_initialized = true;
    }
    else
    {
        // We have already initialized the tracing system.
        // Open the existing file and append to it.
        hFile = CreateFile( g_filename, GENERIC_WRITE, 0, NULL,
                            OPEN_EXISTING, 0, NULL );
        if( hFile == INVALID_HANDLE_VALUE )
            return; // Failure

        // Move to the end of the file for adding more data.
        SetFilePointer( hFile, 0, 0, FILE_END );
    }

#ifdef  WRITE_AS_BINARY
    // Write the count of how many addresses there are.
    WriteFile( hFile, &numEntries, sizeof(numEntries), &writeCount, 0 );
    // Write the array of addresses.
    WriteFile( hFile, pAddresses, sizeof(pAddresses[0]) * numEntries, &writeCount, 0 );
#else
    WriteFile( hFile, _message, strlen( _message ), &writeCount, 0 );
    char buffer[100];
    sprintf_s( buffer, "%u entries:\r\n", numEntries );
    WriteFile( hFile, buffer, strlen( buffer ), &writeCount, 0 );
    for( ULONG i = 0; i < numEntries; ++i )
    {
        sprintf_s( buffer, "\t%p\r\n", pAddresses[ i ] );
        WriteFile( hFile, buffer, strlen( buffer ), &writeCount, 0 );
    }
#endif
    CloseHandle( hFile );
    }

    void SystemAdapter_X360::WriteStackBackTrace(const void* _userData, size_t _userDataSize)
    {
        const DWORD maxBackTrace = 20;
        VOID* backTrace[maxBackTrace];

        // Capture a stack back trace.
        HRESULT hResult = DmCaptureStackBackTrace( _countof( backTrace ), backTrace );

        if( hResult == XBDM_NOERR )
        {
            // Find out how many valid entries are in the result.
            ULONG count = 0;
            for( ULONG i = 0; i < _countof( backTrace ) && backTrace[i]; ++i )
                count = i;

            // Write the valid entries to the output file.
            WriteAddresses( count, backTrace, _userData, _userDataSize );
        }
    }
#endif // ITF_FINAL

    void SystemAdapter_X360::initMemory(   )
    {
    }



} // namespace ITF



