#ifndef _ITF_BTACTIONSTAYIDLELOOKAT_H_
#define _ITF_BTACTIONSTAYIDLELOOKAT_H_

#ifndef _ITF_BTACTIONSTAYIDLE_H_
#include "gameplay/ai/BTs/BTActionStayIdle.h"
#endif // _ITF_BTACTIONSTAYIDLE_H_

namespace ITF
{

class BTActionStayIdleLookAt : public BTActionStayIdle
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionStayIdleLookAt,BTActionStayIdle,3543953953);

public:

    BTActionStayIdleLookAt();
    ~BTActionStayIdleLookAt() {}

    virtual bbool                               validate( Actor* _actor ) const;
    virtual void                                onActivate();
    virtual bbool                               update( f32 _dt );
    virtual void                                updateAnimInput();

private:

    const class BTActionStayIdleLookAt_Template*    getTemplate() const;

    void                                        lookAt();

    bbool                                       m_doingUTurn;
};

class BTActionStayIdleLookAt_Template : public BTActionStayIdle_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionStayIdleLookAt_Template,BTActionStayIdle_Template,760234255);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTActionStayIdleLookAt);

public:

    BTActionStayIdleLookAt_Template();
    ~BTActionStayIdleLookAt_Template() {}

    const StringID&         getPickableFactName() const { return m_pickableFact; }
    const StringID&         getPosFactName() const { return m_posFact; }

private:

    StringID                m_pickableFact;
    StringID                m_posFact;
};

ITF_INLINE const BTActionStayIdleLookAt_Template*  BTActionStayIdleLookAt::getTemplate() const {return static_cast<const BTActionStayIdleLookAt_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONSTAYIDLELOOKAT_H_

