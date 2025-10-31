#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCOSTUMECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeManagerComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

namespace ITF
{
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PlayerCostumeComponent)

BEGIN_SERIALIZATION_CHILD(Ray_PlayerCostumeComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_PlayerCostumeComponent)
    VALIDATE_COMPONENT_PARAM("",getTemplate()->getPlayerIDInfo().isValid(), "PlayerIDInfo not set");
    VALIDATE_COMPONENT_PARAM("shape",getTemplate()->getShape(), "Shape not set");
    VALIDATE_COMPONENT_PARAM("AnimComponent",m_animComponent!=NULL, "Needs an anim component");
END_VALIDATE_COMPONENT()

Ray_PlayerCostumeComponent::Ray_PlayerCostumeComponent()
: m_state(STATE_LOCKED)
, m_animComponent(NULL)
, m_scaledShape(NULL)
, m_newIconRef(ITF_INVALID_OBJREF)
{
}

Ray_PlayerCostumeComponent::~Ray_PlayerCostumeComponent()
{
    SF_DEL(m_scaledShape);
}

void Ray_PlayerCostumeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    //spawnPlayerActors();

    if ( !getTemplate()->getFX().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_fxGenerator, getTemplate()->getFX());
    }

    if (getTemplate()->getShape())
    {
        scaleShape();
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374), this);

    // Register costume to the manager
    Ray_PlayerCostumeManagerComponent* manager = getManager();
    ITF_WARNING_CATEGORY(GPP,m_actor,manager,"Costume manager not ready so the costume will not be registered");

    if ( manager )
    {
        manager->registerCostume(m_actor->getRef());
    }

    if ( !getTemplate()->getNewIconPath().isEmpty() )
    {
        Actor* newIcon = ACTORSMANAGER->spawnActor(m_actor->getPos(),m_actor->getScene(),getTemplate()->getNewIconPath(),btrue);

        if ( newIcon )
        {
            newIcon->setIsSpawned(btrue);
            newIcon->disable();
            m_newIconRef = newIcon->getRef();
        }
    }
}

void Ray_PlayerCostumeComponent::onActorClearComponents()
{
	if ( m_actor )
	{
		u32 numActors = m_playerActors.size();
		Scene* scene = m_actor->getScene();

		if ( scene )
		{
			for ( u32 i = 0; i < numActors; i++ )
			{
				Actor* actor = m_playerActors[i].m_actor.getActor();

				if ( actor )
				{
					scene->unregisterPickable(actor);
				}
			}
		}

        Ray_PlayerCostumeManagerComponent* manager = getManager();

        if ( manager )
        {
            manager->unregisterCostume(m_actor->getRef());
        }
	}

    Actor* newIcon = m_newIconRef.getActor();

    if ( newIcon )
    {
        newIcon->requestDestruction();
    }
}

class Ray_PlayerCostumeManagerComponent* Ray_PlayerCostumeComponent::getManager() const
{
    Actor* actor = AIUtils::getActor(RAY_GAMEMANAGER->getCostumeManager());

    if ( actor )
    {
        Ray_PlayerCostumeManagerComponent* manager = actor->GetComponent<Ray_PlayerCostumeManagerComponent>();

        return manager;
    }

    return NULL;
}

void Ray_PlayerCostumeComponent::onBecomeActive()
{
    Super::onBecomeActive();
}

void Ray_PlayerCostumeComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    //Ray_PlayerCostumeManagerComponent* manager = getManager();

    //if ( manager )
    //{
    //    manager->unregisterCostume(m_actor->getRef());
    //}
}

Scene* Ray_PlayerCostumeComponent::getPlayerScene() const
{
    World* playerWorld = (World*)GETOBJECT(GAMEMANAGER->getPlayerWorld());

    if ( playerWorld )
    {
        return playerWorld->getRootScene();
    }

    return NULL;
}

void Ray_PlayerCostumeComponent::spawnPlayerActors()
{
    ITF_ASSERT(m_playerActors.size()==0);

    if ( getTemplate()->getPlayerIDInfo().isValid() )
    {
        Scene* scene = m_actor->getScene();

        if ( scene )
        {
            const PlayerIDInfo* pInfo = GAMEMANAGER->getPlayerIDInfo(getTemplate()->getPlayerIDInfo());

            if ( pInfo )
            {
                const PlayerIDInfo::GameScreenInfo* gsInfo = pInfo->getGameScreenInfo(GAMEMANAGER->getCurrentGameScreen());

                if ( gsInfo )
                {
                    u32 numActors = gsInfo->getNumActors();

                    for ( u32 i = 0; i < numActors; i++ )
                    {
                        const PlayerIDInfo::ActorInfo& actorInfo = gsInfo->getActorInfo(i);

                        ActorsManager::SpawnInfo spawnInfo;

                        spawnInfo.m_pScene = scene;
                        spawnInfo.m_luaPath = actorInfo.getFile();
                        spawnInfo.m_init = btrue;
                        spawnInfo.m_pos = m_actor->getPos();

                        Actor* newActor = ACTORSMANAGER->spawnActor(spawnInfo);

                        if ( newActor )
                        {
                            newActor->disable();

                            Player::RegisteredActor regActor;

                            regActor.m_actor = newActor->getRef();
                            regActor.m_gameModes = actorInfo.getGameModes();
                            regActor.m_alwaysActive = actorInfo.isAlwaysActive();
                            regActor.m_isPlayable = actorInfo.isPlayable();

                            m_playerActors.push_back(regActor);
                        }
                    }
                }
            }
        }
    }
}

void Ray_PlayerCostumeComponent::scaleShape()
{
    if ( !m_scaledShape ||
          m_scaledShape->GetObjectClassCRC() != getTemplate()->getShape()->GetObjectClassCRC() )
    {
        SF_DEL(m_scaledShape);
        m_scaledShape = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getShape()->GetObjectClassCRC());
    }

    PhysShape::CopyShapeScaled(getTemplate()->getShape(), m_scaledShape, m_actor->getScale());
}

void Ray_PlayerCostumeComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    CostumeState newState = calculateState();

    setState(newState);
    updateNewIcon();
    
    if ( m_state == STATE_AVAILABLE &&
        !RAY_GAMEMANAGER->isSequencePlaying() )
    {
        triggerActors();
    }
}

void Ray_PlayerCostumeComponent::triggerActors()
{
    AABB myAABB;
    Vec2d shapePos = getShapePos();

    m_scaledShape->calculateAABB(shapePos, shapePos, m_actor->getAngle(), myAABB);

    const DepthRange depthRange(m_actor->getDepth());
    const i32 playerCount = GAMEMANAGER->getMaxPlayerCount();
    FixedArray <ActorRef,32> actorsInside;

    for (i32 i = 0; i < playerCount; ++i)
    {
        Player* player = GAMEMANAGER->getPlayer(i);

        if ( !player->getActive() || player->isDead() || player->getCurrentPlayerIDInfo() == getTemplate()->getPlayerIDInfo() )
        {
            continue;
        }

        Actor* playerActor = player->getActor();
        if ( !playerActor || depthRange != DepthRange(playerActor->getDepth()) )
        {
            continue;
        }

        EventQueryPhysShape queryShape;

        playerActor->onEvent(&queryShape);

        if ( !queryShape.getPhysShape() )
        {
            continue;
        }

        AABB playerAABB;

        queryShape.getPhysShape()->calculateAABB(playerActor->get2DPos(), playerActor->get2DPos(), playerActor->getAngle(), playerAABB);

        if ( !myAABB.checkOverlap(playerAABB) )
        {
            continue;
        }

        PhysContactsContainer contacts;
        PhysSweepInfo sweepA, sweepB;

        PhysCollisionSolver::calculateSweepInfo(playerActor->get2DPos(), playerActor->get2DPos(), playerActor->getAngle(), queryShape.getPhysShape(), sweepA);
        PhysCollisionSolver::calculateSweepInfo(shapePos, shapePos, m_actor->getAngle(), m_scaledShape, sweepB);
        PhysCollisionSolver::collide(sweepA, sweepB, contacts);

        if ( contacts.size() == 0 )
        {
            continue;
        }

        actorsInside.push_back(playerActor->getRef());

        if ( m_actorsInside.find(playerActor->getRef()) != -1 )
        {
            continue;
        }

        applyCostume(player,playerActor);
    }

    m_actorsInside.clear();

    u32 numActorsInside = actorsInside.size();

    for ( u32 i = 0; i < numActorsInside; i++ )
    {
        m_actorsInside.push_back(actorsInside[i]);
    }
}

void Ray_PlayerCostumeComponent::updateNewIcon()
{
    Actor* newIcon = m_newIconRef.getActor();

    if ( newIcon )
    {
        if ( RAY_GAMEMANAGER->hasCostumeBeenUsed(getTemplate()->getPlayerIDInfo()) )
        {
            newIcon->disable();
        }
        else if ( m_state == STATE_AVAILABLE )
        {
            Vec2d pos2D = getTemplate()->getNewIconOffset().truncateTo2D().Rotate(m_actor->getAngle()) + m_actor->get2DPos();

            newIcon->setPos(pos2D.to3d(m_actor->getPos().m_z+getTemplate()->getNewIconOffset().m_z));
            newIcon->setAngle(m_actor->getAngle());

            newIcon->enable();
        }
    }
}

Ray_PlayerCostumeComponent::CostumeState Ray_PlayerCostumeComponent::calculateState()
{
#ifdef ITF_SUPPORT_CHEAT
    if ( RAY_CHEATMANAGER->getIsCheatAllCostumesUnlocked() )
    {
        return STATE_AVAILABLE;
    }
#endif

    Ray_PlayerCostumeManagerComponent* manager = getManager();

    if ( manager )
    {
        if ( manager->isCostumeAvailable(getTemplate()->getPlayerIDInfo()) )
        {
            if ( RAY_GAMEMANAGER->isPlayerIDInfoUsed(getTemplate()->getPlayerIDInfo()) &&
                !RAY_GAMEMANAGER->isAllGameLevelsCompleted() )
            {
                return STATE_USED;
            }
            else
            {
                return STATE_AVAILABLE;
            }
        }
        else if ( manager->isCostumeShown(getTemplate()->getPlayerIDInfo()) )
        {
            return STATE_SHOWN;
        }
    }

    return STATE_LOCKED;
}

Vec2d Ray_PlayerCostumeComponent::getShapePos() const
{
    Vec2d ret = getTemplate()->getShapeOffset() * m_actor->getScale().m_x;

    if ( m_actor->isFlipped() )
    {
        ret *= -1.f;
    }

    ret = ret.Rotate(m_actor->getAngle());

    return m_actor->get2DPos() + ret;
}

void Ray_PlayerCostumeComponent::setState( CostumeState _state )
{
    if ( m_state != _state )
    {
        static u32 s_blendingFrames = 12;

        m_state = _state;

        switch(m_state)
        {
            case STATE_LOCKED:
                if ( getTemplate()->getLockedAnim().isValid() )
                {
                    m_animComponent->setAnim(getTemplate()->getLockedAnim(),s_blendingFrames);
                }
                break;
            case STATE_SHOWN:
                if ( getTemplate()->getShownAnim().isValid() )
                {
                    m_animComponent->setAnim(getTemplate()->getShownAnim(),s_blendingFrames);
                }
                break;
            case STATE_AVAILABLE:
                if ( getTemplate()->getAvailableAnim().isValid() )
                {
                    m_animComponent->setAnim(getTemplate()->getAvailableAnim(),s_blendingFrames);
                }
                break;
            case STATE_USED:
                if ( getTemplate()->getUsedAnim().isValid() )
                {
                    m_animComponent->setAnim(getTemplate()->getUsedAnim(),s_blendingFrames);
                }
                break;
        }
    }
}

void Ray_PlayerCostumeComponent::applyCostume( Player* _player, Actor* _actor )
{
    EventTeleport myPos(_actor->getPos(),_actor->getAngle(),btrue);

    Player* playerFound = NULL;
    u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();

    Ray_PlayerHudScoreComponent* myHud = Ray_PlayerHudScoreComponent::getInstance(_player->getIndex());
    u32 myPrevVisualOrder = myHud->getVisualOrder();

    bbool applyCameraSwapSubject = btrue;

    for ( u32 i = 0; i < numPlayers; i++ )
    {
        Player* p = RAY_GAMEMANAGER->getPlayer(i);

        if ( p && p != _player && p->getCurrentPlayerIDInfo() == getTemplate()->getPlayerIDInfo() && !p->getActive() )
        {
            playerFound = p;
            break;
        }
    }

    if ( playerFound )
    {
        Ray_PlayerHudScoreComponent* otherHud = Ray_PlayerHudScoreComponent::getInstance(playerFound->getIndex());
        u32 otherPrevVisualOrder = otherHud->getVisualOrder();

        // camera swap subjects
        if ( const Actor* actorFound = playerFound->getActor() )
        {
            applyCameraSwapSubject = bfalse;
            CAMERACONTROLLERMANAGER->swapSubjects( _actor->getRef(), actorFound->getRef() );
        }

        // remember HP
        Ray_Player* rayPlayer = static_cast<Ray_Player*>(_player);
        u32 hp = rayPlayer->getHitPoints();
        
        RAY_GAMEMANAGER->swapPlayer(_player->getIndex(),playerFound->getIndex());

        // copy HP so heart shield don't get lost (RO-13911)
        rayPlayer->setHitPoints(hp);

        otherHud = Ray_PlayerHudScoreComponent::getInstance(playerFound->getIndex());
        otherHud->setVisualOrder(otherPrevVisualOrder);
    }
    else
    {
        spawnPlayerActors();

        EventPlayerIndexChange myPlayerIndex(_player->getIndex());

        Scene* playerScene = getPlayerScene();
        Scene* currentScene = m_actor->getScene();

        const Player::RegisteredActorList& registeredActors = _player->getRegisteredActors();
        u32 numActors = registeredActors.size();

        for ( u32 i = 0; i < numActors; i++ )
        {
            Actor* actor = registeredActors[i].m_actor.getActor();

            if ( actor )
            {
                actor->requestDestruction();
            }
        }

        numActors = m_playerActors.size();

        for ( u32 i = 0; i < numActors; i++ )
        {
            Actor* actor = m_playerActors[i].m_actor.getActor();

            if ( actor )
            {
                if ( playerScene != currentScene )
                {
                    if ( currentScene )
                    {
                        currentScene->transferPickable(actor,*playerScene);
                        actor->loadResources();
                    }
                }
            }
        }

        _player->setRegisteredActors(m_playerActors,btrue);

        numActors = m_playerActors.size();

        for ( u32 i = 0; i < numActors; i++ )
        {
            Actor* actor = m_playerActors[i].m_actor.getActor();

            if ( actor )
            {
                actor->onEvent(&myPlayerIndex);
            }
        }

        m_playerActors.clear();
        //spawnPlayerActors();

        RAY_GAMEMANAGER->changePlayerIdInfoForPlayer(_player,RAY_GAMEMANAGER->getPlayerIDInfo(getTemplate()->getPlayerIDInfo()));
    }

    Actor* currentActor = _player->getCurrentActor().getActor();

    // camera swap subjects
    if ( applyCameraSwapSubject )
    {
        CAMERACONTROLLERMANAGER->swapSubjects( _actor->getRef(), currentActor->getRef());
    }

    currentActor->onEvent(&myPos);

    if ( !getTemplate()->getFX().isEmpty() )
    {
        m_fxGenerator.getSpawnee(m_actor->getScene(),currentActor->getPos(),0.0f);
    }

    myHud = Ray_PlayerHudScoreComponent::getInstance(_player->getIndex());
    myHud->setVisualOrder(myPrevVisualOrder);
}

StringID Ray_PlayerCostumeComponent::getCostumeID() const
{
    return getTemplate()->getPlayerIDInfo();
}

void Ray_PlayerCostumeComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( EventScaleChanged* crushedEvent = _event->DynamicCast<EventScaleChanged>(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374)) )
    {
        scaleShape();
    }
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_PlayerCostumeComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
    {
        return;
    }

    if ( m_scaledShape )
    {
        DebugDraw::shape(getShapePos().to3d(m_actor->getPos().m_z), m_actor->getAngle(), m_scaledShape,
            Color::white(), 0.f, "ainetwork node", 85 * MTH_DEGTORAD);
    }
}
#endif // ITF_SUPPORT_EDITOR

//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PlayerCostumeComponent_Template)
BEGIN_SERIALIZATION(Ray_PlayerCostumeComponent_Template)
    SERIALIZE_MEMBER("newIconPath",m_newIconPath);
    SERIALIZE_MEMBER("newIconOffset",m_newIconOffset);
    SERIALIZE_MEMBER("playerIDInfo",m_playerIDInfo);
    SERIALIZE_MEMBER("animLocked",m_animLocked);
    SERIALIZE_MEMBER("animShown",m_animShown);
    SERIALIZE_MEMBER("animAvailable",m_animAvailable);
    SERIALIZE_MEMBER("animUsed",m_animUsed);
    SERIALIZE_MEMBER("fx",m_fx);
    SERIALIZE_OBJECT_WITH_FACTORY("shape", m_shape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("shapeOffset",m_shapeOffset);

END_SERIALIZATION()

Ray_PlayerCostumeComponent_Template::Ray_PlayerCostumeComponent_Template()
: m_newIconOffset(Vec3d::Zero)
, m_shape(NULL)
, m_shapeOffset(Vec2d::Zero)
{
}

Ray_PlayerCostumeComponent_Template::~Ray_PlayerCostumeComponent_Template()
{
    SF_DEL(m_shape);
}

}
