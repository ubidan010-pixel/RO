#ifndef _ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_
#define _ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    //forward
    class Ray_AIBoulderSentBackBehavior_Template;

    //------------------------------------------------------------------
    class Ray_AIBoulderSentBackBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBoulderSentBackBehavior, AIBehavior,3013093676);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIBoulderSentBackBehavior();
        virtual ~Ray_AIBoulderSentBackBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void                        update(f32 _dt);
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        //virtual void                        onEvent(Event * _event);

        ITF_INLINE void setDirection(const Vec2d & _direction);
        ITF_INLINE void setSpeedValue(const f32 _speedValue);
        ITF_INLINE f32 getTimer() const;

    protected:
        ITF_INLINE const Ray_AIBoulderSentBackBehavior_Template * getTemplate() const;

        //state management
        void updateFlight(const f32 _dt);
        void updateActorAngle(const f32 _dt);

        //Data
    private: 
        PhysComponent * m_physComponent;
        f32 m_behaviorTimer;
        f32 m_speedValue;
        Vec2d m_direction;
        Vec2d m_speed;
    };

    //------------------------------------------------------------------
    class Ray_AIBoulderSentBackBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBoulderSentBackBehavior_Template,TemplateAIBehavior,3800231555);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIBoulderSentBackBehavior);

    public:

        Ray_AIBoulderSentBackBehavior_Template();
        ~Ray_AIBoulderSentBackBehavior_Template();

        ITF_INLINE f32 getGravity() const;
        ITF_INLINE f32 getFriction() const;


        //Data
    private:
        f32 m_gravity;
        f32 m_friction;
    };

    //------------------------------------------------------------------
    ITF_INLINE const Ray_AIBoulderSentBackBehavior_Template * Ray_AIBoulderSentBackBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIBoulderSentBackBehavior_Template *>(m_template);
    }

    //------------------------------------------------------------------
    ITF_INLINE void Ray_AIBoulderSentBackBehavior::setDirection(const Vec2d &_direction)
    {
        m_direction = _direction;
    }

    ITF_INLINE void Ray_AIBoulderSentBackBehavior::setSpeedValue(const f32 _speedValue)
    {
        m_speedValue = _speedValue;
    }

    //------------------------------------------------------------------
    ITF_INLINE f32 Ray_AIBoulderSentBackBehavior::getTimer() const
    {
        return m_behaviorTimer;
    }


    //------------------------------------------------------------------
    ITF_INLINE f32 Ray_AIBoulderSentBackBehavior_Template::getGravity() const
    {
        return m_gravity;
    }

    //------------------------------------------------------------------
    ITF_INLINE f32 Ray_AIBoulderSentBackBehavior_Template::getFriction() const
    {
        return m_friction;
    }
}

#endif //_ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_


