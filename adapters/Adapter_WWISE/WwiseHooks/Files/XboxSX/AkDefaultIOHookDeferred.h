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
// Examples of streaming initialization:
// 
// Standalone (registered as the one and only File Location Resolver):
/* 
	// Create Stream Manager.
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings( stmSettings );
	AK::IAkStreamMgr * pStreamMgr = AK::StreamMgr::Create( stmSettings );
	AKASSERT( pStreamMgr );

	// Create deferred device.
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings( deviceSettings );
	CAkDefaultIOHookDeferred hookIODeferred;
	AKRESULT eResult = hookIODeferred.Init( deviceSettings );
	AKASSERT( AK_SUCCESS == eResult );
*/
//
// As part of a system with multiple devices (the File Location Resolver is 
// implemented by CAkDefaultLowLevelIODispatcher):
/* 
	// Create Stream Manager.
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings( stmSettings );
	AK::IAkStreamMgr * pStreamMgr = AK::StreamMgr::Create( stmSettings );
	AKASSERT( pStreamMgr );

	// Create and register the File Location Resolver.
	CAkDefaultLowLevelIODispatcher lowLevelIODispatcher;
	AK::StreamMgr::SetFileLocationResolver( &lowLevelIODispatcher );

	// Create deferred device.
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings( deviceSettings );
	CAkDefaultIOHookDeferred hookIODeferred;
	AKRESULT eResult = hookIODeferred.Init( deviceSettings );
	AKASSERT( AK_SUCCESS == eResult );

	// Add it to the global File Location Resolver.
	lowLevelIODispatcher.AddDevice( hookIODeferred );

	// Create more devices.
	// ...
*/
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_DEFAULT_IO_HOOK_DEFERRED_H_
#define _AK_DEFAULT_IO_HOOK_DEFERRED_H_

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include "../Common/AkMultipleFileLocation.h"
#include <AK/Tools/Common/AkAssert.h>

#include <dstorage_xs.h>
#include <wrl/client.h>

//-----------------------------------------------------------------------------
// Name: class CAkDefaultIOHookDeferred.
// Desc: Implements IAkLowLevelIOHook low-level I/O hook, and 
//		 IAkFileLocationResolver. Can be used as a standalone Low-Level I/O
//		 system, or as part of a system with multiple devices.
//		 File location is resolved using simple path concatenation logic.
//-----------------------------------------------------------------------------
class CAkDefaultIOHookDeferred : public AK::StreamMgr::IAkFileLocationResolver
								,public AK::StreamMgr::IAkLowLevelIOHook
								,public CAkMultipleFileLocation
{
public:

	struct AkXSXFileDesc : public AkFileDesc
	{
		AkXSXFileDesc() {};
		AkXSXFileDesc(const AkXSXFileDesc& in_rCopy)
			: AkFileDesc(in_rCopy)
			, pStorage(in_rCopy.pStorage)
		{}
		Microsoft::WRL::ComPtr<IDStorageFileX> pStorage;
	};

	typedef AkXSXFileDesc AkFileDescType;

	CAkDefaultIOHookDeferred();
	virtual ~CAkDefaultIOHookDeferred();

	// Populates out_deviceSettings with the default settings that should be passed to Init()
	// to get optimal results when using DirectStorage APIs.
	// With these settings it will be possible to reach s_MaxActiveBatches and s_MaxReadsPerBatch.
	// The granularity is also set to 64 KB (the recommendation is to read at least 32k with each read request).
	static void GetDefaultDeviceSettings(
		AkDeviceSettings& out_deviceSettings			///< The device settings to populate
		);

	// Initialization/termination. Init() registers this object as the one and 
	// only File Location Resolver if none were registered before. Then 
	// it creates a streaming device.
	AKRESULT Init(
		const AkDeviceSettings &	in_deviceSettings	// Device settings.
		);

	void Term();

	// Allow creation of specialized AkFileDesc class for derivatives of this class.
	// Default implementation allocates a AkFileDesc with AkNew, which should be freed in Close().
	virtual AkFileDesc* CreateDescriptor(const AkFileDesc* in_pCopy = nullptr);
	
	//
	// IAkFileLocationAware interface.
	//-----------------------------------------------------------------------------

	virtual AKRESULT OutputSearchedPaths(
		AKRESULT in_result,						///< Result of the open call
		const AkFileOpenData& in_FileOpen,		///< File name or file ID (only one should be valid!), open flags, open mode
		AkOSChar* out_searchedPath,				///< String containing all searched paths
		AkInt32 in_pathSize						///< The maximum size of the string
	) override;

	//
	// IAkLowLevelIOHook interface.
	//-----------------------------------------------------------------------------

	virtual void BatchOpen(AkUInt32 in_uNumFiles, AkAsyncFileOpenData** in_ppItems) override;

	virtual void BatchRead(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems) override;

	virtual void BatchWrite(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems) override;

	// Cleans up a file.
	virtual AKRESULT Close(
		AkFileDesc*				in_pFileDesc				///< File descriptor.
		) override;

	// Returns the block size for the file or its storage device. 
	virtual AkUInt32 GetBlockSize(
		AkFileDesc&  			in_fileDesc				///< File descriptor.
		) override;

	// Returns a description for the streaming device above this low-level hook.
    virtual void GetDeviceDesc(
		AkDeviceDesc&  			out_deviceDesc         	///< Description of associated low-level I/O device.
		) override;

	// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
	virtual AkUInt32 GetDeviceData() override;

protected:

	// Open a file (synchronous)
	// This is a helper for subclasses.
	// Note: In this implementation, Open is ALWAYS synchronous, so subclasses can rely on the return code to know the result of the operation.
	virtual AKRESULT Open(
		const AkFileOpenData& in_FileOpen,		///< File open information (name, flags, etc)
		AkFileDesc*& out_pFileDesc              ///< File descriptor produced
	);

	// Stores all DirectStorage objects
	struct DStorage
	{
		void Reset()
		{
			status.Reset();
			queue.Reset();
			factory.Reset();
		}

		Microsoft::WRL::ComPtr<IDStorageFactoryX> factory;
		Microsoft::WRL::ComPtr<IDStorageQueueX> queue;
		Microsoft::WRL::ComPtr<IDStorageStatusArrayX> status;
	};

	// State for a batch.
	enum BatchState
	{
		BatchState_Free,      // The batch is ready to receive more read requests. 
		                      // The I/O thread will transition it to Enqueued when it is full.

		BatchState_Enqueued,  // The batch is full, and ready to be submitted. 
		                      // The I/O thread will transition it to Submitted when DStorage queue is submitted.

		BatchState_Submitted  // The batch has been submitted and its requests are in-flight. 
		                      // The DStorage callback thread will transition it back to Free when all requests are finished.
	};

	// Represents a batch of requests sent together with one
	// submission on the IDStorageQueueX.
	struct Batch
	{
		AkUInt32 uNumTransferInfo;
		AkAsyncIOTransferInfo** pTransferInfo;
		AkAtomic32 state; // See BatchState enum

		void SpinWaitForState(AkUInt32 newState);
		bool IsInState(AkUInt32 uState);
	};

	//
	// Win32 API Read Implementation
	//-----------------------------------------------------------------------------

	// Reads data from a file (asynchronous) using Win32 API
	// FileIOCompletionRoutine is called when read operation is complete.
	void Win32Read(
		AkFileDesc& in_fileDesc,						///< List of transfer items to process
		AkAsyncIOTransferInfo& in_transferInfo 			///< Information on the transfer, will be passed back to the callback
		);

	// Local callback for overlapped I/O.
	static VOID CALLBACK FileIOCompletionRoutine(
		DWORD dwErrorCode,
		DWORD dwNumberOfBytesTransfered,
		LPOVERLAPPED lpOverlapped
		);

	//
	// DirectStorage API Read Implementation
	//-----------------------------------------------------------------------------

	// Opens a file for read using the DirectStorage API
	// Returns a file descriptor for a given file name (string).
	// This should be called from the I/O thread.
	AKRESULT DStorageOpen(
		const AkFileOpenData& in_FileOpen,
		AkXSXFileDesc * out_pFileDesc						///< Returned file descriptor.
		);

	// Reads data from a file (asynchronous) using DirectStorage API
	// Enqueues this read request to be submitted for the specified batch
	// The batch MUST be in the Free state and MUST have space available for more requests.
	// This should be called from the I/O thread.
	void DStorageRead(
		Batch * pBatch,                                 ///< Batch to enqueue read requests
		BatchIoTransferItem& in_pTransferItem           ///< List of transfer items to process
		);

	// Enqueues a status update for the current batch and proceeds to the next batch.
	// The enqueued batch can then be submitted by calling DStorageSubmitBatch
	// This allows us to enqueue multiple batches before submitting them all at once.
	// This should be called from the I/O thread.
	void DStorageEnqueueBatch(
		Batch * in_pBatch                       ///< Batch to enqueue
		);

	// Actually submits all batches that were enqueued.
	// Will wake-up the consumer thread to handle request completion and dispatch callbacks.
	// This should be called from the I/O thread.
	void DStorageSubmitEnqueuedBatches();

	// Blocks until the next batch is ready to receive new read requests, then returns it.
	// This should be called from the I/O thread.
	Batch* DStorageWaitForNextFreeBatch();

protected:
	DStorage m_storage;

	AkDeviceID m_deviceID = AK_INVALID_DEVICE_ID;
	bool m_bAsyncOpen = false;	// If true, opens files asynchronously when it can.

	AkAsyncIOTransferInfo** m_pTransferInfoData = nullptr; // block of data for all transfer info pointers for each batch
	AkUInt32 m_uNextBatchIdx = 0; // indicates which m_pBatches item to populate next for the next request batch to submit
	AkUInt32 m_uNumEnqueuedBatches = 0;
	Batch* m_pBatches = nullptr; // a ring-buffer of batch information of size s_MaxActiveBatches

	// Consumer thread and synchronization objects
	static AK_DECLARE_THREAD_ROUTINE(AkDefaultIOHookDeferredCallback);
	AkThread m_callbackThread = nullptr;
	volatile bool m_bStopThreads = false;
	AkSemaphore m_DStorageNumBatchesSubmittedSemaphore = nullptr;

#ifdef _DEBUG
	// Optional thread to collect errors
	static AK_DECLARE_THREAD_ROUTINE(AkDefaultIOHookDeferredErrorRoutine);
	AkThread m_errorThread = nullptr;
	AkEvent m_errorEvent = nullptr;
#endif
};

#endif //_AK_DEFAULT_IO_HOOK_DEFERRED_H_
