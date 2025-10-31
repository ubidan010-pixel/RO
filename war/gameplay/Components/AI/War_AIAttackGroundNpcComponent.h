#ifndef _ITF_WAR_AIATTACKGROUNDNPCCOMPONENT_H_
#define _ITF_WAR_AIATTACKGROUNDNPCCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_WAR_AIROAMINGBEHAVIOR_H_
#include "war/gameplay/ai/Behaviors/War_AIRoamingBehavior.h"
#endif //_ITF_WAR_AIROAMINGBEHAVIOR_H_

#ifndef _ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_
#include "war/gameplay/ai/Behaviors/War_AIAttackGroundBehavior.h"
#endif //_ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_

#ifndef _ITF_WAR_AIRECEIVEHITBEHAVIOR_H_
#include "war/gameplay/AI/Behaviors/War_AIReceiveHitBehavior.h"
#endif //_ITF_WAR_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_AIDEATHBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIDeathBehavior.h"
#endif //_ITF_AIDEATHBEHAVIOR_H_

namespace ITF
{
    class War_AIAttackGroundNpcComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AIAttackGroundNpcComponent, AIComponent,889251451)

    public:
        DECLARE_SERIALIZE()

        War_AIAttackGroundNpcComponent();

        void                        postLoad();

        virtual void                onActorLoaded();
        virtual void                Update( f32 _deltaTime );
        virtual void                onEvent( Event* _event);
        virtual void                onBehaviorFinished();

    private:

        void                        receiveHit( War_HitStim* _hit );
        bbool                       checkEnemy();

        War_AIRoamingBehavior       m_roamingBehavior;
        War_AIAttackGroundBehavior  m_attackGroundBehavior;
        War_AIReceiveHitBehavior    m_receiveHitBehavior;
        AIDeathBehavior             m_deathBehavior;
        f32                         m_detectRadius;
        bbool                       m_walkLeft;
    };

}

#endif //_ITF_WAR_AIATTACKGROUNDNPCCOMPONENT_H_