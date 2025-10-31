#ifndef _ITF_RAY_AIGROUNDRECEIVEHITACTION_H_
#define _ITF_RAY_AIGROUNDRECEIVEHITACTION_H_

#ifndef _ITF_RAY_AIRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIReceiveHitAction.h"
#endif //_ITF_RAY_AIRECEIVEHITACTION_H_

namespace ITF
{

    class Ray_AIGroundReceiveHitAction : public Ray_AIReceiveHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundReceiveHitAction, Ray_AIReceiveHitAction,1900720002);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_ACTION()

    public:

        Ray_AIGroundReceiveHitAction();

        virtual void onActivate();
        virtual void onDeactivate();
        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    protected:
        StickToPolylinePhysComponent * m_physComponent;
        virtual void                        updateHit( f32 _dt );
        bbool                               m_justLanded;
        bbool                               m_previouslyInAir;

    private:
        void                                processStimNotify( EventStimNotify* _stimNotify );

        ITF_INLINE const class Ray_AIGroundReceiveHitAction_Template* getTemplate() const;
        
        f32                                 m_previousUnstickMultiplier;
    };

    class Ray_AIGroundReceiveHitAction_Template : public Ray_AIReceiveHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundReceiveHitAction_Template,Ray_AIReceiveHitAction_Template,1674581096);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIGroundReceiveHitAction);

    public:

        Ray_AIGroundReceiveHitAction_Template();
        ~Ray_AIGroundReceiveHitAction_Template() {}

        f32                                 getUnstickMultiplier() const { return m_unstickMultiplier; }

    private:

        f32                                 m_unstickMultiplier;
    };

    ITF_INLINE const class Ray_AIGroundReceiveHitAction_Template* Ray_AIGroundReceiveHitAction::getTemplate() const { return static_cast<const Ray_AIGroundReceiveHitAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AIGROUNDRECEIVEHITACTION_H_