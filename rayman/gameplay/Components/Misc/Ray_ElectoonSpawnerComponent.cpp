#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ELECTOONSPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ElectoonSpawnerComponent.h"
#endif //_ITF_RAY_ELECTOONSPAWNERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ElectoonSpawnerComponent)
BEGIN_SERIALIZATION_CHILD(Ray_ElectoonSpawnerComponent)
SERIALIZE_MEMBER("numToSpawn",m_numToSpawn);
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_ElectoonSpawnerComponent)
VALIDATE_COMPONENT_PARAM("path", !getTemplate()->getPath().isEmpty(), "path mandatory");
VALIDATE_COMPONENT_PARAM("linkComponent", m_linkComponent, "link component mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_ElectoonSpawnerComponent::Ray_ElectoonSpawnerComponent()
    : Super()
    , m_linkComponent(NULL)
    , m_processCurves(bfalse)
    , m_numToSpawn(5)
    , m_spawnTime(2.0f)
    , m_destinationRadius(3.0f)
    , m_startSpawning(bfalse)
    , m_timer(0.0f)
{
}

//------------------------------------------------------------------------------
Ray_ElectoonSpawnerComponent::~Ray_ElectoonSpawnerComponent()
{
}

//------------------------------------------------------------------------------
void Ray_ElectoonSpawnerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);


    m_linkComponent = m_actor->GetComponent<LinkComponent>();

    // initialize spawner
    if (!getTemplate()->getPath().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_generator, getTemplate()->getPath());
    }
    
    // register to activate event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
    m_processCurves = bfalse;
    m_targets.clear();
    m_startSpawning = bfalse;

}
f32 getTatDist(Bezier4<Vec2d> & _b, f32 _dist, u32 _numSamples)
{
    if (_dist == 0.0f)
        return 0.0f;

    const f32 tInc = 1.0f / (f32)_numSamples;
    f32 t = 0.0f;
    f32 prevT = 0.0f;
    f32 dist = 0.0f;
    Vec2d prev = _b.m_p0;
    for (u32 i = 0; i < _numSamples; ++i)
    {
        const Vec2d curPos = _b.getPos(t);
        const f32 distInc = (curPos - prev).norm();

        if ((dist + distInc) >= _dist && dist < _dist)
        {
            return t + ((_dist - dist) / distInc) * tInc;
        }

        dist += distInc;
        prev = curPos;
        prevT = t;
        t += tInc;

    }
    return 1.0f;
}


//------------------------------------------------------------------------------
void Ray_ElectoonSpawnerComponent::Update(f32 _dt)
{
    Super::Update(_dt);
    if (!m_processCurves)
    {
        m_processCurves = btrue;
        m_targets.clear();

        const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();

        for (LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it)
        {
            const ChildEntry & child = *it;
            const ObjectPath& obj = child.getPath();
            Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,obj);
            if (pickable)
            {
                for (u32 i = 0; i < m_numToSpawn; ++i)
                {
                    ElectoonTarget t;
                    t.m_targetRef = pickable->getRef();
                    t.m_departureTime = Seeder::getSharedSeeder().GetFloat(0.0f,m_spawnTime);
                    m_targets.push_back(t);
                }
            }
        }
    }

    AABB aabb(m_actor->get2DPos());
    bbool updateAABB = bfalse;
    if (m_startSpawning)
    {
        for (TargetList::iterator it = m_targets.begin(); it != m_targets.end(); ++it)
        {
            ElectoonTarget & target = *it;
            if (m_timer > target.m_departureTime)
            {
                if (!target.m_electoon.isValid() )
                {
                    //spawn!
                    spawnElectoon(target);
                }
                if (target.m_electoon.isValid() && target.m_targetRef.isValid())
                {
                    updateAABB = updateElectoon(target, _dt,aabb) || updateAABB;
                }
            }
        }
        m_timer += _dt;
    }

    if (updateAABB)
    {
        Actor * actor = m_activator.getActor();
        if (actor)
            aabb.grow(actor->get2DPos());
    }

    m_actor->growRelativeAABBFromAbsolute(aabb);
}



//------------------------------------------------------------------------------
void Ray_ElectoonSpawnerComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);
    
    if (m_processCurves)
    {
        if (EventTrigger* trig = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (trig->getActivated())
            {
                processTrigger(_event->getActivator());
            }
        }
    }
}

void Ray_ElectoonSpawnerComponent::processTrigger( ActorRef _actor )
{
    Actor * actor = _actor.getActor();
    if (!actor)
        return;

    m_activator = _actor;

    if (!m_startSpawning)
    {
        m_startSpawning = btrue;
        m_timer = 0.0f;
    }



}


const f32 Ray_ElectoonSpawnerComponent::getTanLength( const f32 speed ) const
{
    const f32 maxLength = getTemplate()->getMaxTangeantLength();
    const f32 minLength = getTemplate()->getMinTangeantLength();
    const f32 diff = maxLength - minLength;

    const f32 minSpeed = getTemplate()->getTangeantSpeedMin();
    const f32 maxSpeed = getTemplate()->getTangeantSpeedMax();
    const f32 speedDiff = maxSpeed - minSpeed;

    const f32 tanLength = minLength +  ((speed - minSpeed)/speedDiff) * diff;	
    return tanLength;
}
//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ElectoonSpawnerComponent_Template)
BEGIN_SERIALIZATION(Ray_ElectoonSpawnerComponent_Template)
SERIALIZE_MEMBER("path", m_path);
SERIALIZE_MEMBER("maxTangeantLength", m_maxTangeantLength);
SERIALIZE_MEMBER("minTangeantLength", m_minTangeantLength);
SERIALIZE_MEMBER("tangeantSpeedMax", m_tangeantSpeedMax);
SERIALIZE_MEMBER("tangeantSpeedMin", m_tangeantSpeedMin);
SERIALIZE_MEMBER("electoonSpeed", m_electoonSpeed);
SERIALIZE_MEMBER("electoonAcceleration", m_electoonAcceleration);
SERIALIZE_MEMBER("targetSmooth", m_targetSmooth);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_ElectoonSpawnerComponent_Template::Ray_ElectoonSpawnerComponent_Template()
    : Super()
    , m_path()
    , m_maxTangeantLength(2.0f)
    , m_minTangeantLength(1.0f)
    , m_tangeantSpeedMax(5.0f)
    , m_tangeantSpeedMin(1.0f)
    , m_electoonSpeed(5.0f)
    , m_electoonAcceleration(10.0f)
    , m_targetSmooth(0.5f)
{
}

//------------------------------------------------------------------------------
Ray_ElectoonSpawnerComponent_Template::~Ray_ElectoonSpawnerComponent_Template()
{
}


void Ray_ElectoonSpawnerComponent::spawnElectoon( ElectoonTarget &t )
{
    Actor * actor = m_activator.getActor();
    if (actor)
    {


        //get trigger info
        EventQueryPhysicsData query;
        actor->onEvent(&query);

        const Vec2d pos = actor->get2DPos();
        const Vec2d speedVector = query.getSpeed();
        f32 speed = speedVector.norm();
        Vec2d dir;
        if (speed < MTH_EPSILON)
        {
            speed = getTemplate()->getTangeantSpeedMin();
            dir = AIUtils::getLookDir(actor);
        }
        else
            dir = speedVector/speed;


        //calculate first point and tangeant;
        const f32 tanLength = getTanLength(speed);

        const Vec2d tanA = pos + dir * tanLength;

        //calculate all other points

        Scene * scene = m_actor->getScene();

        BaseObject * b = t.m_targetRef.getObject();
        if (b)
        {
            Pickable * p = b->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
            const f32 dist = Seeder::getSharedSeeder().GetFloat(0.0f,m_destinationRadius);
            const f32 angle = Seeder::getSharedSeeder().GetFloat(0.0f,MTH_2PI);
            if (p)
            {
                Bezier4<Vec2d> & b = t.m_bezier;
                b.m_p0 = pos;
                b.m_p1 = tanA;

                const Vec2d targetPos = p->get2DPos() + Vec2d::Right.Rotate(angle) * dist;

                Vec2d tanBdir = pos - targetPos;
                tanBdir.normalize();

                b.m_p2 = targetPos + tanBdir * tanLength;
                b.m_p3 = targetPos ;

                Actor * s = m_generator.getSpawnee(scene,pos.to3d(m_actor->getDepth()));
                if (s)
                {
                    t.m_electoon = s->getRef();
                }
            }
        }
    }
}

bbool Ray_ElectoonSpawnerComponent::updateElectoon( ElectoonTarget &target, f32 _dt, AABB & _aabb )
{
    Actor * electoon = target.m_electoon.getActor();
    if (electoon)
    {
        target.m_targetDist += target.m_curSpeed * _dt;
        const f32 t = getTatDist(target.m_bezier,target.m_targetDist,32);

        //            target.m_curT = (t-target.m_curT) * getTemplate()->getTargetSmooth() + target.m_curT;
        target.m_curT  = t;
        if (target.m_curT >= 1.0f)
        {
            electoon->requestDestruction();
            target.m_electoon.invalidate();
            target.m_targetRef.invalidate();
            return bfalse;
        }

        _aabb.grow(target.m_bezier.m_p0);
        _aabb.grow(target.m_bezier.m_p1);
        _aabb.grow(target.m_bezier.m_p2);
        _aabb.grow(target.m_bezier.m_p3);

        electoon->set2DPos(target.m_bezier.getPos(target.m_curT));
        target.m_curSpeed = Clamp(target.m_curSpeed + _dt * getTemplate()->getElectoonAcceleration(),0.0f,getTemplate()->getElectoonSpeed());
        return btrue;
    }
    return bfalse;
}

#ifdef ITF_SUPPORT_EDITOR
    void Ray_ElectoonSpawnerComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        Bezier4<Vec2d> bezier;
        for (TargetList::const_iterator it = m_targets.begin(); it != m_targets.end(); ++it)
        {
            const ElectoonTarget & t = *it;
            
            BaseObject * b = t.m_targetRef.getObject();
            if (b)
            {
                Pickable * p = b->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
                if (p)
                {
                    DebugDraw::bezier(t.m_bezier,m_actor->getDepth(),Color::yellow(),1.0f,0.1f);
                }
            }
        }
    }


 

#endif // ITF_SUPPORT_EDITOR



} // namespace ITF