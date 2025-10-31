#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CAGEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_CageAIComponent.h"
#endif //_ITF_RAY_CAGEAICOMPONENT_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_CageAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_CageAIComponent_Template)
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_CageAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_CageAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("CageIndex",m_cageIndex);
        END_CONDITION_BLOCK()
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_MEMBER("wasBrokenInSession", m_wasBrokenInSession);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()
    
} // namespace ITF

