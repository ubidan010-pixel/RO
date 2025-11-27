
#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#define _ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

namespace ITF
{
    //const u32 MAX_FX_INSTANCES = 20;

    class SoundComponent;
    class PolylineComponent;
    class AnimLightComponent;
    class FxBankComponent;
    struct FxInstance;

    enum FXPlayMode
    {
        FXPlayMode_Actor = 0,
        FXPlayMode_ActorBase,
        FXPlayMode_AttachedToBone,
        FXPlayMode_Position,
    };

    class FXControllerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FXControllerComponent,ActorComponent,2370830262);

    public:
        DECLARE_SERIALIZE()

        FXControllerComponent();
        ~FXControllerComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }
        virtual bbool keepAlive();

        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void onCheckpointLoaded();
        virtual void onEvent(Event * _event);
        virtual void Update( f32 _deltaTime );

       
		void removeFromStopListFromIndex(u32 _index);

        virtual void onBecomeActive();
        virtual void onResourceReady();

        virtual void onBecomeInactive();

        void        setMaterial( const StringID& _matID ){ m_material = _matID;}

        u32         playFX(StringID _fxName);
		u32         playFeedback(const ObjectRef& _actorRef, const StringID& _action, const ObjectRef& _targetRef, const StringID & _animFriendly = StringID::Invalid );
		u32         playMaterialFX(StringID _fxName, const GameMaterial_Template* _mat = NULL);
        u32         playMaterialFX(StringID _fxName, StringID _animFriendly, const GameMaterial_Template* _mat);
        void        stopFXFromName(StringID _fxName, bbool _immediateParticlesStop = bfalse);
	void        stopFXFromHandle(u32 _handle, bbool _onlyStopLooping = bfalse, bbool _immediateParticlesStop = bfalse, bbool _stopOnDestroy = bfalse);
	void        stopAllFX(bbool _stopOnDestroy = bfalse, bbool _onlyStopLooping = bfalse, bbool _immediateParticlesStop = bfalse);


        bbool       IsAllFXStopped() { return !m_playingFX; }
	bbool isPlayingFromHandle(u32 _handle);
	bbool isPlayingFromName(const StringID& _name);
	bbool isSoundPlayingFromHandle(u32 _handle);
	bbool isParticlePlayingFromHandle(u32 _handle);
        u32 acquireFXInstance(u32& _index);
        //************************************
        // @name	ITF::FXControllerComponent::getParticuleGenerator()
        // @public 
        // @param	_handle : handle given by playFX()
        // @param   _fxName : name of the specific ParticleGenerator to get
        // @return	a pointer of the current instance of ParticleGenerator
        //************************************
        ITF_ParticleGenerator* getParticleGenerator(u32 _handle, StringID _fxName);

        //************************************
        // @name	ITF::FXControllerComponent::getParticuleGenerators()
        // @public 
        // @param	_handle : handle given by playFX()
        // @param   _generators : array filled in with particle generators
        // Fill in _generators array with all particle generators used by this handle
        //************************************
        void getParticleGenerators(u32 _handle, SafeArray<ITF_ParticleGenerator*>& _generators);

        //************************************
        // @name	ITF::FXControllerComponent::setFXPos()
        // @public 
        // @param	_handle : handle given by playFX()
        // @param   _pos    : new position to set
        // Set position of the FxInstance (which can be sound(s) + VFX(s))
        // It will stick to this position even if a bone has been set in FXControl param
        //************************************
        void setFXPosFromHandle(u32 _handle, const Vec3d& _pos);
        void setFXPosFromIndex(u32 index, const Vec3d& _pos);

        //************************************
        // @name	ITF::FXControllerComponent::setFXPos()
        // @public 
        // @param	_name   : FXControl name
        // @param   _pos    : new position to set
        // Set position of the FxInstance (which can be sound(s) + VFX(s))
        // It will stick to this position even if a bone has been set in FXControl param
        //************************************
        void setFXPosFromName(StringID _fxName, const Vec3d& _pos);

        //************************************
        // @name	ITF::FXControllerComponent::setFXAngle()
        // @public 
        // @param	_handle : handle given by playFX()
        // @param   _pos    : new position to set
        // Set angle of the FxInstance (which can be sound(s) + VFX(s))
        //************************************
        void setFXAngleFromHandle(u32 _handle, f32 _angle);
        void setFXAngleFromIndex(u32 _index, f32 _angle);

        //************************************
        // @name	ITF::FXControllerComponent::setFXAngle()
        // @public 
        // @param	_name   : FXControl name
        // @param   _pos    : new position to set
        // Set angle of the FxInstance (which can be sound(s) + VFX(s))
        //************************************
        void setFXAngleFromName(StringID _fxName, f32 _angle);

        //************************************
        // @name	ITF::FXControllerComponent::setFXPos()
        // @public 
        // @param	_handle : handle given by playFX()
        // @param   _boneIndex : index of the bone to attach with
        // attach FxInstance to specified bone
        //************************************
        void attachToBoneFromHandle(u32 _handle, u32 _boneIndex);

        void attachToBoneFromIndex(u32 index, u32 _boneIndex);

        //************************************
        // @name	ITF::FXControllerComponent::setFXPos()
        // @public 
        // @param	_name   : FXControl name
        // @param   _boneIndex : index of the bone to attach with
        // attach FxInstance to specified bone
        //************************************
        void attachToBoneFromName(StringID _fxName, u32 _boneIndex);

        virtual void onUnloadResources();
        const GameMaterial_Template* getMaterial() const;
        const StringID& getMaterialID() const { return m_material; }

        const FXControl* getFXControl( const StringID& _fxName ) const;

    protected:
        struct FXInstance
        {
            FXInstance()
                : m_control(NULL)
                , m_soundInstance(ITF::SoundHandle::getInvalidHandle())
                , m_particleInstance(U32_INVALID)
                , m_playMode(FXPlayMode_Actor)
                , m_handle(U32_INVALID)
            {
            }

            const FXControl* m_control;
            ITF::SoundHandle             m_soundInstance;
            SafeArray<ITF::SoundHandle>  m_soundInstances;
            u32             m_particleInstance;
            SafeArray<u32>  m_particleInstances;
            StringID    m_fromAnim;
            // Play mode
            FXPlayMode      m_playMode;
            u32 m_handle;
        };
        static const String8 s_default;
        u32             playFXInternal(StringID _fxName, const StringID & _animFriendly, f32 _ratio);
		u32				playFeedbackInternal( const StringID& _actor
                                            , const StringID& _actorArchetype
                                            , const StringID& _defaultActor
                                            , const StringID& _action
                                            , const StringID& _target
                                            , const StringID& _targetArchetype
                                            , const StringID& _defaultTarget
                                            , const StringID & _animFriendly = StringID::Invalid
                                            , const StringID& _customCategory = StringID::Invalid);
	void releaseFXInstanceFromHandle(u32 _handle);
	void releaseFXInstanceFromIndex(u32 _handle);
	void stopFXFromIndex(u32 _index, bbool _onlyStopLooping = bfalse, bbool _immediateParticlesStop = bfalse, bbool _stopOnDestroy = bfalse);

        FxInstance* getFxInstance(FxBankComponent* m_fxBankComponent, u32 fxInstanceIndex, StringID _fxName);

		bbool isPlayingFromIndex(u32 _index);
		bbool isSoundPlayingFromIndex(u32 _index);
		bbool isParticlePlayingFromIndex(u32 _index);

     
        void            startDefaultFx();
	void stopDefaultFx(bbool _stopOnDestroy = bfalse);

        void			startTriggerFx();
	void stopTriggerFx(bbool _stopOnDestroy = bfalse);



        //components
        SoundComponent * m_soundComponent;
        FxBankComponent * m_fxBankComponent;
        PolylineComponent * m_polylineComponent;
        AnimLightComponent * m_animLightComponent;

        //currentAnim
        StringID m_currentSubAnim;
        //list of fx  to play once
        ITF_VECTOR<StringID> m_fxPlayOnceList;
        //list of fx to stop 
        struct Fx
        {
            Fx() : handle(U32_INVALID){}
            StringID    fxName;
            StringID    animFriendly;
            u32         handle;
        };
        typedef ITF_VECTOR< Fx > FxToStopContainer ;
        FxToStopContainer m_fxToStop;

        KeyArray<FXInstance> m_fxInstances;
        u32 m_idCount;

        SafeArray<StringID> m_stoppedAnims;

        //MATERIAL
        StringID m_material;


        //TRIGGER
        u32         m_triggerFxHandle;
        bbool       m_triggerFxStopOnUntrigger;

        // Default FX (always-on if specified)
        u32         m_defaultFxHandle;

        bbool       m_dieRequested;

        // In order to know if there is at least 1 FX being played
        bbool       m_playingFX;

        // In order to launch FX in update if trigger in event failed
        bbool       m_triggerFXisActive;

	private:
        ITF_INLINE const class FXControllerComponent_Template*  getTemplate() const;
        void    clear();
    };


    class FXControllerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FXControllerComponent_Template,TemplateActorComponent,238378088);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(FXControllerComponent);

    public:

        FXControllerComponent_Template();
        ~FXControllerComponent_Template();

        virtual void onTemplateLoaded();

        ITF_INLINE const ITF_MAP<StringID,FXControl*>& getEvents() const { return m_events; }
        ITF_INLINE const ITF_VECTOR<FXControl*>& getControlList() const { return m_fxControlList; }
        ITF_INLINE StringID getTriggerFx() const { return m_triggerFx; }
        ITF_INLINE StringID getDefaultFx() const { return m_defaultFx; }

    private:
        ITF_MAP<StringID,FXControl*>	m_events;
        ITF_VECTOR<FXControl*>			m_fxControlList;
        StringID						m_triggerFx;
        StringID						m_defaultFx;
    };

    ITF_INLINE const FXControllerComponent_Template*  FXControllerComponent::getTemplate() const {return static_cast<const FXControllerComponent_Template*>(m_template);}

}

#endif // _ITF_FXCONTROLLERCOMPONENT_H_

