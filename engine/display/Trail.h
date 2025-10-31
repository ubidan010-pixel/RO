#ifndef _ITF_TRAIL_H_
#define _ITF_TRAIL_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BONEID_H_
#include "engine/animation/BoneID.h"
#endif //_ITF_BONEID_H_


namespace ITF
{
    class TrailPoint
    {
    public:
        Vec3d   m_pos;
        Vec2d   m_norm;
        f32     m_thickness;
        Vec2d   m_uv1;
        Vec2d   m_uv2;

        f32     m_alpha;
        i32     m_life;
    };

    class Trail
    {
    private:

        const class Trail_Template* m_template;
    public:
        DECLARE_SERIALIZE();

        Trail();
        ~Trail();

        bbool            directDraw();
        ITF_INLINE const Path &  getTextureFile();
        ResourceID &    getTexture() { return m_texture; }
        void            setTexture(ResourceID _texture) { m_texture = _texture; }
        const StringID &getAttachedBone() { return m_attachBone; }

        void            addPoint(Vec3d &_pos, f32 _angle);
        void            update();
        void            startFade();
        bbool           fade(f32 _deltaTime);

        void            updateAABB(AABB &_aabb);
        void            multiplyColor( const Color& _color );
        void            clear();

        void            setTemplate(const Trail_Template* _template)  {m_template = _template;}

        void            setThicknessScale(f32 _scale) {m_thicknessScale = _scale;}
        void            setIs2D(bbool _is2D) {m_is2D = _is2D;}
        void            setAlphaMultiplier(f32 _alphaMultiplier) {m_alphaMultiplier = _alphaMultiplier;}

        void            setColor( const Color& _color );
        void            resetSetColor();

#if defined(ANIMUSESHADER) || defined(ANIMUSEGEOMETRYSHADER)
#else
        void            initMesh();
        ITF_Mesh *      getMesh() { return &m_mesh; }
#endif

        void            initData();
        void            setUseLife(bbool _useLife) { m_useLife = _useLife;}
    private:


        ITF_INLINE const class Trail_Template*  getTemplate() const;

        u32                         m_hdiv;
        u32                         m_vdiv;

        ResourceID                  m_texture;
        StringID                    m_attachBone;

        Color                       m_color;
        f32                         m_thicknessScale;
        bbool                       m_is2D;
        f32                         m_alphaMultiplier;
        

        f32                         m_trailFaidingValue;

        u32                         m_currTrailLenght;
        u32                         m_currTrailBegin;
        SafeArray<TrailPoint>       m_trailPointTab;

#if defined(ANIMUSESHADER) || defined(ANIMUSEGEOMETRYSHADER)
        SafeArray<Vec3d>            m_shaderPointBuffer;
#else
        ITF_Mesh                    m_mesh;
        #ifndef VBMANAGER_USE_DYNAMICRING_VB
        SafeArray<VertexPCT>        m_meshPointBuffer;
        #endif // VBMANAGER_USE_DYNAMICRING_VB
#endif
        bbool                       m_useLife;

        bbool                       m_useSetColor;
        Color                       m_setColor;
    };


    class Trail_Template
    {
        DECLARE_SERIALIZE()

    public:

        Trail_Template();
        ~Trail_Template() {}

        const Path&             getTextureFile() const { return m_textureFile; }
        u32                     getNBFrames() const { return m_nbFrames; }
        f32                     getTrailFadingTime() const { return m_trailFadingTime; }
        f32                     getThicknessBegin() const { return m_thicknessBegin; }
        f32                     getThicknessEnd() const { return m_thicknessEnd; }
        f32                     getAlphaBegin() const { return m_alphaBegin; }
        f32                     getAlphaEnd() const { return m_alphaEnd; }
        f32                     getTrailBlending() const { return m_trailBlending; }
        f32                     getFadeLength() const { return m_fadeLength; }
        GFX_BLENDMODE           getBlendMode() const { return m_blendMode; }
        const Color&            getInitColor() const { return m_initColor; }
        const StringID&         getAttachBone() const { return m_attachBone; }

    private:

        Path                    m_textureFile;
        u32                     m_nbFrames;
        f32                     m_trailFadingTime;
        f32                     m_thicknessBegin;
        f32                     m_thicknessEnd;
        f32                     m_alphaBegin;
        f32                     m_alphaEnd;
        f32                     m_trailBlending;
        f32                     m_fadeLength;
        GFX_BLENDMODE           m_blendMode;
        Color                   m_initColor;
        StringID                m_attachBone;

    };



    ITF_INLINE const Trail_Template*  Trail::getTemplate() const {return static_cast<const Trail_Template*>(m_template);}

    ITF_INLINE const Path &  Trail::getTextureFile() { return getTemplate()->getTextureFile(); }
}

#endif // _ITF_TRAIL_H_

