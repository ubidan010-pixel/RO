#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ASPINETWORKCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AspiNetworkComponent.h"
#endif //_ITF_RAY_ASPINETWORKCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GeyserPlatformAIComponent.h"
#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    #define Z_OFFSET -0.002f;

    IMPLEMENT_OBJECT_RTTI(Ray_AspiNetworkComponent_Template)

    BEGIN_SERIALIZATION(Ray_AspiNetworkComponent_Template)
        SERIALIZE_MEMBER("enterDuration", m_enterDuration);
        SERIALIZE_MEMBER("enterBezierMultiplier", m_enterBezierMultiplier);
        SERIALIZE_MEMBER("speed", m_speed);
        SERIALIZE_MEMBER("exitSpeed", m_exitSpeed);
        SERIALIZE_MEMBER("exitDist", m_exitDist);
        SERIALIZE_MEMBER("exitRestoreZDist", m_exitRestoreZDist);
        SERIALIZE_CONTAINER_OBJECT("fxData", m_fxData);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_AspiNetworkComponent_Template, FxData)
        SERIALIZE_MEMBER("playerFamily", m_playerFamily);
        SERIALIZE_MEMBER("fxIn", m_fxIn);
        SERIALIZE_MEMBER("fxOut", m_fxOut);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_AspiNetworkComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_AspiNetworkComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_AspiNetworkComponent)
        VALIDATE_COMPONENT_PARAM("speed", getTemplate()->getSpeed() > 0.f, "must be positive!");
        VALIDATE_COMPONENT_PARAM("exitSpeed", getTemplate()->getExitSpeed() > 0.f, "must be positive!");
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "Ray_AspiNetworkComponent requires a LinkComponent");
        VALIDATE_COMPONENT_PARAM("", m_detectorComponent, "Ray_AspiNetworkComponent requires a DetectorComponent");
        VALIDATE_COMPONENT_PARAM("", m_fxController, "Ray_AspiNetworkComponent requires a FXControllerComponent");
    END_VALIDATE_COMPONENT()

    Ray_AspiNetworkComponent::Ray_AspiNetworkComponent()
        : m_linkComponent(NULL)
        , m_detectorComponent(NULL)
        , m_geyserComponent(NULL)
        , m_fxController(NULL)
    {
    }

    Ray_AspiNetworkComponent::~Ray_AspiNetworkComponent()
    {
    }


    void Ray_AspiNetworkComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>(); // mandatory, validated
        m_detectorComponent = m_actor->GetComponent<DetectorComponent>(); // mandatory, validated
        m_geyserComponent = m_actor->GetComponent<Ray_GeyserPlatformAIComponent>(); // not mandatory
        m_fxController = m_actor->GetComponent<FXControllerComponent>(); // mandatory
        
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventCaughtInPipe,1030315255), this);
    }

    void Ray_AspiNetworkComponent::processNewTraveler( Actor* travelerActor, FriseFollower& friseFollower )
    {
        PhysComponent* travelerPhys = travelerActor->GetComponent<PhysComponent>();
        if (!travelerPhys)
        {
            return;
        }

        bbool isPlayer = GAMEMANAGER->isPlayerActor(travelerActor->getRef());
        if (isPlayer)
        {
            Ray_EventCaughtInPipe eventCatch;
            eventCatch.setSender(m_actor->getRef());
            eventCatch.setCatch(btrue);
            travelerActor->onEvent(&eventCatch);

            if (!eventCatch.getAcceptCatch())
            {
                return;
            }
        }


        // traveler accepted!

        m_travelers.push_back(Traveler());
        Traveler& traveler = m_travelers.back();
        traveler.m_actor = travelerActor->getRef();
        traveler.m_isPlayer = isPlayer;
        traveler.m_initialZ = travelerActor->getDepth();

        if (!traveler.m_isPlayer)
        {
            travelerPhys->setDisabled(btrue);
        }

        const Vec2d entryDir = AIUtils::getLookDir(m_actor);
        Vec3d entryPoint = friseFollower.getEntryPoint();
        entryPoint.m_z += Z_OFFSET;
        traveler.m_enterBezier.m_p0 = travelerActor->getPos();
        traveler.m_enterBezier.m_p1 = travelerActor->getPos() + travelerPhys->getSpeed().to3d().normalize();
        traveler.m_enterBezier.m_p2 = entryPoint + entryDir.to3d() * getTemplate()->getEnterBezierMultiplier();
        traveler.m_enterBezier.m_p3 = entryPoint;

        playFx(traveler, travelerActor);
    }

    void Ray_AspiNetworkComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (Ray_EventCaughtInPipe* eventPipe = _event->DynamicCast<Ray_EventCaughtInPipe>(ITF_GET_STRINGID_CRC(Ray_EventCaughtInPipe,1030315255)))
        {
            if ( eventPipe->getCatch() == bfalse )
            {
                // someone is releasing himself
                u32 numTravellers = m_travelers.size();

                for (u32 i = 0; i < numTravellers; i++)
                {
                    Traveler& traveler = m_travelers[i];

                    if ( traveler.m_actor != eventPipe->getSender() )
                    {
                        continue;
                    }

                    m_travelers.eraseNoOrder(i);
                    break;
                }
            }
        }
    }

    void Ray_AspiNetworkComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        // TODO initialize network (onActorLoaded+editor changes OR on each new traveler)

        AIUtils::LinkIterator iterator(m_linkComponent, btrue);
        Frise* frise = iterator.getNextRelativeFrise();
        iterator.reset();
        Actor* exit = iterator.getNextRelativeActor();

        if (!frise || !exit)
        {
            m_travelers.clear();
            return;
        }

        FriseFollower friseFollower = FriseFollower(frise, m_actor->get2DPos(), exit->get2DPos());


        // detect new travelers

        bbool isOpen = !m_geyserComponent || m_geyserComponent->isOpen();
        if (isOpen)
        {
            ActorRefList& actorsInsideThisFrame = m_detectorComponent->getActorsInside();

            for (u32 i = 0; i < actorsInsideThisFrame.size(); i++)
            {
                ActorRef& actorInside = actorsInsideThisFrame[i];
                if ( m_actorsInsideLastFrame.find(actorInside) == -1 )
                {
                    if ( Actor* travelerActor = actorInside.getActor() )
                    {
                        processNewTraveler(travelerActor, friseFollower);
                    }
                }
            }

            m_actorsInsideLastFrame = actorsInsideThisFrame;
        }



        // update travelers

        for (u32 i = 0; i < m_travelers.size(); i++)
        {
            Traveler& traveler = m_travelers[i];
            Actor* travelerActor = traveler.m_actor.getActor();
            PhysComponent* travelerPhys = travelerActor ? travelerActor->GetComponent<PhysComponent>() : NULL;

            if (!travelerPhys)
            {
                ITF_ASSERT_MSG(0, "where did you go, traveler??");
                m_travelers.eraseNoOrder(i);
                i--;
                return;
            }


            switch (traveler.m_state)
            {
            case State_Enter:
                {
                    traveler.m_timer += _dt;
                    const f32 t = traveler.m_timer / getTemplate()->getEnterDuration();

                    if (t < 1.f)
                    {
                        const Vec3d pos = traveler.m_enterBezier.getPos(t);
                        travelerActor->setPos(pos);
                    }
                    else
                    {
                        traveler.m_timer = 0;
                        traveler.m_state = State_Travel;
                    }

                    updateFx(traveler, travelerActor);
#ifndef ITF_FINAL
                    //DebugDraw::bezier(traveler.m_enterBezier, Color::red(), 1.f, 0.1f);
#endif // ITF_FINAL
                }
                break;
            case State_Travel:
                {
                    traveler.m_timer += _dt;
                    const f32 dist = traveler.m_timer * getTemplate()->getSpeed();

                    if (dist < friseFollower.getLength())
                    {
                        Vec3d pos = friseFollower.getPos(dist);
                        pos.m_z += Z_OFFSET;    // hide the player behind the frise
                        travelerActor->setPos(pos);

                        updateFx(traveler, travelerActor);
                    }
                    else
                    {
                        traveler.m_timer = 0;
                        traveler.m_state = State_Exit;

                        stopFx(traveler, travelerActor);
                    }
                }
                break;
            case State_Exit:
                {
                    traveler.m_timer += _dt;
                    const f32 dist = traveler.m_timer * getTemplate()->getExitSpeed();

                    const Vec2d exitDir = AIUtils::getLookDir(exit);
                    const Vec2d exitSpeed = exitDir * getTemplate()->getExitSpeed();

                    if (dist < getTemplate()->getExitDist())
                    {
                        Vec2d pos = travelerActor->get2DPos();
                        pos += exitSpeed * _dt;
                        if (dist < getTemplate()->getExitRestoreZDist())
                        {
                            travelerActor->set2DPos(pos);
                        }
                        else // restore initial z after a certain dist
                        {
                            travelerActor->setPos( pos.to3d(traveler.m_initialZ) );
                        }
                    }
                    else
                    {
                        travelerPhys->setSpeed(exitSpeed);

                        if (traveler.m_isPlayer)
                        {
                            Ray_EventCaughtInPipe eventRelease;
                            eventRelease.setSender(m_actor->getRef());
                            eventRelease.setCatch(bfalse);
                            travelerActor->onEvent(&eventRelease);
                        }
                        else
                        {
                            travelerPhys->setDisabled(bfalse);
                        }

                        m_travelers.eraseNoOrder(i);
                        i--;
                    }

#ifndef ITF_FINAL
                    //Vec3d exit = friseFollower.getExitPoint();
                    //DebugDraw::line3D(exit, exit + (exitDir * getTemplate()->getExitDist()).to3d(), Color::red());
#endif // ITF_FINAL
                }
                break;
            }
        }
    }


    /* FX:
        when player enters, play in FX (looping)
        when player travels, update FX pos
        when player exits, stop in FX, and play out FX (one-shot)
    */

    const Ray_AspiNetworkComponent_Template::FxData* Ray_AspiNetworkComponent_Template::getFxData( Actor* _travelerActor ) const
    {
        if ( Player* player = GAMEMANAGER->getPlayerFromActor(_travelerActor->getRef()) )
        {
            if ( const PlayerIDInfo* playerInfo = GAMEMANAGER->getPlayerIDInfo(player->getCurrentPlayerIDInfo()) )
            {
                for (u32 i = 0; i < m_fxData.size(); i++)
                {
                    const FxData& fxData = m_fxData[i];
                    if (playerInfo->getFamily() == fxData.m_playerFamily)
                    {
                        return &fxData;
                    }
                }
            }
        }

        return NULL;
    }

    void Ray_AspiNetworkComponent::playFx( Traveler& _traveler, Actor* _travelerActor )
    {
        if (const Ray_AspiNetworkComponent_Template::FxData* fxData = getTemplate()->getFxData(_travelerActor))
        {
            _traveler.m_fxInHandle = m_fxController->playFX(fxData->m_fxIn);
            m_fxController->setFXPos(_traveler.m_fxInHandle, _travelerActor->getPos());
        }
    }

    void Ray_AspiNetworkComponent::updateFx( Traveler& _traveler, Actor* _travelerActor )
    {
        m_fxController->setFXPos(_traveler.m_fxInHandle, _travelerActor->getPos());
    }

    void Ray_AspiNetworkComponent::stopFx( Traveler& _traveler, Actor* _travelerActor )
    {
        m_fxController->stopFX(_traveler.m_fxInHandle);
        _traveler.m_fxInHandle = U32_INVALID;

        if (const Ray_AspiNetworkComponent_Template::FxData* fxData = getTemplate()->getFxData(_travelerActor))
        {
            u32 handle = m_fxController->playFX(fxData->m_fxOut);
            m_fxController->setFXPos(handle, _travelerActor->getPos());
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void Ray_AspiNetworkComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        drawEditFrise();
    }

    void Ray_AspiNetworkComponent::drawEditFrise() const
    {
        AIUtils::LinkIterator iterator(m_linkComponent, btrue);

        Frise* frise = iterator.getNextRelativeFrise();
        if (!frise)
        {
            return;
        }

        iterator.reset();
        Actor* exit = iterator.getNextRelativeActor();
        if (!exit)
        {
            return;
        }

        Matrix44 matrix = frise->getTransformMatrix(btrue);
        const PolyPointList& controlPoints = frise->getPointsData();
        const u32 controlPointsCount = controlPoints.getPosCount();

        for (u32 i = 0; i < controlPointsCount - 1; i++)
        {
            Vec3d p0 = frise->transformPos( matrix, controlPoints.getPosAt(i).to3d() );
            Vec3d p1 = frise->transformPos( matrix, controlPoints.getPosAt(i + 1).to3d() );
            DebugDraw::line3D(p0, p1);
            //DebugDraw::circle(p0, 0.3f);
        }

        FriseFollower friseFollower = FriseFollower(frise, m_actor->get2DPos(), exit->get2DPos());
        friseFollower.drawDebug();
    }
#endif // ITF_SUPPORT_EDITOR

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void Ray_AspiNetworkComponent::FriseFollower::drawDebug()
    {
        const PolyPointList& controlPoints = m_frise->getPointsData();

        if (m_inEdgeIndex < m_outEdgeIndex)
        {
            Vec3d inPos         = m_frise->transformPos( m_matrix, (m_inEdge.getPos() + m_inEdge.m_vector * m_inEdgeRatio).to3d() );
            Vec3d inPosEnd      = m_frise->transformPos( m_matrix, (m_inEdge.getPos() + m_inEdge.m_vector).to3d() );
            DebugDraw::arrow3D(inPos, inPosEnd, Color::green());

            for (i32 i = m_inEdgeIndex + 1; i < m_outEdgeIndex; i++)
            {
                Vec3d p0 = m_frise->transformPos( m_matrix, controlPoints.getPosAt(i).to3d() );
                Vec3d p1 = m_frise->transformPos( m_matrix, controlPoints.getPosAt(i + 1).to3d() );
                DebugDraw::arrow3D(p0, p1, Color::blue());
            }

            Vec3d outPosStart   = m_frise->transformPos( m_matrix, m_outEdge.getPos().to3d() );
            Vec3d outPos        = m_frise->transformPos( m_matrix, (m_outEdge.getPos() + m_outEdge.m_vector * m_outEdgeRatio).to3d() );
            DebugDraw::arrow3D(outPosStart, outPos, Color::red());
        }
        else if (m_inEdgeIndex > m_outEdgeIndex)
        {
            Vec3d inPosStart    = m_frise->transformPos( m_matrix, (m_inEdge.getPos()).to3d() );
            Vec3d inPos         = m_frise->transformPos( m_matrix, (m_inEdge.getPos() + m_inEdge.m_vector * m_inEdgeRatio).to3d() );
            DebugDraw::arrow3D(inPos, inPosStart, Color::green());

            ITF_ASSERT(m_inEdgeIndex > 0);
            for (i32 i = m_inEdgeIndex - 1; i >= m_outEdgeIndex; i--)
            {
                Vec3d p0 = m_frise->transformPos( m_matrix, controlPoints.getPosAt(i).to3d() );
                Vec3d p1 = m_frise->transformPos( m_matrix, controlPoints.getPosAt(i + 1).to3d() );
                DebugDraw::arrow3D(p1, p0, Color::blue());
            }

            Vec3d outPos        = m_frise->transformPos( m_matrix, (m_outEdge.getPos() + m_outEdge.m_vector * m_outEdgeRatio).to3d() );
            Vec3d outPosEnd     = m_frise->transformPos( m_matrix, (m_outEdge.getPos() + m_outEdge.m_vector).to3d() );
            DebugDraw::arrow3D(outPosEnd, outPos, Color::red());
        }
        else // ==
        {
            Vec3d inPos     = m_frise->transformPos( m_matrix, (m_inEdge.getPos() + m_inEdge.m_vector * m_inEdgeRatio).to3d() );
            Vec3d outPos    = m_frise->transformPos( m_matrix, (m_outEdge.getPos() + m_outEdge.m_vector * m_outEdgeRatio).to3d() );
            DebugDraw::arrow3D(inPos, outPos, Color::blue());
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    Ray_AspiNetworkComponent::FriseFollower::FriseFollower( const Frise* _frise, const Vec2d& _from, const Vec2d& _to )
        : m_frise(_frise)
        , m_length(0.f)
        , m_inEdgeIndex(0)
        , m_inEdgeRatio(0.f)
        , m_outEdgeIndex(0)
        , m_outEdgeRatio(0.f)
    {
        m_matrix = m_frise->getTransformMatrix(btrue);

        m_frise->getNearestEdgeFromPos(_from, m_inEdge, m_inEdgeIndex, m_inEdgeRatio);

        m_frise->getNearestEdgeFromPos(_to, m_outEdge, m_outEdgeIndex, m_outEdgeRatio);


        if (m_inEdgeIndex < m_outEdgeIndex)
        {
            m_length = m_inEdge.m_length * (1 - m_inEdgeRatio);

            for (i32 i = m_inEdgeIndex + 1; i < m_outEdgeIndex; i++)
            {
                m_length += m_frise->getEdgeAt(i).m_length;
            }

            m_length += m_outEdge.m_length * m_outEdgeRatio;
        }
        else if (m_inEdgeIndex > m_outEdgeIndex)
        {
            m_length = m_inEdge.m_length * m_inEdgeRatio;

            for (i32 i = m_outEdgeIndex + 1; i < m_inEdgeIndex; i++)
            {
                m_length += m_frise->getEdgeAt(i).m_length;
            }

            m_length += m_outEdge.m_length * (1.f - m_outEdgeRatio);
        }
        else // ==
        {
            m_length = m_inEdge.m_length * f32_Abs(m_outEdgeRatio - m_inEdgeRatio);
        }
    }

    Vec3d Ray_AspiNetworkComponent::FriseFollower::getPos( f32 _dist )
    {
        Vec3d pos;

        if (m_inEdgeIndex < m_outEdgeIndex)
        {
            if ( parseEdge(m_inEdge, m_inEdgeRatio, 1.f, _dist, pos) )
            {
                return pos;
            }

            for (i32 i = m_inEdgeIndex + 1; i < m_outEdgeIndex; i++)
            {
                if ( parseEdge(m_frise->getEdgeAt(i), 0.f, 1.f, _dist, pos) )
                {
                    return pos;
                }
            }

            if ( parseEdge(m_outEdge, 0.f, m_outEdgeRatio, _dist, pos) )
            {
                return pos;
            }
        }
        else if (m_inEdgeIndex > m_outEdgeIndex)
        {
            if ( parseEdgeReverse(m_inEdge, m_inEdgeRatio, 0.f, _dist, pos) )
            {
                return pos;
            }

            ITF_ASSERT(m_inEdgeIndex > 0);
            for (i32 i = m_inEdgeIndex - 1; i >= m_outEdgeIndex; i--)
            {
                if ( parseEdgeReverse(m_frise->getEdgeAt(i), 1.f, 0.f, _dist, pos) )
                {
                    return pos;
                }
            }

            if ( parseEdgeReverse(m_outEdge, 1.f, m_outEdgeRatio, _dist, pos) )
            {
                return pos;
            }
        }
        else // ==
        {
            bbool ok;
            if (m_inEdgeRatio < m_outEdgeRatio)
            {
                ok = parseEdge(m_inEdge, m_inEdgeRatio, m_outEdgeRatio, _dist, pos);
            }
            else
            {
                ok = parseEdgeReverse(m_inEdge, m_inEdgeRatio, m_outEdgeRatio, _dist, pos);
            }

            ITF_ASSERT(ok);
            return pos;
        }

        return Vec3d::Zero;
    }

    bbool Ray_AspiNetworkComponent::FriseFollower::parseEdge(
        const PolyLineEdge& _edge, const f32 _edgeStart, const f32 _edgeEnd,
        f32& _dist, Vec3d& _pos )
    {
        f32 edgeLength = _edge.m_length * (_edgeEnd - _edgeStart);

        if (edgeLength >= _dist)
        {
            Vec2d edgePos = _edge.getPos() + _edge.m_normalizedVector * (_edgeStart * _edge.m_length + _dist);
            _pos = m_frise->transformPos(m_matrix, edgePos.to3d());
            return btrue;
        }
        else
        {
            _dist -= edgeLength;
            return bfalse;
        }
    }

    bbool Ray_AspiNetworkComponent::FriseFollower::parseEdgeReverse(
        const PolyLineEdge& _edge, const f32 _edgeStart, const f32 _edgeEnd,
        f32& _dist, Vec3d& _pos )
    {
        f32 edgeLength = _edge.m_length * (_edgeStart - _edgeEnd);

        if (edgeLength >= _dist)
        {
            //Vec2d start = _edge.getPos() + _edge.m_normalizedVector * _edgeStart * _edge.m_length;
            //Vec2d end = _edge.getPos() + _edge.m_normalizedVector * _edgeEnd * _edge.m_length;
            //DebugDraw::arrow2D(m_frise->transformPos(m_matrix, start.to3d()), m_frise->transformPos(m_matrix, end.to3d()));

            Vec2d edgePos = _edge.getPos() + _edge.m_normalizedVector * (_edgeStart * _edge.m_length - _dist);
            _pos = m_frise->transformPos(m_matrix, edgePos.to3d());
            return btrue;
        }
        else
        {
            _dist -= edgeLength;
            return bfalse;
        }
    }

    Vec3d Ray_AspiNetworkComponent::FriseFollower::getEntryPoint()
    {
        return m_frise->transformPos( m_matrix, (m_inEdge.getPos() + m_inEdge.m_vector * m_inEdgeRatio).to3d() );
    }

    Vec3d Ray_AspiNetworkComponent::FriseFollower::getExitPoint()
    {
        return m_frise->transformPos( m_matrix, (m_outEdge.getPos() + m_outEdge.m_vector * m_outEdgeRatio).to3d() );
    }

    Vec2d Ray_AspiNetworkComponent::FriseFollower::getEntryDir()
    {
        Vec2d dir;

        if (m_inEdgeIndex < m_outEdgeIndex)
        {
            dir = m_inEdge.m_normalizedVector;
        }
        else if (m_inEdgeIndex > m_outEdgeIndex)
        {
            dir = -m_inEdge.m_normalizedVector;
        }
        else // ==
        {
            if (m_inEdgeRatio < m_outEdgeRatio)
            {
                dir = m_inEdge.m_normalizedVector;
            }
            else
            {
                dir = -m_inEdge.m_normalizedVector;
            }
        }

        //return m_frise->transformPos(m_matrix, dir.to3d()).truncateTo2D();
        return m_frise->transformPos(dir.to3d(), bfalse).truncateTo2D();
    }

    Vec2d Ray_AspiNetworkComponent::FriseFollower::getExitDir()
    {
        Vec2d dir;

        if (m_outEdgeRatio < m_outEdgeRatio)
        {
            dir = m_outEdge.m_normalizedVector;
        }
        else
        {
            dir = -m_outEdge.m_normalizedVector;
        }

        //return m_frise->transformPos(m_matrix, dir.to3d()).truncateTo2D();
        return m_frise->transformPos(dir.to3d(), bfalse).truncateTo2D();
    }

};
