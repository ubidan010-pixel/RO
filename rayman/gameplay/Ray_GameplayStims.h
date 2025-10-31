#ifndef _ITF_RAY_GAMEPLAYSTIMS_H_
#define _ITF_RAY_GAMEPLAYSTIMS_H_

#ifndef _ITF_GAMEPLAYSTIMS_H_
#include "gameplay/GameplayStims.h"
#endif //_ITF_GAMEPLAYSTIMS_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

namespace ITF
{
    class PunchStim : public HitStim
    {
        DECLARE_OBJECT_CHILD_RTTI(PunchStim, HitStim,200533519);
        DECLARE_SERIALIZE()

    public:

        PunchStim()
            : Super()
            , m_receivedHitType(RECEIVEDHITTYPE_FRONTPUNCH)
            , m_pushBackDistance(0.0f)
            , m_radial(bfalse)
            , m_identifier(0)
            , m_bounceMultiplier(1.f)
            , m_hitEnemiesOnce(bfalse)
        {}
        virtual ~PunchStim() {}

        void                        setReceivedHitType( EReceivedHitType _type ) { m_receivedHitType = _type; }
        EReceivedHitType            getReceivedHitType() const { return m_receivedHitType; }

        ITF_INLINE f32              getPushBackDistance() const { return m_pushBackDistance; }
        ITF_INLINE void             setPushBackDistance(f32 _val) { m_pushBackDistance = _val; }

        ITF_INLINE u32              getIsRadial() const { return m_radial; }
        ITF_INLINE void             setIsRadial( bbool _val) { m_radial = _val; }

        ITF_INLINE u32              getIdentifier() const { return m_identifier; }
        ITF_INLINE void             setIdentifier(u32 _val) { m_identifier = _val; }

        ITF_INLINE f32              getBounceMultiplier() const { return m_bounceMultiplier; }
        ITF_INLINE void             setBounceMultiplier( const f32 _value ) { m_bounceMultiplier = _value; }

        virtual StringID            getActionFromHit() const;

        ITF_INLINE bbool            getHitEnemiesOnce() const { return m_hitEnemiesOnce; }
        ITF_INLINE void             setHitEnemiesOnce(bbool _val) { m_hitEnemiesOnce = _val; }

    private:

        EReceivedHitType            m_receivedHitType;
        f32                         m_pushBackDistance;
        bbool                       m_radial;
        u32                         m_identifier;
        f32                         m_bounceMultiplier;
        bbool                       m_hitEnemiesOnce;
    };

    class BounceStim : public EventStim
    {
        DECLARE_OBJECT_CHILD_RTTI(BounceStim, EventStim,819805069);
        DECLARE_SERIALIZE()

    public:

        BounceStim();
        BounceStim( BounceType _bounceType, const Vec2d& _dir, f32 _multiplier, bbool _radial );
        ~BounceStim() {}

        ITF_INLINE BounceType       getBounceType() const { return m_bounceType; }
        ITF_INLINE void             setBounceType(BounceType _val) { m_bounceType = _val; }

        ITF_INLINE const Vec2d&     getBounceDir() const { return m_dir; }
        ITF_INLINE void             setBounceDir(const Vec2d& _val) { m_dir = _val; m_radial = bfalse; }

        ITF_INLINE u32              getIsRadial() const { return m_radial; }
        ITF_INLINE void             setIsRadial( bbool _val) { m_radial = _val; }

        ITF_INLINE f32              getMultiplier() const { return m_multiplier; }
        ITF_INLINE void             setMultiplier( f32 _val) { m_multiplier = _val; }

        ITF_INLINE bbool            getSkipped() const { return m_skipped; }
        ITF_INLINE void             setSkipped( bbool _val ) { m_skipped = _val; }

    private:

        BounceType      m_bounceType;
        Vec2d           m_dir;
        f32             m_multiplier;
        bbool           m_radial;
        bbool           m_skipped;
    };

    class RehitStim : public PunchStim
    {
        DECLARE_OBJECT_CHILD_RTTI(RehitStim, PunchStim,1008690800);
        DECLARE_SERIALIZE()
        ITF_INLINE ActorRef getRewardReceiver() const { return m_rewardReceiver; }
        ITF_INLINE void setRewardReceiver(ActorRef _ref) { m_rewardReceiver = _ref; }
    private:
        ActorRef m_rewardReceiver;
    };

    class ScareStim : public EventStim
    {
        DECLARE_OBJECT_CHILD_RTTI(ScareStim, EventStim,499118231);
        DECLARE_SERIALIZE()

    public:
    private:
    };

    class VacuumStim : public WindStim
    {
        DECLARE_OBJECT_CHILD_RTTI(VacuumStim, WindStim,593568508);
        DECLARE_SERIALIZE()

    public:
		VacuumStim() : m_vacuumDist( 0.0f ), m_vacuumSpeedFactor( 1.0f ) {}
		ITF_INLINE f32		getVacuumDist()	const				{ return m_vacuumDist; }
		ITF_INLINE void		setVacuumDist( f32 _dist )			{ m_vacuumDist = _dist; }
		ITF_INLINE f32		getVacuumSpeedFactor() const		{ return m_vacuumSpeedFactor; }
		ITF_INLINE void		setVacuumSpeedFactor( f32 _factor )	{ m_vacuumSpeedFactor = Max( 0.0f, _factor ); }
		
    private:
		f32	m_vacuumDist;
		f32 m_vacuumSpeedFactor;

    };
}

#endif // _ITF_RAY_GAMEPLAYSTIMS_H_