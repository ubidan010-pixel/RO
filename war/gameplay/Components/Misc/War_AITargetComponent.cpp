#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AITARGETCOMPONENT_H_
#include "war/gameplay/Components/Misc/War_AITargetComponent.h"
#endif //_ITF_WAR_AITARGETCOMPONENT_H_#

#ifdef ITF_IPAD
#include <algorithm>
#endif

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AITargetComponent)

    BEGIN_SERIALIZATION_CHILD(War_AITargetComponent)
        SERIALIZE_MEMBER("maxAttackerCount", m_maxAttackerCount, ESerialize_Template|ESerialize_PropertyEdit);
    END_SERIALIZATION()

    War_AITargetComponent::War_AITargetComponent()
        : Super()
        , m_maxAttackerCount(U32_INVALID)       // infinite
    {
    }

    War_AITargetComponent::~War_AITargetComponent()
    {
    }

    void War_AITargetComponent::onActorLoaded()
    {
    }

    void War_AITargetComponent::Update(f32 _deltaTime)
    {
    }

    //void War_AITargetComponent::registerAttacker( Actor* _attacker )
    //{
    //    m_attackers.push_back(_attacker);
    //}

    //void War_AITargetComponent::unregisterAttacker( Actor* _attacker )
    //{
    //    TargetList::iterator it = find(m_attackers.begin(), m_attackers.end(), _attacker);
    //    m_attackers.erase(it);
    //}

    bbool War_AITargetComponent::canGetAttacked()
    {
        return m_pendingAttacks.size() < m_maxAttackerCount;
    }

    void War_AITargetComponent::requestAttack( Actor* _attacker )
    {
        m_pendingAttacks.push_back(_attacker);

        if (m_pendingAttacks.size() == 1)
        {
            War_EventAttackRequestGranted grantRequest;
            _attacker->onEvent(&grantRequest);
        }
    }

    void War_AITargetComponent::attackPerformed( Actor* _attacker )
    {
        TargetList::iterator it = find(m_pendingAttacks.begin(), m_pendingAttacks.end(), _attacker);
        m_pendingAttacks.erase(it);

        if (m_pendingAttacks.size() > 0)
        {
            War_EventAttackRequestGranted grantRequest;
            m_pendingAttacks.at(0)->onEvent(&grantRequest);
        }
    }

};