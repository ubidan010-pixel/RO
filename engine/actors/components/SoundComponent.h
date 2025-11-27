
#ifndef _ITF_SOUNDCOMPONENT_H_
#define _ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_SOUNDINSTANCE_H_
#include "engine/sound/SoundInstance.h"
#endif //_ITF_SOUNDINSTANCE_H_


#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#define MAX_NUM_INSTANCES 20

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    class SoundComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SoundComponent, ActorComponent,2111333436);
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        SoundComponent();
        ~SoundComponent();
        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual bbool   keepAlive();
        
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onActorClearComponents();
        virtual void    Update( f32 _deltaTime );
        virtual void    onEvent(Event * _event);
        virtual void    onUnloadResources();
        virtual void    onBecomeInactive();
        virtual void    onBecomeActive();


        ITF::SoundHandle     playSound( StringID _name, u32 _animBoneIndex = U32_INVALID, const SoundDescriptor_Template * _soundDesc = NULL, const StringID& _customCategory = StringID::Invalid);
        void            stopSound(SoundHandle _handle, bbool _onlyStopLooping = bfalse);
        void            stopAll();
        bbool           isPlaying(SoundHandle _handle) const;
        f32             getVolume(SoundHandle _handle) const;
        f32             getVolume() const;

        template <typename T>
        void                        setInput(StringID _inputName, T _value );
        template <typename T>
        void                        setInputFromIndex(u32 _index, T _value );
        const InputContainer &      getInputs() const { return m_inputList; }
        void                        setUpdateSoundInput( IUpdateSoundInputCriteria* _update ) { m_soundInputUpdate = _update; }
        u32                         getNumInputs() const { return m_inputList.size();}
        u32                         findInputIndex( StringID _id ) const;
#ifdef ITF_DEBUG_STRINGID
        const char*                 getInputName( u32 _index ) const { return m_inputList[_index].getName(); }
#endif
        InputType                   getInputType( u32 _index ) const { return m_inputList[_index].getType(); }

        template <typename T>
        void                        getInputValue( u32 _index, T& _val ) const
        {
            const Input& input = m_inputList[_index];
            input.getValue(_val);
        }

#ifndef ITF_FINAL
        void            postPropertyLoad();
        void            debugSolo(bbool _solo)
        {
            if (_solo)
            {
                //mute everyone if there are no other solos otherwise just 
                //unmute self
                debugMute(bfalse);
            }
            else
            {
                //unmute everyone if there are no solos left
            }
        }
        void            debugMute(bbool _mute)
        {
            if (_mute && !m_debugIsMute )
            {
                m_debugPreviousVolume = m_volume;
                m_volume.setratio(0.0f);
            }
            else if (!_mute && m_debugIsMute)
            {
                m_volume = m_debugPreviousVolume;
            }
            m_debugIsMute = _mute;
        }
#endif //ITF_FINAL
    
    void setSoundList(const SafeArray<SoundDescriptor_Template *> & soundDescriptorList);
    void setSoundPos(SoundHandle _handle, const Vec3d& _pos);
    void attachToBone(SoundHandle _handle, u32 _boneIndex);
    ITF_VECTOR<SoundDescriptor> & getSoundList() { return m_soundList; }
    typedef ITF_MAP<StringID, u32> SoundMap;
    SoundMap& getSounds() { return m_sounds; }

    protected:
        void            clearInstance(SoundHandle _handle);

    private:
        ITF_INLINE const class SoundComponent_Template* getTemplate() const;
        void            clear();
        SoundHandle getHandleFromIndexAndIncrement(u32 _index);
        SoundHandle getHandleFromIndex(u32 _index) const;
        u32         getIndexFromHandle(SoundHandle _handle) const;
        SoundInstance * getInstanceFromHandle(SoundHandle _handle) const;

        ITF_VECTOR<std::pair<u32,SoundInstance*> >  m_instance;

        Seeder                      m_random;
        SoundHandle                 m_defaultSoundInstance;

        Vec3d                       m_cameraPrevPos;

        Volume                      m_volume;
        IUpdateSoundInputCriteria*  m_soundInputUpdate;

        bbool                       m_dieRequested;

        InputContainer              m_inputList;

        ITF_VECTOR<SoundDescriptor> m_soundList;
    
        SoundMap m_sounds;

        ITF_MAP<StringID,InstanceLimiter*> m_limiters;
		// Used to lock slots on feedbackFXManager
		ITF_MAP<StringID,SoundDescriptor *> m_feedbackSounds;



#ifndef ITF_FINAL
        //debug MUTE/SOLO
        Volume      m_debugPreviousVolume;
        bbool       m_debugIsMute;
        bbool       m_debugEditorMute;
#endif //ITF_FINAL

    };


    template <typename T>
    ITF_INLINE void SoundComponent::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if (input.getId() == _inputName)
            {
                input.setValue(_value);
                break;
            }
        }
    }

    template <typename T>
    ITF_INLINE void SoundComponent::setInputFromIndex( u32 _index, T _value )
    {
        m_inputList[_index].setValue(_value);
    }

    class SoundComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SoundComponent_Template, TemplateActorComponent,3645729875);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SoundComponent);

    public:

        SoundComponent_Template();
        ~SoundComponent_Template();
        virtual void onTemplateLoaded();

        typedef ITF_VECTOR<SoundDescriptor_Template *> SoundList;

        ITF_INLINE const SoundList & getSoundList() const { return m_soundList; }
        ITF_INLINE StringID         getDefaultSound() const { return m_defaultSound; }
        ITF_INLINE Volume           getVolume() const { return m_volume; }
        ITF_INLINE const SafeArray<InputDesc>& getInputList() const { return m_inputList; }
    private:
        SoundList                   m_soundList;
        StringID                    m_defaultSound;
        Volume                      m_volume;
        SafeArray <InputDesc>       m_inputList;
        ITF_VECTOR<Path>            m_wwiseBankList;


    };

    ITF_INLINE const class SoundComponent_Template* SoundComponent::getTemplate() const
    {
        return static_cast<const class SoundComponent_Template*>(m_template);
    }
}

#endif // _ITF_SOUNDCOMPONENT_H_

