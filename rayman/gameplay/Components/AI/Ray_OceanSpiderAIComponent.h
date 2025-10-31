#ifndef _ITF_RAY_OCEANSPIDERAICOMPONENT_H_
#define _ITF_RAY_OCEANSPIDERAICOMPONENT_H_

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

    class Ray_OceanSpiderAIComponent : public Ray_SimpleAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_OceanSpiderAIComponent,Ray_SimpleAIComponent,2810138667)
        DECLARE_SERIALIZE();

    public:

        Ray_OceanSpiderAIComponent() : Super(), m_hitAddAnimTimer( 99.f ) {}
        virtual ~Ray_OceanSpiderAIComponent() {}

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBehaviorFinished();
        virtual void        updateAnimInput();
        virtual void        Update( f32 _dt );

    protected:

        ITF_INLINE const class Ray_OceanSpiderAIComponent_Template * getTemplate() const;

        virtual void        startDeath();
        virtual void        processActivate( EventActivate * onActivate );
        virtual void        processHit( HitStim* _hit );
    private:

        //SpawneeGenerator m_spawner;
        f32 m_hitAddAnimTimer;
    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class TemplateAIBehavior;
    class AIReceiveHitBehavior_Template;
    class TemplateAIBehavior;

    class Ray_OceanSpiderAIComponent_Template : public Ray_SimpleAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_OceanSpiderAIComponent_Template,Ray_SimpleAIComponent_Template,3273024315);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_OceanSpiderAIComponent);

    public:

        Ray_OceanSpiderAIComponent_Template() : Super() {}
        virtual ~Ray_OceanSpiderAIComponent_Template()  {}

        //ITF_INLINE const SafeArray<StringID> & getBones() const { return m_bones; }
        //ITF_INLINE const ITF::Path & getSpawnPath() const { return m_spawnPath; }
        ITF_INLINE const StringID & getaddHitInputName() const { return m_addHitInputName; }
    private:
        //SafeArray<StringID> m_bones;
        //Path m_spawnPath;
        StringID m_addHitInputName;
    };

    ITF_INLINE const class Ray_OceanSpiderAIComponent_Template * Ray_OceanSpiderAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_OceanSpiderAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_OCEANSPIDERAICOMPONENT_H_

