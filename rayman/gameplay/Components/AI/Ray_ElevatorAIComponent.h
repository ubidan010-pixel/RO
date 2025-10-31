#ifndef _ITF_RAY_ELEVATORAICOMPONENT_H_
#define _ITF_RAY_ELEVATORAICOMPONENT_H_

namespace ITF
{

    class AnimatedComponent;
    class SoundComponent;
    class PolylineComponent;

    class Ray_ElevatorAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElevatorAIComponent, ActorComponent,3597187499)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ElevatorAIComponent();
        virtual ~Ray_ElevatorAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onResourceReady();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

                void        addForce(u32 _level);
                Vec2d       getBoneDetectPos();
                void        hit();
                bbool       getLaunchPursuit() const;

    private:
        ITF_INLINE const class Ray_ElevatorAIComponent_Template* getTemplate() const;

        AnimatedComponent*  m_animatedComponent;
        SoundComponent*     m_soundComponent;
        PolylineComponent*  m_polylineComponent;

        f32                 m_currentProgress;
        f32                 m_minClampProgress;
        f32                 m_currentProgressOld;
        f32                 m_force;

        BoneID              m_boneDetect;

        u32                 m_state;
        bbool               m_hitRecieved;
        bbool               m_isJustTopDown;

        Vec2d               m_boneDetectCurPos;
        Vec2d               m_boneDetectPrevPos;

        f32                 m_currentWeight;


		bbool				m_stuckTop;
		f32					m_timeStuckTop;


    };


    //*****************************************************************************
    // Template
    //*****************************************************************************
	
    class Ray_ElevatorAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElevatorAIComponent_Template, TemplateActorComponent,674588016);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ElevatorAIComponent);

    public:

        Ray_ElevatorAIComponent_Template();
        virtual ~Ray_ElevatorAIComponent_Template();

        ITF_INLINE f32 getClampForceMin() const                 { return m_clampForceMin; }
        ITF_INLINE f32 getClampForceMax() const                 { return m_clampForceMax; }
        ITF_INLINE f32 getReboundPercentForce() const           { return m_reboundPercentForce; }
        ITF_INLINE f32 getPercentLaunchMonsterAttack() const    { return m_percentLaunchMonsterAttack; }
        ITF_INLINE f32 getForceGravity() const                  { return m_forceGravity; }
        ITF_INLINE f32 getForceHitMonster() const               { return m_forceHitMonster; }
        ITF_INLINE f32 getDistanceMax() const                   { return m_DistanceMax; }

        ITF_INLINE f32 getForceUpPerHit_Level_0() const                 { return m_forceUpPerHit_Level_0; }
        ITF_INLINE f32 getForceUpPerHit_Level_1() const                 { return m_forceUpPerHit_Level_1; }
        ITF_INLINE f32 getForceUpPerHit_Level_2() const                 { return m_forceUpPerHit_Level_2; }

		ITF_INLINE f32 getTimeStuckTop() const                 { return m_timeStuckTop; }


    private:

        f32 m_clampForceMin;
        f32 m_clampForceMax;

        f32 m_reboundPercentForce;

        f32 m_percentLaunchMonsterAttack;

        f32 m_forceUpPerHit_Level_0;
        f32 m_forceUpPerHit_Level_1;
        f32 m_forceUpPerHit_Level_2;

        f32 m_forceGravity;

        f32 m_forceHitMonster;

        f32 m_DistanceMax; // Change only if the animation change.

		f32 m_timeStuckTop;

    };

    //*****************************************************************************

    ITF_INLINE const Ray_ElevatorAIComponent_Template* Ray_ElevatorAIComponent::getTemplate() const
    {
        return static_cast<const Ray_ElevatorAIComponent_Template*>(m_template);
    }
	
}; // end namespace

#endif //_ITF_RAY_ELEVATORAICOMPONENT_H_