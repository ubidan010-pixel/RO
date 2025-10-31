#ifndef _ITF_RAY_AIREDWIZARDWATERSWIMBEHAVIOR_H_
#define _ITF_RAY_AIREDWIZARDWATERSWIMBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AIRedWizardWaterSwimBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRedWizardWaterSwimBehavior, AIBehavior,2721698425)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIRedWizardWaterSwimBehavior();
        virtual ~Ray_AIRedWizardWaterSwimBehavior();

        virtual void                            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                            update( f32 _delta  );
        virtual void                            onActivate();
        virtual void                            onDeactivate();
        virtual void                            onEvent( Event * _event);
        virtual void                            onActionFinished();

    private:

        ITF_INLINE const class Ray_AIRedWizardWaterSwimBehavior_Template * getTemplate() const;

        void                                    processBlockingContact( class EventPolylineBlockingContact* _blockedEvent );
        void                                    startSwim();
        void                                    startJump();
        void                                    onFinishedSwim();
        class StickToPolylinePhysComponent*     m_physComponent;
        class AIAction*                         m_swim;
        class AIJumpAction*                     m_jump;
        bbool                                   tryJump();

        class Ray_GroundAIComponent*            m_groundAiComponent;


        f32                                     m_timer;
        f32                                     m_timeToMove;
        Vec2d                                   m_currentDir;
    };

    class Ray_AIRedWizardWaterSwimBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRedWizardWaterSwimBehavior_Template,TemplateAIBehavior,3238344605);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIRedWizardWaterSwimBehavior);

    public:

        Ray_AIRedWizardWaterSwimBehavior_Template();
        ~Ray_AIRedWizardWaterSwimBehavior_Template();

        const class AIAction_Template*          getSwim() const { return m_swim; }
        const class AIJumpAction_Template*      getJump() const { return m_jump; }
        f32                                     getPushMinSpeed() const { return m_pushMinSpeed; }
        f32                                     getPushMaxSpeed() const { return m_pushMaxSpeed; }
        f32                                     getPushMinForce() const { return m_pushMinForce; }
        f32                                     getPushMaxForce() const { return m_pushMaxForce; }
        f32                                     getMoveForce() const { return m_moveForce; }

    private:

        class AIAction_Template*                m_swim;
        class AIJumpAction_Template*            m_jump;
        f32                                     m_pushMinSpeed;
        f32                                     m_pushMaxSpeed;
        f32                                     m_pushMinForce;
        f32                                     m_pushMaxForce;
        f32                                     m_moveForce;
    };

    ITF_INLINE const class Ray_AIRedWizardWaterSwimBehavior_Template * Ray_AIRedWizardWaterSwimBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIRedWizardWaterSwimBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIREDWIZARDWATERSWIMBEHAVIOR_H_

