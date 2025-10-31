#ifndef _ITF_SOUNDADAPTER_H_CTR_
#define _ITF_SOUNDADAPTER_H_CTR_

#if defined (ITF_CTR)
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

    class SoundAdapter_CTR : public SoundAdapter
	{
	public:
		SoundAdapter_CTR      (   );
		~SoundAdapter_CTR     (   );

        //ENGINE
		//************************************
		// @name	ITF::SoundAdapter_CTR::init
		// @public 
		// @return	ITF::bbool
		//************************************
		virtual bbool init();

        //************************************
        // @name	ITF::SoundAdapter_CTR::isRunning
        // @public 
        // @return	ITF::bbool
        //************************************
        virtual bbool isRunning() const;
		//************************************
		// @name	ITF::SoundAdapter_CTR::shutdown
		// @public 
		// @return	void
		//************************************
		virtual void shutdown();
		//************************************
		// @name	ITF::SoundAdapter_CTR::update
		// @public 
		// @param	f32 _dt
		// @return	void
		//************************************
		virtual void update(f32 _dt);

        //RESOURCE LOADING
		//************************************
		// @name	ITF::SoundAdapter_CTR::loadSound
		// @public 
		// @param	Sound * _sound
		// @param	const char * _path
		// @return	ITF::bbool
		//************************************
		virtual bbool loadSound(Sound * _sound, const char * _path);
		//************************************
		// @name	ITF::SoundAdapter_CTR::unloadSound
		// @public 
		// @param	Sound * _sound
		// @return	void
		//************************************
		virtual void unloadSound(Sound * _sound);

        //SOURCE MANAGEMENT
		//************************************
		// @name	ITF::SoundAdapter_CTR::createSource
		// @virtual private 
		// @param	const SourceInfo * _params
		// @return	ITF::SourceHandle
		//************************************
		virtual SourceHandle createSource(const SourceInfo * _params);
		//************************************
		// @name	ITF::SoundAdapter_CTR::destroySource
		// @public 
		// @param	const SourceHandle _source
		// @return	void
		//************************************
		virtual void destroySource(const SourceHandle _source);
        //************************************
        // @name	ITF::SoundAdapter_CTR::emptySource
        // @public 
        // @param	const SourceHandle _source
        // @return	void
        //************************************
        virtual void emptySource(const SourceHandle _source);

        //SOUND CONTROL
		//************************************
		// @name	ITF::SoundAdapter_CTR::play
		// @public 
		// @param	const SourceHandle _source
		// @return	void
		//************************************
		virtual void play(const SourceHandle _source);
		//************************************
		// @name	ITF::SoundAdapter_CTR::stop
		// @public 
		// @param	const SourceHandle _source
		// @return	void
		//************************************
		virtual void stop(const SourceHandle _source);
        //************************************
        // @name	ITF::SoundAdapter_CTR::resume
        // @public 
        // @param	const SourceHandle _source
        // @return	void
        //************************************
        virtual void resume(const SourceHandle _source);
        //************************************
        // @name	ITF::SoundAdapter_CTR::getPlayingVoices
        // @public 
        // @param	SafeArray<SourceHandle> & _playingVoices
        // @return	void
        //************************************
        virtual void getPlayingVoices(SafeArray<SourceHandle> &_playingVoices);
        //************************************
        // @name	ITF::SoundAdapter_CTR::isPlaying
        // @public 
        // @param	const SourceHandle _source
        // @return	ITF::bbool
        //************************************
        virtual bbool isPlaying(const SourceHandle _source);
        //************************************
        // @name	ITF::SoundAdapter_CTR::isStreamingSource
        // @public 
        // @param	const SourceHandle _source
        // @return	ITF::bbool
        //************************************
        virtual bbool isStreamingSource(const SourceHandle _source);
		//************************************
		// @name	ITF::SoundAdapter_CTR::pushBuffer
		// @virtual public 
		// @param	const SourceHandle _source
		// @param	const SoundBuffer * _buffer
		// @return	void
		//************************************
		virtual void pushBuffer(const SourceHandle _source, const SoundBuffer * _buffer){};
		//************************************
		// @name	ITF::SoundAdapter_CTR::setLooping
		// @virtual public 
		// @param	const SourceHandle _source
		// @return	void
		//************************************
		virtual void setLooping(const SourceHandle _source){};

        //PARAMETER CONTROL
        //************************************
        // @name	ITF::SoundAdapter_CTR::setVolume
        // @public 
        // @param	const SourceHandle _source
        // @param	const f32 _volume
        // @return	void
        //************************************
        virtual void setVolume(const SourceHandle _source, const f32 _volume);
        //************************************
        // @name	ITF::SoundAdapter_CTR::getVolume
        // @public 
        // @param	const SourceHandle _source
        // @return	ITF::f32
        //************************************
        virtual f32 getVolume(const SourceHandle _source) const;
        //************************************
        // @name	ITF::SoundAdapter_CTR::setOutputVolumes
        // @public 
        // @param	const SourceHandle _source
        // @param	f32 * _volumes
        // @param	u32 _sourceChannels
        // @param	u32 _destChannels
        // @return	void
        //************************************
        virtual void setOutputVolumes(const SourceHandle _source, f32 * _volumes, u32 _sourceChannels, u32 _destChannels);
        //************************************
        // @name	ITF::SoundAdapter_CTR::setMasterVolume
        // @public 
        // @param	f32 _volume
        // @return	void
        //************************************
        virtual void setMasterVolume(f32 _volume);
        //************************************
        // @name	ITF::SoundAdapter_CTR::setPitch
        // @public 
        // @param	const SourceHandle _source
        // @param	f32 _pitch
        // @return	void
        //************************************
        virtual void setPitch(const SourceHandle _source, f32 _pitch);
        //************************************
        // @name	ITF::SoundAdapter_CTR::setFilter
        // @public 
        // @param	const SourceHandle _source
        // @param	FilterType _filterType
        // @param	f32 _cutoff
        // @param	f32 _oneOverQ
        // @return	void
        //************************************
        virtual void setFilter(const SourceHandle _source,FilterType _filterType, f32 _cutoff, f32 _oneOverQ);
        //************************************
        // @name	ITF::SoundAdapter::setBusVolume
        // @virtual public 
        // @param	StringID _bus
        // @param	Volume _volume
        // @return	void
        //************************************
        virtual void setBusVolume(StringID _bus, Volume _volume, f32 _time);
        //************************************
        // @name	ITF::SoundAdapter::setBusFilter
        // @virtual public 
        // @param	StringID _bus
        // @param	FilterType _filterType
        // @param	f32 _cutoff
        // @param	f32 _oneOverQ
        // @return	void
        //************************************
        virtual void setBusFilter(StringID _bus,FilterType _filterType, f32 _cutoff, f32 _oneOverQ);
        //************************************
        // @name	ITF::SoundAdapter::enableBusReverb
        // @virtual public 
        // @param	StringID _bus
        // @param	bbool _enable
        // @return	void
        //************************************
        virtual void enableBusReverb(StringID _bus,bbool _enable);
        //************************************
        // @name	ITF::SoundAdapter::setBusReverbPreset
        // @public 
        // @param	StringID _bus
        // @param	ReverbPreset _preset
        // @return	void
        //************************************
        virtual void setBusReverbPreset(StringID _bus,ReverbPreset _preset);

        //DEBUG
        //************************************
        // @name	ITF::SoundAdapter_CTR::toggleDebug
        // @public 
        // @return	void
        //************************************
        virtual void toggleDebug();
        //************************************
        // @name	ITF::SoundAdapter_CTR::isDebug
        // @public 
        // @return	ITF::bbool
        //************************************
        virtual bbool isDebug();

        //STREAMING
        //************************************
        // @name	ITF::SoundAdapter_CTR::openStreamFile
        // @public 
        // @param	const String & _filename
        // @return	SoundStreamerDataSource *
        //************************************
        virtual SoundStreamerDataSource *openStreamFile(const String &_filename);
        //************************************
        // @name	ITF::SoundAdapter_CTR::createStreamingSourceHandle
        // @public 
        // @param	SoundStreamerDataSource * _dataSource
        // @return	ITF::SourceHandle
        //************************************
        virtual SourceHandle createStreamingSourceHandle(SoundStreamerDataSource *_dataSource);
        //************************************
        // @name	ITF::SoundAdapter_CTR::createStream
        // @public 
        // @param	SoundStreamerDataSource * _dataSource
        // @param	SourceHandle _soundSourceHandle
        // @return	ITF::SoundStreamHandle
        //************************************
        virtual SoundStreamHandle createStream(SoundStreamerDataSource* _dataSource, SourceHandle _soundSourceHandle);
        //************************************
        // @name	ITF::SoundAdapter_CTR::addDataSourceToStream
        // @public 
        // @param	SoundStreamHandle _stream
        // @param	SoundStreamerDataSource * _dataSource
        // @param	bbool _isLooping
        // @param	SoundStreamExhaustedCallback _reachEndOfStreamCallback
        // @return	void
        //************************************
        virtual void addDataSourceToStream(SoundStreamHandle _stream, SoundStreamerDataSource* _dataSource, bbool _isLooping, SoundStreamExhaustedCallback _reachEndOfStreamCallback);
        //************************************
        // @name	ITF::SoundAdapter_CTR::rewindDataSource
        // @public 
        // @param	SoundStreamHandle _stream
        // @param	SoundStreamerDataSource * _dataSource
        // @return	void
        //************************************
        virtual void rewindDataSource(SoundStreamHandle _stream, SoundStreamerDataSource* _dataSource);
        //************************************
        // @name	ITF::SoundAdapter_CTR::cancelStream
        // @public 
        // @param	SoundStreamHandle _handle
        // @return	ITF::bbool
        //************************************
        virtual bbool cancelStream(SoundStreamHandle _handle);  

		//************************************
        // @name	ITF::SoundAdapter::getSoundDuration
        // @public 
        // @param	const Sound * _sound
        // @return	ITF::f32 duration of sound
        //************************************
		virtual f32 getSoundDuration(const Sound * _sound) {return 0.0f;};

        virtual void terminateStreamer() {};
        virtual void resetBusses() {};
        virtual void pause(const SourceHandle _source) {};
        virtual SourceHandle createSource(SourceInfo * _params){ };



    private:
        //************************************
        // @name	ITF::SoundAdapter_CTR::initInternal
        // @public 
        // @return	ITF::bbool
        //************************************
        bbool initInternal();
        //************************************
        // @name	ITF::SoundAdapter_CTR::initReverbPresets
        // @public 
        // @return	void
        //************************************
        void initReverbPresets();
        //************************************
        // @name	ITF::SoundAdapter_CTR::createSubmixVoice
        // @private 
        // @param	Bus * _bus
        // @param	u32 _processingStage
        // @return	void
        //************************************
        void createSubmixVoice(Bus * _bus);
        //************************************
        // @name	ITF::SoundAdapter_CTR::initBus
        // @private 
        // @param	Bus * _bus
        // @return	void
        //************************************
        void initBus( Bus * _bus );
        //************************************
        // @name	ITF::SoundAdapter_CTR::destroySubmixVoice
        // @private 
        // @param	Bus * _bus
        // @return	void
        //************************************
        void destroySubmixVoice(Bus * _bus);
        //************************************
        // @name	ITF::SoundAdapter_CTR::acquireSource
        // @private 
        // @return	ITF::SourceHandle
        //************************************
        SourceHandle acquireSource();
        ITF_MAP<StringID,Bus*> m_busses;
	};

} // namespace ITF


#endif //(ITF_X360) || defined (ITF_WIN32)
#endif //_ITF_SOUNDADAPTER_H_CTR_