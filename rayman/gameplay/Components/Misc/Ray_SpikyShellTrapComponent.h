#ifndef _ITF_RAY_SPIKYSHELLTRAPCOMPONENT_H_
#define _ITF_RAY_SPIKYSHELLTRAPCOMPONENT_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif

namespace ITF
{

    class Ray_SpikyShellTrapComponent_Template;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// This class defines a single spike. Contained in the component class defined below
    class Ray_SpikyShellElement
    {
    public:
        enum State_t
        {
            State_Idle,
            State_Shaking,
            State_Spike,
        };

        static const u32 m_vertexCount = 4*3;
        static const u32 m_indexCount = 6*3;

        Ray_SpikyShellElement();
        ~Ray_SpikyShellElement();

        void setup(
            Actor *_act,
            const Ray_SpikyShellTrapComponent_Template *_template,
            u32 _backBaseTexIndex,u32 _frontBaseTexIndex,u32 _spikeTexIndex,
            const Vec3d &_pos, const Vec3d &_normal, 
            const Vec3d &_baseWidthVect, const Vec3d &_baseHeightVect, 
            const Vec3d &_spikeWidthVec, 
            const Vec3d &_spikeOffsetVec,
            f32 _restHeightPercent, f32 _maxHeight);
        void setup_dontChangeState(
            Actor *_act,
            const Ray_SpikyShellTrapComponent_Template *_template,
            const Vec3d &_pos, const Vec3d &_normal, 
            const Vec3d &_baseWidthVect, const Vec3d &_baseHeightVect, 
            const Vec3d &_spikeWidthVec, 
            const Vec3d &_spikeOffsetVec,
            f32 _restHeightPercent, f32 _maxHeight);

        void resetState() {m_state = State_Idle; m_timeInCurrentState = 0; m_currentHeight = m_restHeightPercent;}
        void setState(State_t _state);
        void signalNearestEnemy(f32 _distance);
        void fillBuffers(Texture *_tex, VertexPCT *&_destVertex, VertexPCT *_endVertex, u16 *&_destIndex, u16 *_endIndex, u16 &_indexOffset);
        void updateCurrentState(f32 _dt, const ActorList &_enemies, ActorList &_hitEnemies);
        void getSpikeGeometry(f32 _currentHeightPercent, Vec3d &_spikeLowLeftCorner, Vec3d &_heightVec);
        void addVictimsToList(const ActorList &_enemies, ActorList &_hitEnemies);
        bbool computeCollisionShape(PhysShapePolygon &_poly);
#ifdef ITF_SUPPORT_EDITOR
        void drawDebugInfo();
#endif // ITF_SUPPORT_EDITOR
        void playSound(const StringID &_id);

        Actor *m_actor;
        Vec3d m_pos;
        Vec3d m_baseWidthVect;
        Vec3d m_baseHeightVect;
        Vec3d m_normal;

        //spikes
        f32 m_maxHeight;
        f32 m_restHeightPercent;
		Vec3d m_spikeWidthVec;
        Vec3d m_spikeOffsetVec;

        //subtexture indices
        u32 m_backBaseTexIndex, m_frontBaseTexIndex, m_spikeTexIndex ;
        f32 m_spikeAnimationFrameTimeLeft;
        f32 m_baseAnimationTimeLeft;

        //
        f32 m_currentHeight; //in 0..1, mostly in m_restHeightPercent..1
        State_t m_state;
        f32 m_timeInCurrentState;
        f32 m_speed;
        f32 m_nearestDistance;

        // sound
        u32 m_soundFx;

        const Ray_SpikyShellTrapComponent_Template *m_template;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// The component itself
    class Ray_SpikyShellTrapComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpikyShellTrapComponent, ActorComponent,2579463222)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        Ray_SpikyShellTrapComponent();
        virtual ~Ray_SpikyShellTrapComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
		virtual void        onEvent( Event* _event ) {}
        virtual void        Update( f32 _dt );
        virtual void        Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_SpikyShellTrapComponent_Template * getTemplate() const;

        void computeGround(Vec3d &_p1, Vec3d &_p2, Vec3d &_normal);
        void populate(bbool _initialize = btrue);
        void mayCreateVertexIndexBuffers();
        void destroyIndexVertexBuffers();
        void updateAABB();
        void sendStim(Actor *act, const Vec3d &_normal);
#ifndef ITF_FINAL
        void drawDebug();
#endif // ITF_FINAL

        ITF_VECTOR<Ray_SpikyShellElement> m_elements;
        ResourceID m_texture;
        MarsagliaSeeder m_seeder;
        ITF_Mesh m_mesh;
        Vec3d m_spikeOffset;
        Vec3d m_lastPos;
        u32 m_currentIndexCount, m_currentVertexCount;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_SpikyShellTrapComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpikyShellTrapComponent_Template, TemplateActorComponent,3861753903);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SpikyShellTrapComponent);

    public:
        Ray_SpikyShellTrapComponent_Template();

        const Path& getTexturePath() const {return m_texturePath;}
        f32 getBaseWidth() const {return m_baseWidth;}
        f32 getBaseHeight() const {return m_baseHeight;}
        f32 getMinSpacing() const {return m_minSpacing;}
        f32 getMaxSpacing() const {return m_maxSpacing;}
        f32 getRestHeightPercent() const {return m_restHeightPercent;}
        f32 getMaxHeight() const {return m_maxHeight;}
		
        u32 getFrontBaseTexIndex_Idle() const {return m_frontBaseTexIndex_Idle;}
        u32 getFrontBaseFirstTexIndex_Shaking() const {return m_frontBaseFirstTexIndex_Shaking;}
        u32 getFrontBaseLastTexIndex_Shaking() const {return m_frontBaseLastTexIndex_Shaking;}
        u32 getBackBaseTexIndex_Idle() const {return m_backBaseTexIndex_Idle;}
        u32 getBackBaseFirstTexIndex_Shaking() const {return m_backBaseFirstTexIndex_Shaking;}

		u32 getSpikeFirstTexIndex() const {return m_spikeFirstTexIndex;}
		u32 getSpikeLastTexIndex() const {return m_spikeLastTexIndex;}
		f32 getSpikeWidth() const {return m_spikeWidth;}
        u32 getFaction() const {return m_faction;}
        f32 getShakeDetectionRadius() const {return m_shakeDetectionRadius;}
        f32 getSpikeDetectionRadius() const {return m_spikeDetectionRadius;}
        f32 getSpikeDetectionRadius_Emergency() const {return m_spikeDetectionRadius_Emergency;}
        f32 getEndOfSpikeDetectionRadius() const {return m_endOfSpikeDetectionRadius;}
        f32 getMinAlertDuration() const {return m_minAlertDuration;}
        f32 getMinSpikeDuration() const {return m_minSpikeDuration;}
        f32 getSpikeBounciness() const {return m_spikeBounciness;}
        f32 getShakeAmplitude() const {return m_shakeAmplitude;}
        f32 getShakePulsation() const {return m_shakePulsation;}
        f32 getSpikeAmplitude() const {return m_spikeAmplitude;}
        f32 getSpikeYOffset() const {return m_spikeYOffset;}
        f32 getHitMarginPercent() const {return m_hitMarginPercent;}
        u32 getHitLevel() const {return m_hitLevel;}
        f32 getSpikeAnimationFrameDuration() const {return m_spikeAnimationFrameDuration;}
        f32 getSpikeAnimationFirstFrameDuration() const {return m_spikeAnimationFirstFrameDuration;}
        f32 getBaseAnimationFrameDuration() const {return m_baseAnimationFrameDuration;}
        f32 getRisenHeightPercent() const {return m_risenHeightPercent;}
        f32 getMinScaleFactor() const {return m_minScaleFactor;}
        f32 getSpikeInertia_Out() const {return m_spikeInertia_Out;}
        f32 getSpikeInertia_EmergencyOut() const {return m_spikeInertia_EmergencyOut;}
        f32 getSpikeInertia_Holster() const {return m_spikeInertia_Holster;}
        f32 gethitWidthScale() const {return m_hitWidthScale;}
        f32 getRotationMargin() const {return m_rotationMargin.ToRadians();}
        f32 getSpikeVibration_UpDuration() const {return m_spikeVibration_UpDuration;}
        f32 getSpikeVibration_DownDuration() const {return m_spikeVibration_DownDuration;}

    private:
        
        Path m_texturePath;
        f32 m_baseWidth;
        f32 m_hitWidthScale;
        f32 m_minScaleFactor;
        f32 m_minSpacing;
        f32 m_maxSpacing;
        f32 m_baseHeight;
        f32 m_restHeightPercent;
        f32 m_maxHeight;
		u32 m_spikeFirstTexIndex, m_spikeLastTexIndex;
		f32 m_spikeWidth;
        f32 m_shakeDetectionRadius, m_spikeDetectionRadius, m_endOfSpikeDetectionRadius;
        f32 m_minAlertDuration, m_minSpikeDuration;
        u32 m_faction;
        f32 m_spikeAmplitude;
        f32 m_shakeAmplitude;
        f32 m_shakePulsation;
        f32 m_spikeYOffset;
        f32 m_hitMarginPercent;
        u32 m_hitLevel;
        f32 m_spikeAnimationFrameDuration;
        f32 m_spikeAnimationFirstFrameDuration;
        f32 m_baseAnimationFrameDuration;
        f32 m_risenHeightPercent;
        f32 m_spikeInertia_Out, m_spikeInertia_EmergencyOut, m_spikeInertia_Holster;
        f32 m_spikeDetectionRadius_Emergency;
        f32 m_spikeBounciness;
        u32 m_frontBaseTexIndex_Idle,m_frontBaseFirstTexIndex_Shaking,m_frontBaseLastTexIndex_Shaking;
        u32 m_backBaseTexIndex_Idle,m_backBaseFirstTexIndex_Shaking;
        Angle m_rotationMargin;
        f32 m_spikeVibration_UpDuration, m_spikeVibration_DownDuration;
    };

	///////////////////////////////////////////////////////////////////////////////////////////
	ITF_INLINE const Ray_SpikyShellTrapComponent_Template * Ray_SpikyShellTrapComponent::getTemplate() const
	{
		return static_cast<const class Ray_SpikyShellTrapComponent_Template *>(m_template);
	}

}

#endif
