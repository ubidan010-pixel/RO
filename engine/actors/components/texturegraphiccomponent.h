
#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#define _ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{

class TextureGraphicComponent : public GraphicComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TextureGraphicComponent,GraphicComponent,2068359598);

public:
    DECLARE_SERIALIZE()

    TextureGraphicComponent();
    ~TextureGraphicComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return btrue; }
    virtual void  Draw();
    virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void  Update( f32 _deltaTime );
#ifdef ITF_SUPPORT_EDITOR
    virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
    void setTextureFile(String & _textureFile) { m_textureFile = _textureFile; }

    ITF_INLINE const Color&         getDefaultColor() const;

    ITF_INLINE const class TextureGraphicComponent_Template* getTemplate() const;

private:

    void            clear();
    void            reset();


    Path                    m_textureFile;
    ResourceID              m_textureID;
    GFX_3DQUAD              m_quad;
    Vec3d                   m_curAngle;
};

//-------------------------------------------------------------------------------------
class TextureGraphicComponent_Template : public GraphicComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(TextureGraphicComponent_Template,GraphicComponent_Template,2628666149);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(TextureGraphicComponent);

public:

    TextureGraphicComponent_Template();
    ~TextureGraphicComponent_Template() {}


    ITF_INLINE const Path&   getTextureFile()   const {return m_textureFile;}
    ITF_INLINE const Color&  getDefaultColor()  const {return m_defaultColor;}
    ITF_INLINE const Angle&  getAngleX()        const {return m_angleX;}
    ITF_INLINE const Angle&  getAngleY()        const {return m_angleY;}
    ITF_INLINE const Angle&  getAngleZ()        const {return m_angleZ;}
    ITF_INLINE f32           getSpeedRotX()     const {return m_speedRotX;}
    ITF_INLINE f32           getSpeedRotY()     const {return m_speedRotY;}
    ITF_INLINE f32           getSpeedRotZ()     const {return m_speedRotZ;}

private:

    Path                    m_textureFile;
    Color                   m_defaultColor;
    Angle                   m_angleX;
    Angle                   m_angleY;
    Angle                   m_angleZ;
    f32                     m_speedRotX;
    f32                     m_speedRotY;
    f32                     m_speedRotZ;
};


const TextureGraphicComponent_Template*  TextureGraphicComponent::getTemplate() const {return static_cast<const TextureGraphicComponent_Template*>(m_template);}
const Color&         TextureGraphicComponent::getDefaultColor() const { return getTemplate()->getDefaultColor(); }


}

#endif // _ITF_GRAPHICCOMPONENT_H_

