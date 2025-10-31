#ifndef _ITF_AIFALLACTION_H_
#define _ITF_AIFALLACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class StickToPolylinePhysComponent;

    class AIFallAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIFallAction, AIAction,618718479);
        DECLARE_VALIDATE_ACTION()

    public:

        AIFallAction();
        virtual ~AIFallAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        update( f32 _delta );

    private:

        StickToPolylinePhysComponent*   m_physComponent;
    };

    class AIFallAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIFallAction_Template,AIAction_Template,2620008885);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIFallAction);

    public:

        AIFallAction_Template();
        ~AIFallAction_Template(){}


    private:
    };
}

#endif // _ITF_AIFALLACTION_H_