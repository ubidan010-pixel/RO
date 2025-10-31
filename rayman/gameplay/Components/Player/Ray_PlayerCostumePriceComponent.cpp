#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCOSTUMEPRICECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumePriceComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEPRICECOMPONENT_H_

#ifndef _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeManagerComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

namespace ITF
{
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PlayerCostumePriceComponent)

BEGIN_SERIALIZATION_CHILD(Ray_PlayerCostumePriceComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_PlayerCostumePriceComponent)
    VALIDATE_COMPONENT_PARAM("AnimComponent",m_animComponent!=NULL, "Needs an anim component");
    VALIDATE_COMPONENT_PARAM("TextComponent",m_textComponent!=NULL, "Needs a text component");
END_VALIDATE_COMPONENT()

Ray_PlayerCostumePriceComponent::Ray_PlayerCostumePriceComponent()
: m_animComponent(NULL)
{
}

Ray_PlayerCostumePriceComponent::~Ray_PlayerCostumePriceComponent()
{
}

void Ray_PlayerCostumePriceComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();
    m_textComponent = m_actor->GetComponent<UITextBoxesComponent>();

    getTemplate()->getElectoonPath().getString(m_electoonPath);
}

void Ray_PlayerCostumePriceComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    bbool visible = bfalse;

    Actor* actor = AIUtils::getActor(RAY_GAMEMANAGER->getCostumeManager());

    if ( actor )
    {
        Ray_PlayerCostumeManagerComponent* manager = actor->GetComponent<Ray_PlayerCostumeManagerComponent>();

        if ( manager )
        {
            u32 prize;
            ObjectRef nextCostume = manager->getNextCostumeAvailable(prize);

            if ( nextCostume != ITF_INVALID_OBJREF )
            {
                Actor* actorCostume = AIUtils::getActor(nextCostume);

                if ( actorCostume )
                {
                    visible = btrue;
                    
                    Vec3d targetPos = actorCostume->getPos();

                    targetPos.m_x += getTemplate()->getOffset().m_x;
                    targetPos.m_y += getTemplate()->getOffset().m_y;

                    m_actor->setPos(targetPos);

                    m_prizeStr.setTextFormat("%u [actor:%s]",prize,m_electoonPath.cStr());
                    m_textComponent->setText(getTemplate()->getBoneName(),m_prizeStr);
                }
            }
        }
    }

    if ( visible != m_animComponent->isDrawEnabled() )
    {
        m_animComponent->enableDraw(visible);
    }
}

//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PlayerCostumePriceComponent_Template)
BEGIN_SERIALIZATION(Ray_PlayerCostumePriceComponent_Template)

    SERIALIZE_MEMBER("offset",m_offset);
    SERIALIZE_MEMBER("bone",m_boneName);
    SERIALIZE_MEMBER("electoonPath",m_electoonPath);

END_SERIALIZATION()

Ray_PlayerCostumePriceComponent_Template::Ray_PlayerCostumePriceComponent_Template()
: m_offset(0.f,2.f)
{
}

Ray_PlayerCostumePriceComponent_Template::~Ray_PlayerCostumePriceComponent_Template()
{

}

}

