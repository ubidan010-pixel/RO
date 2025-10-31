#include "precompiled_gameplay_rayman.h"

#ifndef __RAY_AIJANODROAMINGBASEBEHAVIOR_H__
#include "rayman/gameplay/ai/Behaviors/Janod/Ray_AIJanodRoamingBaseBehavior.h"
#endif //__RAY_AIJANODROAMINGBASEBEHAVIOR_H__

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_RAY_JANODAICOMPONENT_H__
#include "rayman/gameplay/Components/AI/Ray_JanodAIComponent.h"
#endif //_ITF_RAY_JANODAICOMPONENT_H__

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_FIGHTMANAGER_H_
#include "rayman/gameplay/Managers/Ray_FightManager.h"
#endif //_ITF_RAY_FIGHTMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIJanodRoamingBaseBehavior_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_AIJanodRoamingBaseBehavior_Template)
        SERIALIZE_MEMBER("hitLevel",m_hitLevel);
        SERIALIZE_MEMBER("bristlingTime", m_bristlingTime);
        END_SERIALIZATION()

        IMPLEMENT_OBJECT_RTTI(Ray_AIJanodRoamingBaseBehavior)

    void Ray_AIJanodRoamingBaseBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
    }

    void Ray_AIJanodRoamingBaseBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
    }
    void Ray_AIJanodRoamingBaseBehavior::onActivate()
    {
    }
    void Ray_AIJanodRoamingBaseBehavior::onDeactivate()
    {
        Super::onDeactivate();
    }
    void Ray_AIJanodRoamingBaseBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBaseBehavior::sendStim(ObjectRef _victimRef)
    {
        Actor *victim = (Actor*)_victimRef.getObject();
        if (!victim) 
            return;

        PhantomComponent *phantom = m_actor->GetComponent<PhantomComponent>();
        if (!phantom)
            return;
        PhysShape *shape = phantom->getShape();
        if (!shape)
            return;

        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>(shape);

        stim->setDepth(m_actor->getDepth());
        stim->setSender(m_actor->getRef());
        stim->setLevel(getTemplate()->getHitLevel());

        Vec2d toVictim = victim->get2DPos()-m_actor->get2DPos();
        toVictim.normalize();
        f32 angle = atan2f(toVictim.m_y, toVictim.m_x);
        f32 deltaAngle = MTH_PI*0.1f;
        if (angle>(MTH_PIBY2-deltaAngle) && angle<(MTH_PIBY2+deltaAngle))
        {
            if (angle>MTH_PIBY2)
            {
                angle = MTH_PIBY2+deltaAngle;
            }
            else
            {
                angle = MTH_PIBY2-deltaAngle;
            }
        }

        stim->setAngle(angle);
        Vec2d pos2d = m_actor->get2DPos(); 
        stim->setPos(pos2d);
        stim->setPrevPos(pos2d);
        stim->setIsRadial(btrue);
        stim->setFaction(m_aiComponent->getFaction());
        STIMSMANAGER->sendStim(stim);
        //m_physComponent->addForce(-toVictim*getTemplate()->getStimFeedback());
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIJanodRoamingBaseBehavior::wasCrushedRecently()
    {
        Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
        ITF_ASSERT_CRASH(janod, "no janod component");

        return janod->getHowLongWasCrushed()<0.25f;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBaseBehavior::attackEnnemies()
    {
        for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player *player = (Ray_Player*)GAMEMANAGER->getPlayer(i);
            if (player && player->getActive() /*&& !player->isDead()*/)
            {
                if (!wasCrushedRecently())
                {
                    sendStim(player->getCurrentActor());
                }
                //RAY_FIGHTMANAGER->requestAttack(m_actor->getRef(), player->getActor()->getRef());
            }
        }
    }


}
