
#ifndef _ITF_RAY_BABYPIRANHACOMPONENT_H_
#define _ITF_RAY_BABYPIRANHACOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

namespace ITF
{

    class Ray_BabyPiranhaComponent : public ActorComponent
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(Ray_BabyPiranhaComponent,ActorComponent,1117370284)
        DECLARE_SERIALIZE()

        DECLARE_VALIDATE_COMPONENT()

		enum State {State_Stand=0, State_Attack=1, State_Count,	
			ENUM_FORCE_SIZE_32(0)
		};//Enum must start with 0 and have consecutive values
		enum AttackType {Attack_Air=0, Attack_Dive=1, Attack_Count,	
		ENUM_FORCE_SIZE_32(1)
		}; //Enum must start with 0 and have consecutive values

        Ray_BabyPiranhaComponent();
        ~Ray_BabyPiranhaComponent();

        virtual bbool           needsUpdate() const { return btrue; }
        virtual bbool           needsDraw() const { return btrue; }
        virtual bbool           needsDraw2D() const { return bfalse; }
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActorClearComponents();
#ifdef ITF_SUPPORT_EDITOR
        virtual void            onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
        virtual void            Update( f32 _deltaTime );
        virtual void            Draw();
        virtual void            onUpdatePos();
        ObjectRef               getSwimPolyline() { return m_swimingPolyline; }
        u32                     getSwimPolylineSurfaceEdge() { return m_surfaceEdge; }
        f32                     getSwimPolylineSurfaceEdgeT() { return m_surfaceEdgeT; }
        
        bbool                   canAttackUp() const {return !m_isScaredByFruit;}
        void                    startAttackUp(f32 percentAttacking)     {startAttack(percentAttacking, Attack_Air); }
        void                    startAttackDown(f32 percentAttacking)   {startAttack(percentAttacking, Attack_Dive); }
        void                    startEscape();
        void                    stopAllAttacks();

        bbool                   isCollidingWithPiranha(Actor *_player, PhysShapeCircle *&_collidingPiranha, Vec2d &_collidingPos);

        void                    setPertubation(f32 _maxPerturbation) { m_maxPerturbation = _maxPerturbation; }

        void                    moveOnPolyline(f32 _delta); //move on swimming polyline by the given _delta

        ITF_INLINE bbool        canGoInsideWater() const;

        static bbool            hasPiranha( ObjectRef _waterPolyline );

    private:
        ITF_INLINE const class Ray_BabyPiranhaComponent_Template * getTemplate() const;

        typedef std::multimap<ObjectRef, ActorRef> PiranhaMap;
        static PiranhaMap s_registeredPiranhas;

        struct ParticleData
        {
            ParticleData()
                : m_relativSpeed(0.f)
                , m_speed(0.f)
                , m_standEdgeIndex(U32_INVALID)
                , m_standInEdgePos(0.f)
                , m_standPosFromActor(0.f)
                , m_idleRadius(0.f)
                , m_idleSpeed(0.f)
                , m_phase(0.f)
                , m_currentGenerator(State_Stand)
                , m_attackTime(0.f)
                , m_attackMoveTime(0.f)
                , m_attackMoveDst(0.f)
                , m_attackIndex(Attack_Air)
                , m_randomizedFruitInfluenceDist(0.f)
                , m_isInfluencedByFruit(bfalse)
                , m_xLimitMultiplier(0.f)
            {}

            f32         m_relativSpeed;     // The current position
            f32         m_speed;            // The current speed

            u32         m_standEdgeIndex;
            f32         m_standInEdgePos;
            f32         m_standPosFromActor;
            f32         m_idleRadius;
            f32         m_idleSpeed;

            f32         m_phase;
            State       m_currentGenerator;

            f32         m_attackTime;
            f32         m_attackMoveTime;
            f32         m_attackMoveDst;
            AttackType  m_attackIndex;

            Vec2d       m_lastPos;
            Angle       m_lastAngle;

            f32         m_randomizedFruitInfluenceDist;
            bbool       m_isInfluencedByFruit;
            f32         m_xLimitMultiplier;
        };


        void                    clear();
        void                    initParticles();

        void                    findFruits(ActorList &_fruits);

        void                    startAttack(f32 percentAttacking, AttackType type);
        void                    updateGenerator( f32 _deltaTime );
        bbool                   updateParticles( f32 _deltaTime ); //return btrue if should not attack
        bbool                   updateParticlesStand( ITF_ParticleGenerator* _gen, const i32 _index, const f32 _dt, Vec2d& _pos, const bbool _updateSpeed );
        bbool                   updateParticlesAttack( ITF_ParticleGenerator* _gen, const i32 _index, const f32 _dt, Vec2d& _pos );
        void                    changeParticleMode( u32 index, State mode, AttackType _attackIndex );
        void                    processAttack();
        bbool                   applyFruitInfluence(Vec2d &_pos, const ParticleData &_data, ActorList &_fruits);
        static void             useSpeedLimit(Vec2d &_pos, const Vec2d &_lastPos, f32 _speed, f32 _deltaTime);
        void                    setSwimingPolyline( PolyLine& _polyline );
        void                    clearSwimingPolyline();

        class PhysComponent*    m_physComponent;
        ObjectRef               m_swimingPolyline;
        class PolyLine*         m_inPolyline;
        
        f32                     m_pertubationRemaing;
        f32                     m_pertubationDec;
        f32                     m_maxPerturbation;
        f32                     m_rotationBlendFactor;

        typedef SafeArray <ParticleData> ParticlesContainer;

        ResourceID              m_resource;
        f32                     m_startRadius;


        // computed params
        u32                     m_numParticles;
        f32                     m_maxXFromCenter;
        f32                     m_minXFromCenter;

        u32                     m_nbAttacking[Attack_Count]; // index 0=up, 1=down.  Piranha attack players both at upper(in air) or lower(in water) positions
        u32                     m_maxAttacking[Attack_Count];

        u32                     m_nbDownAttacking;
        u32                     m_maxDownAttacking;

        Vec2d                   m_lastPos;
        Angle                   m_lastAngle;

        u32                     m_surfaceEdge;
        u32                     m_closestEdge;
        f32                     m_surfaceEdgeT;
        f32                     m_closestEdgeT;
        bbool                   m_inside;
        bbool                   m_onSurface;
        bbool                   m_escaping;
        bbool                   m_isScaredByFruit;
        bbool                   m_canLimitSpeed;

        ParticlesContainer                  m_particles;
        ITF_ParticleGenerator               m_particleGeneratorStand;
        ITF_ParticleGenerator               m_particleGeneratorAttack;
        
        SafeArray<ITF_ParticleGenerator *>  m_particleGeneratorList;
    };

    //-------------------------------------------------------------------------------------
    class Ray_BabyPiranhaComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BabyPiranhaComponent_Template,TemplateActorComponent,4085371851);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BabyPiranhaComponent);

    public:

        Ray_BabyPiranhaComponent_Template();
        ~Ray_BabyPiranhaComponent_Template() {}

        ITF_INLINE const ITF_ParticleGenerator_Template & getParticleGeneratorStand() const { return m_particleGeneratorStand; }
        ITF_INLINE const ITF_ParticleGenerator_Template & getParticleGeneratorAttack() const { return m_particleGeneratorAttack; }
        ITF_INLINE const Path & getTextureFile() const { return m_textureFile; }
        ITF_INLINE f32 getIdleRadiusMin() const { return m_idleRadiusMin; }
        ITF_INLINE f32 getIdleRadiusMax() const { return m_idleRadiusMax; }
        ITF_INLINE f32 getIdleSpeedMin() const { return m_idleSpeedMin; }
        ITF_INLINE f32 getIdleSpeedMax() const { return m_idleSpeedMax; }
        ITF_INLINE f32 getInertia() const { return m_inertia; }
        ITF_INLINE f32 getLimitMultiplier() const { return m_xLimitMultiplier; }
        ITF_INLINE f32 getDesyncRation() const { return m_desyncRation; }
        ITF_INLINE f32 getMinJumpTime() const { return m_minJumpTime; }
        ITF_INLINE f32 getMaxJumpTime() const { return m_maxJumpTime; }
        ITF_INLINE f32 getMinJumpDst() const { return m_minJumpDst; }
        ITF_INLINE f32 getMaxJumpDst() const { return m_maxJumpDst; }
        ITF_INLINE f32 getMinDiveTime() const { return m_minDiveTime; }
        ITF_INLINE f32 getMaxDiveTime() const { return m_maxDiveTime; }
        ITF_INLINE f32 getMinDiveDst() const { return m_minDiveDst; }
        ITF_INLINE f32 getMaxDiveDst() const { return m_maxDiveDst; }
        ITF_INLINE f32 getMinSpeed() const { return m_minSpeed; }
        ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }
        ITF_INLINE f32 getInsideLen() const { return m_insideLen; }
        ITF_INLINE f32 getAttackSlowDown() const { return m_attackSlowDown; }
        ITF_INLINE f32 getDstFromSurface() const { return m_dstFromSurface; }
        ITF_INLINE f32 getPertubationFrequence() const { return m_pertubationFrequence; }
        ITF_INLINE f32 getPerturbationRadiusRatio() const { return m_perturbationRadiusRatio; }
        ITF_INLINE f32 getFruitInfluenceDistMin() const { return m_fruitInfluenceDistMin; }
        ITF_INLINE f32 getFruitInfluenceDistMax() const { return m_fruitInfluenceDistMax; }
        ITF_INLINE f32 getSpeedFractionWhenNearFruit() const { return m_speedFractionWhenNearFruit; }
        ITF_INLINE f32 getPirahnaRadius() const { return m_pirahnaRadius; }
        ITF_INLINE f32 getYOffset() const { return m_yOffset; }
        ITF_INLINE bbool getCanGoInsideWater() const { return m_canGoInsideWater; }
    private:

        ITF_ParticleGenerator_Template              m_particleGeneratorStand;
        ITF_ParticleGenerator_Template              m_particleGeneratorAttack;
        Path                                        m_textureFile;
        f32                                         m_idleRadiusMin;
        f32                                         m_idleRadiusMax;
        f32                                         m_idleSpeedMin;
        f32                                         m_idleSpeedMax;
        f32                                         m_inertia;
        f32                                         m_xLimitMultiplier;
        f32                                         m_desyncRation;
        f32                                         m_minJumpTime;
        f32                                         m_maxJumpTime;
        f32                                         m_minJumpDst;
        f32                                         m_maxJumpDst;
        f32                                         m_minDiveTime;
        f32                                         m_maxDiveTime;
        f32                                         m_minDiveDst;
        f32                                         m_maxDiveDst;
        f32                                         m_minSpeed;
        f32                                         m_maxSpeed;
        f32                                         m_insideLen;
        f32                                         m_attackSlowDown;
        f32                                         m_dstFromSurface;
        f32                                         m_pertubationFrequence;
        f32                                         m_perturbationRadiusRatio;
        f32                                         m_fruitInfluenceDistMin;
        f32                                         m_fruitInfluenceDistMax;
        f32                                         m_speedFractionWhenNearFruit;
        f32                                         m_pirahnaRadius;
        f32                                         m_yOffset;
        bbool m_canGoInsideWater;
    };

    ITF_INLINE const class Ray_BabyPiranhaComponent_Template * Ray_BabyPiranhaComponent::getTemplate() const
    {
        return static_cast<const class Ray_BabyPiranhaComponent_Template *>(m_template);
    }

    ITF_INLINE bbool Ray_BabyPiranhaComponent::canGoInsideWater() const
    {
        return getTemplate()->getCanGoInsideWater();
    }

}

#endif // RAY_BABYPIRANHACOMPONENT
        