#ifndef _ITF_RAY_CHECKPOINTCOMPONENT_H_
#define _ITF_RAY_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_


namespace ITF
{
    class Ray_CheckpointComponent : public CheckpointComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CheckpointComponent,CheckpointComponent,1094572706)

    public:
        DECLARE_SERIALIZE()

        Ray_CheckpointComponent();
        virtual ~Ray_CheckpointComponent();

        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);

        virtual void    Update( f32 _deltaTime );

        Vec2d           getPlayerSpawnPos( u32 _playerIndex ) const;

    protected:
        virtual void    checkpointReached( ObjectRef _player );

    private:

        enum Checkpoint_State
        {
            CPSTATE_NONE,
            CPSTATE_WAITING_CAMERA,
            CPSTATE_WAITING_ANIMATION,
            CPSTATE_DISPLAY_SCORE,
            CPSTATE_WAITING_EXIT,
            CPSTATE_TAKE_PICTURE,
        };

        ITF_INLINE const class Ray_CheckpointComponent_Template * getTemplate() const;

        void                        controlPlayers();
        void                        releasePlayers();
        bbool                       playNextStep();
        void                        displayScore(bbool _show);
        void                        displayAllScores(bbool _show);
        void                        displayContinue(bbool _show);

        AnimLightComponent*         m_animComponent;
        AnimLightComponent*         m_currentPlayerAnimCmp;
        mutable SafeArray <Vec2d>   m_spawnPositions;

        i32                         m_currentStep;
        f32                         m_waitTime;
        f32                         m_timeBetweenRank;
        f32                         m_displayScoreTime;
        f32                         m_currentTime;
        ITF_VECTOR< std::pair<u32,u32> > m_sortedPlayers;
        ITF_VECTOR<ActorRef>         m_fakeActors;
        ITF_VECTOR<SpawneeGenerator> m_spawneeGen;

        Checkpoint_State            m_state;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_CheckpointComponent_Template : public CheckpointComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CheckpointComponent_Template,CheckpointComponent_Template,1279601388);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CheckpointComponent);

    public:

        Ray_CheckpointComponent_Template()
            : m_savesState(bfalse)
            , m_waitTime(1.0f)
            , m_timeBetweenRank(1.0f)
            , m_displayScoreTime(1.0f)
        {}
        ~Ray_CheckpointComponent_Template() {}

        const Path&                 getSequence() const {return m_sequencePath;}
        bbool                       getSavesState() const {return m_savesState;}
        const f32                   getWaitTime() const { return m_waitTime; }
        const f32                   getTimeBetweenRank() const { return m_timeBetweenRank; }
        const f32                   getDisplayScoreTime() const { return m_displayScoreTime; }
        const ITF_VECTOR<StringID>& getStartAnimations() const {return m_startAnimations;}
        const ITF_VECTOR<StringID>& getAnimationsByRank() const {return m_animationsByRank;}
        const ITF_VECTOR<StringID>& getBoneNamesByRank() const {return m_boneNamesByRank;}
        const ITF_VECTOR<Path>&     getFakePlayers() const {return m_fakePlayers;}

    private:

        Path                        m_sequencePath;
        bbool                       m_savesState;
        f32                         m_waitTime;
        f32                         m_timeBetweenRank;
        f32                         m_displayScoreTime;
        ITF_VECTOR<StringID>        m_startAnimations;
        ITF_VECTOR<StringID>        m_animationsByRank;
        ITF_VECTOR<StringID>        m_boneNamesByRank;
        ITF_VECTOR<Path>            m_fakePlayers;
    };

    ITF_INLINE const class Ray_CheckpointComponent_Template * Ray_CheckpointComponent::getTemplate() const
    {
        return static_cast<const class Ray_CheckpointComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_CHECKPOINTCOMPONENT_H_