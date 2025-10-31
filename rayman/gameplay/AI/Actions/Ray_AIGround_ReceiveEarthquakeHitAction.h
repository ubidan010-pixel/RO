#ifndef _ITF_RAY_AIGROUND_RECEIVEEARTHQUAKEHITACTION_H_
#define _ITF_RAY_AIGROUND_RECEIVEEARTHQUAKEHITACTION_H_

#ifndef _ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveNormalHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_

namespace ITF
{
    class Ray_AIGround_ReceiveEarthquakeHitAction : public Ray_AIGround_ReceiveNormalHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveEarthquakeHitAction, Ray_AIGround_ReceiveNormalHitAction,3116000201);
        DECLARE_SERIALIZE()

    public:

        Ray_AIGround_ReceiveEarthquakeHitAction();
        virtual ~Ray_AIGround_ReceiveEarthquakeHitAction();

    protected:

        virtual void    setupHitForce();

    private:

        ITF_INLINE const class Ray_AIGround_ReceiveEarthquakeHitAction_Template* getTemplate() const;

        bbool           m_ejected;
    };

    class Ray_AIGround_ReceiveEarthquakeHitAction_Template : public Ray_AIGround_ReceiveNormalHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveEarthquakeHitAction_Template,Ray_AIGround_ReceiveNormalHitAction_Template,1744635826);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIGround_ReceiveEarthquakeHitAction);

    public:

        Ray_AIGround_ReceiveEarthquakeHitAction_Template();
        ~Ray_AIGround_ReceiveEarthquakeHitAction_Template() {}

        f32             getEjectProbability() const { return m_ejectProbability; }

    private:

        f32             m_ejectProbability;
    };

    ITF_INLINE const class Ray_AIGround_ReceiveEarthquakeHitAction_Template* Ray_AIGround_ReceiveEarthquakeHitAction::getTemplate() const { return static_cast<const Ray_AIGround_ReceiveEarthquakeHitAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AIGROUND_RECEIVEEARTHQUAKEHITACTION_H_