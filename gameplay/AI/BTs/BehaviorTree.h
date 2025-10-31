#ifndef _ITF_BEHAVIORTREE_H_
#define _ITF_BEHAVIORTREE_H_

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

#ifndef _ITF_BLACKBOARD_H_
#include "gameplay/ai/BTs/Blackboard.h"
#endif // _ITF_BLACKBOARD_H_

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif // #ifndef _ITF_EVENTLISTENER_H_

namespace ITF
{

class BehaviorTree : public IEventListener
{
public:

    BehaviorTree();
    ~BehaviorTree();
    
    void                                    init( const class BehaviorTree_Template* _template, class Actor* _actor );
    bbool                                   validate() const;
    void                                    update( f32 _dt );
    void                                    updateAnimInput();
#ifdef ITF_SUPPORT_EDITOR
    void                                    drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    virtual void                            onEvent(Event * _event);

    Blackboard&                             getBlackBoard() { return m_blackboard; }
    const class BehaviorTree_Template*      getTemplate() const { return m_template; }
    class Actor*                            getActor() const { return m_actor; }

private:

    class BTNode*                           m_root;
    Blackboard                              m_blackboard;
    const class BehaviorTree_Template*      m_template;
    class Actor*                            m_actor;
};

class BehaviorTree_Template
{
    DECLARE_SERIALIZE();

public:

    BehaviorTree_Template();
    ~BehaviorTree_Template();

    const BTNode_Template*          getNodeFromRef( const BTNodeTemplate_Ref& _ref ) const;
    const BTNode_Template*          findNode( const StringID& _id ) const;
    const BTNodeTemplate_Ref&       getRoot() const         { return m_root; }

private:

    BTNodeTemplate_Ref              m_root;
    SafeArray <BTNode_Template*>    m_nodes;
};

} // namespace ITF

#endif // _ITF_BEHAVIORTREE_H_