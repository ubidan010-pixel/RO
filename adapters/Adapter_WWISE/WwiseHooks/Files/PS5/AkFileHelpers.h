/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/
//////////////////////////////////////////////////////////////////////
//
// AkFileHelpers.h
//
// Platform-specific helpers for files.
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_HELPERS_H_
#define _AK_FILE_HELPERS_H_

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>

#include "../Common/AkFileHelpersBase.h"

#include <string.h>

#define __AKFILEHELPERS_ASSERT( Condition ) AKASSERT( Condition );	

class CAkFileHelpers : public CAkFileHelpersBase
{
public:

	// wrapper for sceKernelOpen
	static AKRESULT OpenFile(
		const AkOSChar* in_pszFilename,     // File name.
		AkOpenMode      in_eOpenMode,       // Open mode.
		AkFileHandle &  out_hFile           // Returned file identifier/handle.
	)
	{
		// Check parameters.
		__AKFILEHELPERS_ASSERT(in_pszFilename != NULL);
		__AKFILEHELPERS_ASSERT(strlen(in_pszFilename) < AK_MAX_PATH);

		// Open mode
		int flags = 0;
		SceKernelMode mode = 0;
		switch (in_eOpenMode)
		{
		case AK_OpenModeRead:
			flags = SCE_KERNEL_O_RDONLY;
			break;
		case AK_OpenModeWrite:
			flags = SCE_KERNEL_O_WRONLY | SCE_KERNEL_O_CREAT;
			mode = SCE_KERNEL_S_IRWU;
			break;
		case AK_OpenModeWriteOvrwr:
			flags = SCE_KERNEL_O_WRONLY | SCE_KERNEL_O_CREAT | SCE_KERNEL_O_TRUNC;
			mode = SCE_KERNEL_S_IRWU;
			break;
		case AK_OpenModeReadWrite:
			flags = SCE_KERNEL_O_RDWR;
			break;
		default:
			__AKFILEHELPERS_ASSERT(!"Invalid open mode");
			out_hFile = -1;
			return AK_InvalidParameter;
		}
		int result = sceKernelOpen(in_pszFilename, flags, mode);
		if (result < 0)
		{
			out_hFile = -1;
			if (result == SCE_KERNEL_ERROR_ENOENT)
			{
				return AK_FileNotFound;
			}
			else if (result == SCE_KERNEL_ERROR_EACCES)
			{
				return AK_FilePermissionError;
			}
			else
			{
				return AK_UnknownFileError;
			}
		}
		
		out_hFile = result;
		return AK_Success;
	}

	static AKRESULT Open(
		const AkOSChar* in_pszFileName,     // File name.
		AkOpenMode      in_eOpenMode,       // Open mode.
		bool			in_bOverlapped,		// Overlapped IO
		AkFileDesc &    out_fileDesc		// File descriptor
		)
	{
		AKRESULT eResult = OpenFile(
			in_pszFileName,
			in_eOpenMode,
			out_fileDesc.hFile);

		if (eResult == AK_Success)
		{
			// Fill AkFileDescriptor structure.
			SceKernelStat stat;
			int res = sceKernelFstat(out_fileDesc.hFile, &stat);
			if (res == SCE_OK)
			{
				out_fileDesc.iFileSize = stat.st_size;
				out_fileDesc.uSector = 0;
			}
			else
			{
				AKASSERT(!"CAkDefaultIOHookBlocking::Open() - sceKernelFstat() failed!");
				out_fileDesc.iFileSize = 0;
				out_fileDesc.uSector = 0;
				eResult = AK_UnknownFileError;
			}
		}

		return eResult;
	}

	// Wrapper for system file handle closing.
	static AKRESULT CloseFile( const AkFileDesc& in_FileDesc )
	{
		if ( sceKernelClose( in_FileDesc.hFile ) != SCE_OK )
		{
			__AKFILEHELPERS_ASSERT( !"Failed to close file handle" );
			return AK_Fail;
		}
		return AK_Success;
	}

	//
	// Simple platform-independent API to open and read files using AkFileHandles, 
	// with blocking calls and minimal constraints.
	// ---------------------------------------------------------------------------

	// Open file to use with ReadBlocking().
	static AKRESULT OpenBlocking(
        const AkOSChar* in_pszFilename,     // File name.
        AkFileHandle &  out_hFile           // Returned file handle.
		)
	{
		return OpenFile( 
			in_pszFilename,
			AK_OpenModeRead,
			out_hFile );
	}

	// Required block size for reads (used by ReadBlocking() below).
	static const AkUInt32 s_uRequiredBlockSize = 1;

	// Simple blocking read method.
	static AKRESULT ReadBlocking(
		AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pBuffer,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.
		AkInt32			in_uPosition,		// Position from which to start reading.
		AkUInt32		in_uSizeToRead,		// Size to read. Must be a multiple of CAkFileHelpers::s_uRequiredBlockSize.
		AkUInt32 &		out_uSizeRead		// Returned size read.        
	)
	{
		__AKFILEHELPERS_ASSERT(in_uSizeToRead % s_uRequiredBlockSize == 0
			&& in_uPosition % s_uRequiredBlockSize == 0);

		ssize_t uSizeRead = sceKernelPread(in_hFile, in_pBuffer, in_uSizeToRead, in_uPosition);
		if (uSizeRead >= 0)
		{
			out_uSizeRead = (AkUInt32)uSizeRead;
			return AK_Success;
		}
		return AK_Fail;
	}

	static AKRESULT ReadBlocking(
		AkFileDesc& in_fileDesc,		// Returned file identifier/handle.
		void*		in_pBuffer,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.
		AkUInt32	in_uPosition,		// Position from which to start reading.
		AkUInt32	in_uSizeToRead,		// Size to read. Must be a multiple of CAkFileHelpers::s_uRequiredBlockSize.
		AkUInt32&	out_uSizeRead		// Returned size read.        
		)
	{
		return ReadBlocking(in_fileDesc.hFile, in_pBuffer, in_uPosition, in_uSizeToRead, out_uSizeRead);
	}

	/// Returns AK_Success if the directory is valid, AK_Fail if not.
	/// For validation purposes only.
	/// Some platforms may return AK_NotImplemented, in this case you cannot rely on it.
	static AKRESULT CheckDirectoryExists( const AkOSChar* in_pszBasePath )
	{
		SceKernelStat stat;
		if (sceKernelStat(in_pszBasePath, &stat) != SCE_OK)
			return AK_Fail;

		return S_ISDIR(stat.st_mode) ? AK_Success : AK_Fail;
	}

	static AKRESULT CreateEmptyDirectory(const AkOSChar* in_pszDirectoryPath)
	{
		int errNo = sceKernelMkdir(in_pszDirectoryPath, SCE_KERNEL_S_IRWU);
		if (errNo != SCE_OK && errNo != SCE_KERNEL_ERROR_EEXIST)
			return AK_Fail;

		return AK_Success;
	}

	static AKRESULT RemoveEmptyDirectory(const AkOSChar* in_pszDirectoryPath)
	{
		int errNo = sceKernelRmdir(in_pszDirectoryPath);
		if (errNo != SCE_OK)
			return AK_Fail;

		return AK_Success;
	}

	static AKRESULT WriteBlocking(
		AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pData,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.		
		AkUInt64		in_uPosition,		// Position from which to start writing.
		AkUInt32		in_uSizeToWrite)
	{
		AKASSERT(in_pData);

		// Write to file
		ssize_t uSizeWritten = sceKernelPwrite(in_hFile, in_pData, in_uSizeToWrite, in_uPosition);
		if (uSizeWritten > 0)
			return AK_Success;
		else
			return AK_Fail;
	}

	static AKRESULT GetDefaultWritablePath(AkOSChar* out_pszPath, AkUInt32 in_pathMaxSize)
	{
		if (out_pszPath == nullptr)
			return AK_InsufficientMemory;

		out_pszPath[0] = '\0';
		AKPLATFORM::SafeStrCpy(out_pszPath, AKTEXT("/host/"), in_pathMaxSize);
		return AK_Success;
	}
	
	static void NormalizeDirectorySeparators(AkOSChar* io_path) { } // no-op
};

#endif //_AK_FILE_HELPERS_H_
