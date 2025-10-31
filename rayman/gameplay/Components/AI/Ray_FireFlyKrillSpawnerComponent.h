#ifndef _ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_
#define _ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif // _ITF_SPAWNER_H_

namespace ITF
{
    //forward
    class Ray_FireFlyKrillSpawnerComponent_Template;
    class Ray_FireFlyKrillAIComponent;

    class Ray_FireFlyKrillSpawnerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FireFlyKrillSpawnerComponent, ActorComponent,3215081539)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:
        //static members used to manage krill <-> spawner detection
        static bbool getSpawnerInRange(const Vec2d & _position, const f32 _radius, Vec2d & _spawnerPosition);

        Ray_FireFlyKrillSpawnerComponent();
        
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onActorClearComponents();
        virtual void onBecomeActive();
        virtual void onBecomeInactive();
        virtual void Update(f32 _dt);

    private :        
        //static members used to manage krill <-> spawner detection
        static void registerSpawner(const ActorRef &_spawner);
        static void unregisterSpawner(const ActorRef &_spawner);

		//get actor template
		ITF_INLINE const Ray_FireFlyKrillSpawnerComponent_Template * const getTemplate() const;

        //instance registration management
        void registerInstance();
        void unregisterInstance();

        //krill actors management
        void spawnKrill();

        //players detection
        void updatePlayerDetection();

        //Data
    private:
        //static members used to manage krill <-> spawner detection
        static  ActorRefList ms_spawners;

        //spawner
        SpawneeGenerator m_krillSpawner;

        //docked krill actor ref
        ActorRef m_dockedKrill;

        //detected players
        ActorRefList m_detectedPlayers;

        //registered flag
        bbool m_instanceRegistered;
    };

    class Ray_FireFlyKrillSpawnerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FireFlyKrillSpawnerComponent_Template, TemplateActorComponent,3115126182);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FireFlyKrillSpawnerComponent);

    public:
        Ray_FireFlyKrillSpawnerComponent_Template();
        ~Ray_FireFlyKrillSpawnerComponent_Template();

        ITF_INLINE const Path & getKrillActorPath() const;
        ITF_INLINE f32 getPlayerDetectionRadius() const;

        //Data
    private:
        Path m_krillActorPath;
        f32 m_playerDetectionRadius;
    };


    ITF_INLINE const Ray_FireFlyKrillSpawnerComponent_Template * const Ray_FireFlyKrillSpawnerComponent::getTemplate() const
    {
        return static_cast<const Ray_FireFlyKrillSpawnerComponent_Template * const>( m_template );
    }


    ITF_INLINE const Path & Ray_FireFlyKrillSpawnerComponent_Template::getKrillActorPath() const
    {
        return m_krillActorPath;
    }

    ITF_INLINE f32 Ray_FireFlyKrillSpawnerComponent_Template::getPlayerDetectionRadius() const
    {
        return m_playerDetectionRadius;
    }
}

#endif //_ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_