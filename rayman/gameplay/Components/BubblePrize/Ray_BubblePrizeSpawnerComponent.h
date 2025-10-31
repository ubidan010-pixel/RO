#ifndef _ITF_RAY_BUBBLEPRIZESPAWNERCOMPONENT_H_
#define _ITF_RAY_BUBBLEPRIZESPAWNERCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#define STATE_DISABLED      0
#define STATE_SPAWNING      1
#define STATE_SPAWNED       2

namespace ITF
{
    class Ray_BubblePrizeSpawnerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BubblePrizeSpawnerComponent,ActorComponent,1701732717)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BubblePrizeSpawnerComponent();
        virtual ~Ray_BubblePrizeSpawnerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event);
        virtual void        Update( f32 _dt );

    private:
                void        receiveHit();
                void        spawnBubble();

    private:

        ITF_INLINE const class Ray_BubblePrizeSpawnerComponent_Template*  getTemplate() const;

        f32                 m_time;
        SpawneeGenerator    m_bubbleSpawner;

        u32                 m_curCountBubbleSpawned;
        bbool               m_alreadyHit;
        u32                 m_state;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_BubblePrizeSpawnerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BubblePrizeSpawnerComponent_Template,TemplateActorComponent,2131467178);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BubblePrizeSpawnerComponent);

    public:

        Ray_BubblePrizeSpawnerComponent_Template();
        ~Ray_BubblePrizeSpawnerComponent_Template();

        ITF_INLINE const Path&  getBubblePath() const               { return m_bubblePath; }
        ITF_INLINE u32          getBubbleCount() const              { return m_bubbleCount; }
        ITF_INLINE f32          getTimeBetweenBubble() const        { return m_timeBetweenBubble; }
        ITF_INLINE u32          getInternalRewardNumber() const     { return m_internalRewardNumber; }
        ITF_INLINE bbool        getSpawnBubbleOnDeath() const       { return m_spawnBubbleOnDeath; }

    private:

        u32     m_bubbleCount;
        f32     m_timeBetweenBubble;
        Path    m_bubblePath;
        bbool   m_spawnBubbleOnDeath;
        u32     m_internalRewardNumber;            

    };



    ITF_INLINE const Ray_BubblePrizeSpawnerComponent_Template*  Ray_BubblePrizeSpawnerComponent::getTemplate() const {return static_cast<const Ray_BubblePrizeSpawnerComponent_Template*>(m_template);}


}

#endif // _ITF_RAY_BUBBLEPRIZESPAWNERCOMPONENT_H_

