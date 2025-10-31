#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SKIPLEVELCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SkipLevelComponent.h"
#endif //_ITF_RAY_SKIPLEVELCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeManagerComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


#ifndef _ITF_INGAMETEXTCOMPONENT_H_
#include "gameplay/Components/UI/InGameTextComponent.h"
#endif //_ITF_INGAMETEXTCOMPONENT_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SkipLevelComponent)
BEGIN_SERIALIZATION_CHILD(Ray_SkipLevelComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("RELATIVEPOS",m_relativePos);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_SkipLevelComponent)
END_VALIDATE_COMPONENT()

Ray_SkipLevelComponent::Ray_SkipLevelComponent()
: m_animComponent(NULL)
, m_state(State_Inactive)
{
}

Ray_SkipLevelComponent::~Ray_SkipLevelComponent()
{
}

void Ray_SkipLevelComponent::onActorClearComponents()
{
    Super::onActorClearComponents();
}

void Ray_SkipLevelComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    ITF_ASSERT(m_animComponent);
    m_animComponent->setRank2D(getTemplate()->getRank());

    RAY_GAMEMANAGER->setSkipLevelActor(m_actor->getRef());

    updatePosition();
    
    changeState(State_Inactive);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    m_animComponent->setAlpha(0.0f);

}

void Ray_SkipLevelComponent::onBecomeActive()
{
    Super::onBecomeActive();
}

void Ray_SkipLevelComponent::onBecomeInactive()
{
    Super::onBecomeInactive();
}

void Ray_SkipLevelComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    switch (m_state)
    {
    case  State_Prepare:
        m_animComponent->setAlpha(1.0f);
            changeState(State_Appearing);
        break;
    case State_Appearing:
        if (AIUtils::isAnimFinished(m_animComponent,NULL))
        {
            changeState(State_Talking);
            m_animComponent->setAnim(getTemplate()->getTalkAnim());
        }
        break;
    case State_Talking:
        break;
    case State_Disappearing:
        if (AIUtils::isAnimFinished(m_animComponent,NULL))
        {
            changeState(State_Inactive);
        }
    default:
        break;
    }
    updatePosition();
}

void Ray_SkipLevelComponent::changeState( State _state )
{
    m_state = _state;
    switch (_state)
    {
    case State_Prepare:
        break;
    case State_Appearing:
        m_animComponent->setAnim(getTemplate()->getAppearAnim());
        break;
    case State_Talking:
        break;
    case State_Disappearing:
        m_animComponent->setAnim(getTemplate()->getDisappearAnim());
        break;
    case State_Inactive:
        m_animComponent->setAlpha(0.0f);
        break;
    }
}
void Ray_SkipLevelComponent::updatePosition()
{
    Vec2d pos;
    f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
    f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();

    pos.m_x = screenWidth * getTemplate()->getScreenPos().m_x;
    pos.m_y = screenHeight * getTemplate()->getScreenPos().m_y;

    m_actor->set2DPos(pos);

    Vec2d newScale = getTemplate()->getAnimSize();
    Vec2d screenRatio = UIComponent::getScreenRatio();
    newScale *= screenRatio;

    m_actor->setScale(newScale);

}


void Ray_SkipLevelComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);
    if (EventTrigger * onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if (onTrigger->getActivated())
        {
            changeState(State_Prepare);
        }
        else
        {
            changeState(State_Disappearing);
        }
    }
}

void Ray_SkipLevelComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

}   


#ifdef ITF_SUPPORT_EDITOR
void Ray_SkipLevelComponent::onEditorCreated( class Actor* _original )
{
    Super::onEditorCreated(_original);

    // Calculate the relative position
    m_relativePos.m_x = m_actor->getPos().m_x / (f32)GFX_ADAPTER->getScreenWidth();
    m_relativePos.m_y = m_actor->getPos().m_y / (f32)GFX_ADAPTER->getScreenHeight();
}

void Ray_SkipLevelComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    // Calculate the relative position
    m_relativePos.m_x = m_actor->getPos().m_x / (f32)GFX_ADAPTER->getScreenWidth();
    m_relativePos.m_y = m_actor->getPos().m_y / (f32)GFX_ADAPTER->getScreenHeight();
}



#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SkipLevelComponent_Template)
BEGIN_SERIALIZATION(Ray_SkipLevelComponent_Template)
SERIALIZE_MEMBER("textPath",m_textPath);
SERIALIZE_MEMBER("animSize",m_animSize);
SERIALIZE_MEMBER("textAnimSize",m_textAnimSize);
SERIALIZE_MEMBER("screenPos",m_screenPos);
SERIALIZE_MEMBER("textScreenPos",m_textScreenPos);
SERIALIZE_MEMBER("appearAnim",m_appearAnim);
SERIALIZE_MEMBER("disappearAnim",m_disappearAnim);
SERIALIZE_MEMBER("talkAnim",m_talkAnim);
SERIALIZE_MEMBER("locId",m_locId);
SERIALIZE_MEMBER("rank",m_rank);
SERIALIZE_MEMBER("textRank",m_textRank);
SERIALIZE_MEMBER("contextIconRank",m_contextIconRank);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_SkipLevelComponent_Template::Ray_SkipLevelComponent_Template()
: m_animSize(60.0f,60.0f)
, m_textAnimSize(60.0f,60.0f)
, m_screenPos(0.5f,0.5f)
, m_textScreenPos(0.75f,0.75f)
, m_rank(11)
, m_textRank(10)
, m_contextIconRank(7)
{
}

//------------------------------------------------------------------------------
Ray_SkipLevelComponent_Template::~Ray_SkipLevelComponent_Template()
{
}

} // namespace ITF
