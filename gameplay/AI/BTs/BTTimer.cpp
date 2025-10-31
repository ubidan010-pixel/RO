
#include "precompiled_gameplay.h"

#ifndef _ITF_BTTIMER_H_
#include "gameplay/ai/BTs/BTTimer.h"
#endif // _ITF_BTTIMER_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTTimer);

BTTimer::BTTimer()
: m_node(NULL)
, m_waitTime(0.f)
{
}

BTTimer::~BTTimer()
{
    SF_DEL(m_node);
}

void BTTimer::init( class BehaviorTree* _tree, const class BTNode_Template* _template )
{
    Super::init(_tree,_template);

    const BTNode_Template* nodeTemplate = _tree->getTemplate()->getNodeFromRef(getTemplate()->getNodeRef());

    m_node = nodeTemplate ? nodeTemplate->createBTNode() : NULL;

    if ( m_node )
    {
        m_node->init(_tree,nodeTemplate);
    }

    resetTime();
}

bbool BTTimer::validate( Actor* _actor ) const
{
    if ( m_node == NULL )
    {
        ITF_WARNING_PARAM(_actor,"BTTimer",m_node!=NULL,"BTTimer has no child");
        return bfalse;
    }

    if ( getTemplate()->getMinTime() == 0.f && getTemplate()->getMaxTime() == 0.f )
    {
        ITF_WARNING_PARAM(_actor,"min/max Time",0,"BTTimer has time 0");
        return bfalse;
    }

    return btrue;
}

bbool BTTimer::decide( f32 _dt )
{
    bbool retValue;

    m_waitTime -= _dt;

    if ( m_waitTime <= 0.f )
    {
        retValue = btrue;
        resetTime();
    }
    else
    {
        retValue = bfalse;
    }

    return retValue;
}

void BTTimer::onActivate()
{
    Super::onActivate();

    m_node->onActivate();
}

void BTTimer::onDeactivate()
{
    Super::onActivate();

    m_node->onDeactivate();
}

bbool BTTimer::update( f32 _dt )
{
    Super::update(_dt);

    return m_node->update(_dt);
}

void BTTimer::onEvent( Event* _event )
{
    Super::onEvent(_event);

    m_node->onEvent(_event);
}

void BTTimer::resetTime()
{
    m_waitTime = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinTime(),getTemplate()->getMaxTime());
}

#ifdef ITF_SUPPORT_EDITOR
void BTTimer::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags  ) const
{
    Super::drawEdit(drawInterface,_flags);

    return m_node->drawEdit(drawInterface,_flags);
}
#endif // ITF_SUPPORT_EDITOR

IMPLEMENT_OBJECT_RTTI(BTTimer_Template);

BEGIN_SERIALIZATION_CHILD(BTTimer_Template)

    SERIALIZE_OBJECT("node",m_node);
    SERIALIZE_MEMBER("minTime",m_minTime);
    SERIALIZE_MEMBER("maxTime",m_maxTime);

END_SERIALIZATION()

BTTimer_Template::BTTimer_Template()
: m_minTime(1.f)
, m_maxTime(1.f)
{
}

BTTimer_Template::~BTTimer_Template()
{
    SF_DEL(m_node.m_refPtr);
}

}
