#include "precompiled_engine.h"

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_INGAMECAMERAPARAMETERS_H_
#include "engine/actors/components/Camera/InGameCameraParameters.h"
#endif // _ITF_INGAMECAMERAPARAMETERS_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(CameraModifierComponent)
    BEGIN_SERIALIZATION_CHILD(CameraModifierComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_OBJECT("CM",m_CM);
            SERIALIZE_OBJECT("CM_override",m_CM_override);
        END_CONDITION_BLOCK()

        SERIALIZE_FUNCTION(updateModifiersList, ESerialize_PropertyEdit_Load);
    END_SERIALIZATION()
    
    IMPLEMENT_OBJECT_RTTI(CameraModifierComponent_Template)
    BEGIN_SERIALIZATION(CameraModifierComponent_Template)
        SERIALIZE_OBJECT("CM",m_CM);
        SERIALIZE_OBJECT("CM_override",m_CM_override);
    END_SERIALIZATION()

    CameraModifierComponent_Template::CameraModifierComponent_Template()
    {
    }

    CameraModifierComponent_Template::~CameraModifierComponent_Template()
    {
    }

    CameraModifierComponent::CameraModifierComponent()
    : Super()
    , m_isConstraint(bfalse)
    , m_extendedOffsetMinCur(Vec2d::Zero)
    , m_extendedOffsetMaxCur(Vec2d::Zero)
    , m_extendedOffsetMinDest(Vec2d::Zero)
    , m_extendedOffsetMaxDest(Vec2d::Zero)
    , m_isBorderBlending(bfalse)
    {
        m_curveLogistic.m_xMax = 10.f; // to 1
        m_curveLogistic.m_xMin = -10.f; // to 0
    }

    CameraModifierComponent::~CameraModifierComponent()
    {
        CAMERACONTROLLERMANAGER->unregisterCamModifierComponent( this );
    }

    void CameraModifierComponent::onActorLoaded(Pickable::HotReloadType _hotReload) 
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPause,829919267), this);

        m_CM_override.m_template = &getTemplate()->getCMOverride();
        m_Update.init(&m_CM,&getTemplate()->getCM());

        init();           

        //  création de la liste des params a updater
        updateModifiersList();

        CAMERACONTROLLERMANAGER->registerCamModifierComponent( this );
    }

    void CameraModifierComponent::onEvent(Event* _event)
    {
        if ( EventPause* onPause = _event->DynamicCast<EventPause>(ITF_GET_STRINGID_CRC(EventPause,829919267)) )
        {
            if (onPause->getPause())
            {
                CAMERACONTROLLERMANAGER->unregisterCamModifierComponent( this);
            }
            else
            {
                CAMERACONTROLLERMANAGER->registerCamModifierComponent( this);
            }
        }
    }

    void CameraModifierComponent::updateModifiersList()
    {
        //  clear list
        m_Update.m_updateModifiersListTemplate.clear();
        m_Update.m_updateModifiersListInstance.clear();

        if ( m_CM_override.m_template->m_focale ) m_Update.m_updateModifiersListTemplate.push_back(CamModifierUpdate::update_focale);
        if ( m_CM_override.m_template->m_cameraLookAtOffset ) m_Update.m_updateModifiersListTemplate.push_back(CamModifierUpdate::update_cameraLookAtOffset);
        if ( m_CM_override.m_template->m_cameraLookAtOffsetMaxInMulti ) m_Update.m_updateModifiersListTemplate.push_back(CamModifierUpdate::update_cameraLookAtOffsetMaxInMulti);
        if ( m_CM_override.m_template->m_constraintDelayToDisable ) m_Update.m_updateModifiersListTemplate.push_back(CamModifierUpdate::update_constraintDelayToDisable);
        if ( m_CM_override.m_template->m_constraintDelayToActivate ) m_Update.m_updateModifiersListTemplate.push_back(CamModifierUpdate::update_constraintDelayToActivate);
        if ( m_CM_override.m_template->m_cameraModifierInertie ) m_Update.m_updateModifiersListTemplate.push_back(CamModifierUpdate::update_cameraModifierInertie);
        if ( m_CM_override.m_template->m_cameraModifierBlend ) m_Update.m_updateModifiersListTemplate.push_back(CamModifierUpdate::update_cameraModifierBlend);       
        if ( m_CM_override.m_constraintLeftIsActive ) m_Update.m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintLeftIsActive);
        if ( m_CM_override.m_constraintRightIsActive ) m_Update.m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintRightIsActive);
        if ( m_CM_override.m_constraintTopIsActive ) m_Update.m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintTopIsActive);
        if ( m_CM_override.m_constraintBottomIsActive ) m_Update.m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintBottomIsActive);
        if ( m_CM_override.m_constraintMatchView ) m_Update.m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintMatchView);
    }

    void CameraModifierComponent::tryToSetAABBToRatio16By9( AABB &_aabb)
    {
        const f32 aabbHeight = _aabb.getHeight();

        if ( aabbHeight > 0.f )
        {
            const f32 aabbWidth = _aabb.getWidth();
            const f32 aabbRatio = aabbWidth /aabbHeight;
            const f32 deltaRatio = MTH_16BY9 -aabbRatio;

            if ( deltaRatio < 0.f )
            {
                const f32 deltaX = aabbWidth -aabbHeight *MTH_16BY9;
                ITF_ASSERT(deltaX>0.f);
                if ( deltaX <= 1.f )
                {
                    const Vec2d offset( deltaX *0.5f, 0.f);
                    _aabb.setMin( _aabb.getMin() +offset );
                    _aabb.setMax( _aabb.getMax() -offset );
                }
            }
            else if ( deltaRatio > 0.f )
            {
                const f32 deltaY = aabbHeight -aabbWidth *MTH_9BY16;
                ITF_ASSERT(deltaY>0.f);
                if ( deltaY <= 1.f )
                {
                    const Vec2d offset( 0.f, deltaY *0.5f);
                    _aabb.setMin( _aabb.getMin() +offset );
                    _aabb.setMax( _aabb.getMax() -offset );
                }
            }            
        }
    }

    void CameraModifierComponent::updateAABB()
    {
        AABB aabb(GetActor()->getPos().truncateTo2D() - GetActor()->getScale() * 0.5f);
        aabb.grow(GetActor()->getPos().truncateTo2D() + GetActor()->getScale() * 0.5f);

        tryToSetAABBToRatio16By9(aabb);

        GetActor()->changeRelativeAABBFromAbsolute(aabb);
    }

    void CameraModifierComponent::init()
    {
        updateAABB();

        //  sens map
        f32_CosSin(GetActor()->getAngle(), &m_Update.m_cameraModifierDirectionNormalized.m_x, &m_Update.m_cameraModifierDirectionNormalized.m_y);
        if ( GetActor()->getScale().m_x <0.f )
            m_Update.m_cameraModifierDirectionNormalized.m_x = -m_Update.m_cameraModifierDirectionNormalized.m_x; 
        if ( GetActor()->getScale().m_y <0.f )
            m_Update.m_cameraModifierDirectionNormalized.m_y = -m_Update.m_cameraModifierDirectionNormalized.m_y; 

        ITF_ASSERT(m_Update.m_cameraModifierDirectionNormalized.isNormalized());

        initBorderBlendings();
        initConstraintsExtended();

        // constraint active
        m_isConstraint =    ( m_CM_override.m_constraintLeftIsActive && m_CM.m_constraintLeftIsActive )
                        ||  ( m_CM_override.m_constraintRightIsActive && m_CM.m_constraintRightIsActive )
                        ||  ( m_CM_override.m_constraintTopIsActive && m_CM.m_constraintTopIsActive )
                        ||  ( m_CM_override.m_constraintBottomIsActive && m_CM.m_constraintBottomIsActive );

        m_CM.m_zoneNeutral.m_x = Max( 0.f, m_CM.m_zoneNeutral.m_x);
        m_CM.m_zoneNeutral.m_y = Max( 0.f, m_CM.m_zoneNeutral.m_y);
    }

    void CameraModifierComponent::limitBlendingZone( bbool _isVertical)
    {
        f32 longMax;
        
        if ( _isVertical )
            longMax = GetActor()->getAABB().getHeight();
        else
            longMax = GetActor()->getAABB().getWidth();

        if( m_CM.m_blendingZoneStart && m_CM.m_blendingZoneStop )
        {
            f32 longDelta = m_CM.m_blendingZoneStart +m_CM.m_blendingZoneStop -longMax;

            if ( longDelta >0.f)
            {
                m_CM.m_blendingZoneStart -= longDelta *0.5f;
                m_CM.m_blendingZoneStop -= longDelta *0.5f;
            }
        }
        else
        {
            m_CM.m_blendingZoneStart = Min( m_CM.m_blendingZoneStart, longMax);
            m_CM.m_blendingZoneStop = Min( m_CM.m_blendingZoneStop, longMax);
        }
    }

    void CameraModifierComponent::initBorderBlendings()
    {
        m_borderBlendingLeft.init();
        m_borderBlendingRight.init();
        m_borderBlendingTop.init();
        m_borderBlendingBottom.init();

        m_CM.m_blendingZoneStart = f32_Abs(m_CM.m_blendingZoneStart);
        m_CM.m_blendingZoneStop = f32_Abs(m_CM.m_blendingZoneStop);
     
        if ( m_Update.m_cameraModifierDirectionNormalized.m_y <= -MTH_SQRT2BY2 ) // TOP
        {
            limitBlendingZone(btrue);

            if ( m_CM.m_blendingZoneStart )
            {
                m_borderBlendingTop.m_stop = GetActor()->getAABB().getMax().m_y -m_CM.m_blendingZoneStart;
                m_borderBlendingTop.m_ratio = 1.f /m_CM.m_blendingZoneStart;
            }

            if ( m_CM.m_blendingZoneStop )
            {
                m_borderBlendingBottom.m_stop = GetActor()->getAABB().getMin().m_y +m_CM.m_blendingZoneStop;
                m_borderBlendingBottom.m_ratio = 1.f /m_CM.m_blendingZoneStop;
            }
        }
        else if ( m_Update.m_cameraModifierDirectionNormalized.m_y >= MTH_SQRT2BY2 ) // BOTTOM
        {
            limitBlendingZone(btrue);

            if ( m_CM.m_blendingZoneStart )
            {
                m_borderBlendingBottom.m_stop = GetActor()->getAABB().getMin().m_y +m_CM.m_blendingZoneStart;
                m_borderBlendingBottom.m_ratio = 1.f /m_CM.m_blendingZoneStart;
            }
            if ( m_CM.m_blendingZoneStop )
            {
                m_borderBlendingTop.m_stop = GetActor()->getAABB().getMax().m_y -m_CM.m_blendingZoneStop;
                m_borderBlendingTop.m_ratio = 1.f /m_CM.m_blendingZoneStop;
            }
        }
        else if ( m_Update.m_cameraModifierDirectionNormalized.m_x >= MTH_SQRT2BY2 ) // LEFT
        {
            limitBlendingZone(bfalse);

            if ( m_CM.m_blendingZoneStart )
            {
                m_borderBlendingLeft.m_stop = GetActor()->getAABB().getMin().m_x +m_CM.m_blendingZoneStart;
                m_borderBlendingLeft.m_ratio = 1.f /m_CM.m_blendingZoneStart;
            }
            if ( m_CM.m_blendingZoneStop )
            {
                m_borderBlendingRight.m_stop = GetActor()->getAABB().getMax().m_x -m_CM.m_blendingZoneStop;
                m_borderBlendingRight.m_ratio = 1.f /m_CM.m_blendingZoneStop;
            }
        }
        else // RIGHT   
        {
            limitBlendingZone(bfalse);

            if ( m_CM.m_blendingZoneStart )
            {
                m_borderBlendingRight.m_stop = GetActor()->getAABB().getMax().m_x -m_CM.m_blendingZoneStart;
                m_borderBlendingRight.m_ratio = 1.f /m_CM.m_blendingZoneStart;
            }
            if ( m_CM.m_blendingZoneStop )
            {
                m_borderBlendingLeft.m_stop = GetActor()->getAABB().getMin().m_x +m_CM.m_blendingZoneStop;
                m_borderBlendingLeft.m_ratio = 1.f /m_CM.m_blendingZoneStop;
            }
        } 

        m_isBorderBlending = m_borderBlendingTop.m_ratio>0.f || m_borderBlendingBottom.m_ratio>0.f || m_borderBlendingRight.m_ratio>0.f || m_borderBlendingLeft.m_ratio>0.f;
    }

    void CameraModifierComponent::initConstraintsExtended()
    {
        if ( m_CM_override.m_constraintLeftIsActive && m_CM.m_constraintLeftIsActive )
            initConstraintExtended( m_CM.m_constraintExtendedLeft );
        else
            m_CM.m_constraintExtendedLeft.m_offset = 0.f;

        if ( m_CM_override.m_constraintRightIsActive && m_CM.m_constraintRightIsActive )
            initConstraintExtended( m_CM.m_constraintExtendedRight );
        else
            m_CM.m_constraintExtendedRight.m_offset = 0.f;

        if ( m_CM_override.m_constraintTopIsActive && m_CM.m_constraintTopIsActive )
            initConstraintExtended( m_CM.m_constraintExtendedTop );
        else
            m_CM.m_constraintExtendedTop.m_offset = 0.f;

        if ( m_CM_override.m_constraintBottomIsActive && m_CM.m_constraintBottomIsActive )
            initConstraintExtended( m_CM.m_constraintExtendedBottom );
        else
            m_CM.m_constraintExtendedBottom.m_offset = 0.f;
    }

    void CameraModifierComponent::setConstraintExtendedTimeCoeff( f32& _timeCoeff, f32 _time ) const
    {
        if ( _time )
            _timeCoeff = 1.f /_time;
        else
            _timeCoeff = F32_INFINITY *getSign(_time);
    }

    void CameraModifierComponent::initConstraintExtended( ConstraintExtended& _constraint )
    {
        _constraint.m_offset = f32_Abs(_constraint.m_offset);
        _constraint.m_timeToIncrease = f32_Abs(_constraint.m_timeToIncrease);
        _constraint.m_timeToDecrease= Max( 5.f, _constraint.m_timeToDecrease);        
        _constraint.m_timeToWaitBeforeDecrease = Max( 2.f, _constraint.m_timeToWaitBeforeDecrease);
    }

    f32 CameraModifierComponent::getExtendedDecreaseOffset( ConstraintExtended& _constraintExtended, ExtendedParamsDecrease& _decrease, f32 _deltaTime)
    {
        _decrease.m_time += _deltaTime *_decrease.m_timeCoeff;
        _decrease.m_time = Max( 0.f, _decrease.m_time);

        if ( _decrease.m_time <= 0.f )
            return 0.f;       

        return m_curveLogistic.getValue(_decrease.m_time) *_constraintExtended.m_offset;
    }

    void CameraModifierComponent::updateExtendedParams( ConstraintExtended& _constraintExtended, ExtendedParams& _extendedParams, f32& _cur, f32& _dest, f32 _deltaTime)
    {
        if ( _extendedParams.m_state != Extended_None )
        {
            // old decrease
            f32 offsetDecreaseOld = getExtendedDecreaseOffset( _constraintExtended, _extendedParams.m_decrease, _deltaTime);

            _extendedParams.m_time += _deltaTime *_extendedParams.m_timeCoeff;
            _extendedParams.m_time = Clamp( _extendedParams.m_time, 0.f, 1.f);

            if ( _extendedParams.m_state == Extended_WaitToDecrease && offsetDecreaseOld == 0.f )
            {
                if ( _extendedParams.m_time >= 1.f )
                {
                    _extendedParams.m_state = Extended_Decrease;
                    setConstraintExtendedTimeCoeff( _extendedParams.m_timeCoeff, -_constraintExtended.m_timeToDecrease);                                   
                } 
            }
            else
            {                
                _cur = m_curveLogistic.getValue(_extendedParams.m_time) *(_constraintExtended.m_offset -offsetDecreaseOld) +offsetDecreaseOld;

                // stop update
                if ( offsetDecreaseOld == 0.f )
                {
                    if ( _extendedParams.m_state == Extended_Decrease && _extendedParams.m_time == 0.f )
                    {
                        _dest = 0.f; 
                        _extendedParams.m_state = Extended_None;
                    }
                    else if ( _extendedParams.m_state == Extended_Increase && _extendedParams.m_time >= 1.f )
                    {
                        _extendedParams.m_time = 0.f;
                        setConstraintExtendedTimeCoeff( _extendedParams.m_timeCoeff, _constraintExtended.m_timeToWaitBeforeDecrease);                 

                        _extendedParams.m_state = Extended_WaitToDecrease;
                    }
                }
            }
        }
    }

    void CameraModifierComponent::setConstraintExtentedTimeMin( ConstraintExtended& _constraint, ExtendedParams& _params, f32 _delta) const
    {
        if ( _params.m_time <1.f )
        {
            f32 timeMin = _delta/_constraint.m_offset;
            _params.m_time = Max( _params.m_time, timeMin);
        }
    }

    void CameraModifierComponent::setConstraintExtended( ConstraintExtended& _constraint, ExtendedParams& _params, f32& _dest, f32 _delta ) const
    {
        if ( _params.m_state == Extended_Increase )
        {
            setConstraintExtentedTimeMin( _constraint, _params, _delta);
            return;
        }

        if ( _params.m_state == Extended_WaitToDecrease )
        {
            _params.m_time = 0.f;
            return;
        }
        
        if ( _params.m_state == Extended_Decrease )
        {
            _params.m_decrease.m_time = _params.m_time;
            _params.m_decrease.m_timeCoeff = _params.m_timeCoeff;
        }

        _params.m_time = 0.f;  
        setConstraintExtentedTimeMin( _constraint, _params, _delta);

        _params.m_state = Extended_Increase;         
        setConstraintExtendedTimeCoeff( _params.m_timeCoeff, _constraint.m_timeToIncrease);

        _dest = _constraint.m_offset;
    }

    void CameraModifierComponent::updateConstraintExtended( f32 _deltaTime )
    {
        updateExtendedParams( m_CM.m_constraintExtendedLeft, m_extendedLeft, m_extendedOffsetMinCur.m_x, m_extendedOffsetMinDest.m_x, _deltaTime);
        updateExtendedParams( m_CM.m_constraintExtendedRight, m_extendedRight, m_extendedOffsetMaxCur.m_x, m_extendedOffsetMaxDest.m_x, _deltaTime);
        updateExtendedParams( m_CM.m_constraintExtendedBottom, m_extendedBottom, m_extendedOffsetMinCur.m_y, m_extendedOffsetMinDest.m_y, _deltaTime);
        updateExtendedParams( m_CM.m_constraintExtendedTop, m_extendedTop, m_extendedOffsetMaxCur.m_y, m_extendedOffsetMaxDest.m_y, _deltaTime);
    }

    void CameraModifierComponent::Update( f32 _deltaTime )
    {
        updateAABB();
        updateConstraintExtended( _deltaTime );
    }

    void CameraModifierComponent::setConstraintExtendedLeft(f32 _delta)
    { 
        setConstraintExtended( m_CM.m_constraintExtendedLeft, m_extendedLeft, m_extendedOffsetMinDest.m_x, _delta);
    }

    void CameraModifierComponent::setConstraintExtendedRight( f32 _delta) 
    { 
        setConstraintExtended( m_CM.m_constraintExtendedRight, m_extendedRight, m_extendedOffsetMaxDest.m_x, _delta);
    }

    void CameraModifierComponent::setConstraintExtendedBottom(f32 _delta)
    { 
        setConstraintExtended( m_CM.m_constraintExtendedBottom, m_extendedBottom, m_extendedOffsetMinDest.m_y, _delta);
    }

    void CameraModifierComponent::setConstraintExtendedTop(f32 _delta)   
    { 
        setConstraintExtended( m_CM.m_constraintExtendedTop, m_extendedTop, m_extendedOffsetMaxDest.m_y, _delta);
    }

    void CameraModifierComponent::setConstraintExtendedLeftToDest()
    { 
        m_extendedLeft.m_time = 1.f;
        m_extendedOffsetMinCur.m_x = m_extendedOffsetMinDest.m_x;
    }

    void CameraModifierComponent::setConstraintExtendedRightToDest()
    { 
        m_extendedRight.m_time = 1.f;
        m_extendedOffsetMaxCur.m_x = m_extendedOffsetMaxDest.m_x;
    }

    void CameraModifierComponent::setConstraintExtendedBottomToDest()
    { 
        m_extendedBottom.m_time = 1.f;
        m_extendedOffsetMinCur.m_y = m_extendedOffsetMinDest.m_y;
    }

    void CameraModifierComponent::setConstraintExtendedTopToDest()
    { 
        m_extendedTop.m_time = 1.f;
        m_extendedOffsetMaxCur.m_y = m_extendedOffsetMaxDest.m_y;
    }

    AABB CameraModifierComponent::getModifierAABBCur()  const 
    { 
        return AABB( m_actor->getAABB().getMin() -m_extendedOffsetMinCur, m_actor->getAABB().getMax() +m_extendedOffsetMaxCur); 
    }

    AABB CameraModifierComponent::getModifierAABBDest()  const 
    { 
        return AABB( m_actor->getAABB().getMin() -m_extendedOffsetMinDest, m_actor->getAABB().getMax() +m_extendedOffsetMaxDest); 
    }

    AABB CameraModifierComponent::getModifierAABBMax()  const 
    { 
        return AABB( m_actor->getAABB().getMin() -Vec2d(m_CM.m_constraintExtendedLeft.m_offset,m_CM.m_constraintExtendedBottom.m_offset), 
                    m_actor->getAABB().getMax() +Vec2d(m_CM.m_constraintExtendedRight.m_offset,m_CM.m_constraintExtendedTop.m_offset)); 
    }

    f32 CameraModifierComponent::getBorderBlending( const Vec2d& _pos, AABB& _borderAABB, bbool _setAABB) const 
    {
        f32 blend = 1.f;

        if ( m_borderBlendingLeft.m_ratio && _pos.m_x < m_borderBlendingLeft.m_stop )
        {
            blend = ( _pos.m_x -m_actor->getAABB().getMin().m_x ) *m_borderBlendingLeft.m_ratio;

            if ( _setAABB )
            {
                _borderAABB.setMin( m_actor->getAABB().getMin() -Vec2d(1.01f,0.f));
                _borderAABB.setMax( m_actor->getAABB().getMinXMaxY() -Vec2d(0.01f,0.f));
            }
        }
        else if ( m_borderBlendingRight.m_ratio && _pos.m_x > m_borderBlendingRight.m_stop )
        {
            blend = ( m_actor->getAABB().getMax().m_x -_pos.m_x ) *m_borderBlendingRight.m_ratio;

            if ( _setAABB )
            {
                _borderAABB.setMin( m_actor->getAABB().getMaxXMinY() +Vec2d(0.01f,0.f));
                _borderAABB.setMax( m_actor->getAABB().getMax() +Vec2d(1.01f,0.f));
            }
        }
        else if ( m_borderBlendingBottom.m_ratio && _pos.m_y < m_borderBlendingBottom.m_stop )
        {
            blend = ( _pos.m_y -m_actor->getAABB().getMin().m_y ) *m_borderBlendingBottom.m_ratio;

            if ( _setAABB )
            {
                _borderAABB.setMin( m_actor->getAABB().getMin() -Vec2d(0.f,1.01f));
                _borderAABB.setMax( m_actor->getAABB().getMaxXMinY() -Vec2d(0.f,0.01f));
            }
        }
        else if ( m_borderBlendingTop.m_ratio && _pos.m_y > m_borderBlendingTop.m_stop )
        {
            blend = ( m_actor->getAABB().getMax().m_y -_pos.m_y ) *m_borderBlendingTop.m_ratio;

            if ( _setAABB )
            {
                _borderAABB.setMin( m_actor->getAABB().getMinXMaxY() +Vec2d(0.f,0.01f));
                _borderAABB.setMax( m_actor->getAABB().getMax() +Vec2d(0.f,1.01f));
            }
        }

        return Clamp( blend, 0.f, 1.f);
    }

    void CameraModifierComponent::resetExtension()
    {
        if ( isConstraintExtended() )
        {
            m_extendedRight.reset();
            m_extendedLeft.reset();
            m_extendedTop.reset();
            m_extendedBottom.reset();

            m_extendedOffsetMinCur = Vec2d::Zero;
            m_extendedOffsetMaxCur = Vec2d::Zero;
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void CameraModifierComponent::drawDebug()
    {
        const Actor* actor = GetActor();
        if (!actor)
            return;

        f32 lineSizeConstraint = 10.f;
        f32 debugDuration = 0.f;
        f32 z = actor->getPos().m_z;
        AABB aabb = actor->getAABB();
        GFX_ADAPTER->drawDBGAABB( aabb, Color::red(), 0.f, 1.f, z);

        // extended
        GFX_ADAPTER->drawDBGAABB( getModifierAABBMax(), Color::red(), 0.f, 1.f, z);

        //  constraint
        const CamModifier& cm = getCM();

        const AABB modifierAABBCur = getModifierAABBCur();
        if ( cm.m_constraintLeftIsActive && m_CM_override.m_constraintLeftIsActive )
            GFX_ADAPTER->drawDBGLine( modifierAABBCur.getMinXMaxY(), modifierAABBCur.getMin(), Color::red(), debugDuration, lineSizeConstraint, z);  
        if ( cm.m_constraintRightIsActive && m_CM_override.m_constraintRightIsActive )                        
            GFX_ADAPTER->drawDBGLine( modifierAABBCur.getMaxXMinY(), modifierAABBCur.getMax(), Color::red(), debugDuration, lineSizeConstraint, z);  
        if ( cm.m_constraintTopIsActive && m_CM_override.m_constraintTopIsActive )
            GFX_ADAPTER->drawDBGLine( modifierAABBCur.getMax(), modifierAABBCur.getMinXMaxY(), Color::red(), debugDuration, lineSizeConstraint, z);  
        if ( cm.m_constraintBottomIsActive && m_CM_override.m_constraintBottomIsActive )
            GFX_ADAPTER->drawDBGLine( modifierAABBCur.getMin(), modifierAABBCur.getMaxXMinY(), Color::red(), debugDuration, lineSizeConstraint, z);  

        //  sens map du modifier
        if ( getCMTemplate().m_isMainSubject )
        {
            float longArrow;
            if ( actor->getScale().m_y > actor->getScale().m_x )  
                longArrow = f32_Abs(m_Update.m_cameraModifierDirectionNormalized.m_x)*actor->getScale().m_x*0.5f + (1.f-f32_Abs(m_Update.m_cameraModifierDirectionNormalized.m_x))*actor->getScale().m_y*0.5f;
            else
                longArrow = f32_Abs(m_Update.m_cameraModifierDirectionNormalized.m_y)*actor->getScale().m_y*0.5f + (1.f-f32_Abs(m_Update.m_cameraModifierDirectionNormalized.m_y))*actor->getScale().m_x*0.5f;

            GFX_ADAPTER->drawArrow(actor->get2DPos(), actor->get2DPos()+m_Update.m_cameraModifierDirectionNormalized* f32_Abs(longArrow)*0.9f, 1,0,0, 0.5f, actor->getPos().m_z, 0.f, bfalse );
        }

        // blending border
        Color borderColor(Color::red());
        borderColor.m_a = 0.25f;

        if ( m_borderBlendingLeft.m_ratio )
        {   
            GFX_ADAPTER->drawDBGTriangle( aabb.getMin(), aabb.getMinXMaxY(), Vec2d(m_borderBlendingLeft.m_stop, aabb.getMax().m_y), borderColor, 0.f, z);          
            GFX_ADAPTER->drawDBGTriangle( aabb.getMin(), Vec2d(m_borderBlendingLeft.m_stop, aabb.getMax().m_y), Vec2d(m_borderBlendingLeft.m_stop, aabb.getMin().m_y), borderColor, 0.f, z);                      
        }
        if ( m_borderBlendingRight.m_ratio )
        {
            GFX_ADAPTER->drawDBGTriangle( Vec2d(m_borderBlendingRight.m_stop, aabb.getMin().m_y), Vec2d(m_borderBlendingRight.m_stop, aabb.getMax().m_y), aabb.getMax(), borderColor, 0.f, z);             
            GFX_ADAPTER->drawDBGTriangle( Vec2d(m_borderBlendingRight.m_stop, aabb.getMin().m_y), aabb.getMax(), aabb.getMaxXMinY(), borderColor, 0.f, z);                         
        }
        if ( m_borderBlendingTop.m_ratio )
        {   
            GFX_ADAPTER->drawDBGTriangle( Vec2d(aabb.getMin().m_x, m_borderBlendingTop.m_stop), aabb.getMinXMaxY(), aabb.getMax(), borderColor, 0.f, z); 
            GFX_ADAPTER->drawDBGTriangle( Vec2d(aabb.getMin().m_x, m_borderBlendingTop.m_stop), aabb.getMax(), Vec2d(aabb.getMax().m_x, m_borderBlendingTop.m_stop), borderColor, 0.f, z);             
        }
        if ( m_borderBlendingBottom.m_ratio )
        {            
            GFX_ADAPTER->drawDBGTriangle( aabb.getMin(), Vec2d(aabb.getMin().m_x, m_borderBlendingBottom.m_stop), Vec2d(aabb.getMax().m_x, m_borderBlendingBottom.m_stop), borderColor, 0.f, z); 
            GFX_ADAPTER->drawDBGTriangle( aabb.getMin(), Vec2d(aabb.getMax().m_x, m_borderBlendingBottom.m_stop), aabb.getMaxXMinY(), borderColor, 0.f, z); 
        }

        // priority
        Vec3d pos3dIn(actor->getPos());
        Vec3d pos3dOut;
        GFX_ADAPTER->compute3DTo2D(pos3dIn, pos3dOut); 
       
        String txt;
        const f32 textSize = f32(GFX_ADAPTER->getDebugFontSize());
        txt.setTextFormat("P%i", cm.m_cameraModifierPriority);      
        GFX_ADAPTER->draw2dBox( pos3dOut.truncateTo2D(), textSize *1.5f, textSize );
        GFX_ADAPTER->drawDBGText(txt, Color::red(), pos3dOut.m_x, pos3dOut.m_y );
    }
#endif //ITF_SUPPORT_DEBUGFEATURE

}   // namespace ITF

 
