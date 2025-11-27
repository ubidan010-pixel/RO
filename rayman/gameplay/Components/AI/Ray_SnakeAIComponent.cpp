#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSMORAYNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayNodeComponent.h"
#endif //_ITF_RAY_BOSSMORAYNODECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_


//----------------------- factory files ------------------------

#ifndef _ITF_RAY_SNAKEBODYPART_SPRITE_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Sprite.h"
#endif //_ITF_RAY_SNAKEBODYPART_SPRITE_H_

#ifndef _ITF_RAY_SNAKEBODYPART_MIXED_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Mixed.h"
#endif //_ITF_RAY_SNAKEBODYPART_MIXED_H_

#ifndef _ITF_RAY_SNAKEBODYPART_DESTRUCTIBLE_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Destructible.h"
#endif //_ITF_RAY_SNAKEBODYPART_DESTRUCTIBLE_H_

#ifndef _ITF_RAY_SHOOTERGARDIANMORAY_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterGardianMoray.h"
#endif //_ITF_RAY_SHOOTERGARDIANMORAY_H_

#ifndef _ITF_RAY_BOSSMORAYBODYPART_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayBodyPart.h"
#endif //_ITF_RAY_BOSSMORAYBODYPART_H_

//--------------------------------------------------------------

#define MAX_BODYPARTCOUNT 128

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_SnakeAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_SnakeAIComponent)
        SERIALIZE_ActiveMode("activeMode", m_activeMode);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_SnakeAIComponent)
        VALIDATE_COMPONENT_PARAM("bodyParts", getTemplate()->getBodyParts().size() > 0, "you need some bodyParts");
        VALIDATE_COMPONENT_PARAM("sampleLength", getTemplate()->getSampleLength() > 0, "must be positive!");
        VALIDATE_COMPONENT_PARAM("acceleration", getTemplate()->getAcceleration() > 0, "must be positive!");
    END_VALIDATE_COMPONENT()


    Ray_SnakeAIComponent::Ray_SnakeAIComponent()
        : Super()
        , m_activeMode(ActiveMode_Default)
        , m_renderMode(RenderMode_Invalid)
        , m_animComponent( NULL )
        , m_fxController( NULL )
        , m_currentSpeed(0.f)
        , m_targetSpeed(0.f)
        , m_currentNodeDist(0.f)
        , m_currentSpeedMultiplier(1.f)
        , m_targetSpeedMultiplier(1.f)
        , m_speedMultiplierDisabled(bfalse)
        , m_isAttacking(bfalse)
        , m_isDying(bfalse)
        , m_acceleration(0.0f)
        , m_totalBodyLength(0.f)
        , m_headEdgeCount(0)
        , m_trajectoryLengthMultiplier(1.f)
        , m_trajectoryJustInitialized(bfalse)
    {
    }

    Ray_SnakeAIComponent::~Ray_SnakeAIComponent()
    {
        // clear & delete body parts
        for (u32 i = 0; i < m_bodyParts.size(); i++)
        {
            if ( BodyPartBase* bodyPart = m_bodyParts[i] )
            {
                bodyPart->clear();
                delete(bodyPart);
            }
        }
        m_bodyParts.clear();

        for (u32 i = 0; i < m_deadBodyParts.size(); i++)
        {
            if ( BodyPartBase* bodyPart = m_deadBodyParts[i] )
            {
                bodyPart->clear();
                delete(bodyPart);
            }
        }
        m_deadBodyParts.clear();


        m_atlas.clear();


#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), this);
#endif //ITF_SUPPORT_EDITOR
    }

    void Ray_SnakeAIComponent::clear()
    {
        // clear body parts
        for (u32 i = 0; i < m_bodyParts.size(); i++)
        {
            if ( BodyPartBase* bodyPart = m_bodyParts[i] )
            {
                bodyPart->clear();
            }
        }

        for (u32 i = 0; i < m_deadBodyParts.size(); i++)
        {
            if ( BodyPartBase* bodyPart = m_deadBodyParts[i] )
            {
                bodyPart->clear();
            }
        }
    }

    void Ray_SnakeAIComponent::resetMovement()
    {
        m_currentSpeed = 0.f;
        m_targetSpeed = 0.f;
        m_currentNodeDist = 0.f;
    }

    void Ray_SnakeAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();  // not mandatory
        m_fxController = m_actor->GetComponent<FXControllerComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSetInitialNode,3466697001), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSetNodeSelector,116563250), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBossMorayNodeReached,491301045), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443), this);

#ifdef ITF_SUPPORT_EDITOR
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374), this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),this);
#endif //ITF_SUPPORT_EDITOR 

        m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);



        // init body parts
        const BodyPartBaseTemplateList& tplBodyParts = getTemplate()->getBodyParts();
        //m_bodyParts.resize(tplBodyParts.size());  // TODO: validate

        for (u32 i = 0; i < tplBodyParts.size(); i++)
        {
            BodyPartBase_Template* tplBodyPart = tplBodyParts[i];
            BodyPartBase* bodyPart = tplBodyPart->createBodyPart();

            m_bodyParts.push_back(bodyPart);
            bodyPart->setIndex(i);

            bodyPart->setTemplate(tplBodyPart);
            bodyPart->initialize(m_actor, this);

            if (m_renderMode != RenderMode_Invalid)
            {
                ITF_ASSERT_MSG(bodyPart->getRenderMode() == m_renderMode, "all body parts must use the same render mode!");
            }
            m_renderMode = bodyPart->getRenderMode();
        }

        initScale();



        bbool createPolylinePhantom = bfalse;
        // destructible ?
        if ( ( getTemplate()->getDestructibleMode() != Ray_SnakeAIComponent_Template::None ) || getTemplate()->getBroadcastHitToPart() )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
            createPolylinePhantom = btrue;
        }

        m_enclosingPolyline.init(m_actor,this, createPolylinePhantom );
        if ( getTemplate()->getGameMaterial().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
        {
            World::addGameMaterialFromFile(getTemplate()->getGameMaterial(), m_actor->getResourceGroup());
            m_enclosingPolyline.setGameMaterial(getTemplate()->getGameMaterial());
        }



        m_acceleration = getTemplate()->getAcceleration();


        if (m_renderMode == RenderMode_Sprite)
        {
            m_atlas.initialize(m_actor->getResourceGroup(), getTemplate()->getAtlasPath(), m_bodyParts.size());
        }


        m_prevNodes.reserve(getTemplate()->getPrevNodeCount());
    }

    void Ray_SnakeAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        for (u32 i = 0; i < m_bodyParts.size(); i++)
        {
            BodyPartBase* bodyPart = m_bodyParts[i];
            bodyPart->onBecomeActive();
        }

        if (!m_trajectory.isInitialized())
        {
            initTrajectory(m_actor->get2DPos(), Vec2d::Up.Rotate(m_actor->getAngle()));
        }
    }

    void Ray_SnakeAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        clear();    // TODO: isn't that a bit brutal?
    }

    // returns true if the body part has been removed from the body part list
    // happens during gameplay: when a part dies, we either put it in the keepalive list, or destroy it altogether
    //
    bbool Ray_SnakeAIComponent::destroyBodyPart( BodyPart* _bodyPart, u32 _idxInList )
    {
        ITF_ASSERT( _idxInList < m_bodyParts.size() );
        if ( _idxInList >= m_bodyParts.size() )
        {
            return bfalse;
        }

        if ( _bodyPart->getTemplate()->destroyOnDeath() || _bodyPart->removeAndKeepAliveOnDeath() )
        {
            m_bodyParts.removeAt(_idxInList);

            if ( _bodyPart->removeAndKeepAliveOnDeath() )
            {
                // deactivate & keep alive
                _bodyPart->startKeepAlive();
                m_deadBodyParts.push_back(_bodyPart);
            }
            else
            {
                // destroy
                _bodyPart->clear();
                SF_DEL(_bodyPart);
            }

            return btrue;
        }

        return bfalse;
    }

    // scale body part (set length) and compute total length (for trajectory)
    void Ray_SnakeAIComponent::initScale()
    {
        const BodyPartBaseTemplateList& tplBodyParts = getTemplate()->getBodyParts();
        const f32 scale = m_actor->getScale().m_x;

        m_totalBodyLength = 0;

        for (u32 i = 0; i < tplBodyParts.size(); i++)
        {
            BodyPartBase* bodyPart = m_bodyParts[i];

            bodyPart->setLengthOnTrajectory(bodyPart->getTemplate()->getLengthOnTrajectory() * scale);
            m_totalBodyLength += bodyPart->getLengthOnTrajectory();
        }
    }

    void Ray_SnakeAIComponent::initTrajectory( const Vec2d& _pos, const Vec2d& _dir )
    {
        const f32 scale = m_actor->getScale().m_x;

        m_trajectory.init(m_totalBodyLength * m_trajectoryLengthMultiplier, getTemplate()->getSampleLength() * scale, _pos, _dir);
        m_trajectoryJustInitialized = btrue;
        //m_trajectory.drawDebug(m_actor->getDepth(), Color::white(), 20, 15);

        // init prev pos for head angle
        m_prevPos = _pos;
    }



    void Ray_SnakeAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);


        // NODE & TRAJECTORY STUFF
        if ( !m_isDying )
        {
            updateNodes(_dt);

            if (!m_trajectoryJustInitialized)
            {
                // onNodeReached(), we may trigger a relay that will trigger us back:
                // -> this will set a new initial node IN THE MIDDLE OF OUR UPDATE.
                // Even if we teleport there straight away, updateNodes() will then overwrite our pos.
                // We then end up far away from where we initialized the trajectory, and basically overwrite it.
                m_trajectory.update(m_actor->get2DPos());
            }


            blendSpeedAndMultiplier(_dt);

            if ( useUpdateSpeedMultiplier() || useCloseRangeAttack() )
            {
                if ( Actor* target = AIUtils::getClosestLivePlayer(m_actor) )
                {
                    const Vec2d offset = getTemplate()->getTargetEvaluationOffset().Rotate(m_actor->getAngle() + MTH_PIBY2);
                    const f32 targetSqrDist = (target->get2DPos() - m_actor->get2DPos() - offset).sqrnorm();

                    updateSpeedMultiplier(targetSqrDist);

                    updateCloseRangeAttack(targetSqrDist);
                }
            }
        }


        // BODY PART STUFF
        updateBodyParts(_dt);
        updateDeadBodyParts(_dt);

        // AI STUFF
        updateAI();


        m_trajectoryJustInitialized = bfalse;
    }

    void Ray_SnakeAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( _event->GetObjectClassCRC() == ITF_GET_STRINGID_CRC(EventTrigger,1343042510) )   // must be an exact match because Ray_EventNodeReached inherits from EventTrigger...
        {
            processActivateEvent(_event);
        }
        else if ( Ray_EventSetInitialNode* nodeEvent = _event->DynamicCast<Ray_EventSetInitialNode>(ITF_GET_STRINGID_CRC(Ray_EventSetInitialNode,3466697001)) )
        {
            processActivateEvent(_event);
        }
        else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_EventSetNodeSelector,116563250)) )
        {
            m_nodeSelector = _event->getSender();
        }
        else if ( Ray_EventBossMorayNodeReached* nodeReached = _event->DynamicCast<Ray_EventBossMorayNodeReached>(ITF_GET_STRINGID_CRC(Ray_EventBossMorayNodeReached,491301045)) )
        {
            processNodeReached(nodeReached);
        }
        else if ( HitStim* _stim = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
        {
            processHitStim(_stim);
        }
        else if ( EventQueryIsDead* deadQuery = _event->DynamicCast<EventQueryIsDead>(ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443)) )
        {
            deadQuery->setIsDead( m_isDying );
        }
#ifdef ITF_SUPPORT_EDITOR
        else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374)) )
        {
            onScaleChanged();
        }
        else if ( Ray_EventQueryLumCount* queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)) )
        {
            // check destructible bodypart
            for (u32 i = 0; i < m_bodyParts.size(); i++)
            {
                if ( DestructibleBodyPart* part = m_bodyParts[i]->DynamicCast<DestructibleBodyPart>(ITF_GET_STRINGID_CRC(DestructibleBodyPart,81228598)) )
                {
                    const Ray_EventSpawnRewardLum *reward = IRTTIObject::SafeDynamicCast<const Ray_EventSpawnRewardLum>(part->getReward(),ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722));
                    if ( reward )
                    {
                        queryLumCount->addNormalLums( reward->getNumRewards() );
                    }
                }                
            }
        }
#endif // ITF_SUPPORT_EDITOR

        m_enclosingPolyline.onEvent(_event);

        if ( getTemplate()->getBroadcastEventsToPart() )
        {
            for (u32 i = 0; i < m_bodyParts.size(); i++)
            {
                m_bodyParts[i]->onEvent(_event);
            }
        }
    }

    // resolve all pointers for each body part; exit update if invalid
    bbool Ray_SnakeAIComponent::validateBodyParts( const f32 _dt )
    {
        i32 bodyPartCount = m_bodyParts.size();

        for ( i32 i = 0; i < bodyPartCount; i++)
        {
            BodyPartBase* bodyPart = m_bodyParts[i];

            if ( !bodyPart->validate() )
            {
                return bfalse;
            }

            bodyPart->setIndex(i);

            // update the body part
            bodyPart->update( _dt );

            // destroy the part if needed
            if ( BodyPart* destructiblePart = bodyPart->DynamicCast<BodyPart>(ITF_GET_STRINGID_CRC(BodyPart,3043470114)) )   // TODO: this is ugly...
            {
                if ( destructiblePart->isDead() )
                {
                    if ( destructiblePart->isTail() && i > 0 )
                    {
                        m_bodyParts[i-1]->setIndex(i-1);
                    }

                    if ( destroyBodyPart(destructiblePart, i) )
                    {
                        i--;
                        bodyPartCount--;
                    }                
                }  
            }
        }

        return bodyPartCount > 0;
    }

    void Ray_SnakeAIComponent::updateBodyParts( const f32 _dt )
    {
        if ( !validateBodyParts(_dt) )
        {
            m_enclosingPolyline.deactivate();
            return;
        }

        positionBodyPartsOnTrajectory();

        if ( !m_isDying )
        {
            updatePolyline(_dt);
        }
    }


    void Ray_SnakeAIComponent::updateDeadBodyParts( const f32 _dt )
    {
        i32 deadBodyPartCount = m_deadBodyParts.size();

        for ( i32 i = 0; i < deadBodyPartCount; i++)
        {
            BodyPart* bodyPart = static_cast<BodyPart*>(m_deadBodyParts[i]);

            if ( !bodyPart->validate() )
            {
                continue;
            }

            // update the body part
            if ( bodyPart->updateKeepAlive(_dt) )
            {
                bodyPart->clear();
                SF_DEL(bodyPart);
                m_deadBodyParts.removeAt(i);
                i--;
                deadBodyPartCount--;
            }
        }
    }

    void Ray_SnakeAIComponent::positionBodyPartsOnTrajectory( const f32 _offset /*= 0.f*/ )
    {
        // also grow the AABB in the same loop
        AABB aabb(m_actor->getPos());


        // set head angle
        Vec2d delta = (m_actor->get2DPos() - m_prevPos);
        if ( !delta.IsEqual(Vec2d::Zero, MTH_EPSILON) )
        {
            m_actor->setAngle(delta.normalize().getAngle() - MTH_PIBY2);
            m_prevPos = m_actor->get2DPos();
        }


        // position body parts
        Vec2d p0 = _offset ? m_trajectory.getPos(_offset) : m_actor->get2DPos();
        f32 dist = _offset;
        f32 rank = 0;

        for (u32 i = 0; i < m_bodyParts.size(); i++)
        {
            BodyPartBase* bodyPart = m_bodyParts[i];

            dist += bodyPart->getLengthOnTrajectory();
            rank += getTemplate()->getBodyPartsZSpacing();

            const Vec2d p1 = m_trajectory.getPos(dist);
            const f32 angle = (p0 - p1).normalize().getAngle() - MTH_PIBY2;
            const f32 z = m_actor->getDepth() + rank;

            bodyPart->positionOnTrajectory(p0.to3d(z), angle);
            bodyPart->onTrajectoryUpdate();

            //String s;s.setTextFormat("%d", i);
            //DebugDraw::circle(p0, z, 0.1f, Color::red(), 0, s);
            //DebugDraw::line2D(p0, p1, z, Color::red());

            p0 = p1;


            aabb.grow(bodyPart->getAABB());
        }


        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    void Ray_SnakeAIComponent::updatePolyline( const f32 _dt )
    {
        if (!getTemplate()->getUsePolyline())
        {
            return;
        }

        switch (m_renderMode)
        {
        case RenderMode_Actor:
            updatePolyline_actor(_dt);
            break;

        case RenderMode_Sprite:
            updatePolyline_sprite(_dt);
            break;
        }
    }

    void Ray_SnakeAIComponent::updatePolyline_actor( const f32 _dt )
    {
        const i32 bodyPartCount = m_bodyParts.size();

        // create a shape enclosing the whole body
        PolyLine& poly = m_enclosingPolyline.getPolyline();
        poly.clear();

        // head
        if (m_animComponent)
        {
            u32 pointCount;
            const Vec2d* points = m_animComponent->getCurrentPolylinePointBuffer(getTemplate()->getHeadPolyline(), &pointCount);
            for (u32 j = 0; j < pointCount; j++)
            {
                poly.addPoint(points[j]);
            }
            m_headEdgeCount = Max( 0, i32( pointCount - 1 ) );
        }

        // right side of body
        bbool rightSideTailValid = bfalse;
        {
            for ( i32 i = 0; i < bodyPartCount; i++)
            {
                BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[i]);
                u32 pointCount;
                const Vec2d* points = bodyPart->getAnimComponent()->getCurrentPolylinePointBuffer( bodyPart->getTemplate()->getActorRenderer().getOtherPolyline(), &pointCount);
                if (points)
                {
                    if ( i == bodyPartCount )
                    {
                        rightSideTailValid = btrue;     // flag set for left side to known on which edge idx to start
                    }
                    bodyPart->m_rightFirstEdgeIdx = poly.getVectorsCount();
                    bodyPart->m_rightLastEdgeIdx = bodyPart->m_rightFirstEdgeIdx + Max(  0, i32( pointCount - 1 ) );
                    for (u32 j = 0; j < pointCount; j++)
                    {
                        poly.addPoint(points[j]);
                    }
                }
            }
        }

        // left side of body
        {
            for ( i32 i = bodyPartCount - 1; i >= 0; i--)
            {
                BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[i]);
                u32 pointCount;
                const Vec2d* points = bodyPart->getAnimComponent()->getCurrentPolylinePointBuffer( bodyPart->getTemplate()->getActorRenderer().getPolyline(), &pointCount);
                if (points)
                {
                    // is the tail
                    if ( i == ( bodyPartCount - 1 ) )
                    {
                        bodyPart->m_leftFirstEdgeIdx = poly.getVectorsCount();
                        bodyPart->m_leftLastEdgeIdx = bodyPart->m_leftFirstEdgeIdx + pointCount; // add one more edge

                        if ( !rightSideTailValid )
                        {
                            bodyPart->m_rightFirstEdgeIdx = bodyPart->m_leftFirstEdgeIdx;
                            bodyPart->m_rightLastEdgeIdx = bodyPart->m_leftLastEdgeIdx;
                        }
                    }
                    else
                    {
                        bodyPart->m_leftFirstEdgeIdx = poly.getVectorsCount() + 1;
                        bodyPart->m_leftLastEdgeIdx = bodyPart->m_leftFirstEdgeIdx + Max(  0, i32( pointCount - 1 ) );
                    }
                    
                    for (u32 j = 0; j < pointCount; j++)
                    {
                        poly.addPoint(points[j]);
                    }
                }
            }
        }

        // close the polyline
        poly.addPoint(poly.getPosAt(0));


        m_enclosingPolyline.activate();
        m_enclosingPolyline.update(_dt);
    }

    void Ray_SnakeAIComponent::updatePolyline_sprite( const f32 _dt )
    {
        const i32 bodyPartCount = m_bodyParts.size();

        // create a shape enclosing the whole body
        PolyLine& poly = m_enclosingPolyline.getPolyline();
        poly.clear();

        // first pass (right side)
        for ( i32 i = 0; i < bodyPartCount; i++)
        {
            BodyPartBase* bodyPart = m_bodyParts[i];
            bodyPart->addPolylinePoints_sprite(poly);
        }

        // second pass (left side)
        for ( i32 i = bodyPartCount - 1; i >= 0; i--)
        {
            BodyPartBase* bodyPart = m_bodyParts[i];
            bodyPart->addOtherPolylinePoints_sprite(poly);
        }

        // close the polyline
        poly.addPoint(poly.getPosAt(0));


        m_enclosingPolyline.activate();
        m_enclosingPolyline.update(_dt);
    }

    void Ray_SnakeAIComponent::Draw()
    {
        ITF_ASSERT(m_renderMode == RenderMode_Sprite);

        if (getTemplate()->getBodyPartsZSpacing() > 0)
        {
            // draw head to tail

            for (u32 i = 0; i < m_bodyParts.size(); i++)
            {
                BodyPartBase* bodyPart = m_bodyParts[i];
                bodyPart->draw_sprite(m_atlas);
            }

            for (u32 i = 0; i < m_deadBodyParts.size(); i++)
            {
                BodyPartBase* bodyPart = m_deadBodyParts[i];
                bodyPart->draw_sprite(m_atlas);
            }
        }
        else if (!getTemplate()->getDrawHeadBelow())
        {
            // draw tail to head

            for (i32 i = m_bodyParts.size()-1; i >= 0; i--)
            {
                BodyPartBase* bodyPart = m_bodyParts[i];
                bodyPart->draw_sprite(m_atlas);
            }

            for (i32 i = m_deadBodyParts.size()-1; i >= 0; i--)
            {
                BodyPartBase* bodyPart = m_deadBodyParts[i];
                bodyPart->draw_sprite(m_atlas);
            }
        }
        else
        {
            // draw tail to head, expect the head goes below the first body part

            ITF_ASSERT(m_bodyParts.size() >= 2);
            if (m_bodyParts.size() < 2)
            {
                return;
            }

            for (i32 i = m_bodyParts.size()-1; i >= 2; i--)
            {
                BodyPartBase* bodyPart = m_bodyParts[i];
                bodyPart->draw_sprite(m_atlas);
            }

            m_bodyParts[0]->draw_sprite(m_atlas);
            m_bodyParts[1]->draw_sprite(m_atlas);
        }

        m_atlas.draw(m_actor->getDepth());
    }







    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                            AI STUFF
    //  
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    void Ray_SnakeAIComponent::updateAI()
    {
        // Check actor destruction
        //--------------------------
        const u32 bodyPartsCount = m_bodyParts.size();

        if ( !m_isDying && ( bodyPartsCount < 2 ) )
        {
            bbool startDeath = bfalse;

            switch( getTemplate()->getDestructibleMode() )
            {
            case Ray_SnakeAIComponent_Template::TailOnly :
            case Ray_SnakeAIComponent_Template::PartByPart :
                {
                    if ( bodyPartsCount == 0 )
                        startDeath = btrue;
                    break;
                }
            case Ray_SnakeAIComponent_Template::FromTail :
                {
                    if ( bodyPartsCount == 1 )
                        startDeath = btrue;
                    break;
                }
            }

            if ( startDeath )
            {
                bbool fade = btrue;

                // has animation ?
                //
                if ( m_animComponent && getTemplate()->getDeathAnimation().isValid() )
                {
                    m_animComponent->setAnim( getTemplate()->getDeathAnimation() );
                    fade = bfalse;
                }
                // Start death bhv ?
                //
                else if ( getTemplate()->getDeathBhv().isValid() )
                {
                    Ray_AIComponent * aiComp = m_actor->GetComponent<Ray_AIComponent>();
                    if ( aiComp != NULL )
                    {
                        aiComp->setHealth( 0 );
                        aiComp->setRewardReceiver( m_lastStimSender );

                        EventChangeBehaviourQuery changeBhv;
                        changeBhv.setWantedBehaviourName( getTemplate()->getDeathBhv() );
                        m_actor->onEvent( &changeBhv );

                        fade = !changeBhv.isChangeDone();
                    }
                }
                // nothing, fade out
                //
                if ( fade )
                {
                    EventShow hideEvt( 0.f, 0.8f );
                    hideEvt.setPauseOnEnd( btrue );
                    m_actor->onEvent( &hideEvt );
                }
                m_isDying = btrue;
                m_enclosingPolyline.deactivate();
            }
        }
        else if ( m_isDying )
        {
            if ( getTemplate()->getDeathAnimation().isValid() )
            {
                if ( m_animComponent->isSubAnimFinished() )
                {
                    m_actor->disable();
                }            
            }
        }
    }

    bbool Ray_SnakeAIComponent::useCloseRangeAttack() const
    {
        return m_animComponent && getTemplate()->getAttackAnimation().isValid();
    }

    // play attack anim if target is close
    void Ray_SnakeAIComponent::updateCloseRangeAttack( const f32 _targetSqrDist )
    {
        ITF_ASSERT(_targetSqrDist);

        if (!useCloseRangeAttack())
        {
            return;
        }


        if (!m_isAttacking)
        {
            if ( _targetSqrDist < sqr(getTemplate()->getAttackMinDistance()) )
            {
                m_isAttacking = btrue;
                m_animComponent->setAnim( getTemplate()->getAttackAnimation() );
            }
        }
        else
        {
            if (m_animComponent->isSubAnimFinished())
            {
                if ( _targetSqrDist > sqr(getTemplate()->getAttackMaxDistance()) )
                {
                    m_isAttacking = bfalse;
                    m_animComponent->setAnim( m_animComponent->getDefaultAnimation() );
                }
                else
                {
                    m_animComponent->setAnim( getTemplate()->getAttackAnimation() );
                }
            }
        }
    }

    void Ray_SnakeAIComponent::processHitStim( HitStim * _hitStim )
    {
        if ( _hitStim->getSender() == m_actor->getRef() )
        {
            return;
        }


        bbool hasBeenHit = bfalse; 
        const u32 bodyPartCount = m_bodyParts.size();

        // broadcast to all parts
        //
        if ( getTemplate()->getBroadcastHitToPart() )
        {
            for ( u32 i = 0; i < bodyPartCount; ++i )
            {
                hasBeenHit = static_cast<BodyPart*>(m_bodyParts[i])->processHitStim( _hitStim ) || hasBeenHit;
            }
        }

        if ( getTemplate()->getDestructibleMode() != Ray_SnakeAIComponent_Template::None )
        {
            // find the hit body part
            //--------------------------
            const PhysContactsContainer& contactList = _hitStim->getContacts();
            

            const SCollidableContact& contact = contactList[0];
            u32 touchedPartIdx = U32_INVALID;
            bbool isHead = bfalse;
            bbool isLeft = bfalse;
            bbool isTail = bfalse;
            
            if ( contact.m_edgeIndex != U32_INVALID )
            {
                // on head
                if ( contact.m_edgeIndex < m_headEdgeCount )
                {
                    isHead = btrue;
                }
                // on body
                else
                {
                    for ( u32 i = 0; i < bodyPartCount; ++i )
                    {
                        BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[i]);
                        if ( ( contact.m_edgeIndex >= bodyPart->m_rightFirstEdgeIdx ) && ( contact.m_edgeIndex <= bodyPart->m_rightLastEdgeIdx ) )
                        {
                            touchedPartIdx = i;
                            break;
                        }
                        else if ( ( contact.m_edgeIndex >= bodyPart->m_leftFirstEdgeIdx ) && ( contact.m_edgeIndex <= bodyPart->m_leftLastEdgeIdx ) )
                        {
                            touchedPartIdx = i;
                            isLeft = btrue;
                            break;
                        }
                    }

                    if ( getTemplate()->getConsiderHeadAsFirstBodyPart() && ( touchedPartIdx == 0 ) )
                    {
                        isHead = btrue;
                    }
                    else if ( touchedPartIdx == bodyPartCount - 1  )
                    {
                        isTail = btrue;
                    }
                }
            }
            

            // debug info
            /*if ( touchedPartIdx != U32_INVALID )
            {
                Actor * partActor = AIUtils::getActor( m_bodyParts[touchedPartIdx]->m_actorRef );
                if ( partActor != NULL )
                {
                    DebugDraw::circle( partActor->get2DPos(), 0.0f, 0.5f, isLeft ? Color::red() : Color::green(), 1.0f );
                }
            }
            else if ( isHead )
            {
                DebugDraw::circle( m_actor->get2DPos(), 0.0f, 0.5f, Color::blue(), 1.0f );
            }*/

            // concider the head has the first body part
            //
            if ( getTemplate()->getConsiderHeadAsFirstBodyPart() && isHead && ( bodyPartCount > 0 ) )
            {
                touchedPartIdx = 0;
            }

            //  check destructible mode 
            //
            if ( touchedPartIdx != U32_INVALID )
            {
                 switch( getTemplate()->getDestructibleMode() )
                {
                    //**************
                    // FROM TAIL 
                    //**************
                    case Ray_SnakeAIComponent_Template::FromTail :
                    {
                        i32 partToRemoveIdx = bodyPartCount - 2;
                        // keep the head and the tail
                        if ( partToRemoveIdx >= 0 )
                        {
                            hasBeenHit = processBodyPartHitStim( _hitStim, partToRemoveIdx, isLeft ) || hasBeenHit;
                        }
                        break;
                    }                    
                    //**************
                    // PART BY PART
                    //**************
                    case Ray_SnakeAIComponent_Template::PartByPart :
                    {
                        // in general case, the head is destroyed at the end
                        if ( isHead && ( bodyPartCount > 2 ) )
                        {
                            touchedPartIdx = 1;
                        }
                        else if ( isTail && ( bodyPartCount > 2 ) )
                        {
                            touchedPartIdx = bodyPartCount - 2;
                        }

                        hasBeenHit = processBodyPartHitStim( _hitStim, touchedPartIdx, isLeft ) || hasBeenHit;

                        // head and tail leaving, send hit to them
                        if ( getTemplate()->getConsiderHeadAsFirstBodyPart() && ( bodyPartCount == 2 ) )
                        {
                            hasBeenHit = processBodyPartHitStim( _hitStim, m_bodyParts.size() - 1, isLeft ) || hasBeenHit;
                        }
                        break;
                    }
                    //**************
                    // TAIL ONLY
                    //**************
                    case Ray_SnakeAIComponent_Template::TailOnly :
                    {
                        // touch the tail ?
                        if ( ( bodyPartCount > 0 ) && ( touchedPartIdx == bodyPartCount - 1 ) )
                        {
                            hasBeenHit = processBodyPartHitStim( _hitStim, touchedPartIdx, isLeft ) || hasBeenHit;
                        }
                        break;
                    }  
                }
            }
        }

        // Send successful hit event
        if ( hasBeenHit )
        {
            if ( m_fxController != NULL )
            {
                u32 fxHandle = m_fxController->playFeedback( _hitStim->getSender(), _hitStim->getActionFromHit(), m_actor->getRef() );
                m_fxController->setFXPosFromHandle( fxHandle, _hitStim->getFxPos() );
            }

            EventHitSuccessful successEvt;
            successEvt.setSender( m_actor->getRef() );
            Actor * sender = AIUtils::getActor( _hitStim->getSender() );
            if ( sender != NULL )
            {
                sender->onEvent( &successEvt );
            }

            m_lastStimSender = _hitStim->getOriginalSender();
            if ( !m_lastStimSender.isValid() )
            {
                m_lastStimSender = _hitStim->getSender();
            }
        }
    }

    bbool Ray_SnakeAIComponent::processBodyPartHitStim( HitStim* _hitStim, const u32& _partIdx, bbool _leftSide )
    {
        ITF_ASSERT( _hitStim != NULL );
        ITF_ASSERT( _partIdx < m_bodyParts.size() );
        if ( ( _hitStim == NULL ) || _partIdx >= m_bodyParts.size() )
        {
            return bfalse;
        }
        BodyPart* bodyPart = static_cast<BodyPart*>(m_bodyParts[_partIdx]);
        bbool hashit = bodyPart->receiveHit( _hitStim, _leftSide );

        if ( hashit )
        {
            // destroy the part
            if ( bodyPart->isDead() )
            {
                destroyBodyPart( bodyPart, _partIdx );
            }        
        }
        return hashit;
    }





    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                            NODE STUFF
    //  
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************


    void Ray_SnakeAIComponent::updateNodes( const f32 _dt )
    {
        // current gives us the order type, next gives us the destination
        Ray_BossMorayNodeComponent* prev = getNode(m_prevNode);
        Ray_BossMorayNodeComponent* current = getNode(m_currentNode);
        Ray_BossMorayNodeComponent* next = getNode(m_nextNode);

        if (!current || !next)
        {
            return;
        }


        m_currentNodeDist += m_currentSpeed * m_currentSpeedMultiplier * _dt;

        Vec3d newPos;
        while ( current->move(prev ? prev->GetActor() : NULL, next->GetActor(), m_currentNodeDist, newPos) )
        {
            setCurrentNode(current, next);
            prev = current;
            current = next;
            next = getNode(m_nextNode);

            if (!next)
            {
                return;
            }
        }

        newPos.m_z += m_actor->getLocalInitialZ();
        m_actor->setPos(newPos);

#ifndef ITF_SUPPORT_EDITOR
        //debugNodes();
#endif // ITF_SUPPORT_EDITOR
    }

    void Ray_SnakeAIComponent::processActivateEvent( Event* _nodeEvent )
    {
        // this event is sent by a link, that's supposed to link to a node: get it

        Actor* sender = static_cast<ActorRef>(_nodeEvent->getSender()).getActor();
        LinkComponent* link = sender ? sender->GetComponent<LinkComponent>() : NULL;
        ITF_ASSERT(link);
        if (link)
        {
            AIUtils::LinkIterator iterator(link);
            while ( Actor* actorChild = iterator.getNextRelativeActor() )
            {
                if ( Ray_BossMorayNodeComponent* node = actorChild->GetComponent<Ray_BossMorayNodeComponent>() )
                {
                    setInitialNode(node);
                    return;
                }
            }
        }

        ITF_WARNING(m_actor, 0, "could not get the initial node from the Ray_EventSetInitialNode");
    }

    void Ray_SnakeAIComponent::setInitialNode( Ray_BossMorayNodeComponent* _node )
    {
        if (m_activeMode == ActiveMode_AlwaysActiveDeactivateOnEnd)
        {
            AIUtils::setAlwaysActive(m_actor, btrue);
        }

        resetMovement();

        m_prevNodes.clear();

        setCurrentNode(NULL, _node, btrue);
    }

    void Ray_SnakeAIComponent::setCurrentNode( Ray_BossMorayNodeComponent* _prevNode, Ray_BossMorayNodeComponent* _node, bbool _isInitialNode /*= bfalse*/ )
    {
        m_prevNode.invalidate();
        m_currentNode.invalidate();
        m_nextNode.invalidate();


        if ( _prevNode )
        {
            m_prevNode = _prevNode->GetActor()->getRef();

            if (getTemplate()->getPrevNodeCount())
            {
                if (m_prevNodes.size() == m_prevNodes.capacity())
                {
                    m_prevNodes.eraseKeepOrder(0);
                }
                m_prevNodes.push_back(_prevNode->GetActor()->getRef());
            }
        }

        m_currentNode = _node->GetActor()->getRef();


        NodeData nextNodeData = chooseNextNode(_prevNode, _node);

        if (nextNodeData.m_node)
        {
            m_nextNode = nextNodeData.m_node->GetActor()->getRef();
        }

        if (_isInitialNode)
        {
            if (nextNodeData.m_node)
            {
                Vec2d direction = (nextNodeData.m_node->GetActor()->get2DPos() - _node->GetActor()->get2DPos()).normalize();
                initTrajectory( _node->GetActor()->get2DPos(), direction );
            }
            else
            {
                initTrajectory( _node->GetActor()->get2DPos(), Vec2d::Up );
            }
        }

        startNode(_node, nextNodeData);
    }

    void Ray_SnakeAIComponent::startNode( Ray_BossMorayNodeComponent* _node, const NodeData& _nextNodeData )
    {
        _node->onNodeReached(m_actor);

        if (_nextNodeData.m_linkData)
        {
            bbool flip;
            if ( _nextNodeData.m_linkData->getTagValue(ITF_GET_STRINGID_CRC(Flip,534660332), flip) )
            {
                m_actor->setFlipped(flip);
            }
        }

        if (!_nextNodeData.m_node)
        {
            onEndReached();
        }
    }

    void Ray_SnakeAIComponent::onEndReached()
    {
        if (m_activeMode == ActiveMode_AlwaysActiveDeactivateOnEnd)
        {
            AIUtils::setAlwaysActive(m_actor, bfalse);
        }
    }

    void Ray_SnakeAIComponent::processNodeReached( Ray_EventBossMorayNodeReached* _nodeReached )
    {
        m_targetSpeed = _nodeReached->getSpeed();

        // nodes can override the acceleration, otherwise go back to the default one
        m_acceleration = _nodeReached->getAcceleration() ? _nodeReached->getAcceleration() : getTemplate()->getAcceleration();

        m_speedMultiplierDisabled = _nodeReached->getDisableSpeedMultiplier();
    }

    Ray_BossMorayNodeComponent* Ray_SnakeAIComponent::getNode( ActorRef _nodeActor )
    {
        Actor* actor = _nodeActor.getActor();
        return actor ? actor->GetComponent<Ray_BossMorayNodeComponent>() : NULL;
    }



    NodeData Ray_SnakeAIComponent::chooseNextNode( Ray_BossMorayNodeComponent* _prevNode, Ray_BossMorayNodeComponent* _currentNode )
    {
        Ray_BossMorayNodeComponent::NodeDataList childNodes;
        _currentNode->getChildNodes(childNodes);

        // never choose the prevNode as a nextNode
        if (_prevNode)
        {
            for (u32 i = 0; i < childNodes.size(); i++)
            {
                if (childNodes[i].m_node == _prevNode)
                {
                    childNodes.eraseNoOrder(i);
                    break;
                }
            }
        }

        if (!childNodes.size())
        {
            return NodeData();
        }


        if (getTemplate()->getPrevNodeCount())
        {
            Ray_BossMorayNodeComponent::NodeDataList copy = childNodes;

            // remove the nodes we've already been to
            for (u32 p = 0; p < m_prevNodes.size(); p++)
            {
                if ( Ray_BossMorayNodeComponent* prevNode = getNode(m_prevNodes[p]) )
                {
                    for (u32 i = 0; i < childNodes.size(); i++)
                    {
                        if (childNodes[i].m_node == prevNode)
                        {
                            childNodes.eraseNoOrder(i);
                            break;
                        }
                    }
                }
            }

            if (!childNodes.size())
            {
                // no available nodes (already been everywhere): just pick the best one anyway
                childNodes = copy;
            }
        }


        u32 nextNodeIndex = chooseNextNode_usingSelector(_prevNode, _currentNode, childNodes);
        if ( nextNodeIndex == U32_INVALID )
        {
            nextNodeIndex = chooseNextNode_targetPlayer(_prevNode, _currentNode, childNodes);
        }

        if ( nextNodeIndex != U32_INVALID )
        {
            return childNodes[nextNodeIndex];   // returns a copy
        }

        return NodeData();
    }

    // choose the node whose angle to the player is lowest
    u32 Ray_SnakeAIComponent::chooseNextNode_targetPlayer( Ray_BossMorayNodeComponent* _prevNode, Ray_BossMorayNodeComponent* _currentNode, const Ray_BossMorayNodeComponent::NodeDataList& _childNodes )
    {
        u32 nextNodeIndex = 0;   // fallback

        if ( Actor* target = AIUtils::getLeadLivePlayer(m_actor) )
        {
            const Vec2d& nodePos = m_actor->get2DPos();
            const Vec2d& targetPos = target->get2DPos();
            Vec2d nodeToTarget = (targetPos - nodePos).normalize();

            f32 bestDot(-1.f);

            for (u32 i = 0; i < _childNodes.size(); i++)
            {
                const NodeData& childNode = _childNodes[i];

                if (childNode.m_linkData->hasTag(s_Disabled))
                {
                    continue;
                }

                const Actor* childActor = childNode.m_node->GetActor();

                const Vec2d trajectoryDir = (childActor->get2DPos() - nodePos).normalize();
                const f32 dot = trajectoryDir.dot( nodeToTarget );

                if (dot > bestDot)
                {
                    bestDot = dot;
                    nextNodeIndex = i;
                }
            }
        }

        return nextNodeIndex;
    }

    u32 Ray_SnakeAIComponent::chooseNextNode_usingSelector( Ray_BossMorayNodeComponent* _prevNode, Ray_BossMorayNodeComponent* _currentNode, const Ray_BossMorayNodeComponent::NodeDataList& _childNodes )
    {
        if ( Actor* nodeSelector = m_nodeSelector.getActor() )
        {
            if ( LinkComponent* selectorLinkComponent = nodeSelector->GetComponent<LinkComponent>() )
            {
                Ray_BossMorayNodeComponent::NodeList selectorNodes;
                Ray_BossMorayNodeComponent::getChildNodes(selectorLinkComponent, selectorNodes);

                for (u32 i = 0; i < selectorNodes.size(); i++)
                {
                    Ray_BossMorayNodeComponent* selectorNode = selectorNodes[i];

                    for (u32 j = 0; j < _childNodes.size(); j++)
                    {
                        const NodeData& childNode = _childNodes[j];
                        if (childNode.m_node == selectorNode)
                        {
                            m_nodeSelector.invalidate();
                            return j;
                        }
                    }
                }
            }
        }

        return U32_INVALID;
    }

    // choose the next node randomly
    /*u32 Ray_SnakeAIComponent::chooseNextNode_random( Ray_BossMorayNodeComponent* _node )
    {
        Ray_BossMorayNodeComponent::NodeList children;
        _node->getChildNodes(children);

        if ( children.size() )
        {
            u32 i = Seeder::getSharedSeeder().GetU32(0, children.size() - 1);
            return i;
        }

        return U32_INVALID;
    }*/



    bbool Ray_SnakeAIComponent::useUpdateSpeedMultiplier() const
    {
        return getTemplate()->getUseSpeedMultiplier() && !m_speedMultiplierDisabled;
    }

    // adjust speed multiplier according to distance to target
    void Ray_SnakeAIComponent::updateSpeedMultiplier( const f32 _targetSqrDist )
    {
        ITF_ASSERT(_targetSqrDist);

        if ( !useUpdateSpeedMultiplier() )
        {
            return;
        }


        m_targetSpeedMultiplier = 1.f;

        if ( _targetSqrDist < sqr(getTemplate()->getSpeedMultiplierMinDistance()) )
        {
            // player is too close, slow down
            m_targetSpeedMultiplier = getTemplate()->getSpeedMultiplierMinValue();
        }
        else if ( _targetSqrDist > sqr(getTemplate()->getSpeedMultiplierMaxDistance()) )
        {
            // player is too far, accelerate
            m_targetSpeedMultiplier = getTemplate()->getSpeedMultiplierMaxValue();
        }
    }

    // speed is given by the nodes, multiplier is given by the AI
    void Ray_SnakeAIComponent::blendSpeedAndMultiplier( const f32 _dt )
    {
        if (m_currentSpeed < m_targetSpeed)
        {
            m_currentSpeed += m_acceleration * _dt;
            m_currentSpeed = Min(m_currentSpeed, m_targetSpeed);
        }
        else if (m_currentSpeed > m_targetSpeed)
        {
            m_currentSpeed -= m_acceleration * _dt;
            m_currentSpeed = Max(m_currentSpeed, m_targetSpeed);
        }


        if (m_currentSpeedMultiplier < m_targetSpeedMultiplier)
        {
            m_currentSpeedMultiplier += getTemplate()->getSpeedMultiplierAcceleration() * _dt;
            m_currentSpeedMultiplier = Min(m_currentSpeedMultiplier, m_targetSpeedMultiplier);
        }
        else if (m_currentSpeedMultiplier > m_targetSpeedMultiplier)
        {
            m_currentSpeedMultiplier -= getTemplate()->getSpeedMultiplierDeceleration() * _dt;
            m_currentSpeedMultiplier = Max(m_currentSpeedMultiplier, m_targetSpeedMultiplier);
        }
    }





#ifdef ITF_SUPPORT_EDITOR
    void Ray_SnakeAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        m_enclosingPolyline.drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
        {
            return;
        }

        debugNodes();
    }

    void Ray_SnakeAIComponent::debugNodes() const
    {
        DebugDraw::text("Current speed  %.2f * %.2f",
            m_currentSpeed, m_currentSpeedMultiplier);
        DebugDraw::text("Target speed   %.2f * %.2f",
            m_targetSpeed, m_targetSpeedMultiplier);
    }


    void Ray_SnakeAIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        if (m_trajectory.isInitialized())
        {
            m_trajectory.update(m_actor->get2DPos());
            updateBodyParts(0);
            forceUpdateBodyParts();
        }
    }

    void Ray_SnakeAIComponent::onScaleChanged()
    {
        initScale();
        initTrajectory(m_actor->get2DPos(), Vec2d::Up.Rotate(m_actor->getAngle()));

        m_trajectory.update(m_actor->get2DPos());
        updateBodyParts(0);
        forceUpdateBodyParts();
    }

    void Ray_SnakeAIComponent::forceUpdateBodyParts()
    {
        for (u32 i = 0; i < m_bodyParts.size(); i++)
        {
            m_bodyParts[i]->onEditorMove();
        }
    }
#endif // ITF_SUPPORT_EDITOR









    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                                          BODY PARTS FACTORIES
    //  
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************
    void BodyPartFactory_Template::init()
    {
        RegisterObject<BodyPartSprite_Template>(ITF_GET_STRINGID_CRC(BodyPartSprite_Template,4078730029));
        RegisterObject<BodyPart_Template>(ITF_GET_STRINGID_CRC(BodyPart_Template,2951019815));
        RegisterObject<DestructibleBodyPart_Template>(ITF_GET_STRINGID_CRC(DestructibleBodyPart_Template,1660554312));
        RegisterObject<Ray_ShooterGardianMorayBodyPart_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterGardianMorayBodyPart_Template,822361727));
        RegisterObject<Ray_BossMorayBodyPart_Template>(ITF_GET_STRINGID_CRC(Ray_BossMorayBodyPart_Template,4293489626));
    }

    void BodyPartFactory::init()
    {
        RegisterObject<BodyPartSprite>(ITF_GET_STRINGID_CRC(BodyPartSprite,802355147));
        RegisterObject<BodyPart>(ITF_GET_STRINGID_CRC(BodyPart,3043470114));
        RegisterObject<DestructibleBodyPart>(ITF_GET_STRINGID_CRC(DestructibleBodyPart,81228598));
        RegisterObject<Ray_ShooterGardianMorayBodyPart>(ITF_GET_STRINGID_CRC(Ray_ShooterGardianMorayBodyPart,2915376064));
        RegisterObject<Ray_BossMorayBodyPart>(ITF_GET_STRINGID_CRC(Ray_BossMorayBodyPart,4134625562));
    }



    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                                              BODY PARTS
    //  
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    IMPLEMENT_OBJECT_RTTI(BodyPartBase)

    IMPLEMENT_OBJECT_RTTI(BodyPartBase_Template)

    BEGIN_SERIALIZATION_ABSTRACT(BodyPartBase_Template)
        SERIALIZE_MEMBER("lengthOnTrajectory", m_lengthOnTrajectory);
    END_SERIALIZATION()


    //-----------------------------------------------------------------------------------------------------------------------------
    //  Ray_SnakeAIComponent_Template
    //-----------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_SnakeAIComponent_Template)

    BEGIN_SERIALIZATION(Ray_SnakeAIComponent_Template)
        SERIALIZE_MEMBER("headPolyline", m_headPolyline);
        SERIALIZE_CONTAINER_WITH_FACTORY("bodyParts", m_bodyParts, BodyPartFactory_Template::get() );
        SERIALIZE_MEMBER("bodyPartsZSpacing", m_bodyPartsZSpacing);
        SERIALIZE_MEMBER("drawHeadBelow", m_drawHeadBelow);
        SERIALIZE_MEMBER("gameMaterial", m_gameMaterial);
        SERIALIZE_MEMBER("atlas", m_atlasPath);
        SERIALIZE_MEMBER("prevNodeCount", m_prevNodeCount);
        SERIALIZE_MEMBER("sampleLength", m_sampleLength);
        SERIALIZE_MEMBER("acceleration", m_acceleration);
        SERIALIZE_MEMBER("targetEvaluationOffset", m_targetEvaluationOffset);
        SERIALIZE_MEMBER("speedMultiplierMinDistance", m_speedMultiplierMinDistance);
        SERIALIZE_MEMBER("speedMultiplierMaxDistance", m_speedMultiplierMaxDistance);
        SERIALIZE_MEMBER("speedMultiplierMinValue", m_speedMultiplierMinValue);
        SERIALIZE_MEMBER("speedMultiplierMaxValue", m_speedMultiplierMaxValue);
        SERIALIZE_MEMBER("speedMultiplierAcceleration", m_speedMultiplierAcceleration);
        SERIALIZE_MEMBER("speedMultiplierDeceleration", m_speedMultiplierDeceleration);
        SERIALIZE_MEMBER("conciderHeadAsFirstBodyPart", m_considerHeadAsFirstBodyPart );
        SERIALIZE_ENUM_BEGIN("destructibleMode",m_destructibleMode);
            SERIALIZE_ENUM_VAR(None);
            SERIALIZE_ENUM_VAR(FromTail);
            SERIALIZE_ENUM_VAR(PartByPart);
            SERIALIZE_ENUM_VAR(TailOnly);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("broadcastHitToPart", m_broadcastHitToPart );
        SERIALIZE_MEMBER("broadcastEventToPart", m_broadcastEventsToPart );

        SERIALIZE_MEMBER("attackMinDistance", m_attackMinDistance);
        SERIALIZE_MEMBER("attackMaxDistance", m_attackMaxDistance);
        SERIALIZE_MEMBER("deathAnimation", m_deathAnimation );
        SERIALIZE_MEMBER("attackAnimation", m_attackAnimation );
        SERIALIZE_MEMBER("deathBhv", m_deathBhv );
    END_SERIALIZATION()
   
    Ray_SnakeAIComponent_Template::~Ray_SnakeAIComponent_Template()
    {
        for (u32 i = 0; i < m_bodyParts.size(); ++i)
        {
            SF_DEL(m_bodyParts[i]);
        }
    }

    void Ray_SnakeAIComponent_Template::onTemplateLoaded()
    {
        Super::onTemplateLoaded();

        m_useSpeedMultiplier = (m_speedMultiplierMinValue != m_speedMultiplierMaxValue);

        // we use a polyline as soon as at least one part wants it
        for (u32 i = 0; i < m_bodyParts.size(); ++i)
        {
            const BodyPartBase_Template* tplBodyPart = m_bodyParts[i];

            if ( !tplBodyPart )
            {
                ITF_ASSERT(0);
                break;
            }

            if ( tplBodyPart->usePolyline() )
            {
                m_usePolyline = btrue;
                break;
            }
        }
    }

};
