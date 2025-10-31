
#ifndef _ITF_BLENDTREE_H_
#define _ITF_BLENDTREE_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

#ifndef _ITF_BLENDTREETRANSITION_H_
#include "engine/blendTree/BlendTreeTransition.h"
#endif //_ITF_BLENDTREETRANSITION_H_

#ifndef _ITF_BLENDTREENODEADDBRANCH_H_
#include "engine/blendTree/BlendTreeNodeAddBranch.h"
#endif //_ITF_BLENDTREENODEADDBRANCH_H_

#ifndef _ITF_BLENDTREENODEBLENDBRANCHES_H_
#include "engine/blendTree/BlendTreeNodeBlendBranches.h"
#endif //_ITF_BLENDTREENODEBLENDBRANCHES_H_

#ifndef _ITF_BLENDTREENODECHOOSEBRANCH_H_
#include "engine/blendTree/BlendTreeNodeChooseBranch.h"
#endif //_ITF_BLENDTREENODECHOOSEBRANCH_H_

#ifndef _ITF_BLENDTREENODEBRANCHTRANSITION_H_
#include "engine/blendTree/BlendTreeNodeBranchTransition.h"
#endif //_ITF_BLENDTREENODEBRANCHTRANSITION_H_

namespace ITF
{

template <class RESULTTYPE>
class BlendTree
{
public:

    DECLARE_NODE_LIST

    BlendTree();
    ~BlendTree() { clear(); }

    virtual void                                    update( f32 _dt, const InputContainer& _inputs );
    virtual bbool                                   validate() const;
    
    void                                            init( BlendTreeInitDataBase* _initData, const BlendTreeTemplate <RESULTTYPE>* _template );
    void                                            onLoadResource( BlendTreeLoadResourceBase* _loadResData );
    void                                            clear();
    void                                            playNode( const StringID& _node );
    void                                            reset() { m_resetTree = btrue; }
    void                                            resetTransition() { m_resetTransition = btrue; }

    bbool                                           isInTransition() const { return m_currentTransition != NULL; }
    bbool                                           isValid() const { return m_nodesList.size() > 0; }
    RESULTTYPE*                                     getResult();
    u32                                             getBlendFramesResult() const { return m_blendFramesResult; }

    u32                                             getNumNodes() const { return m_nodesList.size(); }
    const StringID&                                 getNodeID( u32 _index ) const;

    ITF_INLINE const class BlendTreeTemplate <RESULTTYPE>*       getTemplate() const;

protected:

    BlendTreeNode <RESULTTYPE>*                     getNode( const StringID& _id ) const;
    void                                            findTransitions( f32 _dt, const InputContainer& _inputs, const NodeList& _prevNodes );

    virtual void                                    setTransition( BlendTreeTransition <RESULTTYPE>* _transition );
    virtual void                                    updateTransitions( f32 _dt, const InputContainer& _inputs );

    const BlendTreeTemplate <RESULTTYPE>*           m_template;
    SafeArray < BlendTreeNode <RESULTTYPE>* >       m_nodesList;
    ITF_MAP<StringID,BlendTreeNode <RESULTTYPE>* >  m_namedNodes;
    SafeArray < BlendTreeTransition <RESULTTYPE>* > m_nodeTransitions;
    RESULTTYPE                                      m_currentBlendResult;
    NodeList                                        m_currentPlayingNodes;
    BlendTreeNode <RESULTTYPE>*                     m_currentNode;
    BlendTreeTransition <RESULTTYPE>*               m_previousTransition;
    BlendTreeTransition <RESULTTYPE>*               m_currentTransition;
    StringID                                        m_playNode;
    u32                                             m_blendFramesResult;
    bbool                                           m_resetTree;
    bbool                                           m_resetTransition;
};

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
class BlendTreeTemplate : public TemplateObj
{
    DECLARE_SERIALIZE_VIRTUAL();

public:

    BlendTreeTemplate();
    ~BlendTreeTemplate();

    const SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&            getNodeList() const { return m_nodesList; }
    SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&                  getEditableNodeList() { return m_nodesList; }
    const SafeArray < BlendTreeTransition_Template <RESULTTYPE>* >&     getNodeTransition() const { return m_nodeTransitions; }
    const ObjectFactory*                                                getFactory() const { return m_factory; }
    const u32                                                           getNumNodes() const { return m_nodesList.size(); }

protected:

    SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >           m_nodesList;
    SafeArray < BlendTreeTransition_Template <RESULTTYPE>* >    m_nodeTransitions;
    ObjectFactory*                                              m_serializerFactory;
    ObjectFactory*                                              m_factory;
};

BEGIN_SERIALIZATION_TEMPLATE_ABSTRACT(BlendTreeTemplate)

    m_serializerFactory = serializer->getFactory();
    serializer->setFactory(m_factory);
    SERIALIZE_CONTAINER_WITH_FACTORY("nodes",m_nodesList,m_factory);
    SERIALIZE_CONTAINER_OBJECT("nodeTransitions",m_nodeTransitions);
    serializer->setFactory(m_serializerFactory);

END_SERIALIZATION()

template <class RESULTTYPE>
BlendTreeTemplate<RESULTTYPE>::BlendTreeTemplate()
: m_serializerFactory(NULL)
, m_factory(NULL)
{
}

template <class RESULTTYPE>
BlendTreeTemplate<RESULTTYPE>::~BlendTreeTemplate()
{
    u32 nodesSize = m_nodesList.size();
    for ( u32 i = 0; i < nodesSize; i++ )
    {
        delete m_nodesList[i];
    }

    // delete all transitions
    u32 numTransitions = m_nodeTransitions.size();

    for ( u32 i = 0; i < numTransitions; i++ )
    {
        delete m_nodeTransitions[i];
    }
}

template <class RESULTTYPE>
const BlendTreeTemplate <RESULTTYPE>* BlendTree<RESULTTYPE>::getTemplate() const
{
    return static_cast<const BlendTreeTemplate <RESULTTYPE>*>(m_template);
}

////////////////////////////////////////////////////////////////////////////////////

template <class RESULTTYPE>
BlendTree<RESULTTYPE>::BlendTree()
: m_template(NULL)
, m_currentNode(NULL)
, m_previousTransition(NULL)
, m_currentTransition(NULL)
, m_blendFramesResult(0)
, m_resetTree(bfalse)
, m_resetTransition(bfalse)
{
}

template <class RESULTTYPE>
void BlendTree<RESULTTYPE>::init( BlendTreeInitDataBase* _initData, const BlendTreeTemplate <RESULTTYPE>* _template )
{
    m_template = _template;

    const SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& template_nodeList = _template->getNodeList();
    const SafeArray < BlendTreeTransition_Template <RESULTTYPE>* >& template_transitions = _template->getNodeTransition();
    BlendTreeInitData <RESULTTYPE>* treeInitData = static_cast<BlendTreeInitData <RESULTTYPE>*>(_initData);

    u32 index = 0;
    u32 numNodes = template_nodeList.size();
    
    treeInitData->m_namedNodes = &m_namedNodes;    

    m_nodesList.resize(numNodes);

    while ( index < numNodes )
    {
        BlendTreeNodeTemplate <RESULTTYPE>* nodeTemplate = template_nodeList[index];
        
        ITF_ASSERT(nodeTemplate->getId().isValid());

        if ( !nodeTemplate->getId().isValid() )
        {
            numNodes--;
            m_nodesList.resize(numNodes); // avoids crash on destruction on map reload
        }
        else
        {
            BlendTreeNode <RESULTTYPE>* node = nodeTemplate->createNode();
            m_nodesList[index] = node;
            node->init(_initData,nodeTemplate);
            index++;
        }
    }

    u32 numTransitions = template_transitions.size();

    m_nodeTransitions.resize(numTransitions);

    for ( u32 i = 0; i < numTransitions; i++ )
    {
        BlendTreeTransition <RESULTTYPE>* transition = newAlloc(mId_Animation,BlendTreeTransition <RESULTTYPE>);

        m_nodeTransitions[i] = transition;
        transition->init(_initData,template_transitions[i]);
    }
}

template <class RESULTTYPE>
void BlendTree<RESULTTYPE>::onLoadResource( BlendTreeLoadResourceBase* _loadResData )
{
    u32 numNodes = m_nodesList.size();

    for ( u32 index = 0; index < numNodes; index++ )
    {
        BlendTreeNode <RESULTTYPE>* node = m_nodesList[index];

        node->onLoadResource(_loadResData);
    }

    u32 numTransitions = m_nodeTransitions.size();

    for ( u32 i = 0; i < numTransitions; i++ )
    {
        BlendTreeTransition <RESULTTYPE>* transition = m_nodeTransitions[i];

        transition->onLoadResource(_loadResData);
    }
}

template <class RESULTTYPE>
void BlendTree<RESULTTYPE>::clear()
{
    u32 nodesSize = m_nodesList.size();
    for ( u32 i = 0; i < nodesSize; i++ )
    {
        delete m_nodesList[i];
    }

    m_nodesList.clear();
    m_namedNodes.clear();

    // delete all transitions
    u32 numTransitions = m_nodeTransitions.size();

    for ( u32 i = 0; i < numTransitions; i++ )
    {
        delete m_nodeTransitions[i];
    }

    m_nodeTransitions.clear();

    m_currentNode = NULL;
    m_previousTransition = NULL;
    m_currentTransition = NULL;
    m_currentBlendResult.clear();
    m_currentPlayingNodes.clear();
}

template <class RESULTTYPE>
void BlendTree<RESULTTYPE>::playNode( const StringID& _node )
{
    m_playNode = _node;

    if ( m_currentNode && m_currentNode->getId() != _node )
    {
        m_resetTree = btrue;
    }
}

template <class RESULTTYPE>
BlendTreeNode <RESULTTYPE>* BlendTree<RESULTTYPE>::getNode( const StringID& _nodeName) const 
{ 
    if (_nodeName.isValid())
    {
        typename ITF_MAP<StringID,BlendTreeNode <RESULTTYPE>* >::const_iterator it = m_namedNodes.find(_nodeName);
        if (it != m_namedNodes.end())
        {
            return it->second;
        }
    }
    return NULL;
}

template <class RESULTTYPE>
RESULTTYPE* BlendTree<RESULTTYPE>::getResult()
{
    return &m_currentBlendResult;
}

template <class RESULTTYPE>
void BlendTree<RESULTTYPE>::update( f32 _dt, const InputContainer& _inputs )
{
    if ( m_playNode.isValid() )
    {
        BlendTreeNode <RESULTTYPE>* newNode = getNode(m_playNode);

        ITF_ASSERT(newNode);

        m_playNode.invalidate();

        if ( newNode != m_currentNode && newNode )
        {
            m_currentNode = newNode;

            if ( m_currentNode )
            {
                m_currentNode->onBecomeActive(_inputs);
            }

            setTransition(NULL);
        }
        else if ( m_resetTree && m_currentNode )
        {
            m_currentNode->onBecomeActive(_inputs);
        }
    }
    else if ( m_resetTree )
    {

        if ( m_currentNode )
        {
            m_currentNode->onBecomeActive(_inputs);
        }
        else
        {
            const char * text = "Crash detected in blendtree with currentnode null, please investigate";
            ITF_UNUSED(text);
            ITF_ASSERT_MSG(0, text);
            ITF_WARNING_CATEGORY(GPP,NULL,NULL, text);
        }
    }

    if ( !m_currentNode )
    {
        return;
    }

    m_resetTree = bfalse;
    m_blendFramesResult = U32_INVALID;
    m_previousTransition = m_currentTransition;
    
    NodeList prevPlayingNodes = m_currentPlayingNodes;

    m_currentPlayingNodes.clear();

    m_currentNode->getPlayingNodes(_inputs,m_currentPlayingNodes);

    if ( m_resetTransition )
    {
        m_currentTransition = NULL;
        m_previousTransition = NULL;
        m_resetTransition = bfalse;
    }
    else
    {
        findTransitions(_dt,_inputs,prevPlayingNodes);

        updateTransitions(_dt,_inputs);
    }

    if ( !m_currentTransition )
    {
        m_currentBlendResult.clear();
        m_currentNode->updateResult(_dt,_inputs,&m_currentBlendResult);
    }
    
}

template <class RESULTTYPE>
bbool BlendTree<RESULTTYPE>::validate() const
{
    u32 nodesSize = m_nodesList.size();
    for ( u32 i = 0; i < nodesSize; i++ )
    {
        if ( !m_nodesList[i]->validate() )
        {
            return bfalse;
        }
    }

    u32 numTransitions = m_nodeTransitions.size();
    for ( u32 i = 0; i < numTransitions; i++ )
    {
        if ( !m_nodeTransitions[i]->validate() )
        {
            return bfalse;
        }
    }

    return btrue;
}

template <class RESULTTYPE>
void BlendTree<RESULTTYPE>::findTransitions( f32 _dt, const InputContainer& _inputs, const NodeList& _prevNodes )
{
    if ( _prevNodes.size() > 0 )
    {
        u32 numCurrentNodes = m_currentPlayingNodes.size();
        u32 numPrevNodes = _prevNodes.size();
        bbool done = bfalse;

        for ( u32 currentNodeIndex = 0; currentNodeIndex < numCurrentNodes; currentNodeIndex++ )
        {
            BlendTreeNode <RESULTTYPE>* playingNode = m_currentPlayingNodes[currentNodeIndex];

            for ( u32 previousNodeIndex = 0; previousNodeIndex < numPrevNodes; previousNodeIndex++ )
            {
                BlendTreeNode <RESULTTYPE>* prevNode = _prevNodes[previousNodeIndex];

                if ( prevNode == playingNode )
                {
                    // This node is still playing, so we can stop searching
                    done = btrue;
                    break;
                }
            }

            if ( done )
            {
                break;
            }

            // This node wasn't playing before, try to find a transition
            const SafeArray < BlendTreeTransition <RESULTTYPE>* >& toTransitions = playingNode->getTransitionTo();
            u32 numToTransitions = toTransitions.size();

            if ( m_currentTransition )
            {
                // First lets check if the transition we were currently playing could work for this animation too
                u32 numToTransitions = toTransitions.size();

                for ( u32 toIndex = 0; toIndex < numToTransitions; toIndex++ )
                {
                    BlendTreeTransition <RESULTTYPE>* toTransition = toTransitions[toIndex];

                    if ( toTransition == m_currentTransition )
                    {
                        done = btrue;
                        break;
                    }
                }

                if ( done )
                {
                    break;
                }

                // If it doesn't work we need to remove the transition and find a new one
                setTransition(NULL);
            }

            m_blendFramesResult = 0;

            // Find the transition that contains a node from the previous playing nodes list
            for ( u32 toIndex = 0; toIndex < numToTransitions; toIndex++ )
            {
                BlendTreeTransition <RESULTTYPE>* toTransition = toTransitions[toIndex];
                bbool discard = bfalse;

                for ( u32 previousNodeIndex = 0; previousNodeIndex < numPrevNodes; previousNodeIndex++ )
                {
                    BlendTreeNode <RESULTTYPE>* prevNode = _prevNodes[previousNodeIndex];
                    const SafeArray < BlendTreeTransition <RESULTTYPE>* >& prevToTransitions = prevNode->getTransitionTo();
                    u32 numPrevToTransitions = prevToTransitions.size();

                    for ( u32 prevToIndex = 0; prevToIndex < numPrevToTransitions; prevToIndex++ )
                    {
                        BlendTreeTransition <RESULTTYPE>* prevToTransition = prevToTransitions[prevToIndex];

                        if ( toTransition == prevToTransition )
                        {
                            // If this transition could be used to be to go to a node that was playing previously
                            // then we discard it, because it means they fall under the same group of TO so it
                            // means there is no change
                            discard = btrue;
                            break;
                        }
                    }

                    if ( discard )
                    {
                        break;
                    }

                    const SafeArray < BlendTreeTransition <RESULTTYPE>* >& prevFromTransitions = prevNode->getTransitionFrom();
                    u32 numPrevFromTransitions = prevFromTransitions.size();

                    for ( u32 prevFromIndex = 0; prevFromIndex < numPrevFromTransitions; prevFromIndex++ )
                    {
                        BlendTreeTransition <RESULTTYPE>* fromTransition = prevFromTransitions[prevFromIndex];

                        if ( toTransition == fromTransition )
                        {
                            // We found a transition
                            setTransition(toTransition);
                            done = btrue;
                            break;
                        }
                    }

                    if ( done )
                    {
                        break;
                    }
                }

                if ( done )
                {
                    break;
                }

                if ( discard )
                {
                    continue;
                }

                if ( !toTransition->hasFrom() )
                {
                    setTransition(toTransition);
                    done = btrue;
                    break;
                }
            }

            if ( done )
            {
                break;
            }
        }
    }
}

template <class RESULTTYPE>
void BlendTree<RESULTTYPE>::setTransition( BlendTreeTransition<RESULTTYPE>* _transition )
{
    if ( m_currentTransition != _transition )
    {
        m_currentTransition = _transition;

        if ( m_currentTransition && m_currentNode )
        {
            m_currentNode->onTransitionPlay();
        }
    }
}

template <class RESULTTYPE>
void BlendTree<RESULTTYPE>::updateTransitions( f32 _dt, const InputContainer& _inputs )
{
    if ( m_currentTransition )
    {
        if ( m_previousTransition != m_currentTransition )
        {
            if ( m_currentTransition->getBlend() )
            {
                m_blendFramesResult = m_currentTransition->getBlend();
            }
            else if ( m_previousTransition && m_currentTransition->getBlendFromTransition() )
            {
                m_blendFramesResult = m_currentTransition->getBlendFromTransition();
            }
            else
            {
                m_blendFramesResult = 0;
            }

            if ( m_currentTransition->getNode() )
            {
                m_currentTransition->getNode()->onBecomeActive(_inputs);
            }
            else
            {
                setTransition(NULL);
            }
        }

        if ( m_currentTransition )
        {
            m_currentBlendResult.clear();
            m_currentTransition->getNode()->updateResult(_dt,_inputs,&m_currentBlendResult);
        }
    }
    else if ( m_previousTransition != m_currentTransition )
    {
        m_blendFramesResult = 0;
    }
}

template <class RESULTTYPE>
const StringID& BlendTree<RESULTTYPE>::getNodeID( u32 _index ) const
{
    ITF_ASSERT(_index<m_nodesList.size());

    if ( _index < m_nodesList.size() )
    {
        BlendTreeNode <RESULTTYPE>* node = m_nodesList[_index];

        return node->getId();
    }

    return StringID::Invalid;
}


    

}

#endif // _ITF_BLENDTREE_H_

