#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DARKTOONAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_DarkToonAIComponent.h"
#endif //_ITF_RAY_DARKTOONAICOMPONENT_H_

#ifndef _ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneWindBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERBASEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterBaseBehavior.h"
#endif //_ITF_RAY_AIWATERBASEBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Darktoon/Ray_AIDarktoonLaunchBehavior.h"
#endif //_ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{

        IMPLEMENT_OBJECT_RTTI(Ray_DarkToonAIComponent)
        BEGIN_SERIALIZATION_CHILD(Ray_DarkToonAIComponent)
        END_SERIALIZATION()


        Ray_DarkToonAIComponent::Ray_DarkToonAIComponent()
        : m_windBehavior(NULL)
        , m_waterBehavior(NULL)
        , m_launchBehavior(NULL)
    {
    }


    Ray_DarkToonAIComponent::~Ray_DarkToonAIComponent()
    {
    }


    void Ray_DarkToonAIComponent::Update( f32 _deltaTime )
    {
        if (m_currentBehavior != m_waterBehavior && m_currentBehavior != m_deathBehavior)
        {
            PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());

            if ( waterPoly )
            {
                startDrowning(waterPoly->getRef());
                return;
            }
        }
        
        // start attacking if possible if landed after a launch
        if (m_launchBehavior &&
            m_currentBehavior == m_launchBehavior &&
            m_launchBehavior->isLanding())
        {
            u32 enemyCount;
            Actor* closestEnemy = getClosestEnemy(getTemplate()->getEnemyDetectionRange(), &enemyCount);
            if (closestEnemy && m_attackBehavior) 
            {
                // disable spotting & attack
                m_hadEnemyInSight = btrue;
                startAttack(closestEnemy->getRef());
            }
        }
        
        Super::Update(_deltaTime);

    }
    void Ray_DarkToonAIComponent::startDrowning( ObjectRef _ref )
    {
        if ( m_waterBehavior )
        {
            m_waterBehavior->setSwimingPolyline(_ref);
            setBehavior(m_waterBehavior);
        }
        else
        {
            startDeath();
        }

    }

    void Ray_DarkToonAIComponent::receiveWind( WindStim* _wind )
    {
        if ( m_currentBehavior != m_windBehavior)
        {

            Vec2d speed = m_physComponent->getSpeed();


            if (_wind->getDir().dot(AIUtils::getLookDir(m_actor,m_actor->isFlipped())) < 0.0f)
            {
                m_lastAttacker = _wind->getSender();
                m_windBehavior->setData(_wind);
                setBehavior(m_windBehavior);
            }
        }
    }

    void Ray_DarkToonAIComponent::processLaunchQuery(Ray_EventQueryChildLaunch* _query)
    {
        if (m_launchBehavior)
        {
            // notify that launch behavior exists
            _query->touch();
        }
    }
    
    void Ray_DarkToonAIComponent::processLaunch(Ray_EventChildLaunch* _launch)
    {
        if (m_launchBehavior)
        {
            // setup the launch behavior
            m_launchBehavior->setOrigin(_launch->getLaunchPos());
            m_launchBehavior->setTarget(m_actor->getPos());
            
            // start it
            setBehavior(m_launchBehavior);
        }
    }

    bbool Ray_DarkToonAIComponent::isSquashAuthorized() const
    {
        // prevent unwanted squashing in launch behavior (RO-3299)
        return !isCurrentBehavior(m_launchBehavior);
    }

    void Ray_DarkToonAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_waterBehavior = createAiBehaviorAndCast<Ray_AIWaterBaseBehavior>(getTemplate()->getWaterBehavior());
        m_windBehavior = createAiBehaviorAndCast<Ray_AILivingStoneWindBehavior>(getTemplate()->getWindBehavior());
        m_launchBehavior = createAiBehaviorAndCast<Ray_AIDarktoonLaunchBehavior>(getTemplate()->getLaunchBehavior());

        Super::onActorLoaded(_hotReload);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(WindStim,2026477007),this);

        // listen to launch events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryChildLaunch,3130623924), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventChildLaunch,2714500499), this);
    }


    void Ray_DarkToonAIComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if ( WindStim* wind = _event->DynamicCast<WindStim>(ITF_GET_STRINGID_CRC(WindStim,2026477007)) )
        {
            receiveWind(wind);
        }
        else if (Ray_EventQueryChildLaunch* queryLaunch = _event->DynamicCast<Ray_EventQueryChildLaunch>(ITF_GET_STRINGID_CRC(Ray_EventQueryChildLaunch,3130623924)))
        {
            processLaunchQuery(queryLaunch);
        }
        else if (Ray_EventChildLaunch* launch = _event->DynamicCast<Ray_EventChildLaunch>(ITF_GET_STRINGID_CRC(Ray_EventChildLaunch,2714500499)))
        {
            processLaunch(launch);
        }
    }

    void Ray_DarkToonAIComponent::onBehaviorFinished()
    {
        if (m_currentBehavior == m_windBehavior)
        {
            Actor* attackerActor = AIUtils::getActor(m_lastAttacker);

            if ( attackerActor )
            {
                if ( m_attackBehavior && AIUtils::isTarget(getFaction(), AIUtils::getFaction(attackerActor)) )
                {
                    // revenge!
                    startAttack(m_lastAttacker);
                }
                else
                {
                    // friendly fire
                    startRoam();
                }
            }
        }
        else if (m_launchBehavior && m_currentBehavior == m_launchBehavior)
        {
            // sleep
            startSleep();
        }
        else
            Super::onBehaviorFinished();

    }

    //----------------------------------------------------------------------------------------------------------------------------------------


    IMPLEMENT_OBJECT_RTTI(Ray_DarkToonAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_DarkToonAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("windBehavior", m_windBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("waterBehavior", m_waterBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("launchBehavior", m_launchBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    END_SERIALIZATION()


    Ray_DarkToonAIComponent_Template::Ray_DarkToonAIComponent_Template()
    : m_windBehavior(NULL)
    , m_waterBehavior(NULL)
    , m_launchBehavior(NULL)
    {
    }

    Ray_DarkToonAIComponent_Template::~Ray_DarkToonAIComponent_Template()
    {
        SF_DEL(m_windBehavior);
        SF_DEL(m_waterBehavior);
        SF_DEL(m_launchBehavior);
    }
}

