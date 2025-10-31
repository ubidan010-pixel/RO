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
// AkDefaultIOHookDeferred.h
//
// Default deferred low level IO hook (AK::StreamMgr::IAkLowLevelIOHook) 
// and file system (AK::StreamMgr::IAkFileLocationResolver) implementation.
// 
// AK::StreamMgr::IAkFileLocationResolver: 
//	Resolves file location using simple path concatenation logic.
//	It can be used as a standalone Low-Level IO system, or as part of a multi device system.
//	In the latter case, you should manage multiple devices by implementing 
//	AK::StreamMgr::IAkFileLocationResolver elsewhere (you may take a look 
//	at class CAkDefaultLowLevelIODispatcher).
//
// AK::StreamMgr::IAkLowLevelIOHook: 
//	Uses DirectStorage APIs for files open with AK_OpenModeRead,
//	otherwise uses standard Win32 APIs.
//
// -> DirectStorage APIs:
//	Calls to BatchRead should not block unless the maximum number of simultaneous
//	batch requests is reached (see s_MaxActiveBatches). All requests in BatchRead
//	are enqueued in batches and submitted after all requests have been enqueued.
//	Each batch has its own entry in the IDStorageStatusArrayX which is polled in
//	a separate thread to dispatch AkBatchIOCallback when the batch operation is complete.
//	When there is no outgoing requests, the consumer thread waits on an event until either
//	a new batch is submitted or thread termination is requested.
//   
// -> Win32 APIs:
//	Calls to ::ReadFile() and ::WriteFile() 
//	do not block because files are opened with the FILE_FLAG_OVERLAPPED flag. 
//	Transfer completion is handled by internal FileIOCompletionRoutine function,
//	which then calls the AkBatchIOCallback.
//
// Init() creates a streaming device (by calling AK::StreamMgr::CreateDevice()).
// If there was no AK::StreamMgr::IAkFileLocationResolver previously registered 
// to the Stream Manager, this object registers itself as the File Location Resolver.
//
//////////////////////////////////////////////////////////////////////
#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "AkDefaultIOHookDeferred.h"
#include <AK/SoundEngine/Common/AkAtomic.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/Tools/Common/AkInstrument.h>
#include <AK/Tools/Common/AkObject.h>
#include "AkFileHelpers.h"
//#include "AkGeneratedSoundBanksResolver.h"

// Device info.
#define XBOXSERIESX_DEFERRED_DEVICE_NAME (L"Xbox Series X Deferred") // Default deferred device name.

// Win32 has a restriction that asynchronous requests be aligned on a 4k boundary and be a multiple of 4k in size. 
// With files opened with FILE_FLAG_NO_BUFFERING accesses should be made in multiple of the sector size.
// DirectStorage does not have a 4k alignment or size restriction. This means you don’t need to pad your data which
// just adds extra size to your package and internal buffers. However since this implementation uses both Win32 and
// DirectStorage, we define the block size as 4 KB. This 4 KB block size will be ignored if the DirectStorage API is used.
#define XBOXSERIESX_NO_BUFFERING_BLOCK_SIZE	(4096)

// We define here the number of read requests that can be batched together as well as how many batches can run concurrently.
// The recommendation is to create a queue with a capacity at least four times the largest number of requests created per frame.
// We use 4 active batches with AK_DEFAULT_MAX_CONCURRENT_IO (8) reads per batch. These numbers should be adjusted to fit your IO needs.
// Notes:
// * You should always submit together as many requests as possible since it can take up to 8 ms for a submission
//   to complete (even for a small number of requests). 
// * AkDeviceSettings::uMaxConcurrentIO should be s_MaxActiveBatch * s_MaxReadsPerBatch
//   to have this many requests dispatched concurrently by the streaming manager.
static const AkUInt32 s_MaxActiveBatches = 4;
static const AkUInt32 s_MaxReadsPerBatch = 8;

//
// Win32 OVERLAPPED Helper Functions.
//-----------------------------------------------------------------------------

// Structures for concurrent asynchronous transfers bookkeeping.
// Note 1: The pool is a fixed block size pool. It returns OVERLAPPED objects. Allocation is guaranteed
// because the pool size is MaxConcurrentIO * sizeof(OVERLAPPED).
// Note 2: accesses to memory pool are not locked, because we only use the platform SDK here,
// which is executed by the I/O thread when it is in an alertable state.
// If you release overlapped objects from your own thread, a lock is mandatory.
// Note 3: we use the hEvent field to store a pointer to the OverlappedIOTransferInfo structure.

struct OverlappedIOTransferInfo
{
	AkAsyncIOTransferInfo* pTransferInfo;	///< List of transfer items to process
};

// Get a free slot for an OVERLAPPED I/O transfer.
// Store the transfer info and callback in the OVERLAPPED::hEvent.
static OVERLAPPED* GetFreeOverlapped(AkAsyncIOTransferInfo* in_pTransferInfo)
{
	OVERLAPPED* pOverlapped = (OVERLAPPED*)AK::MemoryMgr::Malloc(AkMemID_Streaming, sizeof(OVERLAPPED) + sizeof(OverlappedIOTransferInfo));
	AKASSERT(pOverlapped || !"Too many concurrent transfers in the Low-Level IO");
	OverlappedIOTransferInfo* pTransfer = (OverlappedIOTransferInfo*)((AkUInt8*)pOverlapped + sizeof(OVERLAPPED));
	pTransfer->pTransferInfo = in_pTransferInfo;
	pOverlapped->hEvent = pTransfer;
	return pOverlapped;
}

// Release a slot after an OVERLAPPED I/O transfer.
static AkForceInline void ReleaseOverlapped(OVERLAPPED* in_pOverlapped)
{
	AK::MemoryMgr::Free(AkMemID_Streaming, in_pOverlapped);
}

// Local callback for overlapped I/O using Win32 API.
// For files open with write access.
VOID CALLBACK CAkDefaultIOHookDeferred::FileIOCompletionRoutine(
  DWORD dwErrorCode,
  DWORD dwNumberOfBytesTransfered,
  LPOVERLAPPED lpOverlapped
)
{
	OverlappedIOTransferInfo* pTransfer = (OverlappedIOTransferInfo*)(lpOverlapped->hEvent);
	AkAsyncIOTransferInfo* pTransferInfo = pTransfer->pTransferInfo;
	AKRESULT eResult = ERROR_SUCCESS == dwErrorCode ? AK_Success : AK_Fail;
	AKASSERT(eResult != AK_Success || (
		dwNumberOfBytesTransfered >= pTransferInfo->uRequestedSize &&
		dwNumberOfBytesTransfered <= pTransferInfo->uBufferSize));
	pTransferInfo->pCallback(pTransferInfo, eResult);
	ReleaseOverlapped(lpOverlapped);
}

//
// CAkDefaultIOHookDeferred Implementation.
//-----------------------------------------------------------------------------

CAkDefaultIOHookDeferred::CAkDefaultIOHookDeferred()
{
}

CAkDefaultIOHookDeferred::~CAkDefaultIOHookDeferred()
{
}

// Populates out_deviceSettings with the default settings that should be passed to Init()
// to get optimal results when using DirectStorage APIs.
// With these settings it will be possible to reach s_MaxActiveBatches and s_MaxReadsPerBatch.
// The granularity is also set to 64 KB (the recommendation is to read at least 32k with each read request).
void CAkDefaultIOHookDeferred::GetDefaultDeviceSettings(
	AkDeviceSettings& out_deviceSettings		///< The device settings to populate
)
{
	AK::StreamMgr::GetDefaultDeviceSettings(out_deviceSettings);
	
	// The recommendation is to read at least 32k with each read request.
	// Smaller reads will negatively impact overall bandwidth.
	// Note: to read soundbanks in 64 KB chunks, also set AkInitSettings::uBankReadBufferSize to this value.
	out_deviceSettings.uGranularity = 64 * 1024;

	// Allow dispatching as many IO requests as we can send to the DirectStorage APIs.
	out_deviceSettings.uMaxConcurrentIO = s_MaxActiveBatches * s_MaxReadsPerBatch;
}

// Initialization/termination. Init() registers this object as the one and 
// only File Location Resolver if none were registered before. Then 
// it creates a streaming device.
// Finally, DirectStorage objects, internal buffers and threads are allocated and initialized.
AKRESULT CAkDefaultIOHookDeferred::Init(
	const AkDeviceSettings& in_deviceSettings	///< Device settings.
	)
{
	m_uNextBatchIdx = 0;
	m_uNumEnqueuedBatches = 0;
	m_bStopThreads = false;
	
	// If the Stream Manager's File Location Resolver was not set yet, set this object as the 
	// File Location Resolver (this I/O hook is also able to resolve file location).
	if (!AK::StreamMgr::GetFileLocationResolver())
		AK::StreamMgr::SetFileLocationResolver(this);

	// Create a device in the Stream Manager, specifying this as the hook.
	AKRESULT res = AK::StreamMgr::CreateDevice( in_deviceSettings, this, m_deviceID);
	if (res != AK_Success)
		return res;

	// Create Direct Storage Factory
	HRESULT hResult = DStorageGetFactory(__uuidof(IDStorageFactoryX), (void**)(m_storage.factory.ReleaseAndGetAddressOf()));
	if (FAILED(hResult))
		return AK_Fail;

	// Create Direct Storage Queue (to send read requests to the hardware)
	DSTORAGE_QUEUE_DESC queueDesc = {};
	queueDesc.Priority = DSTORAGE_PRIORITY_NORMAL; // This can be set to a higher priority to handle DirectStorage requests before Win32 requests
	// The queue size has enough space for all reads and a status slot for each batch
	queueDesc.Capacity = AkClamp((s_MaxActiveBatches * s_MaxReadsPerBatch) + s_MaxActiveBatches, DSTORAGE_MIN_QUEUE_CAPACITY, DSTORAGE_MAX_QUEUE_CAPACITY);
	queueDesc.Name = "CAkDefaultIOHookDeferred"; // Debug name
	hResult = m_storage.factory->CreateQueue(&queueDesc, __uuidof(IDStorageQueueX), (void**)(m_storage.queue.ReleaseAndGetAddressOf()));
	if (FAILED(hResult))
		return AK_Fail;

	// Create Direct Storage Status Array
	// Reserve one slot per batch. This allows us to have s_MaxActiveBatch running concurrently,
	// each writing into their own slot in the status array upon completion.
	hResult = m_storage.factory->CreateStatusArray(
		s_MaxActiveBatches, u8"CAkDefaultIOHookDeferred Status Array", // Capacity, Debug name
		__uuidof(IDStorageStatusArrayX), (void**)(m_storage.status.ReleaseAndGetAddressOf())
	);
	if (FAILED(hResult))
		return AK_Fail;

	// Allocate info on each submitted request batch, up to s_MaxActiveBatches
	// This used to store batch submission info for callback upon completion. 
	m_pBatches = (CAkDefaultIOHookDeferred::Batch*)AkAlloc(AkMemID_Streaming, sizeof(Batch) * s_MaxActiveBatches);
	if (m_pBatches == nullptr)
		return AK_InsufficientMemory;

	// Allocate memory for transfer info pointers, 1 for each read, for each batch
	// This is used to store each transfer request enqueued for a batch
	// which is sent back to the caller when the batch submission is complete.
	m_pTransferInfoData = (AkAsyncIOTransferInfo**)AkAlloc(AkMemID_Streaming, s_MaxReadsPerBatch * s_MaxActiveBatches * sizeof(AkAsyncIOTransferInfo*));
	if (m_pTransferInfoData == nullptr)
		return AK_InsufficientMemory;

	for (AkUInt64 i = 0; i < s_MaxActiveBatches; ++i)
	{
		m_pBatches[i].state = BatchState_Free;
		m_pBatches[i].uNumTransferInfo = 0;
		m_pBatches[i].pTransferInfo = m_pTransferInfoData + s_MaxReadsPerBatch * i;
	}

	// This event is be used to wait for new submitted batches
	// or for thread termination requests
	if (AKPLATFORM::AkCreateSemaphore(m_DStorageNumBatchesSubmittedSemaphore, 0) != AK_Success)
		return AK_Fail;

	// Create a thread to listen for read callback events on
	// This thread will wait for new batch submissions and dispatch
	// callbacks upon completion.
	extern AkPlatformInitSettings g_PDSettings;
	AkThreadProperties threadProps;
	AKPLATFORM::AkGetDefaultHighPriorityThreadProperties(threadProps);
	AKPLATFORM::AkCreateThread(
		CAkDefaultIOHookDeferred::AkDefaultIOHookDeferredCallback,	// Start address
		this,														// Parameter
		threadProps,												// thread properties
		&m_callbackThread,											// AkHandle
		"AKDefaultIOHookDeferredCallback"							// Debug name
	);
	if (!AKPLATFORM::AkIsValidThread(&m_callbackThread))
		return AK_Fail; // Thread creation failed. 

#ifdef _DEBUG
	// In Debug, listen for error notifications and log errors using OutputDebugMsg
	m_errorEvent = m_storage.queue->GetErrorEvent();
	if (m_errorEvent == nullptr)
		return AK_Success; // Allow error thread creation failure

	{
		AKPLATFORM::AkGetDefaultThreadProperties(threadProps);
		AKPLATFORM::AkCreateThread(
			CAkDefaultIOHookDeferred::AkDefaultIOHookDeferredErrorRoutine,	// Start address
			this,															// Parameter
			threadProps,													// thread properties
			&m_errorThread,													// AkHandle
			"AKDefaultIOHookDeferredErrorRoutine"							// Debug name
		);
		if (!AKPLATFORM::AkIsValidThread(&m_errorThread))
			return AK_Fail; // Thread creation failed. 
	}
#endif

	return AK_Success;
}

void CAkDefaultIOHookDeferred::Term()
{
	m_bStopThreads = true;

	if (AKPLATFORM::AkIsValidThread(&m_callbackThread))
	{
		// Terminate thread and wait for completion
		AKPLATFORM::AkReleaseSemaphore(m_DStorageNumBatchesSubmittedSemaphore, 1);
		AKPLATFORM::AkWaitForSingleThread(&m_callbackThread);
		AKPLATFORM::AkClearThread(&m_callbackThread);
	}

	if (m_pBatches != nullptr && m_storage.queue.Get() != nullptr)
	{
		// Cancel all requests in progress (mask = 0, value = 0 will match all requests)
		m_storage.queue->CancelRequestsWithTag(0, 0);

		// Wait until all operations complete before cleaning up resources
		for (AkUInt32 i = 0; i < s_MaxActiveBatches; ++i)
		{
			if (!m_pBatches[i].IsInState(BatchState_Submitted))
				continue; // no request pending for this batch

			while (!m_storage.status->IsComplete(i))
				AKPLATFORM::AkSleep(1);
		}

		// Clean-up Direct Storage resources
		m_storage.Reset();
	}

#ifdef _DEBUG
	// Also wait for the error thread to complete
	if (AKPLATFORM::AkIsValidThread(&m_errorThread))
	{
		AKPLATFORM::AkSignalEvent(m_errorEvent);
		AKPLATFORM::AkWaitForSingleThread(&m_errorThread);
		AKPLATFORM::AkClearThread(&m_errorThread);
	}
#endif

	if (m_pBatches != nullptr)
	{
		AkFree(AkMemID_Streaming, m_pBatches);
		m_pBatches = nullptr;
	}
	
	if (m_pTransferInfoData != nullptr)
	{
		AkFree(AkMemID_Streaming, m_pTransferInfoData);
		m_pTransferInfoData = nullptr;
	}

	AKPLATFORM::AkDestroySemaphore(m_DStorageNumBatchesSubmittedSemaphore);
#ifdef _DEBUG
	m_errorEvent = nullptr;
#endif

	CAkMultipleFileLocation::Term();

	if ( AK::StreamMgr::GetFileLocationResolver() == this )
		AK::StreamMgr::SetFileLocationResolver( NULL );

	AK::StreamMgr::DestroyDevice( m_deviceID );
}

AkFileDesc* CAkDefaultIOHookDeferred::CreateDescriptor(const AkFileDesc* in_pCopy)
{
	if (!in_pCopy)
		return AkNew(AkMemID_Streaming, AkFileDescType());

	return AkNew(AkMemID_Streaming, AkFileDescType(*(AkFileDescType*)in_pCopy));
}

// Returns a file descriptor for a given file name (string).
AKRESULT CAkDefaultIOHookDeferred::Open(const AkFileOpenData& in_FileOpen, AkFileDesc*& out_pFileDesc)
{
	AkXSXFileDesc* pDesc = (AkXSXFileDesc*)CreateDescriptor();
	if (!pDesc)
		return AK_InsufficientMemory;

	pDesc->deviceID = m_deviceID;

	AKRESULT eResult = AK_Success;
	if (in_FileOpen.eOpenMode == AK_OpenModeRead)
	{
		// If we are just doing reads, then we can use DirectStorage APIs at will
		eResult = DStorageOpen(in_FileOpen, pDesc);
	}
	else
	{
		// Use Win32 API for other requests (write or read+write)
		// (forward the open call to the AkFileHelpers)
		eResult = CAkMultipleFileLocation::Open(in_FileOpen, true, *pDesc);
	}

	if (eResult == AK_Success)
		out_pFileDesc = pDesc;
	else
		AkDelete(AkMemID_Streaming, pDesc);

	return eResult;
}

AKRESULT CAkDefaultIOHookDeferred::OutputSearchedPaths(
	AKRESULT in_result,
	const AkFileOpenData& in_FileOpen,
	AkOSChar* out_searchedPath,
	AkInt32 in_pathSize
)
{
	return CAkMultipleFileLocation::OutputSearchedPaths(in_FileOpen, out_searchedPath, in_pathSize);
}

AKRESULT CAkDefaultIOHookDeferred::DStorageOpen(
	const AkFileOpenData& in_FileOpen,
    AkXSXFileDesc *    io_pFileDesc
)
{
	AKASSERT(in_FileOpen.eOpenMode == AK_OpenModeRead && "Only file read is supported by DirectStorage");

	AkOSChar szFullFilePath[AK_MAX_PATH];
	AKRESULT res = GetFullFilePath(in_FileOpen, szFullFilePath);
	if (res != AK_Success)
		return res;

	Microsoft::WRL::ComPtr<IDStorageFileX> pFile = NULL;
	HRESULT hResult = m_storage.factory->OpenFile(szFullFilePath, __uuidof(IDStorageFileX), (void**)(pFile.ReleaseAndGetAddressOf()));
	if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hResult == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
		return AK_FileNotFound;
	if (hResult == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED) || hResult == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
		return AK_FilePermissionError;
	if (FAILED(hResult))
		return AK_UnknownFileError;
	
	WIN32_FILE_ATTRIBUTE_DATA info;
	if (GetFileAttributesEx(szFullFilePath, GetFileExInfoStandard, &info) == FALSE)
		return AK_UnknownFileError;

	io_pFileDesc->iFileSize = ((AkUInt64)info.nFileSizeHigh << 32) | info.nFileSizeLow;
	io_pFileDesc->uSector = 0;
	io_pFileDesc->pStorage = pFile.Detach();
	io_pFileDesc->hFile = 0;	

	return AK_Success;
}

//
// IAkLowLevelIOHook implementation.
//-----------------------------------------------------------------------------

void CAkDefaultIOHookDeferred::BatchOpen(
	AkUInt32						in_uNumFiles,	///< Number of transfers to process
	AkAsyncFileOpenData** in_ppItems					///< List of files to open. See remarks above.
)
{
	for (AkUInt32 i = 0; i < in_uNumFiles; ++i)
	{
		AkAsyncFileOpenData* pItem = in_ppItems[i];
		AKRESULT eResult = Open(*pItem, pItem->pFileDesc);
		pItem->pCallback(pItem, eResult);
	}
}

// Reads data from a file (asynchronous overload).
void CAkDefaultIOHookDeferred::BatchRead(
	AkUInt32				in_uNumTransfers,		///< Number of transfers to process
	BatchIoTransferItem*	in_pTransferItems		///< List of transfer items to process
	)
{
	Batch* pFreeBatch = nullptr;

	for (AkUInt32 i = 0; i < in_uNumTransfers; ++i)
	{
		BatchIoTransferItem& transferItem = in_pTransferItems[i];

		if (transferItem.pFileDesc->hFile != 0) // For file-handles opened with Win32 OpenFile, just read them with ReadFile
		{
			Win32Read(*transferItem.pFileDesc, *transferItem.pTransferInfo);
		}
		else // Otherwise, use DirectStorage API
		{
			pFreeBatch = DStorageWaitForNextFreeBatch();
			DStorageRead(pFreeBatch, transferItem);

			// Enqueue the batch if it's full
			if (pFreeBatch->uNumTransferInfo == s_MaxReadsPerBatch)
			{
				// Batch is full; enqueue it.
				DStorageEnqueueBatch(pFreeBatch);
				pFreeBatch = nullptr; // Batch is now enqueued, so it is no longer free
			}
		}
	}

	// If we enqueued some requests but have not enqueued a batch yet,
	// enqueue a batch for these requests now
	if (pFreeBatch != nullptr)
	{
		DStorageEnqueueBatch(pFreeBatch);
	}

	// Submit enqueued batches all at once
	DStorageSubmitEnqueuedBatches();
}

void CAkDefaultIOHookDeferred::Win32Read(
	AkFileDesc& in_fileDesc,					///< List of transfer items to process
	AkAsyncIOTransferInfo& in_transferInfo 		///< Information on the transfer, will be passed back to the callback
	)
{
	AKASSERT(in_fileDesc.hFile != INVALID_HANDLE_VALUE
		&& in_transferInfo.uRequestedSize > 0
		&& in_transferInfo.uBufferSize >= in_transferInfo.uRequestedSize);

	// If this assert comes up, it might be because this hook's GetBlockSize() return value is incompatible 
	// with the system's handling of file reading for this specific file handle.
	// If you are using the File Package extension, did you create your package with a compatible
	// block size? It should be a multiple of WIN32_NO_BUFFERING_BLOCK_SIZE. (check -blocksize argument in the File Packager command line)
	AKASSERT((in_transferInfo.uFilePosition % XBOXSERIESX_NO_BUFFERING_BLOCK_SIZE) == 0
		|| !"Requested file position for I/O transfer is inconsistent with block size");

	OVERLAPPED* pOverlapped = GetFreeOverlapped(&in_transferInfo);

	// Set file offset in OVERLAPPED structure.
	pOverlapped->Offset = (DWORD)(in_transferInfo.uFilePosition & 0xFFFFFFFF);
	pOverlapped->OffsetHigh = (DWORD)((in_transferInfo.uFilePosition >> 32) & 0xFFFFFFFF);

	// File was open with asynchronous flag. 
	// Read overlapped. 
	// Note: With a file handle opened with FILE_FLAG_NO_BUFFERING, ::ReadFileEx() supports read sizes that go beyond the end
	// of file. However, it does not support read sizes that are not a multiple of the drive's sector size.
	// Since the buffer size is always a multiple of the block size, let's use in_transferInfo.uBufferSize
	// instead of in_transferInfo.uRequestedSize.
	if (::ReadFileEx(in_fileDesc.hFile,
		in_transferInfo.pBuffer,
		in_transferInfo.uBufferSize,
		pOverlapped,
		CAkDefaultIOHookDeferred::FileIOCompletionRoutine) == FALSE)
	{
		ReleaseOverlapped(pOverlapped);
		in_transferInfo.pCallback(&in_transferInfo, AK_Fail);
	}
}

void CAkDefaultIOHookDeferred::DStorageRead(
	Batch *                 pBatch,                 ///< Batch to enqueue read requests
	BatchIoTransferItem&    in_transferItem         ///< Transfer item to process
	)
{
	AKASSERT(pBatch->IsInState(BatchState_Free));
	AKASSERT(pBatch->uNumTransferInfo < s_MaxReadsPerBatch);

	// Prepare the DirectStorage request
	DSTORAGE_REQUEST request = {};
	memset(&request, 0, sizeof(request));	
	((AkXSXFileDesc*)in_transferItem.pFileDesc)->pStorage.CopyTo(&request.File);
	request.Destination = in_transferItem.pTransferInfo->pBuffer;
	request.DestinationSize = in_transferItem.pTransferInfo->uRequestedSize;
	request.FileOffset = in_transferItem.pTransferInfo->uFilePosition;
	request.SourceSize = in_transferItem.pTransferInfo->uRequestedSize;

	// Use file pointer as the cancellation tag, allowing us to cancel all requests for a file at once.
	// Individual requests could be cancelled by hashing relevant fields of BatchIoTransferItem
	// at the cost of computing this hash.
	request.CancellationTag = (UINT64)request.File;

	// Enqueue the request into the queue
	m_storage.queue->EnqueueRequest(&request);

	// Save batch info for the consumer thread
	pBatch->pTransferInfo[pBatch->uNumTransferInfo] = in_transferItem.pTransferInfo;
	pBatch->uNumTransferInfo++;
}

void CAkDefaultIOHookDeferred::DStorageEnqueueBatch(Batch * in_pBatch)
{
	AKASSERT(in_pBatch->IsInState(BatchState_Free));
	AKVERIFY(AkAtomicCas32(&in_pBatch->state, BatchState_Enqueued, BatchState_Free));

	// Enqueue the status array along with the slot in the array we want to be signaled when all previous enqueued reads have completed
	// this will immediately flip the slot in the status array to not signaled
	m_storage.queue->EnqueueStatus(m_storage.status.Get(), m_uNextBatchIdx);
	++m_uNumEnqueuedBatches;

	// Proceed to the next batch
	m_uNextBatchIdx = (m_uNextBatchIdx + 1) % s_MaxActiveBatches;
}

// Actually submits all requests enqueued for the current batch
void CAkDefaultIOHookDeferred::DStorageSubmitEnqueuedBatches()
{
	if (m_uNumEnqueuedBatches == 0)
		return;

	// Submit pending batches
	m_storage.queue->Submit();
	for (AkUInt32 i = 0; i < m_uNumEnqueuedBatches; i++)
	{
		AkUInt32 idx = ((AkUInt32)m_uNextBatchIdx - (i + 1)) % s_MaxActiveBatches;
		AKVERIFY(AkAtomicCas32(&m_pBatches[idx].state, BatchState_Submitted, BatchState_Enqueued));
	}

	// and wake up consumer thread
	AKPLATFORM::AkReleaseSemaphore(m_DStorageNumBatchesSubmittedSemaphore, m_uNumEnqueuedBatches);
	m_uNumEnqueuedBatches = 0;
}

AK_DECLARE_THREAD_ROUTINE(CAkDefaultIOHookDeferred::AkDefaultIOHookDeferredCallback)
{
	AK_INSTRUMENT_THREAD_START("AkDefaultIOHookDeferredBatchCallback");
	AK::MemoryMgr::InitForThread();

	CAkDefaultIOHookDeferred* pIoHook = (CAkDefaultIOHookDeferred*)(lpParameter);

	// Ring-buffer index used to simulatenously iterate through batches and the status array.
	// Will be used to look for dispatched batches and to wait for the corresponding batch to complete
	// by polling the status array.
	AkUInt32 m_uNextStatus = 0;

	Batch* pBatch = &pIoHook->m_pBatches[m_uNextStatus];

	AKPLATFORM::AkWaitForSemaphore(pIoHook->m_DStorageNumBatchesSubmittedSemaphore);

	while (!pIoHook->m_bStopThreads)
	{
		// We woke up because a new request batch has been submitted.
		// Wait until all submitted requests are completed and dispatch callbacks.
		pBatch->SpinWaitForState(BatchState_Submitted);

		HRESULT hr = pIoHook->m_storage.status->GetHResult(m_uNextStatus);
		while (hr == E_PENDING)
		{
			AkThreadYield();
			hr = pIoHook->m_storage.status->GetHResult(m_uNextStatus);
		}

		// Request is complete, dispatch callback
		AKRESULT eLastResult = hr == S_OK ? AK_Success : AK_Fail;
		// We only have the last error or success otherwise, notify all results with the same value
		for (AkUInt32 i = 0; i < pBatch->uNumTransferInfo; ++i)
		{
			auto pTransferInfo = pBatch->pTransferInfo[i];
			pTransferInfo->pCallback(pTransferInfo, eLastResult);

		}

		pBatch->uNumTransferInfo = 0;
		AKVERIFY(AkAtomicCas32(&pBatch->state, BatchState_Free, BatchState_Submitted));

		m_uNextStatus = (m_uNextStatus + 1) % s_MaxActiveBatches;
		pBatch = &pIoHook->m_pBatches[m_uNextStatus];

		// Batch done, wait until more requests are submitted.
		AKPLATFORM::AkWaitForSemaphore(pIoHook->m_DStorageNumBatchesSubmittedSemaphore);
	}

	AK::MemoryMgr::TermForThread();
	AkExitThread(AK_RETURN_THREAD_OK);
}

// Writes data to a file (asynchronous overload) using Win32 API.
void CAkDefaultIOHookDeferred::BatchWrite(
	AkUInt32				in_uNumTransfers,		///< Number of transfers to process
	BatchIoTransferItem*	in_pTransferItems		///< List of transfer items to process
	)
{
	for (AkUInt32 i = 0; i < in_uNumTransfers; ++i)
	{
		BatchIoTransferItem ioTransferItem = in_pTransferItems[i];

		AKASSERT(ioTransferItem.pFileDesc->hFile != INVALID_HANDLE_VALUE && ioTransferItem.pTransferInfo->uRequestedSize > 0);
		
		OVERLAPPED * pOverlapped = GetFreeOverlapped( ioTransferItem.pTransferInfo );

		// Set file offset in OVERLAPPED structure.
		pOverlapped->Offset = (DWORD)(ioTransferItem.pTransferInfo->uFilePosition & 0xFFFFFFFF);
		pOverlapped->OffsetHigh = (DWORD)((ioTransferItem.pTransferInfo->uFilePosition >> 32) & 0xFFFFFFFF);

		// File was open with asynchronous flag, write with overlapped io
		if (::WriteFileEx(ioTransferItem.pFileDesc->hFile,
			ioTransferItem.pTransferInfo->pBuffer,
			ioTransferItem.pTransferInfo->uRequestedSize,
			pOverlapped,
			CAkDefaultIOHookDeferred::FileIOCompletionRoutine) == FALSE)
		{
			ioTransferItem.pTransferInfo->pCallback(ioTransferItem.pTransferInfo, AK_Fail);
			ReleaseOverlapped(pOverlapped);
		}
	}
}

// Close a file.
AKRESULT CAkDefaultIOHookDeferred::Close(AkFileDesc* in_pFileDesc)
{
	if (in_pFileDesc->hFile != 0) // Win32 API
	{
		CAkFileHelpers::CloseFile(*in_pFileDesc);
	}
	else // DirectStorage API
	{	
		((AkXSXFileDesc*)in_pFileDesc)->pStorage->Close();
	}

	AkDelete(AkMemID_Streaming, in_pFileDesc);

	return AK_Success;
}

// Returns the block size for the file or its storage device. 
AkUInt32 CAkDefaultIOHookDeferred::GetBlockSize(AkFileDesc& in_fileDesc)
{
	// If the hFile value has been populated, this means that this file was opened to be used by the Win32 API.
	// Win32 has a restriction that asynchronous requests be aligned on a 4k boundary and be a multiple of 4k in size. 
	if (in_fileDesc.hFile != 0)
		return XBOXSERIESX_NO_BUFFERING_BLOCK_SIZE;

	// Otherwise, this file will use the DirectStorage API.
	// DirectStorage does not have a 4k alignment or size restriction. This means you don’t need to pad your data which
	// just adds extra size to your package and internal buffers.
	return 1;
}

// Returns a description for the streaming device above this low-level hook.
void CAkDefaultIOHookDeferred::GetDeviceDesc(
	AkDeviceDesc &  
#ifndef AK_OPTIMIZED
	out_deviceDesc      ///< Description of associated low-level I/O device.
#endif
	)
{
#ifndef AK_OPTIMIZED
	AKASSERT( m_deviceID != AK_INVALID_DEVICE_ID || !"Low-Level device was not initialized" );

	// Deferred scheduler.
	out_deviceDesc.deviceID       = m_deviceID;
	out_deviceDesc.bCanRead       = true;
	out_deviceDesc.bCanWrite      = true;
	AKPLATFORM::SafeStrCpy(out_deviceDesc.szDeviceName, XBOXSERIESX_DEFERRED_DEVICE_NAME, AK_MONITOR_DEVICENAME_MAXLENGTH);
	out_deviceDesc.uStringSize   = (AkUInt32)wcslen(out_deviceDesc.szDeviceName) + 1;
#endif
}

// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
// Tip: An interesting application for custom profiling data in deferred devices is to display
// the number of requests currently pending in the low-level IO.
AkUInt32 CAkDefaultIOHookDeferred::GetDeviceData()
{
	return m_bAsyncOpen ? 1 : 0;
}

CAkDefaultIOHookDeferred::Batch * CAkDefaultIOHookDeferred::DStorageWaitForNextFreeBatch()
{
	Batch * pBatch = &m_pBatches[m_uNextBatchIdx];
	AkUInt32 state = AkAtomicLoad32(&pBatch->state);
	switch (state)
	{
	case BatchState_Free:
		// Got it!
		return pBatch;
	case BatchState_Enqueued:
		// This means we went around the batch ring and all batches are enqueued.
		// It's time to commit, which will turn it into the Submitted state.
		DStorageSubmitEnqueuedBatches();
		break;
	case BatchState_Submitted:
		// Nothing to do, we're going to wait on it.
		break;
	default:
		AKASSERT(!"Invalid batch state!");
	}

	pBatch->SpinWaitForState(BatchState_Free);
	return pBatch;
}

void CAkDefaultIOHookDeferred::Batch::SpinWaitForState(AkUInt32 uNewState)
{
	while (!IsInState(uNewState))
	{
		AkThreadYield();
	}
}

bool CAkDefaultIOHookDeferred::Batch::IsInState(AkUInt32 uState)
{
	return AkAtomicLoad32(&state) == uState;
}

//
// Error Reporting Thread
//-----------------------------------------------------------------------------

#ifdef _DEBUG

AK_DECLARE_THREAD_ROUTINE(CAkDefaultIOHookDeferred::AkDefaultIOHookDeferredErrorRoutine)
{
	AK_INSTRUMENT_THREAD_START("AkDefaultIOHookDeferredBatchCallback");

	CAkDefaultIOHookDeferred* pIoHook = (CAkDefaultIOHookDeferred*)(lpParameter);

	while (true)
	{
		AKPLATFORM::AkWaitForEvent(pIoHook->m_errorEvent);
		if (pIoHook->m_bStopThreads)
			break;

		DSTORAGE_ERROR_RECORD errorRecord;
		pIoHook->m_storage.queue->RetrieveErrorRecord(&errorRecord);

		AKPLATFORM::OutputDebugMsgV("CAkDefaultIOHookDeferred: %d DirectStorage failure(s). Last Failure Info:\n");
		if (errorRecord.FirstFailure.CommandType == DSTORAGE_COMMAND_TYPE_REQUEST)
		{
			AKPLATFORM::OutputDebugMsgV("  REQUEST: Error (%d) reading file \"%s\" (offset=%d, size=%d)\n",
				errorRecord.FirstFailure.HResult,
				errorRecord.FirstFailure.Request.Filename,
				errorRecord.FirstFailure.Request.FileOffset,
				errorRecord.FirstFailure.Request.SourceSize
			);
		}
		else if (errorRecord.FirstFailure.CommandType == DSTORAGE_COMMAND_TYPE_STATUS)
		{
			AKPLATFORM::OutputDebugMsgV("  STATUS: Error (%d) writing status index %d\n",
				errorRecord.FirstFailure.HResult,
				errorRecord.FirstFailure.Status.Index
			);
		}
		else // SIGNAL or NONE
		{
			// Since we don't use D3D12Fence, we don't expect SIGNAL errors
			AKASSERT(!"Unexpected DirectStorage Error");
		}
	}

	AkExitThread(AK_RETURN_THREAD_OK);
}

#endif // _DEBUG

#endif
