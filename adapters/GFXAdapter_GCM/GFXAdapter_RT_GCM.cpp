#include "precompiled_GFXAdapter_GCM.h"

#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#include "core/math/Vec2d.h"
#include "engine/display/Camera.h"
#include "engine/display/View.h"
#include "GFXAdapter_GCM.h"
#include "Surface_GCM.h"
#include "Texture_GCM.h"

///////////////////////////////////////////////////////////////////////////////
// Render to target methods
///////////////////////////////////////////////////////////////////////////////

namespace ITF
{
    void GFXAdapter_GCM::cleanupRenderTarget(renderTarget* _target)
    {
        if (_target)
        {
            if (_target->getSurface())
            {
                GCM::Surface * surf = reinterpret_cast<GCM::Surface *>(_target->getSurface());
                delete surf;
                _target->setSurface(0);
            }

            if ( _target->getTexture() && _target->getTexture()->m_adapterimplementationData )
            {
                GCM::Texture * tex = reinterpret_cast<GCM::Texture *>(_target->getTexture()->m_adapterimplementationData);
                delete tex;
                _target->getTexture()->m_adapterimplementationData = 0;
            }
        }
    }


    void GFXAdapter_GCM::enableRenderTarget(renderTarget* _target)
    {
        if (!_target || !_target->getSurface()) return;

        _target->m_prevColourSurface = u32(m_currentRenderTarget);
        GCM::Surface * newRT = reinterpret_cast<GCM::Surface *>(_target->getSurface());
        setRenderTarget(newRT);
    }

    void GFXAdapter_GCM::disableRenderTarget(renderTarget* _target)
    {
        if (!_target || !_target->getSurface() || !_target->m_prevColourSurface) return;

        GCM::Surface * prevRT = reinterpret_cast<GCM::Surface *>(_target->m_prevColourSurface);
        setRenderTarget(prevRT);
    }

    void GFXAdapter_GCM::clearRenderTarget( renderTarget* _target, u32 _color)
    {
        // switch to targetting our texture
        enableRenderTarget(_target);
        
        // Clear it.
        m_rdrCtxt.gcmCtxt->SetClearColor(ITFCOLOR_TO_ARGB(_color)); // color is in ARGB format !
        m_rdrCtxt.gcmCtxt->SetClearSurface(CELL_GCM_CLEAR_R | CELL_GCM_CLEAR_G | CELL_GCM_CLEAR_B | CELL_GCM_CLEAR_A);

        // switch back to rendering to previous target
        disableRenderTarget(_target);
    }

    void GFXAdapter_GCM::initialiseRenderTarget( renderTarget* _target)
    {
        if (_target->getTexture()->m_adapterimplementationData) return;

        u32 width =  u32(_target->m_area[2] - _target->m_area[0]);
        u32 height = u32(_target->m_area[3] - _target->m_area[1]);

        // adjust resolution :
        // max 512*512
        // pow2
        width = std::min(width, 512u);
        height = std::min(height, 512u);
        ITF_ASSERT(width <= (1u << GCM::GetLog2(width)));
        ITF_ASSERT(height <= (1u << GCM::GetLog2(width)));
        width = 1u << GCM::GetLog2(width);
        height = 1u << GCM::GetLog2(height);
        if (width==1u && height != 1u)
        {
            width = height; // constraint width==1 => height==1 
        }

        if (!height || !width) return;

        GCM::Surface * surf = NULL;
        GCM::Texture * tex = NULL;

        surf = GCM::CreateSwizzledSurface(width, height, 4);
        if (!surf) return;
        tex = GCM::CreateTextureFromSurface(surf);
        if (!tex)
        {
            delete surf;
            return;
        }

        _target->setSurface((u32)surf);
        _target->getTexture()->m_adapterimplementationData = (u32)tex;

        // Set Original data Size.
        Texture *texture = _target->getTexture();
        texture->m_datasizeY = height;
        texture->m_datasizeX = width;
        texture->setSize(width, height);
        updateCachedScaleValues(_target->getTexture());
    }

    void GFXAdapter_GCM::resizeRenderTarget( renderTarget *_target , const Size& _size)
    {
        ITF_ASSERT(0); // Only useful for HD screenshots
    }

    void GFXAdapter_GCM::startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear)
    {
        _rt->set();

        if (_mustClear)
        {
            m_rdrCtxt.gcmCtxt->SetClearColor(ITFCOLOR_TO_ARGB(_rt->getBackGroundColor())); // color is in ARGB format !
            m_rdrCtxt.gcmCtxt->SetClearSurface(CELL_GCM_CLEAR_R | CELL_GCM_CLEAR_G | CELL_GCM_CLEAR_B | CELL_GCM_CLEAR_A);
        }

        setOrthoView(xmin, xmax, ymin, ymax);

        GFX_RECT Viewport;
        Viewport.left = 0;
        Viewport.top = 0;
        Viewport.right = _rt->m_area[2];
        Viewport.bottom = _rt->m_area[3];
        setupViewport(&Viewport);

        /// Ready to go !!!.
    }

    void GFXAdapter_GCM::stopImpostorRendering(renderTarget* _rt)
    {
        _rt->unSet();
        /// restore backbuffer and copy result of afterfx texture.
        //setBackBuffer();

        /// restore 3d View.
        CAMERA->apply();
    }

} // namespace ITF
