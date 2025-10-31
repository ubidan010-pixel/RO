#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#include "gameplay/Components/Misc/CameraSubjectComponent.h"
#endif //_ITF_CAMERASUBJECTCOMPONENT_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateDeath)

////////////////////
// Death
////////////////////
Ray_PlayerControllerComponent::StateDeath::~StateDeath()
{
    destroyFX();
}

void Ray_PlayerControllerComponent::StateDeath::destroyFX()
{
    u32 numFx = m_fxList.size();

    for ( u32 i = 0; i < numFx; i++ )
    {
        Actor* fx = m_fxList[i].getActor();

        if ( fx )
        {
            fx->requestDestruction();
        }
    }

    m_fxList.clear();
}

void Ray_PlayerControllerComponent::StateDeath::onEnter()
{
    Super::onEnter();

    m_triggerDeadSoul = bfalse;
    m_triggerSequence = bfalse;
    m_spawnFX = bfalse;
    m_gameContinues = -1;

    EventStopPlayer stopPlayer(btrue);
    m_actor->onEvent(&stopPlayer);

    m_parent->setStance(STANCE_STAND);

    CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());
    spawnOffscreenFX();

    if ( m_deactivateOnFinish && m_parent->m_playerIndex != U32_INVALID )
    {
        Ray_Player* player = static_cast<Ray_Player*>(RAY_GAMEMANAGER->getPlayer(m_parent->m_playerIndex));

        if ( player )
        {
            player->setIsLeaving(btrue);
        }
    }
}

void Ray_PlayerControllerComponent::StateDeath::onExit()
{
    Super::onExit();

    m_explodeDirectly = bfalse;
    m_deactivateOnFinish = bfalse;
    m_reviveOnFirstLivePlayer = bfalse;
    m_parent->resetDeath();
}

bbool Ray_PlayerControllerComponent::StateDeath::gameContinues()
{
    /* We need to make 2 decisions: do we go to bubble? do we go to death sequence?
       I'm not 100% sure that both paths will always be evaluated, and in what order
       (depending on the type of death, the anim, explodeDirectly, ...),
       but we need the value to be the same, so the first that runs stores it.
       (otherwise we risk having a non-bubbled player but no death sequence)
    */

    if (m_gameContinues == -1)
    {
        const bbool needsOnscreenPlayer =
            m_parent->m_lastOnscreenPlayerKillsEveryone &&
            !LOADINGSCREEN->isVisible() &&
            !CINEMATICMANAGER->getLockPlayers();
        m_gameContinues = needsOnscreenPlayer ?
            m_parent->hasOtherLivePlayerOnscreen() :
            m_parent->hasOtherLivePlayer();
    }

    return m_gameContinues;
}

bbool Ray_PlayerControllerComponent::StateDeath::checkStateChange( f32 _dt )
{
    if (m_reviveOnFirstLivePlayer)
    {
        m_parent->changeState(&m_parent->m_stateDeadSoul);
        m_parent->m_stateDeadSoul.reviveOnFirstLivePlayer();
        return btrue;
    }

    bbool animFinished = isCurrentAnimFinished();

    if ( m_deactivateOnFinish && animFinished )
    {
        m_deactivateOnFinish = bfalse;
        CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());
        GAMEMANAGER->changePlayerActivation(m_parent->m_playerIndex,bfalse);
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    if ( m_triggerDeadSoul || animFinished )
    {
        m_triggerDeadSoul = bfalse;

        if ( gameContinues() )
        {
            // go to bubble mode
            EventStopPlayer stopPlayer(bfalse);
            m_actor->onEvent(&stopPlayer);

            m_parent->changeState(&m_parent->m_stateDeadSoul);
            return btrue;
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateDeath::update(f32 _dt)
{
    if ( m_spawnFX )
    {
        if ( !m_parent->getTemplate()->getDeathFXActorSpawn().isEmpty() )
        {
            Actor * spawn = m_parent->m_generator.getSpawnee(m_actor->getScene(),m_actor->getPos(),0.0f);

            if (spawn)
            {
                spawn->setStartFlipped(m_actor->isFlipped());
                spawn->setFlipped(m_actor->isFlipped());
                spawn->loadResources();

                m_fxList.push_back(spawn->getRef());
            }
        }

        m_spawnFX = bfalse;
    }

    if ( m_triggerSequence &&
         !RAY_GAMEMANAGER->isSequencePlaying() &&
         !gameContinues() )
    {
        RAY_GAMEMANAGER->triggerRestoreDeath(m_actor->getRef());
        m_triggerSequence = bfalse;
    }



    updateOffscreenFX();
    bringToScreenEdge();
}

void Ray_PlayerControllerComponent::StateDeath::onEvent(Event * _event)
{
    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Bubble,2553917431) )
        {
            if (!m_explodeDirectly)
            {
                m_triggerDeadSoul = btrue;
            }
        }
        else if ( animEvent->getName() == MRK_Death )
        {
            m_triggerSequence = btrue;
        }
        else if ( animEvent->getName() == MRK_SpawnFX )
        {
            m_spawnFX = btrue;
        }
    }
}

void Ray_PlayerControllerComponent::StateDeath::spawnOffscreenFX()
{
    m_offscreenFX.invalidate();


    AABB screenAABB;
    if ( !getScreenAABBCropped(m_parent->getTemplate()->getOffscreenDeathFxMargin(), screenAABB) )
    {
        return;
    }
    //DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0.f, screenAABB, Color::white(), 0.f, 5);

    const AABB subjectAABB = m_parent->m_cameraSubjectComponent->getAABB();
    if ( !screenAABB.checkOverlap(subjectAABB) )
    {
        const Vec2d pos = m_actor->get2DPos();
        const Vec2d targetPos(
            Clamp(pos.m_x, screenAABB.getMin().m_x, screenAABB.getMax().m_x),
            Clamp(pos.m_y, screenAABB.getMin().m_y, screenAABB.getMax().m_y) );
        //DebugDraw::circle(targetPos, m_actor->getDepth(), 0.3f, Color::red(), 0.f);

        if ( Actor* spawn = m_parent->m_generator.getSpawnee(m_actor->getScene(), targetPos.to3d(m_actor->getDepth()), 0.0f) )
        {
            m_offscreenFX = spawn->getRef();
            spawn->setStartFlipped(m_actor->isFlipped());
            spawn->setFlipped(m_actor->isFlipped());
            spawn->loadResources();

            // also position the player at the same place
            m_actor->set2DPos(targetPos);

            m_fxList.push_back(m_offscreenFX);
        }
    }
}

void Ray_PlayerControllerComponent::StateDeath::updateOffscreenFX()
{
    Actor* spawn = m_offscreenFX.getActor();
    if (!spawn)
    {
        return;
    }

    AABB screenAABB;
    if ( !getScreenAABBCropped(m_parent->getTemplate()->getOffscreenDeathFxMargin(), screenAABB) )
    {
        return;
    }
    //DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0.f, screenAABB, Color::green(), 0.f, 5);

    const Vec2d pos = spawn->getWorldInitialPos().truncateTo2D();
    const Vec2d targetPos(
        Clamp(pos.m_x, screenAABB.getMin().m_x, screenAABB.getMax().m_x),
        Clamp(pos.m_y, screenAABB.getMin().m_y, screenAABB.getMax().m_y) );
    //DebugDraw::circle(targetPos, m_actor->getDepth(), 0.3f, Color::red(), 0.f);

    spawn->set2DPos(targetPos);
}

void Ray_PlayerControllerComponent::StateDeath::bringToScreenEdge()
{
    if (!m_offscreenFX.isValid())
    {
        // no fx means we weren't offscreen enough upon death
        // to justify being brought back to the center of the screen
        return;
    }


    AABB screenAABB;
    if ( !getScreenAABBCropped(m_parent->getTemplate()->getOffscreenDeathMargin(), screenAABB) )
    {
        return;
    }
    //DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0.f, screenAABB, Color::white(), 0.f, 5);

    const Vec2d pos = m_actor->get2DPos();
    const Vec2d targetPos(
        Clamp(pos.m_x, screenAABB.getMin().m_x, screenAABB.getMax().m_x),
        Clamp(pos.m_y, screenAABB.getMin().m_y, screenAABB.getMax().m_y) );
    //DebugDraw::circle(targetPos, m_actor->getDepth(), 0.3f, Color::red(), 0.f);

    m_actor->set2DPos(pos + (targetPos - pos) * m_parent->getTemplate()->getOffscreenDeathSmooth());
}

bbool Ray_PlayerControllerComponent::StateDeath::getScreenAABBCropped( const Margin& _margin, AABB& _screenAABB )
{
    if ( CAMERACONTROLLERMANAGER->getAABB(m_actor->getDepth(), _screenAABB) )
    {
        //_margin.addTo(_screenAABB);

        Vec2d min = _screenAABB.getMin();
        Vec2d max = _screenAABB.getMax();

        if ( (min.m_x - _margin.m_left) < (max.m_x + _margin.m_right) )
        {
            min.m_x -= _margin.m_left;
            max.m_x += _margin.m_right;
        }
        else
        {
            min.m_x = max.m_x = (min.m_x + max.m_x) * 0.5f;
        }

        if ( (min.m_y - _margin.m_bottom) < (max.m_y + _margin.m_top) )
        {
            min.m_y -= _margin.m_bottom;
            max.m_y += _margin.m_top;
        }
        else
        {
            min.m_y = max.m_y = (min.m_y + max.m_y) * 0.5f;
        }

        _screenAABB.setMin(min);
        _screenAABB.setMax(max);
        ITF_ASSERT(_screenAABB.isValid());

        return btrue;
    }

    return bfalse;
}

}

