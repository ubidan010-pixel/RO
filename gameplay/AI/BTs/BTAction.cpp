
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTAction);

BTAction::BTAction()
{
}

IMPLEMENT_OBJECT_RTTI(BTAction_Template);

BEGIN_SERIALIZATION_CHILD(BTAction_Template)
END_SERIALIZATION()

}