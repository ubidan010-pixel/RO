#include "precompiled_gameplay.h"

#ifndef _ITF_TRAJECTORYSPAWNERCOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectorySpawnerComponent.h"
#endif //_ITF_TRAJECTORYSPAWNERCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_TRAJECTORYFOLLOWERCOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryFollowerComponent.h"
#endif //_ITF_TRAJECTORYFOLLOWERCOMPONENT_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TrajectorySpawnerComponent)
BEGIN_SERIALIZATION_CHILD(TrajectorySpawnerComponent)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("spawneePaths",  m_spawneePaths);
    END_CONDITION_BLOCK()
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(TrajectorySpawnerComponent)
    VALIDATE_COMPONENT_PARAM("", m_trajectoryNodeComponent, "TrajectoryNodeComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
TrajectorySpawnerComponent::TrajectorySpawnerComponent()
    : Super()
    , m_activated(bfalse)
    , m_spawneePaths()
    , m_spawneeGenerators()
    , m_trajectoryNodeComponent(NULL)
{
}

//------------------------------------------------------------------------------
TrajectorySpawnerComponent::~TrajectorySpawnerComponent()
{
}

//------------------------------------------------------------------------------
void TrajectorySpawnerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    // mandatory, validated
    m_trajectoryNodeComponent = m_actor->GetComponent<TrajectoryNodeComponent>();

    // register events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrajectorySpawn,3846946197),this);

    // if set, the template spawnee path takes precedence over the instance one
    if (!getTemplate()->getSpawneePaths().empty())
    {
        m_spawneePaths = getTemplate()->getSpawneePaths();
    }
    
    // initialize spawnee generator
    if (m_spawneePaths.empty())
    {
        ITF_WARNING(
            m_actor, bfalse,
            "Set at least one spawnee path in the PropertiesEditor and reload"
            );
    }
    else
    {
        TrajectorySpawneeList::iterator it;
        for (it = m_spawneePaths.begin(); it != m_spawneePaths.end(); ++it)
        {
            SpawneeGenerator newSpawneeGenerator;
            SPAWNER->declareNeedsSpawnee(m_actor, &newSpawneeGenerator, *it);
            m_spawneeGenerators.push_back(newSpawneeGenerator); // copy
        }
        ITF_ASSERT(m_spawneeGenerators.size() == m_spawneePaths.size());
        m_activated = btrue;
    }
}

//------------------------------------------------------------------------------
void TrajectorySpawnerComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);
    
    // process events
    if (EventTrajectorySpawn* trajectorySpawnEvt = _event->DynamicCast<EventTrajectorySpawn>(ITF_GET_STRINGID_CRC(EventTrajectorySpawn,3846946197)))
    {
        processTrajectorySpawn(trajectorySpawnEvt);
    }
}

//------------------------------------------------------------------------------
void TrajectorySpawnerComponent::processTrajectorySpawn(EventTrajectorySpawn* _trajectorySpawnEvt)
{
    if (!m_activated)
    {
        return;
    }

    ITF_ASSERT(m_spawneeGenerators.size() == m_spawneePaths.size());
    
    // index too large? warn and skip.
    u32 spawneeIndex = _trajectorySpawnEvt->getSpawneeIndex();
    if (spawneeIndex >= m_spawneeGenerators.size())
    {
        ITF_WARNING(
            m_actor, bfalse,
            "No spawnee defined at index %d",
            spawneeIndex
            );
        return;
    }
    
    // spawn
    Actor* spawnee = m_spawneeGenerators[spawneeIndex].getSpawnee(m_actor->getScene(), m_actor->getPos());
    if (!spawnee)
    {
        ITF_WARNING(
            m_actor, bfalse,
            "Can't spawn '%s'",
            m_spawneePaths[spawneeIndex].getString8().cStr()
            );
        m_activated = bfalse;
        return;
    }
    
    // get follower component
    TrajectoryFollowerComponent* followerComponent = spawnee->GetComponent<TrajectoryFollowerComponent>();
    if (!followerComponent)
    {
        ITF_WARNING(
            m_actor, bfalse,
            "Actor '%s' has no TrajectoryFollowerComponent, this won't work",
            m_spawneePaths[spawneeIndex].getString8().cStr()
            );
        spawnee->requestDestruction();
        m_activated = bfalse;
        return;
    }
    
    // flip spawnee if spawner is flipped (RO-6516)
    spawnee->setFlipped(m_actor->isFlipped());
    
    // start following
    followerComponent->activate(m_trajectoryNodeComponent);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TrajectorySpawnerComponent_Template)
BEGIN_SERIALIZATION(TrajectorySpawnerComponent_Template)
    SERIALIZE_MEMBER("spawneePaths", m_spawneePaths);
END_SERIALIZATION()

//------------------------------------------------------------------------------
TrajectorySpawnerComponent_Template::TrajectorySpawnerComponent_Template()
    : Super()
    , m_spawneePaths()
{
}

//------------------------------------------------------------------------------
TrajectorySpawnerComponent_Template::~TrajectorySpawnerComponent_Template()
{
}

} // namespace ITF
