#ifndef _ITF_AIBEHAVIOR_H_
#define _ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#define DECLARE_VALIDATE_BEHAVIOR() virtual void validate(bbool &_isBehaviorValidated);
#define BEGIN_VALIDATE_BEHAVIOR(_classname_) void _classname_::validate(bbool &_isBehaviorValidated) { Super::validate(_isBehaviorValidated);
#define END_VALIDATE_BEHAVIOR() }


namespace ITF
{
    class Actor;
    class AIComponent;
    class AIAction;
    class AnimLightComponent;
    class AnimatedComponent;
    class FXControllerComponent;
    class SoundComponent;

    class AIBehavior : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIBehavior,IRTTIObject,1709812938);

    public:

        AIBehavior();
        virtual ~AIBehavior();

        void                    init( Actor* _actor, AIComponent* _aiComponent );
        virtual void            validate( bbool& _isBehaviorValidated );
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/) {}
        virtual void            onResourceReady();
        virtual void            onActorBecomeActive();
        virtual void            onActorBecomeInactive();
        virtual void            onForceMove();

        virtual void            update( f32 _dt  );
        virtual void            onEvent( Event * _event);
        virtual void            updateAnimInputs();
        virtual void            updateSoundInputs();
        virtual void            updateFxInputs();

        virtual bbool           isActivatedByEvent( Event* _event ) { return bfalse; }
        virtual void            setData( Event* _event ) {}
        virtual void            onActivate() = 0;
        virtual void            onDeactivate();
        virtual void            onActionFinished() {}

        bbool                   isCurrentAction( AIAction* _action ) const { return m_currentAction == _action; }
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void            getDebugText( String8& _text, const String8& _separator = " - " );
#endif // ITF_SUPPORT_DEBUGFEATURE

#ifdef ITF_SUPPORT_EDITOR
        virtual void            drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
        virtual void            onEditorMove(bbool _modifyInitialPos = btrue) {}
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE AIAction*    getCurrentAction() const { return m_currentAction; }
        void                    setTemplate( const class TemplateAIBehavior* _template ) { m_template = _template; }

        const class TemplateAIBehavior* getTemplate() const { return m_template; }

		ITF_INLINE void			setHasExtern()		{ m_isExtern = btrue; }
		ITF_INLINE bbool		isExtern()	const 	{ return m_isExtern; }


    protected:

        void                    setAction( AIAction* _action, bbool _restart = bfalse );
        void                    setNullAction();
        AIAction *              createAiAction(const AIAction_Template * _template);
        template <typename T> T* createAiActionAndCast(const AIAction_Template * _template);

        const class TemplateAIBehavior * m_template;

        AnimLightComponent*     m_animComponent;
        AnimatedComponent*      m_animatedComponent;
        FXControllerComponent*  m_fxController;
        SoundComponent*         m_soundComponent;

        Actor*                  m_actor;
        AIComponent*            m_aiComponent;

        typedef ITF_VECTOR <AIAction*>  ActionsArray;
        ActionsArray            m_actions;
        AIAction*               m_currentAction;

        bbool                   m_destroyRegisteredActions;
		bbool					m_isExtern;

    private:
        void                    addAction( AIAction* _action );


    };

    template <typename T> T* AIBehavior::createAiActionAndCast(const AIAction_Template* _template)
    {
        if (_template)
        {
            AIAction * action = _template->createAIAction();
            if (T* castedAction =  action->DynamicCast<T>(T::GetClassCRCStatic()))
            {
                addAction(action);
                action->setTemplate(_template);
                return castedAction;
            }
            SF_DEL(action);
        }
        return NULL;
    }

#define DECLARE_AIBEHAVIOR_TEMPLATE(aiBehaviorClass) \
public: \
    virtual class AIBehavior* createAIBehavior() const { return newAlloc(mId_System, aiBehaviorClass()); } \


    class TemplateAIBehavior : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(TemplateAIBehavior,IRTTIObject,3890605354);
        DECLARE_SERIALIZE_VIRTUAL()
    public:

        TemplateAIBehavior() {}
        ~TemplateAIBehavior() {}

        virtual class AIBehavior* createAIBehavior() const = 0;
        virtual void onTemplateLoaded() {}

		ITF_INLINE const StringID & getName() const { return m_name; }

    private:
		StringID m_name;
    };
}

#endif // _ITF_AIBEHAVIOR_H_

