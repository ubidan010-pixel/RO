#ifndef _ITF_UITEXTBOX_H_
#define _ITF_UITEXTBOX_H_


#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

namespace ITF
{
    class XMLFile;

    class UITextBox : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UITextBox,ActorComponent,3507273453);

    public:
        DECLARE_SERIALIZE()      

        enum BoxPosition
        {
            BoxPosition_TopLeft,
            BoxPosition_Center,
            BoxPosition_Left,
            BoxPosition_Count
        };

        UITextBox();
        ~UITextBox();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const ;
        virtual     bbool       needsDraw2D         (       ) const ;
        virtual     void        onActorClearComponents();
        virtual     void        onActorLoaded       ( Pickable::HotReloadType /*_hotReload*/ );
        virtual     void        Update              ( f32 _deltaTime );
        virtual     void        onBecomeInactive    (       );
        virtual     void        onBecomeActive      (       );
        void                    changePage();
        virtual     void        Draw                (       );
        virtual     void        Draw2D              (       );
        virtual     void        onEvent             ( Event * );

        void        setData(const Vec3d& _pos2d, f32 _highestSize, const TextInfo& _textInfo, f32 _width, f32 _height, i32 _depthRank, const Vec2d& _startUV, const Vec2d& _endUV, 
            u32 _colour = COLOR_WHITE, bbool _useBackground = bfalse, const BoxPosition& _boxPosition = BoxPosition_Center, const Vec2d& _shadowOffset = Vec2d::Zero, u32 _shadowColor = COLOR_BLACK, f32 _lineFactor = 1.f, const ResourceID& _backTexture = ResourceID::Invalid, 
                            u32 _fontIndex = U32_INVALID, u32 _mode = U32_INVALID, u32 _modeY = 0);
  
        void        setTextAt(const TextInfo& _textInfo, u32 _index );
        const u32   getPagesCount() const { return m_textTabWithVarTags.size(); }
        void        addTextFromLineId(const LocalisationId& _lineId, const Color& _color );
        void        setTextFromLineId(const LocalisationId& _lineId, const Color& _color );
        void        addTextFromStr( const TextInfo& _textInfo );
        void        clearText();

        f32         getScreenRatio();

        ITF_INLINE void setBoxDimension(const Vec2d & _dimension);
        ITF_INLINE f32  getBoxWidth() const { return m_boxWidth; }
        ITF_INLINE void setBoxWidth( f32 _width ) { m_boxWidth = _width; }
        ITF_INLINE f32  getBoxHeight() const { return m_boxHeight; }
        ITF_INLINE void setBoxHeight( f32 _height ) { m_boxHeight = _height; }

        ITF_INLINE void setNoScaleWithActor(bbool _val) { m_noScaleWithActor = _val; }

        ITF_INLINE void setTextScale(const Vec2d & _scale) 
        { 
            m_textScale.m_x = _scale.m_x;
            m_textScale.m_y = _scale.m_y;
        }
        ITF_INLINE void setTextRotation(const f32 _rotation)
        {
            m_textRotation = _rotation;
        }
        ITF_INLINE void setTextAlpha(const f32 _alpha)
        {
            m_textAlpha = _alpha;
        }
  
        ITF_INLINE void setNeedToComputePosition(bbool needToCompute) { m_needToComptutePosition = needToCompute; }

        void show();
        void hide();

        bbool isDialogPlaying() const;
        bbool isTextUpdated() const { return m_textChanged == bfalse && m_actorsReadyToDraw == btrue; }

#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const ;
        virtual void    onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        void scaleBox(bbool _only2d = bfalse);
        void setDepthRank( i32 _rank ) { m_depthRank = _rank; }

        void setLaunchAudio() { m_launchAudio = btrue; }

        static u32 calculateNumOfCharactersFromLoc(const LocalisationId& _locId );
        static u32 calculateNumOfCharactersFromStr( const String& _str );

    private:

        struct ActorInfo
        {
            ActorRef                m_ref;
            String8                 m_pathId;
        };

         ITF_INLINE const class UITextBox_Template*  getTemplate() const;
         ///////////////////////////////////////////////////////////////////////////////////////////
         /// Get the text from UI_TEXTMANAGER with the friendlies wished
         void            fillTextTab             (      );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Update the value of the variables in the text
        void            updateVariables (       );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Parse the text and replace the [var] tag by the value of the variable
        String          parseVariableTag   (const String &_text);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Parse the text and find actors that need to be inserted
        void            parseActorTag( TextInfo& _textInfo, const String& _text, bbool _recurse );

        void            updateAABB();

        void            drawFontAtPos(const Vec3d& _pos);

        void            updateActors();
        void            updateReadyToDraw();

        void            onLanguageChanged(ITF_LANGUAGE _oldLanguage, ITF_LANGUAGE _newLanguage);

        void            showActors( bbool _show );

        void            launchAudio();
        void            stopAudio();

        ///////////////////////////////////////////////////////////////////////////////////////////
        //draw box methods
#ifdef ITF_SUPPORT_EDITOR
        void            drawBox(const f32 _border = 1.0f) const;
        void            drawBox2D(const Vec2d & _minBound, const Vec2d & _maxBound, const f32 _border) const;
        void            drawBox3D(const Vec2d & _minBound, const Vec2d & _maxBound, const f32 _border) const;
#endif //ITF_SUPPORT_EDITOR
        void            getBoxBounds2D(const Vec3d & _refPosition, Vec2d & _minBound, Vec2d & _maxBound) const;
        void            getBoxBounds3D(const Vec3d & _refPosition, Vec2d & _minBound, Vec2d & _maxBound) const;

        // Allow to keep the text in the box
        enum CroppingMode
        {
            Cropping_Scale,
            Cropping_Cut,
            Cropping_Count,
            ENUM_FORCE_SIZE_32(0)
        };

        struct BlockData
        {
            BlockData() 
                : m_index(U32_INVALID)
            {

            }

            StringID    m_friendly;
            u32         m_index;
        };

        ObjectRef               m_UIref;

        u32                     m_fontIndex;

        f32                     m_boxWidth;
        f32                     m_boxHeight;
        f32                     m_highestSize;

        ITF_VECTOR<BlockData>   m_blockData;

        ITF_VECTOR<TextInfo>    m_textTab; // Text without tags
        ITF_VECTOR<TextInfo>    m_textTabWithVarTags; // Var tags haven't been parsed (only this one)
        SafeArray <StringID>    m_playingSounds;

        u32                     m_currentPage;
        bbool                   m_display;
        i32                     m_depthRank;
        
        ResourceID              m_backTexture;
        GFX_3DQUAD              m_backTextureQuad;
        Vec2d                   m_startUV, m_endUV; 
        Vec2d                   m_textureOffset2D;  
        Vec2d                   m_anchorOffset2D;     
        u32                     m_backgroundColour;
        u32                     m_mode;
        u32                     m_modeY;
        f32                     m_textBoxScale; 
        bbool                   m_useBackground;

        BoxPosition             m_boxPosition;
        
        Vec2d                   m_shadowOffset;
        u32                     m_shadowColor;

        f32                     m_lineSpacingFactor;

        ObjectRef               m_sender;
        bbool                   m_needToComptutePosition;
        bbool                   m_textChanged;
        bbool                   m_stopAudio;
        bbool                   m_launchAudio;
        bbool                   m_restart;
        bbool                   m_noScaleWithActor;

        ITF_VECTOR <ActorInfo>          m_spawnedActors;
        bbool                           m_actorsReadyToDraw;

        Vec3d                   m_textScale;
        f32                     m_textRotation;
        f32                     m_textAlpha;
    };

    //---------------------------------------------------------------------------------------------------

    class UITextBox_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UITextBox_Template,TemplateActorComponent,2138714152);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UITextBox);

    public:

        // Allow to keep the text in the box
        enum CroppingMode
        {
            Cropping_Scale,
            Cropping_Cut,
            Cropping_Count
        };      

        enum BoxPosition
        {
            BoxPosition_TopLeft,
            BoxPosition_Center,
            BoxPosition_Left,
            BoxPosition_Count
        };

        UITextBox_Template();
        ~UITextBox_Template() {}

        ITF_INLINE const Path&                  getFontName() const { return m_fontName; }
        ITF_INLINE u32                          getTextMode() const { return m_textMode; }
        ITF_INLINE u32                          getTextModeY() const { return m_textModeY; }
        ITF_INLINE f32                          getTextHeight() const { return m_textHeight; }
        ITF_INLINE f32                          getInitialBoxWidth() const { return m_initialBoxWidth; }
        ITF_INLINE f32                          getInitialBoxHeight() const { return m_initialBoxHeight; }
        ITF_INLINE bbool                        getIsDrawBox() const { return m_isDrawBox; }
        ITF_INLINE CroppingMode                 getCroppingMode() const { return m_croppingMode; }
        ITF_INLINE bbool                        getUsePages() const { return m_usePages; }
        ITF_INLINE BoxPosition                  getBoxPosition() const { return m_boxPosition; }
        ITF_INLINE const SafeArray<StringID>&   getBlocksFriendly() const { return m_blocksFriendly; }
        ITF_INLINE const Path&                  getPath() const { return m_textureFile; }
        ITF_INLINE const Vec2d&                 getInitTextureOffset2D() const { return m_initTextureOffset2D; }
        ITF_INLINE const Vec2d&                 getInitAnchorOffset2D() const { return m_initAnchorOffset2D; }
        ITF_INLINE f32                          getZOffset() const { return m_zOffset; }
        ITF_INLINE bbool                        getIs2DText() const { return m_is2DText; }
        ITF_INLINE f32                          getActorsZOffset() const { return m_actorsZOffset; }
        ITF_INLINE bbool                        getUseActorPosition() const { return m_useActorPosition; }
        ITF_INLINE bbool                        getUseActorScale() const { return m_useActorScale; }
        ITF_INLINE bbool                        getUseActorRotation() const { return m_useActorRotation; }

        ITF_INLINE const Vec2d&                 getShadowOffset() const { return m_textShadowOffset; }
        ITF_INLINE const Color&                 getTextShadowColor() const { return m_textShadowColor; }


        ITF_INLINE const f32                    getlineSpacingFactor() const { return m_lineSpacingFactor; }

    private:

        Path                    m_fontName;
        u32                     m_textMode;
        u32                     m_textModeY;
        f32                     m_textHeight;
        f32                     m_initialBoxWidth;
        f32                     m_initialBoxHeight;
        bbool                   m_isDrawBox;
        CroppingMode            m_croppingMode;
        bbool                   m_usePages;
        BoxPosition             m_boxPosition;
        SafeArray<StringID>     m_blocksFriendly;
        Path                    m_textureFile;
        Vec2d                   m_initTextureOffset2D;  
        Vec2d                   m_initAnchorOffset2D;
        f32                     m_zOffset;
        f32                     m_actorsZOffset;
        bbool                   m_is2DText; // A screen pourcent is apply in this case
        bbool                   m_useActorPosition;
        bbool                   m_useActorScale;
        bbool                   m_useActorRotation;
        Color                   m_textShadowColor;
        Vec2d                   m_textShadowOffset;
        f32                     m_lineSpacingFactor;
    };


    ITF_INLINE const UITextBox_Template*  UITextBox::getTemplate() const {return static_cast<const UITextBox_Template*>(m_template);}

    ITF_INLINE void UITextBox::setBoxDimension(const Vec2d & _dimension)
    {
        m_boxWidth = _dimension.m_x;
        m_boxHeight = _dimension.m_y;
    }
}
#endif // _ITF_UITEXTBOX_H_
