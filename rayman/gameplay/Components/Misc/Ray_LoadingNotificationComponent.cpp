#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LOADINGNOTIFICATIONCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_LoadingNotificationComponent.h"
#endif //_ITF_RAY_LOADINGNOTIFICATIONCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_LoadNotificationComponent)

BEGIN_SERIALIZATION_CHILD(Ray_LoadNotificationComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("RELATIVEPOS",m_relPos);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_LoadNotificationComponent)
    VALIDATE_COMPONENT_PARAM("", m_animComponent, "AnimComponent mandatory");
END_VALIDATE_COMPONENT()

Ray_LoadNotificationComponent::Ray_LoadNotificationComponent()
: m_animComponent(NULL)
, m_relPos(0.8f,0.8f)
{
}

Ray_LoadNotificationComponent::~Ray_LoadNotificationComponent()
{
}

void Ray_LoadNotificationComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if ( RAY_GAMEMANAGER->getLoadNotificationActor() == m_actor->getRef() )
    {
        RAY_GAMEMANAGER->setLoadNotificationActor(ActorRef::InvalidRef);
    }
}

void Ray_LoadNotificationComponent::onBecomeActive()
{
    Super::onBecomeActive();

    m_animComponent->resetCurTime();
}

void Ray_LoadNotificationComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        
    RAY_GAMEMANAGER->setLoadNotificationActor(m_actor->getRef());
}

void Ray_LoadNotificationComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    f32 screenWidth = static_cast<f32>(GFX_ADAPTER->getScreenWidth());
    f32 screenHeight = static_cast<f32>(GFX_ADAPTER->getScreenHeight());
    f32 ratioX = screenWidth/UI2D_WidthRef;
    f32 ratioY = screenHeight/UI2D_HeightRef;
    f32 screenPosX = m_relPos.m_x*screenWidth;
    f32 screenPosY = m_relPos.m_y*screenHeight;
    f32 scaleX = getTemplate()->getAnimSize().m_x * ratioX;
    f32 scaleY = getTemplate()->getAnimSize().m_y * ratioY;
    Vec2d p(screenPosX, screenPosY);
    Vec2d scale(scaleX,scaleY);

    m_actor->set2DPos(p);
    m_actor->setScale(scale);
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_LoadNotificationComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    m_relPos.m_x = m_actor->getPos().m_x / (f32)GFX_ADAPTER->getScreenWidth();
    m_relPos.m_y = m_actor->getPos().m_y / (f32)GFX_ADAPTER->getScreenHeight();        
}
#endif

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_LoadNotificationComponent_Template)
BEGIN_SERIALIZATION(Ray_LoadNotificationComponent_Template)

    SERIALIZE_MEMBER("animSize",m_animSize);

END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_LoadNotificationComponent_Template::Ray_LoadNotificationComponent_Template()
: m_animSize(16.f,16.f)
{
}

//------------------------------------------------------------------------------
Ray_LoadNotificationComponent_Template::~Ray_LoadNotificationComponent_Template()
{
}

} // namespace ITF
