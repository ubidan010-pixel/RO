#include "precompiled_engine.h"

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_PLUGINGATEWAY_H_   // could be limited to _WIN32 only
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_   // could be limited to _WIN32 only

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{

Camera::Camera() 
: m_focale(MTH_PIBY4)
, m_screen_ratio(1.f)
{
    m_x = 0.0f;
    m_y = 0.0f;
    m_z = 150.f;        
    setMode(Camera_InGame);
    m_bUseBias = bfalse;
    m_biasVertAngle = 0.f;
    m_biasHorizAngle = 0.f;
    m_biasDeltaPos = Vec3d(0,0,0);
    m_near = 1.0f;
    m_far = 10000.f;
    m_topLeft = Vec2d::Zero;
    m_bottomRight = Vec2d::One;
    m_changedBiasedCamera = bfalse;
    m_rotateZAngle = 0.f;
    m_useRotateCam = bfalse;
}


Camera::~Camera() 
{

}


//////////////////////////////////////////////////////////////////////////////
void Camera::setBiasedDeltaPos(const Vec3d &_newDelta) 
{
    if (m_biasDeltaPos != _newDelta)
    {
        m_biasDeltaPos = _newDelta; 
        m_changedBiasedCamera = btrue;
    }
}

//////////////////////////////////////////////////////////////////////////////
void Camera::setBiasedRotationMode(const Vec3d &_rotationCenter, const Vec3d &_biasEyeLookAtVector, bbool _apply) 
{
    m_bias_LookAt_RotationCenter = _rotationCenter;
    GFX_ADAPTER->setBiasEyeLookAt(_biasEyeLookAtVector);
    m_bUseBias = btrue; 
    m_changedBiasedCamera=btrue;
    if (_apply)
        apply();
}
//////////////////////////////////////////////////////////////////////////////
void Camera::setBiasAngles(f32 _angleX, f32 _angleY) 
{
    if (_angleX != m_biasHorizAngle || _angleY != m_biasVertAngle)
    {
        m_biasHorizAngle = _angleX; 
        m_biasVertAngle = _angleY;
        m_changedBiasedCamera=btrue;
    }
}
//////////////////////////////////////////////////////////////////////////////
void    Camera::startFrame()
{
    // determine camera origin
    Vec2d cam(m_x, m_y);

    // create clipping vectors

    apply();
    m_rayOrigin = Vec3d(m_x, m_y, m_z);
    f32 x = m_x;
    f32 y = m_y;
    f32 z = m_z;

    f32 scrW = (f32)GFX_ADAPTER->getScreenWidth();
    f32 scrH = (f32)GFX_ADAPTER->getScreenHeight();
    if (getRotateCam())
    {
        scrH = scrW = f32_Sqrt(scrW*scrW + scrH*scrH);
    }
    f32 camW = f32_Abs(m_bottomRight.m_x - m_topLeft.m_x) * scrW;
    f32 camH = f32_Abs(m_bottomRight.m_y - m_topLeft.m_y) * scrH;
    f32 camStartX = m_topLeft.m_x * scrW;
    f32 camStartY = m_topLeft.m_y * scrH;
    f32 camEndX = m_bottomRight.m_x * scrW;
    f32 camEndY = m_bottomRight.m_y * scrH;
    GFX_ADAPTER->coord2DTo3D(Vec2d(camStartX, camStartY + camH * 0.5f), 0.f, m_Left);
    GFX_ADAPTER->coord2DTo3D(Vec2d(camEndX, camStartY + camH * 0.5f), 0.f, m_Right);
    GFX_ADAPTER->coord2DTo3D(Vec2d(camStartX + camW * 0.5f, camStartY), 0.f, m_Up);
    GFX_ADAPTER->coord2DTo3D(Vec2d(camStartX + camW * 0.5f, camEndY), 0.f, m_Down);
    m_Left.m_x = m_Left.m_x * z + x;
    m_Left.m_y = m_Left.m_y * z +y;
    m_Right.m_x = m_Right.m_x * z + x;
    m_Right.m_y = m_Right.m_y * z +y;
    m_Up.m_x = m_Up.m_x * z + x;
    m_Up.m_y = m_Up.m_y * z +y;
    m_Down.m_x = m_Down.m_x * z + x;
    m_Down.m_y = m_Down.m_y * z +y;

    // create clipping vector for left side of the screen
    Vec3d rayDirection;
    rayDirection.m_x = m_Left.m_x;
    rayDirection.m_y = m_Left.m_y;
    rayDirection.m_z = 0.0f;
    rayDirection -= m_rayOrigin;
    rayDirection.normalize();
    if (rayDirection.m_z == 0.f)
    {
        rayDirection.m_z = 0.0001f; // set an epsilon to avoid 0 divide
    }
    m_clippingInfo[0].m_x = rayDirection.m_x;
    m_clippingInfo[0].m_y = 1.f / rayDirection.m_z;

    // create clipping vector for right side of the screen
    rayDirection.m_x = m_Right.m_x;
    rayDirection.m_y = m_Right.m_y;
    rayDirection.m_z = 0.0f;
    rayDirection -= m_rayOrigin;
    rayDirection.normalize();
    if (rayDirection.m_z == 0.f)
    {
        rayDirection.m_z = 0.0001f; // set an epsilon to avoid 0 divide
    }
    m_clippingInfo[1].m_x = rayDirection.m_x;
    m_clippingInfo[1].m_y = 1.f / rayDirection.m_z;

    // create clipping vector for up side of the screen
    rayDirection.m_x = m_Up.m_x;
    rayDirection.m_y = m_Up.m_y;
    rayDirection.m_z = 0.0f;
    rayDirection -= m_rayOrigin;
    rayDirection.normalize();
    if (rayDirection.m_z == 0.f)
    {
        rayDirection.m_z = 0.0001f; // set an epsilon to avoid 0 divide
    }
    m_clippingInfo[2].m_x = rayDirection.m_y; // up ray -> take Y
    m_clippingInfo[2].m_y = 1.f / rayDirection.m_z;


    // create clipping vector for down side of the screen
    rayDirection.m_x = m_Down.m_x;
    rayDirection.m_y = m_Down.m_y;
    rayDirection.m_z = 0.0f;
    rayDirection -= m_rayOrigin;
    rayDirection.normalize();
    if (rayDirection.m_z == 0.f)
    {
        rayDirection.m_z = 0.0001f; // set an epsilon
    }
    m_clippingInfo[3].m_x = rayDirection.m_y; // bottom ray -> take Y
    m_clippingInfo[3].m_y = 1.f / rayDirection.m_z;

    // create corner vectors
    Vec2d TopLeft, TopRight, BottomLeft, BottomRight;
    apply();
    GFX_ADAPTER->coord2DTo3D(Vec2d(camStartX, camEndY), 0.f, BottomLeft);
    GFX_ADAPTER->coord2DTo3D(Vec2d(camEndX, camEndY), 0.f, BottomRight);
    GFX_ADAPTER->coord2DTo3D(Vec2d(camStartX, camStartY), 0.f, TopLeft);
    GFX_ADAPTER->coord2DTo3D(Vec2d(camEndX, camStartY), 0.f, TopRight);

    TopLeft.m_x = TopLeft.m_x * z + x;
    TopLeft.m_y = TopLeft.m_y * z +y;
    TopRight.m_x = TopRight.m_x * z + x;
    TopRight.m_y = TopRight.m_y * z +y;
    BottomLeft.m_x = BottomLeft.m_x * z + x;
    BottomLeft.m_y = BottomLeft.m_y * z +y;
    BottomRight.m_x = BottomRight.m_x * z + x;
    BottomRight.m_y = BottomRight.m_y * z +y;

    m_minAABBAtAZZero = TopLeft;
    m_maxAABBAtAZZero = BottomRight;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // force minimum window size to avoid activation issues and always simulate 4 players cam
        f32 camMinX = m_minAABBAtAZZero.m_x;
        f32 camMinY = m_maxAABBAtAZZero.m_y;
        f32 camMaxX = m_maxAABBAtAZZero.m_x;
        f32 camMaxY = m_minAABBAtAZZero.m_y;
        f32 delta = camMaxX - camMinX;
        static const f32 minWindowWidth = 40.0f;    // width of cam window at Z = 0 when playing 4 players
        static const f32 minWindowHeight = 20.0f;   // height of cam window at Z = 0 when playing 4 players
        if (delta > 0.f && delta < minWindowWidth)
        {
            f32 center = (camMaxX + camMinX) * 0.5f;
            m_minAABBAtAZZero.m_x = center - minWindowWidth * 0.5f;
            m_maxAABBAtAZZero.m_x = center + minWindowWidth * 0.5f;
        }
        delta = camMaxY - camMinY;
        if (delta > 0.f && delta < minWindowHeight)
        {
            f32 center = (camMaxY + camMinY) * 0.5f;
            m_minAABBAtAZZero.m_y = center + minWindowHeight * 0.5f;
            m_maxAABBAtAZZero.m_y = center - minWindowHeight * 0.5f;
        }
    /////////////////////////////////////////////////////////////////////////////////////////////


    m_screenCornersRays[0].m_x = TopLeft.m_x;
    m_screenCornersRays[0].m_y = TopLeft.m_y;
    m_screenCornersRays[0].m_z = 0.0f;
    m_screenCornersRays[0] -= m_rayOrigin;
    m_screenCornersRays[0].normalize();

    m_screenCornersRays[1].m_x = TopRight.m_x;
    m_screenCornersRays[1].m_y = TopRight.m_y;
    m_screenCornersRays[1].m_z = 0.0f;
    m_screenCornersRays[1] -= m_rayOrigin;
    m_screenCornersRays[1].normalize();

    m_screenCornersRays[2].m_x = BottomLeft.m_x;
    m_screenCornersRays[2].m_y = BottomLeft.m_y;
    m_screenCornersRays[2].m_z = 0.0f;
    m_screenCornersRays[2] -= m_rayOrigin;
    m_screenCornersRays[2].normalize();

    m_screenCornersRays[3].m_x = BottomRight.m_x;
    m_screenCornersRays[3].m_y = BottomRight.m_y;
    m_screenCornersRays[3].m_z = 0.0f;
    m_screenCornersRays[3] -= m_rayOrigin;
    m_screenCornersRays[3].normalize();

	MWII_HalfScreenForZEqualOne.m_x = f32_Abs(m_clippingInfo[0].m_y * m_clippingInfo[0].m_x);
	MWII_HalfScreenForZEqualOne.m_y = f32_Abs(MWII_HalfScreenForZEqualOne.m_x * scrH / scrW) ;
#ifdef ITF_SUPPORT_DEBUGFEATURE
    if (DEBUGINFO)
        DEBUGINFO->setCameraInfo(this);
#endif //!ITF_SUPPORT_DEBUGFEATURE

    computeDeltaXY();
}

void  Camera::computeDeltaXY()
{
    f32 halfFocaleX = getFocale();
    ITF_ASSERT(f32_Abs(halfFocaleX-MTH_PIBY2) > MTH_EPSILON);
    m_deltaX = f32_Abs(tanf(halfFocaleX));

    f32 w = (f32)GFX_ADAPTER->getScreenWidth();
    f32 h = (f32)GFX_ADAPTER->getScreenHeight();

    f32 halfFocaleY = halfFocaleX * h / w;
    ITF_ASSERT(f32_Abs(halfFocaleY-MTH_PIBY2) > MTH_EPSILON);

    m_deltaY = f32_Abs(tanf(halfFocaleY));


}

void Camera::copyTo(Camera& dst)
{
    dst = *this;
}

void Camera::getScreenCornersAtZ (float _z, Vec2d* _corners) const
{
    if (m_screenCornersRays[0].m_z != 0.0f)
    {
        float k = (_z - m_rayOrigin.m_z) / m_screenCornersRays[0].m_z;
        _corners[0].m_x = m_rayOrigin.m_x + k * m_screenCornersRays[0].m_x;
        _corners[0].m_y = m_rayOrigin.m_y + k * m_screenCornersRays[0].m_y;
    }
    if (m_screenCornersRays[1].m_z != 0.0f)
    {
        float k = (_z - m_rayOrigin.m_z) / m_screenCornersRays[1].m_z;
        _corners[1].m_x = m_rayOrigin.m_x + k * m_screenCornersRays[1].m_x;
        _corners[1].m_y = m_rayOrigin.m_y + k * m_screenCornersRays[1].m_y;
    }
    if (m_screenCornersRays[2].m_z != 0.0f)
    {
        float k = (_z - m_rayOrigin.m_z) / m_screenCornersRays[2].m_z;
        _corners[2].m_x = m_rayOrigin.m_x + k * m_screenCornersRays[2].m_x;
        _corners[2].m_y = m_rayOrigin.m_y + k * m_screenCornersRays[2].m_y;
    }
    if (m_screenCornersRays[3].m_z != 0.0f)
    {
        float k = (_z - m_rayOrigin.m_z) / m_screenCornersRays[3].m_z;
        _corners[3].m_x = m_rayOrigin.m_x + k * m_screenCornersRays[3].m_x;
        _corners[3].m_y = m_rayOrigin.m_y + k * m_screenCornersRays[3].m_y;
    }
}

AABB Camera::getAABBatZ (float _z) const
{
    Vec2d min;  // bottom-left
    if (m_screenCornersRays[2].m_z != 0.0f)
    {
        float k = (_z - m_rayOrigin.m_z) / m_screenCornersRays[2].m_z;
        min.m_x = m_rayOrigin.m_x + k * m_screenCornersRays[2].m_x;
        min.m_y = m_rayOrigin.m_y + k * m_screenCornersRays[2].m_y;
    }

    Vec2d max;  // top-right
    if (m_screenCornersRays[1].m_z != 0.0f)
    {
        float k = (_z - m_rayOrigin.m_z) / m_screenCornersRays[1].m_z;
        max.m_x = m_rayOrigin.m_x + k * m_screenCornersRays[1].m_x;
        max.m_y = m_rayOrigin.m_y + k * m_screenCornersRays[1].m_y;
    }

    return AABB(min, max);
}

bbool Camera::is3dAABBVisible_Slow(const Vec3d &_boxMin, const Vec3d &_boxMax) const
{
    for (int i=0; i<CameraPlane_Count; i++)
    {
        const Plane &plane = m_frustumPlanes[i];
        f32 nx = plane.m_normal.m_x;
        f32 ny = plane.m_normal.m_y;
        f32 nz = plane.m_normal.m_z;

        const Vec3d minExtremity(   f32_Sel(nx, _boxMin.m_x, _boxMax.m_x),
                                    f32_Sel(ny, _boxMin.m_y, _boxMax.m_y),
                                    f32_Sel(nz, _boxMin.m_z, _boxMax.m_z));

        if (minExtremity.dot(plane.m_normal) > plane.m_constant)
            return bfalse; //completely outside frustum
    }

    return btrue; //fully inside
}

#if defined(ITF_PS3) || defined(ITF_X360)
#define FULL_MATHEMATICAL_AABB_VISIBILITY

// The following functions are there to compute frustum vs AABB culling

inline f32 signedDistToPlane(f32 _pnx, f32 _pny, f32 _pnz, f32 _pcst, f32 _bx, f32 _by, f32 _bz)
{
    return _pcst - (_pnx * _bx + _pny * _by + _pnz * _bz);
}

inline f32 condAABBSideOfPlane(const Plane & _plane, f32 _minX, f32 _minY, f32 _maxX, f32 _maxY, f32 _z)
{
    f32 pnx = _plane.m_normal.m_x;
    f32 pny = _plane.m_normal.m_y;
    f32 pnz = _plane.m_normal.m_z;
    f32 cst = _plane.m_constant;
    f32 bx = f32_Sel(pnx, _minX, _maxX);
    f32 by = f32_Sel(pny, _minY, _maxY);
    f32 dist = signedDistToPlane(pnx, pny, pnz, cst, bx, by, _z);
    return f32_Sel( dist, 1.f, 0.f);
}

inline bbool is2DAABBVisible_FullMath(Plane _planes[Camera::CameraPlane_Count], f32 _minX, f32 _minY, f32 _maxX, f32 _maxY, f32 _z)
{
    f32 cond = condAABBSideOfPlane(_planes[Camera::CameraPlane_Left], _minX, _minY, _maxX, _maxY, _z);
    cond    *= condAABBSideOfPlane(_planes[Camera::CameraPlane_Right], _minX, _minY, _maxX, _maxY, _z);
    cond    *= condAABBSideOfPlane(_planes[Camera::CameraPlane_Top], _minX, _minY, _maxX, _maxY, _z);
    cond    *= condAABBSideOfPlane(_planes[Camera::CameraPlane_Bottom], _minX, _minY, _maxX, _maxY, _z);
    cond    *= condAABBSideOfPlane(_planes[Camera::CameraPlane_Near], _minX, _minY, _maxX, _maxY, _z);
    return cond!= 0;
}

// The following fct is using additional constraints on the camera to perform the culling
inline bbool fullMathRayClipping(Vec2d _clippingInfo[4], f32 _origX, f32 _origY, f32 _zRel, f32 _minX, f32 _minY, f32 _maxX, f32 _maxY)
{
    float k;
    float x, y;
    f32 cond;

    // perform clipping against the left border of the screen
    k = _zRel * _clippingInfo[0].m_y;
    x = _origX + k * _clippingInfo[0].m_x;
    cond = f32_Sel(_maxX - x, 1.f, 0.f);

    // perform clipping against the right border of the screen
    k = _zRel * _clippingInfo[1].m_y;
    x = _origX + k * _clippingInfo[1].m_x;
    cond *= f32_Sel(x - _minX, 1.f, 0.f);

    // perform clipping against the up border of the screen
    k = _zRel * _clippingInfo[2].m_y;
    y = _origY + k * _clippingInfo[2].m_x;
    cond *= f32_Sel(y - _minY, 1.f, 0.f);

    // perform clipping against the bottom border of the screen
    k = _zRel * _clippingInfo[3].m_y;
    y = _origY + k * _clippingInfo[3].m_x;
    cond *= f32_Sel(_maxY-y, 1.f, 0.f);
    return cond != 0;
}
#endif
#ifndef ITF_WII // Philippe V => wii cost cut
bbool Camera::isRectVisible(const AABB& _aabb, float _z)
{
#ifdef FULL_MATHEMATICAL_AABB_VISIBILITY
    // is2DAABBVisible_FullMath is a version using the GFX adapter planes to perform a more classical frustrum vs aabb culling
    //return is2DAABBVisible_FullMath(m_frustumPlanes, _aabb.getMin().m_x, _aabb.getMin().m_y, _aabb.getMax().m_x, _aabb.getMax().m_y, _z);
    // Right now, the ray clipping is the fastest one, using 
    return fullMathRayClipping(m_clippingInfo, m_rayOrigin.m_x, m_rayOrigin.m_y, _z - m_rayOrigin.m_z, _aabb.getMin().m_x, _aabb.getMin().m_y, _aabb.getMax().m_x, _aabb.getMax().m_y);
#else
    // use directX planes
    if (m_bUseBias)
    {
        bbool isVisible = is3dAABBVisible_Slow(Vec3d(_aabb.getMin().m_x, _aabb.getMin().m_y, _z),
            Vec3d(_aabb.getMax().m_x, _aabb.getMax().m_y, _z));
        return isVisible;
    }

    // algorithmic clipping
    f32 zRel = _z - m_rayOrigin.m_z;

    {
        // perform clipping against the left border of the screen
        float k = zRel * m_clippingInfo[0].m_y;
        float x = m_rayOrigin.m_x + k * m_clippingInfo[0].m_x;
        if (x > _aabb.getMax().m_x)
            return bfalse;
    }

    {
        // perform clipping against the right border of the screen
        float k = zRel * m_clippingInfo[1].m_y;
        float x = m_rayOrigin.m_x + k * m_clippingInfo[1].m_x;
        if (x < _aabb.getMin().m_x)
            return bfalse;

    }

    {
        // perform clipping against the up border of the screen
        float k = zRel * m_clippingInfo[2].m_y;
        float y = m_rayOrigin.m_y + k * m_clippingInfo[2].m_x;
        if (_aabb.getMin().m_y > y)
            return bfalse;
    }

    {
        // perform clipping against the bottom border of the screen
        float k = zRel * m_clippingInfo[3].m_y;
        float y = m_rayOrigin.m_y + k * m_clippingInfo[3].m_x;
        if (_aabb.getMax().m_y < y)
            return bfalse;
    }

    return btrue;
#endif
}
#else
bbool   Camera::isRectVisible_NotInlined(const AABB& _aabb, float _z)
{
	float FactorMul = (m_rayOrigin.m_z - _z );
	Vec2d AABBMax = _aabb.getMax();
	Vec2d AABBMin = _aabb.getMin();
	Vec2d Ptest,HZEO = MWII_HalfScreenForZEqualOne * FactorMul;
	AABBMax = AABBMax + HZEO;
	AABBMin = AABBMin - HZEO;
	Vec2d::Max(&Ptest,&AABBMin,(Vec2d *)&m_rayOrigin);
	Vec2d::Min(&Ptest,&AABBMax,&Ptest);
	Ptest = Ptest - *(Vec2d *)&m_rayOrigin;
	if (Vec2d::Dot(&Ptest,&Ptest) != 0) return false;
	return true;
}
#endif


void Camera::forcePosition(const Vec3d& _newPos)
{
    m_x = _newPos.m_x;
    m_y = _newPos.m_y;
    m_z = _newPos.m_z;
}


void Camera::update(float _dt)
{
    m_screen_ratio  = (f32_Abs(m_bottomRight.m_x - m_topLeft.m_x) * (f32)GFX_ADAPTER->getScreenWidth())/(f32_Abs(m_bottomRight.m_y - m_topLeft.m_y) * (f32)GFX_ADAPTER->getScreenHeight());
}


void Camera::setMode(Mode _mode)
{
#ifdef _DEBUG
    switch(_mode)
    {
    case Camera_Editor:
    case Camera_InGame:
    case Camera_Slave:
        break;
    default:
        ITF_ASSERT(0); // you are setting an invalid mode for camera
        break;
    }
#endif // _DEBUG

#if defined(ITF_SUPPORT_EDITOR)
    if (PLUGINGATEWAY && _mode != m_camMode) // could be limited to _WIN32 only
        PLUGINGATEWAY->onCameraModeChange((int)m_camMode, (int)_mode);
#endif //ITF_SUPPORT_EDITOR
    m_camMode = _mode;
}

void Camera::apply()
{
    GFX_ADAPTER->setCamera(m_x, m_y, m_z, this);
}


} // namespace ITF
