#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LEVELTITLECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_LevelTitleComponent.h"
#endif //_ITF_RAY_LEVELTITLECOMPONENT_H_

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
IMPLEMENT_OBJECT_RTTI(Ray_LevelTitleComponent)
BEGIN_SERIALIZATION_CHILD(Ray_LevelTitleComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_LevelTitleComponent)
END_VALIDATE_COMPONENT()

Ray_LevelTitleComponent::Ray_LevelTitleComponent()
{
}

Ray_LevelTitleComponent::~Ray_LevelTitleComponent()
{
}

void Ray_LevelTitleComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    RAY_GAMEMANAGER->setLevelTitleActor(m_actor->getRef());
}


//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_LevelTitleComponent_Template)
BEGIN_SERIALIZATION(Ray_LevelTitleComponent_Template)
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_LevelTitleComponent_Template::Ray_LevelTitleComponent_Template()
{
}

//------------------------------------------------------------------------------
Ray_LevelTitleComponent_Template::~Ray_LevelTitleComponent_Template()
{
}

} // namespace ITF
