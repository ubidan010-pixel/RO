#ifndef _ITF_SOUNDADAPTER_H_WII_
#define _ITF_SOUNDADAPTER_H_WII_

#if defined (ITF_WII)
#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_
namespace ITF
{
    class Bus 
    {
    public:
        Bus( BusDef * _busdef)
            : m_busDef(_busdef)
            , m_voice(NULL)
            , m_processingStage(U32_INVALID)
            , m_timer(0.0f)
            , m_targetVolume(1.0f)
            , m_initialVolume(1.0f)
            , m_timeToFade(0.0f)
        {

        }

        ITF_INLINE StringID getName() const { return m_busDef->m_name; }
        BusDef * m_busDef;
        void * m_voice;
        ITF_VECTOR<StringID> m_inputs;
        u32 m_processingStage;
        f32 m_timer;
        f32 m_targetVolume;
        f32 m_initialVolume;
        f32 m_timeToFade;
    };

    class SoundAdapter_WII : public SoundAdapter
	{
	public:
		SoundAdapter_WII      (   );
		~SoundAdapter_WII     (   );

        //ENGINE
		//************************************
		// @name	ITF::SoundAdapter_WII::init
		// @public 
		// @return	ITF::bbool
		//************************************
        virtual bbool init(bbool _audioCaptureAllowed);

        //************************************
        // @name	ITF::SoundAdapter_WII::isRunning
        // @public 
        // @return	ITF::bbool
        //************************************
        virtual bbool isRunning() const;
		//************************************
		// @name	ITF::SoundAdapter_WII::shutdown
		// @public 
		// @return	void
		//************************************
		virtual void shutdown();

        //RESOURCE LOADING
		//************************************
		// @name	ITF::SoundAdapter_WII::loadSound
		// @public 
		// @param	Sound * _sound
		// @param	const char * _path
		// @return	ITF::bbool
		//************************************
		virtual bbool loadSound(Sound * _sound, const char * _path);
		//************************************
		// @name	ITF::SoundAdapter_WII::unloadSound
		// @public 
		// @param	Sound * _sound
		// @return	void
		//************************************
		virtual void unloadSound(Sound * _sound);

        //SOURCE MANAGEMENT
		//************************************
		// @name	ITF::SoundAdapter_WII::createSource
		// @virtual private 
		// @param	const SourceInfo * _params
		// @return	ITF::SourceHandle
		//************************************
		virtual SourceHandle createSource( SourceInfo * _params);
        //SOUND CONTROL


        //DEBUG
        //************************************
        // @name	ITF::SoundAdapter_WII::toggleDebug
        // @public 
        // @return	void
        //************************************
        virtual void toggleDebug();
        //************************************
        // @name	ITF::SoundAdapter_WII::isDebug
        // @public 
        // @return	ITF::bbool
        //************************************
        virtual bbool isDebug();

		//************************************
        // @name	ITF::SoundAdapter::getSoundDuration
        // @public 
        // @param	const Sound * _sound
        // @return	ITF::f32 duration of sound
        //************************************
		virtual f32 getSoundDuration(const Sound * _sound) {return 0.0f;};

        //************************************
        // @name	ITF::SoundAdapter_XAudio::submitSourceBuffer
        // @public 
        // @param	SourceHandle _handle
        // @param	const XAUDIO2_BUFFER & _xaudioBuffer
        // @param	const XAUDIO2_BUFFER_WMA & _xwmaBuffer
        // @return	ITF::bbool
        //************************************
        bbool submitSourceBuffer();
        //************************************
        // @name	ITF::SoundAdapter_XAudio::submitSourceBuffer
        // @public 
        // @param	SourceXAudio * _source
        // @return	ITF::bbool
        //************************************
        bbool submitSourceBuffer(void *_source);
        //************************************
        // @name	ITF::SoundAdapter_XAudio::submitSourceBuffer
        // @public 
        // @param	SourceHandle _handle
        // @param	const XAUDIO2_BUFFER & _xaudioBuffer
        // @return	ITF::bbool
        //************************************
        bbool submitSourceBuffer(SourceHandle _handle, const u32 &_xaudioBuffer);
        //************************************
        // @name	ITF::SoundAdapter_XAudio::registerFlagsDone
        // @private 
        // @param	SourceXAudio * _source
        // @return	void
        //************************************
        void registerFlagsDone(u32 * _source);

#ifndef ITF_FINAL
        void getPerformanceDebugInfo(String &debugInfo, char paramDelimiter = '\n', char valueDelimiter = ':');
#endif

    private:
        //************************************
        // @name	ITF::SoundAdapter_WII::initInternal
        // @public 
        // @return	ITF::bbool
        //************************************
        bbool initInternal();


        void * m_axBuffer;

	};

} // namespace ITF


#endif //(ITF_X360) || defined (ITF_WIN32)
#endif //_ITF_SOUNDADAPTER_H_WII_
