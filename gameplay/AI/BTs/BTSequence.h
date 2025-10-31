#ifndef _ITF_BTSEQUENCE_H_
#define _ITF_BTSEQUENCE_H_

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

namespace ITF
{

class BTSequence : public BTNode
{
    DECLARE_OBJECT_CHILD_RTTI(BTSequence,BTNode,2825733134);

public:

    BTSequence();
    ~BTSequence();

    virtual void                        init( class BehaviorTree* _tree, const class BTNode_Template* _template );
    virtual bbool                       validate( Actor* _actor ) const;
    virtual void                        onActivate();
    virtual void                        onDeactivate();
    virtual bbool                       update( f32 _dt );
    virtual void                        updateAnimInput();
    virtual void                        onEvent( Event* _event );
#ifdef ITF_SUPPORT_EDITOR
    virtual void                        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

private:

    void                                setCurrentNode( u32 _nodeIndex );

    const class BTSequence_Template*    getTemplate() const;

    SafeArray <BTNode*>                 m_nodes;
    u32                                 m_currentNode;
};

class BTSequence_Template : public BTNode_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTSequence_Template,BTNode_Template,2865513351);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTSequence);

public:

    BTSequence_Template() {}
    ~BTSequence_Template();

    u32                                     getNumNodes() const { return m_nodes.size(); }
    const BTNodeTemplate_Ref&               getNodeRef( u32 _index ) const { return m_nodes[_index]; }

private:

    SafeArray <BTNodeTemplate_Ref>          m_nodes;
};

ITF_INLINE const BTSequence_Template*  BTSequence::getTemplate() const {return static_cast<const BTSequence_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTSEQUENCE_H_