#ifndef _ITF_INGAMECAMERAPARAMETERS_H_
#define _ITF_INGAMECAMERAPARAMETERS_H_

namespace ITF
{
    struct CamModifier_Template
    {
        DECLARE_SERIALIZE()

        CamModifier_Template()
            : m_cameraModifierBlend(0.f)
            , m_cameraModifierInertie(0.f)
            , m_ignoreZ(bfalse)
            , m_focale(MTH_PIBY4)                                  
            , m_zoneScaleDepthMin(8.f)
            , m_zoneScaleDepthMax(22.f)
            , m_zoneScaleAtDepthMin(Vec2d(0.1f,0.1f))
            , m_zoneScaleAtDepthMax(Vec2d(2.f,2.f))
            , m_zoneScaleSpeed(0.01f)
            
            // camera decentering
            , m_cameraDecenteringOffsetAtSpeedMin(Vec2d(0.1f, 0.1f))
            , m_cameraDecenteringOffsetAtSpeedMax(Vec2d(0.1f, 0.1f))
            , m_cameraDecenteringSpeedMin(Vec2d::Zero)
            , m_cameraDecenteringSpeedMax(Vec2d::Zero)
            , m_cameraDecenteringBlendAtSpeedMin(Vec2d::Zero)
            , m_cameraDecenteringBlendAtSpeedMax(Vec2d::Zero)
            , m_cameraDecenteringDelay(Vec2d::Zero)
            , m_cameraDecenteringInertieAtSpeedMin(Vec2d::Zero)
            , m_cameraDecenteringInertieAtSpeedMax(Vec2d::Zero)
            , m_cameraDecenteringSmooth(Vec2d::Zero)            
            , m_cameraDecenteringSubjectLookDirWeight(Vec2d::Zero)
            , m_cameraDecenteringDepthMin(0.f)
            , m_cameraDecenteringDepthMax(0.f)
            // camera moving
            , m_cameraMovingBlendAtSpeedMin(Vec2d::Zero)
            , m_cameraMovingBlendAtSpeedMax(Vec2d::Zero)
            , m_cameraMovingSpeedMin(Vec2d::Zero)
            , m_cameraMovingSpeedMax(Vec2d::Zero)
            , m_cameraMovingSmooth(Vec2d::Zero)            
            , m_cameraLookAtOffset(Vec3d::Zero)
            , m_cameraLookAtOffsetYUp(0.f)
            , m_cameraLookAtOffsetYDown(0.f)
            , m_cameraMovingBlendAccelerationJustSticked(0.f)
            , m_rayCastScaleMax(0.f)
            // screen
            , m_screenLimitUpSpeedMax(0.f)
            , m_screenLimitUpScale(0.f)
            // constraint          
            , m_constraintDelayToActivate(Vec3d::Zero)
            , m_constraintDelayToDisable(Vec3d::Zero)

            // multi players
            , m_cameraLookAtOffsetMaxInMulti(Vec3d::Zero)                        
            , m_subjectWeightRegisterDelay(0.f)
            , m_subjectWeightUnregisterDelay(0.f)
            , m_zoomBlendInMultiplayer(0.f)
            , m_zoomInertieInMultiplayer(0.f)
            , m_zoomDelayInMultiplayer(0.f)            
            , m_dezoomBlendInMultiplayer(0.f)
            , m_dezoomInertieInMultiplayer(0.f)                                        
            , m_subjectMainVisibilityHorizontal(Vec2d::One)
            , m_subjectMainVisibilityVertical(Vec2d::One)
            , m_cameraMovingSmoothCoeffForLeader(0.5f)
            , m_subjectMainVisibilitySwitchAxeSpeed(0.f)

            // lock position
            , m_lockPositionBlendOnEnter(0.01f)
            , m_lockPositionBlendOnExit(0.1f)

            , m_cameraDriftDelay(0.f)
            , m_controllerDriftDelay(0.f)
            , m_isMainSubject(btrue)
        {
        }
        
        f32     m_cameraModifierBlend;
        f32     m_cameraModifierInertie;

        bbool   m_ignoreZ;
        bbool   m_isMainSubject;
        f32     m_focale;

        f32     m_zoneScaleDepthMin;
        f32     m_zoneScaleDepthMax;
        Vec2d   m_zoneScaleAtDepthMin;
        Vec2d   m_zoneScaleAtDepthMax;
        f32     m_zoneScaleSpeed;
                        
        Vec2d   m_cameraDecenteringOffsetAtSpeedMin;
        Vec2d   m_cameraDecenteringOffsetAtSpeedMax;
        Vec2d   m_cameraDecenteringSpeedMin;
        Vec2d   m_cameraDecenteringSpeedMax;
        Vec2d   m_cameraDecenteringBlendAtSpeedMin;
        Vec2d   m_cameraDecenteringBlendAtSpeedMax;
        Vec2d   m_cameraDecenteringDelay;
        Vec2d   m_cameraDecenteringInertieAtSpeedMin;
        Vec2d   m_cameraDecenteringInertieAtSpeedMax;
        Vec2d   m_cameraDecenteringSmooth;
        Vec2d   m_cameraDecenteringSubjectLookDirWeight;        
        f32     m_cameraDecenteringDepthMin;
        f32     m_cameraDecenteringDepthMax;

        Vec2d   m_cameraMovingBlendAtSpeedMin;
        Vec2d   m_cameraMovingBlendAtSpeedMax;
        Vec2d   m_cameraMovingSpeedMin;
        Vec2d   m_cameraMovingSpeedMax;
        Vec2d   m_cameraMovingSmooth;
        f32     m_cameraMovingBlendAccelerationJustSticked;
        f32     m_rayCastScaleMax;

        Vec3d   m_cameraLookAtOffset;
        f32     m_cameraLookAtOffsetYUp;
        f32     m_cameraLookAtOffsetYDown;

        // screen
        f32     m_screenLimitUpScale;
        f32     m_screenLimitUpSpeedMax;

        // constraint                    
        Vec3d   m_constraintDelayToActivate;
        Vec3d   m_constraintDelayToDisable;

        // multi players
        Vec3d   m_cameraLookAtOffsetMaxInMulti;                       
        f32     m_subjectWeightRegisterDelay;
        f32     m_subjectWeightUnregisterDelay;
        Vec2d   m_subjectMainVisibilityHorizontal;
        Vec2d   m_subjectMainVisibilityVertical;
        f32     m_cameraMovingSmoothCoeffForLeader;
        f32     m_subjectMainVisibilitySwitchAxeSpeed;

        // zoom
        f32     m_zoomBlendInMultiplayer;
        f32     m_zoomInertieInMultiplayer;
        f32     m_zoomDelayInMultiplayer;

        // dezoom
        f32     m_dezoomBlendInMultiplayer;
        f32     m_dezoomInertieInMultiplayer;                

        // lock position
        f32     m_lockPositionBlendOnEnter;
        f32     m_lockPositionBlendOnExit;

        // camera drift
        f32     m_cameraDriftDelay;
        f32     m_controllerDriftDelay;
    };

    //////////////////////////////////////////////////////////////////////////
    ///Defines which members of InGameCameraParameters are to be used
    struct CamModifierOverride_Template
    {
        DECLARE_SERIALIZE()

        CamModifierOverride_Template()
            : m_focale(bfalse)
            , m_cameraLookAtOffset(bfalse)
            , m_cameraLookAtOffsetMaxInMulti(bfalse)
            , m_constraintDelayToDisable(bfalse)
            , m_constraintDelayToActivate(bfalse)
            , m_cameraModifierInertie(bfalse)
            , m_cameraModifierBlend(bfalse)
        {}

        // new cam
        bbool m_focale;
        bbool m_cameraLookAtOffset;
        bbool m_cameraLookAtOffsetMaxInMulti;
        bbool m_constraintDelayToDisable;
        bbool m_constraintDelayToActivate;
        bbool m_cameraModifierBlend;
        bbool m_cameraModifierInertie;       
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///Parameters for InGameCameraComponent : Area, constraints, alterations of the camera
    struct ConstraintExtended
    {
        DECLARE_SERIALIZE()

        ConstraintExtended()
            : m_offset(0.f)
            , m_timeToIncrease(1.f)
            , m_timeToDecrease(2.f)
            , m_timeToWaitBeforeDecrease(5.f)
        {
        }

        f32 m_offset;
        f32 m_timeToIncrease;
        f32 m_timeToDecrease;
        f32 m_timeToWaitBeforeDecrease;
    };

    struct CamModifier //rename to InGameCameraParameters
    {
        DECLARE_SERIALIZE()

        CamModifier()
            : m_constraintLeftIsActive(btrue)
            , m_constraintRightIsActive(btrue)
            , m_constraintTopIsActive(btrue)
            , m_constraintBottomIsActive(btrue)
            , m_constraintMatchView(bfalse)    
            , m_cameraModifierPriority(0)
            , m_blendingZoneStart(0.f)
            , m_blendingZoneStop(0.f)
            , m_zoneNeutral(Vec2d::Zero)
            , m_useDecentering(btrue)
        {
        }

        i32     m_cameraModifierPriority;
        bbool   m_constraintLeftIsActive;
        bbool   m_constraintRightIsActive;
        bbool   m_constraintTopIsActive;
        bbool   m_constraintBottomIsActive;
        bbool   m_constraintMatchView;
        Vec2d   m_zoneNeutral;
        bbool   m_useDecentering;

        ConstraintExtended  m_constraintExtendedLeft;
        ConstraintExtended  m_constraintExtendedRight;
        ConstraintExtended  m_constraintExtendedTop;
        ConstraintExtended  m_constraintExtendedBottom;

        f32     m_blendingZoneStart;
        f32     m_blendingZoneStop;
    };

    //////////////////////////////////////////////////////////////////////////
    ///Defines which members of InGameCameraParameters are to be used
    struct CamModifierOverride
    {
        DECLARE_SERIALIZE()

        CamModifierOverride()
            : m_template(NULL)
            , m_constraintLeftIsActive(btrue)
            , m_constraintRightIsActive(btrue)
            , m_constraintTopIsActive(btrue)
            , m_constraintBottomIsActive(btrue)
            , m_constraintMatchView(btrue)
        {}

        // new cam
        const struct CamModifierOverride_Template* m_template;
        bbool   m_constraintLeftIsActive;
        bbool   m_constraintRightIsActive;
        bbool   m_constraintTopIsActive;
        bbool   m_constraintBottomIsActive;
        bbool   m_constraintMatchView;        
    };

    struct CamModifierUpdate
    {
        CamModifierUpdate()
            : m_camModifier(NULL)
            , m_camModifierTemplate(NULL)
            , m_focale(MTH_PIBY4)
            , m_cameraLookAtOffsetYUp(0.f)
            , m_cameraLookAtOffsetYDown(0.f)
            , m_cameraLookAtOffset(Vec3d::Zero)
            , m_cameraLookAtOffsetMaxInMulti(Vec3d::Zero)
            , m_constraintDelayToActivate(Vec3d::Zero)
            , m_constraintDelayToDisable(Vec3d::Zero)
            , m_cameraModifierDirectionNormalized(Vec2d::Right)
            , m_cameraLookAtOffset_SpeedBlend(Vec3d::Zero)
            , m_cameraLookAtOffset_SpeedBlendMaxInMulti(Vec3d::Zero)
            , m_cameraModifierInertie(0.f)
            , m_cameraModifierBlend(0.f)
            , m_constraintLeftIsActive(bfalse)
            , m_constraintRightIsActive(bfalse)
            , m_constraintTopIsActive(bfalse)
            , m_constraintBottomIsActive(bfalse)
            , m_constraintMatchView(bfalse)
            , m_ignoreZ(bfalse)
            , m_directionLookAtOffset(0.f)
            , m_directionLookAtOffset_SpeedBlend(0.f)
            , m_isMainSubject(btrue)
        {
        }

        void                                                getModifierList( CamModifierUpdate& _to ) const;
        void                                                init( const CamModifier* _cm, const CamModifier_Template* _cmTemplate );
        void                                                clear();
        void                                                setModifierList(CamModifierUpdate& _dest, f32 _blend, f32 _inertie );
        void                                                setModifierListFixed( CamModifierUpdate& _dest);
        void                                                blendModifierVec3d( Vec3d& _dest, Vec3d& _cur, Vec3d& _curSpeed, f32 _blend, f32 _inertie );
        void                                                blendModifierF32( f32& _dest, f32& _cur, f32& _curSpeed, f32 _blend, f32 _inertie );
        const Vec2d&                                        getDirection() const {return m_cameraModifierDirectionNormalized;}
        void                                                borderBlendingModifierVec3d( Vec3d& _cur, Vec3d& _dest, Vec3d& _last, f32 _blend);
        void                                                borderBlendingModifierF32( f32& _cur, f32& _dest, f32& _last, f32 _blend);
        void                                                setModifierListByBorderBlending(CamModifierUpdate& _dest, CamModifierUpdate& _last, f32 _blendingBorder );

        const CamModifier*                                  m_camModifier;
        const CamModifier_Template*                         m_camModifierTemplate;
        f32                                                 m_focale;
        f32                                                 m_cameraLookAtOffsetYUp;
        f32                                                 m_cameraLookAtOffsetYDown;
        Vec3d                                               m_cameraLookAtOffset;
        Vec3d                                               m_cameraLookAtOffsetMaxInMulti;
        Vec3d                                               m_constraintDelayToActivate;
        Vec3d                                               m_constraintDelayToDisable;
        Vec2d                                               m_cameraModifierDirectionNormalized;
        Vec3d                                               m_cameraLookAtOffset_SpeedBlend;
        Vec3d                                               m_cameraLookAtOffset_SpeedBlendMaxInMulti;
        f32                                                 m_cameraModifierInertie;
        f32                                                 m_cameraModifierBlend;
        bbool                                               m_ignoreZ;
        bbool                                               m_constraintLeftIsActive;
        bbool                                               m_constraintRightIsActive;
        bbool                                               m_constraintTopIsActive;
        bbool                                               m_constraintBottomIsActive;
        bbool                                               m_constraintMatchView;
        f32                                                 m_directionLookAtOffset;
        f32                                                 m_directionLookAtOffset_SpeedBlend;
        bbool                                               m_isMainSubject;

        static void update_focale(const CamModifier_Template& _from, CamModifierUpdate &_to) { _to.m_focale = _from.m_focale; }
        static void update_cameraLookAtOffset(const CamModifier_Template& _from, CamModifierUpdate &_to) { _to.m_cameraLookAtOffset = _from.m_cameraLookAtOffset; }
        static void update_cameraLookAtOffsetMaxInMulti(const CamModifier_Template& _from, CamModifierUpdate &_to) { _to.m_cameraLookAtOffsetMaxInMulti = _from.m_cameraLookAtOffsetMaxInMulti; }
        static void update_constraintDelayToActivate(const CamModifier_Template& _from, CamModifierUpdate &_to) { _to.m_constraintDelayToActivate = _from.m_constraintDelayToActivate; }
        static void update_constraintDelayToDisable(const CamModifier_Template& _from, CamModifierUpdate &_to) { _to.m_constraintDelayToDisable = _from.m_constraintDelayToDisable; }
        static void update_cameraModifierInertie(const CamModifier_Template& _from, CamModifierUpdate &_to) { _to.m_cameraModifierInertie = _from.m_cameraModifierInertie; }
        static void update_cameraModifierBlend(const CamModifier_Template& _from, CamModifierUpdate &_to) { _to.m_cameraModifierBlend = _from.m_cameraModifierBlend; }        
        static void update_constraintLeftIsActive(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintLeftIsActive = _from.m_constraintLeftIsActive; }
        static void update_constraintRightIsActive(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintRightIsActive = _from.m_constraintRightIsActive; }
        static void update_constraintTopIsActive(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintTopIsActive = _from.m_constraintTopIsActive; }
        static void update_constraintBottomIsActive(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintBottomIsActive = _from.m_constraintBottomIsActive; }
        static void update_constraintMatchView(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintMatchView = _from.m_constraintMatchView; }                    

        typedef void (*ingameCameraParameterTemplateFncDecl)(const CamModifier_Template&, CamModifierUpdate&); // from, to
        typedef void (*ingameCameraParameterInstanceFncDecl)(const CamModifier&, CamModifierUpdate&); // from, to

        ITF_VECTOR<ingameCameraParameterTemplateFncDecl>    m_updateModifiersListTemplate;
        ITF_VECTOR<ingameCameraParameterInstanceFncDecl>    m_updateModifiersListInstance;


    };
}

#endif
