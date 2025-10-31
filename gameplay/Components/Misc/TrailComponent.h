
#ifndef _ITF_TRAILCOMPONENT_H_
#define _ITF_TRAILCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{
    //#define _PATCH_USESHADER

    class Lua;

    class TrailPointInfo
    {
    public:
        Vec2d   m_pos;
        Vec2d   m_norm;
        f32     m_thickness;
        Vec2d   m_uv1;
        Vec2d   m_uv2;

        f32     m_alpha;
        i32     m_life;
    };
    void drawPatchCurve(Patch32Data *_pData, Texture *texture, TrailPointInfo *_ppA, TrailPointInfo  *_ppB, u32 _hdiv, u32 _vdiv, f32 depth);

    class TrailComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TrailComponent,GraphicComponent,2721209168);
        DECLARE_SERIALIZE();

    public:

        TrailComponent();
        ~TrailComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const;
        virtual bbool needsDraw2D() const;
        virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void  onResourceReady();
        virtual void    Update( f32 _deltaTime );
        virtual void    Draw();
        virtual void    Draw2D();
        void            DirectDraw();
        virtual void onEvent( Event * _event);

        void                clear();

    private:

        ITF_INLINE const class TrailComponent_Template*  getTemplate() const;

        void        updateAABB();
        void        getAttachPos( Vec2d& _pos );

        static      Patch32Data *m_PData;
        static      u32          m_PDataCount;

        ResourceID  m_texture;

        bbool       m_trailActive;
      
        bbool       m_trailFaiding;
        f32         m_trailFaidingTime;
        f32         m_trailFaidingValue;

        u32             m_currTrailLenght;
        u32             m_currTrailBegin;
        TrailPointInfo *m_trailPointTab;

        ITF_Mesh        m_mesh;
        BoneID      m_attachBone;

        AnimLightComponent* m_animComponent;

    };

    //-------------------------------------------------------------------------------------
    class TrailComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TrailComponent_Template,GraphicComponent_Template,3058571470);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TrailComponent);

    public:

        TrailComponent_Template();
        ~TrailComponent_Template() {}

        const Path&     getTextureFile() const { return m_textureFile; }
        u32             getNBFrames() const { return m_nbFrames; }
        f32             getThicknessBegin() const { return m_thicknessBegin; }
        f32             getThicknessEnd() const { return m_thicknessEnd; }
        f32             getAlphaBegin() const { return m_alphaBegin; }
        f32             getAlphaEnd() const { return m_alphaEnd; }
        bbool           getStartActive() const { return m_startActive; }
        f32             getTrailFadingTime() const { return m_trailFadingTime; }
        f32             getTrailBlending() const { return m_trailBlending; }
        const BoneID&   getAttachBone() const { return m_attachBone; }
        bbool           getDraw2D() const { return m_draw2D; }

    private:

        Path        m_textureFile;
        u32         m_nbFrames;
        f32         m_thicknessBegin;
        f32         m_thicknessEnd;
        f32         m_alphaBegin;
        f32         m_alphaEnd;
        bbool       m_startActive;
        f32         m_trailFadingTime;
        f32         m_trailBlending;
        BoneID      m_attachBone;
        bbool       m_draw2D;
    };


    const TrailComponent_Template*  TrailComponent::getTemplate() const {return static_cast<const TrailComponent_Template*>(m_template);}

}

#endif // _ITF_TRAILCOMPONENT_H_

