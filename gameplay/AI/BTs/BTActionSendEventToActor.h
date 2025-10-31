#ifndef _ITF_BTACTIONSENDEVENTTOACTOR_H_
#define _ITF_BTACTIONSENDEVENTTOACTOR_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_FACT_H_
#include "gameplay/ai/BTs/Fact.h"
#endif // _ITF_FACT_H_

namespace ITF
{

    class BTActionSendEventToActor : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(BTActionSendEventToActor,BTAction,2114685915);

    public:

        BTActionSendEventToActor();
        ~BTActionSendEventToActor();

        virtual void                            init( class BehaviorTree* _tree, const class BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual void                            onActivate();
        virtual bbool                           update( f32 _dt );

    private:

        const class BTActionSendEventToActor_Template*  getTemplate() const;

        Event*                                  m_event;
    };

    class BTActionSendEventToActor_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(BTActionSendEventToActor_Template,BTAction_Template,4110749335);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(BTActionSendEventToActor);

    public:

        BTActionSendEventToActor_Template();
        ~BTActionSendEventToActor_Template();

        StringID                            getTarget() const { return m_target; }
        const Event*                        getEvent() const { return m_event; }

    private:

        StringID                            m_target;
        Event*                              m_event;
    };

    ITF_INLINE const BTActionSendEventToActor_Template*  BTActionSendEventToActor::getTemplate() const {return static_cast<const BTActionSendEventToActor_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONSENDEVENTTOACTOR_H_

