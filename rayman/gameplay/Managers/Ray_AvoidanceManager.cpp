#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AVOIDANCEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AvoidanceManager.h"
#endif //_ITF_RAY_AVOIDANCEMANAGER_H_

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
    Ray_AvoidanceManager* Ray_AvoidanceManager::s_instance = NULL;

    Ray_AvoidanceManager::Ray_AvoidanceManager()
#ifdef ITF_SUPPORT_DEBUGFEATURE
        : m_drawDebug(bfalse)
#endif // ITF_SUPPORT_DEBUGFEATURE
    {
    }

    Ray_AvoidanceManager::~Ray_AvoidanceManager()
    {
        for (MemberList::iterator it = m_members.begin(); it != m_members.end(); it++)
        {
            delete *it;
        }
    }

    void Ray_AvoidanceManager::init()
    {
    }

    Ray_SquadMemberData* Ray_AvoidanceManager::addMember( ObjectRef _actor )
    {
        // check if member is already registered
        u32 memberCount = m_members.size();
        for (u32 i = 0; i < memberCount; i++)
        {
            if (m_members[i]->m_actor == _actor)
                return m_members[i];
        }

        // add new member
        Ray_SquadMemberData* member = newAlloc(mId_Gameplay, Ray_SquadMemberData());
        member->m_actor = _actor;
        m_members.push_back(member);
        update(0);
        return member;
    }

    void Ray_AvoidanceManager::removeMember( ObjectRef _actor )
    {
        for (MemberList::iterator it = m_members.begin(); it != m_members.end(); it++)
        {
            Ray_SquadMemberData* member = *it;
            if (member->m_actor == _actor)
            {
                delete member;
                m_members.erase(it);
                update(0);
                return;
            }
        }
    }

    void Ray_AvoidanceManager::update( f32 _dt )
    {
        m_enabledMembers.clear();
        u32 memberCount = m_members.size();
        for (u32 i = 0; i < memberCount; i++)
        {
            Ray_SquadMemberData* member = m_members[i];
            if (member->getEnabled())
                m_enabledMembers.push_back(member);
        }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        if (m_drawDebug)
        {
            drawDebug();
        }
#endif // ITF_SUPPORT_DEBUGFEATURE
    }

    Vec3d Ray_SquadMemberData::get3DPos()
    {
        return AIUtils::getActor(m_actor)->getPos();
    }

    void Ray_SquadMemberData::getNeighbors( Ray_SquadMemberData*& _left, Ray_SquadMemberData*& _right )
    {
        f32 closestSqrDistLeft = F32_INFINITY;
        f32 closestSqrDistRight = F32_INFINITY;
        //f32 closestSqrDistLeft = (getRadius() + getMinDelta()) * (getRadius() + getMinDelta());
        //f32 closestSqrDistRight = closestSqrDistLeft;

        u32 memberCount = RAY_AVOIDANCEMANAGER->m_enabledMembers.size();
        for (u32 i = 0; i < memberCount; i++)
        {
            Ray_SquadMemberData* member = RAY_AVOIDANCEMANAGER->m_enabledMembers[i];
            
            if (member->m_actor != m_actor)
            {
                Vec2d vecToMember = member->getPosition() - getPosition();
                f32 sqrDist = vecToMember.sqrnorm();

                if (vecToMember.dot(Vec2d::Right) < 0)
                {
                    // member is on our left side
                    if (sqrDist < closestSqrDistLeft)
                    {
                        closestSqrDistLeft = sqrDist;
                        _left = member;
                    }
                }
                else
                {
                    // member is on our right side
                    if (sqrDist < closestSqrDistRight)
                    {
                        closestSqrDistRight = sqrDist;
                        _right = member;
                    }
                }
            }
        }
    }

    void Ray_SquadMemberData::avoidNeighbors( bbool _targetIsLeft,AvoidanceData & _data)
    {
        Ray_SquadMemberData* memberLeft = NULL;
        Ray_SquadMemberData* memberRight = NULL;
        getNeighbors(memberLeft, memberRight);

        f32 distLeft = 0;
        f32 avoidanceRadiusLeft = 0;
        f32 availableDistLeft = 0;
        f32 penetrationLeft = 0;    // how much we collide with the guy on the left
        f32 destinationLeft = 0;    // how far we'll go if we decide to move away from the guy on the left
        bbool blockedLeft = bfalse;

        f32 distRight = 0;
        f32 avoidanceRadiusRight = 0;
        f32 availableDistRight = 0;
        f32 penetrationRight = 0;
        f32 destinationRight = 0;
        bbool blockedRight = bfalse;

        if (memberLeft)
        {
            //distLeft = (memberLeft->getDestination() - getPosition()).norm();
            distLeft = f32_Abs(memberLeft->getDestination().m_x - getPosition().m_x);
            avoidanceRadiusLeft = memberLeft->getRadius() + getRadius();
            availableDistLeft = (distLeft - avoidanceRadiusLeft);
        }

        if (memberRight)
        {
            //distRight = (memberRight->getDestination() - getPosition()).norm();
            distRight = f32_Abs(memberRight->getDestination().m_x - getPosition().m_x);
            avoidanceRadiusRight = memberRight->getRadius() + getRadius();
            availableDistRight = (distRight - avoidanceRadiusRight);
        }

        if (memberLeft)
        {
            getPenetration(
                distLeft, avoidanceRadiusLeft, _targetIsLeft,
                memberRight && memberRight->getIsBlocked(), availableDistRight,
                penetrationLeft, destinationLeft, blockedLeft);
        }

        if (memberRight)
        {
            getPenetration(
                distRight, avoidanceRadiusRight, !_targetIsLeft,
                memberLeft && memberLeft->getIsBlocked(), availableDistLeft,
                penetrationRight, destinationRight, blockedRight);
        }

        _data.penetrationLeft = penetrationLeft;
        _data.destinationLeft = destinationLeft;
        _data.penetrationRight = penetrationRight;
        _data.destinationRight = destinationRight;
        _data.blocked = (blockedLeft || blockedRight);
    }

    void Ray_SquadMemberData::getPenetration( f32 _dist, f32 _avoidanceRadius, bbool _isInTargetDir,
        bbool _oppositeIsBlocking, f32 _oppositeAvailableDist,
        f32& _penetration, f32& _destination, bbool& _blocked )
    {
        if (_dist < _avoidanceRadius - m_minDelta)
        {
            _penetration = (_avoidanceRadius - _dist);      // positive penetration: get away
        }
        else if (_dist > _avoidanceRadius + m_minDelta && _isInTargetDir)
        {
            _penetration = (_avoidanceRadius - _dist);      // negative penetration: get closer
        }

        _destination = (_avoidanceRadius - _dist);


        /*if ( !_isInTargetDir && _oppositeIsBlocking )
        {
            // opposite dude is blocking, so clamp our penetration

            if (_oppositeAvailableDist > m_avoidanceMinDelta)
            {
                if ( f32_Abs(_penetration) > _oppositeAvailableDist )
                {
                    _penetration = _oppositeAvailableDist;
                }
            }
            else if (_oppositeAvailableDist < 0)
            {
                // we're too close already
                _penetration = 0;
                _blocked = btrue;
            }
        }*/
        if ( !_isInTargetDir && _oppositeIsBlocking )
        {
            // opposite dude is blocking...

            if (_oppositeAvailableDist > m_minDelta)
            {
                // ...so we can't get away too much
                if ( _penetration > _oppositeAvailableDist )
                {
                    _penetration = _oppositeAvailableDist;
                }
            }
            else if (_oppositeAvailableDist < 0)
            {
                // we're too close from the opposite dude already
                _penetration = 0;
                _blocked = btrue;
            }
            /*else if (_oppositeAvailableDist < 0)
            {
                // we're too close from the opposite dude already
                if ( _penetration > 0)
                {
                    // so don't get any closer
                    _penetration = 0;
                    _blocked = btrue;
                }
            }*/
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void Ray_AvoidanceManager::drawDebug()
    {
        u32 memberCount = m_enabledMembers.size();
        for (u32 i = 0; i < memberCount; i++)
        {
            Ray_SquadMemberData* member = m_enabledMembers[i];

            const Vec3d& memberPos = member->get3DPos();
            GFX_ADAPTER->drawDBGCircle(memberPos, member->getRadius(), Color::green());

            String8 text;text.setTextFormat("%d", i);
            Vec3d textPos;
            GFX_ADAPTER->compute3DTo2D(memberPos + Vec3d(0, 1.5f, 0), textPos);
            GFX_ADAPTER->drawDBGText(text, member->getIsBlocked() ? Color::black() : Color::white(), textPos.m_x, textPos.m_y);

            Ray_SquadMemberData* memberLeft = NULL;
            Ray_SquadMemberData* memberRight = NULL;
            member->getNeighbors(memberLeft, memberRight);
            if (memberLeft)
                GFX_ADAPTER->drawDBG3DLine(memberPos, memberLeft->get3DPos());
            if (memberRight)
                GFX_ADAPTER->drawDBG3DLine(memberPos, memberRight->get3DPos());
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

} //namespace ITF
