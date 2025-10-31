#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ELEVATORMONSTERAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_ElevatorMonsterAIComponent.h"
#endif //_ITF_RAY_ELEVATORMONSTERAICOMPONENT_H_

#ifndef _ITF_RAY_ELEVATORAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_ElevatorAIComponent.h"
#endif //_ITF_Ray_ElevatorAIComponent_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_ElevatorMonsterAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ElevatorMonsterAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_ElevatorMonsterAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent,   "Ray_ElevatorMonsterAIComponent requires an LinkComponent.");
        VALIDATE_COMPONENT_PARAM("", !m_actor->getParentBind(),      "This actor shouldn't be binding because it's moving by itself in the world");
    END_VALIDATE_COMPONENT() 


    //*****************************************************************************

    Ray_ElevatorMonsterAIComponent::Ray_ElevatorMonsterAIComponent()
    :
    m_linkComponent(NULL),
    m_launchAttack(bfalse),
    m_countDownAttack(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_ElevatorMonsterAIComponent::~Ray_ElevatorMonsterAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_ElevatorMonsterAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get Components
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        ITF_ASSERT_MSG(m_linkComponent, "Ray_DanceStoneComponent requires an LinkComponent.");

        // Get Actor (Elevator) linked
        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList& childList  = m_linkComponent->getChildren();
            ITF_WARNING(m_actor, childList.size() != 0, "This ElevatorMonster is not link with a Elevator !");
            ITF_WARNING(m_actor, childList.size() <= 1, "This ElevatorMonster is link with too many actor !");
            
            for (LinkComponent::ChildrenList::const_iterator it = childList.begin(); it != childList.end(); ++it)
            {
                const ObjectPath& objPath = it->getPath();
                Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objPath);

                if (pickable)
                {
                    m_elevator = pickable->getRef();
                    break;
                }
            }
        }

    }

    //*****************************************************************************

    void Ray_ElevatorMonsterAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        Actor* elevatorAct = m_elevator.getActor();
        if (elevatorAct)
        {
            Ray_ElevatorAIComponent* elevatorComponent = elevatorAct->GetComponent<Ray_ElevatorAIComponent>();
            if (elevatorComponent)
            {
                // Update start for attack
                if (!m_launchAttack)
                {
                    m_launchAttack = elevatorComponent->getLaunchPursuit();
                }

                if (m_launchAttack)
                {
                    Vec2d   posElevator     = elevatorComponent->getBoneDetectPos();
                    Vec2d   posCur          = m_actor->get2DPos();
                    f32     dist            = (posElevator - posCur).norm();
                
                    // TODO : METTRE UN WARNING SI ON EST BINDE!!!!!!!!!

                    if (m_countDownAttack <= 0.0f)
                    {
                        m_actor->set2DPos(posCur + ((Vec2d::Up * dist) * getTemplate()->getRatioSpeed())); 
                    }
   
#ifndef ITF_FINAL
                    //DebugDraw::text("Dist : %f", dist);
#endif // ITF_FINAL

                    if (dist < getTemplate()->getDistanceForHit())
                    { 
                        elevatorComponent->hit();
                        m_countDownAttack = getTemplate()->getCountDownAttack();
                    }
                }
            }
        }

        m_countDownAttack -= _dt;
        m_countDownAttack = Max(m_countDownAttack, 0.0f);

    }

	//*****************************************************************************
	// Template
	//*****************************************************************************
	
	IMPLEMENT_OBJECT_RTTI(Ray_ElevatorMonsterAIComponent_Template)

    BEGIN_SERIALIZATION(Ray_ElevatorMonsterAIComponent_Template)
        SERIALIZE_MEMBER("ratioSpeed",          m_ratioSpeed);
        SERIALIZE_MEMBER("distanceForHit",      m_distanceForHit);
        SERIALIZE_MEMBER("countDownAttack",     m_countDownAttack);
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_ElevatorMonsterAIComponent_Template::Ray_ElevatorMonsterAIComponent_Template()
    :
    m_ratioSpeed(0.005f),
    m_distanceForHit(4.0f),
    m_countDownAttack(2.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_ElevatorMonsterAIComponent_Template::~Ray_ElevatorMonsterAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************

	
};