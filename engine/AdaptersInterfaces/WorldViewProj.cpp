#include "precompiled_engine.h"

#include "WorldViewProj.h"

#include "core/math/MathTools.h"

#include "core/math/BooleanLogic.h"

namespace ITF
{
    inline void WorldViewProj::checkDirtyStatesValidity() const
    {
        ITF_ASSERT(imply(m_viewProjDirty, m_worldViewProjDirty)); // m_viewProjDirty => m_worldViewProjDirty as m_viewProjDirty is used to compute m_worldViewProjDirty
        ITF_ASSERT(imply(m_viewProjDirty, m_viewProjInvDirty));   // m_viewProjDirty => m_viewProjInvDirty
    }

    inline void WorldViewProj::invalidateAll() const
    {
        invalidateWorld();
        invalidateView();
        invalidateProj();
            invalidateRenderView();
    }

    inline void WorldViewProj::invalidateWorld() const
    {
        m_worldViewDirty = true;
        m_worldViewProjDirty = true;
            m_worldViewRenderProjDirty = true;
    }

    inline void WorldViewProj::invalidateView() const
    {
        m_worldViewDirty = true;
        m_worldViewProjDirty = true;
        m_viewProjDirty = true;
        m_viewProjInvDirty = true;
        m_viewNoTProjInvDirty = true;
            m_worldViewRenderProjDirty = true;
    }

    inline void WorldViewProj::invalidateProj() const
    {
        m_worldViewProjDirty = true;
        m_viewProjDirty = true;
        m_viewProjInvDirty = true;
        m_viewNoTProjInvDirty = true;
            m_worldViewRenderProjDirty = true;
        }

        inline void WorldViewProj::invalidateRenderView() const
        {
            m_worldViewRenderProjDirty = true;
    }

    // 
    WorldViewProj::WorldViewProj()
        : m_worldViewProjDirty(false)
        , m_worldViewDirty(false)
        , m_viewProjDirty(false)
        , m_viewProjInvDirty(false)
        , m_viewNoTProjInvDirty(false)
            , m_worldViewRenderProjDirty(false)
    {
            m_stateMat.m_viewRender.setIdentity();
        m_stateMat.m_proj.setIdentity();
        m_stateMat.m_view.setIdentity();
        m_stateMat.m_world.setIdentity();

        m_worldView.setIdentity();
        m_viewProj.setIdentity();
        m_worldViewProj.setIdentity();
        m_viewProjInv.setIdentity();
        m_viewNoTProjInv.setIdentity();
            m_worldViewRenderProj.setIdentity();
        checkDirtyStatesValidity();
    }

    void WorldViewProj::setProj(const Matrix44 & _proj)
    {
        m_stateMat.m_proj = _proj;
        invalidateProj();
        checkDirtyStatesValidity();
    }

        void WorldViewProj::setViewRender(const Matrix44 & _viewRender)
        {
            m_stateMat.m_viewRender = _viewRender;
            invalidateRenderView();
            checkDirtyStatesValidity();
        }

    void WorldViewProj::setProjPerspectiveRH(f32 _fovY, f32 _aspect, f32 _znear, f32 _zfar)
    {
        // Perspective projection
        float tan_fov = tanf(_fovY * 0.5f);
        float h =  1.0f / tan_fov;
        float w =  h/_aspect;
        float q = _zfar / ( _znear -  _zfar);

        m_stateMat.m_proj.setFromFloat(
                            w,          0.f,        0.f,        0.f,
                            0.f,        h,          0.f,        0.f,
                            0.f,        0.f,        q,     q*_znear,
                            0.f,        0.f,       -1.f,        0.f
                            );

        invalidateProj();
        checkDirtyStatesValidity();
    }

    void WorldViewProj::setProjOrtho(f32 _l, f32 _r, f32 _b, f32 _t)
    {
        f32 t = _b;
        f32 b = _t;
        f32 n = 1.0f;
        f32 f = -1.0f;
        f32 q = 1.0f/(f-n);
        f32 w = _r-_l;
        f32 h = b-t;

        m_stateMat.m_proj.setFromFloat(
                         2.0f/w,                    0.f,        0.f,        0.f,
                            0.f,                  2.f/h,        0.f,        0.f,
                            0.f,                    0.f,         -q,        0.f,
                (_l+_r)/(_l-_r),    (_t + _b)/( _b- _t),       -q*n,        1.f
                            );
        
        invalidateProj();
        checkDirtyStatesValidity();
    }

    void WorldViewProj::setView(const Matrix44 & _view)
    {
        m_stateMat.m_view = _view;
        invalidateView();
        checkDirtyStatesValidity();
    }

    void WorldViewProj::setViewLookAtRH(const Vector3 & _eye, const Vector3 & _at, const Vector3& _upRef)
    {
        Vector3 zaxis = (_eye - _at).normalize();
        Vector3 xaxis = (_upRef ^ zaxis).normalize();
        Vector3 yaxis = zaxis ^ xaxis;  // assumed normalized (xaxis and zaxis already normalized)

        m_stateMat.m_view.setFromFloat(
                                xaxis.getX(),     yaxis.getX(),     zaxis.getX(),   0.f,
                                xaxis.getY(),     yaxis.getY(),     zaxis.getY(),   0.f,
                                xaxis.getZ(),     yaxis.getZ(),     zaxis.getZ(),   0.f,
                            -xaxis.dot(_eye), -yaxis.dot(_eye), -zaxis.dot(_eye),   1.f
                            );
        invalidateView();
        checkDirtyStatesValidity();
    }

    void WorldViewProj::setWorld(const Matrix44 & _world)
    {
        m_stateMat.m_world = _world;
        invalidateWorld();
        checkDirtyStatesValidity();
    }

    const Matrix44 & WorldViewProj::getView() const
    {
        return m_stateMat.m_view;
    }

    const Matrix44 & WorldViewProj::getWorldView() const
    {
        checkDirtyStatesValidity();
        if (m_worldViewDirty)
        {
                m_worldView = m_stateMat.m_world * m_stateMat.m_view;
            m_worldViewDirty = false;
        }
        checkDirtyStatesValidity();
        return m_worldView;
    }

    const Matrix44 & WorldViewProj::getWorldViewProj() const
    {
        checkDirtyStatesValidity();
        if (m_worldViewProjDirty)
        {
                m_worldViewProj = m_stateMat.m_world * getViewProj();
            m_worldViewProjDirty = false;
        }
        checkDirtyStatesValidity();
        return m_worldViewProj;
    }

    const Matrix44 & WorldViewProj::getViewProj() const
    {
        checkDirtyStatesValidity();
        if (m_viewProjDirty)
        {
                m_viewProj = m_stateMat.m_view * m_stateMat.m_proj;
            m_viewProjDirty = false;
        }
        checkDirtyStatesValidity();
        return m_viewProj;
    }

    const Matrix44 & WorldViewProj::getViewProjInv() const
    {
        checkDirtyStatesValidity();
        if (m_viewProjInvDirty)
        {
                M44_matrixInverse(m_viewProjInv.fa16, getViewProj().fa16); 
            m_viewProjInvDirty = false;
        }
        checkDirtyStatesValidity();
        return m_viewProjInv;
    }

    const Matrix44 & WorldViewProj::getViewNoTProjInv() const
    {
        checkDirtyStatesValidity();
        if (m_viewNoTProjInvDirty)
        {
            Matrix44 viewNoT = m_stateMat.m_view;
                viewNoT._m41 = viewNoT._m42 = viewNoT._m43 = 0.f;

            Matrix44 viewNoTProj;
                M44_mul4x4Out(&viewNoTProj, &viewNoT, &m_stateMat.m_proj);

                Matrix44 invm;
                M44_matrixInverse(m_viewNoTProjInv.fa16, viewNoTProj.fa16); 
            m_viewNoTProjInvDirty = false;
        }
        checkDirtyStatesValidity();
        return m_viewNoTProjInv;            
    }


    const Matrix44 & WorldViewProj::getWorldViewRenderProj() const
    {
        checkDirtyStatesValidity();
        if (m_worldViewRenderProjDirty)
        {
            m_worldViewRenderProj = getWorldView() * m_stateMat.m_viewRender * m_stateMat.m_proj;
            m_worldViewProjDirty = false;
        }
        checkDirtyStatesValidity();
        return m_worldViewRenderProj;
    }

    void WorldViewProj::push() // save temporary
    {
        m_stateStack.push_back(m_stateMat);
    }

    void WorldViewProj::pop() // restore
    {
        m_stateMat = m_stateStack.back();
        invalidateAll();
        m_stateStack.pop_back();
    }

}
