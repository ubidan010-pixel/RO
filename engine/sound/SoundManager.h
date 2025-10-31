
#ifndef _ITF_SOUNDMANAGER_H_
#define _ITF_SOUNDMANAGER_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_SOUNDINSTANCE_H_
#include "engine/sound/SoundInstance.h"
#endif //_ITF_SOUNDINSTANCE_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_SOUNDDESCRIPTOR_H_
#include "engine/sound/SoundDescriptor.h"
#endif //_ITF_SOUNDDESCRIPTOR_H_




#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_


#define MaxSoundInstances 256
#define MaxStreamInstances 2
#define MAX_BUS_MIX 8
namespace ITF
{




    ///////////////////////////////////////////////////////////////////////////////////////////
    enum LimiterMode : i32
    {
        LimiterMode_RejectNew,
        LimiterMode_StopOldest,
        LimiterMode_StopLowestVolume,
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    struct InstanceRef 
    {
        InstanceRef(ObjectRef _actor, ITF::SoundHandle _handle, u32 _time) : m_actor(_actor), m_handle(_handle), m_time(_time){}
        ObjectRef m_actor;
        ITF::SoundHandle m_handle;
        u32 m_time;
        bool operator==(const InstanceRef & _other) const
        {
            return m_actor == _other.m_actor && m_handle == _other.m_handle;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class InstanceLimiter
    {
    public:
        InstanceLimiter() : m_maxInstances(0), m_mode(LimiterMode_RejectNew), m_curFrame(0), m_soundsAddableInFrame(0){}
        void    addInstance(ObjectRef _actor, ITF::SoundHandle _handle )
        {
            ITF_ASSERT(canPlay());

            if (m_instances.size()>=m_maxInstances)
            {
                switch(m_mode)
                {
                case LimiterMode_StopLowestVolume:
                    stopLowestVolume();
                    break;
                case LimiterMode_StopOldest:
                    stopOldest();
                    break;
                default:
                    break;
                }
            }
            m_instances.push_back(InstanceRef(_actor,_handle,CURRENTFRAME));
            m_soundsAddableInFrame = m_soundsAddableInFrame > 0 ? m_soundsAddableInFrame-1: 0;
        }
        void    removeInstance(ObjectRef _actor, ITF::SoundHandle _handle)
        {
            ITF_VECTOR<InstanceRef>::iterator it = find(m_instances.begin(),m_instances.end(),InstanceRef(_actor,_handle,0));
            if (it != m_instances.end())
                m_instances.erase(it);
        }
        bbool   canPlay()
        {
            if (m_mode == LimiterMode_RejectNew && m_instances.size() >= m_maxInstances)
                return bfalse;
            else if (m_mode == LimiterMode_StopOldest )
            {
                if (CURRENTFRAME != m_curFrame)
                {
                    m_curFrame = CURRENTFRAME;
                    m_soundsAddableInFrame = m_maxInstances;
                }
                if (m_soundsAddableInFrame == 0)
                    return bfalse;
                else
                    return btrue;
            }
            return btrue;
        }
        ITF_INLINE StringID getLimitCategory() const { return m_limitCategory; }
        void init(const LimiterDef & _def);

    private:
        void stopLowestVolume();
        void stopOldest();
        ITF_VECTOR<InstanceRef> m_instances;
        LimiterMode m_mode;
        StringID m_limitCategory;
        u32 m_maxInstances;
        u32 m_soundsAddableInFrame;
        u32 m_curFrame;
    };



    class ObjectFactory;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class SoundManager : public IEventListener,public TemplateSingleton<SoundManager>
    {
    public:
        SoundManager();
        ~SoundManager();
        
        //************************************
        // @name	ITF::SoundManager::init
        // @public 
        // @return	void
        //************************************
        void init();
        //************************************
        // @name	ITF::SoundManager::acquireInstance
        // @public 
        // @param	SoundStreamerDataSource * _dataSource
        // @param	Sound * _sound
        // @param	SoundDescriptor * _desc
        // @return	SoundInstance *
        //************************************
        SoundInstance * acquireInstance(Sound * _sound, SoundDescriptor * _desc);
        bbool           acquireAndPlay(SoundInstance * _instance);
        //************************************
        // @name	ITF::SoundManager::releaseInstance
        // @public 
        // @param	SoundInstance * _instance
        // @return	void
        //************************************
        void releaseInstance(SoundInstance * _instance);
        //************************************
        // @name	ITF::SoundManager::play
        // @public 
        // @param	SoundInstance * _instance
        // @return	void
        //************************************
        void play(SoundInstance * _instance);
        //************************************
        // @name	ITF::SoundManager::stop
        // @public 
        // @param	SoundInstance * _instance
        // @return	void
        //************************************
        void stop(SoundInstance * _instance);
        //************************************
        // @name	ITF::SoundManager::update
        // @public 
        // @param	f32 _dt
        // @return	void
        //************************************
        static void staticUpdate(f32 _dt);
        void update(f32 _dt);
        //************************************
        // @name	ITF::SoundManager::getModifierFactory
        // @public 
        // @return	ObjectFactory *
        //************************************
        ObjectFactory * getModifierFactory() { return &m_modifierFactory;}
        //************************************
        // @name	ITF::SoundManager::onEvent
        // @public 
        // @param	Event * _event
        // @return	void
        //************************************
        virtual void onEvent(Event * _event);
        //************************************
        // @name	ITF::SoundManager::getLimiter
        // @public 
        // @param	StringID _limitCategory
        // @return	InstanceLimiter*
        //************************************
        //************************************ 
        InstanceLimiter*   getLimiter(StringID _limitCategory)
        {
            ITF_MAP<StringID,InstanceLimiter*>::iterator it = m_limiters.find(_limitCategory);
            if (it != m_limiters.end())
            {
                return it->second;
            }
            return NULL;
        }

        void playBusMix(const BusMix * _busMix, bbool _relative  = bfalse);
        void stopBusMix(const BusMix * _busMix, bbool _relative  = bfalse);
        bbool isBusMixPlaying(ObjectRef _ref, const BusMix * _busMix);
        void playBusMix(StringID _name, bbool _relative  = bfalse);
        void stopBusMix(StringID _name, bbool _relative  = bfalse);

#ifndef ITF_FINAL
        const ITF_VECTOR<BusMix>& getBusMix() const { return m_busMixStack; }
        ITF_INLINE const BusMix & getRelativeBusMixTarget() const { return m_relativeBusMixTarget; }
#endif

        ITF_INLINE const SafeArray<SoundInstance*>* getPlayingInstances() const { return &m_playingInstances; }

    private:

        void updateBusMix(f32 _dt);
        Volume getLowerPrioVolume(u32 _prio, StringID _bus);
        void  setBusVolumes(u32 _priority, f32 _duration);

        FixedArray<SoundInstance,MaxSoundInstances> m_instances;
        FixedArray<SoundInstance,MaxStreamInstances> m_streamInstances;

        ObjectFactory m_modifierFactory;
        ITF_MAP<StringID,InstanceLimiter*> m_limiters;

        f32 m_busMixTotalTimer;

        ITF_VECTOR<BusMix> m_busMixStack;
        BusMix m_relativeBusMixTarget;
        SafeArray<SoundInstance*> m_playingInstances;
    };

#define SOUND_MANAGER SoundManager::getptr

#define SOUND_BUS_MIX_NOSOUND ITF_GET_STRINGID_CRC(noSound,258229311)
#define SOUND_BUS_MIX_FADEOUT ITF_GET_STRINGID_CRC(fadeOut,872188705)
#define SOUND_BUS_MIX_PAUSE ITF_GET_STRINGID_CRC(pause,2442195216)
#define SOUND_BUS_MIX_PLAYINGMOVIE ITF_GET_STRINGID_CRC(playingVideo,1043713749)
}
#endif // _ITF_SOUNDMANAGER_H_

