
#include "precompiled_gameplay.h"

#ifndef _ITF_BTSEQUENCE_H_
#include "gameplay/ai/BTs/BTSequence.h"
#endif // _ITF_BTSEQUENCE_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTSequence);

BTSequence::BTSequence()
: m_currentNode(U32_INVALID)
{
}

BTSequence::~BTSequence()
{
    u32 numNodes = m_nodes.size();

    for ( u32 i = 0; i < numNodes; i++ )
    {
        SF_DEL(m_nodes[i]);
    }
}

void BTSequence::init( class BehaviorTree* _tree, const class BTNode_Template* _template )
{
    Super::init(_tree,_template);

    u32 numNodes = getTemplate()->getNumNodes();

    m_nodes.resize(numNodes);

    for ( u32 i = 0; i < numNodes; i++ )
    {
        const BTNode_Template* nodeTemplate = _tree->getTemplate()->getNodeFromRef(getTemplate()->getNodeRef(i));

        m_nodes[i] = nodeTemplate ? nodeTemplate->createBTNode() : NULL;

        if ( m_nodes[i] )
        {
            m_nodes[i]->init(_tree,nodeTemplate);
        }
    }
}

bbool BTSequence::validate( Actor* _actor ) const
{
    u32 numNodes = m_nodes.size();

    if ( numNodes == 0 )
    {
        ITF_WARNING_PARAM(_actor,"BTSequence",numNodes>0,"Behavior Sequence has no nodes");
        return bfalse;
    }

    for ( u32 i = 0; i < numNodes; i++ )
    {
        BTNode* node = m_nodes[i];

        if ( node )
        {
            bbool valid = node->validate(_actor);

            if ( !valid )
            {
                return bfalse;
            }
        }
        else
        {
            ITF_WARNING_PARAM(_actor,"BTSequence",node!=NULL,"Behavior Sequence has a NULL node");
            return bfalse;
        }
    }

    return btrue;
}

void BTSequence::onActivate()
{
    Super::onActivate();

    setCurrentNode(U32_INVALID);
}

void BTSequence::onDeactivate()
{
    Super::onDeactivate();

    setCurrentNode(U32_INVALID);
}

bbool BTSequence::update( f32 _dt )
{
    Super::update(_dt);

    u32 numNodes = m_nodes.size();
    bbool retVal = bfalse;

    if ( m_currentNode != U32_INVALID )
    {
        if ( m_currentNode > numNodes )
        {
            return bfalse;
        }

        retVal = m_nodes[m_currentNode]->update(_dt);
    }

    while ( !retVal )
    {
        // go to the next node
        u32 newNode;
        
        if ( m_currentNode == U32_INVALID )
        {
            newNode = 0;
        }
        else
        {
            newNode = m_currentNode + 1;
        }

        while ( newNode < numNodes )
        {
            if ( m_nodes[newNode]->decide(_dt) )
            {
                break;
            }
            else
            {
                newNode++;
            }
        }

        if ( newNode < numNodes )
        {
            setCurrentNode(newNode);
            retVal = m_nodes[newNode]->update(_dt);
        }
        else
        {
            m_nodes[m_currentNode]->onDeactivate();
            m_currentNode = numNodes;
            break;
        }
    }

    return retVal;
}

void BTSequence::setCurrentNode( u32 _nodeIndex )
{
    u32 numNodes = m_nodes.size();

    if ( m_currentNode < numNodes )
    {
        m_nodes[m_currentNode]->onDeactivate();
    }

    m_currentNode = _nodeIndex;

    if ( m_currentNode < numNodes )
    {
        m_nodes[m_currentNode]->onActivate();
    }
}

void BTSequence::updateAnimInput()
{
    Super::updateAnimInput();

    if ( m_currentNode < m_nodes.size() )
    {
        m_nodes[m_currentNode]->updateAnimInput();
    }
}

void BTSequence::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( m_currentNode < m_nodes.size() )
    {
        m_nodes[m_currentNode]->onEvent(_event);
    }
}

#ifdef ITF_SUPPORT_EDITOR
void BTSequence::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const
{
    Super::drawEdit(drawInterface,_flags);

    if ( m_currentNode < m_nodes.size() )
    {
        m_nodes[m_currentNode]->drawEdit(drawInterface,_flags);
    }
}
#endif // ITF_SUPPORT_EDITOR

IMPLEMENT_OBJECT_RTTI(BTSequence_Template);

BEGIN_SERIALIZATION_CHILD(BTSequence_Template)

    SERIALIZE_CONTAINER_OBJECT("nodes",m_nodes);

END_SERIALIZATION()

BTSequence_Template::~BTSequence_Template()
{
    u32 numNodes = m_nodes.size();

    for ( u32 i = 0; i < numNodes; i++ )
    {
        SF_DEL(m_nodes[i].m_refPtr);
    }
}

}
