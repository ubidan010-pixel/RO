#ifndef _ITF_RAY_ABYSSALHANDAICOMPONENT_H_
#define _ITF_RAY_ABYSSALHANDAICOMPONENT_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_
namespace ITF
{
    class PlayerDetectorComponent;
    class Ray_AbyssalHandAIComponent_Template;

    //------------------------------------------------------------------
    class Ray_AbyssalHandAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AbyssalHandAIComponent, ActorComponent,4177230497)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_AbyssalHandAIComponent();
        virtual ~Ray_AbyssalHandAIComponent();

        virtual bbool                   needsUpdate() const { return btrue; }
        virtual bbool                   needsDraw() const { return btrue; }
        virtual bbool                   needsDraw2D() const { return bfalse; }

        virtual void                    onActorLoaded(Pickable::HotReloadType _hotReload);
        //virtual void                    onActorClearComponents();
        virtual void                    onBecomeActive();
        //virtual void                    onBecomeInactive();
        virtual void                    Update( f32 _dt );
        virtual void                    Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void                    drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags ) const;
        virtual void                    onEditorMove(bbool _modifyInitialPos = btrue);
#endif //ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const Ray_AbyssalHandAIComponent_Template * getTemplate() const;


        //init data from position
        void initDataFromPosition();

        //player detection
        void updatePlayerDetection();

        //light detection
        bbool collideWithLight(const bbool _far) const;
        bbool checkLightPresence(const bbool _far) const;

        //hand state management       
        enum AbyssalHandState
        {
            AbyssalHandState_UNDEFINED = -1,
            AbyssalHandState_IDLE,
            AbyssalHandState_FORWARD,
            AbyssalHandState_FLEE,
            AbyssalHandState_BACKWARD,
            AbyssalHandState_ATTACK,
            ENUM_FORCE_SIZE_32(AbyssalHandState)
        };
        void updateState(const f32 _dt);
        void changeState(const AbyssalHandState _state) ;
        void startIdle();
        void updateIdle();
        void startForward();
        void updateForward(const f32 _dt);
        void startBackward();
        void updateBackward(const f32 _dt);
        void startFlee();
        void updateFlee(const f32 _dt);
        void startAttack();
        void updateAttack();

        //curve management
        void initCurve();
        void updateCurve();

        //orientation & position management
        void updateOrientation(const f32 _dt);
        void setHandPosition(const Vec2d & _position);
        void updateHandSpeed(const f32 _dt);

        //player stim
        void sendStim();

        //Data
    private:
        //components
        AnimLightComponent * m_animLightComponent;
        //curve
        BezierCurve m_curve;
        BezierCurveRenderer m_curveRenderer;
        //initial position
        Vec2d m_initialPosition;
        //current position
        Vec2d m_handPosition;
        //arm length
        f32 m_length;
        //root orientation
        Vec2d m_rootOrientation;
        //hand orientation
        Vec2d m_handOrientation;
        Vec2d m_handTargetOrientation;
        //hand speed
        f32 m_handCurrentSpeed;
        f32 m_handTargetSpeed;
        //target
        Actor * m_closestPlayer;
        Vec2d m_closestPlayerPosition;
        Vec2d m_closestPlayerDirection;
        f32 m_closestPlayerDistance;
        f32 m_timeSinceLastStim;
        //state 
        AbyssalHandState m_handState;
        f32 m_handStateTimer;
        //detection AABB
        PhysShapePolygon m_detectionShape;
    };





    //------------------------------------------------------------------
    class Ray_AbyssalHandAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AbyssalHandAIComponent_Template,TemplateActorComponent,959313734);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AbyssalHandAIComponent);

    public:

        Ray_AbyssalHandAIComponent_Template();
        virtual ~Ray_AbyssalHandAIComponent_Template();

        ITF_INLINE f32 getHandForwardSpeed() const;
        ITF_INLINE f32 getHandBackwardSpeed() const;
        ITF_INLINE f32 getHandEscapeSpeed() const;
        ITF_INLINE f32 getHandAcceleration() const;
        ITF_INLINE const BezierCurveRenderer_Template& getBezierRenderer() const;
        ITF_INLINE const PhysShapePolygon & getDetectionShape() const;
        ITF_INLINE f32 getHandLightDistance() const;
        ITF_INLINE const Angle & getHandRotationSpeed() const;
        ITF_INLINE const StringID & getEndCurveBoneName() const;
        ITF_INLINE f32 getStimDelay() const;
        ITF_INLINE f32 getStimDistance() const;
        ITF_INLINE f32 getIdleMinTime() const;
        //tangents parameters
        ITF_INLINE f32 getRootTangentFactor() const;
        ITF_INLINE f32 getHandTangentFactor() const;
        ITF_INLINE f32 getTangentMaxFactorLength() const;
        ITF_INLINE f32 getTangentNullFactorLength() const;

        //Data
    private:
        BezierCurveRenderer_Template m_bezierRenderer;
        f32 m_handForwardSpeed;
        f32 m_handBackwardSpeed;
        f32 m_handEscapeSpeed;
        f32 m_handAcceleration;
        PhysShapePolygon m_detectionShape;
        f32 m_handLightDistance;
        Angle m_handRotationSpeed;
        StringID m_endCurveBoneName;
        f32 m_stimDelay;
        f32 m_stimDistance;
        f32 m_idleMinTime;
        //tangents parameters
        f32 m_rootTangentFactor;
        f32 m_handTangentFactor;
        f32 m_tangentMaxFactorLength;
        f32 m_tangentNullFactorLength;
    };

    ITF_INLINE const Ray_AbyssalHandAIComponent_Template* Ray_AbyssalHandAIComponent::getTemplate() const
    {
        return static_cast<const Ray_AbyssalHandAIComponent_Template*>(m_template);
    }


    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getHandForwardSpeed() const
    {
        return m_handForwardSpeed;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getHandBackwardSpeed() const
    {
        return m_handBackwardSpeed;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getHandEscapeSpeed() const
    {
        return m_handEscapeSpeed;
    }
    
    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getHandAcceleration() const
    {
        return m_handAcceleration;
    }

    ITF_INLINE const BezierCurveRenderer_Template& Ray_AbyssalHandAIComponent_Template::getBezierRenderer() const
    { 
        return m_bezierRenderer; 
    }

    ITF_INLINE const PhysShapePolygon & Ray_AbyssalHandAIComponent_Template::getDetectionShape() const
    {
        return m_detectionShape;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getHandLightDistance() const
    {
        return m_handLightDistance;
    }
        
    ITF_INLINE const Angle & Ray_AbyssalHandAIComponent_Template::getHandRotationSpeed() const
    {
        return m_handRotationSpeed;
    }

    ITF_INLINE const StringID & Ray_AbyssalHandAIComponent_Template::getEndCurveBoneName() const
    {
        return m_endCurveBoneName;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getStimDelay() const
    {
        return m_stimDelay;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getStimDistance() const
    {
        return m_stimDistance;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getIdleMinTime() const
    {
        return m_idleMinTime;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getRootTangentFactor() const
    {
        return m_rootTangentFactor;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getHandTangentFactor() const
    {
        return m_handTangentFactor;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getTangentMaxFactorLength() const
    {
        return m_tangentMaxFactorLength;
    }

    ITF_INLINE f32 Ray_AbyssalHandAIComponent_Template::getTangentNullFactorLength() const
    {
        return m_tangentNullFactorLength;
    }

}

#endif //_ITF_RAY_ABYSSALHANDAICOMPONENT_H_
