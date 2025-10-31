#ifndef _ITF_SOUNDDESCRIPTOR_H_
#define _ITF_SOUNDDESCRIPTOR_H_



#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_SOUNDMODIFIER_H_
#include "engine/sound/SoundModifier.h"
#endif //_ITF_SOUNDMODIFIER_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"  // included because we have a vector of resourceIDs as a member
#endif //_ITF_RESOURCE_H_


namespace ITF
{


    ///////////////////////////////////////////////////////////////////////////////////////////
    enum PlayMode
    {
        PlayMode_PlayFirst,
        PlayMode_Random,
        PlayMode_RandomRememberLast,
        PlayMode_RandomSequence,
        PlayMode_Sequence,
        PlayMode_Input,
        PlayMode_Count,
        ENUM_FORCE_SIZE_32(PlayMode)
    };
    class InstanceLimiter;
    class SoundStreamerDataSource;
    class SoundInstance;
    class SoundModifer;

    


    struct SoundParams
    {
        DECLARE_SERIALIZE()
        //DECLARE_VALIDATE_OBJECT()
        SoundParams() : 
        m_numChannels(1)
            , m_loop(bfalse)
            , m_playMode(PlayMode_PlayFirst)
            , m_randomPitchMin(1.0f)
            , m_randomPitchMax(1.0f)
            , m_fadeInTime(0.0f)
            , m_fadeOutTime(0.0f)
            , m_filterFrequency(0.0f)
            , m_filterType(FilterType_None)
            , m_filterQ(1.0f)
            , m_delay(0.0f)
            , m_randomDelay(0.0f)
            , m_metronomeType(U32_INVALID)
            , m_playOnNext(U32_INVALID)

        {
            m_randomVolMin.setratio(1.0f);
            m_randomVolMax.setratio(1.0f);
        }

        ~SoundParams()
        {
            for (u32 i = 0; i < m_modifiers.size(); ++i)
            {
                SF_DEL(m_modifiers[i]);
            }
            m_modifiers.clear();
        }

        //************************************
        // @name	ITF::SoundParams::getNumChannels
        // @public 
        // @return	ITF_INLINE u32
        //************************************
        ITF_INLINE u32          getNumChannels() const { return m_numChannels; }
        //************************************
        // @name	ITF::SoundParams::getLoop
        // @public 
        // @return	ITF_INLINE bbool
        //************************************
        ITF_INLINE bbool        getLoop() const { return m_loop; }
        //************************************
        // @name	ITF::SoundParams::getPlayMode
        // @public 
        // @return	ITF_INLINE PlayMode
        //************************************
        ITF_INLINE PlayMode     getPlayMode() const { return m_playMode; }
        //************************************
        // @name	ITF::SoundParams::getRandomVolMin
        // @public 
        // @return	ITF_INLINE Volume
        //************************************
        ITF_INLINE Volume       getRandomVolMin() const { return m_randomVolMin; }
        //************************************
        // @name	ITF::SoundParams::getRandomVolMax
        // @public 
        // @return	ITF_INLINE Volume
        //************************************
        ITF_INLINE Volume       getRandomVolMax() const { return m_randomVolMax; }
        //************************************
        // @name	ITF::SoundParams::getRandomPitchMin
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32          getRandomPitchMin() const { return m_randomPitchMin; }
        //************************************
        // @name	ITF::SoundParams::getRandomPitchMax
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32          getRandomPitchMax() const { return m_randomPitchMax; }
        //************************************
        // @name	ITF::SoundParams::getFadeInTime
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32          getFadeInTime() const { return m_fadeInTime; }
        //************************************
        // @name	ITF::SoundParams::getFadeOutTime
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32          getFadeOutTime() const { return m_fadeOutTime; }
        //************************************
        // @name	ITF::SoundParams::getFilterFrequency
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32          getFilterFrequency() const { return m_filterFrequency; }
        //************************************
        // @name	ITF::SoundParams::getFilterType
        // @public 
        // @return	ITF_INLINE FilterType
        //************************************
        ITF_INLINE FilterType   getFilterType() const { return m_filterType; }
        //************************************
        // @name	ITF::SoundParams::getFilterQ
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32          getFilterQ() const { return m_filterQ; }
        //************************************
        // @name	ITF::SoundParams::getPlayModeInput
        // @public 
        // @return	ITF_INLINE StringID
        //************************************
        ITF_INLINE StringID     getPlayModeInput() const { return m_playModeInput; }
        ITF_INLINE f32          getDelay() const { return m_delay; }
        ITF_INLINE f32          getRandomDelay() const { return m_randomDelay; }

        ITF_INLINE const u32    getMetronomeType() const { return m_metronomeType; }
        ITF_INLINE const u32    getPlayOnNext() const { return m_playOnNext; }

        ITF_INLINE const SafeArray<SoundModifier*> & getModifiers() const { return m_modifiers; }

        bbool usePitch() const;
        bbool useFilter() const;

        u32 m_numChannels;
        bbool m_loop;
        PlayMode m_playMode;
        StringID m_playModeInput;
        Volume m_randomVolMin;
        Volume m_randomVolMax;
        f32 m_randomPitchMin;
        f32 m_randomPitchMax;
        f32 m_fadeInTime;
        f32 m_fadeOutTime;
        f32 m_filterFrequency;
        FilterType m_filterType;
        f32 m_filterQ;
        f32 m_delay;
        f32 m_randomDelay;
        SafeArray<SoundModifier*> m_modifiers;
        u32 m_metronomeType;  // define on which metronome this sound should be synchronized
        u32 m_playOnNext; // define at which time this sound should be synchronized (1 bar = 24)
    };

    class SoundDescriptor_Template
    {
        DECLARE_SERIALIZE();
        SoundDescriptor_Template() 
        : m_isStream(bfalse)
        , m_limitMode(0)
        , m_maxInstances(U32_INVALID)
        {
            m_volume.setratio(1.0f);
			m_resourceList.clear();
            m_files.clear();
            m_wwiseEventGuid.invalidate();
        }

        ~SoundDescriptor_Template();

        void onTemplateLoaded();

		void addToResourceGroup(ResourceGroup* _resourceGroup);
        void removeFromResourceGroup(ResourceGroup* _resourceGroup);
            
        //************************************
        // @name	ITF::SoundDescriptor::getVolume
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32      getVolume() const { return m_volume.ratio(); }
        //************************************
        // @name	ITF::SoundDescriptor::GetName
        // @public 
        // @return	ITF_INLINE StringID
        //************************************
        ITF_INLINE StringID GetName() const { return m_name; }
        //************************************
        // @name	ITF::SoundDescriptor::getRandomVolMin
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32      getRandomVolMin() const { return m_params.getRandomVolMin().ratio(); }
        //************************************
        // @name	ITF::SoundDescriptor::getRandomVolMax
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32      getRandomVolMax() const { return m_params.getRandomVolMax().ratio(); }
        //************************************
        // @name	ITF::SoundDescriptor::getRandomPitchMin
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32      getRandomPitchMin() const { return m_params.getRandomPitchMin(); }
        //************************************
        // @name	ITF::SoundDescriptor::getRandomPitchMax
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32      getRandomPitchMax() const { return m_params.getRandomPitchMax(); }
        //************************************
        // @name	ITF::SoundDescriptor::getFadeInTime
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32      getFadeInTime() const { return m_params.getFadeInTime(); }
        //************************************
        // @name	ITF::SoundDescriptor::getFadeOutTime
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32      getFadeOutTime() const { return m_params.getFadeOutTime(); }
        //************************************
        // @name	ITF::SoundDescriptor::getFiles
        // @public 
        // @return	ITF_INLINE const ITF_VECTOR<String> &
        //************************************
        ITF_INLINE u32      getFileCount() const {return m_files.size();}
        //************************************
        // @name	ITF::SoundDescriptor::getLoop
        // @public 
        // @return	ITF_INLINE bbool
        //************************************
        ITF_INLINE bbool    getLoop() const { return m_params.getLoop(); }
        //************************************
        // @name	ITF::SoundDescriptor::getNumChannels
        // @public 
        // @return	ITF_INLINE u32
        //************************************
        ITF_INLINE u32      getNumChannels() const { return m_params.getNumChannels(); }
        //************************************
        // @name	ITF::SoundDescriptor::getPlayMode
        // @public 
        // @return	ITF_INLINE PlayMode
        //************************************
        ITF_INLINE PlayMode getPlayMode() const { return m_params.getPlayMode(); }
        //************************************
        // @name	ITF::SoundDescriptor::getLimitCategory
        // @public 
        // @return	ITF_INLINE StringID
        //************************************
        ITF_INLINE StringID getLimitCategory() const { return m_limitCategory; }
        //************************************
        // @name	ITF::SoundDescriptor::setLimitCategory
        // @public 
        // @param	StringID _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void     setLimitCategory(StringID _val) { m_limitCategory = _val; }
        //************************************
        // @name	ITF::SoundDescriptor::isMusic
        // @public 
        // @return	ITF_INLINE bbool
        //************************************
        ITF_INLINE bbool    isMusic() const { return m_isStream; }
        //************************************
        // @name	ITF::SoundDescriptor::usePitch
        // @public 
        // @return	ITF_INLINE bbool
        //************************************
        ITF_INLINE bbool    usePitch() const { return m_params.usePitch(); }
        //************************************
        // @name	ITF::SoundDescriptor::useFilter
        // @public 
        // @return	ITF_INLINE bbool
        //************************************
        ITF_INLINE bbool    useFilter() const { return m_params.useFilter(); }
        //************************************
        // @name	ITF::SoundDescriptor::getParams
        // @public 
        // @return	SoundParams *
        //************************************
        const SoundParams * getParams() const { return &m_params; }
        //************************************
        // @name	ITF::SoundDescriptor::getCategory
        // @public 
        // @return	ITF_INLINE StringID
        //************************************
        ITF_INLINE StringID getCategory() const { return m_category; }
        //************************************
        // @name	ITF::SoundDescriptor::getPlayModeInput
        // @public 
        // @return	ITF_INLINE StringID
        //************************************
        ITF_INLINE StringID getPlayModeInput() const { return m_params.getPlayModeInput(); }

        ITF_INLINE const ITF_VECTOR<Path> &getFiles() const { return m_files; }
        ITF_INLINE f32          getDelay() const { return m_params.getDelay(); }
        ITF_INLINE f32          getRandomDelay() const { return m_params.getRandomDelay(); }
        const StringID& getWwiseEventGuid() const { return m_wwiseEventGuid; }
        const u32 getSoundEventId() const { return m_soundEventID; }


        ITF_INLINE LimiterDef getLimiter() const { return LimiterDef(m_name,m_limitMode,m_maxInstances); }
        // Special edition only
        ITF_INLINE void         addFile(Path       _file) { m_files.push_back(_file); }
        ITF_INLINE void         setName(StringID   _name) { m_name = _name; }

		ITF_INLINE const ITF_VECTOR<ResourceID>& getResourceList() const { return m_resourceList; }

        // For sequence use
        void                    setFullParams(const SoundParams & _params );
        void                    setVolume(const Volume & _volume) { m_volume = _volume; }
        void                    setCategory(const StringID & _category) { m_category = _category; }
        void                    setIsStream(bbool _isStream) { m_isStream = _isStream; }
        void                    setWwiseEventGUID(const StringID& _value) { m_wwiseEventGuid = _value; }


    private:

        ///////////////////////////////////////////////////////////////////////////////////////////
        ITF_VECTOR<Path> m_files;
        StringID    m_name;
        Volume      m_volume;
        StringID    m_category;
        StringID    m_limitCategory;
        SoundParams m_params;
        bbool       m_isStream;
        u32         m_limitMode;
        u32         m_maxInstances;
		ITF_VECTOR<ResourceID> m_resourceList;
        StringID m_wwiseEventGuid;
        u32      m_soundEventID = 0;

    };

    class SoundDescriptor final
    {
    public:
        DECLARE_SERIALIZE()
        void validate(bbool& _isComponentValidated);

    public:
        SoundDescriptor() : m_template(NULL), m_soundComponent(NULL), m_lastIndexPlayed(U32_INVALID), m_freeTemplateOnDelete(bfalse), m_customCategory(StringID::Invalid), m_playModeInputIndex(U32_INVALID)
        {
        }

        ~SoundDescriptor();

        void init(const SoundDescriptor_Template * _template,class SoundComponent * _component, bbool _staticTemplate = btrue);
		void uninit();
        //************************************
        // @name	ITF::SoundDescriptor::getResourceList
        // @public 
        // @return	ITF_INLINE ITF_VECTOR<ResourceID> &
        //************************************
        ITF_INLINE const ITF_VECTOR<ResourceID>& getResourceList() const { return m_template->getResourceList(); }

        ITF_INLINE StringID getCategory() const { return m_customCategory != StringID::Invalid ? m_customCategory : getTemplate()->getCategory(); }
        ITF_INLINE void     setCustomCategory(const StringID& _category) { m_customCategory = _category; }

        void onActorLoaded(Actor * _actor);
        void clear( class Actor* _actor );
        ITF_INLINE const SoundDescriptor_Template * getTemplate() const { return m_template; }
        u32 getSoundIndex();
        u32 getSoundEventID();

    private:
        void clear();

        ///////////////////////////////////////////////////////////////////////////////////////////
        SafeArray<u32> m_indices;
        u32 m_lastIndexPlayed;

        const SoundDescriptor_Template * m_template;
        class SoundComponent *  m_soundComponent;
        StringID                m_customCategory;
        u32                     m_playModeInputIndex;
        bbool                   m_freeTemplateOnDelete;
    };
}

#endif // _ITF_SOUNDDESCRIPTOR_H_

