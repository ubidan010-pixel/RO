#ifndef _ITF_RAKIINTERFACE_H_
#define _ITF_RAKIINTERFACE_H_




#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_

#ifndef _ITF_RAKISYSTEM_H_
#include "engine/sound/raki/RakiSystem.h"
#endif //_ITF_RAKISYSTEM_H_







namespace raki
{
    class Initializer;
}


namespace ITF
{

    class RakiInterface 
        : public TemplateSingleton<RakiInterface>
    {
    public:

        struct InitStruct 
        {
            InitStruct()
#ifdef RAKI_USING_XAUDIO2
                : m_xaudio2( NULL )
#endif // RAKI_USING_XAUDIO2

            {
            }

#ifdef RAKI_USING_XAUDIO2
            IXAudio2 * m_xaudio2;
#endif // RAKI_USING_XAUDIO2

#ifdef ITF_WII
            void * m_axBuffer;
#endif // ITF_WII

        };

        RakiInterface();
        virtual ~RakiInterface();

        void initLowLevelAudio( const InitStruct * _initStruct );
        void uninitLowLevelAudio();

        void update(f32 _dt);

    private:

        RakiAllocator                       m_rakiAllocator;
        raki::Initializer *                 m_initializer;

    };


#define RAKIINTERFACE RakiInterface::getptr()




}
#endif // _ITF_RAKIINTERFACE_H_

