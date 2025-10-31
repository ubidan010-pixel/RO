#ifndef _ITF_WAR_AIPERFORMHITRAYCASTACTION_H_
#define _ITF_WAR_AIPERFORMHITRAYCASTACTION_H_

#ifndef _ITF_WAR_AIPERFORMHITBASEACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitBaseAction.h"
#endif //_ITF_WAR_AIPERFORMHITBASEACTION_H_

namespace ITF
{
    class War_AIPerformHitRaycastAction : public War_AIPerformHitBaseAction
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AIPerformHitRaycastAction, War_AIPerformHitBaseAction,3352508404)
        DECLARE_SERIALIZE()

    public:

        War_AIPerformHitRaycastAction();
        virtual ~War_AIPerformHitRaycastAction();

        //virtual void        onActorLoaded();

    protected:

        virtual void        performHit( class AnimGameplayEvent* _animEvent );

    private:

        StringID            m_boneName;
        u32                 m_boneIndex;
        StringID            m_polylineImpactFX;
    };

}

#endif //_ITF_WAR_AIPERFORMHITRAYCASTACTION_H_

