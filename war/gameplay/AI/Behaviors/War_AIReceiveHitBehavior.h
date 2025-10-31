#ifndef _ITF_WAR_AIRECEIVEHITBEHAVIOR_H_
#define _ITF_WAR_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIReceiveHitAction;

    class War_AIReceiveHitBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AIReceiveHitBehavior, AIBehavior,3140134935)
        DECLARE_SERIALIZE()

    public:

        War_AIReceiveHitBehavior();
        virtual ~War_AIReceiveHitBehavior();

        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActionFinished();
        virtual void                        updateAnimInputs();

        virtual bbool                       setData( HitStim* _hit );

    private:

        void                                postLoad();

        struct ReceiveHitData
        {
            DECLARE_SERIALIZE()

            ReceiveHitData()
                : m_level(U32_INVALID)
                , m_dead(bfalse)
                , m_action(NULL)
            {
            }
            ~ReceiveHitData();

            SafeArray<u32> m_types;                         // empty = any type
            //SafeArray<War_EReceivedHitType> m_types;        // empty = any type
            u32                             m_level;        // invalid = any level
            bbool                           m_dead;
            AIReceiveHitAction*             m_action;
        };

        ITF_VECTOR<ReceiveHitData>          m_receiveHits;

        u32                                 m_consecutiveHitCount;
    };

}

#endif //_ITF_WAR_AIRECEIVEHITBEHAVIOR_H_