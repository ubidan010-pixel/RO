#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_STARGATENODECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_StargateNodeComponent.h"
#endif //_ITF_RAY_STARGATENODECOMPONENT_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_StargateNodeComponent)

BEGIN_SERIALIZATION_CHILD(Ray_StargateNodeComponent)
END_SERIALIZATION()

Ray_StargateNodeComponent::Ray_StargateNodeComponent()
{
}

Ray_StargateNodeComponent::~Ray_StargateNodeComponent()
{
}

//*****************************************************************************
// Template
//*****************************************************************************

IMPLEMENT_OBJECT_RTTI(Ray_StargateNodeComponent_Template)

BEGIN_SERIALIZATION(Ray_StargateNodeComponent_Template)
    
    SERIALIZE_ENUM_BEGIN("type",m_type);
        SERIALIZE_ENUM_VAR(STARGATENODETYPE_POINT);
        SERIALIZE_ENUM_VAR(STARGATENODETYPE_TELEPORT);
    SERIALIZE_ENUM_END();
    SERIALIZE_MEMBER("fadeTime",m_fadeTime);
    SERIALIZE_MEMBER("fadeLength",m_fadeLength);
    SERIALIZE_MEMBER("timeMultiplier",m_timeMultiplier);

END_SERIALIZATION()

Ray_StargateNodeComponent_Template::Ray_StargateNodeComponent_Template()
: m_type(STARGATENODETYPE_POINT)
, m_fadeTime(0.f)
, m_fadeLength(1.f)
, m_timeMultiplier(1.f)
{
}

Ray_StargateNodeComponent_Template::~Ray_StargateNodeComponent_Template()
{
}

};
