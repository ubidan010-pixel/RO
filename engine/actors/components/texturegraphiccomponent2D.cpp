#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT2D_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
#define UI2D_WIDTH_REF  1280.0f
#define UI2D_HEIGHT_REF 720.0f

    IMPLEMENT_OBJECT_RTTI(TextureGraphicComponent2D)
        BEGIN_SERIALIZATION_CHILD(TextureGraphicComponent2D)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_ENUM_BEGIN("ALIGN", m_align);
    SERIALIZE_ENUM_VAR(align_free);
    SERIALIZE_ENUM_VAR(align_centerX);
    SERIALIZE_ENUM_VAR(align_centerY);
    SERIALIZE_ENUM_VAR(align_centerXY);
    SERIALIZE_ENUM_END();
    SERIALIZE_MEMBER("SCREEN_POURCENT_X",m_screenPourcentX);
    SERIALIZE_MEMBER("SCREEN_POURCENT_Y",m_screenPourcentY);
    SERIALIZE_MEMBER("texture",m_instanceFile);
    SERIALIZE_MEMBER("quadSize",m_quadSize);
    SERIALIZE_MEMBER("angle", m_angle2D);
    SERIALIZE_MEMBER("useAlpha", m_useAlpha);
    END_CONDITION_BLOCK()

        END_SERIALIZATION()

        TextureGraphicComponent2D::TextureGraphicComponent2D()
        : Super()
        , m_depthRank(0)
        , m_align(align_free)
        , m_screenPourcentX(0.0f)
        , m_screenPourcentY(0.0f)
        , m_width(0.0f)
        , m_height(0.0f)
        , m_isWaitingForTex(bfalse)
        , m_quadSize()
        , m_logicalDataOwnerAdded(bfalse)
        , m_drawColor(COLOR_WHITE)
        , m_angle2D(0.f)
        ,m_useAlpha(bfalse)
    {}

    TextureGraphicComponent2D::~TextureGraphicComponent2D()
    {
        clear();
    }

    void TextureGraphicComponent2D::clear()
    {
        if (m_actor && m_textureID.isValidResourceId())
        {
            if(m_logicalDataOwnerAdded)
            {
                RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(m_textureID);
                m_logicalDataOwnerAdded = bfalse;
            }
            else
            {
                m_actor->removeResource(m_textureID);
            }

            m_textureID.invalidateResourceId();
        }
    }

    void TextureGraphicComponent2D::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        if ( !m_instanceFile.isEmpty() )
        {
            m_textureID = m_actor->addResource(Resource::ResourceType_Texture, m_instanceFile);
        }
        else if ( !getTemplate()->getTextureFile().isEmpty() )
        {
            m_textureID = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureFile());
        }

        m_depthRank = getTemplate()->getDepthRank();
        m_isWaitingForTex = btrue;
        if (m_useAlpha) {
            m_alphalColor = Color(m_drawColor);
            m_alpha = m_colorFactor.getAlpha();
        }
    }

    Texture* TextureGraphicComponent2D::getTexture()
    {
        if (m_textureID.isValidResourceId())
            return static_cast<Texture*>(m_textureID.getResource());
        else
            return NULL;
    }

    void TextureGraphicComponent2D::changeTexture( ResourceID _newResourceID )
    {
        clear();

        m_textureID = _newResourceID;
        if(m_textureID.isValidResourceId())
        {
            RESOURCE_MANAGER->queryAddUserToResourceLogicalData(m_textureID);
            m_logicalDataOwnerAdded = btrue;
        }
    }

    void TextureGraphicComponent2D::setScreenPourcentValues()
    {
        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {

            const Vec2d scale = m_actor->getScale();

            int Screen_W = 0, Screen_H = 0;
            SYSTEM_ADAPTER->getWindowSize(Screen_W, Screen_H);

#ifdef ITF_WINDOWS
            if(m_quadSize == Vec2d::Zero)
            {
                m_quadSize.m_x = (f32)tex->getSizeX();
                m_quadSize.m_y = (f32)tex->getSizeY();
            }
#endif //ITF_WINDOWS

            m_screenPourcentX = (100.0f * m_quadSize.m_x * scale.m_x) / UI2D_WIDTH_REF;
            m_screenPourcentY = (100.0f * m_quadSize.m_y * scale.m_y) / UI2D_HEIGHT_REF;

            m_width = Screen_W * m_screenPourcentX * 0.01f;
            m_height = Screen_H * m_screenPourcentY * 0.01f;
        }
    }

    void  TextureGraphicComponent2D::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);

        // Update AABB
        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {
            if(m_isWaitingForTex)
            {
                setScreenPourcentValues();
                m_isWaitingForTex = bfalse;
            }
            int Screen_W = 0, Screen_H = 0;
            SYSTEM_ADAPTER->getWindowSize(Screen_W, Screen_H);

            // Compute texture size displayed on screen depending on its screen pourcentage
            m_width = Screen_W * m_screenPourcentX * 0.01f;
            m_height = Screen_H * m_screenPourcentY * 0.01f;

            const f32 halfTexWidth = m_width * 0.5f;
            const f32 halfTexHeight = m_height * 0.5f;

            const Vec2d vMin = Vec2d(m_actor->getPos().m_x - halfTexWidth, m_actor->getPos().m_y - halfTexHeight);
            const Vec2d vMax = Vec2d(m_actor->getPos().m_x + halfTexWidth, m_actor->getPos().m_y + halfTexHeight);

            const AABB aabb(Vec2d(vMin.m_x,vMin.m_y),Vec2d(vMax.m_x,vMax.m_y));
            m_actor->growRelativeAABBFromAbsolute(aabb);
        }
    }

    void TextureGraphicComponent2D::Draw2D()
    {
        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {
            const Vec2d scale = m_actor->getScale();
            Quad2DInfo texInfo;
            if (m_useAlpha) {
                m_alphalColor.setAlpha(m_alpha);
                texInfo.m_color = m_alphalColor.getAsU32();
            }
            else {
                texInfo.m_color = m_drawColor;
            }

            texInfo.m_width = m_width * scale.m_x;
            texInfo.m_height = m_height * scale.m_y;
            texInfo.m_texture = tex;
            texInfo.m_uvStart = Vec2d::Zero;
            texInfo.m_uvEnd = Vec2d::One;
            texInfo.m_pos2D.m_x = m_actor->getPos().m_x - (texInfo.m_width * 0.5f);
            texInfo.m_pos2D.m_y = m_actor->getPos().m_y - (texInfo.m_height * 0.5f);
            texInfo.m_angle = m_actor->getAngle() + m_angle2D;

            u32 depthRank = UI_TEXTMANAGER->getMenusDepthRank() + 1 - m_depthRank;
            GFX_ADAPTER->addPrimitive2d(Vec2d::Zero, Vec2d::One, depthRank, GFX_QUADS, NULL, NULL, &texInfo);
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void TextureGraphicComponent2D::onEditorCreated( class Actor* _original )
    {
        Super::onEditorCreated(_original);

        // Calculate the relative position
        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {
            setScreenPourcentValues();
            m_isWaitingForTex = bfalse;
        } else
            m_isWaitingForTex = btrue;
    }

    void TextureGraphicComponent2D::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {
            setScreenPourcentValues();
        }

        // Calculate the relative position
    }
#endif // ITF_SUPPORT_EDITOR

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(TextureGraphicComponent2D_Template)
        BEGIN_SERIALIZATION_CHILD(TextureGraphicComponent2D_Template)
        SERIALIZE_MEMBER("texture",m_textureFile);
    SERIALIZE_MEMBER("rank",m_depthRank);
    END_SERIALIZATION()

        TextureGraphicComponent2D_Template::TextureGraphicComponent2D_Template():
    m_depthRank(0)
    {
    }

    TextureGraphicComponent2D_Template::~TextureGraphicComponent2D_Template()
    {
    }
}

