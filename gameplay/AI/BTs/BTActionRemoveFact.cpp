
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONREMOVEFACT_H_
#include "gameplay/ai/BTs/BTActionRemoveFact.h"
#endif // _ITF_BTACTIONREMOVEFACT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionRemoveFact);

BTActionRemoveFact::BTActionRemoveFact()
{
}

bbool BTActionRemoveFact::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getFact().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"fact",getTemplate()->getFact().isValid(),"%s doesn't specify an fact to remove",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void BTActionRemoveFact::onActivate()
{
    m_tree->getBlackBoard().removeFact(getTemplate()->getFact());
}

bbool BTActionRemoveFact::update( f32 _dt )
{
    Super::update(_dt);

    return bfalse;
}

IMPLEMENT_OBJECT_RTTI(BTActionRemoveFact_Template);

BEGIN_SERIALIZATION_CHILD(BTActionRemoveFact_Template)

    SERIALIZE_MEMBER("fact",m_fact);

END_SERIALIZATION()

}