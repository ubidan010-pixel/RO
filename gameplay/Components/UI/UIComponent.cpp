#include "precompiled_gameplay.h"

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_XMLFILE_H_
#include "engine/XML/XMLFile.h"
#endif //_ITF_XMLFILE_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIComponent)
    BEGIN_SERIALIZATION_CHILD(UIComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("RELATIVEPOSX",m_relativePosX);
            SERIALIZE_MEMBER("RELATIVEPOSY",m_relativePosY);
            SERIALIZE_MEMBER("friendly",m_friendly);
            SERIALIZE_MEMBER("id", m_id);
            SERIALIZE_MEMBER("menuBaseName",m_menuFriendly);
            SERIALIZE_MEMBER("menuSonBaseName",m_menuSonFriendly);
            SERIALIZE_MEMBER("locFileName",m_locFileName);
            SERIALIZE_MEMBER("defaultSelectedByInstance",m_bDefaultSelectedByInstance);
            SERIALIZE_ENUM_BEGIN("align", m_align);
                SERIALIZE_ENUM_VAR(align_free);
                SERIALIZE_ENUM_VAR(align_centerX);
                SERIALIZE_ENUM_VAR(align_centerY);
                SERIALIZE_ENUM_VAR(align_centerXY);
            SERIALIZE_ENUM_END();
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIComponent::UIComponent()
    : Super()
    , m_UIref(ITF_INVALID_OBJREF)
    , m_fontHeight(-1.f)
    , m_fontIndex(U32_INVALID)
    , m_bSelected(bfalse)
	, m_canBeSelected(btrue)
    , m_UIComponentID(StringID::Invalid)
    , m_menuID(StringID::Invalid)
    , m_menuSonID(StringID::Invalid)
    , m_relativePosX(0.0f)
    , m_relativePosY(0.0f)
    , m_isContentFill(bfalse)
    , m_textBox(NULL)
    , m_isTextBoxActive(btrue)
    , m_isActive(btrue)
    , m_textChanged(bfalse)
    , m_animLightComponent(NULL)
    , m_align(align_free)
    , m_bDefaultSelectedByInstance(bfalse)
    , m_useRuntimeDepthRank(bfalse)
    , m_runtimeDepthRank(0)
    , m_isPressed(bfalse)
    , m_forceSelected(bfalse)
    , m_animSizeRef(Vec2d::Zero)
    , m_hasColorOverride(bfalse)
    , m_overrideTextColor(Color::white())
    , m_overrideTextColorHighlighted(Color::yellow())
    , m_overrideTextColorInactive(Color::black())
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIComponent::~UIComponent()
    {    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_bSelected = getTemplate()->getIsDefaultSelected();

        if (!getTemplate()->getFriendly().isEmpty())
            m_friendly = getTemplate()->getFriendly();

        m_content = m_friendly;
        m_UIComponentID = m_friendly;
        m_menuID = StringID(getTemplate()->getMenuBaseName().isEmpty() ? m_menuFriendly : getTemplate()->getMenuBaseName());
        m_menuSonID = StringID(m_menuSonFriendly);
        m_isActive = getTemplate()->getIsActive();

        if (!getTemplate()->getFontName().isEmpty())
            m_fontIndex = GFX_ADAPTER->loadFont(getTemplate()->getFontName());

        m_UIref = m_actor->getRef();

        m_animLightComponent    = GetActor()->GetComponent<AnimLightComponent>();
        if(m_animLightComponent)
        {
            m_animLightComponent->setRank2D(getDepthRank());
        }

        UI_MENUMANAGER->addUIComponentToMenu(m_UIref);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::computeRelativeRef()
    {
        // Set ref
        const AABB aabb = m_actor->getAABB();

        m_animSizeRef.set(aabb.getMax().m_x-aabb.getMin().m_x, aabb.getMax().m_y-aabb.getMin().m_y);
        m_animSizeRef = getTemplate()->getAnimSize() * UIComponent::getScreenRatio();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::updateScreenOccupation()
    {
        Vec2d scale = m_actor->getScale();
        if(scale.m_x>10)
        {
            scale = Vec2d::One;
        }

        f32 X = (100.0f * m_animSizeRef.m_x * scale.m_x) / 1280.0f;
        f32 Y = (100.0f * m_animSizeRef.m_y * scale.m_y) / 720.0f;

        //if (fabs(X)<=1.0f)
        {
            m_screenPourcentX = X;
            m_screenPourcentY = Y;
        }
    }


    void UIComponent::onResourceLoaded()
    {
        updateActorPosition();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::updateActorPosition()
    {
        if( (m_animLightComponent && m_animLightComponent->needsDraw2D()) || getTemplate()->getForceUseAnimSize() )
        {
            Vec2d newScale = getTemplate()->getAnimSize();
            Vec2d screenRatio = UIComponent::getScreenRatio();
            newScale *= screenRatio;

            m_actor->setScale(newScale);
        }

        if ( !getTemplate()->getUpdatePos() )
        {
            return;
        }

        Vec2d pos;
        f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
        f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();

        pos.m_x = screenWidth * m_relativePosX;
        pos.m_y = screenHeight * m_relativePosY;

        // Update 2d position
        m_actor->set2DPos(pos);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::updateRelativeAABB()
    {
        int Screen_W = 0, Screen_H = 0;
        SYSTEM_ADAPTER->getWindowSize(Screen_W, Screen_H);

        // Compute texture size displayed on screen depending on its screen pourcentage
        f32 m_width = Screen_W * m_screenPourcentX * 0.01f;
        f32 m_height = Screen_H * m_screenPourcentY * 0.01f;

        const f32 halfTexWidth = m_width * 0.5f;
        const f32 halfTexHeight = m_height * 0.5f;

        Vec2d vMin = Vec2d(m_actor->getPos().m_x - halfTexWidth, m_actor->getPos().m_y - halfTexHeight);
        Vec2d vMax = Vec2d(m_actor->getPos().m_x + halfTexWidth, m_actor->getPos().m_y + halfTexHeight);

        const AABB aabb(Vec2d(vMin.m_x,vMin.m_y),Vec2d(vMax.m_x,vMax.m_y));

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::Update ( f32 _deltaTime )
    {
        if (!m_isContentFill && UI_TEXTMANAGER->isReady())
        {
            fillContent();
            m_isContentFill = btrue;
        }

        // Scale the font and the box to fit with the screen (screen resolution of reference: 800x600)
        const f32 scale = (f32)GFX_ADAPTER->getScreenHeight() / 600.0f;
        m_fontHeight = scale * getTemplate()->getInitialFontHeight();

        if(m_animSizeRef == Vec2d::Zero)
        {
            computeRelativeRef();
            updateScreenOccupation();
        }
        else
        {
            updateRelativeAABB();
        }

        updateActorPosition();

        m_textChanged = bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::fillContent()
    {
        TextInfo textInfo;
        u32 indexStart = 0;
        UI_TEXTMANAGER->getTextByFriendly(StringID(m_locFileName + "." + m_friendly), textInfo, indexStart);
        if (!textInfo.m_text.isEmpty())
            m_content = textInfo.m_text;
    }

    void UIComponent::drawBox( const Vec2d& boxSize, float border, u32 color ) const
    {
        const f32 halfBoxWidth  = boxSize.m_x * 0.5f;
        const f32 halfBoxHeight = boxSize.m_y * 0.5f;

        // Draw box with lines
        Vec2d vN1, vN2, vN3, vN4;
        vN1 = Vec2d(m_actor->getPos().m_x - halfBoxWidth, m_actor->getPos().m_y - halfBoxHeight);
        vN2 = Vec2d(m_actor->getPos().m_x - halfBoxWidth, m_actor->getPos().m_y + halfBoxHeight);
        vN3 = Vec2d(m_actor->getPos().m_x + halfBoxWidth, m_actor->getPos().m_y - halfBoxHeight);
        vN4 = Vec2d(m_actor->getPos().m_x + halfBoxWidth, m_actor->getPos().m_y + halfBoxHeight);


        GFX_ADAPTER->draw2dLine(vN1, vN2, border, color);
        GFX_ADAPTER->draw2dLine(vN1, vN3, border, color);
        GFX_ADAPTER->draw2dLine(vN3, vN4, border, color);
        GFX_ADAPTER->draw2dLine(vN2, vN4, border, color);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Vec2d UIComponent::getScreenRatio()
    {
        f32 widthRatio = GFX_ADAPTER->getScreenWidth() / UI2D_WidthRef;
        f32 heightRatio = GFX_ADAPTER->getScreenHeight() / UI2D_HeightRef;
        return Vec2d(widthRatio, heightRatio);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    i32 UIComponent::getDepthRank() const
    {
        // use runtime depth rank if set
        if (m_useRuntimeDepthRank)
        {
            return m_runtimeDepthRank;
        }
        return (UI_TEXTMANAGER->getMenusDepthRank() - getTemplate()->getRank());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 UIComponent::getColor() const
    {
        u32 pU32TextColor;
        
        if (m_hasColorOverride)
        {
            if (m_bSelected)
                pU32TextColor = m_overrideTextColorHighlighted.getAsU32();
            else if (m_isActive)
                pU32TextColor = m_overrideTextColor.getAsU32();
            else
                pU32TextColor = m_overrideTextColorInactive.getAsU32();
        }
        else
        {
            if (m_bSelected)
                pU32TextColor = getTemplate()->getTextColorHighlighted().getAsU32();
            else if (m_isActive)
                pU32TextColor = getTemplate()->getTextColor().getAsU32();
            else
                pU32TextColor = getTemplate()->getTextColorInactive().getAsU32();
        }

        return pU32TextColor;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 UIComponent::getActorColor() const
    {
        u32 pU32TextColor;
        if (m_bSelected)
            pU32TextColor = getTemplate()->getActorColorHiglighted().getAsU32();
        else
        {
            if (m_isActive)
                pU32TextColor = getTemplate()->getActorColor().getAsU32();
            else
                pU32TextColor = getTemplate()->getActorColorInactive().getAsU32();
        }

        return pU32TextColor;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 UIComponent::getShadowColor() const
    {
        return getTemplate()->getTextShadowColor().getAsU32();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 UIComponent::getLineSpacingFactor() const
    {
         return getTemplate()->getLineSpacingFactor();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const Vec2d& UIComponent::getShadowOffset() const
    {
        return getTemplate()->getShadowOffset();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 UIComponent::getFontHeight() const
    {
        if(!getTemplate())
            return 1.0f;

        f32 _FontHeight = (getIsSelected() || m_forceSelected) ?  getTemplate()->getFontHeightSelected() : getTemplate()->getInitialFontHeight();        
        if(_FontHeight<0 && (getIsSelected() || m_forceSelected))
              _FontHeight = getTemplate()->getInitialFontHeight();
        return _FontHeight;// * getScreenRatio().m_x;
    }

    void UIComponent::get2DBoundingBox( Vec2d& boxMinPosition, Vec2d&  boxMaxPosition )
    {
        Actor* pActor = GetActor();
        if (!pActor)
            return;

        UITextBox* pUITextBox = pActor->GetComponent<UITextBox>();
        if(pUITextBox)
        {
            boxMinPosition = Vec2d::Zero;
            boxMaxPosition = Vec2d::Zero;

            const f32 boxWidth      = pUITextBox->getBoxWidth();
            const f32 boxHeight     = pUITextBox->getBoxHeight();

            const f32 halfBoxWidth  = boxWidth * 0.5f;
            const f32 halfBoxHeight = boxHeight * 0.5f;

            const Vec3d& actorPos = pActor->getPos();
            boxMinPosition.set(actorPos.m_x - halfBoxWidth, actorPos.m_y - halfBoxHeight);
            boxMaxPosition.set(actorPos.m_x + halfBoxWidth, actorPos.m_y + halfBoxHeight);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::get2DBoundingBox( AABB& bb )
    {
        Vec2d min;
        Vec2d max;
        get2DBoundingBox(min, max);
        if (min.isValid() && max.isValid())
        {
            bb = AABB(Vec3d(min.m_x, min.m_y, 0.f), Vec3d(max.m_x, max.m_y, 0.f));
        }
        else
        {
            bb.invalidate();
        }
    }
#ifdef ITF_SUPPORT_EDITOR
    void UIComponent::drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const
    {
        //        drawBox(getTemplate()->getAnimSize()*1.1, 2.0f, COLOR_RED);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::onEditorCreated( class Actor* _original )
    {
        Super::onEditorCreated(_original);

        // Calculate the relative position
        m_relativePosX = m_actor->getPos().m_x / (f32)GFX_ADAPTER->getScreenWidth();
        m_relativePosY = m_actor->getPos().m_y / (f32)GFX_ADAPTER->getScreenHeight();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        // update text
        m_content = m_friendly;

        // Calculate the relative position
        f32 X = m_actor->getPos().m_x / (f32)GFX_ADAPTER->getScreenWidth();
        f32 Y = m_actor->getPos().m_y / (f32)GFX_ADAPTER->getScreenHeight();
        if (fabs(X)<=1.0f)
        {
            m_relativePosX = X;
            m_relativePosY = Y;

            switch (m_align)
            {
            case align_centerX:
                m_relativePosX = 0.50f;
                break;
            case align_centerY:
                m_relativePosY = 0.50f;
                break;
            case align_centerXY:
                m_relativePosX = 0.50f;
                m_relativePosY = 0.50f;
                break;
            case align_free:
            default: break;
            }

            updateScreenOccupation();
            updateActorPosition();
        }
    }


#endif // ITF_SUPPORT_EDITOR

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIComponent_Template)
    BEGIN_SERIALIZATION(UIComponent_Template)
        SERIALIZE_MEMBER("fontName",m_fontName);
        SERIALIZE_MEMBER("fontHeight",m_initialFontHeight);
        SERIALIZE_MEMBER("textColor",m_textColor);
        SERIALIZE_MEMBER("textColorHighlighted",m_textColorHighlighted);
        SERIALIZE_MEMBER("textColorInactive",m_textColorInactive);
        SERIALIZE_MEMBER("actorColorHighlighted",m_actorColorHiglighted);
        SERIALIZE_MEMBER("actorColorInactive",m_actorColorInactive);
        SERIALIZE_MEMBER("actorColor",m_actorColor);
        SERIALIZE_MEMBER("textMode",m_textMode);
        SERIALIZE_MEMBER("textModeY",m_textModeY);
        SERIALIZE_MEMBER("defaultSelected",m_bDefaultSelected);
        SERIALIZE_MEMBER("isActive",m_isActive);
        SERIALIZE_MEMBER("updatePos",m_updatePos);
        SERIALIZE_MEMBER("rank",m_rank);
        SERIALIZE_MEMBER("friendly",m_friendly);
        SERIALIZE_MEMBER("menuBaseName",m_menuBaseName);
        SERIALIZE_MEMBER("animSize",m_animSize);
        SERIALIZE_MEMBER("fontHeightSelected",m_FontHeightSelected);
        SERIALIZE_MEMBER("textShadowOffset",m_textShadowOffset);
        SERIALIZE_MEMBER("textShadowColor",m_textShadowColor);
        SERIALIZE_MEMBER("lineSpacingFactor",m_lineSpacingFactor);
        SERIALIZE_MEMBER("forceUseAnimSize",m_forceUseAnimSize);
    END_SERIALIZATION()

    UIComponent_Template::UIComponent_Template()
    : m_initialFontHeight(-1.f)
    , m_textColor(Color::white())
    , m_textColorHighlighted(Color::yellow())
    , m_textColorInactive(Color::black())
    , m_actorColorHiglighted(Color::white())
    , m_actorColorInactive(Color::white())
    , m_actorColor(Color::white())
    , m_textMode(0)
    , m_textModeY(0)
    , m_bDefaultSelected(bfalse)
    , m_isActive(btrue)
    , m_updatePos(btrue)
    , m_rank(0)
    , m_friendly("")
    , m_menuBaseName()
    , m_animSize(Vec2d::One)
    , m_FontHeightSelected(-1.f)
    , m_textShadowOffset(Vec2d::Zero)
    , m_textShadowColor(Color::black())
    , m_forceUseAnimSize(bfalse)
    , m_lineSpacingFactor(1.f)
    {
    }

    UIComponent_Template::~UIComponent_Template()
    {
    }
}
