#ifndef _ITF_BTACTIONCOPYFACT_H_
#define _ITF_BTACTIONCOPYFACT_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_FACT_H_
#include "gameplay/ai/BTs/Fact.h"
#endif // _ITF_FACT_H_

namespace ITF
{

    class BTActionCopyFact : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(BTActionCopyFact,BTAction,1556541169);

    public:

        BTActionCopyFact();
        ~BTActionCopyFact() {}

        virtual bbool                           validate( Actor* _actor ) const;
        virtual void                            onActivate();
        virtual bbool                           update( f32 _dt );

    private:

        const class BTActionCopyFact_Template*  getTemplate() const;
    };

    class BTActionCopyFact_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(BTActionCopyFact_Template,BTAction_Template,1389052205);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(BTActionCopyFact);

    public:

        BTActionCopyFact_Template() {}
        ~BTActionCopyFact_Template() {}

        const StringID&             getSrc() const { return m_src; }
        const StringID&             getDst() const { return m_dst; }

    private:

        StringID                    m_src;
        StringID                    m_dst;
    };

    ITF_INLINE const BTActionCopyFact_Template*  BTActionCopyFact::getTemplate() const {return static_cast<const BTActionCopyFact_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONCOPYFACT_H_