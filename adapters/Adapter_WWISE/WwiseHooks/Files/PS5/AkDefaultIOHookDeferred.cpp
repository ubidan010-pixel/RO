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
// AkDefaultIOHookDeferred.cpp
//
// Default Deferred low level IO hook (AK::StreamMgr::IAkLowLevelIOHook) 
// and file system (AK::StreamMgr::IAkFileLocationResolver) implementation 
// on PS5. It can be used as a standalone implementation of the 
// Low-Level I/O system.
// 
// AK::StreamMgr::IAkFileLocationResolver: 
// Resolves file location using simple path concatenation logic 
// (implemented in ../Common/CAkFileLocationBase). It can be used as a 
// standalone Low-Level IO system, or as part of a multi device system. 
// In the latter case, you should manage multiple devices by implementing 
// AK::StreamMgr::IAkFileLocationResolver elsewhere (you may take a look 
// at class CAkDefaultLowLevelIODispatcher).
//
// AK::StreamMgr::IAkLowLevelIOHook: 
// Uses the standard, Deferred API for I/O (cellFsRead, cellFsWrite).
//
// Init() creates a streaming device (by calling AK::StreamMgr::CreateDevice()).
// If there was no AK::StreamMgr::IAkFileLocationResolver previously registered 
// to the Stream Manager, this object registers itself as the File Location Resolver.
//
//////////////////////////////////////////////////////////////////////
#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE

#include "AkDefaultIOHookDeferred.h"
#include "AkFileHelpers.h"
#include "../Common/AkGeneratedSoundBanksResolver.h"

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkMemoryMgrFuncs.h>
#include <AK/Tools/Common/AkInstrument.h>
#include <AK/Tools/Common/AkObject.h>
#include <ampr.h>

#define _DEFERRED_DEVICE_NAME		(L"PS5 Deferred")	// Device Deferred name.

const AkUInt32 CmdBufferDataSize = 1 * 1024;
const AkUInt32 AprBatchReadSize = 8;
const AkUInt32 MaxActiveBatches = 4;

const AkInt32 EvIdTerminateThread = MaxActiveBatches + 1;

CAkDefaultIOHookDeferred::CAkDefaultIOHookDeferred()
: m_deviceID( AK_INVALID_DEVICE_ID )
, m_pAprBatches(NULL)
, m_uNextAprBatchIdx(0)
, m_pAprCmdBufferData(NULL)
, m_ppAprTransferInfoData(NULL)
{
}

CAkDefaultIOHookDeferred::~CAkDefaultIOHookDeferred()
{
}

// Initialization/termination. Init() registers this object as the one and 
// only File Location Resolver if none were registered before. Then 
// it creates a streaming device with scheduler type AK_SCHEDULER_Deferred.
AKRESULT CAkDefaultIOHookDeferred::Init(
	const AkDeviceSettings &	in_deviceSettings,		// Device settings.
	AkOSChar * 					in_pszMountPoint/*=NULL*/	// Mount point (if NULL, SYS_DEV_HDD0 is used).
	)
{	
	// If the Stream Manager's File Location Resolver was not set yet, set this object as the 
	// File Location Resolver (this I/O hook is also able to resolve file location).
	if ( !AK::StreamMgr::GetFileLocationResolver() )
		AK::StreamMgr::SetFileLocationResolver( this );

	// Create a device in the Stream Manager, specifying this as the hook.
	AKRESULT res = AK::StreamMgr::CreateDevice( in_deviceSettings, this, m_deviceID);
	if (res != AK_Success)
		return res;

	// Creation of eventQueue for ioHook callback thread
	if (sceKernelCreateEqueue(&m_callbackEventQueue, "AkDefaultIOHookDeferredEqueue") != SCE_OK)
	{
		return AK_Fail;
	}

	// Creation of kernel event to terminate ioHook Callback thread
	if (sceKernelAddUserEvent(m_callbackEventQueue, EvIdTerminateThread) != SCE_OK)
	{
		return AK_Fail;
	}

	// Initialize the array of aprBatches
	m_pAprBatches = (AprBatch*)AkAlloc(AkMemID_Streaming, sizeof(AprBatch) * MaxActiveBatches);
	m_pAprCmdBufferData = AkAlloc(AkMemID_Streaming, CmdBufferDataSize * MaxActiveBatches);
	m_ppAprTransferInfoData = (AkAsyncIOTransferInfo**)AkAlloc(AkMemID_Streaming, sizeof(AkAsyncIOTransferInfo*) * AprBatchReadSize * MaxActiveBatches);
	if (m_pAprBatches == NULL || m_pAprCmdBufferData == NULL || m_ppAprTransferInfoData == NULL)
	{
		return AK_InsufficientMemory;
	}
	for (AkUInt32 i = 0; i < MaxActiveBatches; ++i)
	{
		m_pAprBatches[i].submitId = 0;
		m_pAprBatches[i].resultBuffer.result = SCE_OK;
		m_pAprBatches[i].resultBuffer.errorOffset = 0;

		m_pAprBatches[i].uNumTransferInfos = 0;
		m_pAprBatches[i].ppTransferInfos = (m_ppAprTransferInfoData + AprBatchReadSize * i);
		m_pAprBatches[i].bAvailableForWrite = true;

		// Creation of kernel events to indicate batch op completion
		if (sceKernelAddAmprEvent(m_callbackEventQueue, i, &m_pAprBatches[i]) != SCE_OK)
		{
			return AK_Fail;
		}
	}
	m_uNextAprBatchIdx = 0;

	// Create a thread to listen for read callback events on
	extern AkPlatformInitSettings g_PDSettings;
	AkThreadProperties threadProps;
	AKPLATFORM::AkGetDefaultHighPriorityThreadProperties(threadProps);
	AKPLATFORM::AkCreateThread(
		CAkDefaultIOHookDeferred::AkDefaultIOHookDeferredCallback,			// Start address
		this,										// Parameter
		threadProps,								// thread properties
		&m_callbackThread,							// AkHandle
		"AKDefaultIOHookDeferredCallback");			// Debug name
	if (!AKPLATFORM::AkIsValidThread(&m_callbackThread))
	{
		// Thread creation failed. 
		return AK_Fail;
	}


	return AK_Success;
}

void CAkDefaultIOHookDeferred::Term()
{
	// Fire the specific eventId to terminate the thread, and wait for it to finish
	if (AKPLATFORM::AkIsValidThread(&m_callbackThread))
	{
		sceKernelTriggerUserEvent(m_callbackEventQueue, EvIdTerminateThread, NULL);
		AKPLATFORM::AkWaitForSingleThread(&m_callbackThread);
		AKPLATFORM::AkClearThread(&m_callbackThread);
	}

	sceKernelDeleteUserEvent(m_callbackEventQueue, EvIdTerminateThread);
	for (AkUInt32 i = 0; i < MaxActiveBatches; ++i)
	{
		sceKernelDeleteAmprEvent(m_callbackEventQueue, i);
	}
	sceKernelDeleteEqueue(m_callbackEventQueue);

	AkFree(AkMemID_Streaming, m_pAprCmdBufferData);
	AkFree(AkMemID_Streaming, m_ppAprTransferInfoData);
	AkFree(AkMemID_Streaming, m_pAprBatches);
	m_pAprCmdBufferData = NULL;
	m_ppAprTransferInfoData = NULL;
	m_pAprBatches = NULL;

	CAkMultipleFileLocation::Term();

	if ( AK::StreamMgr::GetFileLocationResolver() == this )
		AK::StreamMgr::SetFileLocationResolver( NULL );
	
	AK::StreamMgr::DestroyDevice( m_deviceID );
}

//
// IAkFileLocationAware implementation.
//-----------------------------------------------------------------------------

AKRESULT CAkDefaultIOHookDeferred::OutputSearchedPaths(
	AKRESULT in_result,
	const AkFileOpenData& in_FileOpen,
	AkOSChar* out_searchedPath,
	AkInt32 in_pathSize
)
{
	return CAkMultipleFileLocation::OutputSearchedPaths(in_FileOpen, out_searchedPath, in_pathSize);
}

//
// IAkLowLevelIOHook implementation.
//-----------------------------------------------------------------------------
bool CAkDefaultIOHookDeferred::BatchOpenWorker(AkOSChar* in_szBasePath, AkUInt32 in_uNumFiles, AkAsyncFileOpenData** in_ppItems, AKRESULT* io_pDispatchResults)
{
	AkUInt32 uReadCount = 0;
	AkUInt32 uTotalCompleted = 0;
	
	SceAprFileId* aprFileId = (SceAprFileId*)AkAlloca(sizeof(SceAprFileId) * in_uNumFiles);
	size_t* fileSize = (size_t*)AkAlloca(sizeof(size_t) * in_uNumFiles);	
	char* pPathStorage = (char*)AkAlloca(AK_MAX_PATH * in_uNumFiles);
	AkOSChar** ppPaths = (char**)AkAlloca(sizeof(AkOSChar*) * in_uNumFiles);
	
	//Find the items we did not try to open yet (in case of multiple locations)
	for (AkUInt32 i = 0; i < in_uNumFiles; i++)
	{
		if (in_ppItems[i]->pFileDesc == nullptr)
		{
			if (in_ppItems[i]->eOpenMode == AK_OpenModeRead)	//Write mode should go through regular API.
			{
				ppPaths[uReadCount] = pPathStorage;
				pPathStorage += AK_MAX_PATH;

				AKRESULT res = GetFullFilePath(*in_ppItems[i], ppPaths[uReadCount], in_szBasePath);
				if (res != AK_Success)
				{
					pPathStorage -= AK_MAX_PATH;
					io_pDispatchResults[i] = res;
					continue;
				}
				uReadCount++;
			}
		}
	}

	AkUInt32 uOriginalIdx = 0;
	while (uReadCount != 0)
	{
		AkUInt32 errorIdx = uReadCount;
		int sceErr = sce::Ampr::Apr::resolveFilepathsToIdsAndFileSizes((const AkOSChar**)ppPaths, uReadCount, aprFileId, fileSize, &errorIdx);

		if (sceErr == SCE_OK)
			errorIdx = uReadCount;

		AkUInt32 uStart = uOriginalIdx;
		AkUInt32 uLast = uOriginalIdx + errorIdx;		
		for (AkUInt32 i = 0; uOriginalIdx < uLast; uOriginalIdx++, i++)
		{
			if (in_ppItems[uOriginalIdx]->pFileDesc == nullptr && in_ppItems[uOriginalIdx]->eOpenMode == AK_OpenModeRead)
			{
				in_ppItems[uOriginalIdx]->pFileDesc = CreateDescriptor();
				if (in_ppItems[uOriginalIdx]->pFileDesc)
				{
					in_ppItems[uOriginalIdx]->pFileDesc->iFileSize = fileSize[i];

					((AkPS5FileDesc*)(in_ppItems[uOriginalIdx]->pFileDesc))->idApr = aprFileId[i];
					in_ppItems[uOriginalIdx]->pFileDesc->hFile = 0;
					in_ppItems[uOriginalIdx]->pFileDesc->deviceID = m_deviceID;
					io_pDispatchResults[uOriginalIdx] = AK_Success;
				}
				else
					io_pDispatchResults[uOriginalIdx] = AK_InsufficientMemory;

				uTotalCompleted++;
			}
		}

		AkUInt32 range = uStart + uOriginalIdx;
		if (sceErr != SCE_OK)
		{
			switch (sceErr)
			{
			case SCE_KERNEL_ERROR_ENAMETOOLONG: io_pDispatchResults[uOriginalIdx] = AK_FilePathTooLong; break;
			case SCE_KERNEL_ERROR_EINVAL:	//Invalid parameter is returned if the path is a network path. Let's go through the regular file API.
			case SCE_KERNEL_ERROR_ENOENT:
			case SCE_KERNEL_ERROR_ENOTDIR: io_pDispatchResults[uOriginalIdx] = AK_FileNotFound; break;
			default: io_pDispatchResults[uOriginalIdx] = AK_UnknownFileError; break;
			}

			range++;	//Skip the entry with an error
		}

		ppPaths += range;
		uReadCount -= range;
		aprFileId += range;
		fileSize += range;
	}

	return uTotalCompleted == in_uNumFiles;
}

AkFileDesc* CAkDefaultIOHookDeferred::CreateDescriptor(const AkFileDesc* in_pCopy)
{
	if (!in_pCopy)
		return AkNew(AkMemID_Streaming, AkFileDescType());

	return AkNew(AkMemID_Streaming, AkFileDescType(*(AkFileDescType*)in_pCopy));
}

void CAkDefaultIOHookDeferred::BatchOpen(
	AkUInt32	in_uNumFiles,		///< Number of transfers to process
	AkAsyncFileOpenData** in_ppItems	///< List of files to open. See remarks above.
)
{
	bool bAllProcessed = false;
	AKRESULT * arBatchOpenResults = (AKRESULT*)AkAlloca(sizeof(AKRESULT) * in_uNumFiles);
	for (AkListBareLight<FilePath>::Iterator it = m_Locations.Begin(); it != m_Locations.End() && !bAllProcessed; ++it)
		bAllProcessed = BatchOpenWorker((*it)->szPath, in_uNumFiles, in_ppItems, arBatchOpenResults);

	//Try to open the missing files through the regular API. This is necessary for Write access and also for network locations.
	for (AkUInt32 i = 0; i < in_uNumFiles; i++)
	{
		AkAsyncFileOpenData* pItem = in_ppItems[i];
		if (pItem->pFileDesc == nullptr)
		{
			// File could not be opened by BatchOpenWorker
			AKRESULT eResult = Open(*pItem, pItem->pFileDesc);
			pItem->pCallback(pItem, eResult);
		}
		else
		{
			// File was opened by BatchOpenWorker, report the result
			pItem->pCallback(pItem, arBatchOpenResults[i]);
		}
	}
}

// Reads data from a file (asynchronous).
void CAkDefaultIOHookDeferred::BatchRead(
	AkUInt32				in_uNumTransfers,		///< Number of transfers to process
	BatchIoTransferItem*	in_pTransferItems		///< List of transfer items to process
	)
{
	// Arrays of data to send to submitAprReads
	AkUInt32 uNumDeferredReads = 0;
	AkAsyncIOTransferInfo* ppAprTransferInfos[AprBatchReadSize];
	SceAprFileId pAprFileIds[AprBatchReadSize];

	for (AkUInt32 i = 0; i < in_uNumTransfers; ++i)
	{
		// For file-handles opened with sceKernel functionality, just read them each and callback immediately
		AkFileHandle hFile = in_pTransferItems[i].pFileDesc->hFile;
		if (hFile != 0)
		{
			AkAsyncIOTransferInfo* transferInfo = in_pTransferItems[i].pTransferInfo;
			ssize_t sizeRead = sceKernelPread(hFile,
				transferInfo->pBuffer,
				transferInfo->uRequestedSize,
				transferInfo->uFilePosition);
			if (sizeRead == transferInfo->uRequestedSize)
			{
				AKRESULT eTransferResult = AK_Success;
				transferInfo->pCallback(transferInfo, eTransferResult);
			}
			else
			{
				transferInfo->pCallback(transferInfo, AK_Fail);
			}
		}
		else // For APR file-ids, add them to the list of transferInfos to submit (and submit them if our batch is full)
		{
			ppAprTransferInfos[uNumDeferredReads] = in_pTransferItems[i].pTransferInfo;
			
			pAprFileIds[uNumDeferredReads] = ((AkPS5FileDesc*)(in_pTransferItems[i].pFileDesc))->idApr;
			uNumDeferredReads++;
			if (uNumDeferredReads == AprBatchReadSize)
			{
				SubmitAprReads(uNumDeferredReads, ppAprTransferInfos, pAprFileIds);
				uNumDeferredReads = 0;
			}
		}
	}

	// Submit any more deferred reads that are queued up
	if (uNumDeferredReads > 0)
	{
		SubmitAprReads(uNumDeferredReads, ppAprTransferInfos, pAprFileIds);
	}
}

void CAkDefaultIOHookDeferred::SubmitAprReads(
	AkUInt32 numTransferInfos,
	AkAsyncIOTransferInfo** ppTransferInfos,
	SceAprFileId* pAprFileIds)
{
	// Get the next available aprBatch (wait for it to finish if it hasn't already) and advance the batch index
	AkUInt32 batchIdx = m_uNextAprBatchIdx;
	AprBatch* pAprBatch = &m_pAprBatches[batchIdx];
	if (!pAprBatch->bAvailableForWrite)
	{
		// wait for command buffer completion before proceeding
		sce::Ampr::Apr::waitCommandBufferCompletion(pAprBatch->submitId);

		// wait for bAvailableForWrite to actually be flushed so we can start writing to the batch again
		while (!pAprBatch->bAvailableForWrite)
		{
			sceKernelUsleep(10);
		}
	}
	m_uNextAprBatchIdx = (batchIdx < MaxActiveBatches - 1) ? batchIdx + 1 : 0;

	// Prepare the command buffer and other batch data
	sce::Ampr::AprCommandBuffer cmdBuffer;
	cmdBuffer.setBuffer((void*)((uintptr_t)m_pAprCmdBufferData + CmdBufferDataSize * batchIdx), CmdBufferDataSize);
	for (AkUInt32 i = 0; i < numTransferInfos; ++i)
	{
		AkAsyncIOTransferInfo* transferInfo = ppTransferInfos[i];
		pAprBatch->ppTransferInfos[i] = ppTransferInfos[i];
		AKASSERT(AKPLATFORM::CheckMemoryProtection(transferInfo->pBuffer, transferInfo->uRequestedSize, SCE_KERNEL_PROT_AMPR_ALL));
		if (cmdBuffer.readFile(pAprFileIds[i], transferInfo->pBuffer, transferInfo->uRequestedSize, transferInfo->uFilePosition) != SCE_OK)
		{
			goto critical_error;
		}
	}

	if (cmdBuffer.writeKernelEventQueueOnCompletion(m_callbackEventQueue, batchIdx, 0) != SCE_OK)
	{
		goto critical_error;
	}

	pAprBatch->uNumTransferInfos = numTransferInfos;
	pAprBatch->bAvailableForWrite = false;

	if (sce::Ampr::Apr::submitCommandBufferAndGetResult(
			&cmdBuffer,
			sce::Ampr::Apr::Priority::kPriority3,
			&pAprBatch->resultBuffer,
			&pAprBatch->submitId) 
		!= SCE_OK)
	{
		goto critical_error;
	}

	return;

critical_error:
	// Mark all transfers as failed.
	for (AkUInt32 i = 0; i < numTransferInfos; ++i)
	{
		AkAsyncIOTransferInfo* transferInfo = ppTransferInfos[i];
		transferInfo->pCallback(transferInfo, AK_Fail);
	}
}

void CAkDefaultIOHookDeferred::CompleteAprBatch(AprBatch* pAprBatch)
{
	// Finalize the aprBatch result, and close it up
	sce::Ampr::Apr::waitCommandBufferCompletion(pAprBatch->submitId);

	// Issue the batch callback
	AKRESULT eCumulativeResult = pAprBatch->resultBuffer.result == SCE_OK ? AK_Success : AK_Fail;
	for (AkUInt32 i = 0; i < pAprBatch->uNumTransferInfos; i++)
	{
		AkAsyncIOTransferInfo* pTransferInfo = pAprBatch->ppTransferInfos[i];
		pTransferInfo->pCallback(pTransferInfo, eCumulativeResult);
	}

	// Set the flag indicating that we're entirely done with this batch
	pAprBatch->bAvailableForWrite = true;
}

AK_DECLARE_THREAD_ROUTINE(CAkDefaultIOHookDeferred::AkDefaultIOHookDeferredCallback)
{
	AK_INSTRUMENT_THREAD_START("AkDefaultIOHookDeferredCallback");
	AK::MemoryMgr::InitForThread();
	CAkDefaultIOHookDeferred* pIoHook = (CAkDefaultIOHookDeferred*)(lpParameter);
	
	SceKernelEvent eventData;
	AkInt32 numEventsReceived = 0;

	//AKRESULT eResults[AprBatchReadSize];
	while (true)
	{
		int result = sceKernelWaitEqueue(pIoHook->m_callbackEventQueue, &eventData, 1, &numEventsReceived, NULL);
		if (result == SCE_OK)
		{
			// if the eventId wasn't for our termination 'user' eventid, handle the event in question; otherwise end the loop
			if (sceKernelGetEventId(&eventData) != EvIdTerminateThread)
			{
				pIoHook->CompleteAprBatch((AprBatch*)sceKernelGetEventUserData(&eventData));
			}
			else
			{
				break;
			}
		}
	}

	AK::MemoryMgr::TermForThread();
	AkExitThread(AK_RETURN_THREAD_OK);
}

// Writes data to a file (synchronous). 
void CAkDefaultIOHookDeferred::BatchWrite(
	AkUInt32				in_uNumTransfers,		///< Number of transfers to process
	BatchIoTransferItem*	in_pTransferItems		///< List of transfer items to process
)
{
	for (AkUInt32 i = 0; i < in_uNumTransfers; ++i)
	{
		BatchIoTransferItem ioTransferItem = in_pTransferItems[i];
		AKASSERT(ioTransferItem.pTransferInfo->pBuffer);
		AKASSERT(ioTransferItem.pFileDesc->hFile != 0);
		// Write to file
		ssize_t uSizeWritten = sceKernelPwrite(
			ioTransferItem.pFileDesc->hFile,
			ioTransferItem.pTransferInfo->pBuffer,
			ioTransferItem.pTransferInfo->uRequestedSize,
			ioTransferItem.pTransferInfo->uFilePosition);
		
		AKRESULT eResult;
		if (uSizeWritten == 0)
		{
			eResult = AK_Fail;
		}
		else
		{
			eResult = AK_Success;
		}
		ioTransferItem.pTransferInfo->pCallback(ioTransferItem.pTransferInfo, eResult);
	}
}

AKRESULT CAkDefaultIOHookDeferred::Open(const AkFileOpenData& in_FileOpen, AkFileDesc*& out_pFileDesc)
{
	out_pFileDesc = CreateDescriptor();
	if (out_pFileDesc)
	{
		AKRESULT eResult = CAkMultipleFileLocation::Open(in_FileOpen, true, *out_pFileDesc);
		out_pFileDesc->deviceID = m_deviceID;
		if (eResult != AK_Success)
		{
			AkDelete(AkMemID_Streaming, out_pFileDesc);
			out_pFileDesc = nullptr;
		}
		return eResult;
	}
	else
	{
		return AK_InsufficientMemory;
	}
}

// Cleans up a file.
AKRESULT CAkDefaultIOHookDeferred::Close(
    AkFileDesc * in_pFileDesc	// File descriptor.
    )
{
	if (in_pFileDesc)
	{
		 if(in_pFileDesc->hFile != 0)
			CAkFileHelpers::CloseFile(*in_pFileDesc);

		 AkDelete(AkMemID_Streaming, in_pFileDesc);
	}
	return AK_Success;
}

// Returns the block size for the file or its storage device. 
AkUInt32 CAkDefaultIOHookDeferred::GetBlockSize(
    AkFileDesc &  /*in_fileDesc*/     // File descriptor.
    )
{
	// No constraint on block size (file seeking).
    return 1;
}


// Returns a description for the streaming device above this low-level hook.
void CAkDefaultIOHookDeferred::GetDeviceDesc(
    AkDeviceDesc & 
#ifndef AK_OPTIMIZED
	out_deviceDesc      // Description of associated low-level I/O device.
#endif
    )
{
#ifndef AK_OPTIMIZED
	out_deviceDesc.deviceID       = m_deviceID;
	out_deviceDesc.bCanRead       = true;
	out_deviceDesc.bCanWrite      = true;
	AKPLATFORM::SafeStrCpy( out_deviceDesc.szDeviceName, _DEFERRED_DEVICE_NAME, AK_MONITOR_DEVICENAME_MAXLENGTH );
	out_deviceDesc.uStringSize   = (AkUInt32)wcslen( out_deviceDesc.szDeviceName ) + 1;
#endif
}

// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
AkUInt32 CAkDefaultIOHookDeferred::GetDeviceData()
{
	return 1;
}
#endif
