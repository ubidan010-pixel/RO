#include "precompiled_engine.h"

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#if defined(ITF_SUPPORT_ONLINETRACKING)

//#define NO_USING_QUAZAL_DIRECTIVE
#undef UNICODE
#undef _UNICODE
#include "../extern/RendezVous/RendezVous.h"
#include "../extern/RendezVous/Services/Tracking.h"

#if defined(ITF_WINDOWS) && !defined(_DEBUG) && _SECURE_SCL
// Check size of STL containers
#error "In Release/Retail Win32, OSDK compiled with _SECURE_SCL=0 to match this Framework"
#endif


// HOW TO TEST THIS?
//
// Check results:
// - Access the Rendez-Vous Management Console (RVMC) from the Sandbox Manager
// - Access the Tracking section
// - Play the game
// - Check the results (you may pause tag flushing in order to see the tags)
// 
// Dev: add breakpoints in:
// - OnlineConfigFetcher::requestOnlineConfig()
//   Means the network has been detected.
// - OnlineConfigFetcher::update() in all three cases of switch
//   Gives the result on the Online Config request
//   You may also break in OnlineConfigClient::SetHost() and JobPopulateConfig::WaitForOnlineConfigServiceResponse().
// - extern/RendezVous/Services/Tracking/src/TrackingClient/Client/Tracking.cpp in
//   Tracking::HandleStateInit() on call to  RendezVousLogin():
//	 Means the Online Config has been retrieved.
// - extern/RendezVous/Services/Tracking/src/TrackingClient/Client/TrackingClient.cpp in
//   TrackingClient::SendTags():
//   Means the stored tags are really been sent to the server.
//
// Operations to test:
// - Auto sign-in
//   - Xbox 360: sign in a Xbox Live enabled profile, then launch game
//   - PS3: sign-in a PSN account with auto-connect option, then launch game
// - Manual sign-in
//   - Xbox 360/PS3: make sure not signed in, launch game, sign-in
// - PC
//   - Start game
//   - Unplug network cable, start game, plug network cable

namespace ITF
{
	class OnlineConfigFetcher
	{
	public:
		OnlineConfigFetcher() : m_OnlineConfigClient(NULL), m_currentState(NotReadyForOCRequest), m_nextAttemptTimestamp(0) {}
		~OnlineConfigFetcher();
		void requestOnlineConfig(OnlineTrackingAdapter *adapter);
		void update(OnlineTrackingAdapter *adapter);
		enum State
		{
			NotReadyForOCRequest,
			CreatingOC,
			AwaitingNextOCRequest,
			RequestingOC,
			OCAvailable,
			NumStates
		};
	private:
		Quazal::OnlineConfigClient *m_OnlineConfigClient;
		CallContext m_callContext;
		State m_currentState;
		f64 m_nextAttemptTimestamp; // used in state AwaitingNextOCRequest
	};

	//----------------------------------------------------------------------------

#ifdef ONLINETRACKING_HOW_MUCH_RENDEZVOUS_MEMORY
	ITF_THREAD_CRITICAL_SECTION MemLogCS;
	std::map<void *,size_t> PtrMap;
	size_t AllocatedBytesForRendezVous = 0;
	size_t MaxAllocatedKBForRendezVous = 0;
	bool Deinitialized = false;
#endif

	void *TheAllocFunc(size_t stSize)
	{
        void *ptr = Memory::mallocCategory(stSize,MemoryId::mId_OnlineTracking);

#ifdef ONLINETRACKING_HOW_MUCH_RENDEZVOUS_MEMORY
		if (!Deinitialized)
		{
			Synchronize::enterCriticalSection(&MemLogCS);
			AllocatedBytesForRendezVous += stSize;
			PtrMap[ptr] = stSize;
			//LOG("RdvAlloc: (+%u b) %u KB", stSize, AllocatedBytesForRendezVous / 1024);
			size_t allocKB = AllocatedBytesForRendezVous / 1024;
			if (allocKB > MaxAllocatedKBForRendezVous)
			{
				MaxAllocatedKBForRendezVous = allocKB;
				LOG("RdvMaxAlloc: %u KB", MaxAllocatedKBForRendezVous);
			}
			Synchronize::leaveCriticalSection(&MemLogCS);
		}
#endif
		return ptr;
	}
	
	void TheFreeFunc(void* ptr)
	{
#ifdef ONLINETRACKING_HOW_MUCH_RENDEZVOUS_MEMORY
		// Deletion of RDV globals will still call the funcs!
		if (!Deinitialized)
		{
			Synchronize::enterCriticalSection(&MemLogCS);
			size_t stSize = PtrMap[ptr];
			AllocatedBytesForRendezVous -= stSize;
			//LOG("RdvAlloc: (-%u b) %u KB", stSize, AllocatedBytesForRendezVous / 1024);
			Synchronize::leaveCriticalSection(&MemLogCS);
		}
		else
			LOG("RdvFree");
#endif
		Memory::free(ptr);
	}

	OnlineTrackingAdapter::OnlineTrackingAdapter()
	{
		// Warning: Rendez-Vous keeps calling the functions
		// passed to SetBasicMemoryFunctions() until the very end,
		// even to delete its global objects (ex:
		// Quazal::FixedSizeMemoryPool::FixedSizeMemoryChunk::Delete()
		// freeing Quazal::PacketOut::s_PacketOutPool)
#ifdef ONLINETRACKING_HOW_MUCH_RENDEZVOUS_MEMORY
		Synchronize::createCriticalSection(&MemLogCS);
#endif
		Quazal::MemoryManager::SetBasicMemoryFunctions(TheAllocFunc, TheFreeFunc);

		m_OnlineConfigFetcher = newAlloc(mId_System, OnlineConfigFetcher());
	}

	OnlineTrackingAdapter::~OnlineTrackingAdapter()
	{
		delete m_OnlineConfigFetcher;
		Quazal::MemoryManager::SetBasicMemoryFunctions(NULL, NULL); // ignored for deletion of previous allocations
#ifdef ONLINETRACKING_HOW_MUCH_RENDEZVOUS_MEMORY
		Deinitialized = true;
		Synchronize::destroyCriticalSection(&MemLogCS);
#endif
	}

	void OnlineTrackingAdapter::init()
	{
		//Quazal::TraceLog::GetInstance()->SetFlag(TRACE_ALL);	    
		//Quazal::EventLog::GetInstance()->SetLogLevel(EventLog::Debug);
	
#ifdef ITF_SUPPORT_NETWORKSERVICES
		// Try connecting (silently) if network is available
		if (NETWORKSERVICES)
		{
			if (NETWORKSERVICES->isUserSignedIn(0))
				onSignIn(0); // not called from game manager if already signed in when game starts 
		}
#endif // ITF_SUPPORT_NETWORKSERVICES

		Quazal::Tracking::Create(getSandboxAccessKey());
	}

	void OnlineTrackingAdapter::onSignIn(u32 dwController)
	{
		m_OnlineConfigFetcher->requestOnlineConfig(this);
	}

	void OnlineTrackingAdapter::update()
	{
		m_OnlineConfigFetcher->update(this);
	}

	void OnlineTrackingAdapter::term()
	{
		Quazal::Tracking::Destroy();
	}

	void OnlineTrackingAdapter::sendTag(const char *tag, const char *attributes, int controllerIndex)
	{
		// Compile-time check of default value
		ITF_COMPILE_TIME_ASSERT(RendezVous_changed, CONTROLLER_INDEX_NOT_APPLICABLE == -1);
		//LOG("sendTag %s",tag);
		Quazal::Tracking::GetInstance()->SendTag(tag, attributes, controllerIndex);
	}

	//----------------------------------------------------------------------------

	void OnlineConfigFetcher::requestOnlineConfig(OnlineTrackingAdapter *adapter)
	{
		switch (m_currentState)
		{
		case RequestingOC:
		case OCAvailable:
			return;
		default:;
		}

		if (!m_OnlineConfigClient)
		{
			m_currentState = CreatingOC;
			ITF_ASSERT(Quazal::Core::IsUsingThreads()); // otherwise we would have to resolve the online config host in a separate thread to avoid blocking during DNS lookup
			m_OnlineConfigClient = adapter->tryCreatingOnlineConfigClient();
			if (!m_OnlineConfigClient)
				return; // retry in next update()
			m_currentState = AwaitingNextOCRequest;
			m_nextAttemptTimestamp = 0; // asap
		}

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL) && defined(USE_UAT_SANDBOX)
		m_OnlineConfigClient->SetOnlineConfigServiceHost("uat-onlineconfigservice.ubi.com");
#else
		// (Consoles) The host is deduced from the environment.
		// (Win32) In debug target, the host is taken from onlineconfighost.txt
		//char *cwd = _getcwd(0, 0); // if you want to check where the file is read (#include <direct.h>)
		// The release/final versions use the retail lib => onlineconfigservice.ubi.com
#endif

		m_callContext.Reset();

		// NB: (Win32) This can crash randomly in Curl_ssl_init() if osdk_libcurl.lib is compiled with
		// USE_RAND_SCREEN enabled, because that uses a function that must be used at startup only.
		// See http://groups.google.com/group/mailing.openssl.dev/browse_thread/thread/2b4de9c67edaa468/3f64759a908beca3?lnk=raot
		bool res = m_OnlineConfigClient->FetchConfig(m_callContext);
		ITF_ASSERT(m_callContext.GetState() == CallContext::CallInProgress || !res);
		if (res)
			m_currentState = RequestingOC;
	}

	void OnlineConfigFetcher::update(OnlineTrackingAdapter *adapter)
	{
		switch (m_currentState)
		{
		case CreatingOC:
			requestOnlineConfig(adapter);
			break;
		case RequestingOC:
			switch (m_callContext.GetState())
			{
			case CallContext::CallSuccess:
				m_currentState = OCAvailable;
				break;
			case CallContext::CallFailure:
				LOG("[OnlineTracking] Offline (OnlineConfig not found from %s)", m_OnlineConfigClient->GetOnlineConfigServiceHost().CStr());
				m_currentState = AwaitingNextOCRequest; // retry later
				m_nextAttemptTimestamp = SYSTEM_ADAPTER->getTime() + 60.f; // in 1 minute
				//m_callContext.GetOutcome().Trace(0);
				break;
			case CallContext::CallCancelled:
				ITF_ERROR("Unexpected OC state");
				m_currentState = AwaitingNextOCRequest;
				m_nextAttemptTimestamp = ~0;
				break;
			default:; // wait
			}
			break;
		case AwaitingNextOCRequest:
			// Retry when time elapsed
			if (SYSTEM_ADAPTER->getTime() >= m_nextAttemptTimestamp)
				requestOnlineConfig(adapter);
			break;
		default:;
		}
	}

	OnlineConfigFetcher::~OnlineConfigFetcher()
	{
		if (m_OnlineConfigClient)
			delete m_OnlineConfigClient;
	}
}

#else

namespace ITF
{
	OnlineTrackingAdapter::OnlineTrackingAdapter() {}
	OnlineTrackingAdapter::~OnlineTrackingAdapter() {}
	void OnlineTrackingAdapter::init() {}
	void OnlineTrackingAdapter::term() {}
	void OnlineTrackingAdapter::sendTag(const char *tag, const char *attributes, int controllerIndex) {}
	void OnlineTrackingAdapter::update() {}
	void OnlineTrackingAdapter::onSignIn(u32 dwController) {}
}

#endif //ITF_SUPPORT_ONLINETRACKING
