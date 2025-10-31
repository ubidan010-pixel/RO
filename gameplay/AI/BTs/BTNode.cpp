
#include "precompiled_gameplay.h"

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

namespace ITF
{

BTNodeTemplate_Ref::BTNodeTemplate_Ref()
: m_refPtr(NULL)
{
}

BEGIN_SERIALIZATION(BTNodeTemplate_Ref)

    SERIALIZE_MEMBER("nameId",m_refId);
    SERIALIZE_OBJECT_WITH_FACTORY("node",m_refPtr,TEMPLATEDATABASE->getBTNodesFactory());

END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(BTNode);

BTNode::BTNode()
: m_template(NULL)
, m_tree(NULL)
{
}

void BTNode::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    m_template = _template;
    m_tree = _tree;
}

IMPLEMENT_OBJECT_RTTI(BTNode_Template);

BEGIN_SERIALIZATION_ABSTRACT(BTNode_Template)

    SERIALIZE_MEMBER("name",m_nameId);

END_SERIALIZATION()

}