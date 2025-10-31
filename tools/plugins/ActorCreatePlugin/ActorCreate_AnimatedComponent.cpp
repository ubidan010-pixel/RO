// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)


#ifndef _ITF_ACTORCREATE_ANIMATEDCOMPONENT_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreate_AnimatedComponent.h"
#endif //_ITF_ACTORCREATE_ANIMATEDCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

#ifndef _ITF_ACTORCREATE_BLENDTREE_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreate_BlendTree.h"
#endif //_ITF_ACTORCREATE_BLENDTREE_H_

namespace ITF
{
    void ActorCreate_AnimatedComponent::ReceiveMsg(class Blob& _blob)
    {
        String message;
        _blob.extractString(message);

        if ( message == getRecvGetSubAnimList() )
        {
            onGetSubAnimList(_blob);
        }        
        else if ( message == getRecvGetInputList() )
        {
            onGetInputList(_blob);
        }
        else if ( message == getRecvGetListOfAnimRoots() )
        {
            onGetListOfNodes(_blob);
        }
        else if ( message == getRecvGetTree() )
        {
            onGetTree(_blob);
        }
        else if ( message == getRecvGetNodeData() )
        {
            onGetNodeData(_blob);
        }
        else if ( message == getRecvGetListOfClass() )
        {
            onGetListOfClass(_blob);
        }
        // animations
        else if ( message == getRecvAddNodeInAnimTree() )
        {
            onAddNodeInAnimTree(_blob);
        }
        else if ( message == getRecvRemoveNodeInAnimTree() )
        {
            onRemoveNodeInAnimTree(_blob);
        }
        // transitions
        else if ( message == getRecvAddNodeInTransitionTree() )
        {
            onAddNodeInTransitionTree(_blob);
        }
        else if ( message == getRecvRemoveNodeInTransitionTree() )
        {
            onRemoveNodeInTransitionTree(_blob);
        }
        else if ( message == getRecvUpdateNodeData() )
        {
            onUpdateNodeData(_blob);
        }
        else if( message == getRecvGetPlayingNodesCurrent())
        {
            onPlayingNodesCurrent(_blob);
        }
        else if ( message == getRecvMovePositionChild() )
        {
            onMovePositionChild(_blob);
        }
        else if ( message == getRecvMoveLinkNode() )
        {
            onMoveLinkNode(_blob);
        }
        else if( message == getRecvSetCurrentNodeInAnimTree() )
        {
            onSetCurrentNodeInAnimTree(_blob);
        }
        else if ( message == getRecvSetCurrentNodeInTranstionTree() )
        {
            onSetCurrentNodeInTranstionTree(_blob);
        }
        else if ( message == getRecvGetListOfTransitionsRoots() )
        {
            onGetListOfTransitionsRoots(_blob);
        }
        else if ( message == getRecvGetListOfTransitionsInfos() )
        {
            sendListOfTransitionsInfos();
        }
        else if ( message == getRecvGetTransitionData() )
        {
            u32 indexRoot = _blob.extractUint32();
            sendTransitionsData(indexRoot);
        }
        else if ( message == getRecvUpdateTransitionData() )
        {
            updateTransitionData(_blob);
        }
        else if ( message == getRecvAddTransition() )
        {
            addTransition();
        }
        else if ( message == getRecvRemoveTransition() )
        {
            removeTransition(_blob);
        }
        else if ( message == getRecvGetAnimData() )
        {
            u32 index = _blob.extractUint32();
            sendAnimData(index);
        }
        else if ( message == getRecvUpdateAnimData() )
        {
            updateAnimData(_blob);
        }
        else if ( message == getRecvGetInputData() )
        {
            u32 index = _blob.extractUint32();
            sendInputData(index);
        }
        else if ( message == getRecvUpdateInputData() )
        {
            updateInputData(_blob);
        }

    }


    void ActorCreate_AnimatedComponent::onSetCurrentNodeInAnimTree(class Blob& _blob)
    {
        u32 index = 0;
        index = _blob.extractUint32();

        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        u32 count = animTreeTemplate->getNodeList().size();
        if(index < count)
        {
            m_currentNode = (BlendTreeNodeTemplate <AnimTreeResult>*)animTreeTemplate->getNodeList()[index];
        }
    }


    void ActorCreate_AnimatedComponent::onSetCurrentNodeInTranstionTree(class Blob& _blob)
    {
        u32 index = 0;
        index = _blob.extractUint32();

        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;
        
        u32 count = animTreeTemplate->getNodeTransition().size();
        if(index < count)
        {
            m_currentNode = (BlendTreeNodeTemplate <AnimTreeResult>*)animTreeTemplate->getNodeTransition()[index]->getNode();
        }
    }

    void ActorCreate_AnimatedComponent::fillHeader(class Blob& _blob)
    {
        _blob.pushString(ActorCreatePlugin::getPluginName());  // plugin ID
        _blob.pushString(GetSpViewMessage());
        _blob.pushString(GetComponentName());
    }

    /************************************************************************/
    /* Action                                                               */
    /************************************************************************/

    void ActorCreate_AnimatedComponent::onGetSubAnimList(class Blob& _blob)
    {
        sendSubAnimList();
    }

    void ActorCreate_AnimatedComponent::onGetInputList(class Blob& _blob)
    {
        sendInputList();
    }

    void ActorCreate_AnimatedComponent::onGetListOfNodes(class Blob& _blob)
    {
        sendListOfAnimRoot();
    }

    void ActorCreate_AnimatedComponent::onGetTree(class Blob& _blob)
    {
        sendTree();
    }
    
    void ActorCreate_AnimatedComponent::onGetNodeData(class Blob& _blob)
    {
        u32 nodeId = (u32)_blob.extractInt32();        
        sendNodeData(nodeId);
    }

    void ActorCreate_AnimatedComponent::onGetListOfClass(class Blob& _blob)
    {
        sendListOfClass();
    }

    void ActorCreate_AnimatedComponent::onAddNodeInAnimTree(class Blob& _blob)
    {
        addNodeInAnimTree(_blob);
    }

    void ActorCreate_AnimatedComponent::onRemoveNodeInAnimTree(class Blob& _blob)
    {
        removeNodeInAnimTree(_blob);
    }

    void ActorCreate_AnimatedComponent::onAddNodeInTransitionTree(class Blob& _blob)
    {
        addNodeInTransitionTree(_blob);
    }

    void ActorCreate_AnimatedComponent::onRemoveNodeInTransitionTree(class Blob& _blob)
    {
        removeNodeInTransitionTree(_blob);
    }

    void ActorCreate_AnimatedComponent::onUpdateNodeData(class Blob& _blob)
    {
        updateNodeData(_blob);
    }

    void ActorCreate_AnimatedComponent::onPlayingNodesCurrent(class Blob& _blob)
    {
        sendPlayingNodesCurrent();
    }

    void ActorCreate_AnimatedComponent::onMovePositionChild(class Blob& _blob)
    {
        movePositionChild(_blob);
    }

    void ActorCreate_AnimatedComponent::onMoveLinkNode(class Blob& _blob)
    {
        moveLinkNode(_blob);
    }

    void ActorCreate_AnimatedComponent::onGetListOfTransitionsRoots(class Blob& _blob)
    {
        sendListOfTransitionsRoot();
    }

    /************************************************************************/
    /* Send messages                                                        */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendSubAnimList()
    {
		Actor_Template *actor = m_actorCreatePlugin->getActorTemplate();
		if (!actor)
			return;

		AnimatedComponent_Template *animComp = actor->GetComponent<AnimatedComponent_Template>();
		ITF_ASSERT(animComp);
		if (!animComp)
			return;

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendSubAnimList());

        const SubAnimSet_Template *subAnimSet = animComp->getSubAnimTemplate();
        u32 count = subAnimSet->getSubAnimList().size();
        b.pushUInt32(count);
        for (u32 i=0; i<count; i++)
        { 
            const SubAnim_Template subAnim = subAnimSet->getSubAnimList()[i];

            #ifdef ITF_DEBUG_STRINGID
            b.pushString(subAnim.getFriendlyName().getDebugString());
            #else
            b.pushString("No StringID strings");
            #endif
        }
        PLUGINGATEWAY->sendPacket(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
    }

    /************************************************************************/
    /* List of inputs                                                       */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendInputList()
    {
		Actor_Template *actor = m_actorCreatePlugin->getActorTemplate();
		if (!actor)
			return;

		AnimatedComponent_Template *animComp = actor->GetComponent<AnimatedComponent_Template>();
		ITF_ASSERT(animComp);
		if (!animComp)
			return;

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendInputList());

        u32 count = animComp->getInputList().size();
        b.pushUInt32(count);
        for (u32 i = 0; i < count; i++)
        {
            //b.pushInt32(_animComp->getInputType(i));

            #ifdef ITF_DEBUG_STRINGID
            b.pushString(animComp->getInputList()[i].getName());
            #else
            b.pushString("No StringID strings");
            #endif

            //if(_animComp->getInputType(i) == Input::InputType::InputType_F32)
            //{
            //    f32 value;
            //    _animComp->getInputValue(i, value);
            //    b.pushFloat32(value);
            //}
            //else if(_animComp->getInputType(i) == Input::InputType::InputType_U32)
            //{
            //    u32 value;
            //    _animComp->getInputValue(i, value);
            //    b.pushUInt32(value);
            //}
        }

        PLUGINGATEWAY->sendPacket(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
    }

    /************************************************************************/
    /* Send list of class                                                   */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendListOfClass()
    {
        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        const ObjectFactory* factory = animTreeTemplate->getFactory();


        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendListOfClass());


        // count of nodes
        u32 count = factory->GetObjectsNames().size();
        b.pushUInt32(count);
        for (u32 i = 0; i < count; i++)
        {
            String name = factory->GetObjectsNames()[i].getDebugString();
            b.pushString(name);
        } 


        PLUGINGATEWAY->sendPacket(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
    }


    /************************************************************************/
    /* List of animation roots                                              */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendListOfAnimRoot()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendListOfAnimRoot());

        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        // send count of nodes
        u32 count = animTreeTemplate->getNodeList().size();
        b.pushUInt32(count);

        // send list of nodes
        for (u32 iNode = 0; iNode < count; iNode++)
        {
            #ifdef ITF_DEBUG_STRINGID
            b.pushString(animTreeTemplate->getNodeList()[iNode]->getId().getDebugString());
            #else
            b.pushString("No StringID strings");
            #endif

        }

        PLUGINGATEWAY->sendPacket(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
    }

    /************************************************************************/
    /* List of transition roots                                             */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendListOfTransitionsRoot()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendListOfTransitions());

        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        const u32 count = animTreeTemplate->getNodeTransition().size();
        b.pushUInt32(count);

        // send list of nodes
        for (u32 iNode = 0; iNode < count; iNode++)
        {
            if(animTreeTemplate->getNodeTransition()[iNode]->getNode())
            {            
                // initialize numId
                //animTreeTemplate->getNodeTransition()[iNode]->getNode()->setNumId(iNode);

                b.pushInt32(iNode);
                b.pushString(animTreeTemplate->getNodeTransition()[iNode]->getNode()->GetObjectClassName());

                #ifdef ITF_DEBUG_STRINGID
                b.pushString(animTreeTemplate->getNodeTransition()[iNode]->getNode()->getId().getDebugString());
                #else
                b.pushString("No StringID strings");
                #endif
            }
            else 
            {
                b.pushInt32(iNode);
                b.pushString("no node");
                b.pushString("no node");
            }

            // from
            u32 countFrom = animTreeTemplate->getNodeTransition()[iNode]->getFromList().size();
            b.pushUInt32(countFrom);
            for (u32 i = 0; i < countFrom; i++)
            {
                #ifdef ITF_DEBUG_STRINGID
                b.pushString(animTreeTemplate->getNodeTransition()[iNode]->getFromList()[i].getDebugString());
                #else
                b.pushString("No StringID strings");
                #endif
            }

            // to
            u32 countTo = animTreeTemplate->getNodeTransition()[iNode]->getToList().size();
            b.pushUInt32(countTo);
            for (u32 i = 0; i < countTo; i++)
            {
                #ifdef ITF_DEBUG_STRINGID
                b.pushString(animTreeTemplate->getNodeTransition()[iNode]->getToList()[i].getDebugString());
                #else
                b.pushString("No StringID strings");
                #endif
            }

        }

        PLUGINGATEWAY->sendPacket(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
    }

    /************************************************************************/
    /* Send transitions informations and data                               */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendListOfTransitionsInfos()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendListOfTransitionsInfos());

        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        const u32 count = animTreeTemplate->getNodeTransition().size();
        b.pushUInt32(count);

        // send list of nodes
        for (u32 iNode = 0; iNode < count; iNode++)
        {
            if(animTreeTemplate->getNodeTransition()[iNode]->getNode())
            {
                b.pushInt32(iNode);
                b.pushString(animTreeTemplate->getNodeTransition()[iNode]->getNode()->GetObjectClassName());

                #ifdef ITF_DEBUG_STRINGID
                b.pushString(animTreeTemplate->getNodeTransition()[iNode]->getNode()->getId().getDebugString());
                #else
                b.pushString("No StringID strings");
                #endif
            }
            else
            {
                b.pushInt32(iNode);
                b.pushString("no node");
                b.pushString("no node");
            }

            // from
            u32 countFrom = animTreeTemplate->getNodeTransition()[iNode]->getFromList().size();
            b.pushUInt32(countFrom);
            for (u32 i = 0; i < countFrom; i++)
            {
                #ifdef ITF_DEBUG_STRINGID
                b.pushString(animTreeTemplate->getNodeTransition()[iNode]->getFromList()[i].getDebugString());
                #else
                b.pushString("No StringID strings");
                #endif
            }

            // to
            u32 countTo = animTreeTemplate->getNodeTransition()[iNode]->getToList().size();
            b.pushUInt32(countTo);
            for (u32 i = 0; i < countTo; i++)
            {
                #ifdef ITF_DEBUG_STRINGID
                b.pushString(animTreeTemplate->getNodeTransition()[iNode]->getToList()[i].getDebugString());
                #else
                b.pushString("No StringID strings");
                #endif
            }

        }
        PLUGINGATEWAY->sendPacket(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
    }

    void ActorCreate_AnimatedComponent::sendTransitionsData(u32 _indexRoot)
    {
        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        if(!(_indexRoot < animTreeTemplate->getNodeTransition().size()))
        {
            sendFeedBack(bfalse);
            return;
        }

        char* buffer = NULL;

        // serialize
        SerializerToolXML serializer;
        uSize size;
        serializer.OpenToWrite(&buffer, &size);
        animTreeTemplate->getNodeTransition()[_indexRoot]->Serialize(&serializer,(ESerializeFlags)0); //ESerialize_PropertyEdit_Save
        serializer.Close();
        

        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(blob);
        blob.pushString(getSendTransitionData());
        blob.pushUInt32(_indexRoot);
        blob.pushString(buffer);

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
        if(!bSendOk)
            m_actorCreatePlugin->m_editorPeer = NULL;

        SF_DEL(buffer);       
    }

    void ActorCreate_AnimatedComponent::updateTransitionData(class Blob& _blob)
    {
        // id node where the new node is updating
        u32 indexRoot = _blob.extractUint32();

        // class name of new node
        String data;         
        _blob.extractString(data);


        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        if(!(indexRoot < animTreeTemplate->getNodeTransition().size()))
        {
            sendFeedBack(bfalse);
            return;
        }

        UNDOREDO_MANAGER->preChange(m_actorCreatePlugin->getActor());

        bbool modified = false;        
        if(animTreeTemplate->getNodeTransition()[indexRoot])
        {
            SerializerToolXML serializer;

            StringConverter converted(data);

            serializer.OpenToRead(converted.getChar());
            animTreeTemplate->getNodeTransition()[indexRoot]->Serialize(&serializer,ESerialize_PropertyEdit_Load);
            serializer.Close();
            modified =  btrue;
        }
        
        sendFeedBack(modified);                
    }

    void ActorCreate_AnimatedComponent::addTransition()
    {
        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        bbool modified = bfalse;

        BlendTreeTransition_Template <AnimTreeResult>* transition = new BlendTreeTransition_Template <AnimTreeResult>();
        if(transition)
        {
            SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >& listNodes = (SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >&)animTreeTemplate->getNodeTransition();
            listNodes.push_back(transition);
            modified = btrue;
        }

        sendFeedBack(modified);
    }

    void ActorCreate_AnimatedComponent::removeTransition(class Blob& _blob)
    {
        // id node
        u32 indexRoot = _blob.extractUint32();


        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        if(!(indexRoot < animTreeTemplate->getNodeTransition().size()))
        {
            sendFeedBack(bfalse);
            return;
        }


        SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >& listNodes = (SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >&)animTreeTemplate->getNodeTransition();

        bbool modified = bfalse;

        BlendTreeNodeTemplate <AnimTreeResult>* node = (BlendTreeNodeTemplate <AnimTreeResult>*)listNodes[indexRoot]->getNode();
        if(node)
        {
            m_blendTreeTool.removeChildrenNode(node, node);
            SF_DEL(node);
        }
        listNodes.eraseKeepOrder(indexRoot);
        modified = btrue;

        sendFeedBack(modified);
    }

    /************************************************************************/
    /* Tree                                                                 */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendTree()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendTree());
        
        if(m_currentNode)
        {
            // count and init nodes
            u32 nodeCount = 0;
            m_blendTreeTool.getChildrenCountAndInit(m_currentNode, nodeCount);
            b.pushUInt32(nodeCount);

            // send node data list (id, numId, capacity, ...)
            m_blendTreeTool.getChildren(b, m_currentNode);

            // send links list (1 <-> 3; 1 <-> 5; ...)
            m_blendTreeTool.getChildrenForLink(b, m_currentNode, -1);   // -1: root

            PLUGINGATEWAY->sendPacket(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
        }
    }


    
    void ActorCreate_AnimatedComponent::sendPlayingNodesCurrent()
    {        
        //AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        //if(!animTreeTemplate)
        //    return;

        //Actor *actor = m_actorCreatePlugin->getActor();
        //if (!actor)
        //    return;

        //AnimatedComponent *animComp = actor->GetComponent<AnimatedComponent>();
        //ITF_ASSERT(animComp);
        //if (!animComp)
        //    return;

        //ITF_VECTOR<AnimTreeNode*> list;
        //animComp->dbgGetCurNodesNumId(list);
        
        //ITF_VECTOR<u32> listId;

        //const u32 count = animComp->getAnimTree()->getTemplate()->getNodeList().size();

        //u32 numNodes = list.size();
        //for ( u32 i = 0; i < numNodes; i++ )
        //{
        //    AnimTreeNode* node = list[i];
        //    u32 countNode = 0;

        //    // send list of links
        //    for (u32 iNode = 0; iNode < count; iNode++)
        //    {   
        //        StringID id = animComp->getAnimTree()->getNodeID(iNode);
        //        i32 index = getChildrenNumId(animComp->getAnimTree()->getNode(id), node, countNode);

        //        if(index != -1)
        //        {
        //            listId.push_back(index);
        //            continue;
        //        } 
        //    }
        //}
       

        //Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        //fillHeader(b);
        //b.pushString(getSendPlayingNodesCurrent());

        //b.pushInt32(numNodes);
        //for ( u32 i = 0; i < numNodes; i++ )
        //{
        //    b.pushInt32(listId[i]);
        //}
        //PLUGINGATEWAY->send(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);

    }
    

    /************************************************************************/
    /* Generic                                                              */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendNodeData(u32 _nodeId)
    {
        if(!m_currentNode)
            return;

        char* buffer = NULL;

        m_blendTreeTool.getBufferNode(m_blendTreeTool.getNodeByNumId(m_currentNode, _nodeId), &buffer);


        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(blob);
        blob.pushString(getSendNodeData());
        blob.pushUInt32(_nodeId);
        blob.pushString(buffer);

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
        if(!bSendOk)
            m_actorCreatePlugin->m_editorPeer = NULL;

        SF_DEL(buffer);       
    }

    void ActorCreate_AnimatedComponent::updateNodeData(class Blob& _blob)
    {
        // id node where the new node is updating
        i32 nodeId = _blob.extractInt32();

        // class name of new node
        String data;         
        _blob.extractString(data);


        if(!m_currentNode)
            return;

        UNDOREDO_MANAGER->preChange(m_actorCreatePlugin->getActor());
        
        bbool modified = m_blendTreeTool.setBufferNode(m_blendTreeTool.getNodeByNumId(m_currentNode, nodeId), data);
        sendFeedBack(modified);                
    }

    void ActorCreate_AnimatedComponent::movePositionChild(class Blob& _blob)
    {
        // id node
        u32 nodeId = _blob.extractUint32();

        // position of node
        u32 positionChild = _blob.extractUint32();

        if(!m_currentNode)
            return;

        UNDOREDO_MANAGER->preChange(m_actorCreatePlugin->getActor());

        bbool moved = m_blendTreeTool.movePositionChild(m_currentNode, nodeId, positionChild);
        sendFeedBack(moved);                     
    }

    void ActorCreate_AnimatedComponent::moveLinkNode(class Blob& _blob)
    {
        // id node
        u32 nodeId = _blob.extractUint32();

        // position of node
        u32 parentId = _blob.extractUint32();

        if(!m_currentNode)
            return;

        UNDOREDO_MANAGER->preChange(m_actorCreatePlugin->getActor());

        bbool moved = m_blendTreeTool.moveLinkNode(m_currentNode, nodeId, parentId);
        sendFeedBack(moved);         
    }    

    /************************************************************************/
    /* Feed back                                                            */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendFeedBack(bbool actionIsDone) 
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendUpdateIsDone());

        b.pushBool(actionIsDone);

        PLUGINGATEWAY->sendPacket(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);    
    }

    void ActorCreate_AnimatedComponent::ReceiveActorReload()
    {
        sendFeedBack(btrue);
    }


    ITF_STDSTRING ActorCreate_AnimatedComponent::saveComponent(Actor *actor, Path componentPath, ActorEditorParamsFiles::componentSaveType saveEnable, TemplateActorComponent *actorComponent)
    {
        CSerializerObjectLua serializerLUA;
        CSerializerObject* serializer = &serializerLUA;
        ITF_STDSTRING buffer;
        String8 animCompEndBuffer;
        String8 bufferTmp;

        // case with "AnimatedComponent"
        AnimatedComponent_Template* animated = actorComponent->DynamicCast<AnimatedComponent_Template>(ITF_GET_STRINGID_CRC(AnimatedComponent_Template,2655002388));
        if (animated)
        {

            // directory of animation, nodes, inputs,...
            String nodeDirectory = FilePath::getDirectory(componentPath.getString().cStr()) + "anim/";

            /************************************************************************/
            /* nodes (one node by file)                                             */
            /************************************************************************/
            u32 nodeCount = animated->getAnimTreeTemplate().getNodeList().size();
            for (u32 iNode = 0; iNode < nodeCount; iNode++)
            {
                // create file name:     "anm_node_" + name of node + ".ilu"
                StringID name = animated->getAnimTreeTemplate().getNodeList()[iNode]->getId();
                String nodeFileName; 
                nodeFileName.setTextFormat("anm_node_%s.ilu", name.getDebugString());

                // path
                String nodePathString = nodeDirectory + nodeFileName;
                Path nodePath = nodePathString;

                if(saveEnable == ActorEditorParamsFiles::saveInComponent)
                {
                    String fullPath;
                    FILEMANAGER->TranslatePath(fullPath, nodePath.getString());
                    Directory::create(FilePath::getDirectory(fullPath));

                    // create file
                    File* file = FILEMANAGER->openFile(nodePath.getString(), ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                    if ( file )
                    {
                        file->seek(0, FILE_END);

                        // serialize node
                        BlendTreeNodeTemplate <AnimTreeResult>* node = animated->getAnimTreeTemplate().getNodeList()[iNode];
                        serializerLUA.OpenBufferToWrite("node", btrue, NULL);
                        serializer->SerializeObject("[1]",node,ACTORSMANAGER->getAnimTreeNodeFactory(),ESerialize_Data_Save);                        

                        buffer = serializerLUA.CloseBuffer();                                                   


                        // if not define nodes
                        file->write(iluNodeEndIfDefineNodes(), u32(sizeof(u8)*strlen(iluNodeEndIfDefineNodes())));

                        // serialize node
                        file->write(buffer.c_str(), u32(sizeof(u8)*buffer.length()));

                        // end of file / close lua with "appendTable(...)"                                                                
                        file->write(iluNodeEnd(), u32(sizeof(u8)*strlen(iluNodeEnd())));

                        FILEMANAGER->closeFile(file);

                        // adding include for animated component
                        bufferTmp.setTextFormat(ActorCreatePlugin::actInclude(), nodePath.getString().cStr());
                        animCompEndBuffer += bufferTmp;
                    }
                }
                else
                {
                    // adding include for animated component
                    bufferTmp.setTextFormat(ActorCreatePlugin::actInclude(), nodePath.getString().cStr());
                    animCompEndBuffer += bufferTmp;
                }
            }
            
            /************************************************************************/
            /* animations                                                           */
            /************************************************************************/
            {
                // serialize sub animation list
                serializerLUA.OpenBufferToWrite("component.AnimatedComponent_Template.animSet", btrue);                       
                ((SubAnimSet_Template*)animated->getSubAnimTemplate())->Serialize(&serializerLUA,ESerialize_Data_Save); // casting: it's for saving, "animated" is already binaryClone
                buffer = serializerLUA.CloseBuffer();

                // name of file
                String fileName; 
                fileName.setTextFormat("animations.ilu");

                // path
                String animPathString = nodeDirectory + fileName;
                Path animPath = animPathString;
                String fullPath;
                FILEMANAGER->TranslatePath(fullPath, animPath.getString());
                Directory::create(FilePath::getDirectory(fullPath));

                if(saveEnable == ActorEditorParamsFiles::saveInComponent)
                {
                    // create file
                    File* file = FILEMANAGER->openFile(animPath.getString(), ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                    if ( file )
                    {
                        file->seek(0, FILE_END);

                        // write
                        file->write(buffer.c_str(), u32(sizeof(u8)*buffer.length()));
                        bufferTmp.setTextFormat(ActorCreatePlugin::iluName(), actorComponent->GetObjectClassName());
                        file->write(bufferTmp.cStr(), sizeof(u8)*bufferTmp.getLen());
                        //file->write(ActorCreatePlugin::iluEnd(), sizeof(u8)*strlen(ActorCreatePlugin::iluEnd()));
                        FILEMANAGER->closeFile(file);

                        // adding include for animated component
                        bufferTmp.setTextFormat(ActorCreatePlugin::actInclude(), animPath.getString().cStr());
                        animCompEndBuffer += bufferTmp;
                    }
                }
                else
                {
                    // adding include for animated component
                    bufferTmp.setTextFormat(ActorCreatePlugin::actInclude(), animPath.getString().cStr());
                    animCompEndBuffer += bufferTmp;
                }
            }


            /************************************************************************/
            /* inputs                                                               */
            /************************************************************************/ 
    
            {
/*              
                // serialize sub animation list
                serializerLUA.OpenBufferToWrite("component.AnimatedComponent_Template.inputs", btrue);
 
                
                Archive memo;
                // //serializer->Serialize("component.AnimatedComponent_Template.inputs",(SafeArray <InputDesc>&)animated->getInputList(),ESerialize_Template_Save);

                //((SafeArray <InputDesc>&)animated->getInputList()).serialize(&memo); // casting: it's for saving, "animated" is already binaryClone
                //memo.Serialize(&serializerLUA,ESerialize_Template_Save);

                //buffer = serializerLUA.CloseBuffer();

                // serialize node
                serializerLUA.OpenBufferToWrite("inputs", btrue, NULL);
                serializer->SerializeObject("[1]",((SafeArray <InputDesc>&)animated->getInputList()),ACTORSMANAGER->getAnimTreeNodeFactory(),ESerialize_Data_Save);                        

                buffer = serializerLUA.CloseBuffer();  

                // name of file
                String fileName;
                fileName.setTextFormat("animation_inputs.ilu");

                // path
                String animPathString = nodeDirectory + fileName;
                Path animPath = animPathString;
                String fullPath;
                FILEMANAGER->TranslatePath(fullPath, animPath.getString());
                Directory::create(FilePath::getDirectory(fullPath));

                // create file
                File* file = FILEMANAGER->openFile(animPath.getString(), ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                file->seek(0, FILE_END);
                if ( file )
                {
                    // if not define nodes
                    file->write(iluNodeEndIfDefineInputs, sizeof(u8)*strlen(iluNodeEndIfDefineNodes));

                    file->write(buffer.c_str(),sizeof(u8)*buffer.length());

                    file->write(iluEnd, sizeof(u8)*strlen(iluEnd));
                    FILEMANAGER->closeFile(file);

                    // adding include for animated component
                    bufferTmp.setTextFormat(actInclude, animPath.getString().cStr());
                    animCompEndBuffer += bufferTmp;
                }
*/
            }
  

            
            /************************************************************************/
            /* animated component                                                   */
            /************************************************************************/
            {
                AnimatedComponent_Template dummyAnimComp;
                BinaryClone(const_cast<AnimatedComponent_Template*>(animated), &dummyAnimComp);
                
                AnimTree_Template& AnimTreetemplate = (AnimTree_Template&)dummyAnimComp.getAnimTreeTemplate();

                // clear list because serialize in "anm_node_...ilu"
                ((SafeArray < BlendTreeNodeTemplate <AnimTreeResult>* >&)AnimTreetemplate.getNodeList()).clear();
                          
                // clear list because serialize in "animations.ilu"
                ((ITF_VECTOR<SubAnim_Template>&)dummyAnimComp.getSubAnimTemplate()->getSubAnimList()).clear();
                ((ITF_VECTOR<BankChange_Template>&)dummyAnimComp.getSubAnimTemplate()->getBankChangeList()).clear();
                //((SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >&)AnimTreetemplate.getNodeTransition()).clear();
                //((SafeArray <InputDesc>&)dummyAnimComp.getInputList()).clear();


                ITF_STDSTRING bufferComponent;
                // serialize animated component
                serializerLUA.OpenBufferToWrite("component", btrue, actorComponent->GetObjectClassName());
                dummyAnimComp.Serialize(&serializerLUA,ESerialize_Data_Save);
                bufferComponent = serializerLUA.CloseBuffer();

                bufferComponent += animCompEndBuffer.cStr();
                return bufferComponent;
            }

            //String fullPath;
            //FILEMANAGER->TranslatePath(fullPath, componentPath.getString());
            //Directory::create(FilePath::getDirectory(fullPath));

            //File* file = FILEMANAGER->openFile(componentPath.getString(), ITF_FILE_ATTR_WRITE);
            //file->seek(0, FILE_END);
            //if ( file )
            //{
            //    file->write(buffer.c_str(),sizeof(u8)*buffer.length());
            //    file->write(animCompEndBuffer.cStr(), sizeof(u8)*animCompEndBuffer.getLen());
            //    file->write(iluAnimCompEnd, sizeof(u8)*strlen(iluAnimCompEnd));
            //    FILEMANAGER->closeFile(file);

            //    bbool found = bfalse;
            //    for (int compIdx = 0; !found && compIdx < i; ++compIdx)
            //    {
            //        if (editorParams->m_componentFiles[compIdx].m_path == componentPath)
            //            found = btrue;
            //    }
            //    if (!found)
            //    {
            //        bufferTmp.setTextFormat(actInclude, componentPath.getString().cStr());
            //        actEndBuffer += bufferTmp;
            //    }
            //}
            
        }
        return NULL;
    }
    
    /************************************************************************/
    /* Anim & Input data                                                    */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::sendAnimData(u32 _index)
    {
		Actor_Template *actor = m_actorCreatePlugin->getActorTemplate();
		if (!actor)
			return;

		AnimatedComponent_Template *animComp = actor->GetComponent<AnimatedComponent_Template>();
		ITF_ASSERT(animComp);
		if (!animComp)
			return;

        const SubAnimSet_Template *subAnimSet = animComp->getSubAnimTemplate();
        u32 count = subAnimSet->getSubAnimList().size();

        if(!(_index < count))
        {
            sendFeedBack(bfalse);
            return;
        }

   

        char* buffer = NULL;

        // serialize
        SerializerToolXML serializer;
        uSize size;
        serializer.OpenToWrite(&buffer, &size);
        
        const SubAnim_Template subAnim = subAnimSet->getSubAnimList()[_index];
        serializer.SerializeObject("CONTENT", (SubAnim_Template&)subAnim, ESerialize_PropertyEdit_Save);
        serializer.Close();


        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(blob);
        blob.pushString(getSendAnimData());
        blob.pushUInt32(_index);
        blob.pushString(buffer);

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
        if(!bSendOk)
            m_actorCreatePlugin->m_editorPeer = NULL;

        SF_DEL(buffer);       
    }

    void ActorCreate_AnimatedComponent::updateAnimData(class Blob& _blob)
    {
        // id node where the new node is updating
        u32 index = _blob.extractUint32();

        // class name of new node
        String data;         
        _blob.extractString(data);


		Actor_Template *actor = m_actorCreatePlugin->getActorTemplate();
		if (!actor)
			return;

		AnimatedComponent_Template *animComp = actor->GetComponent<AnimatedComponent_Template>();
		ITF_ASSERT(animComp);
		if (!animComp)
			return;


        const SubAnimSet_Template *subAnimSet = animComp->getSubAnimTemplate();
        u32 count = subAnimSet->getSubAnimList().size();


        if(!(index < count))
        {
            sendFeedBack(bfalse);
            return;
        }

        UNDOREDO_MANAGER->preChange(m_actorCreatePlugin->getActor());


        const SubAnim_Template &subAnim = subAnimSet->getSubAnimList()[index];

        bbool modified = false;

        SerializerToolXML serializer;

        StringConverter converted(data);

        serializer.OpenToRead(converted.getChar());
        serializer.SerializeObject("CONTENT", (SubAnim_Template&)subAnim, ESerialize_PropertyEdit_Load);
        serializer.Close();
        modified =  btrue;

        sendFeedBack(modified);
    }

    void ActorCreate_AnimatedComponent::sendInputData(u32 _index)
    {
		Actor_Template *actor = m_actorCreatePlugin->getActorTemplate();
		if (!actor)
			return;

		AnimatedComponent_Template *animComp = actor->GetComponent<AnimatedComponent_Template>();
		ITF_ASSERT(animComp);
		if (!animComp)
			return;

		const SafeArray <InputDesc>& inputSet = animComp->getInputList();
		u32 count = inputSet.size();

		if(!(_index < count))
		{
			sendFeedBack(bfalse);
			return;
		}



		char* buffer = NULL;

		// serialize
		SerializerToolXML serializer;
		uSize size;
		serializer.OpenToWrite(&buffer, &size);

		InputDesc input = inputSet[_index];
		serializer.SerializeObject("CONTENT", input, ESerialize_PropertyEdit_Save);
		serializer.Close();


		ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
		fillHeader(blob);
		blob.pushString(getSendInputData());
		blob.pushUInt32(_index);
		blob.pushString(buffer);

		const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
		if(!bSendOk)
			m_actorCreatePlugin->m_editorPeer = NULL;

		SF_DEL(buffer);
    }

    void ActorCreate_AnimatedComponent::updateInputData(class Blob& _blob)
    {
		// id node where the new node is updating
		u32 index = _blob.extractUint32();

		// class name of new node
		String data;         
		_blob.extractString(data);


		Actor_Template *actor = m_actorCreatePlugin->getActorTemplate();
		if (!actor)
			return;

		AnimatedComponent_Template *animComp = actor->GetComponent<AnimatedComponent_Template>();
		ITF_ASSERT(animComp);
		if (!animComp)
			return;


		const SafeArray <InputDesc>& inputSet = animComp->getInputList();
		u32 count = inputSet.size();

		if(!(index < count))
		{
			sendFeedBack(bfalse);
			return;
		}

		UNDOREDO_MANAGER->preChange(m_actorCreatePlugin->getActor());


		InputDesc input = inputSet[index];

		bbool modified = false;

		SerializerToolXML serializer;

		StringConverter converted(data);

		serializer.OpenToRead(converted.getChar());
		serializer.SerializeObject("CONTENT", (InputDesc&)input, ESerialize_PropertyEdit_Load);
		serializer.Close();
		modified =  btrue;

		sendFeedBack(modified);    }

    /************************************************************************/
    /* Animations                                                           */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::addNodeInAnimTree(class Blob& _blob)
    {
        // id node where the new node is adding
        i32 idParent = _blob.extractInt32();

        // class name of new node
        String className;         
        _blob.extractString(className);

        // nameId of new node
        String nameId;
        _blob.extractString(nameId);

        bbool modified = bfalse;



        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        const ObjectFactory* factory = animTreeTemplate->getFactory();
        BlendTreeNodeTemplate<AnimTreeResult> * newNode = factory->CreateObject<BlendTreeNodeTemplate<AnimTreeResult> >(StringConverter(className).getChar());


        if (newNode && m_currentNode)
        {
            StringID strID(nameId);
            newNode->setId(strID);

            SafeArray < BlendTreeNodeTemplate <AnimTreeResult>* >& listNodes = (SafeArray < BlendTreeNodeTemplate <AnimTreeResult>* >&)animTreeTemplate->getNodeList();

            modified = m_blendTreeTool.addNode(m_currentNode, listNodes, newNode, idParent);
        }
        sendFeedBack(modified);
    }

    void ActorCreate_AnimatedComponent::removeNodeInAnimTree(class Blob& _blob)
    {
        // id node
        i32 id = _blob.extractInt32();

        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        if(!m_currentNode)
            return;

        bbool modified = bfalse;

        SafeArray < BlendTreeNodeTemplate <AnimTreeResult>* >& listNodes = (SafeArray < BlendTreeNodeTemplate <AnimTreeResult>* >&)animTreeTemplate->getNodeList();

        modified = m_blendTreeTool.removeNode(m_currentNode, listNodes, id);

        sendFeedBack(modified);
    }

    /************************************************************************/
    /* Transitions                                                          */
    /************************************************************************/
    void ActorCreate_AnimatedComponent::addNodeInTransitionTree(class Blob& _blob)
    {
        // id node where the new node is adding
        i32 idParent = _blob.extractInt32();

        // class name of new node
        String className;
        _blob.extractString(className);

        // nameId of new node
        String nameId;
        _blob.extractString(nameId);

        bbool modified = bfalse;



        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        const ObjectFactory* factory = animTreeTemplate->getFactory();
        BlendTreeNodeTemplate<AnimTreeResult> * newNode = factory->CreateObject<BlendTreeNodeTemplate<AnimTreeResult> >(StringConverter(className).getChar());


        if (newNode)
        {
            StringID strID(nameId);
            newNode->setId(strID);

            SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >& listNodes = (SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >&)animTreeTemplate->getNodeTransition();

            // adding node on root
            if(idParent == 0)
            {
                BlendTreeTransition_Template <AnimTreeResult>* transition = new BlendTreeTransition_Template <AnimTreeResult>();
                transition->setNode(newNode);
                listNodes.push_back(transition);
                modified = btrue;            
            }
            // adding node on a parent node
            else
            {
                BlendTreeNodeTemplate <AnimTreeResult>* nodeParentFound = m_blendTreeTool.getNodeByNumId(m_currentNode, idParent);
                modified = m_blendTreeTool.addChildrenNode(nodeParentFound, newNode);
            }
        }
        sendFeedBack(modified);
    }

    void ActorCreate_AnimatedComponent::removeNodeInTransitionTree(class Blob& _blob)
    {
        // id node
        i32 id = _blob.extractInt32();

        AnimTree_Template* animTreeTemplate = m_actorCreatePlugin->getAnimTreeTemplate();
        if(!animTreeTemplate)
            return;

        SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >& listNodes = (SafeArray < BlendTreeTransition_Template <AnimTreeResult>* >&)animTreeTemplate->getNodeTransition();

        if(!m_currentNode)
            return;

        bbool modified = bfalse;

        // remove a node root
        if(id == 1)
        {
            const u32 count = listNodes.size();
            for (u32 i = 0; i < count; i++)
            {
                if(listNodes[i]->getNode() == m_currentNode)
                {
                    m_blendTreeTool.removeChildrenNode(m_currentNode, m_blendTreeTool.getNodeByNumId(m_currentNode, (u32)id));
                    BlendTreeNodeTemplate <AnimTreeResult>* node = (BlendTreeNodeTemplate <AnimTreeResult>*)listNodes[i]->getNode();
                    SF_DEL(node);
                    //listNodes.eraseKeepOrder(i);
                    listNodes[i]->setNode(NULL);
                    
                    modified = btrue;
                }
            }
        }
        // remove a node child of root
        else
        {
            modified = m_blendTreeTool.removeChildrenNode(m_currentNode, m_blendTreeTool.getNodeByNumId(m_currentNode, (u32)id));
        }

        sendFeedBack(modified);
    }


};

#endif //!ITF_FINAL
