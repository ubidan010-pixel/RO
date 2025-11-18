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
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Tools/Common/AkAssert.h>

#include "../Common/AkFileHelpersBase.h"

#include <nn/fs.h>

class CAkFileHelpers : public CAkFileHelpersBase
{
public:
	static const AkUInt32 s_uRequiredBlockSize = 1;

	// C:/OunceHost will be mounted to mnt: on Init()
	static constexpr AkOSChar kDefaultMountName[] = AKTEXT("mnt");
	static constexpr AkOSChar kDefaultHostDrive[] = AKTEXT("C:");
	static constexpr AkOSChar kDefaultHostFolder[] = AKTEXT("OunceHost");
	static constexpr AkOSChar kDefaultWritablePath[] = AKTEXT("mnt:");

	static inline AKRESULT Init()
	{
		// Mount host root first
		nn::Result result = nn::fs::MountHostRoot();
		if (result.IsFailure())
			return AK_Fail;

		// Mount file system on the local PC
		AkOSChar mountPath[AK_MAX_PATH] = {};
		JoinPath(mountPath, AK_MAX_PATH, kDefaultHostDrive, kDefaultHostFolder);
		result = nn::fs::MountHost(kDefaultMountName, mountPath);
		return result.IsSuccess() ? AK_Success : AK_Fail;
	}

	static inline void Term()
	{
		nn::fs::UnmountHostRoot();
		nn::fs::Unmount(kDefaultMountName);
	}

	static inline AKRESULT OpenFile(
        const AkOSChar* in_pszFilename, // File name.
        AkOpenMode		in_eOpenMode,	// Open mode.
		AkFileDesc &	out_fileDesc    // Returned file identifier/handle.
		)
	{
		AKASSERT(in_pszFilename != NULL);
		AKASSERT(strlen(in_pszFilename) < AK_MAX_PATH);

		if (in_eOpenMode == AK_OpenModeRead)
		{
			nn::Result result = nn::fs::OpenFile(&out_fileDesc.hFile, in_pszFilename, nn::fs::OpenMode_Read);
			if (!result.IsSuccess())
			{
				if (nn::fs::ResultPathNotFound::Includes(result))
					return AK_FileNotFound;

				return AK_UnknownFileError;
			}

			result = nn::fs::GetFileSize(&out_fileDesc.iFileSize, out_fileDesc.hFile);
			if (!result.IsSuccess())
			{
				out_fileDesc.iFileSize = 0;
			}
		}
		else if ((in_eOpenMode == AK_OpenModeWriteOvrwr) || (in_eOpenMode == AK_OpenModeWrite))
		{
			nn::Result result = nn::fs::CreateFile(in_pszFilename, 0);
			if (nn::fs::ResultPathNotFound::Includes(result))
			{
				return AK_PathNotFound;
			}
			else if (nn::fs::ResultPathAlreadyExists::Includes(result)
				|| nn::fs::ResultTargetLocked::Includes(result))
			{
				// The target file already exists.
				// This error handling is not necessary when there is no possibility of the file existing.
			}
			else if (nn::fs::ResultUsableSpaceNotEnough::Includes(result))
			{
				// Insufficient data save region for the save data.
				return AK_UnknownFileError;
			}
			result = nn::fs::OpenFile(&out_fileDesc.hFile, in_pszFilename, nn::fs::OpenMode_Write | nn::fs::OpenMode_AllowAppend);
			if (nn::fs::ResultPathNotFound().Includes(result))
				return AK_FileNotFound;
			if (!result.IsSuccess())
				return AK_UnknownFileError;
			out_fileDesc.iFileSize = 0;
		}
		else
		{
			AKASSERT( 0 && "Unexpected file open mode" );
			return AK_InvalidParameter;
		}

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
			out_fileDesc);

		return eResult;
	}

	static inline AKRESULT CloseFile( AkFileHandle in_hFile )
	{
		nn::Result result = nn::fs::FlushFile(in_hFile);
		(void)result;

		nn::fs::CloseFile(in_hFile);
		in_hFile.handle = nullptr;

		return AK_Success;
	}

	// Wrapper for system file handle closing.
	static inline AKRESULT CloseFile( AkFileDesc &	in_fileDesc )
	{
		return CloseFile( in_fileDesc.hFile );
	}

	//
	// Simple platform-independent API to open and read files using AkFileHandles, 
	// with blocking calls and minimal constraints.
	// ---------------------------------------------------------------------------

	// Open file to use with ReadBlocking().
	static inline AKRESULT OpenBlocking(
        const AkOSChar* in_pszFilename,     // File name.
        AkFileHandle &  out_hFile           // Returned file handle.
		)
	{
		AkFileDesc desc;

		AKRESULT eResult = OpenFile( 
			in_pszFilename,     // File name.
			AK_OpenModeRead,	// Open mode.
			desc );	// Returned file entrynum.
		out_hFile = desc.hFile;

		return eResult;
	}

	// Simple blocking read method.
	static inline AKRESULT ReadBlocking(
        AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pBuffer,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.
		AkUInt32		in_uPosition,		// Position from which to start reading.
		AkUInt32		in_uSizeToRead,		// Size to read. Must be a multiple of CAkFileHelpers::s_uRequiredBlockSize.
		AkUInt32 &		out_uSizeRead		// Returned size read.        
		)
	{
		nn::Result result = nn::fs::ReadFile(in_hFile, in_uPosition, in_pBuffer, in_uSizeToRead);
		if (result.IsSuccess())
		{
			out_uSizeRead = in_uSizeToRead;
			return AK_Success;
		}
		
		return AK_Fail;
	}

	static AKRESULT ReadBlocking(
		AkFileDesc& in_fileDesc,		// Returned file identifier/handle.
		void* in_pBuffer,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.
		AkUInt32	in_uPosition,		// Position from which to start reading.
		AkUInt32	in_uSizeToRead,		// Size to read. Must be a multiple of CAkFileHelpers::s_uRequiredBlockSize.
		AkUInt32& out_uSizeRead		// Returned size read.        
		)
	{
		return ReadBlocking(in_fileDesc.hFile, in_pBuffer, in_uPosition, in_uSizeToRead, out_uSizeRead);
	}

	static inline AKRESULT WriteBlocking(
		AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pData,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.		
		AkUInt64		in_uPosition,		// Position from which to start writing.
		AkUInt32		in_uSizeToWrite)
	{
		nn::Result result = nn::fs::WriteFile(in_hFile, in_uPosition, in_pData, in_uSizeToWrite, nn::fs::WriteOption());
		return result.IsSuccess() ? AK_Success : AK_Fail;
	}

	/// Returns AK_Success if the directory is valid, AK_Fail if not.
	/// For validation purposes only.
	/// Some platforms may return AK_NotImplemented, in this case you cannot rely on it.
	static inline AKRESULT CheckDirectoryExists(const AkOSChar* in_pszBasePath)
	{
		nn::fs::DirectoryEntryType eType;
		nn::Result result = nn::fs::GetEntryType(&eType, in_pszBasePath);
		return (result.IsSuccess() && eType == nn::fs::DirectoryEntryType_Directory) ? AK_Success : AK_Fail;
	}

	static inline AKRESULT CreateEmptyDirectory(const AkOSChar* in_pszDirectoryPath)
	{
		nn::Result result;
		result = nn::fs::CreateDirectory(in_pszDirectoryPath);
		if (!result.IsSuccess() && !nn::fs::ResultPathAlreadyExists::Includes(result))
			return AK_Fail;

		return AK_Success;
	}

	static inline AKRESULT RemoveEmptyDirectory(const AkOSChar* in_pszDirectoryPath)
	{
		nn::Result result;
		result = nn::fs::DeleteDirectory(in_pszDirectoryPath);
		if (!result.IsSuccess())
			return AK_Fail;

		return AK_Success;
	}

	static inline AKRESULT GetDefaultWritablePath(AkOSChar* out_pszPath, size_t in_maxPathSize)
	{
		if (out_pszPath == nullptr)
			return AK_InsufficientMemory;

		out_pszPath[0] = '\0';
		AKPLATFORM::SafeStrCpy(out_pszPath, kDefaultWritablePath, in_maxPathSize);
		return AK_Success;
	}

	/// Translate path separators to "/"
	static inline AkOSChar* ConvertPath(AkOSChar* io_path)
	{
		for (int i = 0; i < strlen(io_path); i++)
			if (io_path[i] == '\\')
				io_path[i] = '/';
		return io_path;
	}
	
	static void NormalizeDirectorySeparators(AkOSChar* io_path)
	{
		// Ounce API asserts on paths containing backslashes. Convert backslashes to slashes automatically.
		AkOSChar* c = io_path;
		while (*c != '\0')
		{
			if (*c == '\\')
			{
				*c = '/';
			}
			c++;
		}

	}

};
