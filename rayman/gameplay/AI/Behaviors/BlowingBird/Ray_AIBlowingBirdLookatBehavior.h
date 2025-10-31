#ifndef _ITF_RAY_AIBLOWINGBIRDLOOKATBEHAVIOR_H_
#define _ITF_RAY_AIBLOWINGBIRDLOOKATBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AIBlowingBirdLookatBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBlowingBirdLookatBehavior, AIBehavior,284233621);

    public:

        Ray_AIBlowingBirdLookatBehavior();
        virtual ~Ray_AIBlowingBirdLookatBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );

        virtual void                        onActivate();

        virtual void                        onActionFinished();

        virtual void                        updateAnimInputs();
    protected:
        ITF_INLINE const class Ray_AIBlowingBirdLookatBehavior_Template * getTemplate() const;

        void startUturn();
        void onFinishedUturn();


        void startAttack();
        void updateAttack( f32 _delta);
        void onFinishedAttack();

        void    updateLookAt(f32 _dt);
    private:
        AIAction *  m_uturn;
        AIAction *  m_attack;
        Angle       m_lookAtAngle;
        f32         m_timer;
        f32         m_angle;
    };

    class Ray_AIBlowingBirdLookatBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBlowingBirdLookatBehavior_Template,TemplateAIBehavior,927686986);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIBlowingBirdLookatBehavior);

    public:

        Ray_AIBlowingBirdLookatBehavior_Template();
        ~Ray_AIBlowingBirdLookatBehavior_Template();

        ITF_INLINE const AIAction_Template * getUturn() const { return m_uturn; }
        ITF_INLINE const AIAction_Template * getAttack() const { return m_attack; }
        ITF_INLINE f32 getBeginDuration() const { return m_beginDuration; }
        ITF_INLINE Angle getRotationSpeed() const { return m_rotationSpeed; }
        ITF_INLINE Angle getMinAngle() const { return m_minAngle; }
        ITF_INLINE Angle getMaxAngle() const { return m_maxAngle; }

    private:

        AIAction_Template * m_uturn;
        AIAction_Template * m_attack;
        f32 m_beginDuration;
        Angle m_rotationSpeed;
        Angle m_minAngle;
        Angle m_maxAngle;
    };
    ITF_INLINE const Ray_AIBlowingBirdLookatBehavior_Template * Ray_AIBlowingBirdLookatBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIBlowingBirdLookatBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIBLOWINGBIRDLOOKATBEHAVIOR_H_