#ifndef _ITF_BTDELAY_H_
#define _ITF_BTDELAY_H_

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

namespace ITF
{

class BTDelay : public BTNode
{
    DECLARE_OBJECT_CHILD_RTTI(BTDelay,BTNode,2355144226);

public:

    BTDelay();
    ~BTDelay();

    virtual void                        onActivate();
    virtual bbool                       update( f32 _dt );

private:

    const class BTDelay_Template*       getTemplate() const;
    void                                resetTime();

    f32                                 m_waitTime;
};

class BTDelay_Template : public BTNode_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTDelay_Template,BTNode_Template,157870822);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTDelay);

public:

    BTDelay_Template();
    ~BTDelay_Template();

    f32                                     getMinTime() const { return m_minTime; }
    f32                                     getMaxTime() const { return m_maxTime; }

private:

    f32                                     m_minTime;
    f32                                     m_maxTime;
};

ITF_INLINE const BTDelay_Template*  BTDelay::getTemplate() const {return static_cast<const BTDelay_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTDELAY_H_