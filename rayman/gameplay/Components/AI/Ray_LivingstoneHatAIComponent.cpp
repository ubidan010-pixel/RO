#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LIVINGSTONEHATAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LivingstoneHatAIComponent.h"
#endif //_ITF_RAY_LIVINGSTONEHATAICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_


#ifndef _ITF_RAY_AIWATERBASEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterBaseBehavior.h"
#endif //_ITF_RAY_AIWATERBASEBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_LivingstoneHatAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_LivingstoneHatAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_LivingstoneHatAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_physComponent, "Ray_FruitAIComponent requires a StickToPolylinePhysComponent");
    END_VALIDATE_COMPONENT()


    //************************************
    // @name	ITF::Ray_LivingstoneHatAIComponent::Ray_LivingstoneHatAIComponent
    // @public 
    // @return	
    //************************************
    Ray_LivingstoneHatAIComponent::Ray_LivingstoneHatAIComponent() 
        : Super()
        , m_roamBehavior(NULL)
        , m_floatingBehavior(NULL)
    {
        
    }

    //************************************
    // @name	ITF::Ray_LivingstoneHatAIComponent::~Ray_LivingstoneHatAIComponent
    // @public 
    // @return	
    //************************************
    Ray_LivingstoneHatAIComponent::~Ray_LivingstoneHatAIComponent()
    {
    }
    void Ray_LivingstoneHatAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_roamBehavior = createAiBehavior(getTemplate()->getRoamBehavior());
        m_floatingBehavior = static_cast<Ray_AIWaterBaseBehavior*>(createAiBehavior(getTemplate()->getFloatingBehavior()));
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated
        startRoam();
        m_physComponent->setUpdateRotation(bfalse);
    }

    void Ray_LivingstoneHatAIComponent::Update( f32 _dt )
    {
        if ( m_currentBehavior == m_roamBehavior )
        {
            updateRoam(_dt);
        }
        else if ( m_currentBehavior == m_floatingBehavior )
        {
            updateSwim(_dt);
        }
        else
        {
            ITF_ASSERT_MSG(0, "Unhandled behavior!");
        }

        Super::Update(_dt);
    }


    void Ray_LivingstoneHatAIComponent::updateRoam( f32 _dt )
    {
        PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());

        if ( waterPoly )
        {
            startFloating(waterPoly->getRef());
            return;
        }
    }

    void Ray_LivingstoneHatAIComponent::updateSwim( f32 _dt )
    {
        if ( m_floatingBehavior->getDistanceFromWater() > 0.2f )
        {
            startRoam();
        }
    }
    void Ray_LivingstoneHatAIComponent::startFloating( ObjectRef _ref )
    {
        if ( m_floatingBehavior )
        {
            m_floatingBehavior->setSwimingPolyline(_ref);
            setBehavior(m_floatingBehavior);
        }
    }
    void Ray_LivingstoneHatAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();
        if (m_currentBehavior == m_floatingBehavior)
        {
        }
        else if (m_currentBehavior == m_roamBehavior)
        {

        }
        ITF_ASSERT_MSG(m_currentBehavior,"Bhavior should never be null");
    }


    void Ray_LivingstoneHatAIComponent::startRoam(bbool _restart)
    {
        setBehavior(m_roamBehavior, _restart);
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_LivingstoneHatAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_LivingstoneHatAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("roamBehavior", m_roamBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("floatingBehavior", m_floatingBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    END_SERIALIZATION()

    Ray_LivingstoneHatAIComponent_Template::Ray_LivingstoneHatAIComponent_Template()
    : m_roamBehavior(NULL)
    , m_floatingBehavior(NULL)
    {
    }

    Ray_LivingstoneHatAIComponent_Template::~Ray_LivingstoneHatAIComponent_Template()
    {
        SF_DEL(m_roamBehavior);
        SF_DEL(m_floatingBehavior);
    }


} // namespace ITF

