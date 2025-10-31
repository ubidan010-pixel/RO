#ifndef _ITF_SOUNDINSTANCE_H_
#define _ITF_SOUNDINSTANCE_H_


#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_SOUNDDESCRIPTOR_H_
#include "engine/sound/SoundDescriptor.h"
#endif //_ITF_SOUNDDESCRIPTOR_H_


#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"



namespace ITF
{

    class InstanceLimiter;
    class SoundDescriptor;

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///A sound instance is a voice used by a SoundComponent. Each time we start playing a sound,
    ///We use a new sound instance
    class SoundInstance 
    {
    public:
        SoundInstance() : 
        m_sourceHandle(InvalidSourceHandle)
            ,m_initialVolume(1.0f)
            ,m_initialPitch(1.0f)
            ,m_componentVolume(1.0f)
            ,m_timer(0.0f)
            ,m_state(InstanceState_Unused)
            ,m_categoryLimiter(NULL)
            ,m_limiter(NULL)
            ,m_pos(Vec3d::Zero)
            ,m_desiredPos(Vec3d::Zero)
            ,m_owner(ITF_INVALID_OBJREF)
            ,m_fadeIn(0.0f)
            ,m_fadeOut(0.0f)
            ,m_numChannels(1)
            ,m_filterFrequency(20000.0)
            ,m_filterQ(1.0f)
            ,m_filterType(FilterType_BandPass)
            ,m_loop(bfalse)
            ,m_delay(0.0f)
            ,m_boneIndex(U32_INVALID)
            ,m_playMode(0)
            ,m_postponePlay(bfalse)
            ,m_soundDescriptor(NULL)
            ,m_isStreamed(bfalse)
            ,m_metronomeType(U32_INVALID)
            ,m_playOnNext(U32_INVALID)
        {
        }
        ~SoundInstance() {}
        enum InstanceState
        {
            InstanceState_Unused,
            InstanceState_Playing,
            InstanceState_Stopping,
            InstanceState_Delay,
            ENUM_FORCE_SIZE_32(0)
        };
        //************************************
        // @name	ITF::SoundInstance::stopVoice
        // @public 
        // @return	void
        //************************************
        virtual void        stopVoice(); //stop the voice either by closing the sound handle, or by closing the stream
        //************************************
        // @name	ITF::SoundInstance::clear
        // @public 
        // @return	void
        //************************************
        virtual void        clear();
        //************************************
        // @name	ITF::SoundInstance::setPos
        // @public 
        // @param	const Vec3d & _pos
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void     setPos(const Vec3d & _pos) { m_previousPos = m_pos; m_pos = _pos; setSpeed(m_pos - m_previousPos);}
        //************************************
        // @name	ITF::SoundInstance::getPos
        // @public 
        // @return	ITF_INLINE const Vec3d &
        //************************************
        ITF_INLINE const Vec3d & getPos() const { return m_pos; }
        //************************************
        // @name	ITF::SoundInstance::setDesiredPos
        // @public 
        // @param	_pos : desired position
        // This function WILL NOT move sound instance, but will set its desired position
        // which will be used during sound instance update to set its position
        //************************************
        ITF_INLINE void     setDesiredPos(const Vec3d & _pos) { m_desiredPos = _pos;}
        //************************************
        // @name	ITF::SoundInstance::getDesiredPos
        // @public 
        // @return	desired Position
        //************************************
        ITF_INLINE const Vec3d & getDesiredPos() { return m_desiredPos;}
        //************************************
        // @name	ITF::SoundInstance::isPlaying
        // @public 
        // @return	ITF::bbool
        //************************************
        virtual bbool       isPlaying()const;
        //************************************
        // @name	ITF::SoundInstance::setInitialVolume
        // @public 
        // @param	f32 _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void     setInitialVolume(f32 _val) { m_initialVolume = _val; }
        //************************************
        // @name	ITF::SoundInstance::setInitialPitch
        // @public 
        // @param	f32 _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void     setInitialPitch(f32 _val) { m_initialPitch = _val; }
        //************************************
        // @name	ITF::SoundInstance::getCurrentVolume
        // @public 
        // @return	ITF::f32
        //************************************
        f32                 getCurrentVolume();
        //************************************
        // @name	ITF::SoundInstance::setParams
        // @public 
        // @param	ObjectRef _owner
        // @param	SoundParams * _params
        // @return	void
        //************************************
        void                setParams(ObjectRef _owner,const SoundDescriptor * _params);
        //************************************
        // @name	ITF::SoundInstance::getMicPos
        // @public 
        // @return	ITF::Vec3d
        //************************************
        const Vec3d &       getMicPos() const { return m_micPos; }
        //************************************
        // @name	ITF::SoundInstance::setMicPos
        // @public 
        // @param	Vec3d _pos
        // @return	void
        //************************************
        void                setMicPos(const Vec3d & _pos) { m_micPos = _pos; }
        //************************************
        // @name	ITF::SoundInstance::getPan
        // @public 
        // @return	f32
        //************************************
        f32                getPan();
        //************************************
        // @name	ITF::SoundInstance::getPitch
        // @public 
        // @param	f32 _pitch
        // @return	ITF::f32
        //************************************
        f32                 getPitch(f32 _pitch) ;
        //************************************
        // @name	ITF::SoundInstance::getVolume
        // @public 
        // @param	f32 _volume
        // @return	ITF::f32
        //************************************
        f32                 getVolume(f32 _volume) ;
        //************************************
        // @name	ITF::SoundInstance::update
        // @public 
        // @param	f32 _dt
        // @return	void
        //************************************
        void                update(f32 _dt);
#ifdef ITF_SUPPORT_WWISE
        void                updateVolume();
#endif
        //************************************
        // @name	ITF::SoundInstance::getInputs
        // @public 
        // @return	const InputContainer &
        //************************************
        const InputContainer & getInputs() const;
        //************************************
        // @name	ITF::SoundInstance::getSpeed
        // @public 
        // @return	ITF_INLINE Vec3d
        //************************************
        ITF_INLINE const Vec3d &  getSpeed() const { return m_speed; }
        //************************************
        // @name	ITF::SoundInstance::setSpeed
        // @public 
        // @param	Vec3d _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void     setSpeed(const Vec3d & _val) { m_speed = _val; }
        //************************************
        // @name	ITF::SoundInstance::getMicSpeed
        // @public 
        // @return	ITF_INLINE Vec3d
        //************************************
        ITF_INLINE const Vec3d &   getMicSpeed() const { return m_micSpeed; }
        //************************************
        // @name	ITF::SoundInstance::setMicSpeed
        // @public 
        // @param	Vec3d _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void     setMicSpeed(const Vec3d & _val) { m_micSpeed = _val; }
        //************************************
        // @name	ITF::SoundInstance::getLimiter
        // @public 
        // @return	ITF_INLINE InstanceLimiter *
        //************************************
        ITF_INLINE InstanceLimiter * getCategoryLimiter() const { return m_categoryLimiter; }
        //************************************
        // @name	ITF::SoundInstance::setLimiter
        // @public 
        // @param	InstanceLimiter * _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setCategoryLimiter(InstanceLimiter * _val) { m_categoryLimiter = _val; }
        //************************************
        // @name	ITF::SoundInstance::getLimiter
        // @public 
        // @return	ITF_INLINE InstanceLimiter *
        //************************************
        ITF_INLINE InstanceLimiter * getLimiter() const { return m_limiter; }
        //************************************
        // @name	ITF::SoundInstance::setLimiter
        // @public 
        // @param	InstanceLimiter * _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setLimiter(InstanceLimiter * _val) { m_limiter = _val; }
        //************************************
        // @name	ITF::SoundInstance::getState
        // @public 
        // @return	ITF_INLINE ITF::SoundInstance::InstanceState
        //************************************
        ITF_INLINE InstanceState getState() const { return m_state; }
        //************************************
        // @name	ITF::SoundInstance::setState
        // @public 
        // @param	InstanceState _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setState(InstanceState _val) { m_state = _val; }
        //************************************
        // @name	ITF::SoundInstance::getNumChannels
        // @public 
        // @return	ITF_INLINE u32
        //************************************
        ITF_INLINE u32 getNumChannels() const { return m_numChannels; }
        //************************************
        // @name	ITF::SoundInstance::setNumChannels
        // @public 
        // @param	ITF::u32 _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setNumChannels(u32 _val) { m_numChannels = _val; }
        //************************************
        // @name	ITF::SoundInstance::getHandle
        // @public 
        // @return	ITF_INLINE SourceHandle
        //************************************
        ITF_INLINE SourceHandle getHandle() const { return m_sourceHandle; }
        ITF_INLINE SoundHandle getPlayingHandle() const { return m_playingHandle; }

        //************************************
        // @name	ITF::SoundInstance::setHandle
        // @public 
        // @param	SourceHandle _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setHandle(SourceHandle _val) { m_sourceHandle = _val; }
        //************************************
        // @name	ITF::SoundInstance::getTimer
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32 getTimer() const { return m_timer; }
        //************************************
        // @name	ITF::SoundInstance::setTimer
        // @public 
        // @param	f32 _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setTimer(f32 _val) { m_timer = _val; }
        //************************************
        // @name	ITF::SoundInstance::getFadeIn
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32 getFadeIn() const { return m_fadeIn; }
        //************************************
        // @name	ITF::SoundInstance::setFadeIn
        // @public 
        // @param	f32 _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setFadeIn(f32 _val) { m_fadeIn = _val; }
        //************************************
        // @name	ITF::SoundInstance::getFadeOut
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32 getFadeOut() const { return m_fadeOut; }
        //************************************
        // @name	ITF::SoundInstance::setFadeOut
        // @public 
        // @param	f32 _val
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setFadeOut(f32 _val) { m_fadeOut = _val; }
        //************************************
        // @name	ITF::SoundInstance::play
        // @public 
        // @return	void
        //************************************
        void play();
        //************************************
        // @name	ITF::SoundInstance::stop
        // @public 
        // @return	void
        //************************************
        virtual void        stop();
        //************************************
        // @name	ITF::SoundInstance::getFilterFrequency
        // @public 
        // @param	FilterType _filterType
        // @param	f32 _filterFrequency
        // @param	f32 _filterQ
        // @return	ITF::f32
        //************************************
        f32 getFilterFrequency(FilterType _filterType, f32 _filterFrequency, f32 _filterQ);
        //************************************
        // @name	ITF::SoundInstance::getLoop
        // @public 
        // @return	ITF::bbool
        //************************************
        bbool getLoop() const { return m_loop; }

        void setPlayMode( u32 _mode ) { m_playMode = _mode; }
        u32  getPlayMode() { return m_playMode; }

        ITF_INLINE f32 getDelay() const { return m_delay; }

        ITF_INLINE u32 getBoneIndex() const { return m_boneIndex; }
        ITF_INLINE void setBoneIndex( u32 _index) { m_boneIndex = _index;}

        ITF_INLINE bbool    getPostponePlay() const { return m_postponePlay; }
        ITF_INLINE void     setPostponePlay(bbool _bool) { m_postponePlay = _bool; }

        ITF_INLINE SoundDescriptor *    getSoundDescriptor() const { return m_soundDescriptor;}
        ITF_INLINE void                 setSoundDescriptor(SoundDescriptor * _soundDesc) { m_soundDescriptor = _soundDesc;}

        ITF_INLINE void                 setIsStreamed( bbool _bool ) { m_isStreamed = _bool; }
        ITF_INLINE bbool                getIsStreamed() const { return m_isStreamed; }

        ITF_INLINE void         setMetronomeType(u32 _type) { m_metronomeType = _type; }
        ITF_INLINE const u32    getMetronomeType() const { return m_metronomeType; }
        ITF_INLINE void         setPlayOnNext(u32 _nextSubdiv) { m_playOnNext = _nextSubdiv; }
        ITF_INLINE const u32    getPlayOnNext() const { return m_playOnNext; }


        inline void setPendingSyncStart();

        inline void resetPendingSyncStart();

        inline bool getIsPendingSyncStart() const;



#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE)
        String8 m_debugName;
        //************************************
        // @name	ITF::SoundInstance::getDebugName
        // @public 
        // @return	const String8 &
        //************************************
        const String8 & getDebugName() { return m_debugName; }
        //************************************
        // @name	ITF::SoundInstance::setDebugName
        // @public 
        // @param	const String8 & _name
        // @return	void
        //************************************
        void setDebugName(const String8 & _name)
        {
            m_debugName = _name;
        }

        f32 m_debugVolume;
        f32 m_debugPitch;
#endif

    private:
        SourceHandle m_sourceHandle;
        SoundHandle  m_playingHandle = SoundHandle::getInvalidHandle();
        f32 m_initialVolume;
        f32 m_componentVolume;
        f32 m_timer;
        f32 m_initialPitch;
        InstanceState m_state;
        InstanceLimiter * m_categoryLimiter;
        InstanceLimiter * m_limiter;
        Vec3d m_pos;
        Vec3d m_desiredPos;
        Vec3d m_previousPos;
        Vec3d m_micPos;
        Vec3d m_speed;
        Vec3d m_micSpeed;
        ObjectRef m_owner;
        f32 m_fadeIn;
        f32 m_fadeOut;
        u32 m_numChannels;
        FilterType m_filterType;
        f32 m_filterFrequency;
        f32 m_filterQ;
        bbool   m_loop;
        f32 m_delay;
        u32 m_boneIndex;
        SafeArray<SoundModifier*> m_modifiers;
        u32 m_playMode;
        bbool m_postponePlay;
        SoundDescriptor * m_soundDescriptor;
        bbool m_isStreamed;
        u32 m_metronomeType;  // define on which metronome this sound should be synchronized
        u32 m_playOnNext; // define at which time this sound should be synchronized (1 bar = 24)
        volatile int m_isPendingSyncStart = 0;
        bbool m_loopingAndStarted= bfalse;

        ObjectRef                           m_wwiseObjectref = ITF_INVALID_OBJREF;

    };

    void SoundInstance::setPendingSyncStart()
    {
        m_isPendingSyncStart = 1;
    }

    void SoundInstance::resetPendingSyncStart()
    {
        m_isPendingSyncStart = 0;
    }

    bool SoundInstance::getIsPendingSyncStart() const
    {
        return m_isPendingSyncStart ? true : false;
    }



}
#endif // _ITF_SOUNDINSTANCE_H_

