#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#define _ITF_GFXADAPTER_RENDERTARGET_H_

namespace ITF
{
    class Texture;

    class renderTarget
    {
    public:
            renderTarget();
            ~renderTarget();

            int     m_area[4];
            uPtr    m_prevColourSurface;

            //! holds info about the texture surface before we released it for reset.
            void*   m_savedSurfaceDesc;
            //! true when d_savedSurfaceDesc is valid and texture can be restored.
            bool    m_savedSurfaceDescValid;

            void init();
            void create( f32 _w, f32 _h );
            void recreate();
            void destroy();
            void clean();
            void set();
            void clear(u32 _color);
            void unSet();
            
            ITF_INLINE Texture* getTexture() { return m_Texture; }
            ITF_INLINE uPtr getSurface() { return m_surface; }
            ITF_INLINE void setSurface(uPtr _surf) { m_surface = _surf; }
            ITF_INLINE void setBackGroundColor (u32 _color) { m_BackGroundColor = _color; }
            ITF_INLINE u32 getBackGroundColor () { return m_BackGroundColor; }

            void setDelayedClear(u32 _color)
            {
                m_needDelayedClear = true;
                m_neededDelayedClearColor = _color;
            }

            void clearDelayedClear()
            {
                m_needDelayedClear = false;
            }

            bool needDelayedClear() const { return m_needDelayedClear; }
            u32 delayedClearColor() const { return m_neededDelayedClearColor; }

    private:
            u32      m_BackGroundColor;
            Texture* m_Texture;
            uPtr     m_surface;
            bool     m_needDelayedClear = false;
            u32      m_neededDelayedClearColor = 0;
    };

    class RenderTargetManager
    {
    public:
            RenderTargetManager();
            ~RenderTargetManager();

            void cleanAllRenderTarget();
            void recreateAllRenderTarget();
            void destroyAllRenderTarget();
            void addRenderTarget(renderTarget* _target);
            void removeRenderTarget(renderTarget* _target);

            ITF_INLINE u32 getNumerTarget() {return m_numberTarget;};
            ITF_INLINE ITF_VECTOR<renderTarget*> getTargetList() {return m_targetList;};

    private:
            u32 m_numberTarget;
            ITF_VECTOR<renderTarget*> m_targetList;
    };

} // namespace ITF

#endif // _ITF_GFXADAPTER_RENDERTARGET_H_
