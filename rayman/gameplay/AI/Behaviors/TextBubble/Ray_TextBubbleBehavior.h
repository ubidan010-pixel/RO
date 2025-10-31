#ifndef _RAY_TEXTBUBBLEBEHAVIOR_H_
#define _RAY_TEXTBUBBLEBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    //forwards
    class Ray_TextBubbleBehavior_Template;
    class UITextBox;

    class Ray_TextBubbleBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_TextBubbleBehavior, AIBehavior,2883211760);
        DECLARE_VALIDATE_BEHAVIOR();

    public:
        Ray_TextBubbleBehavior();
        ~Ray_TextBubbleBehavior();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }
        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onActivate();
        virtual void update(f32 _dt);
        virtual void onEvent(Event * _event);
        virtual void onActionFinished();

    private:
        //actions management
        void updateAction();
        void startSleep();
        void updateSleep();
        void startOpen();
        void startIdle();
        void updateIdle();
        void startClose();

        //text management
        Vec3d getTextPosition() const;

    private:
        ITF_INLINE const Ray_TextBubbleBehavior_Template * getTemplate() const;

        //components
        UITextBox * m_textComponent;
        AnimLightComponent * m_animLightComponent;

        //actions
        AIAction * m_sleepAction;
        AIAction * m_openAction;
        AIAction * m_idleAction;
        AIAction * m_closeAction;

        //text data
        LocalisationId m_lineId;

        //flags
        bbool m_activated;
        bbool m_linkToActivator;
        bbool m_textRefreshRequested;
        bbool m_bubbleClosingRequested;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_TextBubbleBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_TextBubbleBehavior_Template, TemplateAIBehavior,300230979);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_TextBubbleBehavior);

    public:

        Ray_TextBubbleBehavior_Template();
        ~Ray_TextBubbleBehavior_Template();

        ITF_INLINE const StringID & getTextBoneName() const;
        ITF_INLINE const AIAction_Template * getSleepActionTemplate() const;
        ITF_INLINE const AIAction_Template * getOpenActionTemplate() const;
        ITF_INLINE const AIAction_Template * getIdleActionTemplate() const;
        ITF_INLINE const AIAction_Template * getCloseActionTemplate() const;


    private:
        StringID m_textBoneName;
        AIAction_Template * m_sleepActionTemplate;
        AIAction_Template * m_openActionTemplate;
        AIAction_Template * m_idleActionTemplate;
        AIAction_Template * m_closeActionTemplate;
    };


    ITF_INLINE const Ray_TextBubbleBehavior_Template * Ray_TextBubbleBehavior::getTemplate() const
    {
        return static_cast<const Ray_TextBubbleBehavior_Template *>( m_template );
    }


    ITF_INLINE const StringID & Ray_TextBubbleBehavior_Template::getTextBoneName() const
    {
        return m_textBoneName;
    }

    ITF_INLINE const AIAction_Template * Ray_TextBubbleBehavior_Template::getSleepActionTemplate() const
    {
        return m_sleepActionTemplate;
    }

    ITF_INLINE const AIAction_Template * Ray_TextBubbleBehavior_Template::getOpenActionTemplate() const
    {
        return m_openActionTemplate;
    }

    ITF_INLINE const AIAction_Template * Ray_TextBubbleBehavior_Template::getIdleActionTemplate() const
    {
        return m_idleActionTemplate;
    }

    ITF_INLINE const AIAction_Template * Ray_TextBubbleBehavior_Template::getCloseActionTemplate() const
    {
        return m_closeActionTemplate;
    }
}

#endif // _RAY_TEXTBUBBLEBEHAVIOR_H_

