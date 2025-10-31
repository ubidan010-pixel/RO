#ifndef _ITF_RAY_SPAWNERCOMPONENT_H_
#define _ITF_RAY_SPAWNERCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif


namespace ITF
{
    class Ray_SpawnerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpawnerComponent, ActorComponent,1510308342)

    public:
        DECLARE_SERIALIZE()

        Ray_SpawnerComponent();
        virtual ~Ray_SpawnerComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _deltaTime );
        virtual void                onEvent(Event* _event);

    private:
        ITF_INLINE const class Ray_SpawnerComponent_Template * getTemplate() const;

        void        clear();
        void        processQueryIsDead( EventQueryIsDead* query );

        SpawneeGenerator m_spawner;

        bbool       m_startSpawning;
        f32         m_timer;
        ActorRefList m_spawns;
    };

    class Ray_SpawnerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpawnerComponent_Template,TemplateActorComponent,1732933546);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SpawnerComponent);

    public:

        Ray_SpawnerComponent_Template();
        ~Ray_SpawnerComponent_Template();


        ITF_INLINE ITF::Path getPath() const { return m_path; }
        ITF_INLINE ITF::u32 getNumToSpawn() const { return m_numToSpawn; }
        ITF_INLINE ITF::f32 getEjectionForce() const { return m_ejectionForce; }
        ITF_INLINE ITF::f32 getEjectionRandomForce() const { return m_ejectionRandomForce; }
        ITF_INLINE ITF::Angle getEjectionAngle() const { return m_ejectionAngle; }
        ITF_INLINE ITF::Angle getEjectionRandomAngle() const { return m_ejectionRandomAngle; }
        ITF_INLINE ITF::f32 getEjectionFrequency() const { return m_ejectionFrequency; }
    private:
        Path        m_path;
        u32         m_numToSpawn;
        f32         m_ejectionForce;
        f32         m_ejectionRandomForce;
        Angle       m_ejectionAngle;
        Angle       m_ejectionRandomAngle;
        f32         m_ejectionFrequency;
    };
    ITF_INLINE const class Ray_SpawnerComponent_Template * Ray_SpawnerComponent::getTemplate() const
    {
        return static_cast<const class Ray_SpawnerComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_SPAWNERCOMPONENT_H_