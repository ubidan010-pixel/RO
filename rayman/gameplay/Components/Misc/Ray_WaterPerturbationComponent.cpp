#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WATERPERTURBATIONCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WaterPerturbationComponent.h"
#endif //_ITF_RAY_WATERPERTURBATIONCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_WaterPerturbationComponent)

BEGIN_SERIALIZATION_CHILD(Ray_WaterPerturbationComponent)
END_SERIALIZATION()

Ray_WaterPerturbationComponent::Ray_WaterPerturbationComponent()
: m_fxRef(ITF_INVALID_OBJREF)
, m_polyline(ITF_INVALID_OBJREF)
, m_surfaceEdge(U32_INVALID)
, m_surfaceT(0.f)
, m_onSurface(bfalse)
, m_closestEdge(U32_INVALID)
, m_closestT(0.f)
, m_currentTimer(0.f)
, m_updated(bfalse)
{
}

Ray_WaterPerturbationComponent::~Ray_WaterPerturbationComponent()
{
    if ( m_polyline != ITF_INVALID_OBJREF )
    {
        PolyLine* poly = AIUtils::getPolyLine(m_polyline);

        if ( poly )
        {
            EventSwiming swiming(m_actor->getRef(), bfalse, Vec2d::Zero, getTemplate()->getRadius(), getTemplate()->getWaterMultiplier(), getTemplate()->getWeight(),getTemplate()->getQueryPosition());
            poly->m_ownerFrise->onEvent(&swiming);
        }
    }
    
    if ( m_fxRef != ITF_INVALID_OBJREF )
    {
        Actor* fxActor = m_fxRef.getActor();

        if ( fxActor )
        {
            fxActor->requestDestruction();
        }

        m_fxRef = ITF_INVALID_OBJREF;
    }
}

void Ray_WaterPerturbationComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if (!getTemplate()->getFxFile().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_fxGenerator, getTemplate()->getFxFile());
    }

    m_noiseGenX.SetFrequency(getTemplate()->getNoiseFrequencyX());
    m_noiseGenX.SetPersistence(getTemplate()->getNoisePersistenceX());
    m_noiseGenX.SetNbOctave(getTemplate()->getNoiseNbOctaveX());
    m_noiseGenY.SetFrequency(getTemplate()->getNoiseFrequencyY());
    m_noiseGenY.SetPersistence(getTemplate()->getNoisePersistenceY());
    m_noiseGenY.SetNbOctave(getTemplate()->getNoiseNbOctaveY());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),this);
}

void Ray_WaterPerturbationComponent::onBecomeActive()
{
    Super::onBecomeActive();

    m_noiseGenX.SetSeed(Seeder::getSharedSeeder().GetU32());
    m_noiseGenY.SetSeed(Seeder::getSharedSeeder().GetU32());
}

void Ray_WaterPerturbationComponent::Update( f32 _dt )
{
    if ( !m_updated )
    {
        m_updated = btrue;

        if ( LINKMANAGER->getParents(m_actor->getRef()) )
        {
            m_actor->disable();
            return;
        }
    }

    m_pos = m_actor->get2DPos();

    if (getTemplate()->getQueryPosition())
    {
        EventQueryWaterInfluence query;
        m_actor->onEvent(&query);
        m_pos = query.getPos();
    }

    updateWaterData();
    updatePos(_dt);
    updateFX();
}

void Ray_WaterPerturbationComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( EventActivate* eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)) )
    {
        if ( eventActivate->getActivated() != m_actor->isEnabled() )
        {
            if ( eventActivate->getActivated() )
            {
                m_actor->enable();
                m_updated = btrue;
            }
            else
            {
                m_actor->disable();
            }
        }
    }
}

void Ray_WaterPerturbationComponent::updatePos( f32 _dt )
{
    if ( m_polyline == ITF_INVALID_OBJREF )
    {
        return;
    }

    if (!getTemplate()->getQueryPosition())
    {
        f32 noiseAmplitudeX = getTemplate()->getNoiseAmplitudeX();
        f32 noiseAmplitudeY = getTemplate()->getNoiseAmplitudeY();
        f32 curTime = m_currentTimer;
        f32 xNoise = m_noiseGenX.Get(curTime) * noiseAmplitudeX;
        f32 yNoise = m_noiseGenY.Get(curTime) * noiseAmplitudeY;

        // Safeguard against Perlin noise going crazy
        // PerlinNoise::Get() seems to return a value between -0.5 and 0.5, will assert otherwise
        if ( abs(xNoise) >= noiseAmplitudeX || abs(yNoise) >= noiseAmplitudeY )
        {
            // Bring the noise back towards zero
            xNoise -= xNoise * _dt;
            yNoise -= yNoise * _dt;
        }

        Vec2d startPos = m_actor->getBoundWorldInitialPos().truncateTo2D();
        Vec2d newPos = startPos + Vec2d(xNoise,yNoise);

        m_actor->setPos(newPos.to3d(m_actor->getPos().m_z));
    }

    m_currentTimer += _dt * getTemplate()->getNoiseTimeMultiplier();
}

void Ray_WaterPerturbationComponent::updateWaterData()
{
    ObjectRef prevPoly = m_polyline;
    AIUtils::PolylineRegionList regions;

    AIUtils::getPolylineRegions(m_actor->getDepth(),m_pos,regions);

    u32 numRegions = regions.size();

    m_polyline = ITF_INVALID_OBJREF;

    for ( u32 i = 0; i < numRegions; i++ )
    {
        AIUtils::PolylineRegionData& data = regions[i];

        if ( data.m_regionId == s_RegionName_Swim && data.m_polyline )
        {
            bbool inside;

            m_polyline = data.m_polyline->getRef();

            AIUtils::getSwimingPolylineData(data.m_polyline,m_pos.to3d(m_actor->getPos().m_z),-Vec2d::Up,1.f,
                m_surfaceEdge,m_surfaceT,m_onSurface,
                m_closestEdge,m_closestT,inside);

            break;
        }
    }

    if ( m_polyline == ITF_INVALID_OBJREF && prevPoly != ITF_INVALID_OBJREF )
    {
        PolyLine* poly = AIUtils::getPolyLine(prevPoly);

        if ( poly )
        {
            m_polyline = prevPoly;

            bbool inside;

            AIUtils::getSwimingPolylineData(poly,m_pos.to3d(m_actor->getPos().m_z),-Vec2d::Up,1.f,
                                            m_surfaceEdge,m_surfaceT,m_onSurface,
                                            m_closestEdge,m_closestT,inside);
        }
    }

    if ( prevPoly != m_polyline )
    {
        if ( prevPoly != ITF_INVALID_OBJREF )
        {
            PolyLine* poly = AIUtils::getPolyLine(prevPoly);

            if ( poly )
            {
                EventSwiming swiming(m_actor->getRef(), bfalse, Vec2d::Zero, getTemplate()->getRadius(), getTemplate()->getWaterMultiplier(), getTemplate()->getWeight(), getTemplate()->getQueryPosition());
                poly->m_ownerFrise->onEvent(&swiming);
            }
        }

        if ( m_polyline != ITF_INVALID_OBJREF )
        {
            PolyLine* poly = AIUtils::getPolyLine(m_polyline);

            if ( poly )
            {
                EventSwiming swiming(m_actor->getRef(), btrue, Vec2d::Zero, getTemplate()->getRadius(), getTemplate()->getWaterMultiplier(), getTemplate()->getWeight(),getTemplate()->getQueryPosition());
                poly->m_ownerFrise->onEvent(&swiming);
            }
        }
    }
}

void Ray_WaterPerturbationComponent::updateFX()
{
    if ( m_polyline == ITF_INVALID_OBJREF )
    {
        if ( m_fxRef != ITF_INVALID_OBJREF )
        {
            Actor* fxActor = m_fxRef.getActor();

            if ( fxActor )
            {
                fxActor->requestDestruction();
            }

            m_fxRef = ITF_INVALID_OBJREF;
        }
    }
    else
    {
        if ( m_fxRef == ITF_INVALID_OBJREF )
        {
            Actor* fxActor = NULL;
            if (m_fxGenerator.isInitialized())
            {
                fxActor = m_fxGenerator.getSpawnee(m_actor->getScene(),m_pos.to3d(m_actor->getPos().m_z),0.0f);
            }

            if ( fxActor )
            {
                m_fxRef = fxActor->getRef();
                m_actor->getBinding()->bindChild(m_fxRef);
            }
        }
        else
        {
            Actor* fxActor = m_fxRef.getActor();

            if ( fxActor )
            {
                PolyLine* polyline;
                const PolyLineEdge* edge;

                AIUtils::getPolyLine(m_polyline,m_surfaceEdge,polyline,edge);

                if ( edge )
                {
                    Vec2d surfacePoint = edge->getPos() + (edge->m_vector * m_surfaceT);

                    fxActor->setPos(surfacePoint.to3d(m_actor->getPos().m_z));
                }
            }
            else
            {
                m_fxRef = ITF_INVALID_OBJREF;
            }
        }
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_WaterPerturbationComponent_Template)
BEGIN_SERIALIZATION(Ray_WaterPerturbationComponent_Template)

    SERIALIZE_MEMBER("fxFile",m_fxFile);
    SERIALIZE_MEMBER("noiseAmplitudeX",m_noiseAmplitudeX);
    SERIALIZE_MEMBER("noiseFrequencyX",m_noiseFrequencyX);
    SERIALIZE_MEMBER("noisePersistenceX",m_noisePersistenceX);
    SERIALIZE_MEMBER("noiseNbOctaveX",m_noiseNbOctaveX);
    SERIALIZE_MEMBER("noiseAmplitudeY",m_noiseAmplitudeY);
    SERIALIZE_MEMBER("noiseFrequencyY",m_noiseFrequencyY);
    SERIALIZE_MEMBER("noisePersistenceY",m_noisePersistenceY);
    SERIALIZE_MEMBER("noiseNbOctaveY",m_noiseNbOctaveY);
    SERIALIZE_MEMBER("noiseTimeMultiplier",m_noiseTimeMultiplier);
    SERIALIZE_MEMBER("radius",m_radius);
    SERIALIZE_MEMBER("waterMultiplier",m_waterMultiplier);
    SERIALIZE_MEMBER("weight",m_weight);
    SERIALIZE_MEMBER("queryPosition",m_queryPosition);

END_SERIALIZATION()

Ray_WaterPerturbationComponent_Template::Ray_WaterPerturbationComponent_Template()
: m_noiseAmplitudeX(2.f)
, m_noiseFrequencyX(1.f)
, m_noisePersistenceX(0.4f)
, m_noiseNbOctaveX(4)
, m_noiseAmplitudeY(1.f)
, m_noiseFrequencyY(1.f)
, m_noisePersistenceY(0.4f)
, m_noiseNbOctaveY(4)
, m_noiseTimeMultiplier(6.f)
, m_radius(1.f)
, m_waterMultiplier(1.f)
, m_weight(1.f)
, m_queryPosition(bfalse)
{
}

Ray_WaterPerturbationComponent_Template::~Ray_WaterPerturbationComponent_Template()
{
}

} // namespace ITF

