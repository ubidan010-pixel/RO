#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#define _ITF_TEXTUREGRAPHICCOMPONENT2D_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

namespace ITF
{
    class TextureGraphicComponent2D : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TextureGraphicComponent2D,GraphicComponent,1712599772);

    public:
        DECLARE_SERIALIZE()

        TextureGraphicComponent2D();
        ~TextureGraphicComponent2D();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return btrue; }
        virtual void  Draw2D();
        virtual void  Update(f32 _deltaTime);
        virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
#ifdef ITF_SUPPORT_EDITOR
        virtual void  onEditorCreated( class Actor* _original );
        virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE const class TextureGraphicComponent2D_Template*  getTemplate() const;
        void            changeTexture(ResourceID _newResourceID);
        Texture*            getTexture();

        u32 getDrawColor() const { return m_drawColor; }
        void setDrawColor(u32 col) { m_drawColor = col; }
        void setAngle2D(f32 angleRad) { m_angle2D = angleRad; }
        f32  getAngle2D() const { return m_angle2D; }

        Vec2d getQuadSize() const { return m_quadSize; }
        void setQuadSize(const Vec2d& quadSize)
        {
            m_quadSize = quadSize;
            m_isWaitingForTex = btrue;
        }
    private:
        void                clear();
        void                setScreenPourcentValues();
        const Path&         getTextureFile() const;

        bbool               m_logicalDataOwnerAdded;
        ResourceID          m_textureID;

        // Percentage of the screen
        u32                 m_depthRank;
        f32                 m_screenPourcentX, m_screenPourcentY;
        f32                 m_width, m_height;
        Vec2d               m_quadSize;
        bbool               m_isWaitingForTex;
        u32                 m_drawColor;
        f32                 m_angle2D;
        bbool               m_useAlpha;
        Color               m_alphalColor;

        enum QuadAlign
        {
            align_free,
            align_centerX,
            align_centerY,
            align_centerXY
        };
        u32                 m_align;
        Path                m_instanceFile;
    };

    class TextureGraphicComponent2D_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TextureGraphicComponent2D_Template,GraphicComponent_Template,2729231470);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TextureGraphicComponent2D);

    public:

        TextureGraphicComponent2D_Template();
        ~TextureGraphicComponent2D_Template();

        const Path&         getTextureFile() const { return m_textureFile; }
        u32                 getDepthRank() const { return m_depthRank; }
    private:

        Path                m_textureFile;
        u32                 m_depthRank;
    };



    const TextureGraphicComponent2D_Template*  TextureGraphicComponent2D::getTemplate() const {return static_cast<const TextureGraphicComponent2D_Template*>(m_template);}
    ITF_INLINE const Path& TextureGraphicComponent2D::getTextureFile() const
    {
        return getTemplate()->getTextureFile();
    }
}
#endif // _ITF_TEXTUREGRAPHICCOMPONENT2D_H_

