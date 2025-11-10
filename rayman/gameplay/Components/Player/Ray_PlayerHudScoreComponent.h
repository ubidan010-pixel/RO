#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#define _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF
{
    class Ray_Player;
    class UITextBox;

    class Ray_PlayerHudScoreComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerHudScoreComponent, GraphicComponent,2384152513)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_PlayerHudScoreComponent();
        virtual ~Ray_PlayerHudScoreComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onActorClearComponents();
        virtual bbool               needsUpdate()  const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return btrue; }

        virtual void                Update( f32 _dt );
        virtual void                Draw2D();
        virtual void                onEvent( Event* _event );
        void                        setVisualOrder(u32 _order);
        u32                         getVisualOrder() const {return m_visualOrder;}


        static void                 setForcedDisplay(u32 _playerIndex);
        static void                 setAllHUDsAlwaysVisible(bbool _alwaysVisible) {m_alwaysVisible = _alwaysVisible;}
        static bbool                areAllHUDsAlwaysVisible() {return m_alwaysVisible;}
        static void                 setIsLumCaptionEnabled(bbool _visibility) {m_lumCaptionEnabled = _visibility;}
        static void                 setVisibility(bbool _visibility) {m_isDrawEnabled = _visibility;}
        
        static void                 flushPendingScoreIncrements(u32 _playerIndex);
        static void                 addPendingScoreIncrement(u32 _playerIndex, u32 _increment);
        static Vec2d                getScorePosFromPlayerIndex( u32 _playerIndex );

        static void                 clearAllLumScore();

        static ITF_INLINE Ray_PlayerHudScoreComponent *getInstance(u32 _index) {return m_instances[_index];}

        // Custom messages need this resource, because it's the reference
        AtlasDynamicObject*         getCharacterAtlas() { return &m_characterAtlas; }

		//Tarik::Fresco
		static Vec2d                getRelicPos();
		static bbool                getIsRelicCaptionEnabled() {return m_relicCaptionEnabled;}
		static void                 setIsRelicCaptionEnabled(bbool _visibility,bbool _isNewCaption=false,u32 _frescoId=0) {m_relicCaptionEnabled = _visibility; m_isNewCaption = _isNewCaption; m_frescoId = _frescoId;}
		
		static const u32 maxPendingScoreIncrements = 1;
    private:        
        enum CaptionType_t {Caption_Lum, Caption_Join};
        enum PulseState_t {Pulse_None=0, Pulse_Increase, Pulse_Sustain, Pulse_Decrease, Pulse_DoublePeak, Pulse_Wait};

        const class Ray_PlayerHudScoreComponent_Template* getTemplate() const;

        void initPressStartText();

        void updateHpBar(f32 _dt, Ray_Player* _player);
        void updateLumCount(f32 _dt);
        void clearSharedFlag(u32 &_mask);
        bbool hasSharedFlag(u32 _mask);
        void setPulseState(PulseState_t _state);
        void updatePulse(f32 _dt);
        static u32 processAddPendingLum(u32 _activeIndex);
        static void maySendScoreMultiplierParticle(u32 _playerIndex, u32 _increment);
        ITF_INLINE Vec2d getDefaultScale();
        f32 getPressStartAlpha();
        bbool isWorldMap();
        bbool canShowLumCaption();

		void initRelicText();//Tarik
		bbool canShowRelicCaption();

        static FixedArray<u32,maxPendingScoreIncrements> m_pendingScoreIncrements[s_maxPlayers];


        u32 m_scoreFontIndex, m_pressStartFontIndex;    

        ITF_VECTOR<TextInfo> m_lumCountTextTab;
        ITF_VECTOR<TextInfo> m_pressStartTextTab;
		ITF_VECTOR<TextInfo> m_relicTextTab;

        void buildAnimList();

        u32 m_playerIndex, m_visualOrder;

        f32 m_currentMaxHpFrame;
        f32 m_currentHpFrame;
        f32 m_growSpeed;

        bbool m_lastActive;
        u32 m_lastMaxHP;
        u32 m_lastHP;
        f32 m_lastLumCount;
        f32 m_stateChangeTimer;     // monitors HP or lum count changes
        PulseState_t m_pulseState;
        f32 m_pulseTimer;
        f32 m_pressStartPulseTimer;
        u32 m_pulseCount;
        f32 m_scoreSizeMultiplier;

        f32 m_timeFromPulseStart;
        CaptionType_t m_captionType;

        AtlasDynamicObject m_characterAtlas;
        AtlasDynamicObject m_lumAtlas;
        f32 m_currentLumScale, m_currentAlpha;


        static u32 m_forcedDisplayMask;
        static u32 m_reallyPulsingType[s_maxPlayers];
        static Ray_PlayerHudScoreComponent *m_instances[s_maxPlayers];
        static bbool m_alwaysVisible;
        static bbool m_isDrawEnabled;
        static bbool m_lumCaptionEnabled;

		static bbool m_relicCaptionEnabled;
		static bbool m_isNewCaption;
		static u32 m_frescoId;

		//Tarik::Fresco
		AtlasDynamicObject m_relic1Atlas,m_relic2Atlas;
    };

    //-------------------------------------------------------------------------------------
    class Ray_PlayerHudScoreComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerHudScoreComponent_Template,GraphicComponent_Template,4042481333);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlayerHudScoreComponent);

    public:

        Ray_PlayerHudScoreComponent_Template();
        ~Ray_PlayerHudScoreComponent_Template() {}

        f32                     getRelativeWidth() const { return m_relativeWidth; }
        f32                     getRelativeSpacing() const { return m_relativeSpacing;}
        const Margin&           getRelativeAdditionalMargin() const { return m_relativeAdditionalMargin; }
        const Margin&           getPressToJoinRelativeAdditionalMargin() const { return m_pressToJoinRelativeAdditionalMargin; }
        f32                     getHpChangeFadeInDuration() const { return m_hpChangeFadeInDuration; }
        f32                     getHpChangeVisibleDuration() const { return m_hpChangeVisibleDuration; }
        f32                     getHpChangeFadeOutDuration() const { return m_hpChangeFadeOutDuration; }
        const Path&             getFontName() const { return m_fontName; }
        const Vec2d&            getScorePos() const { return m_scorePos; }
        const Vec2d&            getScoreSize() const { return m_scoreSize; }
        const Color&            getScoreColor() const { return m_scoreColor; }
        u32                     getLumBigIncreaseSpeed() const { return m_lumBigIncreaseSpeed; }
        u32                     getLumProgressiveBigIncreaseThreshold() const { return m_lumProgressiveBigIncreaseThreshold; }
        u32                     getLumSmallIncreaseSpeed() const { return m_lumSmallIncreaseSpeed; }
        u32                     getLumProgressiveSmallIncreaseThreshold() const { return m_lumProgressiveSmallIncreaseThreshold; }
        f32                     getPulseSustainDuration() const {return m_pulseSustainDuration;}
        f32                     getPulseScale() const {return m_pulseScale;}
        f32                     getPulseDoublePeakMinScale() const {return m_pulseDoublePeakMinScale;}
        f32                     getPulseIncreaseDuration() const {return m_pulseIncreaseDuration;}
        f32                     getPulseDecreaseDuration() const {return m_pulseDecreaseDuration;}
        f32                     getPulseDoublePeakDuration() const {return m_pulseDoublePeakDuration;}
        f32                     getPulseCycleDuration_SingleLum() const {return m_pulseCycleDuration_SingleLum;}
        f32                     getPulseCycleDuration_MultipleLums() const {return m_pulseCycleDuration_MultipleLums;}
        u32                     getScoreMultiplierValue() const {return m_scoreMultiplierValue;}
        const StringID&         getScoreMultiplierParticleID() const {return m_scoreMultiplierParticleID;}
        f32                     getActualWidthPercent() const {return m_actualWidthPercent;}
        f32                     getLumNormalScale() const {return m_lumNormalScale;}
        f32                     getPressStartHeight() const {return m_pressStartHeight;}
        Vec2d                   getPressStartPos() const {return m_pressStartPos;}
        const Path&             getPressStartFontName() const {return m_pressStartFontName;}
        f32                     getGrayedAlpha() const {return m_grayedAlpha;}
        f32                     getIdleAlpha() const {return m_idleAlpha;}
        const Path&             getLumTexturePath() const {return m_lumTexturePath;}
        const Path&             getCharacterTexturePath() const {return m_characterTexturePath;}

        Vec2d                   getScorePosFromVisualOrder( u32 _visualOrder, const Margin& _margin, const bbool _useWidth, Vec2d &_size ) const;

        f32                     getPressStartPulsePeriod() const {return m_pressStartPulsePeriod;}
        f32                     getPressStartFadeInDuration() const {return m_pressStartFadeInDuration;}
        f32                     getPressStartFadeOutDuration() const {return m_pressStartFadeOutDuration;}
        f32                     getPressStartMinAlpha() const {return m_pressStartMinAlpha;}
        f32                     getPressStartMaxAlpha() const {return m_pressStartMaxAlpha;}
        const LocalisationId&   getPressStartTextID() const {return m_pressStartTextID;}
        const Vec2d&            getLumBoxLocalPosition() const {return m_lumBoxLocalPosition;}
        const Vec2d&            getLumBoxLocalSize() const {return m_lumBoxLocalSize;}
        const Vec2d&            getHeadBoxLocalSize() const {return m_headBoxLocalSize;}
		
		//Tarik::Fresco
		const Vec2d&            getRelicPos() const { return m_relicPos; }
		const Vec2d&            getRelicTextPos() const { return m_relicTextPos; }
		const Vec2d&            getRelicSize() const { return m_relicSize; }
		const Path&             getRelic1TexturePath() const {return m_relic1TexturePath;}
		const Path&             getRelic2TexturePath() const {return m_relic2TexturePath;}
		const Vec2d&            getRelicBoxLocalPosition() const {return m_relicBoxLocalPosition;}
		const Vec2d&            getRelicBoxLocalSize() const {return m_relicBoxLocalSize;}
		const LocalisationId&   getNewRelicTextID() const {return m_newRelicTextID;}
		const LocalisationId&   getDuplicateRelicTextID() const {return m_duplicateRelicTextID;}
		const Color&            getRelicTextColor() const { return m_relicTextColor; }
		const Vec2d&            getRelicTextShadowOffset() const { return m_relicTextShadowOffset; }
		const Color&            getRelicTextShadowColor() const { return m_relicTextShadowColor; }


    private:

        LocalisationId m_pressStartTextID;
        u32 m_scoreMultiplierValue;
        StringID m_scoreMultiplierParticleID;
        f32 m_relativeWidth;            // % of screen width
        f32 m_relativeSpacing;
        f32 m_actualWidthPercent;
        Margin m_relativeAdditionalMargin;
        Margin m_pressToJoinRelativeAdditionalMargin;
        f32 m_hpChangeFadeInDuration;   // s
        f32 m_hpChangeVisibleDuration;
        f32 m_hpChangeFadeOutDuration;
        Path m_fontName;
        Path m_pressStartFontName;
        Vec2d m_scorePos;               // %s of actor AABB
        Vec2d m_scoreSize;              // %s of actor AABB
        Color m_scoreColor;
        f32 m_pressStartHeight;
        Vec2d m_pressStartPos;
        f32 m_pulseSustainDuration;
        f32 m_lumNormalScale;
        f32 m_pulseScale;
        f32 m_pulseDoublePeakMinScale;
        u32 m_lumBigIncreaseSpeed, m_lumProgressiveBigIncreaseThreshold;
        u32 m_lumSmallIncreaseSpeed, m_lumProgressiveSmallIncreaseThreshold;
        f32 m_pulseIncreaseDuration, m_pulseDecreaseDuration, m_pulseDoublePeakDuration;
        f32 m_pulseCycleDuration_SingleLum, m_pulseCycleDuration_MultipleLums;
        f32 m_grayedAlpha;
        f32 m_idleAlpha;
        f32 m_pressStartPulsePeriod, m_pressStartFadeInDuration, m_pressStartFadeOutDuration, m_pressStartMinAlpha, m_pressStartMaxAlpha;
        Path m_lumTexturePath,m_characterTexturePath;
        Vec2d m_lumBoxLocalPosition, m_lumBoxLocalSize;
        Vec2d m_headBoxLocalSize;

		//Tarik::Fresco
        f32 m_relicActualWidthPercent;
        Vec2d m_relicPos;               // %s of actor AABB
        Vec2d m_relicTextPos;               // %s of actor AABB
        Vec2d m_relicSize;              // %s of actor AABB
		Path m_relic1TexturePath;
		Path m_relic2TexturePath;
		Vec2d m_relicBoxLocalPosition, m_relicBoxLocalSize;
		LocalisationId m_newRelicTextID,m_duplicateRelicTextID;
		Color m_relicTextColor;
		Color m_relicTextShadowColor;
		Vec2d m_relicTextShadowOffset;
	};

    ITF_INLINE const Ray_PlayerHudScoreComponent_Template* Ray_PlayerHudScoreComponent::getTemplate() const
    {
        return static_cast<const Ray_PlayerHudScoreComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
