#include "precompiled_gameplay.h"

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(PlayerControllerComponent)

BEGIN_SERIALIZATION_CHILD(PlayerControllerComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(PlayerControllerComponent)
    VALIDATE_COMPONENT_PARAM( "StickToPolylinePhysComponent",   m_actor->GetComponent<StickToPolylinePhysComponent>() != NULL,  "Player needs StickToPolylinePhysComponent !" );
    VALIDATE_COMPONENT_PARAM( "AnimatedComponent",              m_actor->GetComponent<AnimatedComponent>() != NULL,             "Player needs AnimatedComponent !" );
    VALIDATE_COMPONENT_PARAM( "FXControllerComponent",          m_actor->GetComponent<FXControllerComponent>() != NULL,         "Player needs FXControllerComponent !" );
    VALIDATE_COMPONENT_PARAM( "SoundComponent",                 m_actor->GetComponent<SoundComponent>() != NULL,                "Player needs SoundComponent !" );
    VALIDATE_COMPONENT_PARAM( "FxBankComponent",                m_actor->GetComponent<FxBankComponent>() != NULL,               "Player needs FxBankComponent !" );
END_VALIDATE_COMPONENT()

PlayerControllerComponent::PlayerControllerComponent()
    : Super()
    , m_animComponent(NULL)
    , m_characterPhys(NULL)
    , m_fxController(NULL)
    , m_soundComponent(NULL)
    , m_fxBankComponent(NULL)
    , m_phantom(NULL)
    , m_currentPhantomShape(NULL)
    , m_currentState(NULL)
    , m_prevState(NULL)
    , m_nextState(NULL)
    , m_playerIndex(U32_INVALID)
    , m_objectsRegistered(bfalse)
    , m_curentFaction(1)
    , m_phantomEnabled(btrue)
{
}

PlayerControllerComponent::~PlayerControllerComponent()
{
}

void PlayerControllerComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if ( m_phantom )
    {
        if ( m_objectsRegistered && m_phantomEnabled )
        {
            PHYSWORLD->removePhantom(m_phantom);
        }

        PHYSWORLD->deallocPhantom(m_phantom);
    }

    if ( m_objectsRegistered )
    {
        AI_MANAGER->removeActor(m_actor->getRef(),m_actor->getDepth());
    }

    SF_DEL(m_currentPhantomShape);
}

void PlayerControllerComponent::setCurrentPhantomShape( const PhysShape* _shape )
{
    if ( !m_currentPhantomShape || m_currentPhantomShape->GetObjectClassCRC() != _shape->GetObjectClassCRC() )
    {
        SF_DEL(m_currentPhantomShape);
        m_currentPhantomShape = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(_shape->GetObjectClassCRC());
    }

    PhysShape::CopyShapeScaled(_shape,m_currentPhantomShape,m_actor->getScale());

    if ( m_phantom )
    {
        m_phantom->setShape(m_currentPhantomShape);
    }
}

void PlayerControllerComponent::createPhantom()
{
    ITF_ASSERT_MSG(!m_phantom, "Phantom already created");
    ITF_ASSERT_MSG(m_currentPhantomShape, "you have to define a phantomShape!");

    PhysPhantomInitCommonInfo initInfo;
    SET_OWNER(initInfo, m_actor->getRef());
    initInfo.m_shape = m_currentPhantomShape;
    initInfo.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;
    initInfo.m_pos = m_actor->get2DPos();
    initInfo.m_angle = m_actor->getAngle();
    initInfo.m_userData = m_actor->getRef();
    m_phantom = PHYSWORLD->allocPhantom(initInfo);
}

void PlayerControllerComponent::onBecomeActive()
{
    Super::onBecomeActive();

    if (m_currentState)
    {
        m_currentState->onBecomeActive();
    }

    registerDatas();
}

void PlayerControllerComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    if (m_currentState)
    {
        m_currentState->onBecomeInactive();
    }

    unregisterDatas();
}

void PlayerControllerComponent::updatePhantom()
{
    if ( PhysShapePolygon* polygon = getCurrentPhantomShape()->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
    {
        bbool flipped = m_actor->isFlipped();

        polygon->setFlipped(flipped);
    }

    m_phantom->setPos3D(m_actor->getPos());
    m_phantom->setAngle(m_actor->getAngle());
    m_phantom->updateAABB();
}

void PlayerControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_characterPhys = m_actor->GetComponent<StickToPolylinePhysComponent>();
    ITF_ASSERT(m_characterPhys);
    m_animComponent = m_actor->GetComponent<AnimatedComponent>();
    ITF_ASSERT(m_animComponent);
    m_fxController = m_actor->GetComponent<FXControllerComponent>();
    ITF_ASSERT(m_fxController);
    m_soundComponent= m_actor->GetComponent<SoundComponent>();
    ITF_ASSERT(m_soundComponent);
    m_fxBankComponent= m_actor->GetComponent<FxBankComponent>();
    ITF_ASSERT(m_fxBankComponent);

    // Faction
    m_curentFaction = getTemplate()->getFaction();

    setCurrentPhantomShape(getTemplate()->getPhantomShape());
    createPhantom();

    if ( m_animComponent != NULL )
    {
        m_animComponent->setUpdateAnimInput(this);
    }
    if ( m_soundComponent != NULL )
    {
        m_soundComponent->setUpdateSoundInput(this);
    }
    if ( m_fxBankComponent != NULL )
    {
        m_fxBankComponent->setUpdateFxInput(this);
    }

    for (u32 i = 0; i < m_stateList.size(); i++)
    {
        m_stateList[i]->init(this, m_animComponent, m_characterPhys, m_fxController);
        m_stateList[i]->onActorLoaded(_hotReload);
    }

    Player* myPlayer = GAMEMANAGER->getPlayerFromActor(m_actor->getRef(),bfalse);

    if ( myPlayer )
    {
        m_playerIndex = myPlayer->getIndex();
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCollide,3122529699),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryPhysShape,2080285933),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPadRumbleStart,2869984648),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPadRumbleStop,2252742105),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTeleport,1075308886),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152),this);

    m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
}

void PlayerControllerComponent::onCheckpointLoaded()
{
    if ( m_phantom )
    {
        updatePhantom();
    }
}

void PlayerControllerComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    updatePhantom();
}

void PlayerControllerComponent::updateCurrentState( f32 _deltaTime )
{
    u32 countSafe = 0;
    while ( m_currentState->checkStateChange(_deltaTime) )
    {
        ;
        //TODO :David it s a workaround,to prevent infinite loop until a properly fix is done
        countSafe++;

        if (countSafe>=1000)
        {
           static bool alreadyDisplay = bfalse;
           if(!alreadyDisplay && SYSTEM_ADAPTER->messageBox("Prevent Crash" ,"PlayerControllerComponent::updateCurrentState infinite", ITF_MSG_ICONERROR | ITF_MSG_OK,ITF_IDYES) == ITF_IDYES)
           {
               alreadyDisplay = btrue;
               break;
           }
        }
    }

    m_currentState->update(_deltaTime);
}

void PlayerControllerComponent::changeState( PlayerControllerState* _newState)
{
    m_prevState = m_currentState;
    m_nextState = _newState;

    if (m_currentState)
    {
        m_currentState->onExit();
    }

    m_currentState = _newState;
    _newState->onEnter();

    m_prevState = NULL;
    m_nextState = NULL;
}

void PlayerControllerComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    internalOnEvent(_event);
}

bbool PlayerControllerComponent::internalOnEvent( Event* _event )
{
    if ( EventCollide* collide = _event->DynamicCast<EventCollide>(ITF_GET_STRINGID_CRC(EventCollide,3122529699)) )
    {
        processCollision(collide);
    }
    else if ( EventQueryPhysShape *queryShape = _event->DynamicCast<EventQueryPhysShape>(ITF_GET_STRINGID_CRC(EventQueryPhysShape,2080285933)) )
    {
        if (m_phantom)
        {
            queryShape->setPhysShape(m_phantom->getShape());
            queryShape->setPosition(m_phantom->getPos());
            queryShape->setAngle(m_phantom->getAngle());
        }
        else
        {
            queryShape->setPhysShape(NULL);
        }
    }
    else if (EventPadRumbleStart* padRumbleStart = _event->DynamicCast<EventPadRumbleStart>(ITF_GET_STRINGID_CRC(EventPadRumbleStart,2869984648)))
    {
        processPadRumbleStart(padRumbleStart);
    }
    else if (EventPadRumbleStop* padRumbleStop = _event->DynamicCast<EventPadRumbleStop>(ITF_GET_STRINGID_CRC(EventPadRumbleStop,2252742105)))
    {
        processPadRumbleStop(padRumbleStop);
    }
    else if ( EventTeleport* teleport = _event->DynamicCast<EventTeleport>(ITF_GET_STRINGID_CRC(EventTeleport,1075308886)) )
    {
        processTeleport(teleport);
    }
    else if ( EventPlayerIndexChange* playerIndex = _event->DynamicCast<EventPlayerIndexChange>(ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152)) )
    {
        processPlayerIndexChange(playerIndex);
    }
    else
    {
        return bfalse;
    }

    return btrue;
}

void PlayerControllerComponent::processTeleport( EventTeleport* _eventTeleport )
{
    updatePhantom();
}

void PlayerControllerComponent::processPlayerIndexChange( EventPlayerIndexChange* _playerIndex )
{
    m_playerIndex = _playerIndex->getNewIndex();
}

void PlayerControllerComponent::processCollision(EventCollide* _collide)
{
    const PhysShape* actorShape = m_phantom->getShape();
    const PhysShape* colliderShape = _collide->getShape();

    AABB actorShapeAABB;
    actorShape->calculateAABB(m_actor->get2DPos(), m_actor->get2DPos(), m_actor->getAngle(), actorShapeAABB);
    AABB colliderShapeAABB;
    colliderShape->calculateAABB(_collide->getPos(), _collide->getPos(), _collide->getAngle(), colliderShapeAABB);

    //const Vec2d& actorPos = m_actor->get2DPos();
    //Vec2d colliderPos = _collide->getPos().truncateTo2D();
    //GFX_ADAPTER->drawDBGAABB(actorShapeAABB, Color::green());
    //AIUtils::drawDBGPhysShape(actorPos, m_actor->getAngle(), actorShape, Color::blue());
    //GFX_ADAPTER->drawDBGAABB(colliderShapeAABB, Color::orange());
    //AIUtils::drawDBGPhysShape(colliderPos, _collide->getAngle(), colliderShape, Color::red());

    if (actorShapeAABB.checkOverlap(colliderShapeAABB))
    {
        PhysContactsContainer contacts;
        PhysSweepInfo sweepA, sweepB;

        PhysCollisionSolver::calculateSweepInfo(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),actorShape,sweepA);
        PhysCollisionSolver::calculateSweepInfo(_collide->getPos(),_collide->getPos(),_collide->getAngle(),colliderShape,sweepB);

        PhysCollisionSolver::collide(sweepA,sweepB,contacts);

        if ( contacts.size() > 0 )
        {
            _collide->setCollide(btrue);
        }
    }
}

void PlayerControllerComponent::processPadRumbleStart(EventPadRumbleStart* _padRumbleStart)
{
#ifndef USE_PAD_HAPTICS
    PADRUMBLEMANAGER->startRumble(_padRumbleStart->getName(), m_playerIndex);
#endif
}

void PlayerControllerComponent::processPadRumbleStop(EventPadRumbleStop* _padRumbleStop)
{
#ifndef USE_PAD_HAPTICS
    PADRUMBLEMANAGER->stopRumble(m_playerIndex);
#endif
}

void PlayerControllerComponent::registerDatas()
{
    if ( !m_objectsRegistered )
    {
        if ( m_phantom && m_phantomEnabled )
        {
            // update phantom before to be sure having the good phantom data this frame
            m_phantom->setPos3D( m_actor->getPos() );
            m_phantom->synchronizePos();
            m_phantom->updateAABB();
            PHYSWORLD->insertPhantom(m_phantom, m_actor->getDepth());
        }

        AIData * data = AI_MANAGER->addActor(m_actor->getRef(), m_actor->getDepth());
        if (data)
            data->m_faction = getFaction();

        m_objectsRegistered = btrue;
    }
}

void PlayerControllerComponent::unregisterDatas()
{
    if ( m_objectsRegistered )
    {
        if ( m_phantom && m_phantomEnabled )
        {
            PHYSWORLD->removePhantom(m_phantom);
        }

        AI_MANAGER->removeActor(m_actor->getRef(), m_actor->getDepth());
        m_objectsRegistered = bfalse;
    }
}

void PlayerControllerComponent::enablePhantom( bbool _val )
{
    if ( m_phantomEnabled != _val )
    {
        m_phantomEnabled = _val;

        if ( m_objectsRegistered && m_phantom )
        {
            if ( m_phantomEnabled )
            {
                PHYSWORLD->insertPhantom(m_phantom, m_actor->getDepth());
            }
            else
            {
                PHYSWORLD->removePhantom(m_phantom);
            }
        }
    }
}

void PlayerControllerComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
{
    Super::onDepthChanged( _oldZ, _newZ);

    if (m_objectsRegistered)
    {
        AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ, _newZ);
    }
}

void PlayerControllerComponent::setFaction( Faction _newFaction )
{
    m_curentFaction = _newFaction;

    if (m_objectsRegistered)
    {
        AIData * data = AI_MANAGER->getAIData(m_actor->getRef(),m_actor->getDepth());
        if (data)
            data->m_faction = m_curentFaction;
    }
}

#ifdef ITF_SUPPORT_EDITOR
void PlayerControllerComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    DebugDraw::shape(m_phantom->getPos(), m_actor->getPos().m_z, m_phantom->getAngle(), m_phantom->getShape(),
        Color::white(), 0.f, "phantom", 65 * MTH_DEGTORAD);
}

void PlayerControllerComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    unregisterDatas();
    registerDatas();
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PlayerControllerComponent_Template)
BEGIN_SERIALIZATION(PlayerControllerComponent_Template)

    SERIALIZE_OBJECT_WITH_FACTORY("phantomShape", m_phantomShape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("faction", m_faction);
    SERIALIZE_MEMBER("deadSoulFaction", m_deadSoulFaction);


END_SERIALIZATION()

PlayerControllerComponent_Template::PlayerControllerComponent_Template()
: m_phantomShape(NULL)
, m_faction(1)
, m_deadSoulFaction(1)
{
}

} // namespace ITF

