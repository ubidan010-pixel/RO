#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "AK/SoundEngine/Common/AkTypes.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"
//#include "adapters/AudioSerializedData/Wwise/WwiseSoundConfigTemplate.h"
//#include "adapters/AudioSerializedData/Wwise/WwiseSoundComponent.h"
#include "engine/actors/actor.h"
#include "adapters/Adapter_WWISE/Configure/WWISE_Configure.h"

namespace	ITF
{

		//
	// ===================================================================================================================
	// ===================================================================================================================
	//
    static void s_bankCallback(AkUInt32 in_bankID, const void* in_pInMemoryBankPtr, AKRESULT in_eLoadResult,  void* in_pCookie)
	{
		Adapter_WWISE		*thisPointer = static_cast<Adapter_WWISE *>(in_pCookie);

		if(thisPointer)
		{
#if !defined(ITF_FINAL) 
			if(in_eLoadResult!=AK_Success)
				thisPointer->addFailedBank(in_bankID);
#endif
			thisPointer->updateLoadingBank(in_bankID,(u32)in_eLoadResult);
		}

	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::updateLoadingBank(u32 _bankID,u32 _LoadResult)
	{
		// TODO update state of all async loaded bank
		if(m_globalBankId == _bankID )
			setGlobalBankLoaded(btrue);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::loadGlobalBank()
	{

		if(isRunning()==false)
			return;
#ifdef ITF_PROJECT_WW1
		char	bankName[256];
		AudioSDK::safeStringCopy(bankName, Wwise::Configure::c_globalBankName);

		char	*pz = strrchr(bankName, '.');
		if(pz) *pz = 0;

		BankLoader	bankLoader;
		AkBankID		bankID = 0; 
		bankLoader.type = BankLoader::GLOBAL;
		bankLoader.ref.soundConfigTemplate= m_config;

		m_loadedBankMutex.lock();
		BankLoaderList	&bankLoaderList = m_loadedBank[bankName];
		const bool		callLoadFunction = bankLoaderList.empty();
		if(callLoadFunction)
		{
			AkBankCallbackFunc	callback = &s_bankCallback;
			const AKRESULT	eResult = AK_SoundEngine_LoadBank(bankName, callback, this, AK_DEFAULT_POOL_ID, bankID);
			if(eResult != AK_Success)
			{
				AUDIO_WARNING("Loading WWISE bank \"%s\" has failed.", bankName);
#if !defined(ITF_FINAL) 
				m_failedBank.push_back(bankName);
#endif
				m_loadedBankMutex.unlock();
				return;
			}
		}
		else
			bankID = bankLoaderList.begin()->BankId;

		bankLoader.BankId = bankID;
		bankLoaderList.push_back(bankLoader);
		m_globalBankId = bankID;
		m_loadedBankMutex.unlock();
#endif

	}
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bbool	Adapter_WWISE::isGlobalBankLoaded()
	{

		if(isRunning()==false)
			return btrue;

		return m_globalBankIsLoaded;
	}
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::unloadGlobalBank()
	{
		if(isRunning()==false)
			return;
#ifdef ITF_PROJECT_WW1
		char	bankName[256];
		AudioSDK::safeStringCopy(bankName, Wwise::Configure::c_globalBankName);

		char	*pz = strrchr(bankName, '.');
		if(pz) *pz = 0;

		bool	callUnloadFunction = false;

		m_loadedBankMutex.lock();

		LoadedBankMap::iterator it = m_loadedBank.find(bankName);

		if(it != m_loadedBank.end())
		{
			BankLoaderList	&bankLoaderList = it->second;

			callUnloadFunction = bankLoaderList.empty();

			for(BankLoaderList::iterator itLoader = bankLoaderList.begin(), itLoaderEnd = bankLoaderList.end(); itLoader != itLoaderEnd;)
			{
				if(itLoader->ref.soundConfigTemplate == m_config)
				{
					itLoader = bankLoaderList.erase(itLoader);
					itLoaderEnd = bankLoaderList.end();
				}
				else
				{
					++itLoader;
				}
			}

			// no more user ?
			callUnloadFunction = bankLoaderList.empty() && !callUnloadFunction;
		}
#if !defined(ITF_FINAL) 
		else
		{
			i32 index = m_failedBank.find(bankName);
			if(index>=0)
				m_failedBank.removeAt(index);
		}
#endif
		m_loadedBankMutex.unlock();

		if(callUnloadFunction)
		{
			//m_delayedUnloadBank.push_back(bankName);
			AK_SoundEngine_UnloadBank(bankName, NULL);
			m_loadedBank.erase(bankName);
		}
#endif
	}
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::loadBank(const char *_path, const SoundComponent_Template *_tpl)
	{

		if(isRunning()==false)
			return;

		AUDIO_ASSERT(_path, "bank path is null");
		AUDIO_ASSERT(_tpl, "tpl is null");

		char	bankName[256];
        AudioSDK::safeStringCopy(bankName, 256, _path);

		char	*pz = strrchr(bankName, '.');
		if(pz) *pz = 0;

		BankLoader	bankLoader;
 		AkBankID		bankID = 0; 
 		bankLoader.BankType = BankLoader::SOUND_COMPONENT;


		m_loadedBankMutex.lock();
		BankLoaderList	&bankLoaderList = m_loadedBank[bankName];
		const bool		callLoadFunction = bankLoaderList.empty();
		if(callLoadFunction)
		{

			AkBankCallbackFunc	callback = &s_bankCallback;
			const AKRESULT	eResult = AK_SoundEngine_LoadBank(bankName, callback, this,  bankID);
			if(eResult != AK_Success)
			{
				AUDIO_WARNING("Loading WWISE bank \"%s\" has failed.", bankName);
#if !defined(ITF_FINAL) 
				m_failedBank.push_back(bankName);
#endif
				m_loadedBankMutex.unlock();
				return;
			}
		}
		else
			bankID = bankLoaderList.begin()->BankId;

		bankLoader.BankId = bankID;
		bankLoaderList.push_back(bankLoader);
		m_loadedBankMutex.unlock();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::unloadBank(const char *_path, const SoundComponent_Template *_tpl)
	{
		if(isRunning()==false)
			return;
		AUDIO_ASSERT(_path, "bank path is null");
		AUDIO_ASSERT(_tpl, "tpl is null");

        char	bankName[256];
        AudioSDK::safeStringCopy(bankName, 256, _path);

		char	*pz = strrchr(bankName, '.');
		if(pz) *pz = 0;

		bool	callUnloadFunction = false;

		m_loadedBankMutex.lock();

		LoadedBankMap::iterator it = m_loadedBank.find(bankName);

		if(it != m_loadedBank.end())
		{
			BankLoaderList	&bankLoaderList = it->second;

			callUnloadFunction = bankLoaderList.empty();

			for(BankLoaderList::iterator itLoader = bankLoaderList.begin(), itLoaderEnd = bankLoaderList.end(); itLoader != itLoaderEnd;)
			{
				if(itLoader->ref.soundComponentTemplate == _tpl)
				{
					itLoader = bankLoaderList.erase(itLoader);
					itLoaderEnd = bankLoaderList.end();
				}
				else
				{
					++itLoader;
				}
			}

			// no more user ?
			callUnloadFunction = bankLoaderList.empty() && !callUnloadFunction;
		}
#if !defined(ITF_FINAL) 
		else
		{
			i32 index = m_failedBank.find(bankName); 
			if(index>=0)
				m_failedBank.eraseKeepOrder(index);
		}
#endif
		m_loadedBankMutex.unlock();

		if(callUnloadFunction)
		{
            //m_delayedUnloadBank.push_back(bankName);
			AK_SoundEngine_UnloadBank(bankName, NULL);
			m_loadedBank.erase(bankName);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::loadBank(const char *_path, const SoundConfig_Template *_tpl)
	{

		if(isRunning()==false)
			return;

        char	bankName[256];
        AudioSDK::safeStringCopy(bankName, 256, _path);

		char	*pz = strrchr(bankName, '.');
		if(pz) *pz = 0;

		BankLoader	bankLoader;
		bankLoader.BankType = BankLoader::SOUND_CONFIG;
		bankLoader.ref.soundConfigTemplate = m_config;

		m_loadedBankMutex.lock();

		BankLoaderList	&bankLoaderList = m_loadedBank[bankName];
		const bool		callLoadFunction = bankLoaderList.empty();
		bankLoaderList.push_back(bankLoader);
		m_loadedBankMutex.unlock();

		if(callLoadFunction)
		{
			AkBankID		bankID ; // Not used here.

			// synchronous
			const AKRESULT	eResult = AK_SoundEngine_LoadBank(bankName /*_path*/, bankID);

			if(eResult != AK_Success)
			{
				AUDIO_WARNING("Loading WWISE bank \"%s\" has failed.", bankName);
#if !defined(ITF_FINAL) 
				m_failedBank.push_back(bankName);
				m_loadedBank.erase(bankName);
#endif
			}
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::unloadBank(const char *_path, const SoundConfig_Template *_tpl)
	{
		if(isRunning()==false)
			return;

        char	bankName[256];
        AudioSDK::safeStringCopy(bankName, 256, _path);

		char	*pz = strrchr(bankName, '.');
		if(pz) *pz = 0;

		bool	callUnloadFunction = false;

		m_loadedBankMutex.lock();

		LoadedBankMap::iterator it = m_loadedBank.find(bankName);
		if(it != m_loadedBank.end())
		{
			BankLoaderList	&bankLoaderList = it->second;
			callUnloadFunction = bankLoaderList.empty();

			for(BankLoaderList::iterator itLoader = bankLoaderList.begin(), itLoaderEnd = bankLoaderList.end(); itLoader != itLoaderEnd;)
			{
				if(itLoader->ref.soundConfigTemplate == _tpl)
				{
					itLoader = bankLoaderList.erase(itLoader);
					itLoaderEnd = bankLoaderList.end();
				}
				else
				{
					++itLoader;
				}
			}

			// no more user ?
			callUnloadFunction = bankLoaderList.empty() && !callUnloadFunction;
		}
#if !defined(ITF_FINAL) 
		else
		{
			i32 index = m_failedBank.find(bankName);
			if(index>=0)
				m_failedBank.eraseKeepOrder(index);
		}
#endif
		m_loadedBankMutex.unlock();

		if(callUnloadFunction)
		{
            //m_delayedUnloadBank.push_back(bankName);
			AK_SoundEngine_UnloadBank(bankName, NULL);
			m_loadedBank.erase(bankName);
		}
	}

//
// ===================================================================================================================
// ===================================================================================================================
//

    AKRESULT Adapter_WWISE::AK_SoundEngine_PrepareBank(const char* in_pszString)
    {
        AK::SoundEngine::PreparationType	in_PreparationType = AK::SoundEngine::Preparation_Load;// AK::SoundEngine::PreparationType::Preparation_Load;
        const AKRESULT	res = AK::SoundEngine::PrepareBank(in_PreparationType, in_pszString, nullptr, nullptr, AK::SoundEngine::AkBankContent_All);

        return res;
    }

    AKRESULT Adapter_WWISE::AK_SoundEngine_UnPrepareBank(const char* in_pszString)
    {
        AK::SoundEngine::PreparationType	in_PreparationType = AK::SoundEngine::PreparationType::Preparation_Unload;
        const AKRESULT	res = AK::SoundEngine::PrepareBank(in_PreparationType, in_pszString, AK::SoundEngine::AkBankContent::AkBankContent_All);

        return res;
    }

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    void Adapter_WWISE::prepareBank(const char* _path)
    {
        if (isRunning() == false)
            return;

        const AKRESULT	eResult = AK_SoundEngine_PrepareBank(_path);
        if (eResult != AK_Success)
            AUDIO_WARNING("Preparing WWISE bank \"%s\" has failed.", _path);
    }

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    void Adapter_WWISE::unprepareBank(const char* _path)
    {
        if (isRunning() == false)
            return;

        AK_SoundEngine_UnPrepareBank(_path);
    }




#ifndef ITF_FINAL

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::unloadAllBanks()
	{
		PathChar_t	pathChar[PATH_C_BUFFERSIZE];

		m_loadedBankMutex.lock();
		for(LoadedBankMap::const_iterator it = m_loadedBank.begin(), itEnd = m_loadedBank.end(); it != itEnd; ++it)
		{
// 			const Path	&path = it->first;
// 			path.toBuffer(pathChar);

			const BankLoaderList	&bankLoaderList = it->second;

			for(BankLoaderList::const_iterator itLoader = bankLoaderList.cbegin(), itLoaderEnd = bankLoaderList.cend(); itLoader != itLoaderEnd; ++itLoader)
			{
				//if(itLoader->type != BankLoader::SOUND_CONFIG)
				{
					//m_delayedUnloadBank.push_back(pathChar);
                    AK_SoundEngine_UnloadBank(pathChar, NULL);
					break;	// call only one time
				}
			}
		}

		m_loadedBankMutex.unlock();
		unloadGlobalBank(); //unload bootstrap.bnk
		// reload config bank
		//m_config->unloadConfigBank();
		m_failedBank.clear();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::reloadAllBanks()
	{
		AkBankID	bankID;			// Not used here
		PathChar_t	pathChar[PATH_C_BUFFERSIZE];

		//m_config->loadConfigBank(); // as config bank order is important, call m_config function is safe

		loadGlobalBank(); //load bootstrap.bnk

		m_loadedBankMutex.lock();
		for(LoadedBankMap::const_iterator it = m_loadedBank.begin(), itEnd = m_loadedBank.end(); it != itEnd; ++it)
		{
// 			const Path	&path = it->first;
// 			path.toBuffer(pathChar);

			const BankLoaderList	&bankLoaderList = it->second;

			for(BankLoaderList::const_iterator itLoader = bankLoaderList.cbegin(), itLoaderEnd = bankLoaderList.cend(); itLoader != itLoaderEnd; ++itLoader)
			{
				//if(itLoader->type != BankLoader::SOUND_CONFIG)
				{
					AK_SoundEngine_LoadBank(pathChar,  bankID);
					break;			// call only one time
				}
			}
		}

		m_loadedBankMutex.unlock();
		m_failedBank.clear();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::traceBankLeaks()
	{
        /*
		m_loadedBankMutex.lock();
		for(LoadedBankMap::const_iterator it = m_loadedBank.begin(), itEnd = m_loadedBank.end(); it != itEnd; ++it)
		{
			PathChar_t				pathChar[PATH_C_BUFFERSIZE];
			const Path				&path = it->first;
			const BankLoaderList	&bankLoaderList = it->second;
			//path.toBuffer(pathChar);

			for(BankLoaderList::const_iterator itLoader = bankLoaderList.cbegin(), itLoaderEnd = bankLoaderList.cend(); itLoader != itLoaderEnd; ++itLoader)
			{
				switch(itLoader->type)
				{
				case BankLoader::SOUND_CONFIG:	// consider this is normal
					break;

				case BankLoader::GLOBAL:	// consider this is normal
					break;

				case BankLoader::RESOURCE:	// TO DO
					break;

				case BankLoader::SOUND_COMPONENT:
					{
						const Path	&tplPath = itLoader->ref.soundComponentTemplate->getActorTemplate()->getFile();
						PathChar_t	tplName[PATH_C_BUFFERSIZE];
					//	tplPath.toBuffer(tplName);
						AUDIO_WARNING("sound bank leak : \"%s\", from sound component template @0x%x", pathChar, tplName);
					}
					break;

				case BankLoader::EVENT:
					{
#ifdef ITF_DEBUG_STRINGID
						ObjectRef	objectRef(itLoader->ref.eventSenderValue);
						const Actor *actor = static_cast<Actor *>(GETOBJECT(objectRef));
						if(actor)
						{
							AUDIO_WARNING("sound bank leak : \"%s\", from event send by actor \"%s\"", pathChar, actor->getUserFriendly().cStr());
						}
						else
#endif
							AUDIO_WARNING("sound bank leak : \"%s\", from event send by actor \"ref %d\"", pathChar, itLoader->ref.eventSenderValue);
					}
					break;

				default:
					AUDIO_WARNING("sound bank leak : \"%s\", from unknown type", pathChar);
					break;
				}
			}
		}

		m_loadedBankMutex.unlock();*/
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	void Adapter_WWISE::addFailedBank(u32 _bankId)
	{
		if(std::find(m_failedBankId.begin(), m_failedBankId.end(), _bankId) != m_failedBankId.end())
			m_failedBankId.push_back(_bankId);
	}
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	void Adapter_WWISE::updateFailedBank()
	{
		u32 bankId = 0;
		
		for(u32 i=0; i<m_failedBankId.size(); ++i)
		{
			bankId= m_failedBankId[i];
			m_loadedBankMutex.lock();
			for(LoadedBankMap::const_iterator it = m_loadedBank.begin(), itEnd = m_loadedBank.end(); it != itEnd; ++it)
			{
				//PathChar_t				pathChar[PATH_C_BUFFERSIZE];
				const Path				&path = it->first;
				const BankLoaderList	&bankLoaderList = it->second;
				//path.toBuffer(pathChar);

				for(BankLoaderList::const_iterator itLoader = bankLoaderList.cbegin(), itLoaderEnd = bankLoaderList.cend(); itLoader != itLoaderEnd; ++itLoader)
				{
					if(itLoader->BankId ==bankId)
					{
						//if(m_failedBank.find(path.getBasename())==-1) //todoisap
							m_failedBank.push_back(path.getBasename());
						break;
					}
				}
			}
			m_loadedBankMutex.unlock();
		}

		for(u32 i=0; i<m_failedBank.size(); ++i)
		{
			m_loadedBank.erase(m_failedBank[i]);
		}

		m_failedBankId.clear();
		
	}

#endif
}
#endif // ITF_SUPPORT_WWISE
