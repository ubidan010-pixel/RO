#ifndef _ITF_RAY_ELECTOONSPAWNERCOMPONENT_H_
#define _ITF_RAY_ELECTOONSPAWNERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{

//------------------------------------------------------------------------------
class Ray_ElectoonSpawnerComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ElectoonSpawnerComponent, ActorComponent,872832010)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_ElectoonSpawnerComponent();
    virtual ~Ray_ElectoonSpawnerComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void Update(f32 _dt);


    virtual void onEvent(Event* _event);
#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit( class ActorDrawEditInterface* , u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

private:

    ITF_INLINE const class Ray_ElectoonSpawnerComponent_Template * getTemplate() const;


    SpawneeGenerator m_generator;
    LinkComponent * m_linkComponent;

    struct ElectoonTarget
    {
        ElectoonTarget() : m_curSpeed(0.0f)
            ,m_targetDist(0.0f)
            , m_curT(0.0f)
            , m_departureTime(0.0f)
        {
        }
        ObjectRef m_targetRef;
        Bezier4<Vec2d> m_bezier;
        ActorRef m_electoon;
        f32 m_curSpeed;
        f32 m_curT;
        f32 m_targetDist;
        f32 m_departureTime;
    };
    typedef ITF_VECTOR< ElectoonTarget > TargetList;
    TargetList m_targets;
    bbool m_processCurves;

    bbool updateElectoon( ElectoonTarget &target, f32 _dt, AABB & _aabb );
    void spawnElectoon( ElectoonTarget &t );

    u32 m_numToSpawn;
    f32 m_spawnTime;
    f32 m_destinationRadius;
    bbool m_startSpawning;
    ActorRef m_activator;
    f32 m_timer;

    void processTrigger(ActorRef _actor);

    const f32 getTanLength( const f32 speed ) const;

};

//------------------------------------------------------------------------------
class Ray_ElectoonSpawnerComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ElectoonSpawnerComponent_Template,TemplateActorComponent,1990605819);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ElectoonSpawnerComponent);

public:
    Ray_ElectoonSpawnerComponent_Template();
    ~Ray_ElectoonSpawnerComponent_Template();

    ITF_INLINE const Path& getPath() const { return m_path; }
    ITF_INLINE f32 getMaxTangeantLength() const { return m_maxTangeantLength; }
    ITF_INLINE f32 getTangeantSpeedMax() const { return m_tangeantSpeedMax; }
    ITF_INLINE f32 getMinTangeantLength() const { return m_minTangeantLength; }
    ITF_INLINE f32 getTangeantSpeedMin() const { return m_tangeantSpeedMin; }
    ITF_INLINE f32 getElectoonAcceleration() const { return m_electoonAcceleration; }
    ITF_INLINE f32 getElectoonSpeed() const { return m_electoonSpeed; }
    ITF_INLINE f32 getTargetSmooth() const { return m_targetSmooth; }

private:
    Path m_path;
    f32 m_maxTangeantLength;
    f32 m_minTangeantLength;
    f32 m_tangeantSpeedMax;
    f32 m_tangeantSpeedMin;
    f32 m_electoonSpeed;
    f32 m_electoonAcceleration;
    f32 m_targetSmooth;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_ElectoonSpawnerComponent_Template*
Ray_ElectoonSpawnerComponent::getTemplate() const
{
    return static_cast<const Ray_ElectoonSpawnerComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_ELECTOONSPAWNERCOMPONENT_H_
