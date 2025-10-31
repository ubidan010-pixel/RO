#ifndef _ITF_SOUNDADAPTER_H_XAUDIO_
#define _ITF_SOUNDADAPTER_H_XAUDIO_

#if defined (ITF_X360) || defined (ITF_WINDOWS)
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#include <xaudio2fx.h>

#include <audiodefs.h>

struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2VoiceCallback;
struct IXAudio2SubmixVoice;
struct IXAudio2Voice;
struct XAUDIO2_PERFORMANCE_DATA;
struct XAUDIO2_BUFFER;
struct XAUDIO2_BUFFER_WMA;
struct XAUDIO2FX_REVERB_PARAMETERS;
namespace ITF
{
    class SourceXAudio;
    class InstanceManager;

    class Bus 
    {
    public:
        Bus( const BusDef * _busdef)
            : m_busDefTemplate(_busdef)
            , m_voice(NULL)
            , m_processingStage(U32_INVALID)
            , m_timer(0.0f)
            , m_targetVolume(1.0f)
            , m_initialVolume(1.0f)
            , m_timeToFade(0.0f)
        {
            ITF_ASSERT(_busdef);
            m_filterType = _busdef->m_filterType;
            m_filterFrequency = _busdef->m_filterFrequency;
            m_filterQ = _busdef->m_filterQ;
        }

        ~Bus();

        ITF_INLINE f32 getTimeToFade() const { return m_timeToFade; }
        ITF_INLINE void setTimeToFade(f32 _time) { m_timeToFade = _time; }
        ITF_INLINE IXAudio2Voice * getVoice() const { return m_voice; }
        ITF_INLINE void setVoice(IXAudio2Voice* _voice) { m_voice = _voice; }
        ITF_INLINE const SafeArray<StringID>& getInputs() const { return m_inputs; }
        ITF_INLINE u32 getProcessingStage() const { return m_processingStage; }
        ITF_INLINE void setProcessingStage(u32 _stage ) { m_processingStage = _stage; }
        ITF_INLINE f32 getTimer() const { return m_timer; }
        ITF_INLINE void setTimer(f32 _timer) { m_timer = _timer; }
        ITF_INLINE f32 getTargetVolume() const { return m_targetVolume; }
        ITF_INLINE void setTargetVolume(f32 _volume ) { m_targetVolume = _volume; } 
        ITF_INLINE f32 getInitialVolume() const { return m_initialVolume; }
        ITF_INLINE void setInitialVolume(f32 _volume ) { m_initialVolume = _volume; }
        ITF_INLINE void addInput(StringID _input) { m_inputs.push_back(_input); }
        ITF_INLINE void setFilterType(FilterType _type) { m_filterType = _type; }
        ITF_INLINE void setFilterFrequency(f32 _freq) { m_filterFrequency = _freq; }
        ITF_INLINE void setFilterQ(f32 _q) { m_filterQ = _q; }
        ITF_INLINE FilterType getFilterType() const { return m_filterType; }
        ITF_INLINE f32 getFilterFrequency() const { return m_filterFrequency; }
        ITF_INLINE f32 getFilterQ() const { return m_filterQ; }


        ITF_INLINE StringID getName() const { return m_busDefTemplate->m_name; }
        ITF_INLINE const SafeArray<StringID> & getOutputs() const { return m_busDefTemplate->m_outputs; }
        ITF_INLINE Volume getVolume() const { return m_busDefTemplate->m_volume; }
        ITF_INLINE const SafeArray<SoundModifier*> & getModifiers() const { return m_busDefTemplate->m_modifiers; }
        ITF_INLINE bbool getHasReverb() const { return m_busDefTemplate->m_hasReverb; }
        ITF_INLINE bbool getReverbActive() const { return m_busDefTemplate->m_reverbActive; }
        ITF_INLINE ReverbPreset getReverbPreset() const { return m_busDefTemplate->m_reverbPreset; }


    private:
        const BusDef * m_busDefTemplate;


        IXAudio2Voice * m_voice;
        SafeArray<StringID> m_inputs;
        u32 m_processingStage;
        f32 m_timer;
        f32 m_targetVolume;
        f32 m_initialVolume;
        f32 m_timeToFade;

        FilterType m_filterType;
        f32 m_filterFrequency;
        f32 m_filterQ;
    };

    class SoundAdapter_XAudio : public SoundAdapter
	{
	public:
		SoundAdapter_XAudio      (   );
		~SoundAdapter_XAudio     (   );

        //ENGINE
		//************************************
		// @name	ITF::SoundAdapter_XAudio::init
		// @public 
		// @return	ITF::bbool
		//************************************
		virtual bbool init(bbool _audioCaptureAllowed);

        //************************************
        // @name	ITF::SoundAdapter_XAudio::isRunning
        // @public 
        // @return	ITF::bbool
        //************************************
        virtual bbool isRunning() const;
		//************************************
		// @name	ITF::SoundAdapter_XAudio::shutdown
		// @public 
		// @return	void
		//************************************
		virtual void shutdown();

        //RESOURCE LOADING
		//************************************
		// @name	ITF::SoundAdapter_XAudio::loadSound
		// @public 
		// @param	Sound * _sound
		// @param	const char * _path
		// @return	ITF::bbool
		//************************************
		virtual bbool loadSound(Sound * _sound, const char * _path);
		//************************************
		// @name	ITF::SoundAdapter_XAudio::unloadSound
		// @public 
		// @param	Sound * _sound
		// @return	void
		//************************************
		virtual void unloadSound(Sound * _sound);

        //SOURCE MANAGEMENT
		//************************************
		// @name	ITF::SoundAdapter_XAudio::createSource
		// @virtual private 
		// @param	const SourceInfo * _params
		// @return	ITF::SourceHandle
		//************************************
		virtual SourceHandle createSource(SourceInfo * _params);
        //************************************
        // @name	ITF::SoundAdapter_XAudio::setMasterVolume
        // @public 
        // @param	f32 _volume
        // @return	void
        //************************************
        virtual void setMasterVolume(f32 _volume);

        //DEBUG
        //************************************
        // @name	ITF::SoundAdapter_XAudio::toggleDebug
        // @public 
        // @return	void
        //************************************
        virtual void toggleDebug();
        //************************************
        // @name	ITF::SoundAdapter_XAudio::isDebug
        // @public 
        // @return	ITF::bbool
        //************************************
        virtual bbool isDebug();

        virtual f32 getSoundDuration(const Sound * _sound);

        ::IXAudio2*  getXAudio2() const {return m_xAudio2;}

#ifndef ITF_FINAL
        void getPerformanceDebugInfo(String &debugInfo, char paramDelimiter = '\n', char valueDelimiter = ':');
#endif

    private:
        //************************************
        // @name	ITF::SoundAdapter_XAudio::initInternal
        // @public 
        // @return	ITF::bbool
        //************************************
        bbool initInternal();
        //************************************
        // @name	ITF::SoundAdapter_XAudio::initReverbPresets
        // @public 
        // @return	void
        //************************************
//        void initReverbPresets();
        //************************************
        // @name	ITF::SoundAdapter_XAudio::createSource
        // @virtual private 
        // @param	const SourceInfo * _params
        // @param	IXAudio2VoiceCallback * _callback
        // @return	ITF::SourceHandle
        //************************************
        SourceHandle createSource(SourceInfo * _params, IXAudio2VoiceCallback * _callback);
        //************************************
        // @name	ITF::SoundAdapter_XAudio::createSubmixVoice
        // @private 
        // @param	Bus * _bus
        // @param	u32 _processingStage
        // @return	void
        //************************************
        void createSubmixVoice(Bus * _bus);
        //************************************
        // @name	ITF::SoundAdapter_XAudio::initBus
        // @private 
        // @param	Bus * _bus
        // @return	void
        //************************************
        void initBus( Bus * _bus );
        //************************************
        // @name	ITF::SoundAdapter_XAudio::destroySubmixVoice
        // @private 
        // @param	Bus * _bus
        // @return	void
        //************************************
        void destroySubmixVoice(Bus * _bus);
        //************************************
        // @name	ITF::SoundAdapter_XAudio::getReverbParams
        // @private 
        // @param	ReverbPreset _preset
        // @param	XAUDIO2FX_REVERB_PARAMETERS & _params
        // @return	void
        //************************************
//        XAUDIO2FX_REVERB_PARAMETERS * getReverbParams(ReverbPreset _preset);
        //************************************
        // @name	ITF::SoundAdapter_XAudio::printConfig
        // @private 
        // @return	void
        //************************************
        void printConfig();

         //************************************
        // @name	ITF::SoundAdapter_XAudio::destroy
        // @private 
        // @return	void
        //************************************
        void destroy();

        void printBusInfo( Bus * bus, u32 numTabs, String & _txt );

        ::IXAudio2 * m_xAudio2;
        IXAudio2MasteringVoice* m_masterVoice;
        XAUDIO2_PERFORMANCE_DATA * m_perf;
        SourceXAudio * m_sources;
//        SourceHandle m_currentMusic;
//        ITF_MAP<StringID,Bus*> m_busses;
        bool m_debug;
        u32 m_operationSet;

#if defined(ITF_WINDOWS) && defined(ITF_SUPPORT_VIDEOCAPTURE)
        class CaptureAPO*     m_pCaptureAPO;
#endif //ITF_WINDOWS && ITF_SUPPORT_VIDEOCAPTURE

//        XAUDIO2FX_REVERB_PARAMETERS m_reverbParams[ReverbPreset_COUNT];


    };

} // namespace ITF


#endif //(ITF_X360) || defined (ITF_WINDOWS)
#endif //_ITF_SOUNDADAPTER_H_XAUDIO_
