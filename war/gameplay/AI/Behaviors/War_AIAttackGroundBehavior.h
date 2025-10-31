#ifndef _ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_
#define _ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

#ifndef _ITF_WAR_AIWAITFORATTACKWINDOWACTION_H_
#include "war/gameplay/AI/Actions/War_AIWaitForAttackWindowAction.h"
#endif //_ITF_WAR_AIWAITFORATTACKWINDOWACTION_H_

#ifndef _ITF_AIWALKINDIRACTION_H_
#include "gameplay/ai/Actions/AIWalkInDirAction.h"
#endif //_ITF_AIWALKINDIRACTION_H_

#ifndef _ITF_WAR_AIPERFORMHITRAYCASTACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitRaycastAction.h"
#endif //_ITF_WAR_AIPERFORMHITRAYCASTACTION_H_

#ifndef _ITF_WAR_AIPERFORMHITSTIMACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitStimAction.h"
#endif //_ITF_WAR_AIPERFORMHITSTIMACTION_H_

namespace ITF
{
    class War_AIAttackGroundBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AIAttackGroundBehavior, AIBehavior,718234413)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        War_AIAttackGroundBehavior();
        virtual ~War_AIAttackGroundBehavior();

                void                        onSerialize();
        virtual void                        onActorLoaded();
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onActionFinished();

                void                        setWalkDir( bbool _left ) { m_walkLeft_dest = _left; }

    private:

        class StickToPolylinePhysComponent* m_physComponent;

        AIPlayAnimAction                    m_surpriseAction;
        AIWalkInDirAction                   m_walkAction;               // actually a run towards detected target
        War_AIWaitForAttackWindowAction     m_hitWaitAction;            // wait between hits
        War_AIPerformHitRaycastAction       m_hitAction;                // perform hit (receive hit is in a different behavior)
        //War_AIPerformHitStimAction          m_hitAction;
        AIPlayAnimAction                    m_reloadAction;

        bbool                               m_walkLeft;
        bbool                               m_walkLeft_dest;

        f32                                 m_attackRadius;
        f32                                 m_sqrAttackRadius;
        u32                                 m_maxConsecutiveHits;       // number of consecutive shots between reloads (aka magazine size)
        u32                                 m_consecutiveHitCount;

        void                                updateWalk( f32 _delta );
        void                                checkEnemyInRange();
    };
}

#endif //_ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_

