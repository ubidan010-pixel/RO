#ifndef _ITF_BTTIMER_H_
#define _ITF_BTTIMER_H_

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

namespace ITF
{

class BTTimer : public BTNode
{
    DECLARE_OBJECT_CHILD_RTTI(BTTimer,BTNode,1347718650);

public:

    BTTimer();
    ~BTTimer();

    virtual void                        init( class BehaviorTree* _tree, const class BTNode_Template* _template );
    virtual bbool                       validate( Actor* _actor ) const;
    virtual void                        onActivate();
    virtual void                        onDeactivate();
    virtual bbool                       decide( f32 _dt );
    virtual bbool                       update( f32 _dt );
    virtual void                        onEvent( Event* _event );
#ifdef ITF_SUPPORT_EDITOR
    virtual void                        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

private:

    const class BTTimer_Template*       getTemplate() const;
    void                                resetTime();

    BTNode*                             m_node;
    f32                                 m_waitTime;
};

class BTTimer_Template : public BTNode_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTTimer_Template,BTNode_Template,3783268492);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTTimer);

public:

    BTTimer_Template();
    ~BTTimer_Template();

    const BTNodeTemplate_Ref&               getNodeRef() const { return m_node; }
    f32                                     getMinTime() const { return m_minTime; }
    f32                                     getMaxTime() const { return m_maxTime; }

private:

    BTNodeTemplate_Ref                      m_node;
    f32                                     m_minTime;
    f32                                     m_maxTime;
};

ITF_INLINE const BTTimer_Template*  BTTimer::getTemplate() const {return static_cast<const BTTimer_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTTIMER_H_