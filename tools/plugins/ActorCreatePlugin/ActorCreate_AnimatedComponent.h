#ifndef _ITF_ACTORCREATE_ANIMATEDCOMPONENT_H_
#define _ITF_ACTORCREATE_ANIMATEDCOMPONENT_H_

#ifndef _ITF_ACTORCREATEPLUGIN_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreatePlugin.h"
#endif //_ITF_ACTORCREATEPLUGIN_H_

#ifndef _ITF_BLENDTREENODE_H_
#include "engine/blendTree/BlendTreeNode.h"
#endif //_ITF_BLENDTREENODE_H_

#ifndef _ITF_ACTORCREATE_BLENDTREE_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreate_BlendTree.h"
#endif //_ITF_ACTORCREATE_BLENDTREE_H_

#ifndef _ITF_ANIMTREERESULT_H_
#include "engine/blendTree/animTree/AnimTreeResult.h"
#endif //_ITF_ANIMTREERESULT_H_

namespace ITF
{
    class AnimTree_Template;

    class ActorCreate_AnimatedComponent : public ActorCreateSpProcess
    {
    public:

        
        BlendTreeNodeTemplate<AnimTreeResult>* m_currentNode;

        ActorCreate_BlendTree<AnimTreeResult> m_blendTreeTool;


        static const char * iluNodeEndIfDefineNodes() {return "if not component.AnimatedComponent_Template.tree.AnimTree_Template.nodes then\n   component.AnimatedComponent_Template.tree.AnimTree_Template.nodes = {}\nend\n\n";}
        //static const char * iluNodeEnd() {return "\nappendTable(component.AnimatedComponent_Template.tree.AnimTree_Template.nodes,{node})\n\n";}
        static const char * iluNodeEnd() {return "\nappendTable(component.AnimatedComponent_Template.tree.AnimTree_Template.nodes,node)\n\n";}

        static const char * iluNodeEndIfDefineInputs() {return "if not component.AnimatedComponent_Template.inputs then\n   component.AnimatedComponent_Template.inputs = {}\nend\n\n";}
        static const char * iluAnimCompEnd() {return "\nappendTable(params.Actor_Template.COMPONENTS, {component})\ncomponent = {}\n";}



        static char *           GetComponentNameStatic() { return "AnimatedComponent_Template"; }
        virtual char *          GetComponentName() { return GetComponentNameStatic(); };
        void                    ReceiveMsg(class Blob& _blob);
        virtual void            ReceiveActorReload();

        /************************************************************************/
        /* receive                                                              */
        /************************************************************************/
        static const char*      getRecvGetSubAnimList()                 {return "getSubAnimList";}
        static const char*      getRecvGetInputList()                   {return "getInputList";}
        static const char*      getRecvGetListOfClass()                 {return "getListOfClass";}

        static const char*      getRecvGetAnimData()                    {return "getAnimData";}
        static const char*      getRecvUpdateAnimData()                 {return "updateAnimData";}
        static const char*      getRecvGetInputData()                   {return "getInputData";}
        static const char*      getRecvUpdateInputData()                {return "updateInputData";}


        static const char*      getRecvGetListOfAnimRoots()             {return "getListOfAnimRoots";}
        static const char*      getRecvGetListOfTransitionsRoots()      {return "getListOfTransitionsRoots";}

        static const char*      getRecvGetTree()                        {return "getTree";}

        static const char*      getRecvSetCurrentNode()                 {return "setCurrentNode";}


        // Animation tree
        static const char*      getRecvAddNodeInAnimTree()              {return "addNodeInAnimTree";}
        static const char*      getRecvRemoveNodeInAnimTree()           {return "removeNodeInAnimTree";}
        // Transition tree
        static const char*      getRecvAddNodeInTransitionTree()        {return "addNodeInTransitionTree";}
        static const char*      getRecvRemoveNodeInTransitionTree()     {return "removeNodeInTransitionTree";}

        // transitions
        static const char*      getRecvGetListOfTransitionsInfos()      {return "getListOfTransitionsInfos";}
        static const char*      getRecvGetTransitionData()              {return "getTransitionData";}
        static const char*      getRecvUpdateTransitionData()           {return "updateTransitionData";}


        //////////////////////////////////////////////////////////////////////////
        static const char*      getRecvGetNodeData()                    {return "getNodeData";}
        static const char*      getRecvUpdateNodeData()                 {return "updateNodeData";}
        static const char*      getRecvMovePositionChild()              {return "movePositionChild";}
        static const char*      getRecvMoveLinkNode()                   {return "moveLinkNode";}
        //////////////////////////////////////////////////////////////////////////

        static const char*      getRecvGetPlayingNodesCurrent()         {return "getPlayingNodesCurrent";}

        static const char*      getRecvSetCurrentNodeInAnimTree()       {return "setCurrentNodeInAnimTree";}
        static const char*      getRecvSetCurrentNodeInTranstionTree()  {return "setCurrentNodeInTranstionTree";}
        static const char*      getRecvAddTransition()                  {return "addTransition";}
        static const char*      getRecvRemoveTransition()               {return "removeTransition";}
        


        /************************************************************************/
        /* send                                                                 */
        /************************************************************************/
        static const char*      getSendListOfAnimRoot()                 {return "ListOfAnimRoots";}
        static const char*      getSendListOfTransitions()              {return "ListOfTransitionsRoots";}
        
        static const char*      getSendSubAnimList()                    {return "SubAnimList";}
        static const char*      getSendInputList()                      {return "InputList";}
        static const char*      getSendListOfClass()                    {return "ListOfClass";}

        //////////////////////////////////////////////////////////////////////////
        static const char*      getSendTree()                           {return "Tree";}
        static const char*      getSendNodeData()                       {return "NodeData";}
        static const char*      getSendPlayingNodesCurrent()            {return "PlayingNodesCurrent";}        
        static const char*      getSendUpdateIsDone()                   {return "UpdateIsDone";}
        //////////////////////////////////////////////////////////////////////////

        static const char*      getSendListOfTransitionsInfos()         {return "ListOfTransitionsInfos";}
        static const char*      getSendTransitionData()                 {return "TransitionData";}

        static const char*      getSendAnimData()                       {return "AnimData";}
        static const char*      getSendInputData()                      {return "InputData";}
        
    protected:
        void                    fillHeader(class Blob& _blob);
        
        ITF_STDSTRING           saveComponent(Actor *actor, Path componentPath, ActorEditorParamsFiles::componentSaveType saveEnable, TemplateActorComponent *actorComponent);

        void                    onGetSubAnimList(class Blob& _blob);
        void                    onGetInputList(class Blob& _blob);
        void                    onGetListOfClass(class Blob& _blob);
        void                    onGetListOfNodes(class Blob& _blob);
        void                    onGetTree(class Blob& _blob);
        void                    onGetNodeData(class Blob& _blob);

        void                    onUpdateNodeData(class Blob& _blob);
        void                    onPlayingNodesCurrent(class Blob& _blob);
        void                    onMovePositionChild(class Blob& _blob);
        void                    onMoveLinkNode(class Blob& _blob);
        void                    onSetCurrentNodeInAnimTree(class Blob& _blob);
        void                    onSetCurrentNodeInTranstionTree(class Blob& _blob);      
        void                    onGetListOfTransitionsRoots(class Blob& _blob);

        void                    onAddNodeInAnimTree(class Blob& _blob);
        void                    onRemoveNodeInAnimTree(class Blob& _blob);
        void                    onAddNodeInTransitionTree(class Blob& _blob);
        void                    onRemoveNodeInTransitionTree(class Blob& _blob);

        void                    sendSubAnimList();
        void                    sendInputList();
        void                    sendListOfClass();
        void                    sendTree();

        void                    sendListOfAnimRoot();
        void                    sendListOfTransitionsRoot();

        void                    sendPlayingNodesCurrent();
        void                    sendFeedBack(bbool actionIsDone);

        void                    sendListOfTransitionsInfos();
        void                    sendTransitionsData(u32 _indexRoot);
        void                    updateTransitionData(class Blob& _blob);
        void                    addTransition();
        void                    removeTransition(class Blob& _blob);

        void                    sendAnimData(u32 _index);
        void                    updateAnimData(class Blob& _blob);
        void                    sendInputData(u32 _index);
        void                    updateInputData(class Blob& _blob);

        void                    addNodeInAnimTree(class Blob& _blob);
        void                    removeNodeInAnimTree(class Blob& _blob);

        void                    addNodeInTransitionTree(class Blob& _blob);
        void                    removeNodeInTransitionTree(class Blob& _blob);

        //////////////////////////////////////////////////////////////////////////
        void                    removeNode(class Blob& _blob);
        void                    updateNodeData(class Blob& _blob);
        void                    movePositionChild(class Blob& _blob);
        void                    moveLinkNode(class Blob& _blob);
        void                    sendNodeData(u32 _nodeId);
        //////////////////////////////////////////////////////////////////////////
    };
}

#endif //_ITF_ACTORCREATE_ANIMATEDCOMPONENT_H_