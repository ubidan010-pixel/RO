#include "precompiled_GFXAdapter_DX12.h"

// All methods not implemented.
// Taken from GFXAdapter_GCM_NI.cpp
// Verified as editor/pc only, or not called

#include "GFXAdapter_DX12.h"

namespace ITF
{
    void GFXAdapter_DX12::setBiasEyeLookAt(const Vec3d& lookAtDir)
    {
        ITF_NOT_IMPLEMENTED();
        // Used for:
        // Camera rotation mode (when the user holds right mouse button + Space). Returns btrue if required delayed execution
    }

    void GFXAdapter_DX12::setOrthoView2DYNeg(f32 _l, f32 _r, f32 _b, f32 _t)
    {
        // Not called
        ITF_NOT_IMPLEMENTED();
        // same as setOrthoView right now
        setOrthoView(_l, _r, _b, _t);

        
    }

    void GFXAdapter_DX12::reshape()
    {
        // resolution change
        ITF_NOT_IMPLEMENTED();
    }


    void GFXAdapter_DX12::acquireDeviceOwnerShip()
    {
        // multithreading not allowed 
        ITF_NOT_IMPLEMENTED();
    }

    void GFXAdapter_DX12::releaseDeviceOwnerShip()
    {
        // multithreading not allowed 
        ITF_NOT_IMPLEMENTED();
    }

    void GFXAdapter_DX12::minimal_present()
    {
        // Not supported right now -> no simply flip rendering
        ITF_NOT_IMPLEMENTED();
    }

#ifndef ITF_CONSOLE_FINAL
    void GFXAdapter_DX12::drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _size, float _z, float _alpha)
    {
        // never called
        ITF_NOT_IMPLEMENTED();
    }
#endif

    void GFXAdapter_DX12::setCullMode(GFX_CULLMODE _Cull)
    {
        // never called
        ITF_NOT_IMPLEMENTED();
    }

    void GFXAdapter_DX12::depthFunc(u32 _Func)
    {
        ITF_NOT_IMPLEMENTED();

        // implementation might be need!
    }

#if !defined(ITF_FINAL)
    void GFXAdapter_DX12::unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44* _MatrixModel, const GMatrix44* _MatrixProj, const int* _View, Vec2d& _out)
    {
        // never called
        ITF_NOT_IMPLEMENTED();
    }

    void GFXAdapter_DX12::setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect)
    {
        // never called
        ITF_NOT_IMPLEMENTED();
    }
#endif

    void GFXAdapter_DX12::saveToMemory(void* _buffer, Texture* _Tex)
    {
        // never called
        // not implemented on win32 either
        ITF_NOT_IMPLEMENTED();
    }

    void GFXAdapter_DX12::project(const Vec3d& _in3d, Vec2d& _out2d)
    {
        // not called
        ITF_NOT_IMPLEMENTED();
    }

    f32 GFXAdapter_DX12::ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY)
    {
        // not called -> a 'getPixel'
        ITF_NOT_IMPLEMENTED();
        return 0.0f;
    }

    void GFXAdapter_DX12::startOverDrawRecord()
    {
        // Not yet done
        ITF_NOT_IMPLEMENTED();
    }

    void GFXAdapter_DX12::showOverDrawRecord()
    {
        // Not yet done
        ITF_NOT_IMPLEMENTED();
    }

} // namespace ITF
