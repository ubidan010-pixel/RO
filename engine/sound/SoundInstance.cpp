#include "precompiled_engine.h"


#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SOUNDINSTANCE_H_
#include "engine/sound/SoundInstance.h"
#endif //_ITF_SOUNDINSTANCE_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"


namespace ITF
{


    //************************************
    // @name	ITF::SoundInstance::stopVoice
    // @public 
    // @return	void
    //************************************
    void SoundInstance::stopVoice()
    {
        if (m_sourceHandle != InvalidSourceHandle)
        {
            if ( getIsStreamed() )
            {
                MUSICMANAGER->stop(m_soundDescriptor->getTemplate()->GetName());
                MUSICMANAGER->removeMusic(m_soundDescriptor->getTemplate()->GetName());
                setHandle(InvalidSourceHandle);
                setState(SoundInstance::InstanceState_Unused);
            }
            else
            {
#ifdef ITF_SUPPORT_RAKI
                SOUND_ADAPTER->stop(m_sourceHandle);
#else
                if (m_playingHandle.isValid())
                {
                    m_loopingAndStarted = false;
                    AUDIO_ADAPTER->stop(m_playingHandle);
                    m_playingHandle.invalidate();
                }

                if (getIsPendingSyncStart())
                {
                    resetPendingSyncStart();
                }

                
#endif
            }
        }
    }

    //************************************
    // @name	ITF::SoundInstance::clear
    // @public 
    // @return	void
    //************************************
    void SoundInstance::clear()
    {
        //for given instance, destroy its voice, and set state to unused
#ifdef ITF_SUPPORT_RAKI
        if (m_sourceHandle != InvalidSourceHandle 
            && !getIsStreamed())
            SOUND_ADAPTER->destroySource(m_sourceHandle);
#else

        if (AUDIO_ADAPTER && !getIsStreamed())
        {
            if (m_playingHandle.isValid())
            {
                AUDIO_ADAPTER->stop(m_playingHandle);
            }
        
            if(m_wwiseObjectref.isValid())
            {
                AUDIO_ADAPTER->unregisterObjectRef(m_wwiseObjectref);
            }

            m_wwiseObjectref.invalidate();
        }

#endif
        m_sourceHandle = InvalidSourceHandle;
        m_state = SoundInstance::InstanceState_Unused;
        m_playingHandle = SoundHandle::getInvalidHandle();


        m_modifiers.clear();
        m_categoryLimiter = NULL;
        m_limiter = NULL;
        m_owner.invalidate();
        m_delay = 0.0f;
        m_boneIndex = U32_INVALID;
        m_postponePlay = bfalse;
        m_loopingAndStarted = false;
    }

    //************************************
    // @name	ITF::SoundInstance::isPlaying
    // @public 
    // @return	ITF::bbool
    //************************************
    bbool SoundInstance::isPlaying() const
    {
        if (getIsPendingSyncStart())
            return true;

        if ( getIsStreamed() )
            return getState() != SoundInstance::InstanceState_Unused;

        if (getState() == SoundInstance::InstanceState_Delay)
            return btrue;

        if (m_sourceHandle == InvalidSourceHandle)
            return bfalse;
#ifdef ITF_SUPPORT_RAKI
        return SOUND_ADAPTER->isPlaying(m_sourceHandle);
#else
        if (!m_playingHandle.isValid())
            return bfalse;

        if (m_loopingAndStarted)
            return true;

        return AUDIO_ADAPTER->isPlaying(m_playingHandle);
#endif
    }

    //************************************
    // @name	ITF::SoundInstance::setParams
    // @public 
    // @param	ObjectRef _owner
    // @param	SoundParams * _params
    // @return	void
    //************************************
    void SoundInstance::setParams( ObjectRef _owner, const SoundDescriptor * _params)
    {
        ITF_ASSERT(m_modifiers.size() == 0);
        ITF_ASSERT(_owner != ITF_INVALID_OBJREF);

        for (u32 i = 0; i < _params->getTemplate()->getParams()->m_modifiers.size(); ++i)
        {   
            m_modifiers.push_back(_params->getTemplate()->getParams()->m_modifiers[i]);
        }

        m_owner = _owner;

#ifndef ITF_SUPPORT_RAKI
#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE)
        m_wwiseObjectref = AUDIO_ADAPTER->createWwiseActor(m_debugName.cStr());
#else
        m_wwiseObjectref = AUDIO_ADAPTER->createWwiseActor();

#endif
#endif
        SoundComponent * sound = static_cast<Actor*>(GETOBJECT(_owner))->GetComponent<SoundComponent>();

        ITF_ASSERT(sound);

        m_componentVolume = sound->getVolume();

        const SoundParams * params = _params->getTemplate()->getParams();

        setInitialVolume( Seeder::getSharedSeeder().GetFloat(params->getRandomVolMin().ratio()*_params->getTemplate()->getVolume(),params->getRandomVolMax().ratio()*_params->getTemplate()->getVolume()));
        setInitialPitch(Seeder::getSharedSeeder().GetFloat(params->getRandomPitchMin(),params->getRandomPitchMax()));
        m_fadeIn = params->getFadeInTime();
        m_fadeOut = params->getFadeOutTime();
        m_filterType = params->getFilterType();
        m_filterFrequency = params->getFilterFrequency();
        m_filterQ = params->getFilterQ();
        m_loop = params->getLoop();
        m_delay = params->getDelay() +Seeder::getSharedSeeder().GetFloat(-params->getRandomDelay(),params->getRandomDelay());
        m_delay = Max(0.0f,m_delay);
        m_metronomeType = params->getMetronomeType();
        m_playOnNext = params->getPlayOnNext();
//         if ( !_params->getTemplate()->isMusic() )
//             SOUND_ADAPTER->setFilter(getHandle(),m_filterType,m_filterFrequency,m_filterQ);
    }

    //************************************
    // @name	ITF::SoundInstance::getPan
    // @public 
    // @return	f32
    //************************************
    f32 SoundInstance::getPan()
    {
        f32 pan = 0.f;

        for (u32 i =0; i< m_modifiers.size();++i)
        {
            pan += m_modifiers[i]->getPan( this );
        }

        return Clamp( pan, -1.f, 1.f );
    }

    //************************************
    // @name	ITF::SoundInstance::getVolume
    // @public 
    // @param	f32 _volume
    // @return	ITF::f32
    //************************************
    f32 SoundInstance::getVolume(f32 _volume)
    {
        for (u32 i =0; i< m_modifiers.size();++i)
        {
            _volume *= m_modifiers[i]->getVolume(this, _volume);
        }
        return _volume * m_initialVolume * m_componentVolume;
    }

    //************************************
    // @name	ITF::SoundInstance::getPitch
    // @public 
    // @param	f32 _pitch
    // @return	ITF::f32
    //************************************
    f32 SoundInstance::getPitch(f32 _pitch)
    {
        for (u32 i =0; i< m_modifiers.size();++i)
        {
            _pitch = Clamp(_pitch * m_modifiers[i]->getPitch(this, _pitch),1.0f/1024.0f,1024.0f);
        }
        return Clamp(_pitch * m_initialPitch,1.0f/1024.0f,1024.0f);
    }

    //************************************
    // @name	ITF::SoundInstance::update
    // @public 
    // @param	f32 _dt
    // @return	void
    //************************************
    void SoundInstance::update(f32 _dt)
    {
        if ( (getHandle() == InvalidSourceHandle) && (getState() != SoundInstance::InstanceState_Delay) )
        {
            return;
        }

        if (!isPlaying())
        {
            return;
        }

#ifndef ITF_FINAL
        //update volume so users can change volume while game is running
        if (m_owner.isValid())
        {
            SoundComponent * sound = static_cast<Actor*>(GETOBJECT(m_owner))->GetComponent<SoundComponent>();
            ITF_ASSERT(sound);
            m_componentVolume = sound->getVolume();
        }
#endif //ITF_FINAL


        //UPDATE FADES AND PLAY DELAYED SOUNDS
        f32 fadeVolume = 1.0f;
        if (getState() == SoundInstance::InstanceState_Delay)
        {
            if (getTimer() > getDelay())
            {
                // since we will use ::play, reset delay so sound is really played now 
                m_delay = 0.f;

                setState(InstanceState_Playing);

                u32 soundIndex = m_soundDescriptor->getSoundIndex();

                ITF_WARNING(NULL,soundIndex < m_soundDescriptor->getResourceList().size(),"SoundInstance::update while trying to play a delayed sound, invalid resource index %d should be less than %d",soundIndex,m_soundDescriptor->getResourceList().size());

                if ( soundIndex < m_soundDescriptor->getResourceList().size() )
                {
                    Sound *sound = static_cast<Sound *>(m_soundDescriptor->getResourceList()[soundIndex].getResource());
                    ITF_ASSERT( sound );

                    if ( sound )
                    {
                        SourceInfo params;
                        //initialize voice parameters
                        params.m_numChannels = m_soundDescriptor->getTemplate()->getNumChannels();
                        setNumChannels(m_soundDescriptor->getTemplate()->getNumChannels());
                        setSoundDescriptor(m_soundDescriptor);
                        params.m_loop = m_soundDescriptor->getTemplate()->getLoop();
                        params.m_sound = sound;
                        params.m_isMusic = m_soundDescriptor->getTemplate()->isMusic();
                        params.m_usePitch = m_soundDescriptor->getTemplate()->usePitch();
                        params.m_useFilter = m_soundDescriptor->getTemplate()->useFilter();
                        params.m_bus = m_soundDescriptor->getCategory();
#ifdef ITF_SUPPORT_RAKI
                        m_sourceHandle = SOUND_ADAPTER->createSource(&params);
#else
                        m_sourceHandle = AUDIO_ADAPTER->createSource(&params,this);
#endif

                        play();
                    }
                }
            }
        }
        else if (getState() == SoundInstance::InstanceState_Playing)
        {
            f32 fadeInTime = getFadeIn();
            if ( fadeInTime > 0.0f && getTimer() < fadeInTime)
            {
                fadeVolume  = Clamp( getTimer() / fadeInTime, 0.f, 1.f );
            }
        }
        else if (getState() == SoundInstance::InstanceState_Stopping)
        {
            f32 fadeOutTime = getFadeOut();
            if ( getTimer() < fadeOutTime)
            {
                fadeVolume = Clamp( ( 1.0f - (getTimer() / fadeOutTime) ), 0.f, 1.f );
            }
            else
            {
                stopVoice();

                return;
            }
        }
#ifdef ITF_SUPPORT_RAKI
        SOUND_ADAPTER->setPan( getHandle(), getPan() );
#else
        AUDIO_ADAPTER->setPan(m_wwiseObjectref, getPan());
#endif

        // It happens that volume returned is not clamped between 0.0 and 1.0, it seems to be a sound modifier issue.
        // remove comments around const when these issues are fixed.
        /*const*/ f32 volume = getVolume(fadeVolume);
        ITF_WARNING_CATEGORY(GPP,NULL,volume >= 0.0f && volume <= 1.0f, "volume computed not valid, please check sound modifiers");
        volume = f32_Clamp(volume,0.0f,1.0f);

        const f32 pitch = getPitch(1.0f);

        if ( getIsStreamed() )
        {
#ifndef ITF_FINAL
            //DebugDraw::text(ITF_POS2D_AUTO, ITF_POS2D_AUTO, Color::white(), "Volume : %.2f, timer : %.2f" , volume, getTimer());
#endif // ITF_FINAL
            MUSICMANAGER->setLinearVolume(m_soundDescriptor->getTemplate()->GetName(),volume);
        }
        else
        {
#ifdef ITF_SUPPORT_RAKI
            SOUND_ADAPTER->setVolume(getHandle(), volume);
#else
            AUDIO_ADAPTER->setVolume(m_wwiseObjectref, volume);
#endif
        }
            

        if (!getIsStreamed())
        {
#ifdef ITF_SUPPORT_RAKI
            SOUND_ADAPTER->setPitch(getHandle(), pitch);
#else
            AUDIO_ADAPTER->setPitch(m_wwiseObjectref, pitch);
#endif
        }
          

        //const f32 filterFreq = getFilterFrequency(0.0f);
        //SOUND_ADAPTER->setFilter(getFilt)

        // don't update time when paused 
        if ( !MUSICMANAGER->isPaused() )
            setTimer(getTimer() + _dt);

#ifndef ITF_FINAL
#ifdef ITF_SUPPORT_RAKI

        // for external debug [start]
        m_debugVolume = Volume::AmplitudeRatioToDecibels(volume);
        m_debugPitch = pitch;
        // for external debug [end]

        if (SOUND_ADAPTER->isDebug()) 
        {
            Vec3d pos2d;
            GFX_ADAPTER->compute3DTo2D(getPos(),pos2d);

            String tempString;
            tempString.setTextFormat("       %s",getDebugName().cStr());
            GFX_ADAPTER->drawDBGText(tempString);
            tempString.setTextFormat("vol: %f pitch: %f", m_debugVolume, m_debugPitch, getDebugName().cStr());
            GFX_ADAPTER->drawDBGText(tempString);

            //f32 halfScreenWidth = 0.5f * (f32)GFX_ADAPTER->getScreenWidth();

            //Vec3d radius2d = pos2d + Vec3d::XAxis * instance.m_params.getRollOffMinDistance() * halfScreenWidth;
            //Vec3d radius;
            //GFX_ADAPTER->compute2DTo3D(radius2d,radius);
            //f32 mindist = (radius-instance.m_pos).norm();

            //radius2d = pos2d + Vec3d::XAxis * instance.m_params.getRollOffMaxDistance() * halfScreenWidth;
            //radius;
            //GFX_ADAPTER->compute2DTo3D(radius2d,radius);
            //f32 maxdist = (radius-instance.getPos()).norm();

            //GFX_ADAPTER->drawDBGCircle(instance.getPos(), mindist,0.0f,1.0f,0.0f);
            //GFX_ADAPTER->drawDBGCircle(instance.getPos(), maxdist,1.0f,0.0f,0.0f);
            GFX_ADAPTER->drawDBGText(getDebugName(),pos2d.m_x,pos2d.m_y);
        }
#endif
#endif

    }
#ifdef ITF_SUPPORT_WWISE
	void SoundInstance::updateVolume()
	{
        if ((getHandle() == InvalidSourceHandle) && (getState() != SoundInstance::InstanceState_Delay))
        {
            return;
        }

        if (!isPlaying())
        {
            return;
        }

#ifndef ITF_FINAL
        //update volume so users can change volume while game is running
        if (m_owner.isValid())
        {
            SoundComponent* sound = static_cast<Actor*>(GETOBJECT(m_owner))->GetComponent<SoundComponent>();
            ITF_ASSERT(sound);
            m_componentVolume = sound->getVolume();
        }
#endif //ITF_FINAL


        //UPDATE FADES AND PLAY DELAYED SOUNDS
        f32 fadeVolume = 1.0f;
         if (getState() == SoundInstance::InstanceState_Playing)
        {
            f32 fadeInTime = getFadeIn();
            if (fadeInTime > 0.0f && getTimer() < fadeInTime)
            {
                fadeVolume = Clamp(getTimer() / fadeInTime, 0.f, 1.f);
            }
        }
        else if (getState() == SoundInstance::InstanceState_Stopping)
        {
            f32 fadeOutTime = getFadeOut();
            if (getTimer() < fadeOutTime)
            {
                fadeVolume = Clamp((1.0f - (getTimer() / fadeOutTime)), 0.f, 1.f);
            }
            else
            {
                stopVoice();

                return;
            }
        }

        // It happens that volume returned is not clamped between 0.0 and 1.0, it seems to be a sound modifier issue.
        // remove comments around const when these issues are fixed.
        /*const*/ f32 volume = getVolume(fadeVolume);
        ITF_WARNING_CATEGORY(GPP, NULL, volume >= 0.0f && volume <= 1.0f, "volume computed not valid, please check sound modifiers");
        volume = f32_Clamp(volume, 0.0f, 1.0f);


        if (getIsStreamed())
        {
            MUSICMANAGER->setLinearVolume(m_soundDescriptor->getTemplate()->GetName(), volume);
        }
        else
        {
          
            AUDIO_ADAPTER->setVolume(m_wwiseObjectref, volume);

        }
	}
#endif

	//************************************
    // @name	ITF::SoundInstance::getInput
    // @public 
    // @param	StringID _input
    // @param	f32 & _result
    // @return	ITF::bbool
    //************************************
    const InputContainer &  SoundInstance::getInputs(  ) const
    {
        SoundComponent * sound = static_cast<Actor*>(GETOBJECT(m_owner))->GetComponent<SoundComponent>();
        ITF_ASSERT(sound);
        return sound->getInputs();
    }

    //************************************
    // @name	ITF::SoundInstance::play
    // @public 
    // @return	void
    //************************************
    void SoundInstance::play()
    {
        //Set instance from voice and other params
        if ( getState() != InstanceState_Stopping )
            setTimer(0.0f);
        else
            setTimer( getFadeOut() * (getFadeIn() - getTimer()) / getFadeIn());
        setState(SoundInstance::InstanceState_Playing);
#ifdef ITF_SUPPORT_RAKI
        SOUND_ADAPTER->setPan( getHandle(), getPan() );
        SOUND_ADAPTER->setPitch(getHandle(), getPitch(1.0f));


        //FADE
        f32 streamVolume = 0.0f;
        if (getFadeIn() > 0.0f)
        {
            if ( getIsStreamed() )
                streamVolume = 0.0f;
            else
                SOUND_ADAPTER->setVolume(getHandle(), 0.0f);
        }
        else if ( getIsStreamed() )
            streamVolume = getVolume(1.0f);
        else
            SOUND_ADAPTER->setVolume(getHandle(), getVolume(1.0f));


        if (getDelay() > 0.0f)
        {
            setState(InstanceState_Delay);
        }
        else
        {
            if ( getIsStreamed() )
            {
                StringID wwiseEventGuid{};
                MUSICMANAGER->addMusic(m_soundDescriptor->getTemplate()->GetName(),m_soundDescriptor->getTemplate()->getFiles()[0],
                    m_soundDescriptor->getTemplate()->getCategory(), wwiseEventGuid);
                MUSICMANAGER->play(m_soundDescriptor->getTemplate()->GetName(),0.0f,streamVolume,getLoop());
            }
            else if ( getMetronomeType() != U32_INVALID && getPlayOnNext() != U32_INVALID
                && MUSICMANAGER->playSyncWithMetronome( getHandle(), getMetronomeType(), getPlayOnNext(), streamVolume ) )
            {
                 // Don't do anything as sound will be played through MUSIC_MANAGER synced with musique
            }
            else 
                SOUND_ADAPTER->play(getHandle());
        }
#else

        AUDIO_ADAPTER->setPan(m_wwiseObjectref, getPan());
        AUDIO_ADAPTER->setPitch(m_wwiseObjectref, getPitch(1.0f));


        //FADE
        f32 streamVolume = 0.0f;
        if (getFadeIn() > 0.0f)
        {
            if (getIsStreamed())
                streamVolume = 0.0f;
            else
                AUDIO_ADAPTER->setVolume(m_wwiseObjectref, 0.0f);
        }
        else if (getIsStreamed())
            streamVolume = getVolume(1.0f);
        else
            AUDIO_ADAPTER->setVolume(m_wwiseObjectref, getVolume(1.0f));


        if (getDelay() > 0.0f)
        {
            setState(InstanceState_Delay);
        }
        else
        {
            if (getIsStreamed())
            {
                MUSICMANAGER->addMusic(m_soundDescriptor->getTemplate()->GetName(), m_soundDescriptor->getTemplate()->getFiles()[0],
                    m_soundDescriptor->getTemplate()->getCategory(), m_soundDescriptor->getTemplate()->getWwiseEventGuid() );
                MUSICMANAGER->play(m_soundDescriptor->getTemplate()->GetName(), 0.0f, streamVolume, getLoop());
            }
            else if (getMetronomeType() != U32_INVALID && getPlayOnNext() != U32_INVALID
                && MUSICMANAGER->playSyncWithMetronome(getHandle(), getMetronomeType(), getPlayOnNext(), streamVolume))
            {
                // Don't do anything as sound will be played through MUSIC_MANAGER synced with musique
            }
            else
            {
                if (m_soundDescriptor->getTemplate())
                {
                    u32 soundEventID = m_soundDescriptor->getSoundEventID();
                    AudioPlayRequest audioPlayRequest(SoundEventID(soundEventID), m_wwiseObjectref);
                    m_playingHandle = AUDIO_ADAPTER->play(audioPlayRequest);

                    if (m_loop)
                        m_loopingAndStarted = true;

                }


            }
        }
#endif
    }

    //************************************
    // @name	ITF::SoundInstance::stop
    // @public 
    // @return	void
    //************************************
    void SoundInstance::stop()
    {
        if (getState() != SoundInstance::InstanceState_Unused)
        {
            if (getFadeOut() > 0.0f)
            {
                if (getState() != SoundInstance::InstanceState_Stopping)
                    setTimer(0.0f);
                setState(SoundInstance::InstanceState_Stopping);
            }
            else
            {
                stopVoice();
            }
        }
    }


    f32 SoundInstance::getCurrentVolume() 
    {
#ifdef ITF_SUPPORT_RAKI
        return SOUND_ADAPTER->getVolume(m_sourceHandle);
#else
         f32 vol = getVolume(1.0f);
        return vol;
#endif
    }

    //************************************
    // @name	ITF::SoundInstance::getFilterFrequency
    // @public 
    // @param	FilterType _filterType
    // @param	f32 _filterFrequency
    // @param	f32 _filterQ
    // @return	ITF::f32
    //************************************
    f32 SoundInstance::getFilterFrequency(FilterType _filterType, f32 _filterFrequency, f32 _filterQ)
    {
        f32 filterFrequency = 0.0f;
        f32 numModifiers = 0.0f;
        for (u32 i =0; i< m_modifiers.size();++i)
        {
            if (m_modifiers[i]->useFilter())
            {
                filterFrequency += m_modifiers[i]->getFilterFrequency(this, _filterType, _filterFrequency, _filterQ);
                ++numModifiers;
            }
        }
        if (numModifiers > 0.0f)
        {
            filterFrequency /= numModifiers;
            return filterFrequency;
        }
        else
        {
            return _filterFrequency;
        }
    }
}


