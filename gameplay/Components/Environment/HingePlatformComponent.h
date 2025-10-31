#ifndef _ITF_HINGEPLATFORMCOMPONENT_H_
#define _ITF_HINGEPLATFORMCOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{
    class HingePlatformComponent : public PolylineComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(HingePlatformComponent,PolylineComponent,3937122948);

    public:

        DECLARE_SERIALIZE()

        HingePlatformComponent();
        virtual ~HingePlatformComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _deltaTime );
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        validate(bbool &_isComponentValidated);

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    protected:

        const class HingePlatformComponent_Template* getTemplate() const;

        struct HingeBoneData
        {
            HingeBoneData();

            // Parameters read from animscene or runtime
            u32                 m_index;                    // Bone index in the animscene
            u32                 m_leafIndex;                // The index in the leaf array if it's a leaf
            u32                 m_parentBoneIndex;          // Parent bone index in the animscene
            u32                 m_parentHingeIndex;         // Parent hinge index in m_hingeBones
            f32                 m_targetAngle;              // Target angle offset for bending
            f32                 m_currentAngle;             // Current angle offset for bending
            f32                 m_currentSpeed;             // The current angular speed
            Vec2d               m_currentScale;             // The current scale
            Vec2d               m_localGravity;             // The gravity in local space of the bone
            Vec2d               m_previousTarget;           // The previous position where this hinge was aiming
            SafeArray <u32>     m_children;                 // Indexes of children in m_hingeBones
            PolyLine*           m_polyline;                 // Polyline used for phantom
            PhysShapePolyline*  m_polyShape;                // Shape used for phantom
            class PhysPhantom*  m_phantom;                  // Phantom for the polyline to receive stims
        };

        struct PlatformData
        {
            DECLARE_SERIALIZE()

            PlatformData();

            // Parameters read from animscene or runtime
            u32                 m_scaleBoneIndex;
            u32                 m_scaleHingeIndex;
            u32                 m_linkBoneIndex;
            u32                 m_linkHingeIndex;
            f32                 m_currentScaleSpeed;        // The current scale speed value
            f32                 m_targetScale;              // The target scale of the platform
            PolyLine*           m_polyline;                 // Polyline used for phantom
            PhysShapePolyline*  m_polyShape;                // Shape used for phantom
            class PhysPhantom*  m_phantom;                  // Phantom for the polyline to receive stims
        };

        struct LeafData
        {
            u32                 m_hingeIndex;
            SafeArray <f32>     m_parentsAngle;             // Parents angle, used to transform the gravity to local space
        };

        typedef ITF_VECTOR <HingeBoneData> HingeBoneList;
        typedef ITF_VECTOR <LeafData> LeafList;
        typedef ITF_VECTOR <PlatformData> PlatformsList;

        virtual void            readModel();
        virtual void            processNewUser( class EventStickOnPolyline* _event );
        virtual void            processNewHanging( class EventHanging* _event );
        virtual void            processAnimUpdated( class EventAnimUpdated* _animUpdated );
        virtual void            processPhysBodyForceEvent( class EventForceTransfer* _physBodyForce );
        virtual void            processQueryBlockHits( EventQueryBlocksHits* _query );
        HingeBoneData*          findHingeDataFromBoneIndex( u32 _boneIndex, u32& _index );

        Vec2d                   transformVectorToLocalLeaf( const LeafData& _leafData, const Vec2d& _vector ) const;
        void                    storeLeafLocalAngle();
        void                    calculateCurrentWeights();
        void                    calculateTargetAngle();
        void                    addForces();
        void                    applyForces( f32 _dt );
        void                    updatePlatforms( f32 _delta );
        void                    updateTransforms( f32 _delta );
        void                    updateHinges();
        void                    updateMovingPolylines();
        void                    processMovingPolyline( f32 _forceMultiplier, f32 _resistance, ProceduralPolyline* _procPoly );
        void                    orderBonesRecursive( u32 _hingeIndex, ITF_VECTOR <bbool>& _processed );
        void                    applyForceOnPlatform( const Vec2d& _pos, const Vec2d& _force, f32 _maxPitch, ProceduralPolyline* _procPoly, PlatformData& _platform );
        f32                     getScaleForWeight( f32 _min, f32 _max, f32 _scaleMin, f32 _scaleMax, f32 _weight ) const;

        void                    clear();

        // stims
        virtual void            processHitStim( class HitStim* _hitStim );

        HingeBoneList           m_hingeBones;                   // Collection of the hinges
        PlatformsList           m_platforms;                    // Platforms where the player can land with bouncing behavior
        LeafList                m_leafs;                        // The terminal leafs of the tree
        SafeArray <f32>         m_currentWeights;               // The weights of each hinge
        SafeArray <f32>         m_currentForces;                // The forces of each hinge
        SafeArray <u32>         m_orderedBones;                 // The bones ordered from leafs to root
        Vec2d                   m_currentGravityDir;            // Gravity dir this frame
        f32                     m_thisFrameDt;                  // delta time on the current frame
        bbool                   m_hingeRegistered;              // If the component objects were registerd

        static const f32        m_strength2Angle;
    };

    //---------------------------------------------------------------------------------------------------

    class HingePlatformComponent_Template : public PolylineComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(HingePlatformComponent_Template,PolylineComponent_Template,187693019);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(HingePlatformComponent);

    public:

        class HingeBoneData
        {
            DECLARE_SERIALIZE()

        public:

            HingeBoneData();

            const StringID&     getName() const { return m_name; }
            const StringID&     getPolylineAnimRefName() const { return m_polylineAnimRef; }
            const Angle&        getMinAngle() const { return m_minAngle; }
            const Angle&        getMaxAngle() const { return m_maxAngle; }
            f32                 getWeight() const { return m_weight; }
            f32                 getResistance() const { return m_resistance; }
            f32                 getHitForce() const { return m_hitForce; }
            f32                 getWindMultiplier() const { return m_windMultiplier; }
            f32                 getWeightMultiplier() const { return m_weightMultiplier; }
            f32                 getBranchStiff() const { return m_branchStiff; }
            f32                 getBranchDamp() const { return m_branchDamping; }
            f32                 getDelayBranchMultiplier() const { return m_delayBranchMultiplier; }
            bbool               getDisableScale() const { return m_disableScale; }
            ITF_INLINE bbool    getDisableCollision() const { return m_disableCollision; }
            ITF_INLINE bbool    getAlwaysApplyAngleLimitation() const { return m_alwaysApplyAngleLimitation; }
            ITF_INLINE bbool    useDynamicBranchStiff() const { return m_useDynamicBranchStiff; }
            ITF_INLINE const Angle & getDynamicBranchStiffMinAngle() const { return m_dynamicBranchStiffMinAngle; }
            ITF_INLINE const Angle & getDynamicBranchStiffMaxAngle() const { return m_dynamicBranchStiffMaxAngle; }
            ITF_INLINE f32      getDynamicBranchStiffMultiplier() const { return m_dynamicBranchStiffMultiplier; }
            ITF_INLINE bbool    getDynamicBranchStiffOnlyWayBack() const { return m_dynamicBranchStiffOnlyWayBack; }

        private:

            // Parameters read from lua
            StringID            m_name;                         // name of the bone
            StringID            m_polylineAnimRef;              // The name of the polyline in the animation if it uses any
            Angle               m_minAngle;                     // minimum angle it will bend
            Angle               m_maxAngle;                     // maximum angle it will bend
            f32                 m_weight;                       // weight of the bone
            f32                 m_resistance;                   // resistance to bending
            f32                 m_hitForce;                     // Force when receiving a hit
            f32                 m_windMultiplier;               // Multiplier for the wind force
            f32                 m_weightMultiplier;             // Multiplier for weight
            f32                 m_branchStiff;                  // The branch bending stiffness
            f32                 m_branchDamping;                // The branch bending damping
            f32                 m_delayBranchMultiplier;        // Multiplier to apply delay on the branches as they move
            bbool               m_disableScale;                 // Forbid bone scaling
            bbool               m_disableCollision;             // Forbid collisions with bone
            bbool               m_alwaysApplyAngleLimitation;   // use max angle and min angle even if hinge bone is a leaf (and is updated by updatePlatforms)
            bbool               m_useDynamicBranchStiff;        // Make bone stiff change with bone angle
            Angle               m_dynamicBranchStiffMinAngle;   // For this angle stiff value will be unchanged
            Angle               m_dynamicBranchStiffMaxAngle;   // For this angle stiff value will be m_branchStiff * m_dynamicBranchStiffMultiplier
            f32                 m_dynamicBranchStiffMultiplier; // Stiff value multiplier (real multiplier value will be extrapolated between 1.0f at m_dynamicBranchStiffMinAngle and m_dynamicBranchStiffMultiplier at m_dynamicBranchStiffMaxAngle)
            bbool               m_dynamicBranchStiffOnlyWayBack;// Only use dynamic stiff multiplier when bone is going back to its origin angle
        };

        class PlatformData
        {
            DECLARE_SERIALIZE()

        public:

            PlatformData();

            const StringID&     getPolylineAnimRef() const { return m_polylineAnimRef; }
            const StringID&     getScaleBone() const { return m_scaleBone; }
            const StringID&     getLinkBone() const { return m_linkBone; }
            const Angle&        getMaxPitch() const { return m_maxPitch; }
            f32                 getScaleMinWeightScale() const { return m_scaleMinWeightScale; }
            f32                 getScaleMinWeight() const { return m_scaleMinWeight; }
            f32                 getScaleMaxWeightScale() const { return m_scaleMaxWeightScale; }
            f32                 getScaleMaxWeight() const { return m_scaleMaxWeight; }
            f32                 getMinScale() const { return m_minScale; }
            f32                 getScalingStiff() const { return m_scalingStiff; }
            f32                 getScalingDamp() const { return m_scalingDamping; }
            ITF_INLINE f32      getHingeMinWeight() const { return m_hingeMinWeight; }
            ITF_INLINE bbool    getDisableCollision() const { return m_disableCollision; }
            ITF_INLINE f32      getHitForceMultiplier() const { return m_hitForceMultiplier; }
            ITF_INLINE bbool    getOnlyCrushAttack() const { return m_onlyCrushAttack; }

        private:

            // Parameters read from lua
            StringID            m_polylineAnimRef;
            StringID            m_scaleBone;
            StringID            m_linkBone;
            Angle               m_maxPitch;
            f32                 m_scaleMinWeightScale;
            f32                 m_scaleMinWeight;
            f32                 m_scaleMaxWeightScale;
            f32                 m_scaleMaxWeight;
            f32                 m_minScale;
            f32                 m_scalingStiff;                 // The stiffness value for scaling the platforms
            f32                 m_scalingDamping;               // The damping value for scaling the platforms
            f32                 m_hingeMinWeight;               // Min weight to trigger hinge movement
            bbool               m_disableCollision;             // Forbid hits collisions with platforms
            f32                 m_hitForceMultiplier;           // hits force multiplier
            bbool               m_onlyCrushAttack;              // Platform can only be hit by a crush attack
        };

        class MovingPolylineData
        {
            DECLARE_SERIALIZE()

        public:

            MovingPolylineData();

            const StringID&     getPolyRef() const { return m_polyRef; }
            f32                 getResistance() const { return m_resistance; }
            f32                 getForceMultiplier() const { return m_forceMultiplier; }

        private:

            StringID            m_polyRef;
            f32                 m_resistance;
            f32                 m_forceMultiplier;
        };

        typedef ITF_VECTOR <HingeBoneData> HingeBoneList;
        typedef ITF_VECTOR <PlatformData> PlatformsList;
        typedef SafeArray <MovingPolylineData> MovingPolylines;

        HingePlatformComponent_Template() {}
        ~HingePlatformComponent_Template() {}

        const HingeBoneList&    getHingeBoneList() const { return m_hingeBones; }
        const PlatformsList&    getPlatformList() const { return m_platforms; }
        const MovingPolylines&  getMovingPolylines() const { return m_movingPolylines; }

        const PlatformData*     findPlatformDataFromPolylineRef( const StringID& _ref, u32& _index ) const;

    private:

        HingeBoneList           m_hingeBones;                   // Collection of the hinges
        PlatformsList           m_platforms;                    // Platforms where the player can land with bouncing behavior
        MovingPolylines         m_movingPolylines;              // Polylines that will move when characters pass fast
    };

    ITF_INLINE const HingePlatformComponent_Template* HingePlatformComponent::getTemplate() const
    {
        return static_cast<const HingePlatformComponent_Template*>(m_template);
    }
}

#endif // _ITF_HINGEPLATFORMCOMPONENT_H_
