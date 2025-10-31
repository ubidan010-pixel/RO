#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#define _ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_BASECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/BaseCameraComponent.h"
#endif //_ITF_BASECAMERACOMPONENT_H_

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_CURVE_H_
#include "engine/gameplay/Curve.h"
#endif // _ITF_CURVE_H_

#include "core/types.h"

namespace ITF
{
    class CameraControllerData;
    class CameraControllerManager;
    class CameraControllerSubject;

    class InGameCameraComponent : public BaseCameraComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(InGameCameraComponent, BaseCameraComponent,303426032)
        DECLARE_SERIALIZE()

        static InGameCameraComponent* s_instance;

    public:
        static InGameCameraComponent* get() { ITF_ASSERT(s_instance); return s_instance; }

        InGameCameraComponent();
        virtual ~InGameCameraComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);  

        virtual Vec3d       getCameraPosition() const { return m_cameraRealPos; }
        virtual f32         getCameraFocale() const { return m_CMCur.m_focale; }
        virtual f32         getDepth() const { return m_depth; }
        virtual void        teleport(const Vec3d& _newPos);        
        virtual void        onManagerUpdate(f32 _dt);
        CameraModifierComponent *getCameraModifier() const;
        void                updateDrift(f32 _deltaTime);
     
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const CameraControllerData& _data );
#endif //ITF_SUPPORT_DEBUGFEATURE

    protected:
        class SubjectStance 
        {
        public:
            SubjectStance()
                : m_objectRef(ITF_INVALID_OBJREF)
                , m_stick(bfalse)
                , m_lookDir(Vec2d::Zero)                    
                , m_joystickMoveNormalized(Vec2d::Zero)
                , m_justUnstick(bfalse)
                , m_justStick(bfalse)      
                , m_performUTurn(bfalse)
                , m_hang(bfalse)
                , m_wallSlide(bfalse)
                , m_wallSlideLast(bfalse)
                , m_performUTurnLast(bfalse)
                , m_jumpOnAirTimerAllowed(bfalse)
                , m_jumpOnAirTimerAllowedLast(bfalse)
                , m_isPlayer(bfalse)
                , m_isLockedPosition(bfalse)
                , m_wasLockedPosition(bfalse)
                , m_lockedPosition(Vec2d::Zero)
                , m_lockedPositionLast(Vec2d::Zero)
                , m_windForce(Vec2d::Zero)
                , m_leadCamera(bfalse)
            {
            }

            ObjectRef m_objectRef;
            Vec2d   m_lookDir;       
            bbool   m_stick;
            bbool   m_justUnstick;
            bbool   m_justStick;
            bbool   m_hang;
            bbool   m_performUTurn;
            bbool   m_performUTurnLast;
            bbool   m_wallSlide;
            bbool   m_wallSlideLast;
            Vec2d   m_joystickMoveNormalized;
            bbool   m_jumpOnAirTimerAllowed;
            bbool   m_jumpOnAirTimerAllowedLast;
            bbool   m_isPlayer;
            bbool   m_isLockedPosition;
            bbool   m_wasLockedPosition;
            Vec2d   m_lockedPosition;
            Vec2d   m_lockedPositionLast;
            Vec2d   m_windForce;
            bbool   m_leadCamera;

            void    initSubjectStance(); 
            void    resetSubjectStance();

        };

        virtual void setSubjectStance( SubjectStance* /*_player*/ ) {}
        virtual bbool isPlayingPageSequence( ObjectRef _objRef ) { return bfalse; }
        virtual bbool isPlayingShooter( ObjectRef _objRef ) { return bfalse; }

    private:

        const class InGameCameraComponent_Template* getTemplate() const;

        void    projectAABB     ( AABB& _aabb, f32 _originalZ, f32 _newZ);
        void    updateInit      ( f32 _deltaTime);

        // modifiers
        CamModifierUpdate   m_CMCur;
        CamModifierUpdate   m_CMDestCur;
        CamModifierUpdate   m_CMDestFinal;
        CamModifierUpdate   m_CMPrevious;

        class Controller
        {
            public:

            Controller(InGameCameraComponent* _component )             
                : m_component(_component)                
                , m_weight(0.f)
                , m_weightDest(1.f)
                , m_subjectAABB(Vec2d::Zero)
                , m_subjectAABBCenterLast(Vec2d::Zero)                
                , m_subjectSpeed(Vec2d::Zero)
                , m_subjectPosReal(Vec3d::Zero)
                , m_zoneAABB(Vec2d::Zero)
                , m_zoneOffsetFromSubject(Vec2d::Zero)
                , m_zoneSpeedByDepth(Vec2d::Zero)
                , m_zoneSpeedReal(Vec2d::Zero)
                , m_zoneAABBCenterLast(Vec2d::Zero)
                , m_screenBaseAABB(Vec2d::Zero)
                , m_screenRealLimitUp(0.f)
                , m_screenRealLimitUpTargetDist(0.f)
                , m_screenRealLimitUpRatio(0.f)    
                , m_screenRealLimitUpDist(0.f)
                , m_screenRealLimitUpSpeedMax(0.f)
                , m_screenTargetAABB(Vec2d::Zero)        
                , m_cameraTargetDecentering(Vec2d::Zero)
                , m_cameraTargetDecenteringDest(Vec2d::Zero)
                , m_cameraTargetDecenteringDir(Vec2d::Zero)
                , m_cameraTargetDecenteringDirFromSubjectStance(Vec2d::Zero)
                , m_cameraTargetDecenteringSpeed(Vec2d::Zero)
                , m_cameraTargetDecenteringDestSpeed(Vec2d::Zero)
                , m_cameraBaseMovingSpeed(Vec3d::Zero)
                , m_cameraBaseMovingLockedDestY(bfalse)
                , m_cameraBasePos(Vec3d::Zero)
                , m_cameraBasePosDest(Vec3d::Zero)
                , m_cameraTargetPos(Vec3d::Zero)                
                , m_cameraBaseMovingBlend(Vec2d::Zero)
                , m_cameraTargetDecenteringSpeedAverageAbsOfTarget(Vec2d::Zero)
                , m_cameraTargetDecenteringSpeedAverageRealOfTarget(Vec2d::Zero)
                , m_cameraTargetDecenteringBlend(Vec2d::Zero)
                , m_cameraTargetDecenteringInertie(Vec2d::Zero)
                , m_cameraTargetDecenteringCoeffLast(Vec2d::Zero)
                , m_cameraBaseMovingSpeedAverageAbsOfTarget(Vec2d::Zero)
                , m_cameraBaseMovingBlendYMinInSticked(0.f)
                , m_rayCastDownLengthMax(0.f)
                , m_rayCastDown(Vec2d::Zero)
                , m_rayCastCollisionDownFound(bfalse)       
                , m_subjectLockedPosition_blend(0.f)
                , m_weightRegisterSpeed(0.f)
                , m_registerStartPos(Vec3d::Zero)
                , m_weightTime(0.f)
                , m_isMainController(bfalse)
                , m_wasMainController(bfalse)
                , m_driftTime(1.f)
                , m_isPaused(bfalse)
            {
            }

            Controller( )
            {
                Controller(NULL);
            }

            InGameCameraComponent* m_component;
            f32   m_weight;    
            f32   m_weightTime;
            f32   m_weightDest;
            f32   m_weightRegisterSpeed;
            Vec3d m_registerStartPos;
            f32   m_driftTime;
            bbool m_isPaused;

            void    update( f32 _deltaTime ); 
            void    teleport(Vec3d _pos);
            bbool   isActive() const { return m_weightDest == 1.f; }

            // subject
            SubjectStance m_subjectStance;
            AABB    m_subjectAABB;
            Vec2d   m_subjectAABBCenterLast;            
            Vec2d   m_subjectSpeed;           
            Vec3d   m_subjectPosReal;
            f32     m_subjectLockedPosition_blend;
            void    setSubjectLockedPosition();

            bbool   m_isMainController;
            bbool   m_wasMainController;
            
            void    setSubject(f32 _deltaTime);           
            void    setSubjectSpeed( Vec2d _pos );
            void    setSubjectAABB(f32 _deltaTime);

            // zone
            AABB    m_zoneAABB;
            Vec2d   m_zoneOffsetFromSubject;
            Vec2d   m_zoneSpeedByDepth;        
            Vec2d   m_zoneSpeedReal;
            Vec2d   m_zoneAABBCenterLast;   
            void    setZone();
            void    setZoneAABB();
            void    setZoneSpeed();
            f32     getZoneOffsetXMax();
            f32     getZoneOffsetYMax();
            void    setZoneOffsetX();
            void    setZoneOffsetY();

            // screen base
            AABB    m_screenBaseAABB;
            f32     m_screenRealLimitUp;
            f32     m_screenRealLimitUpDist;
            f32     m_screenRealLimitUpSpeedMax;
            f32     m_screenRealLimitUpTargetDist;
            f32     m_screenRealLimitUpRatio;
            void    setScreenBase();
            void    setControllerScreenRealLimit();
            void    setScreenBaseAABB();

            // screen target
            AABB    m_screenTargetAABB;
            void    setScreenTarget();

            // camera base
            Vec3d   m_cameraBasePos;
            Vec3d   m_cameraBasePosDest;
            Vec3d   m_cameraBaseMovingSpeed;
            Vec2d   m_cameraBaseMovingSpeedAverageAbsOfTarget;
            Vec2d   m_cameraBaseMovingBlend;
            bbool   m_cameraBaseMovingLockedDestY;
            f32     m_cameraBaseMovingBlendYMinInSticked;
            void    setCameraBaseBlendX();
            void    setCameraBaseBlendY();
            void    setCameraBasePositionX();
            void    setCameraBasePositionY();
            void    setCameraBasePositionYDest();
            void    setCameraBasePositionZ();
            void    setCameraBasePosition();
            void    setCameraBase();
            void    increazeCameraMovingBaseSpeedAverageAbsOfTarget();

            // camera target
            Vec3d   m_cameraTargetPos;
            Vec3d   m_cameraTargetPosPrevious;
            Vec3d   m_cameraTargetSpeedPrevious;
            Vec2d   m_cameraTargetDecentering;       
            Vec2d   m_cameraTargetDecenteringDest;
            Vec2d   m_cameraTargetDecenteringDestSpeed;
            Vec2d   m_cameraTargetDecenteringSpeed;
            Vec2d   m_cameraTargetDecenteringDir; 
            Vec2d   m_cameraTargetDecenteringDirFromSubjectStance;
            Vec2d   m_cameraTargetDecenteringSpeedAverageAbsOfTarget;
            Vec2d   m_cameraTargetDecenteringSpeedAverageRealOfTarget;
            Vec2d   m_cameraTargetDecenteringBlend;
            Vec2d   m_cameraTargetDecenteringInertie; 
            Vec2d   m_cameraTargetDecenteringCoeffLast;
            void    setCameraTarget( f32 _dt );
            void    setCameraTargetPos();
            void    setCameraTargetDecenteringX( f32 _dt );
            f32     getCameraTargetDecenteringDestinationX();
            void    setCameraTargetDecenteringDirectionX( f32 _dt );        
            bbool   setCameraTargetDecenteringFromSubject( f32 _dt );        
            void    setCameraTargetDecentering(f32 _dt);
            void    setCameraTargetDecenteringXBlendAndInertie();
            void    resetCameraTargetDecentering();

            // ray cast
            Vec2d   m_rayCastDown;
            f32     m_rayCastDownLengthMax;
            bbool   m_rayCastCollisionDownFound;
            bbool   isRayCastCollisionDown();
            bbool   isRayCastCollisionUp();
            void    initRayCastCollisionDown();     
                       
        };
        
        // controllers
        u32     m_controllerActiveCount;
        u32     m_controllerActiveCountLast;   
        bbool   m_isScreenLimitCrossed;
        bbool   m_resetDezoomTime;
        f32     m_controllerDriftTimeCoeff;
        ITF_VECTOR<Controller> m_controllers;
        Controller* m_mainController;
        bbool   m_isLeaderController;
        void    setControllersPosition(f32 _deltaTime);
        void    addControllers();
        void    removeControllers();
        void    setControllersSubject(f32 _deltaTime);
        void    setControllersWeight(f32 _dt);
        bbool   setMainController();
        void    updateSubjectGlobalDatas( const Controller& _controller);

        //  subject
        f32     m_subjectPosZ;
        Vec2d   m_subjectZoneNeutral;
        f32     m_subjectRegisterSpeed;
        f32     m_subjectUnregisterSpeed;
        AABB    m_subjectGlobalAABB;
        f32     m_subjectGlobalDepthMin;        
        void    setZoneNeutral(f32 _deltaTime);
      
                    
        // screen target average
        AABB    m_screenTargetAverageAABB;
        void    setScreenTargetAverage();

        // camera target average
        Vec3d   m_cameraTargetAveragePos;
        Vec3d   m_cameraTargetAveragePosPrevious;
        Vec3d   m_cameraTargetAverageSpeed;
        f32     m_cameraTargetAverageDriftTime;
        f32     m_cameraTargetAverageDriftTimeCoeff;
        f32     m_cameraTargetAverageDezoomDestWanted;              
        f32     m_cameraTargetAverageDezoomCur;        
        f32     m_cameraTargetAverageDezoomCurSpeed;
        f32     m_cameraTargetAverageDezoomDest;  
        f32     m_cameraTargetAverageDezoomDestSpeed; 
        f32     m_cameraTargetAverageDezoomDestFinal; 
        f32     m_cameraTargetAverageDezoomDelay;
        void    setCameraTargetAverage(f32 _dt);
        void    setDezoomCur( f32 _dt );        

        // camera real
        AABB    m_screenRealAABB;
        Vec2d   m_screenRealLongBy2;
        f32     m_depth;
        Vec3d   m_cameraRealPos;
        void    setScreenReal();                
        void    setCameraReal( f32 _dt );

        // modifiers
        bbool   m_constraintModifierOnEnter;
        bbool   m_constraintModifierOnExit;
        bbool   m_switchModifier;

        CameraModifierComponent*    m_cameraModifier;
        ObjectRef m_cameraModifierRef;
        f32     m_borderBlending;
        void    setModifiers();
        void    setModifierConstraint();
        bbool   keepModifier();
        void    setBorderBlending();
        void    blendingModifiers();
        void    setModifierLookAtOffset( CamModifierUpdate& _cm);
        void    updateModifiers();
                
        // constraints
        CameraModifierComponent*    m_constraintModifier;
        ObjectRef m_constraintRef;
        AABB    m_constraintAABB;       
        AABB    m_constraintAnticipAABB;
        AABB    m_screenConstraintAABB;
        Vec3d   m_constraintTimeCoeffToActivate;        
        f32     m_constraintLeftTime;
        f32     m_constraintRightTime;
        f32     m_constraintDepthTime;
        f32     m_constraintTopTime;
        f32     m_constraintBottomTime;
        Vec3d   m_constraintLast;
        Vec3d   m_constraintSpeedBeforeSwicthConstraint;
        Vec3d   m_constraint;
        Vec3d   m_constraintPrevious;
        bbool   m_constraintLeftTimeIncreazeToMin;
        bbool   m_constraintRightTimeIncreazeToMin;
        bbool   m_constraintTopTimeIncreazeToMin;
        bbool   m_constraintBottomTimeIncreazeToMin;
        Vec3d   m_constraintTimeToDisable;
        Vec3d   m_constraintTimeCoeffToDisable;
        f32     m_constraintDepthOffsetMax;
        LogisticParams m_curveLogistic;

        bbool   m_constraintMatchViewX;
        bbool   m_constraintMatchViewY;
        f32     m_constraintSecretAreaZ;
        f32     m_constraintSecretAreaZOld;        

        void    updateConstraints( f32 _dt );
        void    setConstraintX();
        void    setConstraintY();
        void    setConstraintZ();
        void    setConstraintAABB();
        void    setConstraintTimeOnEnter();
        void    setConstraintTimeOnEnterX();
        void    setConstraintTimeOnEnterY();
        void    setConstraintTimeCoeffOnEnter();
        void    setConstraintTimeToActivate( f32 _dt ); 
        f32     getConstraintAnticipationX();
        f32     getConstraintAnticipationY();               
        f32     getConstraintByLeft( f32 _dest );
        f32     getConstraintByRight( f32 _dest );        
        f32     getConstraintByTop( f32 _dest );
        f32     getConstraintByBottom( f32 _dest );
        void    setConstraintTimeIncreaseToMinX();
        void    setContraintTimeIncreaseToMinY();
        void    resetConstraints();
        void    resetConstraintX();
        void    resetConstraintY();
        void    resetConstraintZ();
        void    setConstraintTimeToDisable( f32 _dt );
        void    setScreenConstraintAABB();
        void    testConstraintExtendedAxeX();
        void    testConstraintExtendedAxeY();    
        void    setConstraintSecretAreaZ(f32 _screenRatio);
        void    initConstraintParams();
        void    setConstraintMatchView();

        // 
        f32     m_depthCoeff;
        void    setDepthCoeff();        

        // visibility
        AABB    m_visibilityAABB;
        AABB    m_visibilityScreenMinAABB;
        AABB    m_visibilityScreenMaxAABB;        
        Vec2d   m_visibilityDest;
        Vec2d   m_visibilityCur;
        Vec2d   m_visibilityLast;
        Vec2d   m_visibilityPrevious;
        f32     m_visibilitySwitchPlayerTargetTime;
        f32     m_visibilitySwitchSpeed;
        Vec2d   m_visibilitySpeedBeforeSwicthPlayer;

        f32     m_visibilitySecretAreaZCur;
        f32     m_visibilitySecretAreaZLast;
        f32     m_visibilitySecretAreaZTime;

        f32     m_visibilitySwitchAxeTime;
        Vec2d   m_visibilityOffsetLast;
        Vec2d   m_visibilityOffsetCur;
        Vec2d   m_visibilityOffsetDest;

        void    setDezoomDest(f32 _deltaTime);
        void    setScreenAABB( AABB& _aabb, const Vec2d& _pos, f32 _depth);
        void    setScreenAABBLongBy2( Vec2d& _screenLongBy2, f32 _depth );
        void    updateVisibility(f32 _dt);
        void    setVisibilityOffset( f32 _dt );
        Vec2d   getVisibilityOffset();

        f32     m_deltaTimeInv;              
    };   


    class InGameCameraComponent_Template : public BaseCameraComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(InGameCameraComponent_Template,BaseCameraComponent_Template,3647884283)
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(InGameCameraComponent);

    public:

        InGameCameraComponent_Template();
        ~InGameCameraComponent_Template();

        const CamModifier_Template& getCMInit() const { return m_CMInit; }

    private:

        CamModifier_Template                m_CMInit;
    };

    ITF_INLINE const InGameCameraComponent_Template* InGameCameraComponent::getTemplate() const
    {
        return static_cast<const InGameCameraComponent_Template*>(m_template);
    }
}
#endif // _ITF_INGAMECAMERACOMPONENT_H_
