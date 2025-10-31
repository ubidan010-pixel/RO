#include "precompiled_gameplay.h"

#ifndef _ITF_SAVENOTIFICATIONCOMPONENT_H_
#include "gameplay/Components/Misc/SaveNotificationComponent.h"
#endif //_ITF_SAVENOTIFICATIONCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(SaveNotificationComponent)
BEGIN_SERIALIZATION_CHILD(SaveNotificationComponent)
END_SERIALIZATION()

SaveNotificationComponent::SaveNotificationComponent()
: Super()
, m_timeStarted(0.0)
, m_isRegistered(bfalse)
{
}

SaveNotificationComponent::~SaveNotificationComponent()
{
    hide();
    if(m_isRegistered)
        GAMEMANAGER->unregisterSaveNotification();
}

void SaveNotificationComponent::Draw2D()
{
    Super::Draw2D();
}

void SaveNotificationComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    if (LOADINGSCREEN->getRef() != getWorld()->getRef())
    {
        // Not loaded from the loading world
        GAMEMANAGER->registerSaveNotification(GetActor()->getRef());
        
        m_isRegistered = btrue;
    }

    GetActor()->disableDraw(btrue);
}

void SaveNotificationComponent::show()
{
    if(m_timeStarted==0.0)
    {
        GetActor()->disableDraw(bfalse);
    }

    if(m_isRegistered)
    {
        if(!UI_MENUMANAGER->isDisplayingPriority(GAMEINTERFACE->getNotificationMenuPriority()))
            UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getNotificationMenuPriority(), ITF_GET_STRINGID_CRC(trcsavenotification,1066623640), btrue, NULL);
    }

    m_timeStarted = SYSTEM_ADAPTER->getTime();

    UIComponent* pUIComponent = GetActor()->GetComponent<UIComponent>();
    if(pUIComponent)
    {
        pUIComponent->setRelativePos(getTemplate()->getRelativePosition());
    }    
}

void SaveNotificationComponent::hide()
{
    if(m_timeStarted==0.0) return;
    
    GetActor()->disableDraw(btrue);    
    LOG("SaveNotification stopped: %.3f", SYSTEM_ADAPTER->getTime()-m_timeStarted);
    m_timeStarted = 0.0;        
}

void SaveNotificationComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    if (isDisplayed() && !SAVEGAME_ADAPTER->isSaving() && canHideNotification())
    {
        hide();
    }

    if( (LOADINGSCREEN->isVisible() && !isDisplayed()) && (SAVEGAME_ADAPTER->isSaving() || !canHideNotification()))
    {
        show();
    }
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(SaveNotificationComponent_Template)
BEGIN_SERIALIZATION(SaveNotificationComponent_Template)
SERIALIZE_MEMBER("minDisplayingTime", m_minDisplayingTime);
SERIALIZE_MEMBER("relativepos", m_relativePosition);
END_SERIALIZATION()
}

