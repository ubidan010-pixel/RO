#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIRECEIVEHITBEHAVIOR_H_
#include "war/gameplay/AI/Behaviors/War_AIReceiveHitBehavior.h"
#endif //_ITF_WAR_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIRECEIVEHITACTION_H_
#include "gameplay/AI/Actions/AIReceiveHitAction.h"
#endif //_ITF_AIRECEIVEHITACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AIReceiveHitBehavior)

    BEGIN_SERIALIZATION_CHILD(War_AIReceiveHitBehavior)
        SERIALIZE_CONTAINER_OBJECT("receiveHits", m_receiveHits, ESerialize_Template|ESerialize_PropertyEdit);
        SERIALIZE_FUNCTION(postLoad, ESerialize_Template_Load);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(War_AIReceiveHitBehavior, ReceiveHitData)
        SERIALIZE_CONTAINER("types", m_types, ESerialize_Template|ESerialize_PropertyEdit);
        SERIALIZE_MEMBER("level", m_level, ESerialize_Template|ESerialize_PropertyEdit);
        SERIALIZE_MEMBER("dead", m_dead, ESerialize_Template|ESerialize_PropertyEdit);
        SERIALIZE_OBJECT_WITH_FACTORY("action", m_action, ACTORSMANAGER->getAIActionsFactory(), ESerialize_Template|ESerialize_PropertyEdit);
    END_SERIALIZATION()

    War_AIReceiveHitBehavior::War_AIReceiveHitBehavior()
        : Super()
        , m_consecutiveHitCount(U32_INVALID)
    {
    }

    War_AIReceiveHitBehavior::~War_AIReceiveHitBehavior()
    {
        // registered actions have already been destroyed by AIBehavior
        // set them to null here to avoid deleting heap data
        for (u32 i = 0; i < m_receiveHits.size(); i++)
        {
            m_receiveHits[i].m_action = NULL;
        }
    }

    War_AIReceiveHitBehavior::ReceiveHitData::~ReceiveHitData()
    {
        SF_DEL(m_action);
    }

    void War_AIReceiveHitBehavior::postLoad()
    {
        for (u32 i = 0; i < m_receiveHits.size(); i++)
        {
            addAction(m_receiveHits[i].m_action);
        }
    }


    bbool War_AIReceiveHitBehavior::setData( HitStim* _hit )
    {
        War_HitStim* hit = _hit->DynamicCast<War_HitStim>(ITF_GET_STRINGID_CRC(War_HitStim,3270006373));
        ITF_ASSERT_MSG(hit, "received hit is not a War_HitStim!");
        if (!hit)
            return bfalse;


        if (m_consecutiveHitCount != U32_INVALID)
        {
            m_consecutiveHitCount++;
        }


        // apply the damage right now, so we know if it was a deadly hit
        m_aiComponent->receiveHit(hit);
        bbool dead = m_aiComponent->getHealth() <= 0;


        // select the proper action
        AIReceiveHitAction* receiveHit = NULL;

        for (u32 i = 0; i < m_receiveHits.size(); i++)
        {
            ReceiveHitData& data = m_receiveHits[i];

            if ( ( data.m_types.size() == 0 || data.m_types.find(hit->getReceivedHitType()) != -1 ) &&
                 ( data.m_level == U32_INVALID || data.m_level == hit->getLevel(m_actor->get2DPos()) ) &&
                 ( data.m_dead == dead ) )
            {
                receiveHit = data.m_action;
                break;
            }
        }

        if (receiveHit)
        {
            if ( m_currentAction )
            {
                m_currentAction->onDeactivate();
                m_currentAction = NULL;
            }

            receiveHit->setData(hit);
            setAction(receiveHit);

            m_animComponent->resetCurTime();
            //if (m_aiComponent->getCurrentBehavior() == this)
            //{
            //    m_animComponent->resetCurTime();
            //}
        }
        else if (!dead)
        {
            ITF_ASSERT_MSG(0, "no receiveHit action found for this hit!");
        }
        // else: go to death behavior

        return (receiveHit != NULL);
    }

    void War_AIReceiveHitBehavior::onActivate()
    {
        m_consecutiveHitCount = 1;
    }

    void War_AIReceiveHitBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_consecutiveHitCount = U32_INVALID;
    }

    void War_AIReceiveHitBehavior::onActionFinished()
    {
        Super::onActionFinished();

        m_aiComponent->onBehaviorFinished();
    }

    void War_AIReceiveHitBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        static const StringID s_ReceivedHitCount("ReceivedHitCount");

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(s_ReceivedHitCount, m_consecutiveHitCount);
        }
    }
}