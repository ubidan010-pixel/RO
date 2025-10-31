#ifndef _ITF_BTACTIONSETFACT_H_
#define _ITF_BTACTIONSETFACT_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_FACT_H_
#include "gameplay/ai/BTs/Fact.h"
#endif // _ITF_FACT_H_

namespace ITF
{

    class BTActionSetFact : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(BTActionSetFact,BTAction,943887586);

    public:

        BTActionSetFact();
        ~BTActionSetFact() {}

        virtual bbool                           validate( Actor* _actor ) const;
        virtual void                            onActivate();
        virtual bbool                           update( f32 _dt );

    private:

        const class BTActionSetFact_Template*  getTemplate() const;
    };

    class BTActionSetFact_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(BTActionSetFact_Template,BTAction_Template,2150309077);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(BTActionSetFact);

    public:

        BTActionSetFact_Template();
        ~BTActionSetFact_Template() {}

        const StringID&             getFact() const { return m_fact; }
        const String8&              getValue() const { return m_value; }
        EValueType                  getType() const { return m_type; }

    private:

        StringID                    m_fact;
        String8                     m_value;
        EValueType                  m_type;
    };

    ITF_INLINE const BTActionSetFact_Template*  BTActionSetFact::getTemplate() const {return static_cast<const BTActionSetFact_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONPLAYANIM_H_