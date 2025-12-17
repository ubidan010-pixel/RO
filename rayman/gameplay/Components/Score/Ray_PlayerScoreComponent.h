#ifndef _ITF_RAY_PLAYERSCORECOMPONENT_H_
#define _ITF_RAY_PLAYERSCORECOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class Ray_PlayerScoreComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerScoreComponent, ActorComponent,3889959905)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_PlayerScoreComponent();
        virtual ~Ray_PlayerScoreComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        void ShowSpeedUpContextIcon(bbool _show);
        virtual void        Update( f32 _dt );
        virtual void        onEvent(Event * _event);
        void initLumSpeedFactor(u32 _playerNum);
        virtual void        Draw();

        void setPlayer(u32 _playerIndex);
        void HandleSpeedUpQoL();

    private:
        ITF_INLINE const class Ray_PlayerScoreComponent_Template* getTemplate() const;

				u32		getPlayerLums();
				bbool	isPlayerActive();
				u32		updateParticles( f32 _dt );
				void	setupParticles();
				void	processDisplayScore();

        class UITextBoxesComponent*		m_textComponent;
        class BezierCurveComponent*		m_bezierComponent;
        class LinkComponent*			m_linkComponent;
        //class UITextBox*				m_uiTextBox;
		class AnimatedComponent*		m_animComponent;

        u32 m_playerIndex;
        bbool m_speedUpButtonPressed;
        bbool m_showSpeedUpContextIcon;
        f32 m_speedFactor;
        u32 m_lastReleaseIndex;

        struct ParticleData
        {
            Vec3d m_pos;
            f32 m_speed;
            f32 m_curDist;
            f32 m_initialOffset;
        };

        typedef SafeArray <ParticleData> ParticlesContainer;

        ParticlesContainer      m_particles;
        ITF_ParticleGenerator   m_particleGenerator;
        ResourceID              m_resource;

        bbool                   m_releasing;
		bbool					m_releasingFinish;

        f32 m_timer;
        u32  m_arrivedIndex;
        u32 m_releasedIndex;
        u32 m_numLums;
		u32 m_countPlayerActive;
		f32 m_factorPlayer;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_PlayerScoreComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerScoreComponent_Template, TemplateActorComponent,3944500506);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlayerScoreComponent);

    public:

        Ray_PlayerScoreComponent_Template();
        virtual ~Ray_PlayerScoreComponent_Template();
        ITF_INLINE f32 getLumReleaseSpeed() const { return m_lumReleaseSpeed; }
        ITF_INLINE f32 getLumTrajectorySpeed() const { return m_lumTrajectorySpeed; }
        const Path&                             getTextureFile() const { return m_textureFile; }
        const ITF_ParticleGenerator_Template&   getParticleGenerator() const { return m_particleGenerator; }
        ITF_INLINE u32 getIndex() const { return m_index; }
        ITF_INLINE f32 getParticleSize() const { return m_particleSize; }
        ITF_INLINE f32 getParticleMaxOffset() const { return m_particleMaxOffset; }
        ITF_INLINE u32 getParticleGenerationAmplitude() const { return m_particleGenerationAmplitude; }
        ITF_INLINE f32 getSmoothFactor() const { return m_smoothFactor; }
        ITF_INLINE f32 getParticleGenerationFrequency() const { return m_particleGenerationFrequency; }

		ITF_INLINE f32 getFactorSpeedRelease_1P() const { return m_factorSpeedRelease_1P; }
		ITF_INLINE f32 getFactorSpeedRelease_2P() const { return m_factorSpeedRelease_2P; }
		ITF_INLINE f32 getFactorSpeedRelease_3P() const { return m_factorSpeedRelease_3P; }
        ITF_INLINE f32 getFactorSpeedRelease_4P() const { return m_factorSpeedRelease_4P; }
        ITF_INLINE f32 getFactorSpeedUpQoL() const { return m_factorSpeedUpQoL; }
    private:

        f32 m_lumReleaseSpeed;
        f32 m_lumTrajectorySpeed;
        ITF_ParticleGenerator_Template  m_particleGenerator;
        Path                            m_textureFile;
        u32 m_index;
        f32 m_particleSize;
        f32 m_particleMaxOffset;
        u32 m_particleGenerationAmplitude;
        f32 m_particleGenerationFrequency;
        f32 m_smoothFactor;

		f32 m_factorSpeedRelease_1P;
		f32 m_factorSpeedRelease_2P;
		f32 m_factorSpeedRelease_3P;
		f32 m_factorSpeedRelease_4P;
        f32 m_factorSpeedUpQoL;
    };

    //*****************************************************************************

    ITF_INLINE const Ray_PlayerScoreComponent_Template* Ray_PlayerScoreComponent::getTemplate() const
    {
        return static_cast<const Ray_PlayerScoreComponent_Template*>(m_template);
    }

}; // end namespace

#endif //_ITF_RAY_PLAYERSCORECOMPONENT_H_
