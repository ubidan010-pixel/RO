
#include "precompiled_gameplay.h"

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

namespace ITF
{

BehaviorTree::BehaviorTree()
: m_root(NULL)
, m_template(NULL)
, m_actor(NULL)
{
}

BehaviorTree::~BehaviorTree()
{
    SF_DEL(m_root);
}

void BehaviorTree::init( const BehaviorTree_Template* _template, Actor* _actor )
{
    if ( m_root )
    {
        SF_DEL(m_root);
    }

    m_blackboard.clear();
    m_template = _template;
    m_actor = _actor;

    if ( m_template )
    {
        const BTNode_Template* root_template = m_template->getNodeFromRef(m_template->getRoot());

        if ( root_template )
        {
            m_root = root_template->createBTNode();

            if ( m_root )
            {
                m_root->init(this,root_template);
            }
        }
    }
}

bbool BehaviorTree::validate() const
{
    if ( m_root )
    {
        return m_root->validate(m_actor);
    }
    else
    {
        ITF_WARNING_PARAM(m_actor,"root",m_root!=NULL,"Behavior Tree doesn't have a root");
        return bfalse;
    }
}

void BehaviorTree::update( f32 _dt )
{
    m_root->update(_dt);
}

void BehaviorTree::updateAnimInput()
{
    m_root->updateAnimInput();
}

void BehaviorTree::onEvent(Event * _event)
{
    m_root->onEvent(_event);
}

#ifdef ITF_SUPPORT_EDITOR
void BehaviorTree::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const
{
    m_root->drawEdit(drawInterface,_flags);
}
#endif // ITF_SUPPORT_EDITOR

BEGIN_SERIALIZATION(BehaviorTree_Template)

    SERIALIZE_OBJECT("root",m_root);
    SERIALIZE_CONTAINER_WITH_FACTORY("nodes",m_nodes,TEMPLATEDATABASE->getBTNodesFactory());

END_SERIALIZATION()

BehaviorTree_Template::BehaviorTree_Template()
{
}

BehaviorTree_Template::~BehaviorTree_Template()
{
    SF_DEL(m_root.m_refPtr);

	u32 numNodes = m_nodes.size();

	for ( u32 i = 0; i < numNodes; i++ )
	{
		SF_DEL(m_nodes[i]);
	}
}

const BTNode_Template* BehaviorTree_Template::getNodeFromRef( const BTNodeTemplate_Ref& _ref ) const
{
    if ( _ref.m_refPtr )
    {
        return _ref.m_refPtr;
    }
    else
    {
        return findNode(_ref.m_refId);
    }
}

const BTNode_Template* BehaviorTree_Template::findNode( const StringID& _id ) const
{
    u32 numNodes = m_nodes.size();

    for ( u32 i = 0; i < numNodes; i++ )
    {
        const BTNode_Template* node = m_nodes[i];

        if ( node && node->getNameId() == _id )
        {
            return node;
        }
    }

    return NULL;
}

}