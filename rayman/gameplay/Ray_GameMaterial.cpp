#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

namespace ITF
{
	///////////////////////////////////////////////////////////////////////
	// Ray_GameMaterial_Template
	///////////////////////////////////////////////////////////////////////
	IMPLEMENT_OBJECT_RTTI(Ray_GameMaterial_Template)
	BEGIN_SERIALIZATION_CHILD(Ray_GameMaterial_Template)
		SERIALIZE_MEMBER("Bounce",m_bounce);
        SERIALIZE_BOUNCETYPE("BounceType",m_bounceType);
        SERIALIZE_MEMBER("Wave",m_wave);
		SERIALIZE_MEMBER("climbable",m_climbable);
        SERIALIZE_MEMBER("hangable",m_hangable);
		SERIALIZE_MEMBER("IgnoreCorners",m_ignoreCorners);
		SERIALIZE_MEMBER("Character",m_character);
		SERIALIZE_MEMBER("IgnoreLowRoof",m_ignoreLowRoof);
		SERIALIZE_MEMBER("CanFruitStick",m_canFruitStick);
		SERIALIZE_MEMBER("climbVertical",m_climbVertical);
        SERIALIZE_MEMBER("climbSlide",m_climbSlide);
        SERIALIZE_MEMBER("climbForceSideJump",m_climbForceSideJump);
		SERIALIZE_MEMBER("noSquashDamage",m_noSquashDamage);
		SERIALIZE_MEMBER("bounceOnCrushAttack",m_bounceOnCrunchAttack);
		SERIALIZE_MEMBER("Dangerous",m_dangerous);
		SERIALIZE_MEMBER("movePlatformSpeedXMultiplier",m_movePlatformSpeedXMult);
		SERIALIZE_MEMBER("movePlatformSpeedYMultiplier",m_movePlatformSpeedYMult);
        SERIALIZE_MEMBER("shooterBounce",m_shooterBounce);
        SERIALIZE_MEMBER("djembe", m_djembe);
	END_SERIALIZATION()

} // namespace ITF


