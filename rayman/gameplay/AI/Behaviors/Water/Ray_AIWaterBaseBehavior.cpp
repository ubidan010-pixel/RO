#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIWATERBASEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterBaseBehavior.h"
#endif //_ITF_RAY_AIWATERBASEBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#include "engine/actors/components/SoundComponent.h"

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_AIWaterBaseBehavior_Template)

BEGIN_SERIALIZATION_CHILD(Ray_AIWaterBaseBehavior_Template)
    SERIALIZE_MEMBER("gravityMultiplier", m_gravityMultiplier);
END_SERIALIZATION()

Ray_AIWaterBaseBehavior_Template::Ray_AIWaterBaseBehavior_Template()
: m_gravityMultiplier(0.0f)
{
}

IMPLEMENT_OBJECT_RTTI(Ray_AIWaterBaseBehavior)

Ray_AIWaterBaseBehavior::Ray_AIWaterBaseBehavior()
: m_physComponent(NULL)
, m_swimingPolyline(ITF_INVALID_OBJREF)
, m_surfaceEdge(U32_INVALID)
, m_surfaceEdgeT(0.f)
, m_closestEdge(U32_INVALID)
, m_closestEdgeT(0.f)
, m_waterResistance(5.f)
, m_distanceFromWater(0.f)
, m_distanceFromSurface(0.0f)
, m_onSurface(bfalse)
, m_inside(bfalse)
, m_active(bfalse)
{
}

Ray_AIWaterBaseBehavior::~Ray_AIWaterBaseBehavior()
{
    PolyLine* poly = AIUtils::getPolyLine(m_swimingPolyline);

    if ( poly && poly->m_ownerFrise)
    {
        EventSwiming swiming(m_actor->getRef(), bfalse, Vec2d::Zero, 0.f, 1.f, 1.f,bfalse);
        poly->m_ownerFrise->onEvent(&swiming);
    }
}

void Ray_AIWaterBaseBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

    ITF_ASSERT(m_physComponent);
}

void Ray_AIWaterBaseBehavior::onActivate()
{
    m_active = btrue;

    PolyLine* poly = AIUtils::getPolyLine(m_swimingPolyline);

    setSwimingPolylineParameters(poly);
}

void Ray_AIWaterBaseBehavior::onDeactivate()
{
    setSwimingPolyline(ITF_INVALID_OBJREF);
    m_active = bfalse;
}

void Ray_AIWaterBaseBehavior::setSwimingPolyline( ObjectRef _ref )
{
    if ( m_swimingPolyline != _ref )
    {
        PolyLine* poly = AIUtils::getPolyLine(m_swimingPolyline);

        if ( poly && poly->m_ownerFrise)
        {
            EventSwiming swiming(m_actor->getRef(), bfalse, m_physComponent->getSpeed(), 0.f, m_physComponent->getWaterMultiplier(), m_physComponent->getWeight(),bfalse);
            poly->m_ownerFrise->onEvent(&swiming);
        }

        m_swimingPolyline = _ref;

        poly = AIUtils::getPolyLine(m_swimingPolyline);

        if ( poly && poly->m_ownerFrise)
        {
            EventSwiming swiming(m_actor->getRef(), btrue, m_physComponent->getSpeed(), m_actor->getRadius(), m_physComponent->getWaterMultiplier(), m_physComponent->getWeight(),bfalse);
            poly->m_ownerFrise->onEvent(&swiming);
        }

        if ( m_active )
        {
            setSwimingPolylineParameters(poly);
        }
    }
}

void Ray_AIWaterBaseBehavior::setSwimingPolylineParameters( PolyLine* _poly )
{
    static f32 s_defaultWaterResistance = 1.f;

    if ( _poly )
    {
        const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(_poly->getEdgeAt(0).getGameMaterial()));

        if ( mat )
        {
            m_waterResistance = mat->getWaterResistance();
        }
        else
        {
            m_waterResistance = s_defaultWaterResistance;
        }

        m_physComponent->setAirFrictionMultiplier(m_waterResistance);
        m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplier());
        m_physComponent->setUpdateRotation(bfalse);
    }
    else
    {
        m_physComponent->setAirFrictionMultiplier(0.f);
        m_physComponent->setGravityMultiplier(m_physComponent->getInitialGravityMultiplier());
        m_physComponent->setUpdateRotation(btrue);
    }
}

void Ray_AIWaterBaseBehavior::update( f32 _dt )
{
    PolyLine* swimPoly = AIUtils::getPolyLine(m_swimingPolyline);

    if ( swimPoly )
    {
        AIUtils::getSwimingPolylineData(swimPoly,m_actor->getPos(),m_aiComponent->getCurrentGravityDir(),0.1f,
            m_surfaceEdge,m_surfaceEdgeT,m_onSurface,
            m_closestEdge,m_closestEdgeT,m_inside);

        if ( !m_inside && m_closestEdge != U32_INVALID )
        {
            const PolyLineEdge& closestEdge = swimPoly->getEdgeAt(m_closestEdge);
            Vec2d edgePos = closestEdge.getPos() + (closestEdge.m_vector*m_closestEdgeT);

            m_distanceFromWater = ( edgePos - m_actor->get2DPos() ).norm();
        }
        else
        {
            m_distanceFromWater = 0.f;
        }

        const PolyLineEdge& surfaceEdge = swimPoly->getEdgeAt(m_surfaceEdge);
        Vec2d edgePos = surfaceEdge.getPos() + (surfaceEdge.m_vector*m_surfaceEdgeT);

        m_distanceFromSurface = ( edgePos - m_actor->get2DPos() ).norm();

    }
    else
    {
        PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());
        if (waterPoly)
            setSwimingPolyline(waterPoly->getRef());
    }

    Super::update(_dt);
}

void Ray_AIWaterBaseBehavior::updateSoundInputs()
{
    Super::updateSoundInputs();

    if (m_soundComponent)
    {
        m_soundComponent->setInput(ITF_GET_STRINGID_CRC(DistanceFromSurface,3029866390),m_distanceFromSurface);
    }
}

}

