#ifndef _ITF_RAY_AIRECEIVEHITACTION_H_
#define _ITF_RAY_AIRECEIVEHITACTION_H_

#ifndef _ITF_AIRECEIVEHITACTION_H_
#include "gameplay/AI/Actions/AIReceiveHitAction.h"
#endif //_ITF_AIRECEIVEHITACTION_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

namespace ITF
{
    class Ray_EventSpawnReward;

    class Ray_AIReceiveHitAction : public AIReceiveHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIReceiveHitAction, AIReceiveHitAction,4069339919);
        DECLARE_SERIALIZE()

    public:

        Ray_AIReceiveHitAction();
        virtual ~Ray_AIReceiveHitAction();

        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        updateAnimInputs();
        virtual void                        onEvent( Event * _event );
        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        void                                setData( HitStim* _hit );
        
        enum HitDirection
        {
            FRONT,
            BACK,
            ABOVE,
            BELOW,
            ENUM_FORCE_SIZE_32(0)
        };

    protected:

        ITF_INLINE const class Ray_AIReceiveHitAction_Template* getTemplate() const;

        void                                faceHitDir( const Vec2d& _hitDir );
        void                                setupPlayRate();
        void                                setupReward();

        Ray_EventSpawnReward*               m_reward;

        // from setData
        ActorRef                            m_attacker;
        EReceivedHitType                    m_hitType;
        u32                                 m_hitLevel;
        Vec2d                               m_hitDir;
        f32                                 m_hitPushBackDistance;
        HitDirection                        m_hitDirection;
        f32                                 m_prevWindMultiplier;
    };

    class Ray_AIReceiveHitAction_Template : public AIReceiveHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIReceiveHitAction_Template,AIReceiveHitAction_Template,419071238);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIReceiveHitAction);

    public:

        Ray_AIReceiveHitAction_Template();
        ~Ray_AIReceiveHitAction_Template();

        const class Ray_EventSpawnReward*   getReward() const { return m_reward; }
        f32                                 getPlayRateVariation() const { return m_playRateVariation; }
        bbool                               getFaceHitDir() const { return m_faceHitDir; }
        ITF_INLINE ITF::bbool getIgnoreWind() const { return m_ignoreWind; }

    private:

        Ray_EventSpawnReward*               m_reward;
        f32                                 m_playRateVariation;
        bbool                               m_faceHitDir;
        bbool                               m_ignoreWind;
    };

    ITF_INLINE const class Ray_AIReceiveHitAction_Template* Ray_AIReceiveHitAction::getTemplate() const { return static_cast<const Ray_AIReceiveHitAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AIRECEIVEHITACTION_H_