#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "AK/SoundEngine/Common/AkMemoryMgr.h"
#include "adapters/Adapter_WWISE/WwiseHooks/WWISE_Hooks.h"
#include "adapters/Adapter_WWISE/AudioSDK/tools.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_Helper.h"
#include "engine/actors/actor.h"
#include "core/StringID.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_AkGameObject.h"
#include "core/memory/memoryStatsManager.h"
#include "AK/Tools/Common/AkPlatformFuncs.h"

#ifdef ITF_DURANGO
#include <Apu.h>
#endif

//$4
// ***********************************************************************************************************************
//    MEMORY
// ***********************************************************************************************************************
//

namespace	ITF
{

    void* AllocAlignedHook(AkMemPoolId poolId, size_t in_size, AkUInt32 uAlignment)
    {
        void* memory = nullptr;
        memory = ITF::Memory::alignedMallocCategory(in_size, uAlignment, ITF::MemoryId::mId_AudioEngine);
        return memory;
    }
    void* ReallocHook(AkMemPoolId poolId, void* pAddress, size_t in_size)
    {
        void* memory = nullptr;
      //  memory = ITF::Memory::realloc(pAddress,in_size);
        if(pAddress)
            memory = ITF::Memory::alignedRealloc(pAddress, in_size, 32);
        else
            memory = ITF::Memory::alignedMallocCategory(in_size, 32, ITF::MemoryId::mId_AudioEngine);

        return memory;
    }
    void* ReallocAlignedHook(AkMemPoolId poolId, void* pAddress, size_t in_size, AkUInt32 uAlignment)
    {
        void* memory = nullptr;
        memory = ITF::Memory::alignedRealloc(pAddress, in_size, uAlignment);
        return memory;
    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void *AllocHook(AkMemPoolId poolId, size_t in_size)
	{
        return ITF::Memory::alignedMallocCategory(in_size, 32, ITF::MemoryId::mId_AudioEngine);
        //return ITF::Memory::mallocCategory(in_size, ITF::MemoryId::mId_AudioEngine);
		//return newAlloc(mId_AudioEngine, char)[(unsigned int)in_size];
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void FreeHook(AkMemPoolId poolId, void *in_ptr)
	{
        ITF::Memory::alignedFree(in_ptr);
        //ITF::Memory::free(in_ptr);
		//delete[]((char *)in_ptr);
	}

#if defined(ITF_WINDOWS) || defined(ITF_X360) || defined(ITF_XBOX_SERIES)

	//
	// ===================================================================================================================
	//    Note: VirtualAllocHook() may be used by I/O pools of the default implementation of the Stream Manager, to allow
	//    "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING - refer to the Windows SDK documentation for more details).
	//    This is NOT mandatory;
	//    you may implement it with a simple malloc().
	// ===================================================================================================================
	//
	void *VirtualAllocHook(void *in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_dwProtect)
	{
		return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void VirtualFreeHook(void *in_pMemAddress, size_t in_size, DWORD in_dwFreeType)
	{
		VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
	}
#endif


}

//$4-*****************************************************************************************************************

namespace	ITF
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Wwise::Hooks::logHook(AK::Monitor::ErrorCode _errorCode, const AkOSChar *_errorString, AK::Monitor::ErrorLevel _errorLevel, AkPlayingID _playingID, AkGameObjectID _gameObjID)
	{
#ifndef ITF_FINAL
        AkGameObjectFactory*  factory = Wwise::AkGameObjectFactory::s_getSingleton();
        if (!factory)
            return;


        char* errorstring;
        CONVERT_OSCHAR_TO_CHAR(_errorString, errorstring);


	char	actorName[128];

        if(_gameObjID == Wwise::AkGameObject::GLOBAL_GAME_OBJECT_ID)
        {
            AudioSDK::safeStringCopy(actorName, Wwise::AkGameObject::GLOBAL_GAME_OBJECT_NAME);
        }
        else
#ifdef ITF_DEBUG_STRINGID
        if(_gameObjID != AK_INVALID_GAME_OBJECT)
        {
            const Actor* actor = NULL;
            if (factory->isRegistered(_gameObjID))
            {
                ObjectRef objectRef = AkGameObject::s_getObjectRef(_gameObjID);
                AkActor* wwiseActor = static_cast<AkActor*>(GETOBJECT(objectRef));
                if (wwiseActor)
                    snprintf(actorName, sizeof(actorName), " - actor \"%s\" ", wwiseActor->getUserFriendly().cStr());
                else
                {

                    actor = static_cast<Actor*>(GETOBJECT(objectRef));
                    if (actor)
                        snprintf(actorName, sizeof(actorName), " - actor \"%s\" ", actor->getUserFriendly().cStr());
                    else
                        snprintf(actorName, sizeof(actorName),  "unknown");
                }

            }

        }
        else
        {
            snprintf(actorName, sizeof(actorName), "unknown");
        }
#else
        AudioSDK::safeStringCopy(actorName, " ");
#endif


		// translate AkOSChar -> char (AkOSChar could be char or wchar_t)
	//	char	ansiString[512];
	//	AudioSDK::safeStringCopy(ansiString, _errorString);


		if(_errorLevel == AK::Monitor::ErrorLevel_Error)
		{
            // AUDIO_WARNING("[WWISE ERROR] %s (\"%s\" code %d) - AkPlayingID %d", errorstring, Wwise::Helper::getAkErrorCodeString(_errorCode), _errorCode,  _playingID);
		}
		else
		{
			// AUDIO_LOG("%s [WWISE MESSAGE] %s (\"%s\" code %d) - AkPlayingID %d", actorName, errorstring, Wwise::Helper::getAkErrorCodeString(_errorCode), _errorCode,  _playingID);
		}
#endif
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Wwise::Hooks::assertHook(const char *in_pszExpression, const char *in_pszFileName, int in_lineNumber)
	{
#ifndef ITF_FINAL
		AUDIO_ERROR("[WWISE ASSERT] %s(%d): \"%s\"", in_pszFileName, in_lineNumber, in_pszExpression);
#endif
	}
}
#endif
