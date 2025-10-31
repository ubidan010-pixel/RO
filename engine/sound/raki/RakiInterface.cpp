#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_RAKI
#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_



#ifndef RAKI_INITIALIZER_H
#include "RakiEngine/Initializer/Initializer.h"
#endif // RAKI_INITIALIZER_H

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

#if defined ( ITF_WII )
#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_
#endif // ( ITF_WII )



namespace ITF
{


    RakiInterface::RakiInterface()
        : m_initializer( NULL )
    {
#if defined ( ITF_WII )
        const f32 streamBufferDurationSeconds = 1.0f;
#elif defined ( ITF_X360 )
        const f32 streamBufferDurationSeconds = 3.f;
#elif defined ( ITF_PS3 )
        const f32 streamBufferDurationSeconds = 3.f;
#else
        const f32 streamBufferDurationSeconds = 3.f;
#endif 

        m_initializer = newAlloc( mId_AudioEngine, raki::Initializer )( &m_rakiAllocator, streamBufferDurationSeconds );
    }



    RakiInterface::~RakiInterface()
    {
        if ( m_initializer )
            delete m_initializer;

        m_initializer = NULL;
    }


    void RakiInterface::initLowLevelAudio( const RakiInterface::InitStruct * _initStruct )
    {
        ITF_ASSERT( _initStruct );

        raki::EngineInitializer::InitStruct initStruct;
#ifdef RAKI_USING_XAUDIO2
        initStruct.m_xaudio2 = _initStruct->m_xaudio2;
#elif defined ( RAKI_PLATFORM_PS3 )
        initStruct.m_ps3 = NULL;
#elif defined ( RAKI_PLATFORM_WII )
        initStruct.m_consoleIsSetAsMono = ( SYSTEM_ADAPTER_WII->getSoundModeFromWiiSettings() == SystemAdapter_WII::SoundMode_Mono );
        initStruct.m_axBuffer = _initStruct->m_axBuffer;
#endif 

        m_initializer->initAudio( &initStruct );
    }


    void RakiInterface::uninitLowLevelAudio()
    {
        ITF_ASSERT( m_initializer );

        m_initializer->uninitAudio();
    }


    void RakiInterface::update(f32 _dt)
    {

    }

}
#endif
