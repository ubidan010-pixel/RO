#ifndef _ITF_BTDECIDER_H_
#define _ITF_BTDECIDER_H_

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

namespace ITF
{

class BTDecider : public BTNode
{
    DECLARE_OBJECT_CHILD_RTTI(BTDecider,BTNode,2571686809);

public:

    BTDecider();
    ~BTDecider();

    virtual void                        init( class BehaviorTree* _tree, const class BTNode_Template* _template );
    virtual bbool                       validate( Actor* _actor ) const;
    virtual bbool                       decide( f32 _dt );
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

    const class BTDecider_Template*     getTemplate() const;

    SafeArray <BTNode*>                 m_nodes;
    u32                                 m_currentNode;
};

class BTDecider_Template : public BTNode_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTDecider_Template,BTNode_Template,528484444);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTDecider);

public:

    BTDecider_Template() : m_reevaluate(bfalse) {}
    ~BTDecider_Template();

    u32                                     getNumNodes() const { return m_nodes.size(); }
    const BTNodeTemplate_Ref&               getNodeRef( u32 _index ) const { return m_nodes[_index]; }
    const SafeArray <StringID>&             getClearFacts() const { return m_clearFacts; }
    bbool                                   getReevaluate() const { return m_reevaluate; }

private:

    SafeArray <BTNodeTemplate_Ref>          m_nodes;
    SafeArray <StringID>                    m_clearFacts;
    bbool                                   m_reevaluate;
};

ITF_INLINE const BTDecider_Template*  BTDecider::getTemplate() const {return static_cast<const BTDecider_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTDECIDER_H_