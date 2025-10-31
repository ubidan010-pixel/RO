
#include "precompiled_gameplay.h"

#ifndef _ITF_BTDECIDER_H_
#include "gameplay/ai/BTs/BTDecider.h"
#endif // _ITF_BTDECIDER_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTDecider);

BTDecider::BTDecider()
: m_currentNode(U32_INVALID)
{
}

BTDecider::~BTDecider()
{
    u32 numNodes = m_nodes.size();

    for ( u32 i = 0; i < numNodes; i++ )
    {
        SF_DEL(m_nodes[i]);
    }
}

void BTDecider::init( class BehaviorTree* _tree, const class BTNode_Template* _template )
{
    Super::init(_tree,_template);

    u32 numNodes = getTemplate()->getNumNodes();

    m_nodes.resize(numNodes);

    for ( u32 i = 0; i < numNodes; i++ )
    {
        const BTNode_Template* nodeTemplate = _tree->getTemplate()->getNodeFromRef(getTemplate()->getNodeRef(i));

        m_nodes[i] = nodeTemplate ? nodeTemplate->createBTNode() : NULL;

        ITF_ASSERT_MSG(m_nodes[i],"Unable to create node");

        if ( m_nodes[i] )
        {
            m_nodes[i]->init(_tree,nodeTemplate);
        }
    }
}

bbool BTDecider::validate( Actor* _actor ) const
{
    u32 numNodes = m_nodes.size();

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
            ITF_WARNING_PARAM(_actor,"BTDecider",node!=NULL,"Behavior Decider has a NULL node");
            return bfalse;
        }
    }

    return btrue;
}

bbool BTDecider::decide( f32 _dt )
{
    Super::decide(_dt);

    return m_nodes.size() > 0;
}

void BTDecider::onActivate()
{
    Super::onActivate();

    m_currentNode = U32_INVALID;
}

void BTDecider::onDeactivate()
{
    Super::onDeactivate();

    setCurrentNode(U32_INVALID);

    u32 numFacts = getTemplate()->getClearFacts().size();

    for ( u32 i = 0; i < numFacts; i++ )
    {
        const StringID& fact = getTemplate()->getClearFacts()[i];

        if ( fact.isValid() )
        {
            m_tree->getBlackBoard().removeFact(fact);
        }
    }
}

bbool BTDecider::update( f32 _dt )
{
    Super::update(_dt);

    if ( getTemplate()->getReevaluate() && !decide(_dt) )
    {
        return bfalse;
    }

    u32 numNodes = m_nodes.size();
    u32 highPriority = m_currentNode == U32_INVALID ? numNodes : m_currentNode;

    // During this pass, if anyone decides yes, we automatically enter that node because it's more prioritary than the current
    for ( u32 i = 0; i < highPriority; i++ )
    {
        if ( m_nodes[i]->decide(_dt) )
        {
            setCurrentNode(i);
            break;
        }
    }

    if ( m_currentNode == U32_INVALID )
    {
        return bfalse;
    }

    // Update the current node and keep the return value
    bbool retVal = m_nodes[m_currentNode]->update(_dt);

    if ( !retVal )
    {
        u32 currentNode = m_currentNode;

        setCurrentNode(U32_INVALID);

        // If the current node doesn't want to continue, we can try to decide one with less priority
        for ( u32 i = currentNode+1; i < numNodes; i++ )
        {
            if ( m_nodes[i]->decide(_dt) )
            {
                setCurrentNode(i);
                retVal = btrue;
                break;
            }
        }
    }

    return retVal;
}

void BTDecider::updateAnimInput()
{
    Super::updateAnimInput();

    if ( m_currentNode < m_nodes.size() )
    {
        m_nodes[m_currentNode]->updateAnimInput();
    }
}

void BTDecider::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( m_currentNode < m_nodes.size() )
    {
        m_nodes[m_currentNode]->onEvent(_event);
    }
}

void BTDecider::setCurrentNode( u32 _nodeIndex )
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

#ifdef ITF_SUPPORT_EDITOR
void BTDecider::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const
{
    Super::drawEdit(drawInterface,_flags);

    if ( m_currentNode < m_nodes.size() )
    {
        m_nodes[m_currentNode]->drawEdit(drawInterface,_flags);
    }
}
#endif // ITF_SUPPORT_EDITOR

IMPLEMENT_OBJECT_RTTI(BTDecider_Template);

BEGIN_SERIALIZATION_CHILD(BTDecider_Template)

    SERIALIZE_CONTAINER_OBJECT("nodes",m_nodes);
    SERIALIZE_CONTAINER("clearFacts",m_clearFacts);
    SERIALIZE_MEMBER("reevaluate",m_reevaluate);

END_SERIALIZATION()

BTDecider_Template::~BTDecider_Template()
{
    u32 numNodes = m_nodes.size();

    for ( u32 i = 0; i < numNodes; i++ )
    {
        SF_DEL(m_nodes[i].m_refPtr);
    }
}

}
