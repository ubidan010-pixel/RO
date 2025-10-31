#ifndef _ITF_WAR_AIRECEIVEHITACTION_H_
#define _ITF_WAR_AIRECEIVEHITACTION_H_

#ifndef _ITF_AIRECEIVEHITACTION_H_
#include "gameplay/AI/Actions/AIReceiveHitAction.h"
#endif //_ITF_AIRECEIVEHITACTION_H_

#ifndef _ITF_WAR_GAMEPLAYTYPES_H_
#include "war/gameplay/War_GameplayTypes.h"
#endif //_ITF_WAR_GAMEPLAYTYPES_H_

namespace ITF
{
    class War_AIReceiveHitAction : public AIReceiveHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AIReceiveHitAction, AIReceiveHitAction,3967443887)
        DECLARE_SERIALIZE()

    public:

        War_AIReceiveHitAction();
        virtual ~War_AIReceiveHitAction();

        virtual void                        onActorLoaded();
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActorBecomeInactive();
        virtual void                        update( f32 _delta );
        virtual void                        updateAnimInputs();
        virtual void                        onEvent( class Event* _event);

        void                                setData( HitStim* _hit );

    private:

        void                                setupHit();
        void                                setNormalHit();
        void                                updateNormalHit( f32 _dt );
        void                                newHit( War_HitStim* _hit );
        void                                doDamage();
        void                                setupNormalHitReward();

        StickToPolylinePhysComponent*       m_physComponent;

        f32                                 m_hitForce;
        f32                                 m_frictionMultiplier;
        f32                                 m_gravityMultiplier;
        f32                                 m_playRateVariation;
        f32                                 m_minStunTime;
        f32                                 m_maxStunTime;

        ObjectRef                           m_attacker;
        Vec2d                               m_hitDir;
        War_EReceivedHitType                m_receivedHitType;
        Vec3d                               m_hitFxPos;
        u32                                 m_hitLevel;

        f32                                 m_prevFrictionMultiplier;
        f32                                 m_prevGravityMultiplier;
        f32                                 m_animCursor;
        f32                                 m_timeCounter;
        bbool                               m_stunned;
        bbool                               m_canBeFinishedOff;
    };

}

#endif //_ITF_WAR_AIRECEIVEHITACTION_H_