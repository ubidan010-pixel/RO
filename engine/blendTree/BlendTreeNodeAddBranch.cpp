
#include "precompiled_engine.h"

#ifndef _ITF_BLENDTREENODEADDBRANCH_H_
#include "engine/blendTree/BlendTreeNodeAddBranch.h"
#endif //_ITF_BLENDTREENODEADDBRANCH_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI_TEMPLATE(BlendTreeNodeAddBranch)
IMPLEMENT_OBJECT_RTTI_TEMPLATE(BlendTreeNodeAddBranch_Template)

BEGIN_SERIALIZATION(BlendTreeBranchWeight)
    SERIALIZE_MEMBER("weight",m_weight);
    SERIALIZE_OBJECT("proceduralWeight",m_proceduralWeight);
END_SERIALIZATION()

}



