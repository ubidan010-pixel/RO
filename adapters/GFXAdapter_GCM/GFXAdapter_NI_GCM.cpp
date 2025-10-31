#include "precompiled_GFXAdapter_GCM.h"

#include "GFXAdapter_GCM.h"

///////////////////////////////////////////////////////////////////////////////
// Not implemented methods, verified as editor/pc only, or not called
///////////////////////////////////////////////////////////////////////////////

namespace ITF
{

    void GFXAdapter_GCM::setBiasEyeLookAt(const Vec3d &lookAtDir)
    {
        IMPLEMENT_THIS
        ITF_ASSERT(0);
        // Used for:
        // Camera rotation mode (when the user holds right mouse button + Space). Returns btrue if required delayed execution
    }

    void GFXAdapter_GCM::setOrthoView2DYNeg(f32 _l, f32 _r, f32 _b, f32 _t)
    {
        IMPLEMENT_THIS
        ITF_ASSERT(0);
        // same as setOrthoView right now
        setOrthoView(_l, _r, _b, _t);

        // Not called
    }

    void GFXAdapter_GCM::acquireDeviceOwnerShip()
    {
        // multithreading not allowed 
        IMPLEMENT_THIS
        ITF_ASSERT(0);
    }

    void GFXAdapter_GCM::releaseDeviceOwnerShip()
    {
        // multithreading not allowed 
        IMPLEMENT_THIS
        ITF_ASSERT(0);
    }

    void GFXAdapter_GCM::minimal_present()
    {
        // Not supported right now -> no simply flip rendering
        IMPLEMENT_THIS
        ITF_ASSERT(0);
    }

    void GFXAdapter_GCM::drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _size, float _z, float _alpha)
    {
        // never called
        ITF_ASSERT(0);
    }

    void GFXAdapter_GCM::setCullMode(GFX_CULLMODE _Cull)
    {
        // never called
        ITF_ASSERT(0);

        // implementation (to be tested)
        switch(_Cull)
        {
        case GFX_CULL_NONE:
            m_rdrCtxt.gcmCtxt->SetCullFaceEnable(CELL_GCM_FALSE);
            break;
        case GFX_CALL_CCW:
            m_rdrCtxt.gcmCtxt->SetCullFaceEnable(CELL_GCM_TRUE);
            m_rdrCtxt.gcmCtxt->SetFrontFace(CELL_GCM_CW);
            break;
        case GFX_CULL_CW:
            m_rdrCtxt.gcmCtxt->SetCullFaceEnable(CELL_GCM_TRUE);
            m_rdrCtxt.gcmCtxt->SetFrontFace(CELL_GCM_CCW);
            break;
        }
    }

    void GFXAdapter_GCM::depthFunc(u32 _Func)
    {
        // never called
        ITF_ASSERT(0);

        // implementation (to be tested)
        switch(_Func)
        {
        case GFX_CMP_NEVER:
            m_rdrCtxt.gcmCtxt->SetDepthFunc(CELL_GCM_NEVER);
            break;
        case GFX_CMP_LESS:
            m_rdrCtxt.gcmCtxt->SetDepthFunc(CELL_GCM_LESS);
            break;
        case GFX_CMP_EQUAL:
            m_rdrCtxt.gcmCtxt->SetDepthFunc(CELL_GCM_EQUAL);
            break;
        case GFX_CMP_LESSEQUAL:
            m_rdrCtxt.gcmCtxt->SetDepthFunc(CELL_GCM_LEQUAL);
            break;
        case GFX_CMP_GREATER:
            m_rdrCtxt.gcmCtxt->SetDepthFunc(CELL_GCM_GREATER);
            break;
        case GFX_CMP_NOTEQUAL:
            m_rdrCtxt.gcmCtxt->SetDepthFunc(CELL_GCM_NOTEQUAL);
            break;
        case GFX_CMP_GREATEREQUAL:
            m_rdrCtxt.gcmCtxt->SetDepthFunc(CELL_GCM_GEQUAL);
            break;
        case GFX_CMP_ALWAYS:
            m_rdrCtxt.gcmCtxt->SetDepthFunc(CELL_GCM_ALWAYS);
            break;
        }
    }

    void GFXAdapter_GCM::unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44* _MatrixModel, const GMatrix44* _MatrixProj, const int* _View, Vec2d& _out)
    {
        // never called
        ITF_ASSERT(0);

        IMPLEMENT_THIS
    }

    void GFXAdapter_GCM::setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect)
    {  
        // never called
        ITF_ASSERT(0);

        IMPLEMENT_THIS
    }

    void GFXAdapter_GCM::saveToMemory(void* _buffer, Texture* _Tex)
    {
        // never called
        // not implemented on win32
        ITF_ASSERT(0);
        IMPLEMENT_THIS
    }

    bbool GFXAdapter_GCM::isDrawingForScreenshot()
    {
        return bfalse;
    }

    void GFXAdapter_GCM::project(const Vec3d& _in3d, Vec2d & _out2d)
    {
        // not called
        IMPLEMENT_THIS
    }

    f32 GFXAdapter_GCM::ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY)
    {
        // not call -> a 'getPixel'
        IMPLEMENT_THIS
            return 0.0f;
    }

    void GFXAdapter_GCM::startOverDrawRecord()
    {
        // Not yet done
        IMPLEMENT_THIS
        ITF_ASSERT(0);
    }

    void GFXAdapter_GCM::showOverDrawRecord()
    {
        // Not yet done
        IMPLEMENT_THIS
        ITF_ASSERT(0);
    }

} // namespace ITF
