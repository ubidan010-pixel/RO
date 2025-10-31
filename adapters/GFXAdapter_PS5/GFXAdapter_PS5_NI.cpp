#include "precompiled_GFXAdapter_PS5.h"

// All methods not implemented.
// Taken from GFXAdapter_GCM_NI.cpp
// Verified as editor/pc only, or not called

#include "GFXAdapter_PS5.h"

namespace ITF
{

    void GFXAdapter_PS5::setBiasEyeLookAt(const Vec3d& lookAtDir)
    {
        NOT_IMPLEMENTED_ON_PS5
        // Used for:
        // Camera rotation mode (when the user holds right mouse button + Space). Returns btrue if required delayed execution
    }

    void GFXAdapter_PS5::setOrthoView2DYNeg(f32 _l, f32 _r, f32 _b, f32 _t)
    {
        // Not called
        NOT_IMPLEMENTED_ON_PS5
        // same as setOrthoView right now
        setOrthoView(_l, _r, _b, _t);

        
    }

    void GFXAdapter_PS5::reshape()
    {
        // resolution change
        NOT_IMPLEMENTED_ON_PS5
    }


    void GFXAdapter_PS5::acquireDeviceOwnerShip()
    {
        // multithreading not allowed 
        NOT_IMPLEMENTED_ON_PS5
    }

    void GFXAdapter_PS5::releaseDeviceOwnerShip()
    {
        // multithreading not allowed 
        NOT_IMPLEMENTED_ON_PS5
    }

    void GFXAdapter_PS5::minimal_present()
    {
        // Not supported right now -> no simply flip rendering
        NOT_IMPLEMENTED_ON_PS5
    }

#ifndef ITF_CONSOLE_FINAL
    void GFXAdapter_PS5::drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _size, float _z, float _alpha)
    {
        // never called
        NOT_IMPLEMENTED_ON_PS5
    }
#endif

    void GFXAdapter_PS5::setCullMode(GFX_CULLMODE _Cull)
    {
        // never called
        NOT_IMPLEMENTED_ON_PS5
    }

    void GFXAdapter_PS5::depthFunc(u32 _Func)
    {
        NOT_IMPLEMENTED_ON_PS5

        // implementation might be need!
    }

#if !defined(ITF_FINAL)
    void GFXAdapter_PS5::unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44* _MatrixModel, const GMatrix44* _MatrixProj, const int* _View, Vec2d& _out)
    {
        // never called
        NOT_IMPLEMENTED_ON_PS5
    }

    void GFXAdapter_PS5::setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect)
    {
        // never called
        NOT_IMPLEMENTED_ON_PS5
    }
#endif

    void GFXAdapter_PS5::saveToMemory(void* _buffer, Texture* _Tex)
    {
        // never called
        // not implemented on win32 either
        NOT_IMPLEMENTED_ON_PS5
    }

    void GFXAdapter_PS5::project(const Vec3d& _in3d, Vec2d& _out2d)
    {
        // not called
        NOT_IMPLEMENTED_ON_PS5
    }

    f32 GFXAdapter_PS5::ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY)
    {
        // not called -> a 'getPixel'
        NOT_IMPLEMENTED_ON_PS5
        return 0.0f;
    }

    void GFXAdapter_PS5::startOverDrawRecord()
    {
        // Not yet done
        NOT_IMPLEMENTED_ON_PS5
    }

    void GFXAdapter_PS5::showOverDrawRecord()
    {
        // Not yet done
        NOT_IMPLEMENTED_ON_PS5
    }


} // namespace ITF
