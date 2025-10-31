#ifndef _ITF_WAR_AIROAMINGBEHAVIOR_H_
#define _ITF_WAR_AIROAMINGBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIWALKINDIRACTION_H_
#include "gameplay/ai/Actions/AIWalkInDirAction.h"
#endif //_ITF_AIWALKINDIRACTION_H_

#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

namespace ITF
{
    class StickToPolylinePhysComponent;

    class War_AIRoamingBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AIRoamingBehavior, AIBehavior,3749437115)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        War_AIRoamingBehavior();
        virtual ~War_AIRoamingBehavior();

        virtual void                        onActorLoaded();
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onActionFinished();

    private:

        void                                updateWalk( f32 _delta );
        void                                toggleWalkIdle();
        void                                setIdleTime();
        void                                setWalkTime();

        AIWalkInDirAction                   m_walkInDir;
        AIIdleAction                        m_idle;

        // Values from lua
        f32                                 m_minTimeToWalk;
        f32                                 m_maxTimeToWalk;
        f32                                 m_minTimeToIdle;
        f32                                 m_maxTimeToIdle;
        f32                                 m_walkForce;
        f32                                 m_timeCounter;
        bbool                               m_walkLeft;

        StickToPolylinePhysComponent*   m_physComponent;
    };

}

#endif //_ITF_WAR_AIROAMINGBEHAVIOR_H_

