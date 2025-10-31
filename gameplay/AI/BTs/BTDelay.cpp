
#include "precompiled_gameplay.h"

#ifndef _ITF_BTDELAY_H_
#include "gameplay/ai/BTs/BTDelay.h"
#endif // _ITF_BTDELAY_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTDelay);

BTDelay::BTDelay()
: m_waitTime(0.f)
{
}

BTDelay::~BTDelay()
{
}

void BTDelay::onActivate()
{
    Super::onActivate();

    resetTime();
}

bbool BTDelay::update( f32 _dt )
{
    Super::update(_dt);

    if ( m_waitTime > 0.f )
    {
        m_waitTime -= _dt;
    }

    return m_waitTime > 0.f;
}

void BTDelay::resetTime()
{
    m_waitTime = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinTime(),getTemplate()->getMaxTime());
}

IMPLEMENT_OBJECT_RTTI(BTDelay_Template);

BEGIN_SERIALIZATION_CHILD(BTDelay_Template)

    SERIALIZE_MEMBER("minTime",m_minTime);
    SERIALIZE_MEMBER("maxTime",m_maxTime);

END_SERIALIZATION()

BTDelay_Template::BTDelay_Template()
: m_minTime(1.f)
, m_maxTime(2.f)
{
}

BTDelay_Template::~BTDelay_Template()
{
}

}
