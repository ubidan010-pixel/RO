#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Helper/WWISE_AkGameObject.h"
#include "core/file/FileServer.h"
#include "core/types.h"
#include "engine/actors/actor.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"


namespace	ITF
{
	namespace	Wwise
{
	AkGameObjectFactory *AkGameObjectFactory::	ms_singleton = NULL;

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	void AkGameObjectFactory::s_createSingleton()
	{
		AUDIO_ASSERT(ms_singleton == NULL, "AkGameObjectFactory::s_createSingleton was not null");
		ms_singleton = WWISE_NEW(AkGameObjectFactory);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void AkGameObjectFactory::s_destroySingleton()
	{
		AUDIO_ASSERT(ms_singleton, "AkGameObjectFactory::s_createSingleton was null");

		for(AkGameObjectMap::iterator it = ms_singleton->m_akGameObjectMap.begin(), itEnd = ms_singleton->m_akGameObjectMap.end(); it != itEnd; ++it) WWISE_DELETE(it->second);

		WWISE_DELETE(ms_singleton);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AkGameObjectID AkGameObjectFactory::create(const ObjectRef &_objectRef)
	{
		const AkGameObjectID		akGameObjectID = AkGameObject::s_getAkGameObjectID(_objectRef);
		AkGameObjectMap::iterator	it = m_akGameObjectMap.find(akGameObjectID);

		if(it == m_akGameObjectMap.end())
		{
			AkGameObject	*newGao = WWISE_NEW(AkGameObject) (_objectRef);
			bool			isAlive, positionChanged;
			newGao->updatePosition(&isAlive, &positionChanged);
			m_akGameObjectMap[akGameObjectID] = newGao;
		}

		return akGameObjectID;
	}

    ObjectRef AkGameObjectFactory::createAkActor(const char *_name)
    {
        AkActor* pActor = newAlloc(mId_AudioEngine, AkActor(_name));
        //AkGameObjectID gameObjectID = Wwise::AkGameObject::s_getAkGameObjectID(pActor->getRef());
        return pActor->getRef();
    }




    bool AkGameObjectFactory::isRegistered(const AkGameObjectID _akGameObjectID)
    {
        AkGameObjectMap::iterator	it = m_akGameObjectMap.find(_akGameObjectID);

        if (it != m_akGameObjectMap.end())
            return true;

        return false;

    }

    //
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void AkGameObjectFactory::destroy(AkGameObjectID _akGameObjectID)
	{
		AkGameObjectMap::iterator	it = m_akGameObjectMap.find(_akGameObjectID);

		if(it != m_akGameObjectMap.end())
		{
			AkGameObject	*oldGao = it->second;
                        WWISE_DELETE(oldGao);
			m_akGameObjectMap.erase(it);

		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AkGameObject *AkGameObjectFactory::getAkGameObject(const ObjectRef &_objectRef)
	{
		const AkGameObjectID	akGameObjectID = AkGameObject::s_getAkGameObjectID(_objectRef);
		return getAkGameObject(akGameObjectID);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AkGameObject *AkGameObjectFactory::getAkGameObject(AkGameObjectID _akGameObjectID)
	{
		AkGameObjectMap::iterator	it = m_akGameObjectMap.find(_akGameObjectID);

		if(it != m_akGameObjectMap.end()) return it->second;

		return NULL;
	}

	const char *AkGameObject::	GLOBAL_GAME_OBJECT_NAME = "GLOBAL_GAME_OBJECT_NAME";

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	ITF::ObjectRef AkGameObject::s_getObjectRef(AkGameObjectID _akGameObjectID)
	{
		if(_akGameObjectID == GLOBAL_GAME_OBJECT_ID) return ITF_INVALID_OBJREF;
		if(_akGameObjectID == AK_INVALID_GAME_OBJECT) return ITF_INVALID_OBJREF;
        if (_akGameObjectID == 0) return ITF_INVALID_OBJREF;


  

		return ObjectRef(static_cast<ObjectRef>(0xFFFFFFFF &  _akGameObjectID));
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AkGameObjectID AkGameObject::s_getAkGameObjectID(const ObjectRef &_objectRef)
	{
		if(_objectRef.isValid())
            return AkGameObjectID(_objectRef.getValue());

		return GLOBAL_GAME_OBJECT_ID;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AkGameObject::AkGameObject(const ObjectRef &_objectRef) :
		m_position(),
		m_angle(-1.f),
		m_objectRef(_objectRef),
		m_isMultiPosition(false)
	{
		const AkGameObjectID	akGameObjectID = s_getAkGameObjectID(m_objectRef);
		AK_SoundEngine_RegisterGameObj(akGameObjectID, getName());
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AkGameObject::~AkGameObject()
	{

        AkActor* pactor = static_cast<AkActor*>(GETOBJECT(m_objectRef));
        if(pactor)
            SF_DEL(pactor)

		const AkGameObjectID	akGameObjectID = s_getAkGameObjectID(m_objectRef);
		AK_SoundEngine_UnregisterGameObj(akGameObjectID);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void AkGameObject::updatePosition(bool *_isAlive, bool *_positionChanged)
	{
		if(m_objectRef.isValid())
		{
            AkActor* wwiseActor = static_cast<AkActor*>(GETOBJECT(m_objectRef));
            if (wwiseActor)
            {
                *_positionChanged = false;
                *_isAlive = true;
                return;

            }
            
			Actor	*actor = static_cast<Actor *>(GETOBJECT(m_objectRef));
			if(actor && !actor->isDestructionRequested())
			{
				*_isAlive = true;

				if(m_isMultiPosition)
				{
					//
					// when multiposition, the sound emitter position is computed by wwise, we dont
					// know the result, so we consider it never change
					//
					*_positionChanged = false;
				}
				else
				{
					// position
					Vec3d actorPosition = actor->getPos();
 //temp desactivate TODO: get it from soundcomponent
// 					Vec3d soundPosition;
// 					soundPosition.clear();
// 					// get fxcontrollercomponent offset...
// 					FXControllerComponent * fxController = actor->GetComponent<FXControllerComponent>();
// 					if(fxController)
// 					{
// 						soundPosition =  fxController->getSoundPosition();
// 
// 					}
// 					if(!soundPosition.isNullEpsilon())
// 					{
// 						actorPosition = soundPosition;
// 					}
// 
					const f32	angle = actor->getAngle();


					if(isEqualWithEpsilon(m_position, actorPosition, 0.0001f) && isEqualWithEpsilon(m_angle, angle, 0.0001f))
					{
						*_positionChanged = false;
					}
					else
					{
						m_position = actorPosition;
						m_angle = angle;
						*_positionChanged = true;

						AkSoundPosition soundPosition;
						soundPosition.SetPosition(actorPosition.getX(), actorPosition.getY(), actorPosition.getZ());

						// orientation
						soundPosition.SetOrientation( f32_Cos(angle),f32_Sin(angle), 0.f, 0,0,1);

						const AkGameObjectID	akGameObjectID = s_getAkGameObjectID(m_objectRef);
						AK_SoundEngine_SetPosition(akGameObjectID, soundPosition);
					}
				}
			}
			else
			{
				*_isAlive = false;
				*_positionChanged = false;
			}
		}
		else
		{
			*_isAlive = true;
			if(m_angle < 0.f)
			{
				m_angle = 0.f;
				*_positionChanged = true;

				AkSoundPosition soundPosition;

				// position
				soundPosition.SetPosition( 0.f, 0.f, 0.f);

				// orientation
                soundPosition.SetOrientation(1.f, 0.f, 0.f, 0, 0 ,1);

				AK_SoundEngine_SetPosition(GLOBAL_GAME_OBJECT_ID, soundPosition);
			}
			else
			{
				*_positionChanged = false;
			}
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const char *AkGameObject::getName() const
	{
		if(m_objectRef.isValid())
		{
#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE) 
            if(m_objectRef.getValue() == u32(ListenerID::eDefaultListener))
                return "Listener_Default";

            if (m_objectRef.getValue() == u32(ListenerID::eCenterSpeakerListener))
                return "Listener_Center_Speaker";

			
            AkActor* actor = static_cast<AkActor*>(GETOBJECT(m_objectRef));
            if (actor)
            {
                return actor->getUserFriendly().cStr();
            }
            else
            {
                Actor* uafactor = static_cast<Actor*>(GETOBJECT(m_objectRef));
                if(uafactor)
                    return uafactor->getUserFriendly().cStr();
                else
                    return "null actor *";

            }
				
#else
			return " ";
#endif
		}
		else
		{
			return GLOBAL_GAME_OBJECT_NAME;
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void AkGameObject::setPosition(const ITF_VECTOR<Vec3d> &_positions, AudioMultiPositionMode _mode)
	{
		const AkGameObjectID	akGameObjectID = s_getAkGameObjectID(m_objectRef);
		Actor					*actor = static_cast<Actor *>(GETOBJECT(m_objectRef));
		const f32				angle = actor ? actor->getAngle() : 0.f;
		const u32				count = _positions.size();
		AkSoundPosition			*soundPosition = WWISE_NEW(AkSoundPosition[count]);

		for(u32 i = 0; i < count; ++i)
		{
			// position
			soundPosition[i].SetPosition( _positions[i].getX(), _positions[i].getY(), _positions[i].getZ());

			// orientation
			soundPosition[i].SetOrientation( f32_Cos(angle), f32_Sin(angle),0.f,0,1,0 );
		}

		switch(_mode)
		{
		case AUDIO_MULTIPOSITION_MODE_SINGLE_SOURCE:	AK_SoundEngine_SetMultiplePositions(akGameObjectID, soundPosition, AkUInt16(count), AK::SoundEngine::MultiPositionType_SingleSource); break;
		case AUDIO_MULTIPOSITION_MODE_MULTI_SOURCES:	AK_SoundEngine_SetMultiplePositions(akGameObjectID, soundPosition, AkUInt16(count), AK::SoundEngine::MultiPositionType_MultiSources); break;
		case AUDIO_MULTIPOSITION_MODE_MULTI_DIRECTIONS: AK_SoundEngine_SetMultiplePositions(akGameObjectID, soundPosition, AkUInt16(count), AK::SoundEngine::MultiPositionType_MultiDirections); break;
		default:										break;
		}

		m_isMultiPosition = true;
	}
}	// namespace ITF{

AkActor::AkActor(const char* _name)
{
#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE)
    String8 newFriendly8(_name);
    setUserFriendly(newFriendly8);
#endif
}


}		// namespace Wwise{
#endif // ITF_SUPPORT_WWISE
