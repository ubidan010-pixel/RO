#include "precompiled_gameplay.h"

#ifndef _ITF_GAMEPLAYSTIMS_H_
#include "gameplay/GameplayStims.h"
#endif //_ITF_GAMEPLAYSTIMS_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(EventStim)
IMPLEMENT_OBJECT_RTTI(HitStim)
IMPLEMENT_OBJECT_RTTI(WindStim)
IMPLEMENT_OBJECT_RTTI(SolidStim)
IMPLEMENT_OBJECT_RTTI(EventStimNotify)

BEGIN_SERIALIZATION_CHILD(EventStim)
END_SERIALIZATION()

void EventStim::reset()
{
    resetContacts();
    m_stopProcessing = bfalse;
}

BEGIN_SERIALIZATION_CHILD(HitStim)
    SERIALIZE_MEMBER("level",m_level);
    SERIALIZE_MEMBER("direction",m_dir);
    SERIALIZE_MEMBER("faction",m_faction);
END_SERIALIZATION()

void HitStim::reset()
{
 	Super::reset();
    m_originalSender.invalidate();
    m_level = 0;
    m_dir = Vec2d::Zero;
    m_fxPos = Vec3d::Zero;
    m_faction = U32_INVALID;
}

BEGIN_SERIALIZATION_CHILD(WindStim)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(SolidStim)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventStimNotify)
END_SERIALIZATION()

}
