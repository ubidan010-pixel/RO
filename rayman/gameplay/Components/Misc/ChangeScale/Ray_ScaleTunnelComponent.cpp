#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SCALETUNNELCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/ChangeScale/Ray_ScaleTunnelComponent.h"
#endif //_ITF_RAY_SCALETUNNELCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_ScaleTunnelComponent)

BEGIN_SERIALIZATION_CHILD(Ray_ScaleTunnelComponent)
END_SERIALIZATION()


Ray_ScaleTunnelComponent::Ray_ScaleTunnelComponent()
{
}

Ray_ScaleTunnelComponent::~Ray_ScaleTunnelComponent()
{
}

void Ray_ScaleTunnelComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    updateShape();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374),this);
}

void Ray_ScaleTunnelComponent::updateShape()
{
    PhysShape::CopyShapeScaled(&getTemplate()->getShape(), &m_scaledBox, m_actor->getScale());

    AABB aabb(m_actor->get2DPos());
    AABB shapeAABB;
    m_scaledBox.calculateAABB(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),shapeAABB);
    aabb.grow(shapeAABB);
    m_actor->growRelativeAABBFromAbsolute(aabb);
}

void Ray_ScaleTunnelComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    FixedArray <ActorData,32> actorsInside;
    const Vec2d& myPos = m_actor->get2DPos();
    const f32 myAngle = m_actor->getAngle();
    AABB myAABB;

    m_scaledBox.calculateAABB(myPos, myPos, myAngle, myAABB);

    const u32 playerCount = GAMEMANAGER->getMaxPlayerCount();
    const DepthRange depthRange(m_actor->getDepth());
    AABB playerAABB;
    u32 numInsideBefore = m_actorsInside.size();

    for ( u32 i = 0; i < playerCount; ++i )
    {
        Player* player = GAMEMANAGER->getPlayer(i);

        if (!player->getActive())
        {
            continue;
        }

        if (player->isDead())
        {
            continue;
        }

        Actor* playerActor = player->getActor();

        if ( !playerActor || depthRange != DepthRange(playerActor->getDepth()) )
        {
            continue;
        }

        const Vec2d& playerPos = playerActor->get2DPos();

        if ( !myAABB.contains(playerPos) )
        {
            continue;
        }

        bbool ignore = bfalse;
        bbool foundInList = bfalse;

        for ( u32 insideIndex = 0; insideIndex < numInsideBefore; insideIndex++ )
        {
            ActorData& actorData = m_actorsInside[insideIndex];

            if ( actorData.m_ref == playerActor->getRef() )
            {
                ignore = actorData.m_ignore;
                foundInList = btrue;
                break;
            }
        }

        if ( !foundInList )
        {
            f32 targetScale = playerActor->getWorldInitialScale().m_x*getTemplate()->getScale();

            if ( targetScale == playerActor->getScale().m_x )
            {
                f32 t = calculateTFromPos(playerActor->get2DPos());

                if ( t < 0.5f )
                {
                    ignore = btrue;
                }
            }
        }

        if ( !actorsInside.full() )
        {
            ActorData newData;

            newData.m_ignore = ignore;
            newData.m_ref = playerActor->getRef();

            actorsInside.push_back(newData);
        }

        if ( !ignore )
        {
            f32 t = calculateTFromPos(playerActor->get2DPos());

            Ray_EventScaleTunnelTrigger scaleEvent(m_actor->getRef(),getTemplate()->getScale(),t);

            playerActor->onEvent(&scaleEvent);
        }
    }

    u32 numInsideNow = actorsInside.size();

    for ( u32 i = 0; i < numInsideBefore; i++ )
    {
        const ActorData& actorData = m_actorsInside[i];

        if ( !actorData.m_ignore )
        {
            bbool found = bfalse;

            for ( u32 insideIndex = 0; insideIndex < numInsideNow; insideIndex++ )
            {
                ActorData& tmpData = actorsInside[insideIndex];

                if ( actorData.m_ref == tmpData.m_ref )
                {
                    found = btrue;
                    break;
                }
            }

            if ( !found )
            {
                Actor* playerActor = actorData.m_ref.getActor();

                if ( playerActor )
                {
                    Vec2d localPos = ( playerActor->get2DPos() - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());

                    if ( m_actor->isFlipped() )
                    {
                        localPos.m_x *= -1.f;
                    }

                    f32 t;

                    if ( localPos.m_x < 0.f )
                    {
                        t = 0.f;
                    }
                    else
                    {
                        t = 1.f;
                    }

                    Ray_EventScaleTunnelTrigger scaleEvent(m_actor->getRef(),getTemplate()->getScale(),t);

                    playerActor->onEvent(&scaleEvent);
                }
            }
        }
    }

    m_actorsInside.clear();

    for ( u32 i = 0; i < numInsideNow; i++ )
    {
        m_actorsInside.push_back(actorsInside[i]);
    }
}

f32 Ray_ScaleTunnelComponent::calculateTFromPos( const Vec2d& _pos ) const
{
    Vec2d localPos = ( _pos - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());
    f32 t = ( localPos.m_x + m_scaledBox.getExtent().m_x ) / ( m_scaledBox.getExtent().m_x * 2.f );

    t = Clamp(t,0.f,1.f);

    if ( m_actor->isFlipped() )
    {
        t = 1.f - t;
    }

    return t;
}

void Ray_ScaleTunnelComponent::onEvent( Event* _event )
{
    if (EventScaleChanged* scaleChanged = _event->DynamicCast<EventScaleChanged>(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374)))
    {
        processScaleChanged(scaleChanged);
    }
}

void Ray_ScaleTunnelComponent::processScaleChanged( EventScaleChanged* _event )
{
    updateShape();
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_ScaleTunnelComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    DebugDraw::shape(m_actor->get2DPos(), m_actor->getPos().m_z, m_actor->getAngle(), &m_scaledBox,
        Color::white(), 0.f, "area", 85 * MTH_DEGTORAD);
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ScaleTunnelComponent_Template)

BEGIN_SERIALIZATION(Ray_ScaleTunnelComponent_Template)

    SERIALIZE_OBJECT("area",m_shape);
    SERIALIZE_MEMBER("scale",m_scale);

END_SERIALIZATION()

Ray_ScaleTunnelComponent_Template::Ray_ScaleTunnelComponent_Template()
: m_scale(0.5f)
{
}

};