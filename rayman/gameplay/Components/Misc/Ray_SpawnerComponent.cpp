#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SpawnerComponent.h"
#endif //_ITF_RAY_SPAWNERCOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_SpawnerComponent_Template)
    BEGIN_SERIALIZATION(Ray_SpawnerComponent_Template)
        SERIALIZE_MEMBER("actor",m_path);
        SERIALIZE_MEMBER("numToSpawn",m_numToSpawn);
        SERIALIZE_MEMBER("ejectionForce",m_ejectionForce);
        SERIALIZE_MEMBER("ejectionRandomForce",m_ejectionRandomForce);
        SERIALIZE_MEMBER("ejectionAngle",m_ejectionAngle);
        SERIALIZE_MEMBER("ejectionRandomAngle",m_ejectionRandomAngle);
        SERIALIZE_MEMBER("ejectionFrequency",m_ejectionFrequency);
    END_SERIALIZATION()

        Ray_SpawnerComponent_Template::Ray_SpawnerComponent_Template()
        : m_numToSpawn(5)
        , m_ejectionForce(1000.0f)
        , m_ejectionRandomForce(100.0f)
        , m_ejectionAngle(btrue,90.0f)
        , m_ejectionRandomAngle(btrue,10.0f)
        , m_ejectionFrequency(5.0f)
    {
    }

    Ray_SpawnerComponent_Template::~Ray_SpawnerComponent_Template()
    {
    }

IMPLEMENT_OBJECT_RTTI(Ray_SpawnerComponent)

BEGIN_SERIALIZATION_CHILD(Ray_SpawnerComponent)
END_SERIALIZATION()

Ray_SpawnerComponent::Ray_SpawnerComponent()
: m_startSpawning(bfalse)
, m_timer(0.0f)
{
}

Ray_SpawnerComponent::~Ray_SpawnerComponent()
{
    clear();
}

void Ray_SpawnerComponent::clear()
{
    m_timer = 0.0f;
    m_spawns.clear();
}

void Ray_SpawnerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443),this);
    SPAWNER->declareNeedsSpawnee(m_actor, &m_spawner, getTemplate()->getPath());
}

void Ray_SpawnerComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    if (m_startSpawning)
    {
        if (m_timer == 0.0f)
        {
            Actor * spawn = m_spawner.getSpawnee(m_actor->getScene(), m_actor->getPos(), m_actor->getAngle());
            if (spawn)
            {
                Vec2d force = Vec2d::Right.Rotate(getTemplate()->getEjectionAngle().Radians() + m_actor->getAngle() + Seeder::getSharedSeeder().GetFloat(-getTemplate()->getEjectionRandomAngle().Radians(),getTemplate()->getEjectionRandomAngle().Radians()));
                force *= getTemplate()->getEjectionForce() +Seeder::getSharedSeeder().GetFloat(-getTemplate()->getEjectionRandomForce(),getTemplate()->getEjectionRandomForce());
                EventAddForce addForce(force);
                spawn->onEvent(&addForce);
                Ray_EventSpawn onSpawn;
                spawn->onEvent(&onSpawn);
                m_spawns.push_back(spawn->getRef());

                if (m_spawns.size() < getTemplate()->getNumToSpawn())
                {
                    m_timer = 1.0f/getTemplate()->getEjectionFrequency();
                }
                else
                {
                    m_startSpawning = bfalse;
                }
            }
        }
        m_timer = Max(0.0f,m_timer - _deltaTime);
    }

}

void Ray_SpawnerComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);

    if (EventTrigger * onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if (onTrigger->getActivated())
        {
            m_spawns.clear();   // TODO: can we be activated several times?
            m_startSpawning = btrue;
            m_timer = 1.0f / getTemplate()->getEjectionFrequency();
        }
    }
    else if ( EventQueryIsDead* query = _event->DynamicCast<EventQueryIsDead>(ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443)) )
    {
        processQueryIsDead(query);
    }
}

    void Ray_SpawnerComponent::processQueryIsDead( EventQueryIsDead* query )
    {
        // TODO: there's a catch here, keeping references to spawns is dangerous:
        // if the spawn has be recycled and used by another spawner, the player will have to kill him twice!

        if (m_spawns.size() == getTemplate()->getNumToSpawn())
        {
            for (u32 i = 0; i < m_spawns.size(); i++)
            {
                Actor* spawn = m_spawns[i].getActor();
                if (spawn && spawn->isSpawned() && spawn->isEnabled())
                {
                    EventQueryIsDead query;
                    spawn->onEvent(&query);
                    if ( !query.getIsDead() )
                    {
                        return;
                    }
                }
            }

            // if we're here, none of the spawns are active...
            query->setIsDead(btrue);
        }
        // else: we haven't even spawned everyone yet
    }

} // namespace ITF