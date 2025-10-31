#ifndef _ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_
#define _ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_
#include "rayman/gameplay/Managers/Ray_ShooterGameModeController.h"
#endif //_ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_


namespace ITF
{
    class Ray_ShooterCheckPointComponent : public CheckpointComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterCheckPointComponent, CheckpointComponent ,362659308)

    public:
        DECLARE_SERIALIZE();

        Ray_ShooterCheckPointComponent();
        virtual ~Ray_ShooterCheckPointComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
		virtual void        onEvent( Event * _event );
        virtual void        Update( f32 _deltaTime );
        virtual void        onBecomeActive();

        virtual Vec2d       getPlayerSpawnPos( u32 _playerIndex ) const;

        ITF_INLINE  bbool   isExitMode() const;

	private:

        virtual void        checkpointReached( ObjectRef _player );
        bbool               isPlayerColliding( Player * _player );
        void                startLanding( Player * _playerToLand, bbool _landAll );

        class LinkComponent*    m_linkComponent;

        ITF_VECTOR<Vec2d>       m_spawnPositions;
        SafeArray<bbool,4>      m_playersInChpZone;             // player / checkpoint zone detection array

        ITF_INLINE const class Ray_ShooterCheckPointComponent_Template * getTemplate() const;
    };

    
    //-------------------------------------------------------------------------------------
    class Ray_ShooterCheckPointComponent_Template : public CheckpointComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterCheckPointComponent_Template, CheckpointComponent_Template ,2750908377);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterCheckPointComponent);

    public:

        Ray_ShooterCheckPointComponent_Template();
        ~Ray_ShooterCheckPointComponent_Template();

        ITF_INLINE Ray_ShooterGameModeParameters *  getShooterGameModeParameters() const {return m_shooterGameModeParameters;}
        const Vec2d &                               getPlayerSpawnPos( const StringID _playerID )const;
        ITF_INLINE  bbool                           getEnterExitMode() const { return m_enterExit; }
        ITF_INLINE  bbool                           getExitOnlyMode() const { return m_exitOnly; }
        ITF_INLINE  bbool                           useCameraBorderSpawn() const { return m_useCameraBorderSpawn; }
        ITF_INLINE  f32                             getCameraBorderSpawnOffset() const { return m_cameraBorderSpawnOffset; }
        ITF_INLINE  f32                             getCameraBorderSpawnPlayersOffset() const { return m_cameraBorderSpawnPlayersOffset; }
        ITF_INLINE  f32                             getVisualScaleMultiplier() const { return m_visualScaleMultiplier; }

    private:
        class PlayerSpawnPos
        {
        public:
            DECLARE_SERIALIZE();
            StringID m_playerID;
            Vec2d m_offset;
        };

        ITF_VECTOR<PlayerSpawnPos>      m_playersSpawnPosList;
        Ray_ShooterGameModeParameters * m_shooterGameModeParameters;
        bbool                           m_enterExit;
        bbool                           m_exitOnly;
        bbool                           m_useCameraBorderSpawn;
        f32                             m_cameraBorderSpawnOffset;
        f32                             m_cameraBorderSpawnPlayersOffset;
        f32                             m_visualScaleMultiplier;
    };


    ITF_INLINE const class Ray_ShooterCheckPointComponent_Template * Ray_ShooterCheckPointComponent::getTemplate() const
    {
        return static_cast<const class Ray_ShooterCheckPointComponent_Template *>(m_template);
    }

    ITF_INLINE  bbool   Ray_ShooterCheckPointComponent::isExitMode() const
    {
        return ( ( getTemplate() != NULL ) ? getTemplate()->getExitOnlyMode() : bfalse );
    }
};

#endif //_ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_
