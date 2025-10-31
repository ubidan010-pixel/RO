#include "precompiled_engine.h"

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
    void InGameCameraComponent::setModifierConstraint()
    {     
        if ( m_constraintModifier == m_cameraModifier )
            return;

        if ( m_constraintModifier && !m_cameraModifier->isConstraint() )
            if ( m_constraintModifier->getModifierAABBDest().contains(m_cameraModifier->getModifierAABBDest(), 1.f) )
                return;

        if ( m_cameraModifier->isConstraint() )
        {
            m_constraintRef = m_cameraModifierRef;
            m_constraintModifier = m_cameraModifier;
            m_constraintModifierOnEnter = btrue;                    
        }
        else
        {
            if ( m_constraintModifier )
                m_constraintModifierOnExit = btrue;

            m_constraintRef = ITF_INVALID_OBJREF;
            m_constraintModifier = NULL;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// Constraints   
    void InGameCameraComponent::resetConstraints()
    {
        m_constraintTimeCoeffToActivate = Vec3d::Zero;        
        m_constraintLeftTime = 0.f;
        m_constraintRightTime = 0.f;
        m_constraintDepthTime = 0.f;
        m_constraintTopTime = 0.f;
        m_constraintBottomTime = 0.f;
        m_constraintLast = Vec3d::Zero;
        m_constraintSpeedBeforeSwicthConstraint = Vec3d::Zero;
        m_constraint = Vec3d::Zero;
        m_constraintPrevious = Vec3d::Zero;
        m_constraintLeftTimeIncreazeToMin = bfalse;
        m_constraintRightTimeIncreazeToMin = bfalse;
        m_constraintTopTimeIncreazeToMin = bfalse;
        m_constraintBottomTimeIncreazeToMin = bfalse;
        m_constraintTimeToDisable = Vec3d::Zero;
        m_constraintTimeCoeffToDisable = Vec3d::Zero;
        m_constraintRef = ITF_INVALID_OBJREF;
        m_constraintModifier = NULL;
        m_constraintDepthOffsetMax = F32_INFINITY;
        m_constraintSecretAreaZ = 0.f;
        m_constraintSecretAreaZOld = 0.f;
    }

    f32 InGameCameraComponent::getConstraintAnticipationX()
    {
        return m_constraintAABB.getMax().m_x -m_constraintAnticipAABB.getMax().m_x;
    }

    f32 InGameCameraComponent::getConstraintAnticipationY()
    {
        return m_constraintAABB.getMax().m_y -m_constraintAnticipAABB.getMax().m_y;
    }

    f32 InGameCameraComponent::getConstraintByLeft( f32 _dest )
    {
        return m_constraintLast.m_x +( _dest -m_constraintLast.m_x) *m_curveLogistic.getValue(m_constraintLeftTime);
    }

    f32 InGameCameraComponent::getConstraintByRight( f32 _dest )
    {
        return m_constraintLast.m_x +( _dest -m_constraintLast.m_x) *m_curveLogistic.getValue(m_constraintRightTime);
    }

    f32 InGameCameraComponent::getConstraintByTop( f32 _dest )
    {
        return m_constraintLast.m_y +( _dest -m_constraintLast.m_y) *m_curveLogistic.getValue(m_constraintTopTime);
    }

    f32 InGameCameraComponent::getConstraintByBottom( f32 _dest )
    {
        return m_constraintLast.m_y +( _dest -m_constraintLast.m_y) *m_curveLogistic.getValue(m_constraintBottomTime);
    }

    void InGameCameraComponent::setConstraintTimeIncreaseToMinX()
    {
        f32 deltaTargetCenterWithConstraintCenter = m_screenConstraintAABB.getCenter().m_x -m_constraintAABB.getCenter().m_x;
        f32 constraintWidthBy2 = m_constraintAABB.getWidth() *0.5f;

        if ( m_constraintLeftTimeIncreazeToMin )
        {
            m_constraintLeftTime = Max( m_constraintLeftTime, 1.f +(deltaTargetCenterWithConstraintCenter /constraintWidthBy2) );
        }

        if ( m_constraintRightTimeIncreazeToMin )
        {
            m_constraintRightTime = Max( m_constraintRightTime, 1.f -(deltaTargetCenterWithConstraintCenter /constraintWidthBy2) );
        } 
    }

    void InGameCameraComponent::setContraintTimeIncreaseToMinY()
    {
        f32 deltaTargetCenterWithConstraintCenter = m_screenConstraintAABB.getCenter().m_y -m_constraintAABB.getCenter().m_y;
        f32 constraintHeightBy2 = m_constraintAABB.getHeight() *0.5f;

        if ( m_constraintBottomTimeIncreazeToMin )
        {
            m_constraintBottomTime = Max( m_constraintBottomTime, 1.f +(deltaTargetCenterWithConstraintCenter /constraintHeightBy2) );
        }

        if ( m_constraintTopTimeIncreazeToMin )
        {
            m_constraintTopTime = Max( m_constraintTopTime, 1.f -(deltaTargetCenterWithConstraintCenter /constraintHeightBy2) );
        }
    }

    void InGameCameraComponent::setConstraintX()
    {           
        f32 leftDest = 0.f;
        f32 rightDest = 0.f;

        if ( m_CMCur.m_constraintLeftIsActive && m_CMCur.m_constraintRightIsActive 
            && ( m_screenConstraintAABB.getWidth() > m_constraintAABB.getWidth() || m_constraintMatchViewX ) )
        {          
            f32 delta = m_constraintAnticipAABB.getCenter().m_x -m_screenConstraintAABB.getCenter().m_x;
            if ( delta > 0.f )
            {
                leftDest = delta;               
            }
            else
            {
                rightDest = delta;
            }                          
        }
        else 
        {
            setConstraintTimeIncreaseToMinX();

            if ( m_CMCur.m_constraintLeftIsActive && m_screenConstraintAABB.getMin().m_x < m_constraintAnticipAABB.getMin().m_x )
            {
                f32 delta = m_constraintAnticipAABB.getMin().m_x -m_screenConstraintAABB.getMin().m_x;            
                f32 anticip = getConstraintAnticipationX();

                if ( delta <= anticip*2.f )                
                {
                    f32 coeff = Min( delta /(anticip*4.f), 0.5f);
                    leftDest = delta *coeff;                
                }
                else
                {
                    leftDest = m_constraintAABB.getMin().m_x -m_screenConstraintAABB.getMin().m_x;
                }
            }
            else if ( m_CMCur.m_constraintRightIsActive && m_screenConstraintAABB.getMax().m_x > m_constraintAnticipAABB.getMax().m_x )
            {
                f32 delta = m_screenConstraintAABB.getMax().m_x -m_constraintAnticipAABB.getMax().m_x;
                f32 anticip = getConstraintAnticipationX();

                if ( delta <= anticip*2.f )
                {
                    f32 coeff = Min( delta /(anticip*4.f), 0.5f);
                rightDest = -delta *coeff;
            }     
                else
                {                
                    rightDest = m_constraintAABB.getMax().m_x -m_screenConstraintAABB.getMax().m_x;
                }
            }     
        }
        m_constraint.m_x = getConstraintByLeft(leftDest) +getConstraintByRight(rightDest);
    }

    void InGameCameraComponent::setConstraintY()
    {           
        f32 bottomDest = 0.f;
        f32 topDest = 0.f;

        if ( m_CMCur.m_constraintBottomIsActive && m_CMCur.m_constraintTopIsActive 
            && ( m_screenConstraintAABB.getHeight() > m_constraintAABB.getHeight() || m_constraintMatchViewY ) )
        {          
            f32 delta = m_constraintAnticipAABB.getCenter().m_y -m_screenConstraintAABB.getCenter().m_y;
            if ( delta > 0.f )
            {
                bottomDest = delta;               
            }
            else
            {
                topDest = delta;
            }                          
        }
        else 
        {
            setContraintTimeIncreaseToMinY();

            if ( m_CMCur.m_constraintBottomIsActive && m_screenConstraintAABB.getMin().m_y < m_constraintAnticipAABB.getMin().m_y )
            {
                f32 delta = m_constraintAnticipAABB.getMin().m_y -m_screenConstraintAABB.getMin().m_y;            
                f32 anticip = getConstraintAnticipationY();

                if ( delta <= anticip*2.f )
                {
                    f32 coeff = Min( delta /(anticip*4.f), 0.5f);
                    bottomDest =  delta *coeff;
                }
                else
                {
                    bottomDest = m_constraintAABB.getMin().m_y -m_screenConstraintAABB.getMin().m_y;
                }
            }
            else if ( m_CMCur.m_constraintTopIsActive && m_screenConstraintAABB.getMax().m_y > m_constraintAnticipAABB.getMax().m_y )
            {
                f32 delta = m_screenConstraintAABB.getMax().m_y -m_constraintAnticipAABB.getMax().m_y;
                f32 anticip = getConstraintAnticipationY();

                if ( delta <= anticip*2.f )
                {
                    f32 coeff = Min( delta /(anticip*4.f), 0.5f);
                topDest = -delta *coeff;
            }     
                else
                {                
                    topDest = m_constraintAABB.getMax().m_y -m_screenConstraintAABB.getMax().m_y;
                }
            }     
        }

        m_constraint.m_y = getConstraintByBottom(bottomDest) +getConstraintByTop(topDest);
    }

    void InGameCameraComponent::setConstraintSecretAreaZ(f32 _screenRatio)
    {
        Vec2d extensionArea = m_constraintModifier->getExtendedOffsetMin() +m_constraintModifier->getExtendedOffsetMax();
        m_constraintSecretAreaZ = Max( extensionArea.m_x *_screenRatio, extensionArea.m_y *0.5f);
    }

    void InGameCameraComponent::setConstraintZ()
    {        
        f32 tanFocalBy2 = f32_Abs(tanf(m_CMCur.m_focale*0.5f));
        if ( tanFocalBy2== 0.f )
        {
            resetConstraintZ();
            return;
        }

        const AABB modifierAABB = m_constraintModifier->GetActor()->getAABB();

        f32 tanFocaleBy2Inv = 1.f /tanFocalBy2;
        f32 depthDest = F32_INFINITY;         
        f32 screenRatio = 0.5f;
        if( CAMERACONTROLLERMANAGER->m_screenRatio )
            screenRatio /= CAMERACONTROLLERMANAGER->m_screenRatio;

        if ( m_CMCur.m_constraintLeftIsActive && m_CMCur.m_constraintRightIsActive )
        {
            m_constraintDepthOffsetMax = modifierAABB.getWidth() *screenRatio;

            if ( m_screenTargetAverageAABB.getWidth() > m_constraintAABB.getWidth() || m_constraintMatchViewX )
            {
                depthDest = ( m_constraintAABB.getWidth() -m_screenTargetAverageAABB.getWidth() ) *screenRatio;
            }
        }

        if ( m_CMCur.m_constraintTopIsActive && m_CMCur.m_constraintBottomIsActive )
        { 
            m_constraintDepthOffsetMax = Min( m_constraintDepthOffsetMax, modifierAABB.getHeight() *0.5f);

            if ( m_screenTargetAverageAABB.getHeight() > m_constraintAABB.getHeight() || m_constraintMatchViewY )
            {     
                depthDest = Min( depthDest, ( m_constraintAABB.getHeight() -m_screenTargetAverageAABB.getHeight() ) *0.5f );                   
            }
        }        

        if ( depthDest != F32_INFINITY )
        {
            depthDest *= tanFocaleBy2Inv;

            m_constraint.m_z = m_constraintLast.m_z +( depthDest -m_constraintLast.m_z) *m_curveLogistic.getValue(m_constraintDepthTime);
        }
        else
        {
            resetConstraintZ();
        }        

        // set depth offset max
        m_constraintDepthOffsetMax *= tanFocaleBy2Inv;

        // set secret area
        setConstraintSecretAreaZ(screenRatio);
        m_constraintSecretAreaZ *= tanFocaleBy2Inv;
    }

    void InGameCameraComponent::setConstraintMatchView()
    {
        m_constraintMatchViewX = bfalse;
        m_constraintMatchViewY = bfalse;

        if ( !m_CMCur.m_constraintMatchView )
            return;

        const f32 constraintHeight = m_constraintAABB.getHeight();

        if ( constraintHeight <= 0.f )
            return;

        const f32 screenRatioModifier = m_constraintAABB.getWidth() /constraintHeight;
        const f32 deltaRatio = CAMERACONTROLLERMANAGER->m_screenRatio -screenRatioModifier;

        if ( deltaRatio > -MTH_EPSILON && deltaRatio < MTH_EPSILON )
        {
            m_constraintMatchViewX = btrue;
            m_constraintMatchViewY = btrue;
        }
        else if ( deltaRatio > 0.f )
        {
            m_constraintMatchViewX = btrue;
        }
        else
        {
            m_constraintMatchViewY = btrue;
        } 
    }

    void InGameCameraComponent::setConstraintAABB()
    {
        // cur constraint aabb
        m_constraintAABB = m_constraintModifier->getModifierAABBCur();

        // anticip constraint aabb
        Vec2d anticipOffset;
        setScreenAABBLongBy2( anticipOffset, m_CMCur.m_cameraLookAtOffset.m_z );
        anticipOffset *= 0.5f;

        m_constraintAnticipAABB.setMin( m_constraintAABB.getMin() +anticipOffset);        
        m_constraintAnticipAABB.setMax( m_constraintAABB.getMax() -anticipOffset);

        // adjust width with screen AABB
        if ( m_CMCur.m_constraintLeftIsActive && m_CMCur.m_constraintRightIsActive )
        {
            if ( m_screenConstraintAABB.getWidth() > m_constraintAnticipAABB.getWidth() )
            {
                f32 deltaBy2 = m_screenConstraintAABB.getWidth() -m_constraintAnticipAABB.getWidth();
                deltaBy2 = Min( deltaBy2, m_constraintAABB.getWidth() -m_constraintAnticipAABB.getWidth());
                deltaBy2 *= 0.5f;

                m_constraintAnticipAABB.setMin( Vec2d( m_constraintAnticipAABB.getMin().m_x -deltaBy2, m_constraintAnticipAABB.getMin().m_y) );
                m_constraintAnticipAABB.setMax( Vec2d( m_constraintAnticipAABB.getMax().m_x +deltaBy2, m_constraintAnticipAABB.getMax().m_y) );
            }
        }

        // adjust height with screen AABB
        if ( m_CMCur.m_constraintTopIsActive && m_CMCur.m_constraintBottomIsActive )
        {
            if ( m_screenConstraintAABB.getHeight() > m_constraintAnticipAABB.getHeight() )
            {
                f32 deltaBy2 = m_screenConstraintAABB.getHeight() -m_constraintAnticipAABB.getHeight();
                deltaBy2 = Min( deltaBy2, m_constraintAABB.getHeight() -m_constraintAnticipAABB.getHeight());
                deltaBy2 *= 0.5f;

                m_constraintAnticipAABB.setMin( Vec2d( m_constraintAnticipAABB.getMin().m_x, m_constraintAnticipAABB.getMin().m_y -deltaBy2) );
                m_constraintAnticipAABB.setMax( Vec2d( m_constraintAnticipAABB.getMax().m_x, m_constraintAnticipAABB.getMax().m_y +deltaBy2) );
            }
        }

        setConstraintMatchView();
    }

    void InGameCameraComponent::setConstraintTimeCoeffOnEnter()
    {
        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
        {
            m_constraintTimeCoeffToActivate = Vec3d( F32_INFINITY, F32_INFINITY, F32_INFINITY);
        }
        else
        {
            // axe x
            if ( m_CMCur.m_constraintDelayToActivate.m_x > 0.f )
                m_constraintTimeCoeffToActivate.m_x = 1.f /m_CMCur.m_constraintDelayToActivate.m_x;
            else
                m_constraintTimeCoeffToActivate.m_x = F32_INFINITY;

            // axe y
            if ( m_CMCur.m_constraintDelayToActivate.m_y > 0.f )
                m_constraintTimeCoeffToActivate.m_y = 1.f /m_CMCur.m_constraintDelayToActivate.m_y;
            else
                m_constraintTimeCoeffToActivate.m_y = F32_INFINITY;

            // axe z
            if ( m_CMCur.m_constraintDelayToActivate.m_z > 0.f )
                m_constraintTimeCoeffToActivate.m_z = 1.f /m_CMCur.m_constraintDelayToActivate.m_z;
            else
                m_constraintTimeCoeffToActivate.m_z = F32_INFINITY;
        }

        // disable time
        // axe x
        if ( m_CMCur.m_constraintDelayToDisable.m_x > 0.f )
            m_constraintTimeCoeffToDisable.m_x = 1.f /m_CMCur.m_constraintDelayToDisable.m_x;
        else
            m_constraintTimeCoeffToDisable.m_x = F32_INFINITY;

        // axe y
        if ( m_CMCur.m_constraintDelayToDisable.m_y > 0.f )
            m_constraintTimeCoeffToDisable.m_y = 1.f /m_CMCur.m_constraintDelayToDisable.m_y;
        else
            m_constraintTimeCoeffToDisable.m_y = F32_INFINITY;

        // axe z
        if ( m_CMCur.m_constraintDelayToDisable.m_z > 0.f )
            m_constraintTimeCoeffToDisable.m_z = 1.f /m_CMCur.m_constraintDelayToDisable.m_z;
        else
            m_constraintTimeCoeffToDisable.m_z = F32_INFINITY;
    }

    void InGameCameraComponent::setConstraintTimeOnEnterX()
    {
        m_constraintLeftTime = 1.f;
        m_constraintRightTime = 1.f;

        m_constraintLeftTimeIncreazeToMin = btrue;
        m_constraintRightTimeIncreazeToMin = btrue;

        if ( m_CMCur.m_constraintRightIsActive && m_CMCur.m_constraintLeftIsActive )
        {
            if ( m_screenConstraintAABB.getWidth() > m_constraintAABB.getWidth() || m_constraintMatchViewX )
            {
                f32 delta = m_constraintAnticipAABB.getCenter().m_x -m_screenConstraintAABB.getCenter().m_x;
                if ( delta > 0.f )
                {
                    m_constraintLeftTime = 0.f;
                }
                else
                {
                    m_constraintRightTime = 0.f;
                }     

                m_constraintLeftTimeIncreazeToMin = bfalse;
                m_constraintRightTimeIncreazeToMin = bfalse;

                return;
            }
        }

        if ( m_CMCur.m_constraintRightIsActive  )
        {
            if ( m_screenConstraintAABB.getMax().m_x > m_constraintAnticipAABB.getMax().m_x )
            {            
                m_constraintRightTime = 0.f;

                if ( m_screenConstraintAABB.getCenter().m_x < m_constraintAABB.getMax().m_x )
                {
                    m_constraintRightTimeIncreazeToMin = bfalse;
                }

                return;
            }
        }

        if ( m_CMCur.m_constraintLeftIsActive )
        {
            if ( m_screenConstraintAABB.getMin().m_x < m_constraintAnticipAABB.getMin().m_x )
            {
                m_constraintLeftTime = 0.f;

                if ( m_screenConstraintAABB.getCenter().m_x > m_constraintAABB.getMin().m_x )
                {
                    m_constraintLeftTimeIncreazeToMin = bfalse;
                }

                return;
            }
        }

        if ( m_constraintLast.m_x >MTH_EPSILON )
        {
            m_constraintLeftTime = 0.f;
            m_constraintLeftTimeIncreazeToMin = bfalse;

            return;
        }

        if ( m_constraintLast.m_x <-MTH_EPSILON )
        {
            m_constraintRightTime = 0.f;
            m_constraintRightTimeIncreazeToMin = bfalse;

            return;
        }        
    }

    void InGameCameraComponent::setConstraintTimeOnEnterY()
    {
        m_constraintTopTime = 1.f;
        m_constraintBottomTime = 1.f;

        m_constraintBottomTimeIncreazeToMin = btrue;
        m_constraintTopTimeIncreazeToMin = btrue;

        if ( m_CMCur.m_constraintTopIsActive && m_CMCur.m_constraintBottomIsActive )
        {
            if ( m_screenConstraintAABB.getHeight() > m_constraintAABB.getHeight() || m_constraintMatchViewY )
            {
                f32 delta = m_constraintAnticipAABB.getCenter().m_y -m_screenConstraintAABB.getCenter().m_y;
                if ( delta > 0.f )
                {
                    m_constraintBottomTime = 0.f;
                }
                else
                {
                    m_constraintTopTime = 0.f;
                }     

                m_constraintTopTimeIncreazeToMin = bfalse;
                m_constraintBottomTimeIncreazeToMin = bfalse;

                return;
            }
        }

        if ( m_CMCur.m_constraintTopIsActive )
        {
            if ( m_screenConstraintAABB.getMax().m_y > m_constraintAnticipAABB.getMax().m_y )
            {
                m_constraintTopTime = 0.f;

                if ( m_screenConstraintAABB.getCenter().m_y < m_constraintAABB.getMax().m_y  )
                {
                    m_constraintTopTimeIncreazeToMin = bfalse;
                }

                return;
            }
        }

        if ( m_CMCur.m_constraintBottomIsActive )
        {
            if ( m_screenConstraintAABB.getMin().m_y < m_constraintAnticipAABB.getMin().m_y )
            {
                m_constraintBottomTime = 0.f;

                if ( m_screenConstraintAABB.getCenter().m_y > m_constraintAABB.getMin().m_y  )
                {
                    m_constraintBottomTimeIncreazeToMin = bfalse;
                }

                return;
            }
        }

        if ( m_constraintLast.m_y <-MTH_EPSILON )
        {
            m_constraintTopTime = 0.f;
            m_constraintTopTimeIncreazeToMin = bfalse;

            return;
        }

        if ( m_constraintLast.m_y >MTH_EPSILON )
        {
            m_constraintBottomTime = 0.f;
            m_constraintBottomTimeIncreazeToMin = bfalse;

            return;
        }
    }

    void InGameCameraComponent::setConstraintTimeOnEnter()
    {    
        setConstraintTimeCoeffOnEnter();
        setConstraintTimeOnEnterX();
        setConstraintTimeOnEnterY();

        m_constraintDepthTime = 0.f;
    }

    void InGameCameraComponent::setConstraintTimeToActivate( f32 _dt )
    {
        // axe x
        f32 timeX = _dt *m_constraintTimeCoeffToActivate.m_x;
        m_constraintLeftTime += timeX;
        m_constraintRightTime += timeX;

        // axe y
        f32 timeY = _dt *m_constraintTimeCoeffToActivate.m_y;
        m_constraintTopTime += timeY;
        m_constraintBottomTime += timeY;

        // axe z
        m_constraintDepthTime += _dt *m_constraintTimeCoeffToActivate.m_z;
    }

    void InGameCameraComponent::setConstraintTimeToDisable( f32 _dt )
    {
        m_constraintTimeToDisable.m_x -= _dt *m_constraintTimeCoeffToDisable.m_x;
        m_constraintTimeToDisable.m_y -= _dt *m_constraintTimeCoeffToDisable.m_y;
        m_constraintTimeToDisable.m_z -= _dt *m_constraintTimeCoeffToDisable.m_z;
    }

    void InGameCameraComponent::resetConstraintX()
    {
        m_constraint.m_x = m_constraintLast.m_x *m_curveLogistic.getValue(m_constraintTimeToDisable.m_x);
    }

    void InGameCameraComponent::resetConstraintY()
    {
        m_constraint.m_y = m_constraintLast.m_y *m_curveLogistic.getValue(m_constraintTimeToDisable.m_y);
    }

    void InGameCameraComponent::resetConstraintZ()
    {
        m_constraint.m_z = m_constraintLast.m_z *m_curveLogistic.getValue(m_constraintTimeToDisable.m_z);
    }

    void InGameCameraComponent::setScreenConstraintAABB()
    {
        Vec2d screenTargetPos = m_cameraTargetAveragePos.truncateTo2D();
        Vec2d offset( m_screenRealAABB.getWidth() *0.5f, m_screenRealAABB.getHeight() *0.5f);
        m_screenConstraintAABB.setMin( screenTargetPos -offset);
        m_screenConstraintAABB.setMax( screenTargetPos +offset);
    }

    void InGameCameraComponent::testConstraintExtendedAxeX()
    {
        // left
        f32 deltaLeft = 0.f;
        if ( m_CMCur.m_constraintLeftIsActive && m_constraintModifier->isConstraintExtendedLeft() )
        {
            u32 controllersListCount = m_controllers.size();
            for ( u32 i=0; i<controllersListCount; i++)
            {
                const Controller& controller = m_controllers[i];

                if ( controller.isActive() || controller.m_isPaused )
                {
                    deltaLeft = Min( deltaLeft, controller.m_subjectAABB.getMin().m_x -m_constraintModifier->GetActor()->getAABB().getMin().m_x);
                }
            }            
        }
        if ( deltaLeft <0.f)
        {
            m_constraintModifier->setConstraintExtendedLeft(-deltaLeft);

            if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
            {
                m_constraintModifier->setConstraintExtendedLeftToDest();
                setConstraintAABB();
            }
        }

        // right
        f32 deltaRight = 0.f;
        if ( m_CMCur.m_constraintRightIsActive && m_constraintModifier->isConstraintExtendedRight() )
        {
            u32 controllersListCount = m_controllers.size();
            for ( u32 i=0; i<controllersListCount; i++)
            {
                const Controller& controller = m_controllers[i];

                if ( controller.isActive() || controller.m_isPaused )
                {
                    deltaRight = Max(deltaRight, controller.m_subjectAABB.getMax().m_x -m_constraintModifier->GetActor()->getAABB().getMax().m_x);                 
                }
            }            
        }
        if ( deltaRight >0.f)
        {
            m_constraintModifier->setConstraintExtendedRight(deltaRight);

            if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
            {
                m_constraintModifier->setConstraintExtendedRightToDest();
                setConstraintAABB();
            }
        }
    }

    void InGameCameraComponent::testConstraintExtendedAxeY()
    {
        // bottom
        f32 deltaBottom = 0.f;
        if ( m_CMCur.m_constraintBottomIsActive && m_constraintModifier->isConstraintExtendedBottom() )
        {
            u32 controllersListCount = m_controllers.size();
            for ( u32 i=0; i<controllersListCount; i++)
            {
                const Controller& controller = m_controllers[i];

                if ( controller.isActive() || controller.m_isPaused )
                {
                    deltaBottom = Min( deltaBottom, controller.m_subjectAABB.getMin().m_y -m_constraintModifier->GetActor()->getAABB().getMin().m_y);
                }
            }           
        }
        if ( deltaBottom <0.f )
        {
            m_constraintModifier->setConstraintExtendedBottom(-deltaBottom); 

            if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
            {
                m_constraintModifier->setConstraintExtendedBottomToDest();
                setConstraintAABB();
            }
        }

        // top
        f32 deltaTop = 0.f;
        if ( m_CMCur.m_constraintTopIsActive && m_constraintModifier->isConstraintExtendedTop() )
        {
            u32 controllersListCount = m_controllers.size();
            for ( u32 i=0; i<controllersListCount; i++)
            {
                const Controller& controller = m_controllers[i];

                if ( controller.isActive() || controller.m_isPaused )
                {
                    deltaTop = Max(deltaTop, controller.m_subjectAABB.getMax().m_y -m_constraintModifier->GetActor()->getAABB().getMax().m_y);  
                }
            }            
        }
        if ( deltaTop >0.f)
        {
            m_constraintModifier->setConstraintExtendedTop(deltaTop); 

            if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
            {
                m_constraintModifier->setConstraintExtendedTopToDest();
                setConstraintAABB();
            }
        }
     }

    void InGameCameraComponent::initConstraintParams()
    {
        m_constraintDepthOffsetMax = F32_INFINITY;
        m_constraintSecretAreaZOld = m_constraintSecretAreaZ;
        m_constraintSecretAreaZ = 0.f;

        if ( m_constraintModifierOnEnter || m_constraintModifierOnExit )
        {
            m_constraintLast = m_constraint;
            m_constraintTimeToDisable = Vec3d::One;
            m_constraintSecretAreaZOld = 0.f;

            m_constraintSpeedBeforeSwicthConstraint = ( m_constraint -m_constraintPrevious) *m_deltaTimeInv;
        }

        m_constraintPrevious = m_constraint;
    }

    void InGameCameraComponent::updateConstraints( f32 _dt )
    {
        initConstraintParams();
        setConstraintTimeToDisable( _dt );

        // update drift
        m_constraintSpeedBeforeSwicthConstraint *= 0.9f;
        m_constraintLast += m_constraintSpeedBeforeSwicthConstraint *_dt;

        if ( m_constraintModifier )
        {           
            ITF_ASSERT( m_CMCur.m_constraintLeftIsActive || m_CMCur.m_constraintRightIsActive || m_CMCur.m_constraintTopIsActive || m_CMCur.m_constraintBottomIsActive);

            // set screen to apply constraint
            setScreenConstraintAABB();

            // set constraint anticip AABB
            setConstraintAABB();

            // init on enter
            if ( m_constraintModifierOnEnter )
            {                
                setConstraintTimeOnEnter();                             
            }
            
            // set time on left right top and bottom
            setConstraintTimeToActivate(_dt);

            // axe x
            if ( m_CMCur.m_constraintLeftIsActive || m_CMCur.m_constraintRightIsActive )
            {
                testConstraintExtendedAxeX();
                setConstraintX();
            }
            else
            {
                resetConstraintX();
            }

            // axe y
            if ( m_CMCur.m_constraintTopIsActive || m_CMCur.m_constraintBottomIsActive )
            {
                testConstraintExtendedAxeY();
                setConstraintY();
            }
            else
            {
                resetConstraintY();
            }

            setConstraintZ();
        }
        else
        {
            resetConstraintX();
            resetConstraintY();
            resetConstraintZ();
        }

        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
            m_constraintPrevious = m_constraint;
    }

}// namespace ITF
