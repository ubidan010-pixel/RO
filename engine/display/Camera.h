#ifndef _ITF_CAMERA_H_
#define _ITF_CAMERA_H_

#ifndef _ITF_PLANE_H_
#include "core/math/plane.h"
#endif //_ITF_PLANE_H_


namespace ITF
{

class AABB;

class Camera
{

public:
    enum Mode
    {
        Camera_Editor = 0,
        Camera_InGame,
        Camera_Slave,
        Camera_ModeCount ,   // must remain the last element
        ENUM_FORCE_SIZE_32(0)
    };
    
    enum CameraPlane
    {
        CameraPlane_Left=0,
        CameraPlane_Right,
        CameraPlane_Top,
        CameraPlane_Bottom,
        CameraPlane_Near,
        CameraPlane_Count,
        ENUM_FORCE_SIZE_32(1)
    };

    


    Camera();
    ~Camera();

    AABB    getAABBatZ (float _z) const; // bottom-left to top-right
    void    getScreenCornersAtZ (float _z, Vec2d* _corners) const; // top-left, top-right, bottom-left, bottom-right

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// calculate clipping and picking vectors
    void    startFrame(); 
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// check visibility of a rectangle
    /// @param _aabb the rectangle
    /// @param _z the depth of the rectangle
#ifdef ITF_WII // Philippe V => wii cost cut
    ITF_INLINE bbool   isRectVisible(const AABB& _aabb, float _z)
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
	bbool   isRectVisible_NotInlined(const AABB& _aabb, float _z);
#else
	bbool   isRectVisible(const AABB& _aabb, float _z);
	ITF_INLINE bbool   isRectVisible_NotInlined(const AABB& _aabb, float _z) {return isRectVisible(_aabb, _z);};
#endif


    ///////////////////////////////////////////////////////////////////////////////////////////
    /// clone camera
    /// @param dst
    void copyTo(Camera& dst);
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// apply the current parameters to the graphics subsystem
    void apply();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to X (X component of unbiased position)
    f32  getX()   const       {return m_x;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to Y (Y component of unbiased position)
    f32  getY()   const       {return m_y;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to Z (Z component of unbiased position)
    f32  getZ()   const       {return m_z;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to rotate Z angle (camera rotation around z)
    f32  getRotateZAngle()   const       {return m_rotateZAngle;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to z (set)
    /// @param _z
    void setZ(f32 _z )        {m_z = _z;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to m_rotateZAngle (set)
    /// @param _z
    void setRotateZAngle(f32 _a )        {m_rotateZAngle = _a;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// update the camera position depending on mode. Doesn't handle bias
    /// @param _dt frame duration
    void        update                  (   float _dt);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get unbiased position
    Vec3d       getPos                  (   ) const   {return Vec3d(m_x, m_y, m_z);}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get position of camera, taking biased mode into account
    const Vec3d& getCorrectedPos        (   ) const {return m_resultingBiasedPosition;}

    float       getFocale               (   )   {return m_focale;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// set unbiased position; DO NOT use it to move the cam from the gameplay, use teleport() instead
    /// @param _newPos
    void        forcePosition           (   const Vec3d& _newPos   );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Accessor to biased mode (a biased camera transforms the input parameters (without modifying them))
    bbool       isBiased() const        {return m_bUseBias;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Apply a bias delta
    void        setBiasedDeltaPos       (const Vec3d &_newDelta);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Enter biased mode for rotated camera
    void        setBiasedRotationMode   (const Vec3d &_rotationCenter, const Vec3d &_biasEyeLookAtVector, bbool _apply=bfalse);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Cancel bias mode
    /// @param _apply [in] btrue  if you want to update the camera matrices
    void        cancelBiasMode(bbool _apply=bfalse)  {m_bUseBias = bfalse; if (_apply) apply(); }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Restore the bias mode. Only use if you just canceled bias mode
    /// @param _apply [in] btrue  if you want to update the camera matrices
    void        restoreBiasMode(bbool _apply=bfalse)  {m_bUseBias = btrue; if (_apply) apply(); }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// set the angle for bias mode
    /// @param f32 _angleX
    /// @param f32 _angleY
    void        setBiasAngles           (f32 _angleX, f32 _angleY);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// check if a bias mode parameter changed
    bbool       hasBiasedCameraChanged  () const {return m_changedBiasedCamera;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// clear the flag for bias mode parameter change (use with care)
    void        clearChangedBiasedCamera() {m_changedBiasedCamera=bfalse;}    

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// compute unormalized look at direction.
    Vec3d       getUnnormalizedLookAtDirection() const {return m_lookAt-getCorrectedPos();}

    Vec2d       m_minAABBAtAZZero;
    Vec2d       m_maxAABBAtAZZero;


    ///////////////////////////////////////////////////////////////////////////////////////////
    void        computeDeltaXY();    

    ITF_INLINE  f32 getDeltaX() {return m_deltaX;} //slope of line which transforms world AABB to screen AABB
    ITF_INLINE  f32 getDeltaY() {return m_deltaY;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get the camera mode
    Mode    getMode() {return m_camMode;}
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// set camera mode (editor, in-game...)
    /// @param _mode the mode to set
    void    setMode(Mode _mode);

    void    setRotateCam(bbool _v) { m_useRotateCam = _v;}
    bbool   getRotateCam() { return m_useRotateCam;}

    ///////// START VIEWPORT
        float   m_near, m_far;        // near & far planes
        Vec2d   m_topLeft;            // unit : screen size ratio (0..1)
        Vec2d   m_bottomRight;        // unit : screen size ratio (0..1)
    ///////// END  VIEWPORT


    float m_screen_ratio, m_focale;
    Vec2d m_Left, m_Right, m_Up, m_Down;
    Vec3d m_rayOrigin;
    Vec2d m_clippingInfo[4]; // in the order left, right, up, bottom
                             // m_clippingInfo is precomputed from the ray starting at the camera and going along the clipping plane
                             // As the camera roll is 0, the y (for left/right) or x (up/bottom) of the ray direction is not interesting for clipping.
                             // So the member m_x of m_clippingInfo is the x or y of the parametric equation.
                             // The member m_z is the precomputed inverse of the ray z component
    Vec3d m_screenCornersRays[4]; // in the order left, right, up, bottom
    Vec3d m_lookAt;

    //Bias mode is when you turn the camera. The x,y coordinated stay the same but the real camera is offset
    bbool m_bUseBias;
    f32 m_biasVertAngle;
    f32 m_biasHorizAngle;
    Vec3d m_bias_LookAt_RotationCenter;
    Vec3d m_resultingBiasedPosition; //updated as new eye position after a set camera with a bias
    Vec3d m_bias_EyeToLookAt;
    Vec3d m_biasDeltaPos; //Translation added to camera position and lookat after RotationCenter pass. Reset by setCamera() after taken into account

    Plane       m_frustumPlanes[CameraPlane_Count];


	/*****************************************/
	/* WII optimization for IsVisible  BEGIN */
	Vec2d		MWII_HalfScreenForZEqualOne;
	/* WII optimization for IsVisible  END */
	/*****************************************/


private:

    Mode        m_camMode;
    bbool       m_changedBiasedCamera;
    f32         m_deltaX;
    f32         m_deltaY;
    f32         m_x, m_y, m_z;
    f32         m_rotateZAngle;
    bbool       m_useRotateCam;

    bbool   is3dAABBVisible_Slow(const Vec3d &_boxMin, const Vec3d &_boxMax) const;
};

} // namespace ITF



#endif //_ITF_CAMERA_H_
