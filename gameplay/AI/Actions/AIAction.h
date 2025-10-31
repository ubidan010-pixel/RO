#ifndef _ITF_AIACTION_H_
#define _ITF_AIACTION_H_

#define DECLARE_VALIDATE_ACTION() virtual void validate(bbool &_isActionValidated);
#define BEGIN_VALIDATE_ACTION(_classname_) void _classname_::validate(bbool &_isActionValidated) { Super::validate(_isActionValidated);
#define END_VALIDATE_ACTION() }

namespace ITF
{
    class Actor;
    class AIComponent;
    class AIBehavior;
    class AnimLightComponent;
    class AnimatedComponent;
    class FXControllerComponent;
    class StickToPolylinePhysComponent;


    class AIAction : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIAction,IRTTIObject,1899314887);

    public:

        AIAction();
        virtual ~AIAction();

        void                    init( Actor* _actor, AIComponent* _aiComponent, AIBehavior* _aiBehavior );
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onResourceReady() {}
        virtual void            validate( bbool& _isActionValidated ) {}
        virtual void            onActorBecomeActive() {}
        virtual void            onActorBecomeInactive() {}
        virtual void            onForceMove() {}

        virtual void            update( f32 _dt );
        virtual void            onEvent( Event * _event );
        virtual void            updateAnimInputs() {}
        virtual void            updateSoundInputs() {}
        virtual void            updateFxInputs() {}

        virtual void            onActivate();
        virtual void            onDeactivate();
        void                    playAction( StringID _action );
        bbool                   isAnimFinished() const;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void            getDebugText( String8& _text );
#endif //ITF_SUPPORT_DEBUGFEATURE

#ifdef ITF_SUPPORT_EDITOR
        virtual void            drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const {}
#endif // ITF_SUPPORT_EDITOR

        Actor*                  GetActor() const { return m_actor; }

        const class AIAction_Template* getTemplate() const { return m_template; }
        void                           setTemplate( const class AIAction_Template* _template ) { m_template = _template; }

        void                    activateRootPos(bbool _activation);
        ITF_INLINE bbool        getUseRootPos() const { return m_useRootPos; }

        ITF_INLINE StringID     getAnimAction() const;

    protected:

        virtual void            onEndMarkerReached();

        void                    updatePosition(f32 _dt);
        bbool                   isContactBlockingObstacle( const Vec2d & _delta, const struct SCollidableContact& _contact, f32 _angle ) const;

        AnimLightComponent*     m_animComponent;
        AnimatedComponent*      m_animatedComponent;
        FXControllerComponent*  m_fxController;
        StickToPolylinePhysComponent* m_physComponent;

        Actor*                  m_actor;
        AIComponent*            m_aiComponent;
        AIBehavior*             m_aiBehavior;

        bbool                   m_useRootPos;
        f32                     m_timer;
        const class AIAction_Template* m_template;

    };


#define DECLARE_AIACTION_TEMPLATE(aiActionClass) \
public: \
    virtual class AIAction* createAIAction() const { return newAlloc(mId_System, aiActionClass()); } \



    class AIAction_Template : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIAction_Template,IRTTIObject,2801106802);
        DECLARE_SERIALIZE_VIRTUAL();

    public:

        AIAction_Template();
        ~AIAction_Template() {}

        virtual class AIAction* createAIAction() const = 0;
        virtual void onTemplateLoaded() {}


        ITF_INLINE StringID getAnimDefaultAction() const { return m_animDefaultAction; }
        ITF_INLINE StringID getEndMarker() const { return m_endMarker; }
        ITF_INLINE bbool    getUseRootPos() const { return m_useRootPos; }
        ITF_INLINE bbool    getFinishOnContact() const { return m_finishOnContact; }
        ITF_INLINE const Vec2d & getRootPosScale() const { return m_rootPosScale; }
#ifdef ITF_SUPPORT_DEBUGFEATURE
        ITF_INLINE const String8 & getDebugName() const { return m_debugName; }
#endif //ITF_SUPPORT_DEBUGFEATURE
        ITF_INLINE f32 getIgnoreContactDuration() const { return m_ignoreContactDuration; }
    protected:

        StringID                m_animDefaultAction;
        StringID                m_endMarker;
        bbool                   m_useRootPos;
        bbool                   m_finishOnContact;
        Vec2d                   m_rootPosScale;
        f32                     m_ignoreContactDuration;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        String8                 m_debugName;
#endif //ITF_SUPPORT_DEBUGFEATURE
    };

    ITF_INLINE StringID AIAction::getAnimAction() const
    {
        //return m_animDefaultAction.isValid()?m_animDefaultAction:m_template->getAnimDefaultAction();
        return m_template->getAnimDefaultAction();
    }

}

#endif // _ITF_AIACTION_H_

