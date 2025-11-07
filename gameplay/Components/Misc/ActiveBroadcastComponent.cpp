#include "precompiled_gameplay.h"

#ifndef _ITF_ACTIVEBROADCASTCOMPONENT_H_
#include "gameplay/Components/Misc/ActiveBroadcastComponent.h"
#endif //_ITF_ACTIVEBROADCASTCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ActiveBroadcastComponent)

        BEGIN_SERIALIZATION_CHILD(ActiveBroadcastComponent)
        // no fields
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(ActiveBroadcastComponent)
        // nothing to validate
        END_VALIDATE_COMPONENT()

        // -------- Template (direct child of TemplateActorComponent) --------
        IMPLEMENT_OBJECT_RTTI(ActiveBroadcastComponent_Template)

        // IMPORTANT: use the root macro here (not _CHILD)
        BEGIN_SERIALIZATION(ActiveBroadcastComponent_Template)
        // no tunables yet
        END_SERIALIZATION()
}
