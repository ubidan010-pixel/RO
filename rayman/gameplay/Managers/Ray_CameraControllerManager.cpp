#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CAMERACONTROLLERMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CameraControllerManager.h"
#endif //_ITF_RAY_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
    Ray_CameraControllerManager::Ray_CameraControllerManager()
    {
    }

    Ray_CameraControllerManager::~Ray_CameraControllerManager()
    {
    }

    void Ray_CameraControllerManager::onEndUpdateProcess( f32 _dt )
    {
        CameraControllerManager::onEndUpdateProcess(_dt);

        ICameraController* camController = getMainCameraController();
        if (!camController)
        {
            m_limiterData = Ray_CameraLimiterData();
            return;
        }

        Ray_EventQueryCameraLimiter query;
        camController->onCameraEvent(&query);
        m_limiterData = query.getLimiterData();
    }


    bbool Ray_CameraLimiterData::getEjectAABB( const f32 _z, AABB& _aabb ) const
    {
        #define DISABLED 100.f

        if ( CAMERACONTROLLERMANAGER->getAABB(_z, _aabb) )
        {
            Vec2d min = _aabb.getMin();
            Vec2d max = _aabb.getMax();

            min.m_x -= m_ejectMarginActive.m_left ? m_ejectMargin.m_left : DISABLED;
            max.m_x += m_ejectMarginActive.m_right ? m_ejectMargin.m_right : DISABLED;
            min.m_y -= m_ejectMarginActive.m_bottom ? m_ejectMargin.m_bottom : DISABLED;
            max.m_y += m_ejectMarginActive.m_top ? m_ejectMargin.m_top : DISABLED;

            if ( m_constraintActive.m_left )
            {
                min.m_x = Max(min.m_x, m_constraintAABB.getMin().m_x - m_ejectMargin.m_left);
            }

            if ( m_constraintActive.m_right )
            {
                max.m_x = Min(max.m_x, m_constraintAABB.getMax().m_x + m_ejectMargin.m_right);
            }

            if ( m_constraintActive.m_top )
            {
                max.m_y = Min(max.m_y, m_constraintAABB.getMax().m_y + m_ejectMargin.m_top);
            }

            _aabb.setMin(min);
            _aabb.setMax(max);

            return btrue;
        }

        return bfalse;
    }

    bbool Ray_CameraLimiterData::getDeathAABB( const f32 _z, AABB& _aabb ) const
    {
        if ( CAMERACONTROLLERMANAGER->getAABB(_z, _aabb) )
        {
            m_deathMargin.addTo(_aabb);

            if (m_constraintActive.m_bottom)
            {
                Vec2d min = _aabb.getMin();
                min.m_y = Max(min.m_y, m_constraintAABB.getMin().m_y);
                _aabb.setMin(min);
            }

            return btrue;
        }

        return bfalse;
    }

    void Ray_CameraLimiterData::setConstraint( const CameraModifierComponent* _cameraModifier )
    {
        m_constraintAABB = _cameraModifier->getModifierAABBMax();
        m_constraintActive.m_left = _cameraModifier->getUpdateData().m_constraintLeftIsActive;
        m_constraintActive.m_right = _cameraModifier->getUpdateData().m_constraintRightIsActive;
        m_constraintActive.m_bottom = _cameraModifier->getUpdateData().m_constraintBottomIsActive;
        m_constraintActive.m_top = _cameraModifier->getUpdateData().m_constraintTopIsActive;
    }

} //namespace ITF