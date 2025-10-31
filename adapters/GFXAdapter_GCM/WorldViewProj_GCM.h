#if defined (ITF_PS3)

#ifndef ITF_WORLD_VIEW_PROJ_GCM_H_
#define ITF_WORLD_VIEW_PROJ_GCM_H_

#ifndef _ITF_MATRIX44_H_
#include "core/math/matrix44/matrix44.h"
#endif //_ITF_MATRIX44_H_

// this class is handling World/View/Proj matrices
// It's handling modifications of these matrices, avoiding excessive recomputations

namespace ITF
{
    namespace GCM
    {
        // Vertex Declaration
        class WorldViewProj
        {
        public:
            typedef ITF::Matrix44 Matrix44; // TODO -> choose the best matrix type
            typedef ITF::Vec3d Vector3;

            WorldViewProj();

            void setProj(const Matrix44 & _proj);
            void setProjPerspectiveRH(f32 _fovY, f32 _aspect, f32 _znear, f32 _zfar);
            void setProjOrtho(f32 _l, f32 _r, f32 _b, f32 _t);
            void setView(const Matrix44 & _view);
            void setViewRender(const Matrix44 & _viewRender); // set an additional matrix used only for rendering
            void setViewLookAtRH(const Vector3 & _eye, const Vector3 & _at, const Vector3& _upRef);
            void setWorld(const Matrix44 & _world);

            void push(); // save temporary
            void pop(); // restore

            void updateParameterDB(class FxParameterDB & fxParamDB) const;

            const Matrix44 & getView() const;
            const Matrix44 & getWorldViewProj() const;
            const Matrix44 & getWorldView() const;
            const Matrix44 & getViewProj() const;
            const Matrix44 & getViewProjInv() const;
            const Matrix44 & getViewNoTProjInv() const;
            const Matrix44 & getWorldViewRenderProj() const;

        private:
            struct State
            {
                Matrix44 m_viewRender;
                Matrix44 m_proj;
                Matrix44 m_view;
                Matrix44 m_world;
            };

            State m_stateMat;
            Vector<State> m_stateStack;

            mutable bool m_worldViewProjDirty;
            mutable bool m_worldViewDirty;
            mutable bool m_viewProjDirty;
            mutable bool m_viewProjInvDirty;
            mutable bool m_viewNoTProjInvDirty;
            mutable bool m_worldViewRenderProjDirty;

            // mutables (cache of the matrix multiply)
            mutable Matrix44 m_worldView; // = m_world * m_view
            mutable Matrix44 m_viewProj; // = m_view * m_proj
            mutable Matrix44 m_worldViewProj; // = m_world * m_view * m_proj
            mutable Matrix44 m_viewProjInv; // ((m_view * m_proj)-1)
            mutable Matrix44 m_viewNoTProjInv; // ((m_viewWithoutTrans * m_proj)-1)
            mutable Matrix44 m_worldViewRenderProj; // m_world * m_view * m_render * m_proj
            
            // Several conditions between dirty booleans :
            // for instance : m_worldViewDirty => m_worldViewProjDirty
            // ie. ITF_ASSERT(!m_worldViewDirty || m_worldViewProjDirty);
            // everything is checked in :
            void checkDirtyStatesValidity() const;

            void invalidateAll() const;
            void invalidateWorld() const;
            void invalidateView() const;
            void invalidateProj() const;
            void invalidateRenderView() const;

        };
    }
}

#endif // ITF_WORLD_VIEW_PROJ_GCM_H_

#endif // ITF_PS3
