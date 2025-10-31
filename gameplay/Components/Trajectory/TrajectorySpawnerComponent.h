#ifndef _ITF_TRAJECTORYSPAWNERCOMPONENT_H_
#define _ITF_TRAJECTORYSPAWNERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF {

class EventTrajectorySpawn;
class TrajectoryNodeComponent;
class TrajectorySpawnerComponent_Template;

//------------------------------------------------------------------------------
typedef ITF_VECTOR<Path> TrajectorySpawneeList;

//------------------------------------------------------------------------------
class TrajectorySpawnerComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TrajectorySpawnerComponent, ActorComponent,1653629382);
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    TrajectorySpawnerComponent();
    virtual ~TrajectorySpawnerComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onEvent(Event * _event);

private:
    ITF_INLINE const TrajectorySpawnerComponent_Template* getTemplate() const;
    
    void processTrajectorySpawn(EventTrajectorySpawn* _trajectorySpawnEvt);

    bbool m_activated;
    TrajectorySpawneeList m_spawneePaths;
    ITF_VECTOR<SpawneeGenerator> m_spawneeGenerators;
    TrajectoryNodeComponent* m_trajectoryNodeComponent;
};

//------------------------------------------------------------------------------
class TrajectorySpawnerComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TrajectorySpawnerComponent_Template,TemplateActorComponent,32697399);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(TrajectorySpawnerComponent);

    ITF_INLINE const TrajectorySpawneeList& getSpawneePaths() const { return m_spawneePaths; }

public:
    TrajectorySpawnerComponent_Template();
    virtual ~TrajectorySpawnerComponent_Template();

    TrajectorySpawneeList m_spawneePaths;
};

//------------------------------------------------------------------------------
ITF_INLINE
const TrajectorySpawnerComponent_Template*
TrajectorySpawnerComponent::getTemplate() const
{
    return static_cast<const TrajectorySpawnerComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_TRAJECTORYSPAWNERCOMPONENT_H_
