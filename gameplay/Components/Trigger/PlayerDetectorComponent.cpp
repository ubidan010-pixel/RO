#include "precompiled_gameplay.h"

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PlayerDetectorComponent)

    BEGIN_SERIALIZATION_CHILD(PlayerDetectorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(PlayerDetectorComponent)
    END_VALIDATE_COMPONENT()


    PlayerDetectorComponent::PlayerDetectorComponent()
    {
    }

    PlayerDetectorComponent::~PlayerDetectorComponent()
    {
    }

    void PlayerDetectorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_actorsInside.clear();


        f32 maxScale = std::max(m_actor->getScale().m_x, m_actor->getScale().m_y);

        f32 maxDetectionRadiusSQ = getTemplate()->getMaxDetectionRadius();
        if (maxDetectionRadiusSQ>0)
        {
            maxDetectionRadiusSQ *= maxScale;
            maxDetectionRadiusSQ *= maxDetectionRadiusSQ;
        }
        else
        {
            maxDetectionRadiusSQ = MTH_HUGE;
        }

        if ( m_disabled || !m_currentShape )
        {
            return;
        }

        if ( ( getTemplate()->getAllPlayerInMode() != GAMEMODE_UNKNOWN ) && !GAMEMANAGER->areAllActivePlayersInGameMode( getTemplate()->getAllPlayerInMode() ) )
        {
            return;
        }

        const i32 playerCount = GAMEMANAGER->getMaxPlayerCount();

        i32 playerToDetect = -1;
        if ( getTemplate()->getFirstPlayerOnly() )
        {            
            for (i32 i = 0; i < playerCount && playerToDetect == -1; ++i)
            {
                Player* player = GAMEMANAGER->getPlayer(i);

                if ( player->getActive() )
                {
                    if ( !player->isDead() || getTemplate()->getAllowDeadActors())
                    {
                        playerToDetect = i;
                    }
                }
            }
        }

        const Vec2d& myPos = m_shapePos;
        const f32 myAngle = m_actor->getAngle();
        const PhysShape* myShape = m_currentShape;
        AABB myAABB;
        PhysSweepInfo mySweep;
        bbool computedSweepInfo = bfalse;

        const DepthRange depthRange(m_actor->getDepth());
        for (i32 i = 0; i < playerCount; ++i)
        {
            if (getTemplate()->getPlayerID() >= 0)
            {
                if (getTemplate()->getPlayerID() != i)
                {
                    continue;
                }
            }

            Player* player = GAMEMANAGER->getPlayer(i);

            if (!player->getActive())
            {
                continue;
            }
            
            if (!getTemplate()->getAllowDeadActors())
            {
                if (player->isDead())
                {
                    continue;
                }
            }


            Actor* playerActor = player->getActor();
            if ( !playerActor )
            {
                continue;
            }

            const Vec2d& playerPos = playerActor->get2DPos();
            if ((playerPos-myPos).sqrnorm()>maxDetectionRadiusSQ)
            {
                continue;
            }

            if (depthRange != DepthRange(playerActor->getDepth()) )
            {
                continue;
            }

            if (!computedSweepInfo)
            {
                myShape->calculateAABB(myPos, myPos, myAngle, myAABB);
                PhysCollisionSolver::calculateSweepInfo(myPos, myPos, myAngle, myShape, mySweep);
                computedSweepInfo = btrue;
            }

            PlayerControllerComponent* playerController = playerActor->GetComponent<PlayerControllerComponent>();

            if (!playerController)
            {
                continue;
            }


            const f32 playerAngle = playerActor->getAngle();
            const PhysShape* playerShape = playerController->getPhantom()->getShape();
            AABB playerAABB;
            playerShape->calculateAABB(playerPos, playerPos, playerAngle, playerAABB);

            if ( !myAABB.checkOverlap(playerAABB) )
            {
                continue;
            }

            PhysContactsContainer contacts;
            PhysSweepInfo sweepA;

            PhysCollisionSolver::calculateSweepInfo(playerPos, playerPos, playerAngle, playerShape, sweepA);
            PhysCollisionSolver::collide(sweepA, mySweep, contacts);

            if ( contacts.size() == 0 )
            {
                continue;
            }

            if ( !playerController->isDetectable(m_actor->getRef()) )
            {
                continue;
            }

            if ( playerToDetect != -1 && i > playerToDetect)
                return;

            m_actorsInside.push_back(playerActor->getRef());
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(PlayerDetectorComponent_Template)
    BEGIN_SERIALIZATION_CHILD(PlayerDetectorComponent_Template)

        SERIALIZE_MEMBER("playerId", m_playerId);
        SERIALIZE_MEMBER("allowDeadActors", m_allowDeadActors);
        SERIALIZE_MEMBER("firstPlayerOnly",m_firstPlayerOnly);
        SERIALIZE_MEMBER("allPlayerInMode",m_allPlayerInMode );
        SERIALIZE_MEMBER("maxDetectionRadius", m_maxDetectionRadius);
    END_SERIALIZATION()

    PlayerDetectorComponent_Template::PlayerDetectorComponent_Template()
        : m_playerId(-1)
        , m_allowDeadActors(bfalse)
        , m_firstPlayerOnly(bfalse)
        , m_allPlayerInMode( GAMEMODE_UNKNOWN )
        , m_maxDetectionRadius( -1.f )
    {
    }
};