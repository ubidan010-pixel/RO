
#include "precompiled_gameplay.h"

#ifndef _ITF_BTDECIDERHASFACT_H_
#include "gameplay/ai/BTs/BTDeciderHasFact.h"
#endif // _ITF_BTDECIDERHASFACT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTDeciderHasFact);

BTDeciderHasFact::BTDeciderHasFact()
{
}

BTDeciderHasFact::~BTDeciderHasFact()
{
}

bbool BTDeciderHasFact::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( getTemplate()->getFactHave().size() == 0 &&
         getTemplate()->getFactNotHave().size() == 0 )
    {
        ITF_WARNING_PARAM(_actor,"facts",0,"DeciderHasFact doesn't specify a valid fact");
        return bfalse;
    }

    return btrue;
}

bbool BTDeciderHasFact::decide( f32 _dt )
{
    u32 haveFacts = getTemplate()->getFactHave().size();

    for ( u32 i = 0; i < haveFacts; i++ )
    {
        if ( !m_tree->getBlackBoard().factExists(getTemplate()->getFactHave()[i]) )
        {
            return bfalse;
        }
    }

    u32 notHaveFacts = getTemplate()->getFactNotHave().size();

    for ( u32 i = 0; i < notHaveFacts; i++ )
    {
        if ( m_tree->getBlackBoard().factExists(getTemplate()->getFactNotHave()[i]) )
        {
            return bfalse;
        }
    }

    return btrue;
}

IMPLEMENT_OBJECT_RTTI(BTDeciderHasFact_Template);

BEGIN_SERIALIZATION_CHILD(BTDeciderHasFact_Template)

    SERIALIZE_CONTAINER("factsHave",m_factHave);
    SERIALIZE_CONTAINER("factsNotHave",m_factNotHave);

END_SERIALIZATION()

}

