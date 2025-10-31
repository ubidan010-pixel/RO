#ifndef _ITF_AIUTILS_H_
#define _ITF_AIUTILS_H_

#ifndef _ITF_FACTIONMANAGER_H_
#include "gameplay/managers/FactionManager.h"
#endif //_ITF_FACTIONMANAGER_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h" // because of the usage of Color() class
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

namespace ITF
{
    class Actor;
    class PolyLine;
    class PolyLineEdge;
    class PhysShape;
    class HitStim;
    class PhysShapePolygon;
    class AnimPolylineEvent;
    class AnimLightComponent;
    class AnimatedComponent;
    class LinkComponent;

    namespace AIUtils
    {
        bbool setRadialAlpha( Pickable* pickable, Vec2d _pos,const f32 _alphaRadialProgressionSpeed, const f32 _time, const f32 _alphaFadeTime, const bbool _in  );
        f32 calculateAlpha( const f32 dist, const f32 _alphaRadialProgressionSpeed, const f32 _time, const f32 _alphaFadeTime, const bbool _in );
        void setAlpha(Pickable * _pickable, f32 _value);
        Vec2d getLookDir( const f32 _angle, const bbool _flipped );
        ITF_INLINE Vec2d getLookDir( const Actor* _actor, const bbool _flipped ) { return getLookDir(_actor->getAngle(), _flipped); }
        Vec2d getLookDir( const Actor* _actor );
        Vec2d getOffsetPosition( Actor* _actor, bbool _flipped, const Vec2d& _localOffset );
        bbool getFlipped( Actor* _actor );
        void getAngleAndFlipped( const Vec2d& _lookDir, f32& _angle, bbool& _flipped );
        Color getColor( const Actor* _actor );
        void setAlwaysActive( Actor* _bodyPart, const bbool _active );

        Vec2d mirrorVector( const Vec2d& _edge, const Vec2d& _dir, f32 _limit );

        Actor* getActor( ObjectRef _ref );
        Actor* getActorFromContact( ObjectRef _ref );
        PolyLine* getPolyLine( ObjectRef _polyRef );
        const PolyLineEdge* getPolyLineEdge( ObjectRef _polyRef, u32 _edgeIndex );
        void getPolyLine( ObjectRef _polyRef, u32 _edgeIndex, PolyLine*& _polyLine, const PolyLineEdge*& _edge );
        f32  getEdgeFriction( const PolyLineEdge* _poly );

        Faction                             getFaction(const Actor* _actor);
        Faction                             getStimFaction(const HitStim * const stim);

        bbool                               isTarget( Faction _me, Faction _him );
        bbool                               isEnemy( Faction _me, Faction _him, bbool* _receiveDamage = NULL );
        bbool                               isEnemyHit( const HitStim* _hit, Faction _receiverFaction, bbool* _receiveDamage = NULL );
        bbool                               isDamageHit( const HitStim* _hit, Faction _receiverFaction );

        void getActorsByInteraction( Faction _myFaction, Interaction _interaction, const DepthRange& _depthRange, ActorList& _actors );
        void getEnemies( Faction _myFaction, const DepthRange& _depthRange, ActorList& _enemies );
        void getLivePlayers( const DepthRange& _depthRange, ActorList& _players );
        void getLivePlayers( ActorList& _players );
        void getLivePlayers(const AABB & _range, ActorList& _players);
        void getLivePlayersForGameMode( ActorList& _players, GameMode _mode );
        void getPlayers( const DepthRange& _depthRange, ActorList& _players );
        void getPlayers( ActorList& _players );

        void getActorsInRange(Actor* _referenceActor, f32 _range, ActorList& _actors);
        void getSortedPlayersInRange(Actor* _referenceActor, f32 _range, ActorList& _players);
        void getSortedActorsInRange(Actor* _referenceActor, f32 _range, ActorList& _actors);

        Actor* getClosestLivePlayer( Actor* _referenceActor );
        Actor* getClosestLivePlayer(Actor * _referenceActor, const AABB & _range);
        Actor* getClosestLivePlayer(const Vec2d & _position, const DepthRange & _depth, const bbool _flipped, const AABB & _range);
        Actor* getClosestLivePlayer(const Vec2d & _position, const DepthRange & _depth, const PhysShapePolygon * const _range);
        Actor* getLeadLivePlayer( Actor* _referenceActor );

        Actor* getClosestActor( const ActorList& _actors,
                                Actor* _actor, bbool _flipped,
                                f32 _range, bbool _detectBehind = btrue,
                                u32* _actorCount = NULL,
                                bbool _mustHaveOppositeFlip = bfalse );
        Actor* getClosestActor( Interaction _interaction,
                                Actor* _actor, bbool _flipped,
                                f32 _range, bbool _detectBehind = btrue,
                                u32* _actorCount = NULL,
                                bbool _mustHaveOppositeFlip = bfalse );
        Actor* getClosestActor( const ActorList& _actors,
                                Actor* _actor, bbool _flipped,
                                const AABB& _range,
                                u32* _actorCount = NULL );
        Actor* getClosestActor( Interaction _interaction,
                                Actor* _actor, bbool _flipped,
                                const AABB& _range,
                                u32* _actorCount = NULL );
        Actor* getClosestActor( const ActorList& _actors,
                                const Vec2d & _position,
                                const bbool _flipped,
                                const AABB& _range,
                                u32* _actorCount = NULL );
        Actor* getClosestActor( const ActorList& _actors,
                                const Vec2d & _position,
                                const PhysShapePolygon * const _range,
                                u32* _actorCount = NULL );
        bbool hasActorInRange( const ActorList& _actors,
                               Actor* _actor, bbool _flipped,
                               f32 _range, bbool _detectBehind = btrue,
                               bbool _mustHaveOppositeFlip = bfalse );
        bbool hasActorInRange( Interaction _interaction,
                               Actor* _actor, bbool _flipped,
                               f32 _range, bbool _detectBehind = btrue,
                               bbool _mustHaveOppositeFlip = bfalse );
        bbool hasActorInRange( Interaction _interaction,
                               Actor* _actor, bbool _flipped,
                               const AABB& _range );

        Actor* getClosestEnemy( Actor* _actor, bbool _flipped, f32 _range, bbool _detectBehind = btrue, u32* _enemyCount = NULL, bbool _mustHaveOppositeFlip = bfalse );
        Actor* getClosestEnemy( Actor* _actor, bbool _flipped, const AABB& _range, u32* _enemyCount = NULL );
        bbool hasEnemyInRange( Actor* _actor, bbool _flipped, f32 _range, bbool _detectBehind = btrue, bbool _mustHaveOppositeFlip = bfalse );
        bbool hasEnemyInRange( Actor* _actor, bbool _flipped, const AABB& _range );

        Vec2d getCameraDirection();
        Actor* getClosestPursuer(Actor* _actor, f32& _minDot);

        bbool isTargetStillValid ( Actor* _actor, Actor* _target );

        AABB getAbsoluteAABB( const AABB& _relativeAABB, const Vec2d& _absolutePos, bbool _flipped );


        f32 getRemainingDistanceOnEdge( const Vec2d &_start,  const Vec2d& _gravityDir, PolyLine* _poly,
                                        u32 _edgeIndex, bbool _left, f32 _minHeight,
                                        f32 _maxDist = 0.f );
        f32 getRemainingDistanceOnEdge( f32 _t, PolyLine* _poly, u32 _edgeIndex, bbool _left );
        f32 getDistanceBetweenEdgePoints( const PolyLine* _poly, u32 _edgeIndex1, f32 _edgePos1, u32 _edgeIndex2, f32 _edgePos2);
        bbool moveOnEdge( PolyLine* _poly, f32 & delta, PolyLine*& _newPoly, u32 & _edgeIndex, f32 & _edgePos, PolyLineEdge::HoleMode _stopOnHole);
        f32 getSlopeAngle( const Vec2d& _start, PolyLine* _poly, u32 _edgeIndex,
                           bbool _left, f32 _distCheck, const Vec2d& _gravityDir, u32* _slopeEdgeIndex );
        bbool searchEdgeAngle( const Vec2d& _start, PolyLine* _poly, u32 _edgeIndex, bbool _left, f32 _distCheck, const Vec2d& _gravityDir,
                               f32 _minAngle, f32 _maxAngle,
                               PolyLine** _foundPoly, u32* _foundEdge );
        bbool hasWallInFront( const Vec2d& _start, bbool _left,
                              PolyLine* _poly, u32 _edgeIndex,
                              f32 _maxX, f32 _maxY,
                              f32& _wallHeight,
                              bbool m_drawDebug = bfalse );
        bbool hasWallInFront( Actor* _actor, bbool _walkLeft, PolyLine* _poly,
                              u32 _startEdge, f32 _searchDist, f32 _radius, const Vec2d& _gravityDir,
                              f32& _wallHeight, const PolyLineEdge** _wallEdge = NULL , Vec2d * _wallTop = NULL);
        bbool hasHoleInFront( Actor* _actor, bbool _walkLeft, PolyLine* _poly,
                              u32 _startEdge, f32 _searchDist, f32 _radius, const Vec2d& _gravityDir,
                              f32& _holeDepth );

        bbool isWall( bbool _left, const Vec2d & _startPos, PolyLine * _poly, u32 _startEdge, f32 _searchDistance, const Vec2d & _gravityDir, f32 & _wallHeight, Vec2d & _wallTop);

        Vec2d getFeetPos( Actor* _actor );

        ITF_INLINE bbool sortPlayersByScore(std::pair<u32,i32> _p1, std::pair<u32,i32> _p2) { return _p1.second > _p2.second; }

        enum EdgeType
        {
            EdgeType_Invalid,
            EdgeType_Floor,
            EdgeType_Wall,
            EdgeType_Ceiling,
        };

        EdgeType getEdgeType(const PolyLineEdge & _edge);
        EdgeType getEdgeType(const Vec2d & _dir);
        Color getEdgeColor(const PolyLineEdge & _edge);
        Color getEdgeColor(EdgeType _type);

        u32 getNextEdgeInfo( const Vec2d& _start, PolyLine* _poly, u32 _edgeIndex,
            bbool _left, const Vec2d& _gravityDir, AIUtils::EdgeType & _edgeType, 
            f32 & _distanceToEdge, f32 & _edgeLength, bbool & _up, PolyLine ** _nextPoly = NULL);

        struct ContactWithDist
        {
            ContactWithDist()
                : m_dist(0.f)
            {}
            ContactWithDist(SCollidableContact _contact, f32 _dist )
                : m_contact(_contact)
                , m_dist(_dist)
            {}
            SCollidableContact  m_contact;
            f32                 m_dist;
        };
        struct SortedContactInfo
        {
            ObjectRef           m_actor;
            ObjectRef           m_polyline;
            ObjectRef           m_collideable;
            FixedArray<ContactWithDist,15>  m_contacts;
        };

        typedef FixedArray <SortedContactInfo,20> SortedContactInfoArray;

        void sortContactsOnShape( Actor* _sender, const Vec2d& _dir,
                                  PhysShapePolygon& _polygon,
                                  const PhysContactsContainer& _contacts,
                                  SortedContactInfoArray& _result, f32& _shapeStart, f32& _shapeEnd );

        void getAdjacentEdge( PolyLine* _poly, u32 _currentEdge, bbool _right, PolyLine*& _adjPoly, u32& _adjEdge );

        struct PolylineRegionData
        {
            StringID        m_regionId;
            class PolyLine* m_polyline;
        };
        typedef FixedArray<PolylineRegionData,10> PolylineRegionList;

        class PolyLine* getPolylineRegion( const DepthRange& _depthRange, const StringID& _regionId, const Vec2d& _pos );
        void getPolylineRegions( const DepthRange& _depthRange, const Vec2d& _pos, PolylineRegionList& _regionList );
        bbool isWallRunEnabledOnPolyline( const PolyLineEdge* _curEdge, const PolyLineEdge* _prevEdge );
        bbool isWallRunEnabledOnPolyline( const Vec2d& _curEdge, const Vec2d& _prevEdge );
        int getEdgeOrientationType( const Vec2d& _dir, const Vec2d& _gravity );

        void getTravelExitPointWithActorFeet( const Vec3d& _pos, Vec3d & _res_Pos, Actor * _actor );
        Vec2d computeChangePageVerticalEjectForce( Actor* _actor, const Vec2d& _finalPoint );

        class HitShape
        {
        public:
            void setShape(AnimPolylineEvent * _event, AnimLightComponent * _animComponent, Actor * _actor);
            void setShape(StringID _polyID, AnimLightComponent * _animComponent, Actor * _actor);
            const PhysShapePolygon * getShape() const { return &m_shape;}
        private:
            PhysShapePolygon::PointsList    m_shapePoints;
            PhysShapePolygon                m_shape;
        };


        void growAABB( const LinkComponent* _linkComponent );
        void growAABB( const ObjBinding* _objBinding );
        
        bbool isAnimFinished( AnimLightComponent* _animComponent, AnimatedComponent* _animatedComponent );
        bbool isAnimLooped( AnimLightComponent* _animComponent, AnimatedComponent* _animatedComponent );
        bbool matchEvent( const Event* _registeredEvent, const Event* _receivedEvent );

#ifdef ITF_SUPPORT_DEBUGFEATURE
        void        debugDrawPolylineEdgeTypes( const PolyLine& _poly, const Vec2d& _offset = Vec2d::Zero, f32 _duration = 0 );
#endif // ITF_SUPPORT_DEBUGFEATURE

        f32 getSceneZFromPickable( const Pickable* _pickable );

        void disableCollision(Actor* _actor, bbool _disable = btrue);
        ITF_INLINE void enableCollision(Actor* _actor) { disableCollision(_actor, bfalse); }

        class LinkIterator
        {
        public:
            LinkIterator( const LinkComponent* _linkComponent, bbool _ignoreErroneousChildren = btrue )
                : m_linkComponent(_linkComponent)
                , m_ignoreErroneousChildren(_ignoreErroneousChildren)
                , m_i(0)
            {
            }

            Actor*  getNextRelativeActor();
            Actor*  getNextRelativeActorTagFilter( const StringID& _tagFilter, bbool _exclude ); // if !_exclude, return children *with* the given tag
            Frise*  getNextRelativeFrise();
            void    reset() { m_i = 0; }

        private:
            const LinkComponent* m_linkComponent;
            bbool m_ignoreErroneousChildren;
            u32 m_i;
        };


        class ActorBindIterator
        {
        public:
            ActorBindIterator( const Actor* _actor, bbool _ignoreErroneousChildren = btrue )
                : m_actor(_actor)
                , m_ignoreErroneousChildren(_ignoreErroneousChildren)
                , m_i(0)
            {
            }

            Actor*  getNextChild();
            void    reset() { m_i = 0; }

        private:

            const Actor* m_actor;
            bbool m_ignoreErroneousChildren;
            u32 m_i;

        };

        void sendEventToBoundChildren( const Actor* _actor, Event* _event );
        void sendEventToLivePlayers(Event* _event);

        void addListenersToBoundFrises( Actor* _actor );
        void removeListenersToBoundFrises( Actor* _actor );


        f32 walkCalculateEfficiency( f32 _currentSpeed, f32 _maxSpeed, f32 _frictionCoef, f32 _gravity, const Vec2d& _gravityDir, f32 _angle,
                                     f32 _minGroundAngleEfficiencyAngle, f32 _maxGroundAngleEfficiencyAngle,
                                     f32 _minGroundAngleEfficiencyMult, f32 _maxGroundAngleEfficiencyMult,
                                     bbool _useGroundAngle );

        bbool isContactBlockingObstacle( const Vec2d& _edgePos0, const Vec2d& _edgeDir, f32 _edgeLength, const class GameMaterial_Template* _gameMaterial,
                                         PolyLine* _poly, const PolyLineEdge* _edge, f32 _t,
                                         const SCollidableContact& _contact, const Vec2d& _moveDir, f32 _radius,
                                         ObjectRef _stickedPoly, u32 _stickedEdge, f32 _relDist, const Vec2d& _gravityDir, bbool _treatCornerCase );

        // Gets the information of the adjacent edge
        struct AdjacentEdgeData
        {
            PolyLine* m_polyline;           // The polyline of the adjacent edge
            Vec2d m_adjacentEdgeDir;        // The direction of the adjacent edge
            Vec2d m_adjacentEdgeNormal;     // The normal of the adjacent edge
            Vec2d m_adjacentEdgePos;        // The first vertex of the adjacent edge
            Vec2d m_edgesCommonPoint;       // The common point between the two edges
            Vec2d m_normalA;                // The start normal of the edge from where we transition to the adjacent edge
            Vec2d m_normalB;                // The end normal of the edge from where we transition to the adjacent edge
            f32 m_dotProdEdges;             // The dot product of both edges
            f32 m_crossProdEdges;           // The cross product of both edges (gives us the sign of the angle)
            f32 m_offset;                   // How much are we outside from the current edge
            f32 m_adjacentEdgeLength;       // The length of the adjacent edge
            f32 m_adjacentEdgeAngle;        // The angle of the adjacent edge
            u32 m_adjacentEdgeIndex;        // The index of the edge we are transferring to
        };

        void                getEdgeDataInTime( f32 _t, PolyLine* _poly, const PolyLineEdge& _edge, u32 _edgeIndex, Vec2d& _pos, Vec2d& _vec, Vec2d& _dir, f32& _length );
        void                getAdjacentEdgeData( PolyLine* _stickedLine, const PolyLineEdge& _edge, u32 _edgeIndex, const Vec2d& _gravityDir, bbool _back, f32 _t,
                                                 f32 _relDist, AdjacentEdgeData& _adjacentEdgeData );

        // Gets an adjacent edge date when there isn't one
        void                getImaginaryAdjacentEdge( bbool _left, const Vec2d& _gravityDir, Vec2d& _dir, Vec2d& _normal );

        // Return a position outside of collisions
        Vec2d               findFreeCollisionPosition( const Vec2d& _pos, f32 _radius, f32 _depth, bbool _iterate );

        // swiming info
        void getSwimingPolylineData( const PolyLine* _polyLine, const Vec3d& _pos, const Vec2d& _gravityDir, f32 _surfaceRange,
                                     u32& _surfaceEdge, f32& _surfaceEdgeT, bbool& _onSurface,
                                     u32& _closestEdge, f32& _closestEdgeT, bbool& _isInside );
               
        
        void sortOnDirection( const Vec2d& _direction, ActorList& _players );

        ECharacterSize calculateCharacterSize( f32 _initialScale, f32 _currentScale );
        ITF_INLINE ECharacterSize calculateCharacterSize( Actor * _actor) {return calculateCharacterSize(_actor->getWorldInitialScale().m_x, _actor->getScale().m_x);}

        f32 limit(const f32 _value, const f32 _mini, const f32 _max);

        void getNearestPolylineInfo(const Vec3d & _source, Vec3d& _defaultPos, f32& _defaultZ );

        //--------------------------------------------------------------------------
        /// Michel Smooth (TM)
        /// Blend from current position to target position with given smoothing
        /// factor. CAVEAT: framerate dependant! won't reach target!
        template <typename T>
        T michelSmooth(
            T& _pos,
            const T& _target,
            f32 _blendFactor
            );

        //--------------------------------------------------------------------------
        /// Double Michel Smooth (TM)
        /// Blend twice from current position to target position with given
        /// smoothing factors. Pass the previous mid-target, and store it for the
        /// next frame. CAVEAT: framerate dependant! won't reach target!
        template <typename T>
        T doubleMichelSmooth(
            T& _pos,
            T& _midTarget,
            const T& _target,
            f32 _midBlendFactor,
            f32 _blendFactor
            );

        //--------------------------------------------------------------------------
        /// Advanced Michel Smooth (TM)
        /// Blend from current position to target position depending on target's
        /// speed. This speed is smoothed over frames so pass the speed of the 
        /// previous frame as the second argument. CAVEAT: framerate dependant!
        /// won't reach target!
        template <typename T>
        T advancedMichelSmooth(
            T& _pos,
            T& _smoothedSpeed,
            T& _prevTarget,
            const T& _target,
            f32 _speedBlendFactor,
            f32 _speedMin,
            f32 _speedMax,
            f32 _blendFactorAtSpeedMin,
            f32 _blendFactorAtSpeedMax,
            f32 _dt
            );

    };  // namespace AIUTils

    //--------------------------------------------------------------------------
    template <typename T>
    T AIUtils::michelSmooth(
        T& _pos,
        const T& _target,
        f32 _blendFactor
        )
    {
        // interpolate between current and target position
        _pos = Interpolate(_pos, _target, _blendFactor);
        return _pos;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    T AIUtils::doubleMichelSmooth(
        T& _pos,
        T& _midTarget,
        const T& _target,
        f32 _midBlendFactor,
        f32 _blendFactor
        )
    {
        // mid-target "tries to reach" final target
        michelSmooth(_midTarget, _target, _midBlendFactor);

        // position "tries to reach" mid-target
        michelSmooth(_pos, _midTarget, _blendFactor);
        return _pos;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    T AIUtils::advancedMichelSmooth(
        T& _pos,
        T& _smoothedSpeed,
        T& _prevTarget,
        const T& _target,
        f32 _speedBlendFactor,
        f32 _speedMin,
        f32 _speedMax,
        f32 _blendFactorAtSpeedMin,
        f32 _blendFactorAtSpeedMax,
        f32 _dt
        )
    {
        // compute target speed
        T targetSpeed = (_target - _prevTarget) / _dt;
        _prevTarget = _target;

        // smooth target speed
        michelSmooth(_smoothedSpeed, targetSpeed, _speedBlendFactor);

        // compute speed coefficients: the more speed, the more blending
        f32 smoothedSpeedNorm = _smoothedSpeed.norm(); 
        f32 speedDelta = _speedMax - _speedMin;
        f32 speedCoeff = (smoothedSpeedNorm - _speedMin) / speedDelta;
        speedCoeff = Clamp(speedCoeff, 0.0f, 1.0f);

        // compute blend coefficient: always have a minimum of blending
        f32 blendDelta = _blendFactorAtSpeedMax - _blendFactorAtSpeedMin;
        f32 blendFactor = _blendFactorAtSpeedMin + speedCoeff * blendDelta;
        blendFactor = Clamp(blendFactor , 0.0f, 1.0f);

        // blend to target position
        _pos = michelSmooth(_pos, _target, blendFactor);
        return _pos;
    }
};


#endif // _ITF_AIUTILS_H_
