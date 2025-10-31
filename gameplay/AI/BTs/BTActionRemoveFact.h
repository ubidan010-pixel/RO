#ifndef _ITF_BTACTIONREMOVEFACT_H_
#define _ITF_BTACTIONREMOVEFACT_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

class BTActionRemoveFact : public BTAction
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionRemoveFact,BTAction,1401740787);

public:

    BTActionRemoveFact();
    ~BTActionRemoveFact() {}

    virtual bbool                           validate( Actor* _actor ) const;
    virtual void                            onActivate();
    virtual bbool                           update( f32 _dt );

private:

    const class BTActionRemoveFact_Template*  getTemplate() const;
};

class BTActionRemoveFact_Template : public BTAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionRemoveFact_Template,BTAction_Template,1011214178);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTActionRemoveFact);

public:

    BTActionRemoveFact_Template() {}
    ~BTActionRemoveFact_Template() {}

    const StringID&             getFact() const { return m_fact; }

private:

    StringID                    m_fact;
};

ITF_INLINE const BTActionRemoveFact_Template*  BTActionRemoveFact::getTemplate() const {return static_cast<const BTActionRemoveFact_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONPLAYANIM_H_