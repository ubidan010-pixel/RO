#ifndef _ITF_RAY_BOSSMORAYBODYPART_H_
#define _ITF_RAY_BOSSMORAYBODYPART_H_

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_RAY_SNAKEBODYPART_MIXED_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Mixed.h"
#endif //_ITF_RAY_SNAKEBODYPART_MIXED_H_

namespace ITF
{
    class Ray_BossMorayAIComponent;

    class Ray_BossMorayBodyPart : public BodyPart
    {
        DECLARE_OBJECT_CHILD_RTTI( Ray_BossMorayBodyPart, BodyPart ,4134625562)

    public:
        Ray_BossMorayBodyPart();
        virtual ~Ray_BossMorayBodyPart();

        virtual void        onInit();
        virtual void        clear();

        virtual void        onEvent( Event* _event );
        virtual void        update( f32 _dt );

        void                startBuboHit();
        void                startDeath();
        bbool               isPlayingHitAnim() const { return m_state == State_BuboHit; }

    protected:
        virtual Actor*      spawnBodyPart();

    private:
        ITF_INLINE const class Ray_BossMorayBodyPart_Template* getTemplate() const;
        Ray_BossMorayAIComponent* m_aiComponent;

        enum State
        {
            State_Invalid,
            State_Idle,
            State_Scream,
            State_BuboSpawn,
            State_BuboHit,
            State_FireMissile,
            State_CloseRangeAttack,
            State_Spike,
            State_Dash_In,
            State_Dash_Loop,
            State_Dash_Out,
            State_Death,
        };

        State m_state;
        State m_pendingState;
        void updateState();
        void changeState( const State _newState );
        void changeStateNextFrame( const State _newState ) { ITF_ASSERT_MSG(m_pendingState == State_Invalid, "queuing a state but we already have one pending!"); m_pendingState = _newState; }


        // BUBO
        StringID getCurrentBubo() const;
        void spawnBubo( const Vec3d& attachPos, const f32 attachAngle );
        void toggleBubo();
        void updateBubo();
        void processBuboHit( Event* _event );

        SpawneeGenerator m_buboSpawner;
        ActorRef m_bubo;


        // MISSILE
        void processFireMissile( const Ray_EventBossMorayFireMissile* _eventFire );
        void updateMissileSpawner();
        void dispatchMissiles( Event* _event );

        SpawneeGenerator m_missileSpawner;
        bbool m_missileSpawnRequested;
        u32 m_missileSpawnRegionIndex;
        ActorRefList m_missiles;


        // SPIKE
        void processSpike( EventPolylineBlockingContact* _contact );


        // CLOSE RANGE ATTACK
        void checkCloseRangeAttack();


        // DASH
        void processNodeReached( Ray_EventBossMorayNodeReached* _nodeReached );


        void getBoneCoord( const StringID& _boneID, Vec3d& _pos, f32& _angle );
    };

    //-------------------------------------------------------------------------------------
    typedef SafeArray<StringID> StringIDList;

    class Ray_BossMorayBodyPart_Template : public BodyPart_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossMorayBodyPart_Template,BodyPart_Template,4293489626);
        DECLARE_BODYPART_TEMPLATE(Ray_BossMorayBodyPart);
        DECLARE_SERIALIZE();

    public:

        Ray_BossMorayBodyPart_Template()
            : m_closeRangeAttackDistance(8.f)
        {}
        virtual ~Ray_BossMorayBodyPart_Template() {}

        ITF_INLINE const Path& getBuboPath() const { return m_buboPath; }
        ITF_INLINE const StringIDList& getBuboAttachBones() const { return m_buboAttachBones; }
        ITF_INLINE const Path& getMissilePath() const { return m_missilePath; }
        ITF_INLINE const StringID& getMissileBone() const { return m_missileBone; }
        ITF_INLINE f32 getCloseRangeAttackDistance() const { return m_closeRangeAttackDistance; }

    private:

        Path        m_buboPath;
        StringIDList m_buboAttachBones;
        Path        m_missilePath;
        StringID    m_missileBone;
        f32         m_closeRangeAttackDistance;
    };

    ITF_INLINE const class Ray_BossMorayBodyPart_Template* Ray_BossMorayBodyPart::getTemplate() const
    {
        return static_cast<const class Ray_BossMorayBodyPart_Template*>(m_template);
    }

}

#endif //_ITF_RAY_BOSSMORAYBODYPART_H_

