#ifndef _ITF_RAY_BUBBLESPAWNERCOMPONENT_H_
#define _ITF_RAY_BUBBLESPAWNERCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif // _ITF_SPAWNER_H_

namespace ITF
{
    class Ray_BubbleSpawnerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BubbleSpawnerComponent, ActorComponent,4097717947)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BubbleSpawnerComponent();
        virtual ~Ray_BubbleSpawnerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onEvent( Event* _event );

    private:
        ITF_INLINE const class Ray_BubbleSpawnerComponent_Template* getTemplate() const;

        void spawn();
        SpawneeGenerator m_bubbleSpawner;

        // instance-serialized
        f32 m_bubbleLifetime;
        f32 m_bubbleFloatForceX;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_BubbleSpawnerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BubbleSpawnerComponent_Template, TemplateActorComponent,3018169251);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BubbleSpawnerComponent);

    public:

        Ray_BubbleSpawnerComponent_Template()
        {
        }
        virtual ~Ray_BubbleSpawnerComponent_Template() {}

        ITF_INLINE const Path& getBubblePath() const { return m_bubblePath; }

    private:

        Path m_bubblePath;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_BubbleSpawnerComponent_Template* Ray_BubbleSpawnerComponent::getTemplate() const
    {
        return static_cast<const Ray_BubbleSpawnerComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BUBBLESPAWNERCOMPONENT_H_