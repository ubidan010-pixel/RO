#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMEPLAYSTIMS_H_
#include "rayman/gameplay/Ray_GameplayStims.h"
#endif //_ITF_RAY_GAMEPLAYSTIMS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PunchStim)
IMPLEMENT_OBJECT_RTTI(RehitStim)
IMPLEMENT_OBJECT_RTTI(BounceStim)
IMPLEMENT_OBJECT_RTTI(ScareStim)
IMPLEMENT_OBJECT_RTTI(VacuumStim)

BEGIN_SERIALIZATION_CHILD(PunchStim)
    SERIALIZE_RECEIVEDHITTYPE("hitType", m_receivedHitType);
    SERIALIZE_MEMBER("pushBackDistance",m_pushBackDistance);
    SERIALIZE_MEMBER("radial",m_radial);
    SERIALIZE_MEMBER("bounceMultiplier",m_bounceMultiplier);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(BounceStim)
    SERIALIZE_RAY_BOUNCETYPE("bounceType",m_bounceType);
    SERIALIZE_MEMBER("direction",m_dir);
    SERIALIZE_MEMBER("radial",m_radial);
    SERIALIZE_MEMBER("multiplier",m_multiplier);
END_SERIALIZATION()

BounceStim::BounceStim()
: m_bounceType(BOUNCETYPE_BUMPER)
, m_dir(Vec2d::Zero)
, m_multiplier(1.f)
, m_radial(bfalse)
, m_skipped(bfalse)
{
}

BounceStim::BounceStim( BounceType _bounceType, const Vec2d& _dir, f32 _multiplier, bbool _radial )
: m_bounceType(_bounceType)
, m_dir(_dir)
, m_multiplier(_multiplier)
, m_radial(_radial)
, m_skipped(bfalse)
{
}

///////////////////////////////////////////////////////////////////////////////////////////
StringID PunchStim::getActionFromHit() const
{
    bbool senderIsReduced = bfalse;

    if( m_receivedHitType != RECEIVEDHITTYPE_CHARGE_PUNCH )
    {
        BaseObject * const senderObject = m_sender.getObject();
        if( senderObject )
        {
            Actor * const senderActor = senderObject->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if( senderActor )
            {
                senderIsReduced = AIUtils::calculateCharacterSize( senderActor ) == CHARACTERSIZE_SMALL;
            }
        }

        if( !senderIsReduced )
        {
            switch (m_level)
            {
            case 0:
                return ITF_GET_STRINGID_CRC(hit_weak,1483138231);
            case 1:
                return ITF_GET_STRINGID_CRC(hit_strong,4029336239);
            default:
                return ITF_GET_STRINGID_CRC(hit_mega,3569460987);
            }
        }
        else
        {
            if( m_dir != Vec2d::Up )
            {
                return ITF_GET_STRINGID_CRC(hit_mini,1653061163);
            }
            else
            {
                return ITF_GET_STRINGID_CRC(hit_mini_up,3294396079);
            }
        }
    }
    else
    {
        return ITF_GET_STRINGID_CRC(hit_charge,1383656220);
    }
}

BEGIN_SERIALIZATION_CHILD(RehitStim)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(ScareStim)
END_SERIALIZATION()
BEGIN_SERIALIZATION_CHILD(VacuumStim)
END_SERIALIZATION()

}