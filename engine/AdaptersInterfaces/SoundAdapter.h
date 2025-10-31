#ifndef _ITF_SOUND_ADAPTER_
#define _ITF_SOUND_ADAPTER_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

#ifndef _ITF_VOLUME_
#include "core/sound/volume.h"
#endif //ITF_VOLUME_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#if defined ( ITF_WII )
#define DEFAULT_SAMPLING_RATE 32000
#else
#define DEFAULT_SAMPLING_RATE 48000
#endif 

namespace ITF
{
    class InstanceManager;
	class Sound;
    class SoundModifier;
    class SoftwareBus;
    class SoundConfig_Template;


    enum ReverbPreset
    {
        ReverbPreset_DEFAULT,
        ReverbPreset_GENERIC,
        ReverbPreset_PADDEDCELL,
        ReverbPreset_ROOM,
        ReverbPreset_BATHROOM,
        ReverbPreset_LIVINGROOM,
        ReverbPreset_STONEROOM,
        ReverbPreset_AUDITORIUM,
        ReverbPreset_CONCERTHALL,
        ReverbPreset_CAVE,
        ReverbPreset_ARENA,
        ReverbPreset_HANGAR,
        ReverbPreset_CARPETEDHALLWAY,
        ReverbPreset_HALLWAY,
        ReverbPreset_STONECORRIDOR,
        ReverbPreset_ALLEY,
        ReverbPreset_FOREST,
        ReverbPreset_CITY,
        ReverbPreset_MOUNTAINS,
        ReverbPreset_QUARRY,
        ReverbPreset_PLAIN,
        ReverbPreset_PARKINGLOT,
        ReverbPreset_SEWERPIPE,
        ReverbPreset_UNDERWATER,
        ReverbPreset_SMALLROOM,
        ReverbPreset_MEDIUMROOM,
        ReverbPreset_LARGEROOM,
        ReverbPreset_MEDIUMHALL,
        ReverbPreset_LARGEHALL,
        ReverbPreset_PLATE,
        ReverbPreset_COUNT,
        ReverbPreset_CUSTOM,
        ReverbPreset_NONE,
        ENUM_FORCE_SIZE_32(ReverbPreset)        
    };

    enum FilterType
    {
        FilterType_LowPass,
        FilterType_BandPass,
        FilterType_HighPass,
        FilterType_Notch,
        FilterType_None,
        FilterType_Current,
        FilterType_Count,
        ENUM_FORCE_SIZE_32(FilterType)
    };

    enum SpeakerChannel
    {
        SpeakerChannel_Left,
        SpeakerChannel_Right,
        SpeakerChannel_FrontCenter,
        SpeakerChannel_LFE,
        SpeakerChannel_RearLeft,
        SpeakerChannel_RearRight,
        ENUM_FORCE_SIZE_32(SpeakerChannel)
    };

   
    struct BusDef
    {
        DECLARE_SERIALIZE()

        BusDef()
            : m_filterType(FilterType_None)
            , m_filterFrequency(1.0f)
            , m_filterQ(1.0f)
            , m_hasReverb(bfalse)
            , m_reverbActive(bfalse)
            , m_reverbPreset(ReverbPreset(U32_INVALID))
            , m_pauseSensitive(btrue)
        {
        }
        BusDef(const BusDef * _busdef)
            : m_name(_busdef->m_name)
            , m_outputs(_busdef->m_outputs)
            , m_volume(_busdef->m_volume)
            , m_filterType(_busdef->m_filterType)
            , m_filterFrequency(_busdef->m_filterFrequency)
            , m_filterQ(_busdef->m_filterQ)
            , m_modifiers(_busdef->m_modifiers)
            , m_hasReverb(_busdef->m_hasReverb)
            , m_reverbActive(_busdef->m_reverbActive)
            , m_reverbPreset(_busdef->m_reverbPreset)
            , m_pauseSensitive(_busdef->m_pauseSensitive)
        {

        }
        bool operator==(const BusDef & _other) const { return m_name == _other.m_name; }

        const StringID& getVolumeRtpc() const { return m_volumeRtpc; }
        const StringID& getPitchRtpc() const { return m_pitchRtpc; }
        const StringID& getLpfRtpc() const { return m_lpfRtpc; }
        const StringID& getHpfRtpc() const { return m_hpfRtpc; }

        StringID    m_name;
        SafeArray<StringID> m_outputs;
        Volume      m_volume;
        FilterType  m_filterType;
        f32         m_filterFrequency;
        f32         m_filterQ;
        SafeArray<SoundModifier*> m_modifiers;
        bbool           m_hasReverb;
        bbool           m_reverbActive;
        ReverbPreset    m_reverbPreset;
        bbool           m_pauseSensitive;
        StringID m_volumeRtpc;
        StringID m_pitchRtpc;
	    StringID m_lpfRtpc;
	    StringID m_hpfRtpc;
   };

    struct LimiterDef
    {
        DECLARE_SERIALIZE()
        LimiterDef() : m_maxInstances(U32_INVALID), m_mode(0){}
        LimiterDef(StringID _name, u32 _mode, u32 _maxInstances) 
            : m_name(_name)
            , m_mode(_mode)
            , m_maxInstances(_maxInstances)
        {}
        StringID m_name;
        u32 m_maxInstances;
        u32 m_mode;
    };

    struct BusMix
    {
        DECLARE_SERIALIZE();

        BusMix() 
            : m_priority(1)
            , m_duration(-1.f)
            , m_fadeIn(1.0f)
            , m_fadeOut(1.0f)
        {

        }
        StringID m_name;
        u32 m_priority;
        f32 m_duration;
        f32 m_fadeIn;
        f32 m_fadeOut;
        ObjectRef m_ref;
        ITF_VECTOR<BusDef> m_busDefs;
    };


	class SoundBuffer
	{
	public:
		ITF_INLINE SoundBuffer() : m_size(0), m_data(NULL), m_status(BufferStatus_NotAllocated){}
		enum BufferStatus
		{
			BufferStatus_NotAllocated,
			BufferStatus_Invalid,
            BufferStatus_Valid,
            ENUM_FORCE_SIZE_32(0)
		};
		//************************************
		// @name	ITF::SoundBuffer::setSize
		// @public 
		// @param	u32 _size
		// @return	ITF_INLINE void
		//************************************
		ITF_INLINE void setSize( u32 _size)
		{
			ITF_ASSERT(m_size == 0);
			ITF_ASSERT(m_status == BufferStatus_NotAllocated);
			m_size = _size;
		}
		//************************************
		// @name	ITF::SoundBuffer::allocateMem
		// @public 
		// @return	ITF_INLINE void
		//************************************
		ITF_INLINE void allocateMem() 
		{
			ITF_ASSERT(m_data == NULL);
			ITF_ASSERT(m_status == BufferStatus_NotAllocated);
			ITF_ASSERT(m_size > 0);
			m_data = malloc(m_size);
			ITF_ASSERT(m_data);
		}
		//************************************
		// @name	ITF::SoundBuffer::freeMem
		// @public 
		// @return	ITF_INLINE void
		//************************************
		ITF_INLINE void freeMem()
		{
			ITF_ASSERT(m_data);
			ITF_ASSERT(m_status == BufferStatus_Invalid);
			free(m_data);
			m_data = NULL;
			m_size = 0;
			m_status = BufferStatus_NotAllocated;
		}
		//************************************
		// @name	ITF::SoundBuffer::setData
		// @public 
		// @param	void * _data
		// @return	ITF_INLINE void
		//************************************
		ITF_INLINE void setData(void * _data) 
		{
			ITF_ASSERT(m_data == NULL);
			ITF_ASSERT(m_status == BufferStatus_NotAllocated);
			m_data = _data;
		}
		//************************************
		// @name	ITF::SoundBuffer::invalidate
		// @public 
		// @return	ITF_INLINE void
		//************************************
		ITF_INLINE void invalidate() 
		{
			ITF_ASSERT(m_status != BufferStatus_NotAllocated);
			m_status = BufferStatus_Invalid;
		} 
		//************************************
		// @name	ITF::SoundBuffer::validate
		// @public 
		// @return	ITF_INLINE void
		//************************************
		ITF_INLINE void validate()
		{
			ITF_ASSERT(m_status != BufferStatus_NotAllocated);
			m_status = BufferStatus_Valid;
		}
	private:
		u32 m_size;
		void * m_data;
		BufferStatus m_status;
	};

	typedef u32 SourceHandle;
    static const SourceHandle InvalidSourceHandle = 0xFFFFFFFF;
    static const SourceHandle InvalidSubHandle = 0xFFFFFFFF;

	enum SoundDataFormat
	{
		SoundDataFormat_Uncompressed,
        SoundDataFormat_Compressed,
        ENUM_FORCE_SIZE_32(SoundDataFormat)
	};

    typedef void*(SoundCallback)(u32 type, void * _context);

    struct SourceInfo
    {
        const Sound * m_sound;
        bbool m_loop;
        u32 m_numChannels;
        f32 m_volume;
        f32 m_pitch;
        bbool m_isMusic;
        bbool m_usePitch;
        bbool m_useFilter;
        StringID m_bus;
        SourceInfo()
            : m_sound(NULL)
            , m_loop(bfalse)
            , m_numChannels(1)
            , m_volume(1.0f)
            , m_pitch(1.0f)
            , m_isMusic(bfalse)
            , m_usePitch(btrue)
            , m_useFilter(bfalse)
        {

        }
        SoundCallback m_callback;
    };




    class SoundAdapter:public TemplateSingleton<SoundAdapter>
	{
	public:
        SoundAdapter();
		virtual ~SoundAdapter();

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn void init()
		/// @brief  initialises audio. 
		////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bbool init(bbool _audioCaptureAllowed);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn void init()
		/// @brief  returns true if engine creation went well 
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual bbool isRunning() const = 0;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn void shutdown()
		/// @brief  shuts down audio. 
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void shutdown();

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn void loadSound(Sound * _sound, const char * _path)
		/// @brief loads a Sound resource 
		/// @param [in,out] _sound   [in] _path 
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual bbool loadSound(Sound * _sound, const char * _path) = 0;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn void unloadSound(Sound * _sound)
		/// @brief unloads a Sound resource 
		/// @param [in,out] _sound 
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void unloadSound(Sound * _sound) = 0;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn SourceVoice * createSource( Sound * _sound)
		/// @brief creates a source voice from a Sound resource 
		/// @param [in] _sound   
		/// @return pointer to the newly created voice, or NULL if unable to create voice
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual SourceHandle createSource(SourceInfo * _params) = 0;

		//************************************
		// @name	ITF::SoundAdapter::play
		// @virtual public 
		// @param	const SourceHandle _source
		// @return	void
		//************************************
		void play(const SourceHandle _source);
		//************************************
		// @name	ITF::SoundAdapter::stop
		// @virtual public 
		// @param	const SourceHandle _source
		// @return	void
		//************************************
		void stop(const SourceHandle _source);
        //************************************
        // @name	ITF::SoundAdapter::pause
        // @virtual public 
        // @param	const SourceHandle _source
        // @return	void
        //************************************
        void pause(const SourceHandle _source);
        //************************************
        // @name	ITF::SoundAdapter::resume
        // @virtual public 
        // @param	const SourceHandle _source
        // @return	void
        //************************************
        void resume(const SourceHandle _source);
        //************************************
        // @name	ITF::SoundAdapter::getPlayingVoices
        // @virtual public 
        // @param	SafeArray<SourceHandle> & _playingVoices
        // @return	void
        //************************************
        void getPlayingVoices(SafeArray<SourceHandle> &_playingVoices);
        //************************************
        // @name	ITF::SoundAdapter::getPauseSensitivePlayingVoices
        // @virtual public 
        // @param	SafeArray<SourceHandle> & _playingVoices
        // @return	void
        //************************************
        void getPauseSensitivePlayingVoices(SafeArray<SourceHandle> &_playingVoices);
		//************************************
		// @name	ITF::SoundAdapter::setVolume
		// @virtual public 
		// @param	const SourceHandle _source
		// @param	const f32 _volume
		// @return	void
		//************************************
		void setVolume(const SourceHandle _source, const f32 _volume);
        //************************************
        // @name	ITF::SoundAdapter::getVolume
        // @virtual public 
        // @param	const SourceHandle _source
        // @return	ITF::f32
        //************************************
        f32  getVolume(const SourceHandle _source) const; 
        //************************************
        // @name	ITF::SoundAdapter::setPan
        // @virtual public 
        // @param	const SourceHandle _source
        // @param	f32 _pan
        // @return	void
        //************************************
        void setPan(const SourceHandle _source, const f32 _pan);
        //************************************
        // @name	ITF::SoundAdapter::setPitch
        // @virtual public 
        // @param	const SourceHandle _source
        // @param	f32 _pitch
        // @return	void
        //************************************
        void setPitch(const SourceHandle _source, f32 _pitch);
        
        //************************************
        // @name	ITF::SoundAdapter::setBusVolume
        // @virtual public 
        // @param	StringID _bus
        // @param	Volume _volume
        // @return	void
        //************************************
        void setBusVolume(StringID _bus, Volume _volume, f32 _time);
        //************************************
        // @name	ITF::SoundAdapter::setBusFilter
        // @virtual public 
        // @param	StringID _bus
        // @param	FilterType _filterType
        // @param	f32 _cutoff
        // @param	f32 _oneOverQ
        // @return	void
        //************************************
        void setBusFilter(StringID _bus,FilterType _filterType, f32 _cutoff, f32 _oneOverQ);
        //************************************
        // @name	ITF::SoundAdapter::getBusFilter
        // @virtual public 
        // @param	StringID _bus
        // @param	FilterType & _filterType
        // @param	f32 & _cutoff
        // @param	f32 & _oneOverQ
        // @return	void
        //************************************
        void getBusFilter(StringID _bus,FilterType &_filterType, f32 &_cutoff, f32 &_oneOverQ);
        //************************************
        // @name	ITF::SoundAdapter::isPlaying
        // @virtual public 
        // @param	const SourceHandle _source
        // @return	ITF::bbool
        //************************************
        //************************************
        // @name	ITF::SoundAdapter::enableBusReverb
        // @virtual public 
        // @param	StringID _bus
        // @param	bbool _enable
        // @return	void
        //************************************
        void enableBusReverb(StringID _bus,bbool _enable);
        //************************************
        // @name	ITF::SoundAdapter::setBusReverbPreset
        // @public 
        // @param	StringID _bus
        // @param	ReverbPreset _preset
        // @return	void
        //************************************
        void setBusReverbPreset(StringID _bus,ReverbPreset _preset);
        //************************************
        // @name	ITF::SoundAdapter::resetBusses
        // @virtual public 
        // @return	void
        //************************************
        void resetBusses();

        bbool isPlaying(const SourceHandle _source);
        //************************************
        // @name	ITF::SoundAdapter::setMasterVolume
        // @virtual public 
        // @param	f32 _volume
        // @return	void
        //************************************
        void setMasterVolume( f32 _linearVolume );
        //************************************
        // @name	ITF::SoundAdapter::isValidBus
        // @public 
        // @param	StringID _bus
        // @return	ITF::bbool
        //************************************
        bbool isValidBus(StringID _bus) const ;
        //************************************
        // @name	ITF::SoundAdapter::isValidLimitCategory
        // @public 
        // @param	StringID _categor
        // @return	ITF::bbool
        //************************************
        bbool isValidLimitCategory(StringID _category) const;


        //************************************
        // @name	ITF::SoundAdapter::toggleDebug
        // @virtual public 
        // @return	void
        //************************************
        virtual void toggleDebug() = 0;
        //************************************
        // @name	ITF::SoundAdapter::isDebug
        // @virtual public 
        // @return	ITF::bbool
        //************************************
        virtual bbool isDebug() = 0;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn void destroySource(SourceVoice * _source)
		/// @brief destroys a source voice 
		/// @param [in] _source   
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void destroySource(const SourceHandle _source);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn void update(f32 _dt)
		/// @brief audio update
		/// @param [in] _dt  
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void update(f32 _dt);

        const SoundConfig_Template * getConfig() const { return m_config; }

        //************************************
        // @name	ITF::SoundAdapter::getSoundDuration
        // @public 
        // @param	const Sound * _sound
        // @return	ITF::f32 duration of sound
        //************************************
        virtual f32 getSoundDuration(const Sound * _sound) = 0;

        SoftwareBus * getBus( const StringID _name );

        ITF_INLINE InstanceManager * getInstanceManager() { return m_instanceManager; }

#ifndef ITF_FINAL
        //************************************
        // @name	ITF::SoundAdapter::getPerformanceData
        // @public 
        // @param	String &debugInfo
        //************************************
        virtual void getPerformanceDebugInfo(String &debugInfo, char paramDelimiter = '\n', char valueDelimiter = ':') = 0;
#endif

    protected:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        const SoundConfig_Template * m_config;
        bbool       m_bAudioCaptureAllowed;
        InstanceManager * m_instanceManager;
        ITF_MAP<StringID, SoftwareBus*> m_buses;

	};

#if defined(ITF_SUPPORT_RAKI)
#define SOUND_ADAPTER SoundAdapter::getptr()
#endif


} // namespace ITF

#endif //_ITF_SOUND_ADAPTER_
