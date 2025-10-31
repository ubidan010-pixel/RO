#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DarktoonTrapHoleComponent.h"
#endif //_ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_DarktoonTrapHoleComponent)
BEGIN_SERIALIZATION_CHILD(Ray_DarktoonTrapHoleComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_DarktoonTrapHoleComponent)
    VALIDATE_COMPONENT_PARAM("", m_animatedComponent, "AnimatedComponent mandatory");
    VALIDATE_COMPONENT_PARAM("action", getTemplate()->getAnim().isValid(), "Anim name mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_DarktoonTrapHoleComponent::Ray_DarktoonTrapHoleComponent()
    : Super()
    , m_openHole(bfalse)
    , m_animatedComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_DarktoonTrapHoleComponent::~Ray_DarktoonTrapHoleComponent()
{
}

//------------------------------------------------------------------------------
void Ray_DarktoonTrapHoleComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    // play anim + set update input callback
    if (m_animatedComponent && getTemplate()->getAnim().isValid())
    {
        m_animatedComponent->setUpdateAnimInput(this);
        m_animatedComponent->setAnim(getTemplate()->getAnim());
    }
}

//------------------------------------------------------------------------------
void Ray_DarktoonTrapHoleComponent::updateAnimInput()
{
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(OpenHole,980676740), m_openHole);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_DarktoonTrapHoleComponent_Template)
BEGIN_SERIALIZATION(Ray_DarktoonTrapHoleComponent_Template)
    SERIALIZE_MEMBER("anim", m_anim);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_DarktoonTrapHoleComponent_Template::Ray_DarktoonTrapHoleComponent_Template()
    : Super()
    , m_anim()
{
}

//------------------------------------------------------------------------------
Ray_DarktoonTrapHoleComponent_Template::~Ray_DarktoonTrapHoleComponent_Template()
{
}

} // namespace ITF
