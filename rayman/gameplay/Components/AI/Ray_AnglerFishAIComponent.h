#ifndef _ITF_RAY_ANGLERFISHAICOMPONENT_H_
#define _ITF_RAY_ANGLERFISHAICOMPONENT_H_

#ifndef _ITF_RAY_SIMPLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SimpleAIComponent.h"
#endif //_ITF_RAY_SIMPLEAICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{

    class AIBehavior;
    class AIReceiveHitBehavior;

    class Ray_AnglerFishAIComponent : public Ray_SimpleAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AnglerFishAIComponent,Ray_SimpleAIComponent,418892941)
        DECLARE_SERIALIZE();

    public:

        Ray_AnglerFishAIComponent() : Super(), m_boneIndex( U32_INVALID ) {}
        virtual ~Ray_AnglerFishAIComponent() {}

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBehaviorFinished();
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        Update( f32 _dt );

    protected:

        ITF_INLINE const class Ray_AnglerFishAIComponent_Template * getTemplate() const;

        virtual void        startDeath();
        //virtual void        processActivate( EventActivate * onActivate );
    private:

        SpawneeGenerator    m_lightSpawner;
        ActorRef            m_lightActor;
        u32                 m_boneIndex;
    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class TemplateAIBehavior;
    class AIReceiveHitBehavior_Template;
    class TemplateAIBehavior;

    class Ray_AnglerFishAIComponent_Template : public Ray_SimpleAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AnglerFishAIComponent_Template,Ray_SimpleAIComponent_Template,2006657463);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AnglerFishAIComponent);

    public:

        Ray_AnglerFishAIComponent_Template() : Super(), m_deathLightStopDelay( 1.0f ) {}
        virtual ~Ray_AnglerFishAIComponent_Template() {}

        ITF_INLINE const StringID &     getSpawnBone() const { return m_spawnBone; }
        ITF_INLINE const ITF::Path &    getLightSpawnPath() const { return m_lightSpawnPath; }
        ITF_INLINE const f32 &          getDeathLightStopDelay() const { return m_deathLightStopDelay; }
    private:
        StringID    m_spawnBone;
        Path        m_lightSpawnPath;
        f32         m_deathLightStopDelay;
    };

    ITF_INLINE const class Ray_AnglerFishAIComponent_Template * Ray_AnglerFishAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_AnglerFishAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_ANGLERFISHAICOMPONENT_H_

