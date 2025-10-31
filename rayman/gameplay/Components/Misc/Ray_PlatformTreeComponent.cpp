#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLATFORMTREECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_PlatformTreeComponent.h"
#endif //_ITF_RAY_PLATFORMTREECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_LINKCURVECOMPONENT_H_
#include "gameplay/Components/Misc/LinkCurveComponent.h"
#endif //_ITF_LINKCURVECOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_PLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_PlatformAIComponent.h"
#endif //_ITF_RAY_PLATFORMAICOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_CURVE_H_
#include "engine/gameplay/Curve.h"
#endif //_ITF_CURVE_H_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    #define s_ChildExcludeTag ITF_GET_STRINGID_CRC(NoTree,3919588772)

    IMPLEMENT_OBJECT_RTTI(Ray_PlatformTreeComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_PlatformTreeComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("startActivated", m_startActivated);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_PlatformTreeComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "Ray_PlatformTreeComponent requires a LinkComponent");
        VALIDATE_COMPONENT_PARAM("", m_linkCurveComponent, "Ray_PlatformTreeComponent requires a LinkCurveComponent");
        VALIDATE_COMPONENT_PARAM("childMoveSpeed", getTemplate()->getChildMoveSpeed() > 0, "must be positive!");
        VALIDATE_COMPONENT_PARAM("", getTemplate()->getChildOpenDist() > getTemplate()->getChildAppearDist(), "childOpenDist needs to be greater than childAppearDist");
        VALIDATE_COMPONENT_PARAM("", getTemplate()->getChildCloseDist() > getTemplate()->getChildDisappearDist(), "childCloseDist needs to be greater than childDisappearDist");
    END_VALIDATE_COMPONENT()


    Ray_PlatformTreeComponent::Ray_PlatformTreeComponent()
        : Super()
        , m_linkComponent(NULL)
        , m_linkCurveComponent(NULL)
        , m_animComponent(NULL)
        , m_animatedComponent(NULL)
        , m_playerDetector(NULL)
        , m_startActivated(btrue)
        , m_startDelayTimer(0.f)

        , m_state(State_Invalid)
        , m_timer(0.f)
        , m_playersInsideCount(0)
    {
    }

    Ray_PlatformTreeComponent::~Ray_PlatformTreeComponent()
    {
    }

    void Ray_PlatformTreeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        if (!getTemplate()->getIgnoreEventTrigger())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        }
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();   // mandatory, validated
        m_linkCurveComponent = m_actor->GetComponent<LinkCurveComponent>();   // mandatory, validated
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();   // not mandatory
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_playerDetector = m_actor->GetComponent<PlayerDetectorComponent>(); // optional

        m_softCollision.init(&getTemplate()->getSoftCollision(), this);
    }

    void Ray_PlatformTreeComponent::onSceneActive()
    {
        Super::onSceneActive();

        applyStateForced(m_startActivated);
    }

    void Ray_PlatformTreeComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        if (m_state == State_Invalid)   // we may have been retriggered between onActorLoaded and onCheckpointLoaded...
        {
            applyStateForced(m_startActivated);
        }
    }

    void Ray_PlatformTreeComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if (m_state == State_Invalid)   // this is mostly for editor situations: on actor creation, hot-reload, etc.
        {
            applyStateForced(m_startActivated);
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void Ray_PlatformTreeComponent::editor_setActivated( bbool _value )
    {
        if (_value != m_startActivated)
        {
            m_startActivated = _value;
            m_actor->replaceReinitDataByCurrent();
        }

        applyStateForced(m_startActivated);
    }
#endif // ITF_SUPPORT_EDITOR


    // if the child wasn't already present, add and init it, then return it
    Ray_PlatformTreeComponent::ChildData& Ray_PlatformTreeComponent::getChildData( Actor* child )
    {
        ChildDataMap::iterator it = m_childDataMap.find(child->getRef());
        if ( it != m_childDataMap.end() )
        {
            return it->second;
        }


        ChildData& childData = m_childDataMap[child->getRef()];

        childData.m_startDelay = Seeder::getSharedSeeder().GetFloat(0.f, getTemplate()->getMaxStartDelay());

        //const Vec2d p0 = m_linkCurveComponent->getAttachPosition();
        const Vec2d p0 = m_actor->get2DPos();
        Vec2d targetPos = child->getWorldInitialPos().truncateTo2D();
        f32 length = (targetPos - p0).norm();
        childData.m_duration = length / getTemplate()->getChildMoveSpeed();

        childData.m_time = isActivated() ? 1.f : 0.f;

        return childData;
    }

    void Ray_PlatformTreeComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventGeneric* genericEvent = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            if (genericEvent->getId() == ITF_GET_STRINGID_CRC(Open,812432275))
            {
                tryOpen();
            }
            else if (genericEvent->getId() == ITF_GET_STRINGID_CRC(Close,3883599349))
            {
                tryClose();
            }
            else if (genericEvent->getId() == ITF_GET_STRINGID_CRC(OpenClose,883616157))
            {
                // toggle
                tryOpen();
                tryClose();
            }
        }
        else if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if (trigger->getActivated())
            {
                if (!trigger->getFromRetriggerOnCheckpoint())
                {
                    // toggle
                    tryOpen();
                    tryClose();
                }
                else
                {
                    // force toggle
                    applyStateForced(!m_startActivated);
                }
            }
        }
        else
        {
            m_softCollision.onEvent(_event);
        }
    }


    void Ray_PlatformTreeComponent::tryOpen()
    {
        ITF_ASSERT(m_state != State_Invalid);
        if (m_state == State_Closed ||
            m_state == State_Closed_Wiggling ||
            m_state == State_Invalid)
        {
            changeState(State_Opening_In);
        }
    }

    void Ray_PlatformTreeComponent::tryClose()
    {
        ITF_ASSERT(m_state != State_Invalid);
        if (m_state == State_Open ||
            m_state == State_Open_Wiggling ||
            m_state == State_Invalid)
        {
            changeState(State_Closing_In);
        }
    }


    void Ray_PlatformTreeComponent::applyStateForced( const bbool _open )
    {
        if (_open)
        {
            forceOpen();
        }
        else
        {
            forceClose();
        }
    }

    void Ray_PlatformTreeComponent::forceOpen()
    {
        forceChangeState(State_Open);

        AIUtils::LinkIterator iterator(m_linkComponent);
        while ( Actor* child = iterator.getNextRelativeActorTagFilter(s_ChildExcludeTag, btrue) )
        {
            ChildData& childData = getChildData(child);
            childData.m_time = 1.f;

            child->setPos(child->getWorldInitialPos());

            if ( Ray_PlatformAIComponent* childAI = child->GetComponent<Ray_PlatformAIComponent>() )
            {
                childAI->setState(btrue, 1.f, btrue, getTemplate());
            }
            else
            {
                child->enable();
            }
        }
    }

    void Ray_PlatformTreeComponent::forceClose()
    {
        forceChangeState(State_Closed);

        AIUtils::LinkIterator iterator(m_linkComponent);
        while ( Actor* child = iterator.getNextRelativeActorTagFilter(s_ChildExcludeTag, btrue) )
        {
            ChildData& childData = getChildData(child);
            childData.m_time = 0.f;

            child->setPos(m_actor->getPos());

            if ( Ray_PlatformAIComponent* childAI = child->GetComponent<Ray_PlatformAIComponent>() )
            {
                childAI->setState(bfalse, 0.f, btrue, getTemplate());
            }
            else
            {
                child->disable();
            }
        }
    }


    void Ray_PlatformTreeComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        //if (m_actor->isSelected())
        //{
        //    forceOpen();
        //    return;
        //}


        updateChildren(_dt);

        updateState(_dt);

        m_softCollision.update();
    }


    void Ray_PlatformTreeComponent::updateChildren( const f32 _dt )
    {
        m_startDelayTimer += _dt;

        const Vec2d p0 = m_linkCurveComponent->getParentAttachPosition();
        const Vec2d n0 = m_linkCurveComponent->getParentAttachNormal();

        AIUtils::LinkIterator iterator(m_linkComponent);
        while ( Actor* child = iterator.getNextRelativeActorTagFilter(s_ChildExcludeTag, btrue) )
        {
            ChildData& childData = getChildData(child);

            if (m_startDelayTimer > childData.m_startDelay)
            {
                updateChild(_dt, childData, child, p0, n0);
            }
        }
    }

    void Ray_PlatformTreeComponent::updateChild( const f32 _dt, ChildData& _childData, Actor* _child, const Vec2d& p0, const Vec2d& n0 )
    {
        const bbool activated = isActivated();

        // time is lerped; distance is smoothed by a curve
        static LogisticParams timeCurve;

        // compute desired dist; this may affect the child's state, who can then decide if he accepts the move or not
        f32 relDT = (activated ? _dt : -_dt) / _childData.m_duration;
        f32 t = Clamp(_childData.m_time + relDT, 0.f, 1.f);

        f32 dist = 0.f;
        if (getTemplate()->getFastMode())
        {
            if (activated)
            {
                // use only the second half of the curve for a fast exit
                dist = timeCurve.getValue(0.5f + t * 0.5f) * 2.f - 1.f;
            }
            else
            {
                // use only the first half of the curve for a fast reentry
                dist = timeCurve.getValue(t * 0.5f) * 2.f;
            }
        }
        else
        {
            dist = timeCurve.getValue(t);
        }


        // depending on distance, set state on childAI OR enable/disable childActor

        Ray_PlatformAIComponent* childAI = _child->GetComponent<Ray_PlatformAIComponent>();
        if (childAI)
        {
            // is no more linked to the tree, update only the timer
            if ( childAI->isFree() )
            {
                _childData.m_time = t;

                 m_linkCurveComponent->setAlpha(_child, 0.0f );
                 return;
            }
           
            childAI->setState(activated, dist, bfalse, getTemplate());
            if (!childAI->canMove(activated))
            {
                return;
            }

            // no need to update fully closed child
            if (t <= 0.f)
            {
                _child->disable();
            }
            else
            {
                _child->enable();
            }
        }
        else
        {
            if (activated)
            {
                if (dist > getTemplate()->getChildAppearDist())
                {
                    _child->enable();
                }
                else
                {
                    _child->disable();
                }
            }
            else
            {
                if (dist < getTemplate()->getChildDisappearDist())
                {
                    _child->disable();
                }
                else
                {
                    _child->enable();
                }
            }
        }

        // move accepted, save the time
        _childData.m_time = t;


        TweenComponent* childTween = _child->GetComponent<TweenComponent>();
        if (childTween)
        {
            // disable tweening while we move to position (using the tweening's targetPos)
            childTween->setApplyCoordinates(t >= 1.f);
        }

        if (0.f < t && t < 1.f)
        {
            // compute bezier parameters

            const Vec2d& p3 = (childTween ? childTween->getTargetPos() : _child->getWorldInitialPos()).truncateTo2D();
            Vec2d n1 = (p0 - p3);
            f32 length = n1.norm();
            n1 *= 1 / length;

            const Vec2d p1 = p0 + n0 * length * 0.5f;
            const Vec2d p2 = p3 + n1 * length * 0.5f;


            // position child along the bezier

            const Vec2d pos = getBezierPosition(p0, p1, p2, p3, dist);


            // orient child along the bezier end

            Vec2d tangent = getBezierTangent(p0, p1, p2, p3, dist);
            f32 angle = tangent.normalize().getAngle() - MTH_PIBY2;

            // compensate for bezierEndAngle-to-finalAngle when reaching the end
            f32 angleCorrectionDist = getTemplate()->getChildAngleCorrectionDist();
            if (dist > angleCorrectionDist)
            {
                f32 remainingT = (dist - angleCorrectionDist) / (1 - angleCorrectionDist);
                f32 finalAngle = _child->getWorldInitialRot();
                f32 angleDelta = getShortestAngleDelta(angle, finalAngle);
                angle += angleDelta * remainingT;
#ifndef ITF_FINAL
                //DebugDraw::circle(_child->getPos(), 1.f, Color::red());
#endif // ITF_FINAL
            }

            if (childAI)
            {
                childAI->setTargetCoordinates(pos, angle);
            }
            else
            {
                _child->set2DPos(pos);
                _child->setAngle(angle);
            }
        }
        // else: child can take over (AI / tweening)

        // fade out the child link
        f32 alpha = dist * getTemplate()->getChildOneOverLinkFadeDist();
        alpha = Min(1.f, alpha);
        m_linkCurveComponent->setAlpha(_child, alpha);
    }




    void Ray_PlatformTreeComponent::changeState( const State _newState )
    {
        ITF_ASSERT(_newState != m_state);
        m_state = _newState;

        switch (m_state)
        {
        case State_Opening_In:
            {
                LOG_BEN("%d - %s - State_Opening_In", CURRENTFRAME, m_actor->getUserFriendly().cStr());

                setCollisionDisabled(btrue);

                if ( !playAnim(getTemplate()->getAnimDisappear()) )
                {
                    changeState(State_Opening_Loop1);
                }
            }
            break;

        case State_Opening_Loop1:
            {
                playAnim(getTemplate()->getAnimHidden());
            }
            break;

        case State_Opening_Loop2:
            {
                m_timer = 0.f;
            }
            break;

        case State_Opening_Out:
            {
                if ( !playAnim(getTemplate()->getAnimAppear()) )
                {
                    changeState(State_Open);
                }
            }
            break;


        case State_Closing_In:
            {
                LOG_BEN("%d - %s - State_Closing_In", CURRENTFRAME, m_actor->getUserFriendly().cStr());

                setCollisionDisabled(btrue);

                if ( !playAnim(getTemplate()->getAnimDisappear()) )
                {
                    changeState(State_Closing_Loop1);
                }
            }
            break;

        case State_Closing_Loop1:
            {
                playAnim(getTemplate()->getAnimHidden());
            }
            break;

        case State_Closing_Loop2:
            {
                m_timer = 0.f;
            }
            break;

        case State_Closing_Out:
            {
                if ( !playAnim(getTemplate()->getAnimAppear()) )
                {
                    changeState(State_Closed);
                }
            }
            break;


        case State_Open:
            {
                playAnim(getTemplate()->getAnimOpened());

                setCollisionDisabled(bfalse);

                if (getTemplate()->getDisableActorOnOpen())
                {
                    m_actor->disable();
                }
            }
            break;

        case State_Closed:
            {
                playAnim(getTemplate()->getAnimClosed());

                setCollisionDisabled(bfalse);
            }
            break;

        case State_Open_Wiggling:
            {
                playAnim(getTemplate()->getAnimWiggleOpened());
            }
            break;

        case State_Closed_Wiggling:
            {
                playAnim(getTemplate()->getAnimWiggleClosed());
            }
            break;
        }
    }

    void Ray_PlatformTreeComponent::updateState( const f32 _dt )
    {
        switch (m_state)
        {
        case State_Opening_In:
            {
                if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
                {
                    changeState(State_Opening_Loop1);
                }
            }
            break;

        case State_Opening_Loop1:
            {
                if ( allChildrenOpen() )
                {
                    changeState(State_Opening_Loop2);
                }
            }
            break;

        case State_Opening_Loop2:
            {
                m_timer += _dt;
                if (m_timer > getTemplate()->getOpeningDelay())
                {
                    changeState(State_Opening_Out);
                }
            }
            break;

        case State_Opening_Out:
            {
                if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
                {
                    changeState(State_Open);
                }
            }
            break;


        case State_Closing_In:
            {
                if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
                {
                    changeState(State_Closing_Loop1);
                }
            }
            break;

        case State_Closing_Loop1:
            {
                if ( allChildrenClosed() )
                {
                    changeState(State_Closing_Loop2);
                }
            }
            break;

        case State_Closing_Loop2:
            {
                m_timer += _dt;
                if (m_timer > getTemplate()->getClosingDelay())
                {
                    changeState(State_Closing_Out);
                }
            }
            break;

        case State_Closing_Out:
            {
                if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
                {
                    changeState(State_Closed);
                }
            }
            break;


        case State_Open:
            {
                detectWiggle();
            }
            break;

        case State_Closed:
            {
                detectWiggle();
            }
            break;

        case State_Open_Wiggling:
            {
                if (AIUtils::isAnimFinished(m_animComponent, m_animatedComponent))
                {
                    changeState(State_Open);
                }
            }
            break;

        case State_Closed_Wiggling:
            {
                if (AIUtils::isAnimFinished(m_animComponent, m_animatedComponent))
                {
                    changeState(State_Closed);
                }
            }
            break;


        default:
            ITF_ASSERT_MSG(0, "unhandled state!");
            break;
        }
    }

    void Ray_PlatformTreeComponent::detectWiggle()
    {
        if ( !getTemplate()->getCanWiggle() ||
             !m_playerDetector ||
             !m_animComponent )
        {
            return;
        }


        // wiggle when player detected
        if ( m_playerDetector->getActorsInside().size() > m_playersInsideCount )
        {
            changeState((m_state == State_Open) ? State_Open_Wiggling : State_Closed_Wiggling);

            // pad rumble
            if (getTemplate()->getPadRumbleWiggle().isValid())
            {
                ActorRef playerRef = m_playerDetector->getActorsInside().back();
                if (Player* player = GAMEMANAGER->getPlayerFromActor(playerRef))
                {
                    PADRUMBLEMANAGER->startRumble(
                        getTemplate()->getPadRumbleWiggle(),
                        player->getIndex()); 
                }
            }
        }

        // remember actors inside count
        m_playersInsideCount = m_playerDetector->getActorsInside().size();
    }


    bbool Ray_PlatformTreeComponent::isActivated() const
    {
        switch (m_state)
        {
        case State_Opening_In:
        case State_Opening_Loop1:
        case State_Opening_Loop2:
        case State_Opening_Out:
        case State_Open:
        case State_Open_Wiggling:
            return btrue;

        case State_Closing_In:
        case State_Closing_Loop1:
        case State_Closing_Loop2:
        case State_Closing_Out:
        case State_Closed:
        case State_Closed_Wiggling:
            return bfalse;
        }

        ITF_ASSERT(0);
        return bfalse;
    }


    #define OPEN_THRESHOLD 0.99f
    #define CLOSE_THRESHOLD 0.01f

    bbool Ray_PlatformTreeComponent::allChildrenOpen() const
    {
        for (ChildDataMap::const_iterator it = m_childDataMap.begin(); it != m_childDataMap.end(); ++it)
        {
            const ChildData& childData = it->second;

            if (childData.m_time < OPEN_THRESHOLD)
            {
                return bfalse;
            }
        }

        return btrue;
    }

    bbool Ray_PlatformTreeComponent::allChildrenClosed() const
    {
        for (ChildDataMap::const_iterator it = m_childDataMap.begin(); it != m_childDataMap.end(); ++it)
        {
            const ChildData& childData = it->second;

            if (childData.m_time > CLOSE_THRESHOLD)
            {
                return bfalse;
            }
        }

        return btrue;
    }


    void Ray_PlatformTreeComponent::setCollisionDisabled( const bbool _disable )
    {
        if (getTemplate()->getDisableCollisionDuringTransition() &&
            m_softCollision.getDisabled() != _disable)
        {
            m_softCollision.setDisabled(_disable);
            AIUtils::disableCollision(m_actor, _disable);
        }
    }

    bbool Ray_PlatformTreeComponent::playAnim( const StringID _anim )
    {
        if (m_animComponent)
        {
            //ITF_ASSERT(_anim.isValid());
            if (_anim.isValid())
            {
                m_animComponent->setAnim(_anim);
                return btrue;
            }
        }

        return bfalse;
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_PlatformTreeComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface,_flags);

        m_softCollision.drawEdit(_drawInterface, _flags);
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_PlatformTreeComponent_Template)
    BEGIN_SERIALIZATION(Ray_PlatformTreeComponent_Template)
        SERIALIZE_MEMBER("ignoreEventTrigger", m_ignoreEventTrigger);
        SERIALIZE_MEMBER("maxStartDelay", m_maxStartDelay);
        SERIALIZE_MEMBER("childMoveSpeed", m_childMoveSpeed);
        SERIALIZE_MEMBER("childMoveSpeedRandom", m_childMoveSpeedRandom);
        SERIALIZE_MEMBER("fastMode", m_fastMode);
        SERIALIZE_MEMBER("childAngleCorrectionDist", m_childAngleCorrectionDist);
        SERIALIZE_MEMBER("childLinkFadeDist", m_childLinkFadeDist);
        SERIALIZE_MEMBER("childAppearDist", m_childAppearDist);
        SERIALIZE_MEMBER("childDisappearDist", m_childDisappearDist);
        SERIALIZE_MEMBER("childOpenDist", m_childOpenDist);
        SERIALIZE_MEMBER("childCloseDist", m_childCloseDist);
        SERIALIZE_MEMBER("animOpened", m_animOpened);
        SERIALIZE_MEMBER("animClosed", m_animClosed);
        SERIALIZE_MEMBER("animAppear", m_animAppear);
        SERIALIZE_MEMBER("animDisappear", m_animDisappear);
        SERIALIZE_MEMBER("animHidden", m_animHidden);
        SERIALIZE_MEMBER("openingDelay", m_openingDelay);
        SERIALIZE_MEMBER("closingDelay", m_closingDelay);
        SERIALIZE_OBJECT("softCollision", m_softCollision);
        SERIALIZE_MEMBER("canWiggle", m_canWiggle);
        SERIALIZE_MEMBER("padRumbleWiggle", m_padRumbleWiggle);
        SERIALIZE_MEMBER("animWiggleOpened", m_animWiggleOpened);
        SERIALIZE_MEMBER("animWiggleClosed", m_animWiggleClosed);
        SERIALIZE_MEMBER("disableCollisionDuringTransition", m_disableCollisionDuringTransition);
        SERIALIZE_MEMBER("disableActorOnOpen", m_disableActorOnOpen);
    END_SERIALIZATION()

    Ray_PlatformTreeComponent_Template::Ray_PlatformTreeComponent_Template()
        : m_ignoreEventTrigger(bfalse)
        , m_maxStartDelay(1.f)
        , m_childMoveSpeed(3.f)
        , m_childMoveSpeedRandom(0.f)
        , m_fastMode(bfalse)
        , m_childAngleCorrectionDist(0.8f)
        , m_childLinkFadeDist(0.5f)
        , m_childOneOverLinkFadeDist(1.f / m_childLinkFadeDist)
        , m_childAppearDist(0.1f)
        , m_childOpenDist(0.7f)
        , m_childCloseDist(0.7f)
        , m_childDisappearDist(0.1f)
        , m_openingDelay(0.f)
        , m_closingDelay(0.f)
        , m_canWiggle(bfalse)
        , m_padRumbleWiggle()
        , m_animWiggleOpened()
        , m_animWiggleClosed()
        , m_disableCollisionDuringTransition(btrue)
        , m_disableActorOnOpen(bfalse)
    {
    }

    void Ray_PlatformTreeComponent_Template::onTemplateLoaded()
    {
        m_childOneOverLinkFadeDist = 1.f / m_childLinkFadeDist;
    }

};
