#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBASICBULLETACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIBasicBulletAction.h"
#endif //_ITF_RAY_AIBASICBULLETACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIBasicBulletAction)
    BEGIN_VALIDATE_ACTION(Ray_AIBasicBulletAction)
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
Ray_AIBasicBulletAction::Ray_AIBasicBulletAction()
    : Super()
{
}

//------------------------------------------------------------------------------
Ray_AIBasicBulletAction::~Ray_AIBasicBulletAction()
{
}

//------------------------------------------------------------------------------
void Ray_AIBasicBulletAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    m_bulletAbility.init(m_actor, getTemplate()->getBasicBulletTemplate());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetDirection,187560938),m_aiComponent);
}

//------------------------------------------------------------------------------
void Ray_AIBasicBulletAction::onActivate()
{
    Super::onActivate();
    m_bulletAbility.onBecomeActive();

    if ( m_physComponent != NULL )
    {
        m_physComponent->setDisabled( btrue );
    }
}
//------------------------------------------------------------------------------
void Ray_AIBasicBulletAction::onDeactivate()
{
    Super::onDeactivate();
    m_bulletAbility.onBecomeInactive();

    if ( m_physComponent != NULL )
    {
        m_physComponent->setDisabled( bfalse );
    }
}	

//------------------------------------------------------------------------------
void Ray_AIBasicBulletAction::update( f32 _dt )
{
    Super::update(_dt);

    m_bulletAbility.update(_dt);
    if (m_bulletAbility.isObsolete())
    {
        m_aiBehavior->onActionFinished();
        m_aiComponent->setHealth(0);
    }
}

//------------------------------------------------------------------------------
void Ray_AIBasicBulletAction::onEvent(Event* _event)
{
    Super::onEvent(_event);

    if (EventSetDirection* _dir = _event->DynamicCast<EventSetDirection>(ITF_GET_STRINGID_CRC(EventSetDirection,187560938)))
    {
        if (getTemplate()->getHasOwner())
        {
            m_bulletAbility.setOwner(_dir->getSender());
        }
        m_bulletAbility.setMoveDir(_dir->getDirection());
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIBasicBulletAction_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIBasicBulletAction_Template)
    SERIALIZE_OBJECT("basicBullet", m_basicBullet);
    SERIALIZE_MEMBER("hasOwner", m_hasOwner);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIBasicBulletAction_Template::Ray_AIBasicBulletAction_Template()
    : Super()
    , m_basicBullet()
    , m_hasOwner(bfalse)
{
}

} // namespace ITF
