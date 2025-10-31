#ifndef _ITF_RAY_BTACTIONHITTARGET_H_
#define _ITF_RAY_BTACTIONHITTARGET_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

    class Ray_BTActionHitTarget : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionHitTarget,BTAction,4242497727);

    public:

        Ray_BTActionHitTarget();
        ~Ray_BTActionHitTarget();

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );
        virtual void                            onActivate();
        virtual void                            onDeactivate();
        virtual bbool                           update( f32 _dt );
        virtual void                            updateAnimInput();
        virtual void                            onEvent( Event* _event );

    private:

        struct HitQueueInfo
        {
            ObjectRef                   m_receiver;
            ObjectRef                   m_closestPoly;
            u32                         m_closestEdgeIndex;
            Vec2d                       m_pos;
            Vec2d                       m_dir;
            f32                         m_dist;
            f32                         m_animDist;
            bbool                       m_fail;
            bbool                       m_blocks;
            PhysContactsContainer       m_contacts;
            StringID                    m_contactMaterial;
        };

        struct MemorizedHitInfo
        {
            ObjectRef                   m_receiver;
            ObjectRef                   m_poly;
            u32                         m_edgeIndex;
        };

        typedef FixedArray <HitQueueInfo,3> HitQueue;
        typedef FixedArray <MemorizedHitInfo,5> MemorizedQueue;

        const class Ray_BTActionHitTarget_Template*  getTemplate() const;

        void                                    chooseAttack();

        bbool                                   updateChargePunch( f32 _dt, u32 _level, f32 _push, f32 _duration );
        bbool                                   updateCrouchKick( f32 _dt, u32 _level, f32 _push, f32 _duration );
        bbool                                   updateRepeating( f32 _dt, u32 _level, f32 _push, f32 _duration );

        void                                    prepareHit( AnimPolylineEvent* _animPolyEvent );
        void                                    fillHitQueue( bbool _updateAnimValues, const Vec2d& _hitDir );
        void                                    updateAnimValuesFromQueue();
        void                                    calculateHitShapeFromEvent( AnimPolylineEvent* _animPolyEvent );
        void                                    calculateHitShapeFromPolyLine( const Vec2d* _points, u32 _numPoints );
        void                                    calculateHitQueue( const Vec2d& _hitDir, bbool _checkCollision, HitQueue& _queue );
        void                                    releaseHitsFromQueue( PunchStim* _stim );
        void                                    processPerformHit();
        Vec2d                                   getBaseHitDir() const;
        PunchStim*                              setupPunchStim();
        void                                    hitQueueApplyFilter( const PhysContactsContainer& _allContacts,
                                                                     PhysContactsContainer& _filteredContacts, u32& _collisionIndex );
        void                                    setReceiveHitTypeOnStim( PunchStim* _stim ) const;
        Vec3d                                   getHitFxPos( const Vec3d& _default );
        Vec3d                                   getBonePos( u32 _index, const Vec3d& _default ) const;

        void                                    clearHit();

        class AnimatedComponent*                m_animComponent;
        class PhysComponent*                    m_physComponent;
        class FXControllerComponent*            m_fxControllerComponent;
        u32                                     m_currentAttack;
        u32                                     m_previousAttack;
        f32                                     m_timer;

        f32                                     m_currentPunchDistance;
        EPunchReleaseResult                     m_currentPunchResult;

        bbool                                   m_preparingHit;
        bbool                                   m_performingHit;
        bbool                                   m_hitSomethingThisRound;
        bbool                                   m_hitsReleased;
        bbool                                   m_punchUp;
        bbool                                   m_punchCharging;
        bbool                                   m_comboHitNext;
        u32                                     m_hitFxBoneIndex;
        u32                                     m_burstStage;
        HitQueue                                m_hitQueue;
        MemorizedQueue                          m_memorizedHits;
        PhysShapePolygon::PointsList            m_shapePoints;
        PhysShapePolygon                        m_shape;
    };

    class Ray_BTActionHitTarget_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionHitTarget_Template,BTAction_Template,1589912172);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionHitTarget);

    public:

        Ray_BTActionHitTarget_Template();
        ~Ray_BTActionHitTarget_Template() {}

        struct AttackData
        {
            AttackData();

            DECLARE_SERIALIZE();

            EPunchType              m_punchType;
            u32                     m_level;
            f32                     m_pushBackRadius;
            f32                     m_duration;
            StringID                m_anim;
        };

        typedef SafeArray <AttackData> AttackList;

        u32                         getNumAttacks() const { return m_attacks.size(); }
        const AttackData&           getAttack( u32 _index ) const { return m_attacks[_index]; }
        const StringID&             getHitFxBoneName() const { return m_hitFXBoneName; }

    private:

        AttackList                  m_attacks;
        StringID                    m_hitFXBoneName;
    };

    ITF_INLINE const Ray_BTActionHitTarget_Template*  Ray_BTActionHitTarget::getTemplate() const {return static_cast<const Ray_BTActionHitTarget_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONHITTARGET_H_
