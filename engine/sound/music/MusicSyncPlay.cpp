#include "precompiled_engine.h"



#ifndef _ITF_MUSICSYNCPLAY_H_
#include "engine/sound/Music/MusicSyncPlay.h"
#endif // _ITF_MUSICSYNCPLAY_H_



#ifndef RAKI_SOUNDS_SEQUENCER_H
#include "RakiEngine/Sounds/Sequencer/Sequencer.h"
#endif // RAKI_SOUNDS_SEQUENCER_H

#ifndef RAKI_VOICEBASE_H
#include "RakiEngine/Sounds/VoiceBase.h"
#endif // RAKI_VOICEBASE_H

#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_

#include "engine/AdaptersInterfaces/SoundAdapter.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

#ifndef _ITF_INSTANCEMANAGER_H_
#include "engine/sound/InstanceManager.h"
#endif // _ITF_INSTANCEMANAGER_H_


#if defined ( ITF_WII )

#ifndef RAKI_AUDIOCALLBACKMANAGER_WII_H
#include "RakiEngine/Specific/_Wii/AudioCallbackManager_Wii.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_WII_H

#endif // defined ( ITF_WII )



//#define LOG_MUSICSYNCPLAY




namespace ITF
{


MusicSyncPlay::MusicSyncPlay()
    : m_voicesToAdd( ITF_MUSICSYNCPLAY_MAXREGISTEREDINSTANCES )
    , m_nbElementsToPlay( 0 )
    , m_currentMusicPart( NULL )
    , m_subdivisionSampleDuration( 0.f )
    , m_lastCallbackSubdivision( 0 )
{
    for ( u32 ui = 0 ; ui < ITF_MUSICSYNCPLAY_MAXREGISTEREDINSTANCES ; ++ui )
    {
        m_voiceToPlayArray[ ui ].m_subdivision = 0;
        m_voiceToPlayArray[ ui ].m_voiceBase = NULL;
    }
}


MusicSyncPlay::~MusicSyncPlay()
{

}


bbool MusicSyncPlay::registerForPlayAtNextDivision( const u32 _handle, const u32 _playOnNextSubdivision, const f32 _volume )
{
#if defined(ITF_SUPPORT_RAKI)
    raki::VoiceBase * voice = SOUND_ADAPTER->getInstanceManager() ? SOUND_ADAPTER->getInstanceManager()->getInstance( _handle ) : NULL;

    if ( voice )
    {
        return registerForPlayAtNextDivision( voice, _playOnNextSubdivision, _volume );
    }
#else
    SoundInstance*  inst = AUDIO_ADAPTER->getInstanceManager() ? AUDIO_ADAPTER->getInstanceManager()->getInstance(_handle) : NULL;

    if (inst)
    {
        return registerForPlayAtNextDivision(inst, _playOnNextSubdivision, _volume);
    }
    else
    {

#ifdef LOG_MUSICSYNCPLAY
        RAKI_OUTPUT("%p failed to registerForPlayAtNextDivision BECAUSE OF NULL INSTANCE %d ", this, _handle);
#endif // LOG_MUSICSYNCPLAY
    }

#endif

    return bfalse;
}

#if defined(ITF_SUPPORT_RAKI)

bbool MusicSyncPlay::registerForPlayAtNextDivision(raki::VoiceBase* _voice, const u32 _playOnNextSubdivision, const f32 _volume)
{
    if (!_voice)
        return bfalse;

    _voice->setLinearVolume(_volume);

    _voice->updateVolume(true);

    ITF_ASSERT(!_voice->getIsPendingSyncStart());

    ITF_ASSERT(_playOnNextSubdivision);
    ITF_ASSERT(_playOnNextSubdivision <= RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT * 4); // only authorize play in next measure - for the moment 

    if (m_subdivisionSampleDuration > 0.f)
    {
        _voice->setPendingSyncStart(this);

        VoiceInfo info;
        info.m_subdivision = _playOnNextSubdivision;
        info.m_voiceBase = _voice;

        m_voicesToAdd.addElement(info);

#ifdef LOG_MUSICSYNCPLAY
        RAKI_OUTPUT("%p setPendingSyncStart voice 0x%x _playOnNextSubdivision %d", this, _voice, _playOnNextSubdivision);
#endif // LOG_MUSICSYNCPLAY

        return btrue;
    }

#ifdef LOG_MUSICSYNCPLAY
    RAKI_OUTPUT("%p UNABLE TO setPendingSyncStart 0x%x _playOnNextSubdivision", this, _voice, _playOnNextSubdivision);
#endif // LOG_MUSICSYNCPLAY

    return bfalse;
}
#else

bbool MusicSyncPlay::registerForPlayAtNextDivision(SoundInstance* _inst, const u32 _playOnNextSubdivision, const f32 _volume)
{

    if (!_inst)
        return bfalse;

    _inst->setInitialVolume(_volume); //check if volume applied before play

   // ITF_ASSERT(!_inst->getIsPendingSyncStart());

    ITF_ASSERT(_playOnNextSubdivision);
    ITF_ASSERT(_playOnNextSubdivision <= RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT * 4); // only authorize play in next measure - for the moment 

    if (m_subdivisionSampleDuration > 0.f)
    {
        _inst->setPendingSyncStart();

        VoiceInfo info;
        info.m_subdivision = _playOnNextSubdivision;
        info.m_voiceBase = _inst;

        m_voicesToAdd.addElement(info);

#ifdef LOG_MUSICSYNCPLAY
        RAKI_OUTPUT("%p setPendingSyncStart voice 0x%x _playOnNextSubdivision %d", this, _inst, _playOnNextSubdivision);
#endif // LOG_MUSICSYNCPLAY

        return btrue;
    }
    return bfalse;
}
#endif



void MusicSyncPlay::resetMusicAndForceSyncPlays()
{
    // caller must protect from callback 
#if defined ( ITF_WII )
    RAKI_ASSERT( !raki::AudioCallbackManager::singleton().isProcessingAudioCallback() );
#endif // defined ( ITF_WII )

    for ( u32 ui = 0 ; ui < m_nbElementsToPlay ; ++ui )
    {
#if defined(ITF_SUPPORT_RAKI)
        m_voiceToPlayArray[ ui ].m_voiceBase->start();
#else
        m_voiceToPlayArray[ui].m_voiceBase->play();
#endif
        m_voiceToPlayArray[ ui ].m_voiceBase->resetPendingSyncStart();
        m_voiceToPlayArray[ ui ].m_voiceBase = NULL;
        m_voiceToPlayArray[ ui ].m_subdivision = 0;
    }

    m_nbElementsToPlay = 0;

    m_subdivisionSampleDuration = 0.f;
}


void MusicSyncPlay::audioCallback( MusicPart * _musicPart, const u32 _samplePosition )
{
    // careful this is an audio callback or thread, don't do much here
    u32 ui;

    if ( m_currentMusicPart != _musicPart )
    {
        m_currentMusicPart = _musicPart;

        // calculate new subdivision sample duration

        if ( m_currentMusicPart && m_currentMusicPart->getBeatsPerBar() )
        {
            //ITF_ASSERT_MSG( ( m_currentMusicPart->getBeatsPerBar() == 3 ) || ( m_currentMusicPart->getBeatsPerBar() == 4 ), "Invalid beats per bar in music part" ); // code implemented only for 4 beats per bar... 
            ITF_ASSERT_MSG( m_currentMusicPart->getBeatsPerBar(), "Invalid beats per bar in music part" ); // sound tweening is supposed not to be used when rare beats are heard... 

            const u32 nbSubdivisionsInPart = RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT * m_currentMusicPart->getSizeInBars() * m_currentMusicPart->getBeatsPerBar();

            m_subdivisionSampleDuration =  (f32) m_currentMusicPart->getSampleSize() / (f32) nbSubdivisionsInPart;

#ifdef LOG_MUSICSYNCPLAY
            RAKI_OUTPUT( "0x%x MusicSyncPlay::audioCallback setting m_subdivisionSampleDuration %.3f", this, m_subdivisionSampleDuration );
#endif // LOG_MUSICSYNCPLAY
        }
        else
        {
            m_subdivisionSampleDuration = 0.f;

#ifdef LOG_MUSICSYNCPLAY
            RAKI_OUTPUT( "0x%x MusicSyncPlay::audioCallback setting m_subdivisionSampleDuration -> zero", this );
#endif // LOG_MUSICSYNCPLAY
        }
    }



    // check for new elements 

    VoiceInfo * voiceInfo = m_voicesToAdd.getElementIfAvailable();

    while ( voiceInfo )
    {
        ITF_ASSERT( m_nbElementsToPlay < ( ITF_MUSICSYNCPLAY_MAXREGISTEREDINSTANCES - 1 ) );

        if ( m_nbElementsToPlay < ( ITF_MUSICSYNCPLAY_MAXREGISTEREDINSTANCES - 1 ) )
        {
#ifndef ITF_SUPPORT_RAKI
           RAKI_OUTPUT("add new voices  %s", voiceInfo->m_voiceBase->m_debugName.cStr());
#endif
            m_voiceToPlayArray[ m_nbElementsToPlay ].m_subdivision = voiceInfo->m_subdivision;
            m_voiceToPlayArray[ m_nbElementsToPlay ].m_voiceBase = voiceInfo->m_voiceBase ;

            ++m_nbElementsToPlay;
        }

        voiceInfo = m_voicesToAdd.getElementIfAvailable();
    }



    // play registered elements 

    if ( m_currentMusicPart && ( m_subdivisionSampleDuration > 0.f ) )
    {
        u32 currentCallbackSubdivision = ( (u32)( (f32)_samplePosition / m_subdivisionSampleDuration ) ) % ( 4 * RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT );

        if ( m_lastCallbackSubdivision != currentCallbackSubdivision )
        {
            //RAKI_OUTPUT( "currentCallbackSubdivision %d", currentCallbackSubdivision );

            if ( m_nbElementsToPlay )
            {
                ITF_ASSERT( isVoiceToPlayArrayCoherent() );

                do 
                {
                    m_lastCallbackSubdivision = ( m_lastCallbackSubdivision + 1 ) % ( 4 * RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT );

                    int elementsToRemoveAfter = 0;

                    for ( ui = 0 ; ui < m_nbElementsToPlay ; ++ui )
                    {
                        if ( !( currentCallbackSubdivision % m_voiceToPlayArray[ ui ].m_subdivision ) )
                        {
#if defined(ITF_SUPPORT_RAKI)
                            m_voiceToPlayArray[ui].m_voiceBase->startFromAudioCallback();
#else
                            m_voiceToPlayArray[ui].m_voiceBase->play();
#endif
                            m_voiceToPlayArray[ ui ].m_voiceBase->resetPendingSyncStart();
                            m_voiceToPlayArray[ ui ].m_voiceBase = NULL;
                            ++elementsToRemoveAfter;
                        }
                    }

                    // remove elements now

                    while ( elementsToRemoveAfter )
                    {
                        for ( ui = 0 ; ui < m_nbElementsToPlay ; ++ui )
                        {
                            if ( !m_voiceToPlayArray[ ui ].m_voiceBase )
                            {
                                if ( ui == ( m_nbElementsToPlay - 1 ) )
                                {
                                    m_voiceToPlayArray[ ui ].m_subdivision = 0;
                                    m_voiceToPlayArray[ ui ].m_voiceBase = NULL;
                                }
                                else
                                {
                                    m_voiceToPlayArray[ ui ].m_subdivision = m_voiceToPlayArray[ m_nbElementsToPlay - 1 ].m_subdivision;
                                    m_voiceToPlayArray[ ui ].m_voiceBase = m_voiceToPlayArray[ m_nbElementsToPlay - 1 ].m_voiceBase;

                                    m_voiceToPlayArray[ m_nbElementsToPlay - 1 ].m_subdivision = 0;
                                    m_voiceToPlayArray[ m_nbElementsToPlay - 1 ].m_voiceBase = NULL;
                                }

                                break;
                            }
                        }

                        --elementsToRemoveAfter;
                        --m_nbElementsToPlay;
                    }

                    ITF_ASSERT( isVoiceToPlayArrayCoherent() );
                }
                while ( m_lastCallbackSubdivision != currentCallbackSubdivision );

            }
        }
    }
    else
    {
        // no sync - play all now

        if ( m_nbElementsToPlay )
        {
            for ( ui = 0 ; ui < m_nbElementsToPlay ; ++ui )
            {
#if defined(ITF_SUPPORT_RAKI)
                m_voiceToPlayArray[ui].m_voiceBase->start();
#else
                m_voiceToPlayArray[ui].m_voiceBase->play();
#endif
                m_voiceToPlayArray[ ui ].m_voiceBase->resetPendingSyncStart();

                m_voiceToPlayArray[ ui ].m_subdivision = 0;
                m_voiceToPlayArray[ ui ].m_voiceBase = NULL;
            }

            m_nbElementsToPlay = 0;
        }
    }

}


const bool MusicSyncPlay::isVoiceToPlayArrayCoherent()
{
    for ( u32 ui = 0 ; ui < ITF_MUSICSYNCPLAY_MAXREGISTEREDINSTANCES ; ++ui )
    {
        if ( ui < m_nbElementsToPlay )
        {
            if ( !m_voiceToPlayArray[ ui ].m_voiceBase )
                return false;
        }
        else
        {
            if ( m_voiceToPlayArray[ ui ].m_voiceBase )
                return false;
        }
    }

    return true;
}



} // namespace ITF

