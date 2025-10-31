#include "precompiled_gameplay.h"

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIAction_Template)
    BEGIN_SERIALIZATION_ABSTRACT(AIAction_Template)
        SERIALIZE_MEMBER("action", m_animDefaultAction);
        SERIALIZE_MEMBER("endMarker", m_endMarker);
        SERIALIZE_MEMBER("useRootPos", m_useRootPos);
        SERIALIZE_MEMBER("finishOnContact", m_finishOnContact);
        SERIALIZE_MEMBER("rootPosScale", m_rootPosScale);
        SERIALIZE_MEMBER("ignoreContactDuration", m_ignoreContactDuration);

BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifdef ITF_SUPPORT_DEBUGFEATURE
        SERIALIZE_MEMBER("debugName", m_debugName);
#endif //!ITF_SUPPORT_DEBUGFEATURE
END_CONDITION_BLOCK()

    END_SERIALIZATION()

        AIAction_Template::AIAction_Template() 
        : m_useRootPos(bfalse)
        , m_finishOnContact(btrue)
        , m_rootPosScale(1.0f,1.0f)
        , m_ignoreContactDuration(0.05f)
    {
        m_endMarker = MRK_AI_Stop;
    }


    IMPLEMENT_OBJECT_RTTI(AIAction)

        AIAction::AIAction()
        : Super()
        , m_actor(NULL)
        , m_aiComponent(NULL)
        , m_aiBehavior(NULL)
        , m_animComponent(NULL)
        , m_animatedComponent(NULL)
        , m_fxController(NULL)
        , m_physComponent(NULL)
        , m_useRootPos(bfalse)
    {
    }

    AIAction::~AIAction()
    {
    }

    void AIAction::init( Actor* _actor, AIComponent* _aiComponent, AIBehavior* _aiBehavior )
    {
        m_actor = _actor;
        m_aiComponent = _aiComponent;
        m_aiBehavior = _aiBehavior;

        m_animComponent     = m_actor->GetComponent<AnimLightComponent>();
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_fxController      = m_actor->GetComponent<FXControllerComponent>();
        m_physComponent     = m_actor->GetComponent<StickToPolylinePhysComponent>();

        onActorLoaded(Pickable::HotReloadType_None);
    }

    void AIAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
    }

    void AIAction::onEvent( Event * _event )
    {
        if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if ( animEvent->getName() == getTemplate()->getEndMarker() )
            {
                onEndMarkerReached();
            }
        }
    }

    void AIAction::onActivate()
    {
        activateRootPos(getTemplate()->getUseRootPos());
        m_timer = getTemplate()->getIgnoreContactDuration();
        playAction(getAnimAction());
    }

    void AIAction::playAction(StringID _action)
    {
        if (_action.isValid() && _action != ITF_GET_STRINGID_CRC(-,2000040631) )
        {
            if (m_animatedComponent)
                m_animatedComponent->setAnim(_action);
            else if (m_animComponent)
                m_animComponent->setAnim(_action);
        }
    }

    bbool AIAction::isAnimFinished() const
    {
        if ( m_animatedComponent )
        {
            return m_animatedComponent->isMainNodeFinished();
        }
        else if ( m_animComponent )
        {
            return m_animComponent->isSubAnimFinished();
        }
        else
        {
            // There should be one or the other.
            // If this asserts it means there is a problem with the postload
            // Maybe an inherited action is not calling Super::onActorLoaded(_hotReload)?
            ITF_ASSERT_MSG(0, "This action has no AnimLightComponent/AnimatedComponent! Did you forget to add one? Did you forget to call Super::onActorLoaded(_hotReload)?");
            return bfalse;
        }
    }

    void AIAction::onEndMarkerReached()
    {
        m_aiBehavior->onActionFinished();
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void AIAction::getDebugText( String8& _text )
    {
        if (getTemplate()->getDebugName().isEmpty())
        {
            _text += StringID(GetObjectClassCRC()).getDebugString();
        }
        else
        {
            String8 s;s.setTextFormat("%s (%s)", getTemplate()->getDebugName().cStr(), StringID(GetObjectClassCRC()).getDebugString());
            _text += s;
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    void AIAction::activateRootPos( bbool _activation )
    {
        if (_activation != m_useRootPos)
        {
            m_useRootPos = _activation;
            if (_activation)
            {
                if (getTemplate()->getUseRootPos() && m_physComponent)
                {
                    m_physComponent->setSpeed(Vec2d::Zero);
                    m_physComponent->resetForces();
                    m_physComponent->setDisabled(btrue);
                }
            }
            else
            {
                if (getTemplate()->getUseRootPos() && m_physComponent)
                {
                    m_physComponent->setDisabled(bfalse);
                }
            }
        }
    }
    void AIAction::onDeactivate()
    {
        activateRootPos(bfalse);
    }

    void AIAction::update( f32 _dt )
    {
        updatePosition(_dt);
    }

    void AIAction::updatePosition( f32 _dt )
    {
        if (m_useRootPos && m_animComponent->canGetRootDelta())
        {
            Vec2d delta = m_animComponent->getRootDelta();
            delta = delta.Rotate(m_actor->getAngle());
            delta *= getTemplate()->getRootPosScale();

            if (m_physComponent)
            {
                ITF_WARNING_CATEGORY(LD,m_actor, !m_actor->getParentBind(), "This actor shouldn't be bound because it's moving by itself in the world");

                PhysShapeCircle circle(m_physComponent->getRadius());

                PhysContactsContainer contacts;
                PHYSWORLD->checkEncroachment(m_actor->get2DPos(),m_actor->get2DPos() + delta,m_actor->getAngle(),&circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);


                m_physComponent->setSpeed(delta / _dt);

                u32 blockingContact = U32_INVALID;
                f32 blockingContactT = -1.f;
                FixedArray <bbool,32> processedContacts;

                processedContacts.resize(contacts.size());

                for ( u32 i = 0; i < contacts.size(); i++ )
                {
                    const SCollidableContact& c = contacts[i];

                    if ( c.m_edgeIndex == U32_INVALID )
                    {
                        processedContacts[i] = btrue;
                        continue;
                    }

                    PolyLine* polyLine = static_cast<PolyLine*>(GETOBJECT(c.m_collidableUserData));

                    if ( polyLine && polyLine->getOwnerActor() == m_actor )
                    {
                        processedContacts[i] = btrue;
                        continue;
                    }

                    processedContacts[i] = btrue;

                    if ( isContactBlockingObstacle(delta,c,m_actor->getAngle()) )
                    {
                        f32 dist = ( c.m_contactPoint - m_actor->get2DPos() ).sqrnorm();

                        if ( blockingContact == U32_INVALID ||
                            blockingContactT > dist )
                        {
                            blockingContact = i;
                            blockingContactT = dist;
                        }
                    }
                }

                Vec2d pos = m_actor->get2DPos() + delta;

                if ( blockingContact != U32_INVALID && m_timer == 0.0f )
                {
                    // If we found an obstacle we will want to readjust our position
                    m_physComponent->forceFindStickEdge();
                    if (m_physComponent->getStickedEdge() && getTemplate()->getFinishOnContact())
                        m_aiBehavior->onActionFinished();
                    else
                    {
                        pos = contacts[blockingContact].m_contactPoint - contacts[blockingContact].m_normal * m_physComponent->getRadius();
                    }
                }
                
                m_actor->set2DPos(pos);
            }
            else // !m_physComponent
            {
                m_actor->setBoundWorldPos(m_actor->getPos() + delta.to3d());
            }
            m_timer = Max(m_timer-_dt,0.0f);
        }
    }

    bbool AIAction::isContactBlockingObstacle( const Vec2d & _delta, const SCollidableContact& _contact, f32 _angle ) const
    {
        PolyLine* polyLine;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(_contact.m_collidableUserData,_contact.m_edgeIndex,polyLine,edge);

        if ( !polyLine || !edge )
        {
            return bfalse;
        }

        const GameMaterial_Template* mat = (World::getGameMaterial(edge->getGameMaterial()));


        return AIUtils::isContactBlockingObstacle(edge->getPos(),edge->m_normalizedVector,edge->m_length,mat,polyLine,edge,1.f,_contact,_delta,
           m_physComponent->getRadius(),m_physComponent->getStickedPolylineRef(),m_physComponent->getStickedEdgeIndex(),m_physComponent->getRelativeEdgeDistance(),m_physComponent->getCurrentGravityDir(),bfalse);
    }

}