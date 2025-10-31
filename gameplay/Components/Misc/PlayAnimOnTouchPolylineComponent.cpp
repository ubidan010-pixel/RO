#include "precompiled_gameplay.h"

#ifndef _ITF_PLAYANIMONTOUCHPOLYLINECOMPONENT_H_
#include "gameplay/components/misc/PlayAnimOnTouchPolylineComponent.h"
#endif //_ITF_PLAYANIMONTOUCHPOLYLINECOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PlayAnimOnTouchPolylineComponent)

BEGIN_SERIALIZATION_CHILD(PlayAnimOnTouchPolylineComponent)
END_SERIALIZATION()


PlayAnimOnTouchPolylineComponent::PlayAnimOnTouchPolylineComponent()
: m_animComponent(NULL)
, m_polyComponent(NULL)
, m_timer(0.0f)
, m_animPos(0.f)
, m_animSpeed(0.f)
{
}

PlayAnimOnTouchPolylineComponent::~PlayAnimOnTouchPolylineComponent()
{
}

void PlayAnimOnTouchPolylineComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_animComponent = m_actor->GetComponent<AnimatedComponent>();
    m_polyComponent = m_actor->GetComponent<PolylineComponent>();

    if ( m_animComponent )
    {
        m_animComponent->setUpdateAnimInput(this);
    }
}

void PlayAnimOnTouchPolylineComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    m_timer += _deltaTime;

    if ( m_polyComponent )
    {
        f32 targetSpeed;

        if ( m_polyComponent->hasActors() )
        {
            targetSpeed = getTemplate()->getMaxSpeed();
        }
        else
        {
            targetSpeed = -getTemplate()->getMaxSpeed();
        }

        f32 f = ( ( targetSpeed - m_animSpeed ) * getTemplate()->getStiff() ) - ( m_animSpeed * getTemplate()->getDamp() );

        m_animSpeed += f * _deltaTime;
        m_animPos += m_animSpeed*_deltaTime;

        m_animPos = Clamp(m_animPos,0.f,1.f);
    }
}
void PlayAnimOnTouchPolylineComponent::validate( bbool &_isComponentValidated )
{
    Super::validate(_isComponentValidated);

    VALIDATE_COMPONENT_PARAM("PlayAnimOnTouchPolylineComponent::polylineComponent",m_polyComponent!=NULL,
        "Missing Polyline Component on %s",
        m_actor->getUserFriendly().cStr());

    VALIDATE_COMPONENT_PARAM("PlayAnimOnTouchPolylineComponent::animatedComponent",m_animComponent!=NULL,
        "Missing Animated Component on %s",
        m_actor->getUserFriendly().cStr());
}

void PlayAnimOnTouchPolylineComponent::updateAnimInput()
{
    static StringID s_Time = "Time";
    static StringID s_AnimPos = "AnimPos";

    m_animComponent->setInput(s_Time,m_timer);
    m_animComponent->setInput(s_AnimPos,m_animPos);
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PlayAnimOnTouchPolylineComponent_Template)
BEGIN_SERIALIZATION(PlayAnimOnTouchPolylineComponent_Template)

    SERIALIZE_MEMBER("maxSpeed",m_maxSpeed);
    SERIALIZE_MEMBER("stiff",m_stiff);
    SERIALIZE_MEMBER("damp",m_damp);

END_SERIALIZATION()

PlayAnimOnTouchPolylineComponent_Template::PlayAnimOnTouchPolylineComponent_Template()
: m_maxSpeed(1.f)
, m_stiff(5.f)
, m_damp(1.f)
{
}

}

