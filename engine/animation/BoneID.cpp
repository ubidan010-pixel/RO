#include "precompiled_engine.h"

#ifndef _ITF_BONEID_H_
#include "engine/animation/BoneID.h"
#endif //_ITF_BONEID_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

namespace ITF
{
    void BoneID::init( AnimLightComponent* _anim )
    {
        m_index = _anim->getBoneIndex(*this);
    }

} // namespace ITF