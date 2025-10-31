#ifndef _ITF_ACTORCREATE_BLENDTREE_H_
#define _ITF_ACTORCREATE_BLENDTREE_H_

#ifndef _ITF_ACTORCREATEPLUGIN_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreatePlugin.h"
#endif //_ITF_ACTORCREATEPLUGIN_H_

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

namespace ITF
{
    template <class RESULTTYPE>
    class ActorCreate_BlendTree
    {
    public:
        
        void                                getChildrenCountAndInit(BlendTreeNodeTemplate<RESULTTYPE>* _blendTreeNode, u32& nodeCount);
        BlendTreeNodeTemplate <RESULTTYPE>* getChildrenWithID(BlendTreeNodeTemplate<RESULTTYPE>* _blendTreeNode, i32 _id);
        i32                                 getChildrenNumId(BlendTreeNode<RESULTTYPE>* _root, BlendTreeNode<RESULTTYPE>* _blendTreeNode, u32& nodeCount);
        bbool                               addLinkNode(BlendTreeNodeTemplate <RESULTTYPE>* _currentNode, u32 _nodeParentNumId, BlendTreeNodeTemplate <RESULTTYPE>* _node);
        bbool                               removeChildrenNode(BlendTreeNodeTemplate <RESULTTYPE>* _currentNode, BlendTreeNodeTemplate <RESULTTYPE>* _removeNode, bbool _removeChildren = bfalse /*for the first function call*/);
        bbool                               movePositionChild(BlendTreeNodeTemplate <RESULTTYPE>* _currentNode, u32 _nodeNumId, u32 _newPosition);
        BlendTreeNodeTemplate <RESULTTYPE>* getNodeByNumId(BlendTreeNodeTemplate <RESULTTYPE>* _node, u32 _nodeNumId);
        BlendTreeNodeTemplate <RESULTTYPE>* removeLinkAndGetNodeRemoved(BlendTreeNodeTemplate <RESULTTYPE>* _currentNode, u32 _nodeNumId);
        void                                getChildren(Blob& _blob, BlendTreeNodeTemplate<RESULTTYPE>* _blendTreeNode);
        void                                getChildrenForLink(Blob& _blob, BlendTreeNodeTemplate<RESULTTYPE>* _blendTreeNode, i32 _idParent);
        void                                getBufferNode(BlendTreeNodeTemplate <RESULTTYPE>* _node, char** buffer);
        bbool                               setBufferNode(BlendTreeNodeTemplate <RESULTTYPE>* _node, String& buffer);
        bbool                               addChildrenNode(BlendTreeNodeTemplate<RESULTTYPE> * _parentNode, BlendTreeNodeTemplate<RESULTTYPE> * _newNode);
        bbool                               moveLinkNode(BlendTreeNodeTemplate <RESULTTYPE>* _rootNode, u32 _nodeNumId, u32 _nodeParentNumId);

        bbool                               addNode(BlendTreeNodeTemplate <RESULTTYPE>* _rootSelected, SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& _nodesList, BlendTreeNodeTemplate<RESULTTYPE>* _newNode, i32 _idParent);
        bbool                               removeNode(BlendTreeNodeTemplate <RESULTTYPE>* _rootSelected, SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& _nodesList, i32 _id);

        //void                                                                getChildrenCountAndInit(u32& nodeCount);
        //void                                                                getBufferNode(u32 nodeId, char** buffer);
        //bbool                                                               setBufferNode(u32 nodeId, String& buffer);
        //bbool                                                               movePositionChild(u32 _nodeNumId, u32 _positionChild);
        //bbool                                                               addNode(u32 parentNodeId, String className, String nameId);
        //bbool                                                               removeNode(u32 nodeId);
        //bbool                                                               moveLinkNode(u32 _nodeNumId, u32 _nodeParentNumId);

    };

    template <class RESULTTYPE>
    void ActorCreate_BlendTree<RESULTTYPE>::getChildrenCountAndInit(BlendTreeNodeTemplate<RESULTTYPE>* _blendTreeNode, u32& nodeCount)
    {
        nodeCount++;
        if(!_blendTreeNode)
        {
            ITF_ASSERT_MSG(0, "a node is null in anim tree!");
            return;
        }

        _blendTreeNode->setNumId(nodeCount);
        if(_blendTreeNode->getCapacity() > 0)
        {
            BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _blendTreeNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            { 
                SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* > nodes = ((SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&)nodeBlend->getLeafs());
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    getChildrenCountAndInit(nodes[iNode], nodeCount);
                }
            }
        }
    }

    template <class RESULTTYPE>
    BlendTreeNodeTemplate<RESULTTYPE>* ActorCreate_BlendTree<RESULTTYPE>::getChildrenWithID(BlendTreeNodeTemplate<RESULTTYPE>* _blendTreeNode, i32 _id)
    {
        if(_id == _blendTreeNode->getNumId())
            return _blendTreeNode;

        if(_blendTreeNode->getCapacity() > 0)
        {
            const BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _blendTreeNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            {
                const SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* > nodes = nodeBlend->->getLeafs();
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    BlendTreeNodeTemplate <RESULTTYPE>* node = nodes[iNode];
                    BlendTreeNodeTemplate <RESULTTYPE>* nodeFound = getChildrenWithID(node, _id);                         
                    if(nodeFound)
                        return nodeFound;
                }
            }
        }
        return NULL;
    }

    template <class RESULTTYPE>
    i32 ActorCreate_BlendTree<RESULTTYPE>::getChildrenNumId(BlendTreeNode<RESULTTYPE>* _root, BlendTreeNode<RESULTTYPE>* _blendTreeNode, u32& nodeCount)
    {
        nodeCount++;
        if(_root == _blendTreeNode)
            return nodeCount;

        if(_root->getTemplate()->getCapacity() > 0)
        {
            BlendTreeNodeBlend <RESULTTYPE>* nodeBlend = _root->template DynamicCast<BlendTreeNodeBlend <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend,628986251));
            if(nodeBlend)
            {
                SafeArray < BlendTreeNode <RESULTTYPE>* > nodes = nodeBlend->getLeafs();
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    i32 id = getChildrenNumId(nodes[iNode], _blendTreeNode, nodeCount);
                    if(id != -1)
                        return id;
                }
            }
        }
        return -1;
    }

    template <class RESULTTYPE>
    void ActorCreate_BlendTree<RESULTTYPE>::getChildren(Blob& _blob, BlendTreeNodeTemplate<RESULTTYPE>* _blendTreeNode)
    {
        _blob.pushInt32(_blendTreeNode->getNumId());
        _blob.pushString(_blendTreeNode->GetObjectClassName());  

        #ifdef ITF_DEBUG_STRINGID
        _blob.pushString(_blendTreeNode->getId().getDebugString());
        #else
        _blob.pushString("No StringID strings");
        #endif

        _blob.pushInt32(_blendTreeNode->getCapacity());



        if(_blendTreeNode->getCapacity() > 0)
        {
            const BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _blendTreeNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            {
                const SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* > nodes = nodeBlend->getLeafs();
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    getChildren(_blob, nodes[iNode]);
                }
            }
        }
    }

    template <class RESULTTYPE>
    void ActorCreate_BlendTree<RESULTTYPE>::getChildrenForLink(Blob& _blob, BlendTreeNodeTemplate<RESULTTYPE>* _blendTreeNode, i32 _idParent)
    {
        _blob.pushInt32(_blendTreeNode->getNumId());
        _blob.pushInt32(_idParent);

        if(_blendTreeNode->getCapacity() > 0)
        {
            const BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _blendTreeNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            {
                const SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* > nodes = nodeBlend->getLeafs();
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    BlendTreeNodeTemplate <RESULTTYPE>* node = nodes[iNode];
                    getChildrenForLink(_blob, node, _blendTreeNode->getNumId());                         
                }
            }
        }
    }

    template <class RESULTTYPE>
    BlendTreeNodeTemplate <RESULTTYPE>* ActorCreate_BlendTree<RESULTTYPE>::getNodeByNumId(BlendTreeNodeTemplate <RESULTTYPE>* _node, u32 _nodeNumId)
    {    
        if(_nodeNumId == _node->getNumId())
            return _node;

        if(_node->getCapacity() > 0)
        {
            BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _node->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            {
                SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& nodes = ((SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&) nodeBlend->getLeafs());
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    BlendTreeNodeTemplate <RESULTTYPE>* node = nodes[iNode];
                    BlendTreeNodeTemplate <RESULTTYPE>* nodeFound = getNodeByNumId(node, _nodeNumId);
                    if(nodeFound)
                        return nodeFound;
                }
            }
        }
        return NULL;
    }

    template <class RESULTTYPE>
    BlendTreeNodeTemplate <RESULTTYPE>* ActorCreate_BlendTree<RESULTTYPE>::removeLinkAndGetNodeRemoved(BlendTreeNodeTemplate <RESULTTYPE>* _currentNode, u32 _nodeNumId)
    {        
        if(_currentNode->getCapacity() > 0)
        {
            BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _currentNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            {
                SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& nodes = ((SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&) nodeBlend->getLeafs());
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    BlendTreeNodeTemplate <RESULTTYPE>* node = nodes[iNode];
                    if(node->getNumId() == _nodeNumId) 
                    {
                        nodes.eraseKeepOrder(iNode);                       
                        return node;
                    }

                    BlendTreeNodeTemplate <RESULTTYPE>* nodeResult = removeLinkAndGetNodeRemoved(node, _nodeNumId);
                    if(nodeResult)
                        return nodeResult;
                }
            }
        }
        return NULL;
    }


    template <class RESULTTYPE>
    bbool ActorCreate_BlendTree<RESULTTYPE>::addLinkNode(BlendTreeNodeTemplate <RESULTTYPE>* _currentNode, u32 _nodeParentNumId, BlendTreeNodeTemplate <RESULTTYPE>* _node)
    {

        if(_currentNode->getNumId() == _nodeParentNumId) 
        {
            if(_currentNode->getCapacity() > 0)
            {
                BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _currentNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
                if(nodeBlend)
                {
                    SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& nodes = ((SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&) nodeBlend->getLeafs());
                    nodes.push_back(_node);
                }
                return btrue;
            }
            else return bfalse;
        }

        if(_currentNode->getCapacity() > 0)
        {
            BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _currentNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            {
                SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& nodes = ((SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&) nodeBlend->getLeafs());
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    BlendTreeNodeTemplate <RESULTTYPE>* node = nodes[iNode];
                    bbool result = addLinkNode(node, _nodeParentNumId, _node);
                    if(result)
                        return btrue;
                }
            }
        }
        return bfalse;
    }

    template <class RESULTTYPE>
    bbool ActorCreate_BlendTree<RESULTTYPE>::movePositionChild(BlendTreeNodeTemplate <RESULTTYPE>* _currentNode, u32 _nodeNumId, u32 _newPosition)
    {        
        if(_currentNode->getCapacity() > 0)
        {
            BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _currentNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            {
                SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& nodes = ((SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&) nodeBlend->getLeafs());
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    BlendTreeNodeTemplate <RESULTTYPE>* node = nodes[iNode];
                    if(node->getNumId() == _nodeNumId) 
                    {
                        BlendTreeNodeTemplate <RESULTTYPE>* nodeToMove = node;
                        nodes.eraseKeepOrder(iNode);

                        nodes.insert(nodeToMove, _newPosition);                        
                        return btrue;
                    }

                    bbool result = movePositionChild(node, _nodeNumId, _newPosition);
                    if(result)
                        return btrue;
                }
            }
        }
        return bfalse;
    }

    template <class RESULTTYPE>
    bbool ActorCreate_BlendTree<RESULTTYPE>::removeChildrenNode(BlendTreeNodeTemplate <RESULTTYPE>* _parentNode, BlendTreeNodeTemplate <RESULTTYPE>* _removeNode, bbool _removeChildren /*=false for the first function call*/)
    {        
        i32 nodeToRemove = -1;

        if(_parentNode->getCapacity() > 0)
        {
            BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlend = _parentNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
            if(nodeBlend)
            {
                SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& nodes = ((SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&) nodeBlend->getLeafs());
                u32 count = nodes.size();
                for (u32 iNode = 0; iNode < count; iNode++)
                {
                    BlendTreeNodeTemplate <RESULTTYPE>* node = nodes[iNode];

                    // node to remove found
                    if(node == _removeNode)
                    {
                        nodeToRemove = (i32)iNode;

                        // remove all children
                        bbool result = removeChildrenNode(node, _removeNode, btrue); 
                        if(result)
                            return btrue;
                    }
                    else
                    {
                        bbool result = removeChildrenNode(node, _removeNode, _removeChildren);
                        if(result)
                            return btrue;
                    }
                }
                // node to remove no found but remove your children
                if(_removeChildren && nodeToRemove == -1)
                    nodes.clear();

                 // node to remove found and remove the node to its parent
                if(nodeToRemove != -1)
                {
                    SF_DEL(nodes[nodeToRemove]);
                    nodes.eraseKeepOrder(nodeToRemove);
                    return btrue;
                }
            }
        }
        return bfalse;
    }



    template <class RESULTTYPE>
    void ActorCreate_BlendTree<RESULTTYPE>::getBufferNode(BlendTreeNodeTemplate <RESULTTYPE>* _node, char** buffer)
    {       
        if(_node)
        {
            SerializerToolXML serializer;
            uSize size;
            serializer.OpenToWrite(buffer,&size);
            // serialize node
            _node->Serialize(&serializer,ESerialize_PropertyEdit_Save);
            serializer.Close();
        }

        ITF_ASSERT(_node);
    }

    template <class RESULTTYPE>
    bbool ActorCreate_BlendTree<RESULTTYPE>::setBufferNode(BlendTreeNodeTemplate <RESULTTYPE>* _node, String& buffer)
    {
        if(_node)
        {
            SerializerToolXML serializer;

            StringConverter converted(buffer);

            serializer.OpenToRead(converted.getChar());
            _node->Serialize(&serializer,ESerialize_PropertyEdit_Load);
            serializer.Close();
            return btrue;
        }
        ITF_ASSERT(_node);
        return bfalse;
    }


    template <class RESULTTYPE>
    bbool ActorCreate_BlendTree<RESULTTYPE>::addChildrenNode(BlendTreeNodeTemplate<RESULTTYPE> * _parentNode, BlendTreeNodeTemplate<RESULTTYPE> * _newNode)
    {
        if (!_newNode)
            return bfalse;

        if(_parentNode)
        {
            u32 capacity = _parentNode->getCapacity();
            if(capacity > 0)
            {
                BlendTreeNodeBlend_Template <RESULTTYPE>* nodeBlendParentFound = _parentNode->template DynamicCast<BlendTreeNodeBlend_Template <RESULTTYPE> >(ITF_GET_STRINGID_CRC(BlendTreeNodeBlend_Template,1110016475));
                if(nodeBlendParentFound)
                {
                    if(nodeBlendParentFound->getLeafs().size() < capacity)
                    {
                        ((SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >&) nodeBlendParentFound->getLeafs()).push_back(_newNode);
                        return btrue;
                    }
                    else
                    {
                        SF_DEL(_newNode);
                        return bfalse;
                    }
                }
            }
        }
        
        SF_DEL(_newNode);
        return bfalse;
    }

    template <class RESULTTYPE>
    bbool ActorCreate_BlendTree<RESULTTYPE>::moveLinkNode(BlendTreeNodeTemplate <RESULTTYPE>* _rootNode, u32 _nodeNumId, u32 _nodeParentNumId)
    {
        BlendTreeNodeTemplate <RESULTTYPE>* nodeRemoved = NULL;

        {// unlink node
            nodeRemoved = removeLinkAndGetNodeRemoved(_rootNode, _nodeNumId);
        }

        if(nodeRemoved)
        {// link node at a new parent node
            bbool rtn = addLinkNode(_rootNode, _nodeParentNumId, nodeRemoved);
            if(rtn) return btrue;
        }
        ITF_ASSERT_MSG(0, "error in node moved");

        return bfalse;
    }

    template <class RESULTTYPE>
    bbool ActorCreate_BlendTree<RESULTTYPE>::addNode(BlendTreeNodeTemplate <RESULTTYPE>* _rootSelected, SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& _nodesList, BlendTreeNodeTemplate<RESULTTYPE>* _newNode, i32 _idParent)
    {
        if (!_newNode)
            return false;

        bbool modified = bfalse;

        // adding node on root
        if(_idParent == 0)
        {
            _nodesList.push_back(_newNode);
            modified = btrue;            
        }
        // adding node on a parent node
        else
        {
            BlendTreeNodeTemplate <RESULTTYPE>* nodeParentFound = getNodeByNumId(_rootSelected, _idParent);
            modified = addChildrenNode(nodeParentFound, _newNode);
        }

        return modified;
    }

    template <class RESULTTYPE>
    bbool ActorCreate_BlendTree<RESULTTYPE>::removeNode(BlendTreeNodeTemplate <RESULTTYPE>* _rootSelected, SafeArray < BlendTreeNodeTemplate <RESULTTYPE>* >& _nodesList, i32 _id)
    {
        bbool modified = bfalse;

        // remove a node root
        if(_id == 1)
        {
            const u32 count = _nodesList.size();
            for (u32 i = 0; i < count; i++)   
            {
                if(_nodesList[i] == _rootSelected)
                {
                    removeChildrenNode(_rootSelected, getNodeByNumId(_rootSelected, (u32)_id));
                    SF_DEL(_nodesList[i]);
                    _nodesList.eraseKeepOrder(i);
                    modified = btrue;
                    break;
                }
            }
        }
        // remove a node child of root
        else
        {
            modified = removeChildrenNode(_rootSelected, getNodeByNumId(_rootSelected, (u32)_id));
        }

        return modified;
    }
}

#endif //_ITF_ACTORCREATE_BLENDTREE_H_
