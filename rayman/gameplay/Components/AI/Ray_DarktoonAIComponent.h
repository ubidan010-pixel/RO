#ifndef _ITF_RAY_DARKTOONAICOMPONENT_H_
#define _ITF_RAY_DARKTOONAICOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

namespace ITF
{

    class Ray_DarkToonAIComponent : public Ray_GroundAIComponent
    {

        DECLARE_OBJECT_CHILD_RTTI(Ray_DarkToonAIComponent,Ray_GroundAIComponent,3931431627)

    public:
        DECLARE_SERIALIZE()

        Ray_DarkToonAIComponent();
        ~Ray_DarkToonAIComponent();

        virtual void Update( f32 _deltaTime );
        virtual void onEvent(class Event * _event);
        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void onBehaviorFinished();

    protected:
        ITF_INLINE const class Ray_DarkToonAIComponent_Template * getTemplate() const;
        void        startDrowning(ObjectRef _poly);
        void        receiveWind( WindStim* _wind );
        void        processLaunchQuery(Ray_EventQueryChildLaunch* _query);
        void        processLaunch(Ray_EventChildLaunch* _launch);

        virtual bbool isSquashAuthorized() const;

        class Ray_AIWaterBaseBehavior *         m_waterBehavior;
        class Ray_AILivingStoneWindBehavior *   m_windBehavior;
        class Ray_AIDarktoonLaunchBehavior *    m_launchBehavior;

    };

    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_DarkToonAIComponent_Template : public Ray_GroundAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DarkToonAIComponent_Template, Ray_GroundAIComponent_Template,4087876507);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DarkToonAIComponent);

    public:

        Ray_DarkToonAIComponent_Template();
        ~Ray_DarkToonAIComponent_Template();

        ITF_INLINE const class Ray_AIWaterBaseBehavior_Template * getWaterBehavior() const { return m_waterBehavior; }
        ITF_INLINE const class Ray_AILivingStoneWindBehavior_Template * getWindBehavior() const { return m_windBehavior; }
        ITF_INLINE const class Ray_AIDarktoonLaunchBehavior_Template * getLaunchBehavior() const { return m_launchBehavior; }

    private:
        class Ray_AIWaterBaseBehavior_Template *         m_waterBehavior;
        class Ray_AILivingStoneWindBehavior_Template *   m_windBehavior;
        class Ray_AIDarktoonLaunchBehavior_Template *    m_launchBehavior;
    };

    ITF_INLINE const Ray_DarkToonAIComponent_Template * Ray_DarkToonAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_DarkToonAIComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_DARKTOONAICOMPONENT_H_
