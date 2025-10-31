#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "engine/display/View.h"
#include "engine/display/Camera.h"
#include "engine/sound/SoundConfigTemplate.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE_MetronomeState.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE_Listener.h"
#include "adapters/AudioSerializedData/Wwise/WwiseEvents.h"
#include "engine/singleton/Singletons.h"
#include "engine/events/EventManager.h"
#include "AK/SoundEngine/Common/AkQueryParameters.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_Helper.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_AkGameObject.h"

#include "engine/sound/InstanceManager.h"
#include "engine/sound/routing/SoftwareBus.h"
#include "engine/gameplay/musicmanager.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace	ITF
{

    void Adapter_WWISE::UpdateUAF(f32 _deltaTime)
    {
        ITF_MAP<StringID, SoftwareBus*>::iterator it;

        //update fades on buses 
        for (it = m_buses.begin(); it != m_buses.end(); ++it)
        {
            it->second->updateFade(_deltaTime);
        }

        // update volume on buffer instances 
        m_instanceManager->update();

        // update volume on streamed instances 
        MUSICMANAGER->musicInstanceContainerUpdate(_deltaTime);

        // now reset volume flags 
        for (it = m_buses.begin(); it != m_buses.end(); ++it)
        {
            it->second->resetVolumeUpdateFlag();
        }
    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::updatePositions()
	{
       //todoisap
		AK::SoundEngine::Query::AkGameObjectsList akGameObjectsList;

		const AKRESULT	akResult = AK_SoundEngine_Query_GetActiveGameObjects(akGameObjectsList);

		if(akResult == AK_Success)
		{
            const u32	c_size = 64;
            AkPlayingID playingID[c_size];
            AkUInt32	numberOfPlayingID = c_size;
            AKRESULT	result = AK_Success;
            bool isAlive, positionChanged;
           //  AkAuxSendValue envVolumes[AK_MAX_AUX_PER_OBJ];

			for(AK::SoundEngine::Query::AkGameObjectsList::Iterator it = akGameObjectsList.Begin(), itEnd = akGameObjectsList.End(); it != itEnd; ++it)
			{
				const AkGameObjectID	akGameObjectID = *it;
                ITF::Wwise::AkGameObject*akGameObject = Wwise::AkGameObjectFactory::s_getSingleton()->getAkGameObject(akGameObjectID);
                
                if(!akGameObject) continue; // a play could be done by wwise authoring tool => no gao in this case

                akGameObject->updatePosition(&isAlive, &positionChanged);

				if(isAlive)
                {
                    // if here == gao is ok, all works fine

                    /*if (positionChanged && !m_environments.empty())
                    {
                        const Vec3d& actorPos = akGameObject->getPosition();
                        int envIndex = 0;
                        for (EnvironmentComponentVector::iterator envIter = m_environments.begin(), envIterEnd = m_environments.end(); envIter != envIterEnd; ++envIter)
                        {
                            const f32 envVolume = (*envIter)->CalculateEnvValue(actorPos);

                            if (envVolume > 0.f)
                            {
                                envVolumes[envIndex].auxBusID = (*envIter)->GetEnvironmentId();
                                envVolumes[envIndex].fControlValue = envVolume;
                                ++envIndex;
                            }

                            if (envIndex == AK_MAX_AUX_PER_OBJ)
                            {
                                break;
                            }
                        }
                        AK_SoundEngine_SetGameObjectAuxSendValues(akGameObjectID, envVolumes, envIndex);
                    }*/

                }
                else
				{
                    // if here == gao will be deleted

					numberOfPlayingID = c_size;
					result = AK_SoundEngine_Query_GetPlayingIDsFromGameObject(akGameObjectID, numberOfPlayingID, playingID);

					if(result == AK_Success)
					{
						AUDIO_ASSERT(numberOfPlayingID < c_size, "increase \"c_size\" value");

						for(u32 i = 0; i < numberOfPlayingID; ++i)
						{
							PlayingIDs::const_iterator	it = m_playingIDs.find(playingID[i]);
							if(it != m_playingIDs.end())
							{
								if(it->second.m_stopOnObjecDestroy)
								{
									stop(SoundHandle(playingID[i]));
								}
							}
						}
					}

					Wwise::AkGameObjectFactory::s_getSingleton()->destroy(akGameObjectID);
				}

			}
		}

		akGameObjectsList.Term();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::update(f32  _dt  )
	{

#if !defined(ITF_FINAL) && defined(ITF_SUPPORT_EDITOR)
        forceActiveAuxBus();
#endif
		if(!isRunning())
			return;


		//$1
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//    treat end of event signals
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//


		treatEndOfEvent();

  

        UpdateUAF(_dt);


		//$1
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//    update object position/orientation
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//

		updatePositions();

		//$1
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//    update listener position/orientation
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//

		AkListenerPosition	cameraPosition;
        const Camera* camera = View::getMainView()->getCamera();


        cameraPosition.SetPosition(camera->getX(), camera->getY(), camera->getZ()/* + m_config->getMicroZoffset()*/);
        cameraPosition.SetOrientation(0.f, 0.f, 1.f,0.f,1.f,0.f);
 
		AkListenerPosition	listenerPosition;

		for(u32 i = 0; i < Wwise::Configure::LISTENER_COUNT; ++i)
		{
            if (m_listenersState[i].m_lockedOnCameraPosition)
            {
                listenerPosition.SetPosition(cameraPosition.Position());
                m_listenersState[i].m_akPosition.SetPosition(cameraPosition.Position());

            }
			else
				listenerPosition.SetPosition(m_listenersState[i].m_akPosition.Position());

			if(m_listenersState[i].m_lockedOnCameraOrientation)
            {
                listenerPosition.SetOrientation(cameraPosition.OrientationFront(), cameraPosition.OrientationTop());
                m_listenersState[i].m_akPosition.SetOrientation(listenerPosition.OrientationFront(), listenerPosition.OrientationTop());

			}
			else
			{
                listenerPosition.SetOrientation(m_listenersState[i].m_akPosition.OrientationFront(), m_listenersState[i].m_akPosition.OrientationTop());
			
			}
   
            AK_SoundEngine_SetPosition(static_cast<AkGameObjectID>(ITF::ListenerID::eDefaultListener), listenerPosition);
		}
        
        

		//$1
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//    update wwise engine
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//
#if !defined(AK_SUPPORT_THREADS)
// In single-threaded environments, process all pending I/O actions prior to rendering audio.
        AK::StreamMgr::PerformIO();
#endif

		AK_SoundEngine_RenderAudio();

		//$1
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//    update metronomes
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//

		for(u32 metronomeID = 0; metronomeID < METRONOME_TYPE_COUNT; ++metronomeID)
		{
			m_metronomeState[metronomeID]->update();
		}



#if !defined(ITF_FINAL)
		if(m_isInDebugMode)
		{
			updateFailedBank();
			const f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
			u32 size = m_loadedBank.size()+1+ m_failedBank.size()+1;
			f32 y =GFX_ADAPTER->getScreenHeight()-(15.f *(size+1))-75.0f;
			char buf[256];
			f32 R = 0.9f;
			f32 G = 0.9f;
			f32 B = 0.9f;
			SPRINTF_S(buf, sizeof(buf), "Audio Bank loaded :");
            f32 width;
            //, height;
// 			if (!GFX_ADAPTER->drawDBGTextSize( buf, width, height)) //todoisap
 			{
 				width = 200;
			//	height = 15;
 			
 			}
			f32 x = screenWidth - 75.0f - width;
			GFX_ADAPTER->drawDBGText(buf, x, y, R, G, B);
			y += 15.0f;
			for(LoadedBankMap::const_iterator it = m_loadedBank.begin(), itEnd = m_loadedBank.end(); it != itEnd; ++it)
			{
				const Path	&path = it->first;
				SPRINTF_S(buf, sizeof(buf), "%s", path.getBasename());
				GFX_ADAPTER->drawDBGText(buf, x, y, R, G, B);
				y += 15.0f;

			}
			if(m_failedBank.size())
			{
				R = 0.9f;
				G = 0.0f;
				B = 0.0f;
				y += 15.0f;
				SPRINTF_S(buf, sizeof(buf), "Audio Bank failed to Load :");
				GFX_ADAPTER->drawDBGText(buf, x, y, R, G, B);
				y += 15.0f;
				for(u32 i=0; i<m_failedBank.size(); ++i)
				{
					SPRINTF_S(buf, sizeof(buf), "%s", m_failedBank[i]);
					GFX_ADAPTER->drawDBGText(buf, x, y, R, G, B);
					y += 15.0f;

				}
			}
			
		}
#endif
	}
}
#endif // ITF_SUPPORT_WWISE
