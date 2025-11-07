#ifndef _ITF_RAY_MURPHYASSISTAICOMPONENT_H_
#define _ITF_RAY_MURPHYASSISTAICOMPONENT_H_

#ifndef _ITF_RAY_MURPHYAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_MurphyAIComponent.h"
#endif //_ITF_RAY_MURPHYAICOMPONENT_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

namespace ITF
{
    struct MurphyAssistTarget
    {
        enum Type { Cage, SkullCoin, Relic } type;
        ActorRef    actorRef;
        i32         auxIndex;
        bbool       active;
        MurphyAssistTarget(Type t, const ObjectRef& r, i32 aux = -1) : type(t), actorRef(r), auxIndex(aux), active(btrue) {}
    };

    class Ray_MurphyAssistAIComponent : public Ray_MurphyAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MurphyAssistAIComponent, Ray_MurphyAIComponent, 2664383627);
        DECLARE_VALIDATE_COMPONENT()

    public:
        DECLARE_SERIALIZE()

        Ray_MurphyAssistAIComponent();
        virtual ~Ray_MurphyAssistAIComponent();

        void                Update(f32 _dt) override;
        void                onActorLoaded(Pickable::HotReloadType _hotReload);
        void                requestAppear();
        void                forceDisappear();
        void                setCanFollowPlayer(bbool _follow) { m_canFollowPlayer = _follow; }
        void                followPlayer();
        void                markTargetCollected(const ActorRef& _actorRef);
        void                onEvent(Event* _event) override;

        ITF_INLINE const class Ray_MurphyAssistAIComponent_Template* getCustomTemplate() const;

    protected:

        void                setState(State _state) override;

    private:

        // indicator
        ActorRef                    m_indicatorRef;
        SpawneeGenerator            m_indicatorSpawner;
        void                        enableIndicatorAt(const Vec2d _position, const Vec2d _rotation);
        void                        updateOffscreenFX();
        bbool                       getScreenAABBCropped(const Margin& _margin, AABB& _screenAABB);


		enum AssistState
        {
            Assist_None = 0,
            Assist_FollowPlayer,
            Assist_GoToTarget,
            Assist_Talking
        };

        ITF_VECTOR<MurphyAssistTarget>  m_targets;
		AssistState                 m_assistState;
		Vec3d                       m_prevTargetPos;
		Vec3d                       m_smoothSpeed;
        i32                         m_currentTargetIdx;
        f32                         m_probeAccum;
        f32                         m_followAccum;
        bbool                       m_canFollowPlayer;

        void                        considerActorAdd(Actor* a);
        void                        considerActorRemove(Actor* a);
        i32                         findIndexByRef(const ObjectRef& r) const;
        void                        ensureBubbleDisabled();
        void                        enterFollowState();
        void                        enterGoToTargetState(i32 idx);
        void                        stepFollowPlayer(Actor* _player, f32 dt);
        void                        stepGoToTarget(f32 dt);
        i32                         nearestTargetWithin(const ITF_VECTOR<MurphyAssistTarget>& list, const Vec3d& probePos, f32 maxDeltaX);

        ActorRef                    m_toggleBubbleRef;
        SpawneeGenerator            m_toggleBubbleSpawner;
        void                        spawnToggleBubble();

        f32                         m_talkAccum;
        f32                         m_stillAccum;
        f32                         m_lastPlayerX;
        void                        enterTalkState();
        void                        showDialog();
        bbool                       updatePlayerStillness(const Actor* _player, const f32 _dt);
    };

    class Ray_MurphyAssistAIComponent_Template : public Ray_MurphyAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MurphyAssistAIComponent_Template, Ray_MurphyAIComponent_Template, 907154879);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MurphyAssistAIComponent);

    public:

        Ray_MurphyAssistAIComponent_Template();
        ~Ray_MurphyAssistAIComponent_Template() {};

        ITF_INLINE f32                                  getSpeedBlend() const { return m_speedBlend; }
        ITF_INLINE f32                                  getSpeedMin() const { return m_speedMin; }
        ITF_INLINE f32                                  getSpeedMax() const { return m_speedMax; }
        ITF_INLINE f32                                  getBlendAtSpeedMin() const { return m_blendAtSpeedMin; }
        ITF_INLINE f32                                  getBlendAtSpeedMax() const { return m_blendAtSpeedMax; }
        ITF_INLINE const Vec2d&                         getPlayerFollowOffset() const { return m_playerFollowOffset; }
        ITF_INLINE f32                                  getTargeEngageRadius() const { return m_targetEngageRadius; }
        ITF_INLINE f32                                  getArriveStopDist() const { return m_arriveStopDist; }
        ITF_INLINE f32                                  getGoBackMaxDeltaX() const { return m_goBackMaxDeltaX; }
        ITF_INLINE LocalisationId                       getMurphyAssistLocId() const { return m_murphyAssistDialogTextId; }
        ITF_INLINE const Path&                          getIndicatorPath() const { return m_indicatorAct; }
        ITF_INLINE f32                                  getIndicatorZOffset() const { return m_indicatorZOffset; }
        ITF_INLINE const Path&                          getToggleBubblePath() const { return m_toggleBubbleAct; }
        ITF_INLINE const Vec3d&                         getToggleSpawnOffset() const { return m_toggleBubbleSpawnOffset; }
        ITF_INLINE const ITF_VECTOR<LocalisationId>&    getJuiceLineIds() const { return m_juiceLineIds; }
        ITF_INLINE f32                                  getJuiceShowTime() const { return m_juiceShowTime; }
        ITF_INLINE f32                                  getStillTime() const { return m_stillTime; }
        ITF_INLINE f32                                  getMovementEpsilon() const { return m_movementEpsilon; }

    private:

		f32                         m_speedBlend;
		f32                         m_speedMin;
		f32                         m_speedMax;
		f32                         m_blendAtSpeedMin;
		f32                         m_blendAtSpeedMax;
		Vec2d                       m_playerFollowOffset;
        f32                         m_targetEngageRadius;
        f32                         m_arriveStopDist;
        f32                         m_goBackMaxDeltaX;
        LocalisationId              m_murphyAssistDialogTextId;
        Path                        m_indicatorAct;
        f32                         m_indicatorZOffset;
        Path                        m_toggleBubbleAct;
        Vec3d                       m_toggleBubbleSpawnOffset;
        ITF_VECTOR<LocalisationId>  m_juiceLineIds;
        f32                         m_juiceShowTime;
        f32                         m_stillTime;
        f32                         m_movementEpsilon;
    };

    const Ray_MurphyAssistAIComponent_Template* Ray_MurphyAssistAIComponent::getCustomTemplate() const
    {
        return static_cast<const class Ray_MurphyAssistAIComponent_Template*>(m_template);
    }

}

#endif //_ITF_RAY_MURPHYASSISTAICOMPONENT_H_
