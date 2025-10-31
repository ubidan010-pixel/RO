#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FIGHTMANAGER_H_
#include "rayman/gameplay/Managers/Ray_FightManager.h"
#endif //_ITF_RAY_FIGHTMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

namespace ITF
{
    Ray_FightManager* Ray_FightManager::s_instance = NULL;

    Ray_FightManager::Ray_FightManager()
    {
    }

    Ray_FightManager::~Ray_FightManager()
    {
    }

    void Ray_FightManager::init()
    {
    }

    void Ray_FightManager::update( f32 _dt )
    {
        for (u32 i = 0; i < m_pendingAttacks.size(); i++)
        {
            VictimData& victimData = m_pendingAttacks[i];
            victimData.update(_dt);

            // flush all attacks for this frame
            victimData.m_lastFrameRequestedAttacks = victimData.m_requestedAttacks;
            victimData.m_requestedAttacks.clear();
        }

        //m_pendingAttacks.clear();
    }

    void Ray_FightManager::requestAttack( ActorRef _attacker, ActorRef _victim, ObjectRef _swimmingPolyline /*= ITF_INVALID_OBJREF*/ )
    {
        VictimData* victimData(NULL);
        for (u32 i = 0; i < m_pendingAttacks.size(); i++)
        {
            VictimData& data = m_pendingAttacks[i];
            if (data.m_victim == _victim)
            {
                victimData = &data;
                break;
            }
        }

        if (!victimData)
        {
            m_pendingAttacks.push_back( VictimData() );
            victimData = &m_pendingAttacks.back();
            victimData->m_victim = _victim;
        }

        victimData->m_requestedAttacks.push_back( AttackerData() );
        AttackerData& attackerData = victimData->m_requestedAttacks.back();
        attackerData.m_attacker = _attacker;
        attackerData.m_swimmingPolyline = _swimmingPolyline;
    }

    u32 Ray_FightManager::getLastFrameRequestedAttackersCount( ActorRef _victim )
    {
        for (u32 i = 0; i < m_pendingAttacks.size(); i++)
        {
            VictimData& data = m_pendingAttacks[i];
            if (data.m_victim == _victim)
            {
                return data.m_lastFrameRequestedAttacks.size();
            }
        }
        return 0;
    }

    u32 Ray_FightManager::getLastFrameRequestedAttackersCount( ActorRef _victim, ObjectRef _swimmingPolyline )
    {
        for (u32 i = 0; i < m_pendingAttacks.size(); i++)
        {
            VictimData& data = m_pendingAttacks[i];
            if (data.m_victim == _victim)
            {
                u32 count = 0;
                for (u32 j = 0; j < data.m_lastFrameRequestedAttacks.size(); j++)
                {
                    if (data.m_lastFrameRequestedAttacks[j].m_swimmingPolyline == _swimmingPolyline)
                    {
                        count++;
                    }
                }
                return count;
            }
        }
        return 0;
    }

    bbool Ray_FightManager::removeFromLastFrameRequestedAttackers(ActorRef _attacker, ActorRef _victim)
    {
        for (u32 i = 0; i < m_pendingAttacks.size(); i++)
        {
            VictimData& data = m_pendingAttacks[i];
            if (data.m_victim == _victim)
            {
                for (u32 j=0; j<data.m_lastFrameRequestedAttacks.size(); j++)
                {
                    AttackerData &attacker = data.m_lastFrameRequestedAttacks[j];
                    if (attacker.m_attacker == _attacker)
                    {
                        data.m_lastFrameRequestedAttacks.eraseNoOrder(j);
                        return btrue;
                    }
                }
            }
        }
        return bfalse;
    }

    f32 Ray_FightManager::s_minTimeBetweenAttacks = 0.5f;

    i32 sortOnAttackCount(const void* _a, const void* _b)
    {
        const Ray_FightManager::AttackerData* a = (const Ray_FightManager::AttackerData*)_a;
        const Ray_FightManager::AttackerData* b = (const Ray_FightManager::AttackerData*)_b;
        return b->m_attackCount - a->m_attackCount;
    }


    void Ray_FightManager::VictimData::update( f32 _dt )
    {
        m_timer += _dt;

        if (m_timer < s_minTimeBetweenAttacks)
            return;

        if (m_requestedAttacks.size() == 0)
            return;


        bbool victimCanBeAttacked(bfalse);
        Actor* victim = m_victim.getActor();
        if (victim)
        {
            Ray_EventQueryCanBeAttacked query;
            victim->onEvent(&query);
            victimCanBeAttacked = query.getCanBeAttacked();
        }

        if (victimCanBeAttacked)
        {
            // go through all attackers (sorted by lowest attackCount) until one uses the attack

            for (u32 i = 0; i < m_requestedAttacks.size(); i++)
            {
                AttackerData& attackerData = m_requestedAttacks[i];

                i32 prevIndex = m_previousAttackers.find(attackerData);
                if ( prevIndex == -1 )
                {
                    // this guy never attacked
                    attackerData.m_attackCount = 0;
                }
                else
                {
                    // this guy already attacked, copy its attack count
                    attackerData.m_attackCount = m_previousAttackers[prevIndex].m_attackCount;
                }
            }

            qsort(&m_requestedAttacks[0], m_requestedAttacks.size(), sizeof(AttackerData), sortOnAttackCount);

            for (u32 i = 0; i < m_requestedAttacks.size(); i++)
            {
                AttackerData& attackerData = m_requestedAttacks[i];
                Actor* attackerActor = attackerData.m_attacker.getActor();
                if (attackerActor)
                {
                    //GFX_ADAPTER->drawDBGCircle(attackerActor->getPos(),0.5f, Color::green(), 0.1f);

                    Ray_EventAttackGranted eventAttack;
                    eventAttack.setSender(m_victim);
                    attackerActor->onEvent(&eventAttack);

                    if (eventAttack.getAttackUsed())
                    {
                        m_timer = 0;
                        attackerData.m_attackCount++;

                        // remember the fact that this guy attacked us
                        i32 prevIndex = m_previousAttackers.find(attackerData);
                        if ( prevIndex == -1 )
                        {
                            m_previousAttackers.push_back(attackerData);
                        }
                        else
                        {
                            m_previousAttackers[prevIndex].m_attackCount = attackerData.m_attackCount;
                        }

                        //GFX_ADAPTER->drawDBGCircle(attackerActor->getPos(), 1.f, Color::red(), 0.1f);
                        break;
                    }
                }
                // else: the actor was destroyed since it requested an attack... weird but could happen
            }

            //ITF_ASSERT_MSG(attackWasUsed, "some actors requested an attack but no-one used it... is that normal?");
        }
    }

} //namespace ITF
