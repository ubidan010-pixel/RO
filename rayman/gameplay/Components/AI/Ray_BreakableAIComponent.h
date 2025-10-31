#ifndef _ITF_RAY_BREAKABLECOMPONENT_H_
#define _ITF_RAY_BREAKABLECOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{
    class Ray_BreakableAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BreakableAIComponent, Ray_AIComponent,1847158746)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BreakableAIComponent();
        virtual ~Ray_BreakableAIComponent();

        virtual bbool           needsUpdate() const { return btrue; }
        virtual bbool           needsDraw() const { return bfalse; }
        virtual bbool           needsDraw2D() const { return bfalse; }

        virtual void            onBecomeActive();
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onCheckpointLoaded();
        virtual void            Update( f32 _dt );
        virtual void            onEvent( Event* _event );

    private:
        ITF_INLINE const class Ray_BreakableAIComponent_Template * getTemplate() const;

        void                    processStim( PunchStim* _stim );
        void                    processBreakEvent( Ray_EventBreakableBreak* _breakEvent );
        void                    processBreakQuery( Ray_EventBreakableQuery* _breakQuery );

        bbool                   playNextAvailableDestruction();

        void                    deactivateActor();
        void                    reactivateActor();

        u32                     m_currentDestructionStage;
        u32                     m_targetDestructionStage;
        i32                     m_justReceivedHit;  // 0: no, 1: normal direction, -1: reverse direction
        bbool                   m_isPlayingDestruction;
        bbool                   m_mustBreak;
        bbool                   m_mustUnbreak;
        bbool                   m_restoring;

        Ray_EventSpawnReward*   m_reward;
    };

    //-------------------------------------------------------------------------------------
    class Ray_BreakableAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BreakableAIComponent_Template,Ray_AIComponent_Template,1253773257);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BreakableAIComponent);

    public:

        class DestructionStage 
        {
            DECLARE_SERIALIZE()

        public:

            const StringID&     getRumble() const { return m_rumble; }
            const StringID&     getDestroy() const { return m_destroy; }
            const StringID&     getDestroyReverse() const { return m_destroyReverse; }
            const StringID&     getStand() const { return m_stand; }

        private:

            StringID    m_rumble;
            StringID    m_destroy;
            StringID    m_destroyReverse;
            StringID    m_stand;
        };
        typedef SafeArray<DestructionStage> DestructionStageList;

        Ray_BreakableAIComponent_Template();
        ~Ray_BreakableAIComponent_Template();

        const Ray_EventSpawnReward*     getRewardEvent() const { return m_reward; }
        ITF_INLINE u32                  getCrushAttackDamage() const { return m_crushAttackDamage; }
        ITF_INLINE u32                  getFrontDamage() const { return m_frontDamage; }
        ITF_INLINE u32                  getBackDamage() const { return m_backDamage; }
        const DestructionStageList&     getDestructionStages() const { return m_destructionStages; }
        ITF_INLINE f32                  getHitAngleOffset() const { return m_hitAngleOffset.ToRadians(); }
        ITF_INLINE f32                  getHitAngleMinIncidence() const { return m_hitAngleMinIncidence.ToRadians(); }
        ITF_INLINE const StringID&      getRestoreAnim() const { return m_restoreAnim; }
        ITF_INLINE bbool                getCheckHitSenderDirection() const {return m_checkHitSenderDirection;}
        ITF_INLINE ITF::StringID        getOpenAnim() const { return m_openAnim; }
        ITF_INLINE ITF::StringID        getOpenAnimReverse() const { return m_openAnimReverse; }
        ITF_INLINE bbool                getResetOnCheckpoint() const { return m_resetOnCheckpoint; }
    private:

        Ray_EventSpawnReward*   m_reward;
        u32                     m_crushAttackDamage;
        u32                     m_frontDamage;
        u32                     m_backDamage;
        Angle                   m_hitAngleOffset;
        Angle                   m_hitAngleMinIncidence;
        DestructionStageList    m_destructionStages;
        StringID                m_restoreAnim;
        bbool                   m_checkHitSenderDirection;
        StringID                m_openAnim;
        StringID                m_openAnimReverse;
        bbool                   m_resetOnCheckpoint;
    };

    ITF_INLINE const Ray_BreakableAIComponent_Template * Ray_BreakableAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BreakableAIComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_BREAKABLECOMPONENT_H_