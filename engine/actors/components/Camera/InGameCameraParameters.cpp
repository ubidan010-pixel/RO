#include "precompiled_engine.h"

#ifndef _ITF_INGAMECAMERAPARAMETERS_H_
#include "engine/actors/components/Camera/InGameCameraParameters.h"
#endif

namespace ITF
{
    BEGIN_SERIALIZATION(ConstraintExtended)        
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("offset", m_offset);
            SERIALIZE_MEMBER("timeToIncrease", m_timeToIncrease);
            SERIALIZE_MEMBER("timeToWaitBeforeDecrease", m_timeToWaitBeforeDecrease);
            SERIALIZE_MEMBER("timeToDecrease", m_timeToDecrease);            
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(CamModifier)       
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("modifierPriority", m_cameraModifierPriority);
            SERIALIZE_MEMBER("constraintLeftIsActive", m_constraintLeftIsActive);
            SERIALIZE_MEMBER("constraintRightIsActive", m_constraintRightIsActive);
            SERIALIZE_MEMBER("constraintTopIsActive", m_constraintTopIsActive);
            SERIALIZE_MEMBER("constraintBottomIsActive", m_constraintBottomIsActive); 
            SERIALIZE_MEMBER("constraintMatchView", m_constraintMatchView);
            SERIALIZE_OBJECT("constraintExtendedLeft", m_constraintExtendedLeft);
            SERIALIZE_OBJECT("constraintExtendedRight", m_constraintExtendedRight);
            SERIALIZE_OBJECT("constraintExtendedTop", m_constraintExtendedTop);
            SERIALIZE_OBJECT("constraintExtendedBottom", m_constraintExtendedBottom);
            SERIALIZE_MEMBER("blendingZoneStart", m_blendingZoneStart);
            SERIALIZE_MEMBER("blendingZoneStop", m_blendingZoneStop);                       
            SERIALIZE_MEMBER("zoneNeutral", m_zoneNeutral);
            SERIALIZE_MEMBER("useDecentering", m_useDecentering);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(CamModifierOverride)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("constraintLeftIsActive", m_constraintLeftIsActive);
            SERIALIZE_MEMBER("constraintRightIsActive", m_constraintRightIsActive);
            SERIALIZE_MEMBER("constraintTopIsActive", m_constraintTopIsActive);
            SERIALIZE_MEMBER("constraintBottomIsActive", m_constraintBottomIsActive); 
            SERIALIZE_MEMBER("constraintMatchView", m_constraintMatchView);
        END_CONDITION_BLOCK()           
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(CamModifier_Template)
        SERIALIZE_MEMBER("modifierBlend", m_cameraModifierBlend);
        SERIALIZE_MEMBER("modifierInertie", m_cameraModifierInertie);
        SERIALIZE_MEMBER("constraintDelayToActivate", m_constraintDelayToActivate);
        SERIALIZE_MEMBER("constraintDelayToDisable", m_constraintDelayToDisable);

        SERIALIZE_MEMBER("cameraLookAtOffsetYUp", m_cameraLookAtOffsetYUp); 
        SERIALIZE_MEMBER("cameraLookAtOffsetYDown", m_cameraLookAtOffsetYDown);
        SERIALIZE_MEMBER("cameraLookAtOffset", m_cameraLookAtOffset); 
        SERIALIZE_MEMBER("cameraLookAtOffsetMaxInMulti", m_cameraLookAtOffsetMaxInMulti);

        SERIALIZE_MEMBER("focale", m_focale);
        SERIALIZE_MEMBER("ignoreZ", m_ignoreZ);                

        SERIALIZE_MEMBER("zoneScaleDepthMin", m_zoneScaleDepthMin);
        SERIALIZE_MEMBER("zoneScaleDepthMax", m_zoneScaleDepthMax);
        SERIALIZE_MEMBER("zoneScaleAtDepthMin", m_zoneScaleAtDepthMin);
        SERIALIZE_MEMBER("zoneScaleAtDepthMax", m_zoneScaleAtDepthMax);
        SERIALIZE_MEMBER("zoneScaleSpeed", m_zoneScaleSpeed);
        
        SERIALIZE_MEMBER("cameraDecenteringOffsetAtSpeedMin", m_cameraDecenteringOffsetAtSpeedMin);
        SERIALIZE_MEMBER("cameraDecenteringOffsetAtSpeedMax", m_cameraDecenteringOffsetAtSpeedMax);
        SERIALIZE_MEMBER("cameraDecenteringSpeedMin", m_cameraDecenteringSpeedMin);
        SERIALIZE_MEMBER("cameraDecenteringSpeedMax", m_cameraDecenteringSpeedMax);
        SERIALIZE_MEMBER("cameraDecenteringBlendAtSpeedMin", m_cameraDecenteringBlendAtSpeedMin);
        SERIALIZE_MEMBER("cameraDecenteringBlendAtSpeedMax", m_cameraDecenteringBlendAtSpeedMax);
        SERIALIZE_MEMBER("cameraDecenteringDelay", m_cameraDecenteringDelay);
        SERIALIZE_MEMBER("cameraDecenteringInertieAtSpeedMin", m_cameraDecenteringInertieAtSpeedMin);
        SERIALIZE_MEMBER("cameraDecenteringInertieAtSpeedMax", m_cameraDecenteringInertieAtSpeedMax);
        SERIALIZE_MEMBER("cameraDecenteringSmooth", m_cameraDecenteringSmooth);
        SERIALIZE_MEMBER("cameraDecenteringSubjectLookDirWeight", m_cameraDecenteringSubjectLookDirWeight);        

        SERIALIZE_MEMBER("cameraDecenteringDepthMin", m_cameraDecenteringDepthMin);
        SERIALIZE_MEMBER("cameraDecenteringDepthMax", m_cameraDecenteringDepthMax);

        SERIALIZE_MEMBER("cameraMovingSpeedMin", m_cameraMovingSpeedMin);    
        SERIALIZE_MEMBER("cameraMovingSpeedMax", m_cameraMovingSpeedMax);    
        SERIALIZE_MEMBER("cameraMovingBlendAtSpeedMin", m_cameraMovingBlendAtSpeedMin);    
        SERIALIZE_MEMBER("cameraMovingBlendAtSpeedMax", m_cameraMovingBlendAtSpeedMax);    
        SERIALIZE_MEMBER("cameraMovingSmooth", m_cameraMovingSmooth);    
        SERIALIZE_MEMBER("cameraMovingBlendAccelerationJustSticked", m_cameraMovingBlendAccelerationJustSticked);

        SERIALIZE_MEMBER("rayCastScaleMax", m_rayCastScaleMax);    

        // lock position
        SERIALIZE_MEMBER("lockPositionBlendOnEnter", m_lockPositionBlendOnEnter);
        SERIALIZE_MEMBER("lockPositionBlendOnExit", m_lockPositionBlendOnExit);

        // screen
        SERIALIZE_MEMBER("screenLimitUpSpeedMax", m_screenLimitUpSpeedMax);
        SERIALIZE_MEMBER("screenLimitUpScale", m_screenLimitUpScale);

        // multi players                  
        SERIALIZE_MEMBER("subjectWeightUnregisterDelay", m_subjectWeightUnregisterDelay);    
        SERIALIZE_MEMBER("subjectWeightRegisterDelay", m_subjectWeightRegisterDelay);    
                
        SERIALIZE_MEMBER("subjectMainVisibilityHorizontal", m_subjectMainVisibilityHorizontal);
        SERIALIZE_MEMBER("subjectMainVisibilityVertical", m_subjectMainVisibilityVertical);
        SERIALIZE_MEMBER("cameraMovingSmoothCoeffForLeader", m_cameraMovingSmoothCoeffForLeader);
        SERIALIZE_MEMBER("subjectMainVisibilitySwitchAxeSpeed", m_subjectMainVisibilitySwitchAxeSpeed);
        
        SERIALIZE_MEMBER("zoomBlendInMultiplayer", m_zoomBlendInMultiplayer);   
        SERIALIZE_MEMBER("zoomInertieInMultiplayer", m_zoomInertieInMultiplayer);   
        SERIALIZE_MEMBER("zoomDelayInMultiplayer", m_zoomDelayInMultiplayer);           
        SERIALIZE_MEMBER("dezoomBlendInMultiplayer", m_dezoomBlendInMultiplayer);   
        SERIALIZE_MEMBER("dezoomInertieInMultiplayer", m_dezoomInertieInMultiplayer);        

        // camera drift
        SERIALIZE_MEMBER("cameraDriftDelay", m_cameraDriftDelay); 
        SERIALIZE_MEMBER("controllerDriftDelay", m_controllerDriftDelay); 

        SERIALIZE_MEMBER("isMainSubject", m_isMainSubject); 
        
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(CamModifierOverride_Template)
        SERIALIZE_MEMBER("modifierBlend", m_cameraModifierBlend);
        SERIALIZE_MEMBER("modifierInertie", m_cameraModifierInertie);
        SERIALIZE_MEMBER("constraintDelayToActivate", m_constraintDelayToActivate);
        SERIALIZE_MEMBER("constraintDelayToDisable", m_constraintDelayToDisable);
        SERIALIZE_MEMBER("cameraLookAtOffset", m_cameraLookAtOffset);
        SERIALIZE_MEMBER("cameraLookAtOffsetMaxInMulti", m_cameraLookAtOffsetMaxInMulti);
        SERIALIZE_MEMBER("focale", m_focale);    
    END_SERIALIZATION()



    void CamModifierUpdate::init( const CamModifier* _cm, const CamModifier_Template* _cmTemplate )
    {
        m_camModifier = _cm;
        m_camModifierTemplate = _cmTemplate;
        m_cameraModifierDirectionNormalized = Vec2d::Right;

        if ( m_camModifierTemplate )
        {
            m_focale = m_camModifierTemplate->m_focale;
            m_cameraLookAtOffsetYUp = m_camModifierTemplate->m_cameraLookAtOffsetYUp;
            m_cameraLookAtOffsetYDown = m_camModifierTemplate->m_cameraLookAtOffsetYDown;
            m_cameraLookAtOffset = m_camModifierTemplate->m_cameraLookAtOffset;
            m_cameraLookAtOffsetMaxInMulti = m_camModifierTemplate->m_cameraLookAtOffsetMaxInMulti;
            m_constraintDelayToActivate = m_camModifierTemplate->m_constraintDelayToActivate;
            m_constraintDelayToDisable = m_camModifierTemplate->m_constraintDelayToDisable;            
            m_cameraModifierInertie = m_camModifierTemplate->m_cameraModifierInertie;
            m_cameraModifierBlend = m_camModifierTemplate->m_cameraModifierBlend;
            m_ignoreZ = m_camModifierTemplate->m_ignoreZ;
            m_isMainSubject = m_camModifierTemplate->m_isMainSubject;
        }

        if ( m_camModifier )
        {
            m_constraintLeftIsActive = m_camModifier->m_constraintLeftIsActive;
            m_constraintRightIsActive = m_camModifier->m_constraintRightIsActive;
            m_constraintTopIsActive = m_camModifier->m_constraintTopIsActive;
            m_constraintBottomIsActive = m_camModifier->m_constraintBottomIsActive;
            m_constraintMatchView = m_camModifier->m_constraintMatchView;
        }
        else
        {
            m_constraintLeftIsActive = bfalse;
            m_constraintRightIsActive = bfalse;
            m_constraintTopIsActive = bfalse;
            m_constraintBottomIsActive = bfalse;
            m_constraintMatchView = bfalse;
        }
    }

    void CamModifierUpdate::getModifierList(CamModifierUpdate& _to ) const
    {                
        _to.m_cameraModifierDirectionNormalized = m_cameraModifierDirectionNormalized;
        _to.m_isMainSubject = m_isMainSubject;
        _to.m_ignoreZ = m_ignoreZ;

        if ( m_camModifierTemplate )
        {
            ITF_VECTOR<ingameCameraParameterTemplateFncDecl>::const_iterator it =  m_updateModifiersListTemplate.begin();
            ITF_VECTOR<ingameCameraParameterTemplateFncDecl>::const_iterator itend =  m_updateModifiersListTemplate.end();
            for(; it != itend; ++it)
            {
                (*it)(*m_camModifierTemplate, _to);
            }
        }

        if ( m_camModifier )
        {
            ITF_VECTOR<ingameCameraParameterInstanceFncDecl>::const_iterator itInstance =  m_updateModifiersListInstance.begin();
            ITF_VECTOR<ingameCameraParameterInstanceFncDecl>::const_iterator itInstanceend =  m_updateModifiersListInstance.end();
            for(; itInstance != itInstanceend; ++itInstance)
            {
                (*itInstance)(*m_camModifier, _to);
            }
        }
    }

    void CamModifierUpdate::clear()
    {
        m_updateModifiersListTemplate.clear();
        m_updateModifiersListInstance.clear();
    }

    void CamModifierUpdate::setModifierListFixed( CamModifierUpdate& _dest)
    {
        // bbool values       
        m_constraintLeftIsActive = _dest.m_constraintLeftIsActive;
        m_constraintRightIsActive = _dest.m_constraintRightIsActive;
        m_constraintTopIsActive = _dest.m_constraintTopIsActive;
        m_constraintBottomIsActive = _dest.m_constraintBottomIsActive;
        m_constraintMatchView = _dest.m_constraintMatchView; 
        m_isMainSubject = _dest.m_isMainSubject;

        // this values don't need blending
        m_cameraModifierDirectionNormalized = _dest.m_cameraModifierDirectionNormalized;
        m_constraintDelayToActivate = _dest.m_constraintDelayToActivate;
        m_constraintDelayToDisable = _dest.m_constraintDelayToDisable;
        m_cameraModifierBlend = _dest.m_cameraModifierBlend;
        m_cameraModifierInertie = _dest.m_cameraModifierInertie;
    }

    void CamModifierUpdate::setModifierList(CamModifierUpdate& _dest, f32 _blend, f32 _inertie )
    {
        setModifierListFixed( _dest);

        //  blending values
        blendModifierVec3d(_dest.m_cameraLookAtOffset, m_cameraLookAtOffset, m_cameraLookAtOffset_SpeedBlend, _blend, _inertie);
        blendModifierVec3d(_dest.m_cameraLookAtOffsetMaxInMulti, m_cameraLookAtOffsetMaxInMulti, m_cameraLookAtOffset_SpeedBlendMaxInMulti, _blend, _inertie);
        blendModifierF32(_dest.m_directionLookAtOffset, m_directionLookAtOffset, m_directionLookAtOffset_SpeedBlend, _blend, _inertie);
    }

    void CamModifierUpdate::setModifierListByBorderBlending(CamModifierUpdate& _dest, CamModifierUpdate& _last, f32 _blendingBorder )
    {
        setModifierListFixed( _dest);

        //  blending values
        borderBlendingModifierVec3d( m_cameraLookAtOffset, _dest.m_cameraLookAtOffset, _last.m_cameraLookAtOffset, _blendingBorder);
        borderBlendingModifierVec3d( m_cameraLookAtOffsetMaxInMulti, _dest.m_cameraLookAtOffsetMaxInMulti, _last.m_cameraLookAtOffsetMaxInMulti, _blendingBorder);
        borderBlendingModifierF32( m_directionLookAtOffset, _dest.m_directionLookAtOffset, _last.m_directionLookAtOffset, _blendingBorder);
    }

    void CamModifierUpdate::blendModifierVec3d( Vec3d& _dest, Vec3d& _cur, Vec3d& _curSpeed, f32 _blend, f32 _inertie )
    {
        _curSpeed = ( _dest -_cur) *_blend + _curSpeed *_inertie;
        _cur += _curSpeed;
    }

    void CamModifierUpdate::blendModifierF32( f32& _dest, f32& _cur, f32& _curSpeed, f32 _blend, f32 _inertie )
    {
        _curSpeed = ( _dest -_cur) *_blend + _curSpeed *_inertie;
        _cur += _curSpeed;
    }

    void CamModifierUpdate::borderBlendingModifierVec3d( Vec3d& _cur, Vec3d& _dest, Vec3d& _last, f32 _blend)
    {
        _cur = _last +( _dest -_last) *_blend;
    }

    void CamModifierUpdate::borderBlendingModifierF32( f32& _cur, f32& _dest, f32& _last, f32 _blend)
    {
        _cur = _last +( _dest -_last) *_blend;
    }
}