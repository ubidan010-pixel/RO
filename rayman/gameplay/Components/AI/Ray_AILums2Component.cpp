#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AILUMS2COMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AILums2Component.h"
#endif //_ITF_RAY_AILUMS2COMPONENT_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_AILums2Component_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AILums2Component_Template)
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_AILums2Component)

    BEGIN_SERIALIZATION_CHILD(Ray_AILums2Component)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_MEMBER("IsTaken",m_isTaken);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()
    
} // namespace ITF

