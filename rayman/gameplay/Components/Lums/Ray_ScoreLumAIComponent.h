#ifndef _ITF_RAY_SCORELUMAICOMPONENT_H_
#define _ITF_RAY_SCORELUMAICOMPONENT_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef __ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_PickedLumTrajectoryFollower.h"
#endif //__ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF
{

    class Ray_ScoreLumAIComponent : public Ray_FixedAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ScoreLumAIComponent, Ray_FixedAIComponent,2472225950)
        DECLARE_SERIALIZE()

    public:
        Ray_ScoreLumAIComponent() {}

        ~Ray_ScoreLumAIComponent();

        ITF_INLINE const class Ray_ScoreLumAIComponent_Template * getTemplate() const;

        Ray_PickedLumTrajectoryFollower& pushLumTrajectoryFollower(const Ray_PickedLumTrajectoryFollower& _lum);
        virtual void        onEvent( Event* _event);
        void playJoinSound(bbool _joins);

    private:
        virtual bbool       needsDraw2D() const { return btrue; }
        virtual void        Draw2D();
        virtual void        onActorLoaded(Pickable::HotReloadType _hotReloadType);
        void                sendLumReachesScoreLum(Ray_PickedLumTrajectoryFollower &_lum);
        void                flushLumParticles();
        void                reachScoreLumIfNotAlreadyDone(Ray_PickedLumTrajectoryFollower &_lum);

        virtual void        Update( f32 _deltaTime );
        virtual void        onCheckpointLoaded();

        typedef ITF_VECTOR<Ray_PickedLumTrajectoryFollower> FakeLumContainer_t;
        FakeLumContainer_t m_currentLumParticles;
        AtlasDynamicObject m_lumAtlas;

    };

    class Ray_ScoreLumAIComponent_Template : public Ray_FixedAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ScoreLumAIComponent_Template,Ray_FixedAIComponent_Template,3713735267);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ScoreLumAIComponent);

    public:

        Ray_ScoreLumAIComponent_Template() : Ray_FixedAIComponent_Template() , m_framePerSecond(30) {}
        ~Ray_ScoreLumAIComponent_Template() {}

        const Path &getLumAtlasPath() const {return m_lumAtlasPath;}
        f32  getFramePerSecond() const {return m_framePerSecond;}

    private:
        Path m_lumAtlasPath;
        f32  m_framePerSecond;
    };

    ITF_INLINE const class Ray_ScoreLumAIComponent_Template * Ray_ScoreLumAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_ScoreLumAIComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_SCORELUMAICOMPONENT_H_