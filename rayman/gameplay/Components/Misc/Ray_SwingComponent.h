#ifndef _ITF_RAY_SWINGCOMPONENT_H_
#define _ITF_RAY_SWINGCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

#define MAX_SWINGMAN_ARMS 4

namespace ITF
{

    class Ray_SwingComponent : public ActorComponent, IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SwingComponent, ActorComponent,3074882342)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_SwingComponent();
        virtual ~Ray_SwingComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _delta );
        virtual void        onEvent( Event* _event );
        virtual void        onResourceReady();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        updateAnimInput();

        void                setPlayerGraspPoint( const Vec2d& _point, ActorRef _actor );

    protected:
        ITF_INLINE const class Ray_SwingComponent_Template * getTemplate() const;

        struct ActorMem
        {
            ActorRef        m_ref;
            f32             m_counter;
        };

        enum EArmAnimState
        {
            ARMSTATE_NONE,
            ARMSTATE_SLEEP,
            ARMSTATE_WAKING,
            ARMSTATE_STANDING,
            ARMSTATE_STRETCHING,
            ARMSTATE_STRETCH,
            ARMSTATE_UNSTRETCHING,
        };

        struct ArmData
        {
            void            resetGrasp() { m_graspPoint = Vec2d::Zero; m_graspTargetCounter = 0.f; }
            bbool           isValid() const { return m_animState != ARMSTATE_NONE; }

            EArmAnimState   m_animState;                // The state of the anim
            ActorRef        m_target;                   // Actor to point
            f32             m_currentLength;            // The current length of the arm
            f32             m_currentAngle;             // The current angle of the arm
            f32             m_lengthVel;                // The speed of the length increasing/decreasing
            f32             m_angularVel;               // The angular speed
            Vec2d           m_graspPoint;               // The target point of the arm when the player is swinging
            f32             m_graspTargetCounter;       // A timer to interpolate towards the grasp point
            f32             m_realLength;               // The final length
            f32             m_realAngle;                // The final angle
            bbool           m_attached;                 // If a player is attached to it
        };

        bbool                               isInMem( ActorRef _ref ) const;
        void                                checkNewSwing();
        void                                updateMemory( f32 _dt );
        void                                updateArms( f32 _dt );
        void                                updateAttachedArm( ArmData& _arm, u32 _index, f32 _dt );
        void                                updatePointingArm( ArmData& _arm, u32 _index, f32 _dt, bbool _useClamp );
        void                                updateIdleArm( ArmData& _arm, u32 _index, f32 _dt );
        void                                updateArmAnimState( ArmData& _arm );

        void                                clampAngleInRange( f32& _angle ) const;

        bbool                               isActorInDetectRange( Actor* _actor ) const;

        void                                processDetachEvent( class Ray_EventSwingDetach* _event );
        void                                processAnimEvent( class AnimGameplayEvent* _event );
        void                                processArmTransitionFinished( u32 _armIndex );

        class PlayerDetectorComponent*      m_playerDetectorComponent;
        class AnimatedComponent*            m_animatedComponent;
        FixedArray <ActorMem,s_maxPlayers>  m_memPlayers;
        SafeArray <ArmData>                 m_arms;
        bbool                               m_asleep;

        enum ESwingAnimInput
        {
            ARM_STATE,
            ARM_ANGLE,
            ARM_PRESENT,
            ARM_MAX_INPUT,
        };

        u32 m_armAnimInputIndex[ARM_MAX_INPUT][MAX_SWINGMAN_ARMS];
        u32 m_armAnimInputSleep;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_SwingComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SwingComponent_Template,TemplateActorComponent,190444179);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SwingComponent);

    public:

        Ray_SwingComponent_Template();
        ~Ray_SwingComponent_Template() {}

        f32                 getLength() const { return m_length; }
        const Angle&        getAngle() const { return m_angle; }
        f32                 getGravMultiplier() const { return m_gravMultiplier; }
        f32                 getMemoryDuration() const { return m_memoryLength; }
        f32                 getPlayerDetectRange() const { return m_playerDetectRange; }
        f32                 getArmsDistance() const { return m_armsDistance; }
        f32                 getArmsLengthStiff() const { return m_armsLengthStiff; }
        f32                 getArmsLengthDamp() const { return m_armsLengthDamp; }
        f32                 getArmsAngleStiff() const { return m_armsAngleStiff; }
        f32                 getArmsAngleDamp() const { return m_armsAngleDamp; }
        f32                 getArmsGraspInterpolateTime() const { return m_armsGraspInterpolateTime; }
        u32                 getNumArms() const { return m_numArms; }
        u32                 getNumRestAngles() const { return m_restAngles.size(); }
        f32                 getRestAngle( u32 _index ) const { return m_restAngles[_index].ToRadians(); }

    private:

        f32                 m_length;
        Angle               m_angle;
        f32                 m_gravMultiplier;
        f32                 m_memoryLength;
        f32                 m_playerDetectRange;
        f32                 m_armsDistance;
        f32                 m_armsLengthStiff;
        f32                 m_armsLengthDamp;
        f32                 m_armsAngleStiff;
        f32                 m_armsAngleDamp;
        f32                 m_armsGraspInterpolateTime;
        u32                 m_numArms;
        SafeArray <Angle>   m_restAngles;
    };

    ITF_INLINE const class Ray_SwingComponent_Template * Ray_SwingComponent::getTemplate() const
    {
        return static_cast<const class Ray_SwingComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_SWINGCOMPONENT_H_
