
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONSENDEVENTTOACTOR_H_
#include "gameplay/ai/BTs/BTActionSendEventToActor.h"
#endif // _ITF_BTACTIONSENDEVENTTOACTOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionSendEventToActor);

BTActionSendEventToActor::BTActionSendEventToActor()
: m_event(NULL)
{
}

BTActionSendEventToActor::~BTActionSendEventToActor()
{
    SF_DEL(m_event);
}

void BTActionSendEventToActor::init( class BehaviorTree* _tree, const class BTNode_Template* _template )
{
    Super::init(_tree,_template);

    if ( getTemplate()->getEvent() )
    {
        m_event = getTemplate()->getEvent()->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
        BinaryClone(const_cast<Event*>(getTemplate()->getEvent()), m_event);
        m_event->setSender(m_tree->getActor()->getRef());
    }
}

bbool BTActionSendEventToActor::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( m_event == NULL )
    {
        ITF_WARNING_PARAM(_actor,"fact",m_event != NULL,"%s doesn't specify an event to send",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void BTActionSendEventToActor::onActivate()
{
    Super::onActivate();

    if ( getTemplate()->getTarget().isValid() )
    {
        ObjectRef targetPlayer = ITF_INVALID_OBJREF;

        if ( !m_tree->getBlackBoard().getFact(getTemplate()->getTarget(),targetPlayer) || targetPlayer == ITF_INVALID_OBJREF )
        {
            ITF_ASSERT_MSG(0,"BTActionSendEventToPlayer: actor set not valid");
        }
        else
        {
            Actor* targetActor = AIUtils::getActor(targetPlayer);

            if ( targetActor )
            {
                targetActor->onEvent(m_event);
            }
        }
    }
    else
    {
        m_tree->getActor()->onEvent(m_event);
    }
}

bbool BTActionSendEventToActor::update( f32 _dt )
{
    Super::update(_dt);

    return bfalse;
}

IMPLEMENT_OBJECT_RTTI(BTActionSendEventToActor_Template);

BEGIN_SERIALIZATION_CHILD(BTActionSendEventToActor_Template)

    SERIALIZE_MEMBER("target",m_target);
    SERIALIZE_OBJECT_WITH_FACTORY("event",m_event,ACTORSMANAGER->getEventFactory());

END_SERIALIZATION()

BTActionSendEventToActor_Template::BTActionSendEventToActor_Template()
: m_event(NULL)
{
}

BTActionSendEventToActor_Template::~BTActionSendEventToActor_Template()
{
    SF_DEL(m_event);
}

}