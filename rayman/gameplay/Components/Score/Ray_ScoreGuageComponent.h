#ifndef _ITF_RAY_SCOREGUAGECOMPONENT_H_
#define _ITF_RAY_SCOREGUAGECOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF
{
    class Ray_ScoreGuageComponent : public ActorComponent, public IInputListener
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ScoreGuageComponent, ActorComponent,635084377)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

		struct ScorePlayer
		{
			ScorePlayer() {}
			~ScorePlayer() {}

			ActorRef m_scoreRef;
		};


		struct ParticleLumsData
		{
			ParticleLumsData()
			: m_pos(Vec2d::Zero)
			, m_speed(Vec2d::Zero)
			, m_curIndexAtlas(0)
			, m_randomOffsetSpeed(1.0f)
			, m_angle(0.0f)
			{}

			~ParticleLumsData() {}

			Vec2d m_pos;
			Vec2d m_speed;
			f32   m_randomOffsetSpeed;
			u32   m_curIndexAtlas;
			f32	  m_angle;
		};


        Ray_ScoreGuageComponent();
        virtual ~Ray_ScoreGuageComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const   { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
		virtual void		onResourceReady();

        virtual void        Update( f32 _dt );
		virtual void		Draw();
        virtual void        onEvent(Event * _event);

				void		updateAnimInputs();

#ifdef ITF_SUPPORT_EDITOR
		virtual void    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:
        virtual void Receive( u32 deviceID /* player */, f32 axis, const StringID& action ) ;

    private:
        ITF_INLINE const class Ray_ScoreGuageComponent_Template* getTemplate() const;

        void activateMedal(u32 _stage);
        void init();
		void initRef();
        void sortPlayerByLumScoreForCinematic();
        void startArrival();
		void updateParticles( f32 _dt );

		void initAtlasLums();
		const Transform2d getTransform2d();
		void playAnimOtherAct( ActorRef _actorRef, const StringID _anim );
		void playAnim( const StringID _anim );
		void startFiesta();
		void pauseLumsReleasing();
		void resumeLumsReleasing();
		void stopAllSound();

		void startRecap();
		void updateReward();
		void updateAlphaBones(f32 _dt);
		void updateState( f32 _dt );
		void changeState( u32 _newState );
		void setAlphaBone(u32 _indexBone, f32 _alpha);
		void playFxFlash(u32 _indexBone);

        class AnimatedComponent* m_animComponent;
        class SoundComponent* m_soundComponent;
        class UITextBoxesComponent * m_uiTextBoxesComponent;
		class LinkComponent* m_linkComponent;
		class FXControllerComponent* m_fxControllerComponent;

		u32 m_state;

        bbool m_firstUpdate;
        bbool m_arrivalInProgress;
        u32 m_lumCount;
        u32 m_maxLumCount;
        u32 m_lumAdd;
        u32 m_thresholdB;
        u32 m_thresholdC;
        u32 m_thresholdD;

		u32 m_totalLumsWin;

		f32 m_lumCursorSmooth;
        f32 m_lumCursor;
        f32 m_thresholdBCursor;
        f32 m_thresholdCCursor;
        f32 m_thresholdDCursor;


		ITF_VECTOR<ScorePlayer> m_scores;

        u32 m_scoresFinished;

        f32 m_waitingTime;

        void clearListener();

		bbool m_scoreStarted;
        bbool m_listenerRegistered;
        bbool m_menuValidPressed;
        bbool m_menuBackPressed;
		bbool m_scorePlayersDiplayed;
		bbool m_actionButtonAlreadyPressed;

		ActorRef m_cineRef;
		ActorRef m_trophyRef;
		ActorRefList m_fiestaRef;
		ActorRefList m_discoRef;

		bbool    m_initRef;

		// Counters (bones)
		bbool m_fadingCounter_A;
		f32   m_timeFadingCounter_A;
		f32	  m_alphaCounter_A;
		u32 m_indexboneCounterA_1;
		u32 m_indexboneCounterA_2;
		u32 m_indexboneCounterA_3;

		bbool m_fadingCounter_B;
		f32   m_timeFadingCounter_B;
		f32	  m_alphaCounter_B;
		u32 m_indexboneCounterB_1;
		u32 m_indexboneCounterB_2;
		u32 m_indexboneCounterB_3;

		bbool m_fadingCounter_C;
		f32   m_timeFadingCounter_C;
		f32	  m_alphaCounter_C;
		u32 m_indexboneCounterC_1;
		u32 m_indexboneCounterC_2;
		u32 m_indexboneCounterC_3;

		u32 m_indexboneGlassCut_A;
		u32 m_indexboneGlassCut_B;
		u32 m_indexboneGlassCut_C;

		// Reward
		u32 m_rewardIndex;
		bbool m_rewardPlaying;
		bbool m_mrDarkIsDead;

        // Atlas Lums
		ITF_VECTOR<ParticleLumsData> m_listLums;
		
        u32     m_trame;
        u32     m_countAtlasUv;
        AtlasDynamicObject   m_atlas;
        SafeArray<Vec2d> m_listUV;

		Vec2d m_tubeP1;
		Vec2d m_tubeP2;
		Vec2d m_tubeP3;
		Vec2d m_tubeP4;

		u32 m_indexBone_Lums;


		// Sound
		ITF::SoundHandle m_soundHandle;
        ITF::SoundHandle  m_handleSoundWait;
        ITF::SoundHandle  m_handleSoundMusic1;
        ITF::SoundHandle  m_handleSoundMusic2;
        ITF::SoundHandle  m_handleSoundMusic3;
        ITF::SoundHandle  m_handleSoundDisco;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_ScoreGuageComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ScoreGuageComponent_Template, TemplateActorComponent,579944772);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ScoreGuageComponent);

    public:

        Ray_ScoreGuageComponent_Template();
        virtual ~Ray_ScoreGuageComponent_Template();

		ITF_INLINE const Path& getTextureLums() const { return m_textureLums; }

		ITF_INLINE u32 getCountLums() const { return m_countLums; }
		ITF_INLINE f32 getGaugeLumsLimits_X_Min() const { return m_gaugeLumsLimits_X_Min; }
		ITF_INLINE f32 getGaugeLumsLimits_X_Max() const { return m_gaugeLumsLimits_X_Max; }
		ITF_INLINE f32 getGaugeLumsLimits_Y_Min() const { return m_gaugeLumsLimits_Y_Min; }
		ITF_INLINE f32 getGaugeLumsLimits_Y_Max() const { return m_gaugeLumsLimits_Y_Max; }
		ITF_INLINE f32 getSizeLums() const { return m_sizeLums; }
		ITF_INLINE f32 getMaxSpeedLums() const { return m_maxSpeedLums; }
		ITF_INLINE f32 getGaugeOffsetY() const { return m_gaugeOffsetY; }

        ITF_INLINE ITF::StringID getArrivalAnim() const { return m_arrivalAnim; }
        ITF_INLINE ITF::StringID getIdleAnim() const { return m_idleAnim; }
        ITF_INLINE f32 getGuageSmooth() const { return m_guageSmooth; }
        ITF_INLINE f32 getWaitingTime() const { return m_waitingTime; }

		ITF_INLINE const StringID& getCheckMapForMrDark() const { return m_checkMapForMrDark;}

	private:

		Path m_textureLums;

		u32 m_countLums;
		f32 m_gaugeLumsLimits_X_Min;
		f32 m_gaugeLumsLimits_X_Max;
		f32 m_gaugeLumsLimits_Y_Min;
		f32 m_gaugeLumsLimits_Y_Max;
		f32 m_sizeLums;
		f32 m_maxSpeedLums;
		f32 m_gaugeOffsetY;

        StringID	m_arrivalAnim;
        StringID	m_idleAnim;
        f32			m_guageSmooth;
        f32			m_waitingTime;

		StringID m_checkMapForMrDark;
    };

    //*****************************************************************************

    ITF_INLINE const Ray_ScoreGuageComponent_Template* Ray_ScoreGuageComponent::getTemplate() const
    {
        return static_cast<const Ray_ScoreGuageComponent_Template*>(m_template);
    }

}; // end namespace

#endif //_ITF_RAY_SCOREGUAGECOMPONENT_H_
