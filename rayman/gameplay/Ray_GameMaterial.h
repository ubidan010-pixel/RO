#ifndef _ITF_RAY_GAMEMATERIAL_H_
#define _ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

namespace ITF
{
	///////////////////////////////////////////////////////////////////////
	// Ray_GameMaterial_Template
	///////////////////////////////////////////////////////////////////////
	class Ray_GameMaterial_Template : public GameMaterial_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_GameMaterial_Template, GameMaterial_Template,2777394804);
		DECLARE_SERIALIZE()

	public:
		Ray_GameMaterial_Template()
            : Super()
            , m_bounce(bfalse)
            , m_bounceType(BOUNCETYPE_NONE)
            , m_wave(bfalse)
            , m_climbable(bfalse)
            , m_hangable(btrue)
            , m_ignoreCorners(bfalse)
            , m_character(bfalse)
            , m_ignoreLowRoof(bfalse)
            , m_canFruitStick(bfalse)
            , m_climbVertical(bfalse)
            , m_climbSlide(bfalse)
            , m_climbForceSideJump(bfalse)
            , m_noSquashDamage(bfalse)
            , m_bounceOnCrunchAttack(bfalse)
            , m_shooterBounce(btrue)
            , m_movePlatformSpeedXMult(0.0f)
            , m_movePlatformSpeedYMult(0.0f)
            , m_dangerous(0)
            , m_djembe(0)
        {}
		Ray_GameMaterial_Template(const Path& _path)
			: Super(_path)
			, m_bounce(bfalse)
            , m_bounceType(BOUNCETYPE_NONE)
            , m_wave(bfalse)
			, m_climbable(bfalse)
            , m_hangable(btrue)
			, m_ignoreCorners(bfalse)
			, m_character(bfalse)
			, m_ignoreLowRoof(bfalse)
			, m_canFruitStick(bfalse)
			, m_climbVertical(bfalse)
            , m_climbSlide(bfalse)
            , m_climbForceSideJump(bfalse)
			, m_noSquashDamage(bfalse)
            , m_shooterBounce(btrue)
			, m_bounceOnCrunchAttack(bfalse)
            , m_movePlatformSpeedXMult(0.0f)
            , m_movePlatformSpeedYMult(0.0f)
			, m_dangerous(0)
            , m_djembe(0)
		{}

		
		ITF_INLINE bbool	getIsBounce()				const { return m_bounce; }
        ITF_INLINE BounceType getBounceType()			const { return m_bounceType; }
        ITF_INLINE bbool	getIsWave()				    const { return m_wave; }
		ITF_INLINE bbool	getClimbable()			    const { return m_climbable; }
        ITF_INLINE bbool	getHangable()			    const { return m_hangable; }
		ITF_INLINE bbool	ignoreCorners()			    const { return m_ignoreCorners; }
		ITF_INLINE bbool	isCharacter()				const { return m_character; }
		ITF_INLINE bbool	ignoreLowRoof()			    const { return m_ignoreLowRoof; }
		ITF_INLINE bbool	canFruitStick()			    const { return m_canFruitStick; }
		ITF_INLINE bbool	isClimbVertical()			const { return m_climbVertical; }
        ITF_INLINE bbool	isClimbSlide()			    const { return m_climbSlide; }
        ITF_INLINE bbool	isClimbForceSideJump()		const { return m_climbForceSideJump; }
		ITF_INLINE bbool	isNoSquashDamage()			const { return m_noSquashDamage; }
		ITF_INLINE bbool	bounceOnCrushAttack()	    const { return m_bounceOnCrunchAttack; }
		ITF_INLINE f32		getMovePlatformSpeedXMult() const { return m_movePlatformSpeedXMult;}
		ITF_INLINE f32		getMovePlatformSpeedYMult() const { return m_movePlatformSpeedYMult;}
		ITF_INLINE u32		getDangerousLevel()			const { return m_dangerous; }
        ITF_INLINE bbool	getShooterBounce()			const { return m_shooterBounce; }
        ITF_INLINE f32      getDjembe()                 const { return m_djembe; }

	protected:

		bbool			m_bounce;
        BounceType      m_bounceType;
        bbool           m_wave;
		bbool			m_climbable;
        bbool           m_hangable;
		bbool			m_ignoreCorners;
		bbool			m_character;
		bbool			m_ignoreLowRoof;
		bbool			m_canFruitStick;
		bbool			m_climbVertical;
        bbool			m_climbSlide;
        bbool           m_climbForceSideJump;
		bbool			m_noSquashDamage;
		bbool			m_bounceOnCrunchAttack;
        bbool		    m_shooterBounce;
		f32				m_movePlatformSpeedXMult;
		f32				m_movePlatformSpeedYMult;
		u32				m_dangerous;
        f32             m_djembe;
	};
}

#endif //_ITF_RAY_GAMEMATERIAL_H_

