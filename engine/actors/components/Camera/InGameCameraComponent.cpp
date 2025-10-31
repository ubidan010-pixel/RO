#include "precompiled_engine.h"

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
    InGameCameraComponent* InGameCameraComponent::s_instance = NULL;
    static const f32 ScreenHeightBase = 6.6274f; // 6.6 is height screen for 8 meters of depth offset

    IMPLEMENT_OBJECT_RTTI(InGameCameraComponent)
        BEGIN_SERIALIZATION_CHILD(InGameCameraComponent)
        END_SERIALIZATION()

        IMPLEMENT_OBJECT_RTTI(InGameCameraComponent_Template)
        BEGIN_SERIALIZATION_CHILD(InGameCameraComponent_Template)
        SERIALIZE_OBJECT("CM",m_CMInit);
    END_SERIALIZATION()

    InGameCameraComponent_Template::InGameCameraComponent_Template()
    {
    }

    InGameCameraComponent_Template::~InGameCameraComponent_Template()
    {
    }

    InGameCameraComponent::InGameCameraComponent()
        : Super()
        , m_depthCoeff(1.f)
        , m_screenRealLongBy2(Vec2d::One)
        , m_depth(13.f)
        , m_subjectPosZ(0.f)
        , m_cameraTargetAveragePos(Vec3d::Zero)        
        , m_screenTargetAverageAABB(Vec2d::Zero)
        , m_screenRealAABB(Vec2d::Zero)
        , m_cameraRealPos(Vec3d::Zero)
        , m_cameraTargetAveragePosPrevious(Vec3d::Zero)
        , m_cameraTargetAverageSpeed(Vec3d::Zero)
        , m_cameraTargetAverageDriftTime(1.f)
        , m_cameraTargetAverageDriftTimeCoeff(0.f)
        , m_cameraModifier(NULL)
        , m_cameraModifierRef(ITF_INVALID_OBJREF)
        , m_constraintModifierOnEnter(bfalse)
        , m_constraintModifierOnExit(bfalse)
        , m_controllerActiveCount(0)
        , m_controllerActiveCountLast(0)
        , m_constraintModifier(NULL)
        , m_constraintRef(ITF_INVALID_OBJREF)
        , m_constraintLast(Vec3d::Zero)
        , m_constraintSpeedBeforeSwicthConstraint(Vec3d::Zero)
        , m_constraint(Vec3d::Zero)
        , m_constraintPrevious(Vec3d::Zero)
        , m_constraintLeftTime(0.f)
        , m_constraintRightTime(0.f)
        , m_constraintBottomTime(0.f)
        , m_constraintTopTime(0.f)
        , m_constraintDepthTime(0.f)
        , m_constraintTimeCoeffToActivate(Vec3d::Zero)      
        , m_constraintLeftTimeIncreazeToMin(bfalse)
        , m_constraintRightTimeIncreazeToMin(bfalse)
        , m_constraintTopTimeIncreazeToMin(bfalse)
        , m_constraintBottomTimeIncreazeToMin(bfalse)
        , m_constraintTimeCoeffToDisable(Vec3d::Zero)      
        , m_constraintTimeToDisable(Vec3d::Zero)      
        , m_constraintAnticipAABB(Vec2d::Zero)
        , m_constraintAABB(Vec2d::Zero)
        , m_screenConstraintAABB(Vec2d::Zero)
        , m_constraintDepthOffsetMax(F32_INFINITY)
        , m_constraintSecretAreaZ(0.f)
        , m_constraintSecretAreaZOld(0.f)
        , m_cameraTargetAverageDezoomDestWanted(0.f)        
        , m_cameraTargetAverageDezoomCur(0.f)
        , m_cameraTargetAverageDezoomCurSpeed(0.f)    
        , m_cameraTargetAverageDezoomDelay(0.f)    
        , m_cameraTargetAverageDezoomDestSpeed(0.f)
        , m_cameraTargetAverageDezoomDestFinal(0.f)
        , m_cameraTargetAverageDezoomDest(0.f)       
        , m_borderBlending(1.f)
        , m_visibilityDest(Vec2d::Zero)
        , m_visibilityCur(Vec2d::Zero)
        , m_visibilityPrevious(Vec2d::Zero)
        , m_visibilitySpeedBeforeSwicthPlayer(Vec2d::Zero)
        , m_visibilityLast(Vec2d::Zero)
        , m_visibilityOffsetLast(Vec2d::Zero)
        , m_visibilityOffsetCur(Vec2d::Zero)
        , m_visibilityOffsetDest(Vec2d::Zero)
        , m_visibilitySwitchPlayerTargetTime(1.f)
        , m_visibilitySwitchAxeTime(1.f)
        , m_isScreenLimitCrossed(bfalse)
        , m_resetDezoomTime(bfalse)
        , m_subjectZoneNeutral(Vec2d(0.2f,0.3f))
        , m_subjectUnregisterSpeed(0.f)
        , m_subjectRegisterSpeed(0.f)
        , m_switchModifier(bfalse)
        , m_subjectGlobalDepthMin(0.f)
        , m_subjectGlobalAABB(Vec2d::Zero)
        , m_deltaTimeInv(1.f)
        , m_mainController(NULL)
        , m_controllerDriftTimeCoeff(0.f)
        , m_isLeaderController(bfalse)
        , m_constraintMatchViewX(bfalse)
        , m_constraintMatchViewY(bfalse)
        , m_visibilitySecretAreaZCur(0.f)
        , m_visibilitySecretAreaZTime(0.f)
        , m_visibilitySecretAreaZLast(0.f)
        , m_visibilitySwitchSpeed(1.f)
    {
        m_curveLogistic.m_xMax = 10.f; // to 1
        m_curveLogistic.m_xMin = -10.f; // to 0
        m_controllers.reserve(10);    
    }

    InGameCameraComponent::~InGameCameraComponent()
    {
        m_CMCur.clear();
        m_CMDestCur.clear();
        m_CMDestFinal.clear();
        m_CMPrevious.clear();
        m_controllers.clear();

        if (s_instance == this)
        {
            s_instance = NULL;
        }
    }

    void InGameCameraComponent::onActorLoaded(Pickable::HotReloadType _hotReload) 
    {
        Super::onActorLoaded(_hotReload);

        ITF_WARNING_CATEGORY(LD, m_actor, !s_instance, "DUPLICATE CAMERA! most likely the camera was copy-pasted and saved in the scene, find it and remove it.");
        s_instance = this;

        m_CMCur.init(NULL,&getTemplate()->getCMInit());
        m_CMDestCur.init(NULL,&getTemplate()->getCMInit());
        m_CMDestFinal.init(NULL,&getTemplate()->getCMInit());
        m_CMPrevious.init(NULL,&getTemplate()->getCMInit());

        CAMERACONTROLLERMANAGER->setDefaultDepth(m_CMCur.m_cameraLookAtOffset.m_z);

        // register settings
        if ( m_CMCur.m_camModifierTemplate->m_subjectWeightRegisterDelay >0.f)
            m_subjectRegisterSpeed = 1.f /m_CMCur.m_camModifierTemplate->m_subjectWeightRegisterDelay;
        else
            m_subjectRegisterSpeed = F32_INFINITY;

        if ( m_CMCur.m_camModifierTemplate->m_subjectWeightUnregisterDelay >0.f)
            m_subjectUnregisterSpeed = 1.f /m_CMCur.m_camModifierTemplate->m_subjectWeightUnregisterDelay;
        else
            m_subjectUnregisterSpeed = F32_INFINITY;

        // visibility dest
        m_visibilityOffsetDest = m_CMCur.m_camModifierTemplate->m_subjectMainVisibilityHorizontal;

        // drift camera target average delay
        if ( m_CMCur.m_camModifierTemplate->m_cameraDriftDelay >0.f)
            m_cameraTargetAverageDriftTimeCoeff = 1.f /m_CMCur.m_camModifierTemplate->m_cameraDriftDelay;
        else
            m_cameraTargetAverageDriftTimeCoeff = F32_INFINITY;

        // drift controller delay
        if ( m_CMCur.m_camModifierTemplate->m_controllerDriftDelay >0.f)
            m_controllerDriftTimeCoeff = 1.f /m_CMCur.m_camModifierTemplate->m_controllerDriftDelay;
        else
            m_controllerDriftTimeCoeff = F32_INFINITY;

        const u32 playerListCount = CAMERACONTROLLERMANAGER->getSubjectPlayerListCount();
        if ( playerListCount )
        {
            CameraControllerSubject subject;
            CAMERACONTROLLERMANAGER->getSubjectByIndex( 0, subject);

            const Actor* player = (Actor*) GETOBJECT(subject.m_ref);
            if ( player )
            {
                teleport(player->getPos());
            }

            addControllers();

            for( u32 i=0; i<m_controllers.size(); i++)
            {
                Controller& controller = m_controllers[i];

                controller.m_weightRegisterSpeed = F32_INFINITY;
            }
        }

        m_actor->changeRelativeAABB(AABB(-Vec2d::One, Vec2d::One));
        m_actor->setAlwaysVisible(btrue);        
    }

    void InGameCameraComponent::setDepthCoeff()
    {
        const f32 screenHeight = Max( m_screenRealAABB.getHeight(), 1.f);

        m_depthCoeff = ScreenHeightBase /screenHeight;                
    }

    void InGameCameraComponent::setZoneNeutral(f32 _deltaTime)
    {
        static Vec2d zoneNeutralFromDepth(Vec2d::Zero);

        // from depth
        const f32 deltaZoneDepth = m_CMCur.m_camModifierTemplate->m_zoneScaleDepthMax -m_CMCur.m_camModifierTemplate->m_zoneScaleDepthMin;
        if ( deltaZoneDepth >0.f )
        {
            const f32 coeff = Clamp( (m_depth -m_CMCur.m_camModifierTemplate->m_zoneScaleDepthMin) /deltaZoneDepth, 0.f, 1.f);
            zoneNeutralFromDepth = m_CMCur.m_camModifierTemplate->m_zoneScaleAtDepthMin +( m_CMCur.m_camModifierTemplate->m_zoneScaleAtDepthMax -m_CMCur.m_camModifierTemplate->m_zoneScaleAtDepthMin) *coeff;  
        }

        Vec2d zoneNeutralDest = zoneNeutralFromDepth;
        // from modifiers
        if ( m_cameraModifier )
        {
            const Vec2d& zoneNeutralFromModifier = m_cameraModifier->getCM().m_zoneNeutral;

            if ( zoneNeutralFromModifier.m_x )                    
                zoneNeutralDest.m_x = zoneNeutralFromModifier.m_x;                    
            
            if ( zoneNeutralFromModifier.m_y )
                zoneNeutralDest.m_y = zoneNeutralFromModifier.m_y;           
        }
        
        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported)
            m_subjectZoneNeutral = zoneNeutralDest;
        else
            m_subjectZoneNeutral += ( zoneNeutralDest -m_subjectZoneNeutral) *m_CMCur.m_camModifierTemplate->m_zoneScaleSpeed;
    }

    void InGameCameraComponent::updateInit(f32 _deltaTime)
    {
        m_isScreenLimitCrossed = bfalse;
        m_resetDezoomTime = bfalse;

        if ( _deltaTime >0.f )
            m_deltaTimeInv = 1.f /_deltaTime;        
    }

    void InGameCameraComponent::updateDrift(f32 _deltaTime)
    {
        updateModifiers();

        if ( m_controllerActiveCountLast >0 )
        {
            m_cameraTargetAverageDriftTime = 1.f;
        }

        m_cameraTargetAverageDriftTime -= _deltaTime *m_cameraTargetAverageDriftTimeCoeff;
        m_cameraTargetAveragePos += m_cameraTargetAverageSpeed *_deltaTime *m_curveLogistic.getValue(m_cameraTargetAverageDriftTime);  
    }

    void InGameCameraComponent::onManagerUpdate(f32 _deltaTime)
    {
        if ( CAMERACONTROLLERMANAGER->updateCamControllers_SubjectList() )
        {
            removeControllers();
            addControllers();
        }

        updateInit(_deltaTime);
        setControllersSubject(_deltaTime);

        if ( m_controllerActiveCount > 0 )
        {            
            setModifiers();
            setZoneNeutral(_deltaTime);
            
            if ( setMainController() )
            {
                blendingModifiers();
                setControllersPosition(_deltaTime);        
                setControllersWeight(_deltaTime);
                setCameraTargetAverage(_deltaTime);
            }           
        }
        else
        {
            updateDrift(_deltaTime);
        }

        setCameraReal(_deltaTime);

        // update actor
        m_actor->setPos(m_cameraRealPos);       
    }

    void InGameCameraComponent::teleport(const Vec3d& _pos)
    {
        Super::teleport(_pos);

        removeControllers();
        addControllers();

        const Vec2d pos2D(_pos.m_x, _pos.m_y);
        const Vec3d pos3D(_pos.m_x, _pos.m_y, _pos.m_z +getTemplate()->getCMInit().m_cameraLookAtOffset.m_z +CAMERACONTROLLERMANAGER->getCheatZoom());
        
        m_depth = getTemplate()->getCMInit().m_cameraLookAtOffset.m_z;

        resetConstraints();

        // controllers
        ITF_VECTOR<Controller>::iterator it = m_controllers.begin();
        while( it != m_controllers.end() )
        {
            Controller& controller = *it;

            //  remove dead controllers 
            if ( !controller.isActive() )
            {
                it = m_controllers.erase(it);
            }
            else
            {
                controller.teleport(_pos);           
                controller.m_weight = 1.f;
                controller.m_weightRegisterSpeed = F32_INFINITY;
                controller.m_isMainController = btrue;

                it++;
            }
        }

        // modifiers
        m_constraintModifierOnEnter = bfalse;
        m_constraintModifierOnExit = bfalse;
        m_cameraModifier = NULL;
        m_cameraModifierRef = ITF_INVALID_OBJREF;
        m_borderBlending = 1.f;

        m_subjectPosZ = _pos.m_z;               
        m_screenTargetAverageAABB.moveCenter(pos2D);

        m_CMDestFinal.init(NULL,&getTemplate()->getCMInit());
        m_CMPrevious.init(NULL,&getTemplate()->getCMInit());

        m_cameraTargetAveragePos = pos3D;
        m_cameraTargetAveragePosPrevious = pos3D;
        m_cameraTargetAverageSpeed = Vec3d::Zero;
        m_cameraTargetAverageDriftTime = 1.f;

        m_cameraTargetAverageDezoomDestWanted = 0.f;              
        m_cameraTargetAverageDezoomCur = 0.f;        
        m_cameraTargetAverageDezoomCurSpeed = 0.f;
        m_cameraTargetAverageDezoomDest = 0.f;  
        m_cameraTargetAverageDezoomDestSpeed = 0.f; 
        m_cameraTargetAverageDezoomDestFinal = 0.f;
        m_cameraTargetAverageDezoomDelay = 0.f;         

        m_cameraRealPos = pos3D;
        m_screenRealAABB.moveCenter(pos2D);              
        setScreenReal();

        m_visibilityCur = Vec2d::Zero;
        m_visibilitySpeedBeforeSwicthPlayer = Vec2d::Zero;
        m_visibilityDest = Vec2d::Zero;
        m_visibilityLast = Vec2d::Zero; 
        m_visibilityPrevious = Vec2d::Zero;
        m_visibilitySwitchPlayerTargetTime = 1.f;
        m_visibilityOffsetDest = m_CMCur.m_camModifierTemplate->m_subjectMainVisibilityHorizontal;
        m_visibilityOffsetLast = m_visibilityOffsetCur;
        m_visibilitySwitchAxeTime = 1.f;
        m_visibilitySwitchSpeed = 1.f;

        m_visibilitySecretAreaZCur = 0.f;
        m_visibilitySecretAreaZLast = 0.f;
        m_visibilitySecretAreaZTime = 0.f;
    }        

    void InGameCameraComponent::projectAABB(AABB& _aabb, f32 _originalZ, f32 _newZ)
    {
        if ( m_cameraRealPos.m_z <= _newZ || _newZ <= _originalZ )
            return;

        Vec3d aabbMin( _aabb.getMin().m_x, _aabb.getMin().m_y, _originalZ);
        Vec3d aabbMax( _aabb.getMax().m_x, _aabb.getMax().m_y, _originalZ);

        f32 dummy;       
        Plane objectPlane(Vec3d(0,0,1), _newZ);

        objectPlane.testLineIntersection(m_cameraRealPos, aabbMin-m_cameraRealPos, aabbMin, dummy);
        objectPlane.testLineIntersection(m_cameraRealPos, aabbMax-m_cameraRealPos, aabbMax, dummy);

        _aabb.setMin(aabbMin.truncateTo2D());
        _aabb.setMax(aabbMax.truncateTo2D());
    }

    //////////////////////////////////////////////////////////////////////////
    /// Screen Target Average
    void InGameCameraComponent::setScreenTargetAverage()
    {
        const f32 depth = m_cameraTargetAveragePos.m_z -m_subjectPosZ/* -m_constraintSecretAreaZ*/;
        Vec2d screenTargetLongBy2;
        screenTargetLongBy2.m_y = f32_Abs( tanf(m_CMCur.m_focale *0.5f) *depth);
        screenTargetLongBy2.m_x = screenTargetLongBy2.m_y *CAMERACONTROLLERMANAGER->m_screenRatio;

        m_screenTargetAverageAABB.setMin( m_cameraTargetAveragePos.truncateTo2D() -screenTargetLongBy2 );
        m_screenTargetAverageAABB.setMax( m_cameraTargetAveragePos.truncateTo2D() +screenTargetLongBy2 );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Screen Real
    void InGameCameraComponent::setScreenReal()
    {
        m_depth = m_cameraRealPos.m_z -m_subjectPosZ;

        m_screenRealLongBy2.m_y = f32_Abs( tanf(m_CMCur.m_focale *0.5f) *m_depth);
        m_screenRealLongBy2.m_x = m_screenRealLongBy2.m_y *CAMERACONTROLLERMANAGER->m_screenRatio;

        m_screenRealAABB.setMin( m_cameraRealPos.truncateTo2D() -m_screenRealLongBy2 );
        m_screenRealAABB.setMax( m_cameraRealPos.truncateTo2D() +m_screenRealLongBy2 );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Camera Real
    void InGameCameraComponent::setCameraReal( f32 _dt )
    {
        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
        {
            m_cameraRealPos = m_cameraTargetAveragePos +m_constraint;
            setScreenReal();
        }

        updateConstraints(_dt);
       
        m_cameraRealPos = m_cameraTargetAveragePos +m_constraint;
        
        setScreenReal();
        setDepthCoeff();        
    }

    //////////////////////////////////////////////////////////////////////////
    /// Camera Target Average
    void InGameCameraComponent::setCameraTargetAverage( f32 _dt )
    {        
        m_cameraTargetAveragePosPrevious = m_cameraTargetAveragePos;

        Vec2d pos(Vec2d::Zero);
        f32 weightTotal(0.f);

        const u32 controllersListCount = m_controllers.size();
        for ( u32 i=0; i< controllersListCount; i++ )
        {
            Controller& controller = m_controllers[i];

            pos += controller.m_cameraTargetPos.truncateTo2D() *controller.m_weight;            
            weightTotal += controller.m_weight;
        }

        if ( weightTotal > 0.f )
        {
            const f32 coeffPos = 1.f /weightTotal;
            m_cameraTargetAveragePos.m_x = pos.m_x *coeffPos;
            m_cameraTargetAveragePos.m_y = pos.m_y *coeffPos;
        }

        updateVisibility(_dt);
        setDezoomDest(_dt);
        setDezoomCur( _dt );
        setVisibilityOffset( _dt );

        m_cameraTargetAveragePos.m_x += m_visibilityCur.m_x;
        m_cameraTargetAveragePos.m_y += m_visibilityCur.m_y;
        m_cameraTargetAveragePos.m_z = m_mainController->m_cameraTargetPos.m_z +m_cameraTargetAverageDezoomCur;

        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
            m_cameraTargetAverageSpeed = Vec3d::Zero;
        else
        m_cameraTargetAverageSpeed = ( m_cameraTargetAveragePos -m_cameraTargetAveragePosPrevious) *m_deltaTimeInv;

        setScreenTargetAverage();
    }

    void InGameCameraComponent::setScreenAABBLongBy2( Vec2d& _screenLongBy2, f32 _depth )
    {
        _screenLongBy2.m_y = f32_Abs( tanf(m_CMCur.m_focale *0.5f) *_depth);
        _screenLongBy2.m_x = _screenLongBy2.m_y *CAMERACONTROLLERMANAGER->m_screenRatio;
    }

    void InGameCameraComponent::setScreenAABB( AABB& _aabb, const Vec2d& _pos, f32 _depth)
    {
        Vec2d screenLongBy2;
        setScreenAABBLongBy2( screenLongBy2, _depth );

        _aabb.setMin( _pos -screenLongBy2 );
        _aabb.setMax( _pos +screenLongBy2 );
    }

    void InGameCameraComponent::updateVisibility(f32 _dt)
    {
        // set visibility aabb and secret area Z
        if ( m_switchModifier )
        {
            if ( f32_Abs(m_CMDestFinal.m_cameraModifierDirectionNormalized.m_y) > MTH_SQRT2BY2)
                m_visibilityOffsetDest = m_CMCur.m_camModifierTemplate->m_subjectMainVisibilityVertical;
            else
                m_visibilityOffsetDest = m_CMCur.m_camModifierTemplate->m_subjectMainVisibilityHorizontal;

            m_visibilityOffsetLast = m_visibilityOffsetCur;
            m_visibilitySecretAreaZLast = m_visibilitySecretAreaZCur;

            if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported)
            {
                m_visibilitySwitchAxeTime = 1.f;
                m_visibilitySecretAreaZTime = 1.f;
            }
            else
            {
                m_visibilitySwitchAxeTime = 0.f;
                m_visibilitySecretAreaZTime = 0.f;
            }
        }

        // update visibility from secret area
        if ( m_visibilitySecretAreaZTime < 1.f )
        {
            m_visibilitySecretAreaZTime += _dt * 0.5f;
            m_visibilitySecretAreaZCur = m_visibilitySecretAreaZLast +( m_constraintSecretAreaZ -m_visibilitySecretAreaZLast)*m_curveLogistic.getValue(m_visibilitySecretAreaZTime);
        }
        else
        {
            m_visibilitySecretAreaZCur = m_constraintSecretAreaZ;
        }
        

        // set screen min aabb
        setScreenAABB( m_visibilityScreenMinAABB, m_cameraTargetAveragePos.truncateTo2D(), m_CMCur.m_cameraLookAtOffset.m_z);

        // set screen max aabb
        Vec2d screenMaxLongBy2;
        setScreenAABBLongBy2( screenMaxLongBy2, m_CMCur.m_cameraLookAtOffsetMaxInMulti.m_z +m_visibilitySecretAreaZCur);
        m_visibilityScreenMaxAABB.setMin( m_cameraTargetAveragePos.truncateTo2D() -screenMaxLongBy2 );
        m_visibilityScreenMaxAABB.setMax( m_cameraTargetAveragePos.truncateTo2D() +screenMaxLongBy2 );



        //  update time
        m_visibilitySwitchAxeTime += _dt *m_CMCur.m_camModifierTemplate->m_subjectMainVisibilitySwitchAxeSpeed;

        Vec2d offsetWanted = m_visibilityOffsetDest +m_subjectZoneNeutral;
        offsetWanted.m_x = Min( screenMaxLongBy2.m_x -offsetWanted.m_x*0.5f, offsetWanted.m_x);
        offsetWanted.m_y = Min( screenMaxLongBy2.m_y -offsetWanted.m_y*0.5f, offsetWanted.m_y);

        if ( m_visibilitySwitchAxeTime <1.f )
            m_visibilityOffsetCur = m_visibilityOffsetLast +( offsetWanted -m_visibilityOffsetLast) *m_curveLogistic.getValue(m_visibilitySwitchAxeTime);
        else
            m_visibilityOffsetCur = offsetWanted;

        m_visibilityAABB.setMin( m_mainController->m_cameraTargetPos.truncateTo2D() -m_visibilityOffsetCur);
        m_visibilityAABB.setMax( m_mainController->m_cameraTargetPos.truncateTo2D() +m_visibilityOffsetCur);
    }

    Vec2d InGameCameraComponent::getVisibilityOffset()
    {
        if ( !m_CMCur.m_isMainSubject )
            return Vec2d::Zero;

        Vec2d offsetWanted(Vec2d::Zero);
        const Vec2d anticipDelta( m_visibilityAABB.getWidth()*0.25f, m_visibilityAABB.getHeight()*0.25f);
        const AABB anticipAABB( m_visibilityScreenMaxAABB.getMin() +anticipDelta, m_visibilityScreenMaxAABB.getMax() -anticipDelta);

        // axe X
        if ( m_visibilityAABB.getMin().m_x < anticipAABB.getMin().m_x )
        {
            f32 delta = anticipAABB.getMin().m_x -m_visibilityAABB.getMin().m_x;            

            if ( delta <= anticipDelta.m_x *2.f )   
            {
                f32 coeff = delta /(anticipDelta.m_x *4.f); 
                offsetWanted.m_x = -delta *coeff;
            }
            else
            {
                offsetWanted.m_x = m_visibilityAABB.getMin().m_x -m_visibilityScreenMaxAABB.getMin().m_x;
            }               
        }
        else if ( m_visibilityAABB.getMax().m_x > anticipAABB.getMax().m_x )
        {
            f32 delta = m_visibilityAABB.getMax().m_x -anticipAABB.getMax().m_x;

            if ( delta <= anticipDelta.m_x *2.f )
            {
                f32 coeff = delta /(anticipDelta.m_x *4.f);
                offsetWanted.m_x = delta *coeff;
            }
            else
            {
                offsetWanted.m_x = m_visibilityAABB.getMax().m_x -m_visibilityScreenMaxAABB.getMax().m_x;
            }            
        }

        // axe Y
        if ( m_visibilityAABB.getMin().m_y < anticipAABB.getMin().m_y )
        {
            f32 delta = anticipAABB.getMin().m_y -m_visibilityAABB.getMin().m_y;            

            if ( delta <= anticipDelta.m_y *2.f )   
            {
                f32 coeff = Min( delta /(anticipDelta.m_y *4.f), 0.5f); 
                offsetWanted.m_y = -delta *coeff;
            }
            else
            {
                offsetWanted.m_y = m_visibilityAABB.getMin().m_y -m_visibilityScreenMaxAABB.getMin().m_y;
            }               
        }
        else if ( m_visibilityAABB.getMax().m_y > anticipAABB.getMax().m_y )
        {
            f32 delta = m_visibilityAABB.getMax().m_y -anticipAABB.getMax().m_y;

            if ( delta <= anticipDelta.m_y *2.f )
            {
                f32 coeff = delta /(anticipDelta.m_y *4.f);
                offsetWanted.m_y = delta *coeff;
            }
            else
            {
                offsetWanted.m_y = m_visibilityAABB.getMax().m_y -m_visibilityScreenMaxAABB.getMax().m_y;
            }            
        }

        return offsetWanted;
    }

    void InGameCameraComponent::setVisibilityOffset( f32 _dt )
    {       
        // reset
        if ( !m_mainController->m_wasMainController || ( m_switchModifier && !m_CMCur.m_isMainSubject ) )
        {
            m_visibilityLast = m_visibilityCur;
            m_visibilitySwitchPlayerTargetTime = 0.f;

            // set speed switch
            const f32 visibilityCoeff = Clamp( Max( f32_Abs(m_visibilityCur.m_x), f32_Abs( m_visibilityCur.m_y)), 1.f ,2.f) -1.f;
            m_visibilitySwitchSpeed = 1.f -visibilityCoeff*0.5f;

            m_visibilitySpeedBeforeSwicthPlayer = ( m_visibilityCur -m_visibilityPrevious) *m_deltaTimeInv;
        }

        m_visibilityPrevious = m_visibilityCur;

        // update drift
        m_visibilitySpeedBeforeSwicthPlayer *= 0.9f;
        m_visibilityLast += m_visibilitySpeedBeforeSwicthPlayer *_dt;

        if ( m_visibilitySwitchPlayerTargetTime < 1.f )
        {
            m_visibilitySwitchPlayerTargetTime += _dt *m_visibilitySwitchSpeed;       
            m_visibilityCur = m_visibilityLast +( getVisibilityOffset() -m_visibilityLast) *m_curveLogistic.getValue(m_visibilitySwitchPlayerTargetTime);
        }
        else
        {
            m_visibilityCur = getVisibilityOffset();
        }

        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
            m_visibilityPrevious = m_visibilityCur;
    }

    void InGameCameraComponent::setDezoomDest( f32 _deltaTime)
    {
        m_cameraTargetAverageDezoomDestWanted = 0.f;

        const f32 deltaDezoom = m_CMCur.m_cameraLookAtOffsetMaxInMulti.m_z +m_constraintSecretAreaZ -m_CMCur.m_cameraLookAtOffset.m_z;
        if ( deltaDezoom <= 0.f)     
        {
            m_cameraTargetAverageDezoomDestFinal = 0.f;
            return;        
        }

        f32 ratioX = 0.f;    
        f32 ratioY = 0.f;
        f32 deltaScreenLongX = ( m_visibilityScreenMaxAABB.getWidth() -m_visibilityScreenMinAABB.getWidth() )*0.5f;        
        f32 deltaScreenLongY = ( m_visibilityScreenMaxAABB.getHeight() -m_visibilityScreenMinAABB.getHeight() )*0.5f;

        const u32 controllersListCount = m_controllers.size();
        for ( u32 i=0; i< controllersListCount; i++ )
        {
            const Controller& controller = m_controllers[i];

            if ( !controller.isActive() && !controller.m_isPaused )
                continue;

            const Vec2d offsetZoomCurAABB = m_visibilityOffsetCur *controller.m_weight;
            const AABB subjectZoomAABB(controller.m_cameraTargetPos.truncateTo2D() -offsetZoomCurAABB, controller.m_cameraTargetPos.truncateTo2D() +offsetZoomCurAABB);            

// #ifdef DEVELOPER_JAY_CAMERA
//             GFX_ADAPTER->drawDBGAABB(subjectZoomAABB,Color::white());
// #endif

            // axe X
            if( deltaScreenLongX >0.f )
            {
                f32 deltaVisibilityX = Max( 0.f, subjectZoomAABB.getMax().m_x -m_visibilityScreenMinAABB.getMax().m_x);
                deltaVisibilityX = Max( deltaVisibilityX, m_visibilityScreenMinAABB.getMin().m_x -subjectZoomAABB.getMin().m_x);

                if ( deltaVisibilityX >0.f )
                    ratioX = Max( deltaVisibilityX /deltaScreenLongX, ratioX);
            }

            // axe y
            if( deltaScreenLongY >0.f )
            {
                f32 deltaVisibilityY = Max( 0.f, subjectZoomAABB.getMax().m_y -m_visibilityScreenMinAABB.getMax().m_y);
                deltaVisibilityY = Max( deltaVisibilityY, m_visibilityScreenMinAABB.getMin().m_y -subjectZoomAABB.getMin().m_y);

                if ( deltaVisibilityY >0.f )
                    ratioY = Max( deltaVisibilityY /deltaScreenLongY, ratioY);
            }
        }

        ratioX = Min( ratioX, 1.f);
        ratioY = Min( ratioY, 1.f);

        m_cameraTargetAverageDezoomDestWanted = Max( ratioX, ratioY) *deltaDezoom;
    }

    void InGameCameraComponent::setDezoomCur( f32 _dt )
    {
        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )  
        {
            m_cameraTargetAverageDezoomDestFinal = m_cameraTargetAverageDezoomDestWanted; 
            m_cameraTargetAverageDezoomDest = m_cameraTargetAverageDezoomDestWanted; 
            m_cameraTargetAverageDezoomCur = m_cameraTargetAverageDezoomDestWanted; 
            m_cameraTargetAverageDezoomDestSpeed = 0.f;
            m_cameraTargetAverageDezoomCurSpeed = 0.f;
            m_cameraTargetAverageDezoomDelay = 0.f;

            return;
        }

        // reset time if bounce jumped
        if ( m_resetDezoomTime ||(m_isScreenLimitCrossed && m_controllerActiveCount >1))
        {
            m_cameraTargetAverageDezoomDelay = 0.f;
        }

        // set dest final
        if ( m_cameraTargetAverageDezoomDestFinal > m_cameraTargetAverageDezoomDestWanted )
        {
            if ( m_constraintSecretAreaZ < m_constraintSecretAreaZOld )
                m_cameraTargetAverageDezoomDelay = m_CMCur.m_camModifierTemplate->m_zoomDelayInMultiplayer;
            else
                m_cameraTargetAverageDezoomDelay += _dt;

            if ( m_cameraTargetAverageDezoomDelay >= m_CMCur.m_camModifierTemplate->m_zoomDelayInMultiplayer )
            {
                m_cameraTargetAverageDezoomDestFinal = m_cameraTargetAverageDezoomDestWanted;
            }    
        }
        else
        {
            m_cameraTargetAverageDezoomDelay = 0.f;
            m_cameraTargetAverageDezoomDestFinal = m_cameraTargetAverageDezoomDestWanted;          
        }

        // set dest
        if ( m_cameraTargetAverageDezoomDest >= m_cameraTargetAverageDezoomDestFinal ) // zoom
        {
            const f32 speed = ( m_cameraTargetAverageDezoomDestFinal -m_cameraTargetAverageDezoomDest) *m_CMCur.m_camModifierTemplate->m_zoomBlendInMultiplayer;
            m_cameraTargetAverageDezoomDestSpeed = speed +m_cameraTargetAverageDezoomDestSpeed *m_CMCur.m_camModifierTemplate->m_zoomInertieInMultiplayer;
        }
        else // dezoom
        {
            const f32 speed = ( m_cameraTargetAverageDezoomDestFinal -m_cameraTargetAverageDezoomDest) *m_CMCur.m_camModifierTemplate->m_dezoomBlendInMultiplayer;
            m_cameraTargetAverageDezoomDestSpeed = speed +m_cameraTargetAverageDezoomDestSpeed *m_CMCur.m_camModifierTemplate->m_dezoomInertieInMultiplayer;
        }
        m_cameraTargetAverageDezoomDest += m_cameraTargetAverageDezoomDestSpeed;


        // set cur
        if ( m_cameraTargetAverageDezoomCur >= m_cameraTargetAverageDezoomDest ) // zoom
        {
            const f32 speed = ( m_cameraTargetAverageDezoomDest -m_cameraTargetAverageDezoomCur) *m_CMCur.m_camModifierTemplate->m_zoomBlendInMultiplayer;
            m_cameraTargetAverageDezoomCurSpeed = speed +m_cameraTargetAverageDezoomCurSpeed *m_CMCur.m_camModifierTemplate->m_zoomInertieInMultiplayer;
        }
        else // dezoom
        {
            const f32 speed = ( m_cameraTargetAverageDezoomDest -m_cameraTargetAverageDezoomCur) *m_CMCur.m_camModifierTemplate->m_dezoomBlendInMultiplayer;
            m_cameraTargetAverageDezoomCurSpeed = speed +m_cameraTargetAverageDezoomCurSpeed *m_CMCur.m_camModifierTemplate->m_dezoomInertieInMultiplayer;
        }

        m_cameraTargetAverageDezoomCur += m_cameraTargetAverageDezoomCurSpeed;
    }

    //////////////////////////////////////////////////////////////////////////
    /// Modifiers
    CameraModifierComponent* InGameCameraComponent::getCameraModifier() const 
    {
        const Actor* actorConstraint = (Actor*)GETOBJECT( m_constraintRef);
        if ( actorConstraint )        
            return actorConstraint->GetComponent<CameraModifierComponent>(); 

        const Actor* actorModifier = (Actor*)GETOBJECT( m_cameraModifierRef);
        if ( actorModifier )        
            return actorModifier->GetComponent<CameraModifierComponent>(); 

        return NULL;
    }

    bbool InGameCameraComponent::keepModifier()
    {
        if ( !m_constraintModifier )
            return bfalse;

        if ( !CAMERACONTROLLERMANAGER->isCamModifierComponentActive(m_constraintModifier))
            return bfalse;

        if ( !CAMERACONTROLLERMANAGER->isMainCameraController(this) && !CAMERACONTROLLERMANAGER->cameraControllerHasInfluence(this))
            return bfalse;

        return btrue;
//         m_constraintAABB = m_constraintModifier->getModifierAABBCur();
// 
//         if ( m_CMCur.m_constraintLeftIsActive && m_constraint.m_x >0.f && m_controllers[m_controllerMainId].m_subjectAABB.getMax().m_x < m_constraintAABB.getMin().m_x)
//             return btrue;
// 
//         if ( m_CMCur.m_constraintRightIsActive && m_constraint.m_x <0.f && m_controllers[m_controllerMainId].m_subjectAABB.getMin().m_x > m_constraintAABB.getMax().m_x)
//             return btrue;
// 
//         if ( m_CMCur.m_constraintBottomIsActive && m_constraint.m_y >0.f && m_controllers[m_controllerMainId].m_subjectAABB.getMax().m_y < m_constraintAABB.getMin().m_y)
//             return btrue;
// 
//         if ( m_CMCur.m_constraintTopIsActive && m_constraint.m_y <0.f && m_controllers[m_controllerMainId].m_subjectAABB.getMin().m_y > m_constraintAABB.getMax().m_y)
//             return btrue;
// 
//         return bfalse;
    }

    void InGameCameraComponent::setBorderBlending()
    {
        AABB borderBlendingAABB;
        const f32 blend = m_cameraModifier->getBorderBlending(m_mainController->m_subjectAABB.getCenter(), borderBlendingAABB, m_borderBlending == 1.f);

        if ( blend <1.f && m_borderBlending == 1.f )
        {
            const CameraModifierComponent* cameraModifierPrevious = (CameraModifierComponent*) CAMERACONTROLLERMANAGER->getCameraModifierComponent( borderBlendingAABB, m_mainController->m_subjectPosReal.m_z);

            if ( cameraModifierPrevious)
                cameraModifierPrevious->getUpdateData().getModifierList(m_CMPrevious);  
            else
                m_CMPrevious.init(NULL,&getTemplate()->getCMInit());

            setModifierLookAtOffset(m_CMPrevious);

            ITF_ASSERT( cameraModifierPrevious != m_cameraModifier);
        }

        m_borderBlending = blend;
    }

    void InGameCameraComponent::setModifierLookAtOffset(CamModifierUpdate& _cm)
    {        
        // set y
        if ( m_isLeaderController )
            _cm.m_directionLookAtOffset = 0.f;
        else if ( _cm.m_cameraModifierDirectionNormalized.m_y > MTH_SQRT2BY2)
            _cm.m_directionLookAtOffset = _cm.m_cameraLookAtOffsetYUp;        
        else if ( _cm.m_cameraModifierDirectionNormalized.m_y < -MTH_SQRT2BY2 )
            _cm.m_directionLookAtOffset = _cm.m_cameraLookAtOffsetYDown; 
        else
            _cm.m_directionLookAtOffset = 0.f;

        // set z
        _cm.m_cameraLookAtOffsetMaxInMulti.m_z = Min( _cm.m_cameraLookAtOffsetMaxInMulti.m_z, m_constraintDepthOffsetMax);
        _cm.m_cameraLookAtOffset.m_z = Min( _cm.m_cameraLookAtOffset.m_z, _cm.m_cameraLookAtOffsetMaxInMulti.m_z);
    }

    void InGameCameraComponent::blendingModifiers()
    {            
        //  modifiers dest cur
        if ( m_cameraModifier && m_cameraModifier->isBorderBlending())
        {
            setBorderBlending();
            m_CMDestCur.setModifierListByBorderBlending( m_CMDestFinal, m_CMPrevious, m_borderBlending);    
        }
        else
        {             
            m_borderBlending = 1.f;
            m_CMDestCur.setModifierList(m_CMDestFinal, m_CMDestFinal.m_cameraModifierBlend, m_CMDestFinal.m_cameraModifierInertie);     
        }

        // cheat zoom can't be applied on m_CMDestFinal
        if( CAMERACONTROLLERMANAGER->isCheatDezoomMax() )
            m_CMDestCur.m_cameraLookAtOffset.m_z = m_CMDestFinal.m_cameraLookAtOffsetMaxInMulti.m_z; 
        
        //  modifiers cur
        m_CMCur.setModifierList(m_CMDestCur, m_CMDestFinal.m_cameraModifierBlend, m_CMDestFinal.m_cameraModifierInertie);
    }

    void InGameCameraComponent::updateModifiers()
    {
        m_cameraModifier = NULL;
        m_constraintModifier = NULL;

        const Actor* actorModifier = (Actor*)GETOBJECT( m_cameraModifierRef);
        if ( actorModifier )        
            m_cameraModifier =  actorModifier->GetComponent<CameraModifierComponent>(); 
        else
            m_cameraModifierRef.invalidate();

        const Actor* actorConstraint = (Actor*)GETOBJECT( m_constraintRef);
        if ( actorConstraint )        
            m_constraintModifier =  actorConstraint->GetComponent<CameraModifierComponent>(); 
        else
            m_constraintRef.invalidate();
    }

    void InGameCameraComponent::setModifiers()
    {
        m_constraintModifierOnEnter = bfalse;
        m_constraintModifierOnExit = bfalse;
        m_switchModifier = bfalse;

        updateModifiers();

        const CameraModifierComponent* cameraModifierLast = m_cameraModifier;
        m_cameraModifier = (CameraModifierComponent*) CAMERACONTROLLERMANAGER->getCameraModifierComponent( m_subjectGlobalAABB, m_subjectGlobalDepthMin);

        //  modifiers dest final
        if ( m_cameraModifier )
        {
            m_cameraModifierRef = m_cameraModifier->GetActor()->getRef();

            // get modifier list
            m_cameraModifier->getUpdateData().getModifierList(m_CMDestFinal); 

            // switch constraint
            if ( m_cameraModifier != cameraModifierLast )
            {     
                m_switchModifier = btrue;
                setModifierConstraint();
            }                                       
        }
        else 
        {
            if ( keepModifier() )
            {                
                m_cameraModifier = m_constraintModifier;
                m_cameraModifierRef = m_constraintRef;

                // get modifier list
                m_cameraModifier->getUpdateData().getModifierList(m_CMDestFinal);
            }
            else
            {
                // switch constraint
                if ( m_constraintModifier )                        
                    m_constraintModifierOnExit = btrue;    

                // switch modifier
                if ( cameraModifierLast )    
                    m_switchModifier = btrue;

                m_constraintRef.invalidate();      
                m_constraintModifier = NULL;
                m_cameraModifierRef.invalidate();

                m_CMDestFinal.init(NULL,&getTemplate()->getCMInit());
            }
        }

        setModifierLookAtOffset(m_CMDestFinal);

        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
        {
            m_CMDestCur = m_CMDestFinal;
            m_CMCur = m_CMDestFinal;
            m_CMPrevious = m_CMDestFinal;
            m_borderBlending = 1.f;
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    //////////////////////////////////////////////////////////////////////////
    /// Debug
    void InGameCameraComponent::drawDebug( const CameraControllerData& _data )
    {
        if (_data.m_finalWeight < 1.f )
        {
            String txt;
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("InGameCamera does not lead camera");
            GFX_ADAPTER->drawDBGText(txt);
        }

        if( m_controllerActiveCount == 0 )
        {
            String txt;
            txt.setTextFormat("Subject Manager Count = %i", CAMERACONTROLLERMANAGER->getSubjectListCount());
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Camera Position: ( %f, %f, %f )", m_cameraRealPos.m_x, m_cameraRealPos.m_y, m_cameraRealPos.m_z );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("*** ANY SUBJECT TO FILM ***");
            GFX_ADAPTER->drawDBGText(txt);
            return;
        }

        if ( !m_mainController )
        {
            String txt;
            txt.setTextFormat("Subject Manager Count = %i", CAMERACONTROLLERMANAGER->getSubjectListCount());
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Camera Position: ( %f, %f, %f )", m_cameraRealPos.m_x, m_cameraRealPos.m_y, m_cameraRealPos.m_z );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("*** ANY MAIN CONTROLLER ***");
            GFX_ADAPTER->drawDBGText(txt);
            return;
        }

        String txt;
        f32 z = m_subjectPosZ;
        f32 debugDuration = 0.f;
        f32 debugSize = 5.f;        

        /*
        if ( m_controllers.size() == 1)
        {
#ifdef DEVELOPER_JAY_CAMERA
            Controller& controller = m_controllers[0];

            // ray cast
            txt.setTextFormat("Ray Cast Length Max: %f", controller.m_rayCastDownLengthMax );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);

            //  screen limit
            txt.setTextFormat("Screen Limit Up Speed Max: %f", controller.m_screenRealLimitUpSpeedMax );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Screen Limit Up ratio: %f", controller.m_screenRealLimitUpRatio );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Screen Limit Up Target Dist: %f", controller.m_screenRealLimitUpTargetDist );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Screen Limit Up dist: %f", controller.m_screenRealLimitUpDist );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Depth Coeff: %f", m_depthCoeff );
            GFX_ADAPTER->drawDBGText(txt);

            // subject
            txt.setTextFormat("Subject Stance Wall Slide: %i", controller.m_subjectStance.m_wallSlide);
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Stance Look Dir: ( %f, %f)", controller.m_subjectStance.m_lookDir.m_x, controller.m_subjectStance.m_lookDir.m_y );
            GFX_ADAPTER->drawDBGText(txt);    
            txt.setTextFormat("Subject Stance Sticked: %i", controller.m_subjectStance.m_stick);
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Stance Joystick Move: ( %f, %f)", controller.m_subjectStance.m_joystickMoveNormalized.m_x, controller.m_subjectStance.m_joystickMoveNormalized.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Stance Jump On Air Timer Allowed: %i", controller.m_subjectStance.m_jumpOnAirTimerAllowed );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Z: %f", m_subjectPosZ );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Speed: ( %f, %f)", controller.m_subjectSpeed.m_x, controller.m_subjectSpeed.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);

            // zone
            txt.setTextFormat("Zone Speed By Depth: ( %f, %f)", controller.m_zoneSpeedByDepth.m_x, controller.m_zoneSpeedByDepth.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Zone Speed Real: ( %f, %f)", controller.m_zoneSpeedReal.m_x, controller.m_zoneSpeedReal.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);

            // decentering
            txt.setTextFormat("Decentering Target Speed Average Real: ( %f, %f)", controller.m_cameraTargetDecenteringSpeedAverageRealOfTarget.m_x, controller.m_cameraTargetDecenteringSpeedAverageRealOfTarget.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Decentering Target Speed Average Abs: ( %f, %f)", controller.m_cameraTargetDecenteringSpeedAverageAbsOfTarget.m_x, controller.m_cameraTargetDecenteringSpeedAverageAbsOfTarget.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Decentering Inertie: ( %f, %f)", controller.m_cameraTargetDecenteringInertie.m_x, controller.m_cameraTargetDecenteringInertie.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Decentering Blend: ( %f, %f)", controller.m_cameraTargetDecenteringBlend.m_x, controller.m_cameraTargetDecenteringBlend.m_y );
            GFX_ADAPTER->drawDBGText(txt);

            txt.setTextFormat("Decentering Offset: ( %f, %f)", controller.m_cameraTargetDecentering.m_x, controller.m_cameraTargetDecentering.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);

            // camera
            txt.setTextFormat("Camera Target Speed Average Abs: ( %f, %f)", controller.m_cameraBaseMovingSpeedAverageAbsOfTarget.m_x, controller.m_cameraBaseMovingSpeedAverageAbsOfTarget.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Camera Blend Y Min In Sticked: %f", controller.m_cameraBaseMovingBlendYMinInSticked );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Camera Blend: ( %f, %f)", controller.m_cameraBaseMovingBlend.m_x, controller.m_cameraBaseMovingBlend.m_y );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Camera Speed: ( %f, %f, %f)", controller.m_cameraBaseMovingSpeed.m_x, controller.m_cameraBaseMovingSpeed.m_y, controller.m_cameraBaseMovingSpeed.m_z );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);
#endif
        }*/

        if ( CAMERACONTROLLERMANAGER->isDebugDrawModifiers() )
        {    
            // modifier
            const Actor* actorModifier = (Actor*)GETOBJECT( m_cameraModifierRef);
            if ( actorModifier )      
            {
                CameraModifierComponent* modifier = actorModifier->GetComponent<CameraModifierComponent>();
                GFX_ADAPTER->drawDBGAABB( modifier->getModifierAABBCur(), Color::white(), debugDuration, debugSize, z);
            }

            // constraint        
            const Actor* constraintActor = (Actor*)GETOBJECT( m_constraintRef );
            if ( constraintActor )
            {             
                GFX_ADAPTER->drawDBGAABB( m_constraintAABB, Color::white(), debugDuration, debugSize, z);

#ifdef DEVELOPER_JAY_CAMERA
                GFX_ADAPTER->drawDBGAABB( m_screenConstraintAABB, Color::grey(), debugDuration, debugSize*0.5f, z);

                txt.setTextFormat("");
                GFX_ADAPTER->drawDBGText(txt);
                txt.setTextFormat("Constraint Match View X = %i, Y = %i", m_constraintMatchViewX, m_constraintMatchViewY);
                GFX_ADAPTER->drawDBGText(txt);

                f32 debugConstraintSize = 2.5f;

                GFX_ADAPTER->drawDBGAABB( m_constraintAnticipAABB, Color::white(), debugDuration, debugConstraintSize, z);
#endif
            }
        }

#ifdef DEVELOPER_JAY_CAMERA
        txt.setTextFormat("Constraint Secret Area Z: %f", m_constraintSecretAreaZ );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Top Time: %f", m_constraintTopTime );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Bottom Time: %f", m_constraintBottomTime );
        GFX_ADAPTER->drawDBGText(txt);

        txt.setTextFormat("Constraint Left Time: %f", m_constraintLeftTime );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Right Time: %f", m_constraintRightTime );
        GFX_ADAPTER->drawDBGText(txt);
        //                     txt.setTextFormat("Constraint Top Blend: %f", m_constraintTopBlend );
        //             GFX_ADAPTER->drawDBGText(txt);
        //                     txt.setTextFormat("Constraint Bottom Blend: %f", m_constraintBottomBlend );
        //             GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Time: ( %f, %f, %f)", m_constraintTimeCoeffToActivate.m_x, m_constraintTimeCoeffToActivate.m_y, m_constraintTimeCoeffToActivate.m_z );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Depth max: %f", m_constraintDepthOffsetMax);
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("");
        GFX_ADAPTER->drawDBGText(txt);
#endif

        for ( u32 i=0; i<m_controllers.size(); i++)
        {
            Controller& controller = m_controllers[i];
            /*
#ifdef DEVELOPER_JAY_CAMERA
            // screen target AABB
            controller.setScreenTarget();
            GFX_ADAPTER->drawDBGAABB( controller.m_screenTargetAABB, Color::grey(), debugDuration, debugSize, z);

            // screen base AABB
         //   controller.setScreenBase();
            //             GFX_ADAPTER->drawDBGAABB( m_screenBaseAABB, Color::white(), debugDuration, debugSize, z );

            // ray cast
            Color rayCastColor = Color::yellow();
            if ( controller.m_rayCastCollisionDownFound )
                rayCastColor = Color::red();

            GFX_ADAPTER->drawDBGLine( controller.m_subjectAABB.getCenter(), controller.m_subjectAABB.getCenter() +controller.m_rayCastDown,             
                rayCastColor, debugDuration, debugSize, z);

            //  screen limit
            GFX_ADAPTER->drawDBGLine( Vec2d( controller.m_screenTargetAABB.getMax().m_x, controller.m_screenRealLimitUp), Vec2d( controller.m_screenTargetAABB.getMin().m_x, controller.m_screenRealLimitUp),             
                Color::red(), debugDuration, debugSize, z);            

            // decentering
            GFX_ADAPTER->drawDBGCircle( controller.m_cameraBasePos.truncateTo2D() +controller.m_cameraTargetDecenteringDest, 0.12f, Color::red());
            GFX_ADAPTER->drawDBGCircle( controller.m_cameraBasePos.truncateTo2D() +controller.m_cameraTargetDecentering, 0.1f, Color::blue());
#endif
*/
            // zone
            GFX_ADAPTER->drawDBGAABB( controller.m_zoneAABB, Color::black(), debugDuration, debugSize, z );

            Color colorSubject(Color::green());
            if( controller.m_isPaused )
                colorSubject = Color::grey();
            else if ( !controller.isActive())
                colorSubject = Color::black();
            else if ( controller.m_subjectStance.m_leadCamera )
                colorSubject = Color::red();
            else if( controller.m_isMainController )
                colorSubject = Color::orange();
             
            GFX_ADAPTER->drawDBGAABB( controller.m_subjectAABB, colorSubject, debugDuration, debugSize, z );

        }

        // controllers register count
        txt.setTextFormat("Subject Manager Count = %i", CAMERACONTROLLERMANAGER->getSubjectListCount());
        GFX_ADAPTER->drawDBGText(txt);

        // controllers active count
        txt.setTextFormat("Subject Camera Count = %i", m_controllerActiveCount);
        GFX_ADAPTER->drawDBGText(txt);         

// #ifdef DEVELOPER_JAY_CAMERA
//         // Camera target        
//         GFX_ADAPTER->drawDBGAABB( m_screenTargetAverageAABB, Color::grey(), debugDuration, debugSize*0.5f, z);
// #endif

        // Screen
        txt.setTextFormat("Screen Size: ( %f, %f)", m_screenRealAABB.getWidth(), m_screenRealAABB.getHeight());
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Screen Resolution: ( %i, %i)", GFX_ADAPTER->getScreenWidth(), GFX_ADAPTER->getScreenHeight());
        GFX_ADAPTER->drawDBGText(txt);

        // Camera real           
        GFX_ADAPTER->drawDBGAABB( m_screenRealAABB, Color::black(), debugDuration, debugSize, z);

        // lookAt offset
        Vec2d lookAtOffset=Vec2d::Zero;
        if ( m_controllers.size())
        {
            for ( u32 i=0; i<m_controllers.size(); i++)
            {
                Controller& controller = m_controllers[i];

                lookAtOffset += controller.m_cameraTargetDecentering;
            }

            lookAtOffset = lookAtOffset/(f32)m_controllers.size() +m_CMCur.m_cameraLookAtOffset.truncateTo2D();
            lookAtOffset.m_y += m_CMCur.m_directionLookAtOffset;
        }

#ifdef DEVELOPER_JAY_CAMERA
        txt.setTextFormat("Visibility Offset: ( %f, %f)", m_visibilityCur.m_x, m_visibilityCur.m_y);
        GFX_ADAPTER->drawDBGText(txt);        
#endif

        txt.setTextFormat("Constraint: ( %f, %f, %f)", m_constraint.m_x, m_constraint.m_y, m_constraint.m_z );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Camera Offset: ( %f, %f, %f )", lookAtOffset.m_x, lookAtOffset.m_y, m_cameraRealPos.m_z -m_subjectPosZ );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Camera Position: ( %f, %f, %f )", m_cameraRealPos.m_x, m_cameraRealPos.m_y, m_cameraRealPos.m_z );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("");
        GFX_ADAPTER->drawDBGText(txt);

        // Visibility
//         f32 mainDepth = m_mainController->m_subjectPosReal.m_z;
//         GFX_ADAPTER->drawDBGAABB( m_visibilityAABB, Color::pink(), 0.f, 1.f, mainDepth);
//         GFX_ADAPTER->drawDBGAABB( m_visibilityScreenMinAABB, Color::yellow(), 0.f, 1.f, mainDepth);            
//         GFX_ADAPTER->drawDBGAABB( m_visibilityScreenMaxAABB, Color::orange(), 0.f, 1.f, mainDepth);            
    }
#endif

}// namespace ITF
