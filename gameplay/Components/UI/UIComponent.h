#ifndef _ITF_UICOMPONENT_H_
#define _ITF_UICOMPONENT_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{
    class XMLFile;
    class UITextBox;
    class AnimLightComponent;

    static const f32 UI2D_WidthRef = 1280.0f;
    static const f32 UI2D_HeightRef = 720.0f;

    class UIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIComponent, ActorComponent,2232305413);

    public:
        DECLARE_SERIALIZE()

        UIComponent();
        ~UIComponent();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
        virtual     void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     void        Update              ( f32 _deltaTime );
        virtual     void        drawBox(const Vec2d& boxSize, float border, u32 color) const;

#ifdef ITF_SUPPORT_EDITOR
        virtual     void        drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const ;
        virtual     void        onEditorCreated( class Actor* _original );
        virtual     void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE  const String8& getFriendly() const { return m_friendly; }
        ITF_INLINE  ObjectRef   getUIref            (       ) const             { return  m_UIref; }
        ITF_INLINE  bbool       getIsSelected       (       ) const             { return  m_bSelected; }
        ITF_INLINE  void        setIsSelected       ( bbool _bSelected )        { m_bSelected = _bSelected;  m_textChanged = btrue;}
        ITF_INLINE  void        setIsForceSelected  ( bbool _bSelected )        { m_forceSelected = _bSelected; m_textChanged = btrue; }
		ITF_INLINE	void		setCanBeSelected	( bbool _bCanBeSelected)	{ m_canBeSelected = _bCanBeSelected;}
        ITF_INLINE  bbool       getCanBeSelected    (       ) const             { return  m_canBeSelected; }
        ITF_INLINE  StringID    getMenuID           (       ) const             { return  m_menuID; }
        ITF_INLINE  StringID    getMenuSonID        (       ) const             { return  m_menuSonID; }
        ITF_INLINE  bbool       getActive           (       ) const             { return  m_isActive; }
        ITF_INLINE  void        setActive           (bbool _isActive)           { m_isActive = _isActive; m_textChanged = btrue; }
		ITF_INLINE  void        setFriendly         (const String8& _friendly)  { m_friendly = _friendly; }
		ITF_INLINE  void        forceContent        (const String& _content)    { m_content = _content; m_textChanged = btrue; }
        ITF_INLINE  LocalisationId getLineId        (       ) const             { return m_lineId; }
        ITF_INLINE  void        setLineId           (LocalisationId _val)       { m_lineId = _val; m_textChanged = btrue; }
        ITF_INLINE  const StringID&   getID               (       ) const             { return m_id; }
        void                    fillContent         (       );
        u32                     getColor() const;
        u32                     getActorColor() const;
        f32                     getFontHeight() const;
        ITF_INLINE  void        setRelativePos(const Vec2d & m_relativePos) { m_relativePosX = m_relativePos.m_x; m_relativePosY = m_relativePos.m_y; }
        f32                     getRelativePosX() const { return m_relativePosX; }
        f32                     getRelativePosY() const { return m_relativePosY; }

        u32                     getShadowColor() const;
        const Vec2d&            getShadowOffset() const;
        f32                     getLineSpacingFactor() const;

#ifdef ITF_SUPPORT_EDITOR
        virtual     void        onEditorSelected    () {}
#endif // ITF_SUPPORT_EDITOR

        void                    setIsPressed(bbool isPressed)      { m_isPressed = isPressed; }
        bbool                   getIsPressed() const { return m_isPressed; }
        virtual     void        onPressed           () {}
        virtual     void        onReleased          () {}
        virtual     void        onRollover          () {}
        virtual     void        onRollout           () {}
        virtual     void        onAction            (const StringID & action) {}

        virtual     void        onResourceLoaded    ();

        static      Vec2d       getScreenRatio();
        // Template access
        bbool                   getIsDefaultSelected() const;

        ITF_INLINE void         setRuntimeDepthRank(u32 _val) { m_useRuntimeDepthRank = btrue; m_runtimeDepthRank = _val; }
        ITF_INLINE void         resetRuntimeDepthRank() { m_useRuntimeDepthRank = bfalse; m_runtimeDepthRank = 0; }
        i32                     getDepthRank() const;

        /// Warning: only works if a UITextBox Component is found
        virtual void            get2DBoundingBox(Vec2d& boxMinPosition, Vec2d&  boxMaxPosition);
		void					get2DBoundingBox(AABB& bb);

    protected:

        void                    updateRelativeAABB();
        void                    updateScreenOccupation();
        void                    updateActorPosition();
        void                    computeRelativeRef();

        const class UIComponent_Template* getTemplate() const;

        ObjectRef               m_UIref;

        // Percentage of the screen
        f32                     m_relativePosX; 
        f32                     m_relativePosY; 
        f32                     m_screenPourcentX;
        f32                     m_screenPourcentY;

        // Text params
        String                  m_content;
        UITextBox*              m_textBox;
        bbool                   m_isTextBoxActive;
        f32                     m_fontHeight;
        u32                     m_fontIndex;
        LocalisationId          m_lineId;
        LocalisationId          m_previousLineId;
        StringID                m_UIComponentID;
        StringID                m_menuID;
        StringID                m_menuSonID;

        String8                 m_friendly;
        String8                 m_menuFriendly;
        String8                 m_menuSonFriendly;
        String8                 m_locFileName;
        StringID                m_id;

        bbool                   m_bSelected;
		bbool					m_canBeSelected;
        bbool                   m_forceSelected;
        bbool                   m_isContentFill;
        bbool                   m_isActive;
        bbool                   m_textChanged;
        bbool                   m_bDefaultSelectedByInstance;

        ITF_VECTOR<TextInfo>    m_textTab;

        Vec2d                   m_animSizeRef = Vec2d::Zero;
        AnimLightComponent *    m_animLightComponent;

        enum UIAlign
        {
            align_free, 
            align_centerX,
            align_centerY,
            align_centerXY
        };
        u32                     m_align; 

        bbool                   m_useRuntimeDepthRank;
        bbool                   m_isPressed;
        i32                     m_runtimeDepthRank;
    };

    class UIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIComponent_Template, TemplateActorComponent,1440308778);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIComponent);

    public:

        UIComponent_Template();
        ~UIComponent_Template();

        const Path&             getFontName() const { return m_fontName; }
        f32                     getInitialFontHeight() const { return m_initialFontHeight; }
        f32                     getFontHeightSelected() const { return m_FontHeightSelected; }        
        const Color&            getTextColor() const { return m_textColor; }
        const Color&            getTextShadowColor() const { return m_textShadowColor; }
        const f32               getLineSpacingFactor() const { return m_lineSpacingFactor; }
        const Color&            getTextColorHighlighted() const { return m_textColorHighlighted; }
        const Color&            getTextColorInactive() const { return m_textColorInactive; }
        u32                     getTextMode() const { return m_textMode; }
        u32                     getTextModeY() const { return m_textModeY; }
        bbool                   getIsDefaultSelected() const { return m_bDefaultSelected; }
        bbool                   getIsActive() const { return m_isActive; }
        bbool                   getUpdatePos() const { return m_updatePos; }
        i32                     getRank() const { return m_rank; }
        const String8&          getFriendly() const { return m_friendly; }
        const String8&          getMenuBaseName() const { return m_menuBaseName; }
        const Vec2d&            getAnimSize() const { return m_animSize; }
        const Vec2d&            getShadowOffset() const { return m_textShadowOffset; }
        Color                   getActorColorInactive() const { return m_actorColorInactive; }
        Color                   getActorColorHiglighted() const { return m_actorColorHiglighted; }
        Color                   getActorColor() const { return m_actorColor; }
        bbool                   getForceUseAnimSize() const { return m_forceUseAnimSize; }
    private:

        Path                    m_fontName;
        f32                     m_initialFontHeight;
        f32                     m_FontHeightSelected;
        Color                   m_textColor;
        Color                   m_textShadowColor;
        Color                   m_textColorHighlighted;
        Color                   m_textColorInactive;
        Color                   m_actorColorInactive;
        Color                   m_actorColorHiglighted;
        Color                   m_actorColor;
        u32                     m_textMode;
        u32                     m_textModeY;
        bbool                   m_bDefaultSelected;
        bbool                   m_isActive;
        bbool                   m_updatePos;
        i32                     m_rank;

        String8                 m_friendly;
        String8                 m_menuBaseName;
        Vec2d                   m_animSize;
        bbool                   m_forceUseAnimSize;
        Vec2d                   m_textShadowOffset;
        f32                     m_lineSpacingFactor;
    };

    ITF_INLINE const UIComponent_Template* UIComponent::getTemplate() const
    {
        return static_cast<const UIComponent_Template*>(m_template);
    }

    ITF_INLINE bbool UIComponent::getIsDefaultSelected() const { return getTemplate()->getIsDefaultSelected() || m_bDefaultSelectedByInstance; }
}
#endif // _ITF_UICOMPONENT_H_
