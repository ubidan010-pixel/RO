
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONCOPYFACT_H_
#include "gameplay/ai/BTs/BTActionCopyFact.h"
#endif // _ITF_BTACTIONCOPYFACT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionCopyFact);

BTActionCopyFact::BTActionCopyFact()
{
}

bbool BTActionCopyFact::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getSrc().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"src",getTemplate()->getSrc().isValid(),"%s doesn't specify a source fact to copy",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getDst().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"dst",getTemplate()->getDst().isValid(),"%s doesn't specify a destination fact to copy",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void BTActionCopyFact::onActivate()
{
    m_tree->getBlackBoard().copyFact(getTemplate()->getSrc(),getTemplate()->getDst());
}

bbool BTActionCopyFact::update( f32 _dt )
{
    Super::update(_dt);

    return bfalse;
}

IMPLEMENT_OBJECT_RTTI(BTActionCopyFact_Template);

BEGIN_SERIALIZATION_CHILD(BTActionCopyFact_Template)

    SERIALIZE_MEMBER("src",m_src);
    SERIALIZE_MEMBER("dst",m_dst);

END_SERIALIZATION()

}