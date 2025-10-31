#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUND_RECEIVEEARTHQUAKEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveEarthquakeHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVEEARTHQUAKEHITACTION_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveEarthquakeHitAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGround_ReceiveEarthquakeHitAction_Template)
        SERIALIZE_MEMBER("ejectProbability", m_ejectProbability);
    END_SERIALIZATION()

    Ray_AIGround_ReceiveEarthquakeHitAction_Template::Ray_AIGround_ReceiveEarthquakeHitAction_Template()
    : m_ejectProbability(0.5f)
    {

    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveEarthquakeHitAction)

    Ray_AIGround_ReceiveEarthquakeHitAction::Ray_AIGround_ReceiveEarthquakeHitAction()
        : Super()
        , m_ejected(bfalse)
    {
    }

    Ray_AIGround_ReceiveEarthquakeHitAction::~Ray_AIGround_ReceiveEarthquakeHitAction()
    {
    }

    void Ray_AIGround_ReceiveEarthquakeHitAction::setupHitForce()
    {
        if ( const PolyLineEdge* edge = m_physComponent->getStickedEdge() )
        {
            m_ejected = Seeder::getSharedSeeder().GetFloat() < getTemplate()->getEjectProbability();
        }
        else
        {
            // the crush anim only works when sticked: always eject
            m_ejected = btrue;
        }

        if ( m_ejected )
        {
            Super::setupHitForce();
        }
        // else: no ejection, just play the crush anim

        if (m_animatedComponent)
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Ejected,1034487922), m_ejected);
    }

}