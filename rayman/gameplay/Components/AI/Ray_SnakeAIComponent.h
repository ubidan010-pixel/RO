#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#define _ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_

#ifndef _ITF_TRAJECTORYPROVIDER_H_
#include "gameplay/AI/Utils/TrajectoryProvider.h"
#endif //_ITF_TRAJECTORYPROVIDER_H_

#ifndef _ITF_RAY_BOSSMORAYNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayNodeComponent.h"
#endif //_ITF_RAY_BOSSMORAYNODECOMPONENT_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_


namespace ITF
{
    class Ray_SnakeAIComponent;
    class Ray_BossMorayNodeComponent;
    typedef Ray_BossMorayNodeComponent::NodeData NodeData;
    class FXControllerComponent;

    enum RenderMode
    {
        RenderMode_Invalid,
        RenderMode_Actor,
        RenderMode_Sprite,
        //RenderMode_BezierPatch,
    };


    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                                          BODY PARTS FACTORIES
    //  
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************
    class BodyPartFactory_Template : public ObjectFactory
    {
    public:
        ITF_INLINE static void create() { get()->init(); }
        ITF_INLINE static BodyPartFactory_Template* get()
        {
            static BodyPartFactory_Template s_instance;
            return &s_instance;
        }
    private :
        void init();
    };

    class BodyPartFactory : public ObjectFactory
    {
    public:
        ITF_INLINE static void create() { get()->init(); }
        ITF_INLINE static BodyPartFactory* get()
        {
            static BodyPartFactory s_instance;
            return &s_instance;
        }

    private :
        void init();
    };



    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartBase / BodyPartBase_Template
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    class BodyPartBase_Template;

    class BodyPartBase : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT( BodyPartBase, IRTTIObject ,4266510786);
    public:
        BodyPartBase()
            : m_template(NULL)
            , m_parent(NULL)
            , m_parentComponent(NULL)
            , m_index(U32_INVALID)
            , m_lengthOnTrajectory(0.f)
            , m_renderMode(RenderMode_Invalid)
        {}
        virtual ~BodyPartBase() {}

        ITF_INLINE void     setTemplate( const BodyPartBase_Template* _template ) { m_template = _template; }
        ITF_INLINE const BodyPartBase_Template* getTemplate() const { return m_template; }
        ITF_INLINE void     initialize( Actor* _parent, Ray_SnakeAIComponent* _parentComponent ) { ITF_ASSERT( _parent != NULL && _parentComponent != NULL ); m_parent = _parent; m_parentComponent = _parentComponent; onInit(); };
        ITF_INLINE Actor*   getParent() const { return m_parent; }
        ITF_INLINE Ray_SnakeAIComponent* getParentComponent() const { return m_parentComponent; }

        virtual void        onInit() {}
        virtual void        onBecomeActive() {}
        virtual void        clear() {}

        virtual bbool       validate() { return btrue; }
        virtual void        update( f32 _dt ) {}
        virtual void        onEvent( Event* _event ) {}
        virtual void        positionOnTrajectory( const Vec3d& _pos, const f32 _angle ) {}
        virtual void        onTrajectoryUpdate() {} // called after trajectory update

        // TODO: it would be nice to make this non-sprite specific
        virtual void        addPolylinePoints_sprite( PolyLine& _poly ) { ITF_ASSERT_MSG(0, "implement me"); }
        virtual void        addOtherPolylinePoints_sprite( PolyLine& _poly ) { ITF_ASSERT_MSG(0, "implement me"); }
        virtual void        draw_sprite( AtlasDynamicObject& _atlas ) { ITF_ASSERT_MSG(0, "implement me"); }


        ITF_INLINE u32      getIndex() const { return m_index; }
        ITF_INLINE void     setIndex( const u32 _value ) { m_index = _value; }
        ITF_INLINE u32      getBodyPartCount() const;

        ITF_INLINE f32      getLengthOnTrajectory() const { return m_lengthOnTrajectory; }
        ITF_INLINE void     setLengthOnTrajectory( const f32 _value ) { m_lengthOnTrajectory = _value; }

        ITF_INLINE RenderMode getRenderMode() const { return m_renderMode; }

        virtual AABB        getAABB() const = 0;



#ifdef ITF_SUPPORT_EDITOR
        virtual void onEditorMove() {}
#endif // ITF_SUPPORT_EDITOR

    protected:
        const BodyPartBase_Template* m_template;
        Actor*                  m_parent;
        Ray_SnakeAIComponent*   m_parentComponent;

    //private:
        u32 m_index;
        f32 m_lengthOnTrajectory;
        RenderMode m_renderMode;
    };

#define DECLARE_BODYPART_TEMPLATE(bodyPartBaseClass) \
public: \
    virtual class BodyPartBase* createBodyPart() const { return newAlloc( mId_System, bodyPartBaseClass() ); } \

    class BodyPartBase_Template : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BodyPartBase_Template,IRTTIObject,2413114819);
        DECLARE_SERIALIZE_VIRTUAL();
    public:

        BodyPartBase_Template()
            : m_lengthOnTrajectory(0.f)
        {}
        virtual ~BodyPartBase_Template() {}

        virtual class BodyPartBase* createBodyPart() const = 0;
        virtual void onTemplateLoaded() {}

        virtual bbool usePolyline() const { return bfalse; }


        ITF_INLINE f32 getLengthOnTrajectory() const { return m_lengthOnTrajectory; }

    private:

        f32 m_lengthOnTrajectory;   // how much space the part occupies on the trajectory (used to position the part on the trajectory)
    };



    typedef ITF_VECTOR<BodyPartBase_Template*> BodyPartBaseTemplateList;
    typedef ITF_VECTOR<BodyPartBase*> BodyPartBaseList;




    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                      Ray_SnakeAIComponent
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    class Ray_SnakeAIComponent : public ActorComponent
    {
        friend class BodyPartBase;
        DECLARE_OBJECT_CHILD_RTTI(Ray_SnakeAIComponent, ActorComponent,883253688)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        Ray_SnakeAIComponent();
        virtual ~Ray_SnakeAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return m_renderMode == RenderMode_Sprite; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType /*_hotReload*/ );
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );
        virtual void        Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
        void                debugNodes() const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
        void                onScaleChanged();
        void                forceUpdateBodyParts();
#endif // ITF_SUPPORT_EDITOR

    protected:

        ITF_INLINE const class Ray_SnakeAIComponent_Template* getTemplate() const;

        enum ActiveMode
        {
        	ActiveMode_Default,
            ActiveMode_AlwaysActiveDeactivateOnEnd,
            ENUM_FORCE_SIZE_32(ActiveMode)
        };
        
        #define SERIALIZE_ActiveMode(name, val) \
        SERIALIZE_ENUM_BEGIN(name, val); \
            SERIALIZE_ENUM_VAR(ActiveMode_Default); \
            SERIALIZE_ENUM_VAR(ActiveMode_AlwaysActiveDeactivateOnEnd); \
        SERIALIZE_ENUM_END();


        void                clear();

        void                initScale();
        void                initTrajectory( const Vec2d& _pos, const Vec2d& _dir );

        bbool               validateBodyParts( const f32 _dt );
        void                updateBodyParts( const f32 _dt );
        void                updateDeadBodyParts( const f32 _dt );
        void                positionBodyPartsOnTrajectory( const f32 _offset = 0.f );

        void                updatePolyline( const f32 _dt );
        void                updatePolyline_actor( const f32 _dt );
        void                updatePolyline_sprite( const f32 _dt );

        bbool               destroyBodyPart( class BodyPart* _bodyPart, u32 _idxInList );


        AnimLightComponent*     m_animComponent;
        FXControllerComponent*  m_fxController;

        ActiveMode          m_activeMode;
        RenderMode          m_renderMode;
        BodyPartBaseList    m_bodyParts;
        BodyPartBaseList    m_deadBodyParts;
        f32                 m_totalBodyLength;
        u32                 m_headEdgeCount;

        ProceduralPolyline  m_enclosingPolyline;

        TrajectoryProvider  m_trajectory;
        f32                 m_trajectoryLengthMultiplier;   // used to oversample the trajectory, to be able to backpedal
        bbool               m_trajectoryJustInitialized;
        Vec2d               m_prevPos;  // only used for head angle



        ActorRef        m_prevNode;
        ActorRef        m_currentNode;
        ActorRef        m_nextNode;
        ActorRef        m_nodeSelector;
        ActorRefList    m_prevNodes;

        f32             m_currentSpeed;
        f32             m_targetSpeed;
        f32             m_currentNodeDist;

        f32             m_acceleration;
        f32             m_currentSpeedMultiplier;
        f32             m_targetSpeedMultiplier;

        void            resetMovement();
        void            processActivateEvent( Event* _nodeEvent );
        void            processNodeReached( Ray_EventBossMorayNodeReached* _nodeReached );
        void            setInitialNode( Ray_BossMorayNodeComponent* _node );
        void            setCurrentNode( Ray_BossMorayNodeComponent* _prevNode, Ray_BossMorayNodeComponent* _node, bbool _isInitialNode = bfalse );
        void            startNode( Ray_BossMorayNodeComponent* _node, const NodeData& _nextNodeData );
        void            updateNodes( const f32 _dt );
        Ray_BossMorayNodeComponent* getNode( ActorRef _nodeActor );
        virtual void    onEndReached();

        // this is in-between AI and node stuff...
        NodeData        chooseNextNode( Ray_BossMorayNodeComponent* _prevNode, Ray_BossMorayNodeComponent* _currentNode );
        u32             chooseNextNode_targetPlayer( Ray_BossMorayNodeComponent* _prevNode, Ray_BossMorayNodeComponent* _currentNode, const Ray_BossMorayNodeComponent::NodeDataList& _childNodes );
        u32             chooseNextNode_usingSelector( Ray_BossMorayNodeComponent* _prevNode, Ray_BossMorayNodeComponent* _currentNode, const Ray_BossMorayNodeComponent::NodeDataList& _childNodes );

        void            blendSpeedAndMultiplier( const f32 _dt );

        bbool           useUpdateSpeedMultiplier() const;
        void            updateSpeedMultiplier( const f32 _targetSqrDist );
        bbool           m_speedMultiplierDisabled;



        void            processHitStim( HitStim * _hitStim );
        bbool           processBodyPartHitStim( HitStim * _hitStim, const u32 & _partIdx, bbool _leftSide );
        void            updateAI();
        bbool           useCloseRangeAttack() const;
        void            updateCloseRangeAttack( const f32 _targetSqrDist );

        ActorRef        m_lastStimSender;
        bbool           m_isAttacking;
        bbool           m_isDying;



        // RenderMode_Sprite
        AtlasDynamicObject m_atlas;
    };


    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                      Ray_SnakeAIComponent_Template
    //  
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    class Ray_SnakeAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SnakeAIComponent_Template, TemplateActorComponent,340373408);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SnakeAIComponent);

    public:

        enum DestructibleMode
        {
            None        = 0,
            FromTail    = 1,            // when hit received, damages are computed from the part before the tail
            PartByPart  = 2,            // damages are computed on each part
            TailOnly    = 3,            // Only the tail can be damaged
            ENUM_FORCE_SIZE_32( DestructibleMode )
        };

        Ray_SnakeAIComponent_Template()
            : m_bodyPartsZSpacing(0.002f)
            , m_drawHeadBelow(bfalse)
            , m_prevNodeCount(0)
            , m_sampleLength(0.1f)
            , m_acceleration(1.f)
            , m_targetEvaluationOffset(Vec2d::Zero)
            , m_useSpeedMultiplier(btrue)
            , m_speedMultiplierMinDistance(5.f)
            , m_speedMultiplierMaxDistance(10.f)
            , m_speedMultiplierMinValue(0.7f)
            , m_speedMultiplierMaxValue(1.3f)
            , m_speedMultiplierAcceleration(1.f)
            , m_speedMultiplierDeceleration(1.f)
            , m_attackMinDistance(4.f)
            , m_attackMaxDistance(6.f)
            , m_destructibleMode(None)
            , m_considerHeadAsFirstBodyPart(bfalse)
            , m_broadcastHitToPart(bfalse)
            , m_broadcastEventsToPart(bfalse)
            , m_usePolyline(bfalse)
        {
        }
        virtual ~Ray_SnakeAIComponent_Template();

        ITF_INLINE const StringID& getHeadPolyline() const { return m_headPolyline; }
        ITF_INLINE const BodyPartBaseTemplateList& getBodyParts() const { return m_bodyParts; }
        ITF_INLINE f32 getBodyPartsZSpacing() const { return m_bodyPartsZSpacing; }
        ITF_INLINE bbool getDrawHeadBelow() const { return m_drawHeadBelow; }
        ITF_INLINE const Path& getGameMaterial() const { return m_gameMaterial; }
        ITF_INLINE const Path& getAtlasPath() const { return m_atlasPath; }

        ITF_INLINE u32 getPrevNodeCount() const { return m_prevNodeCount; }

        ITF_INLINE f32 getSampleLength() const { return m_sampleLength; }
        ITF_INLINE f32 getAcceleration() const { return m_acceleration; }

        ITF_INLINE const Vec2d& getTargetEvaluationOffset() const { return m_targetEvaluationOffset; }

        ITF_INLINE bbool getUseSpeedMultiplier() const { return m_useSpeedMultiplier; }
        ITF_INLINE f32 getSpeedMultiplierMinDistance() const { return m_speedMultiplierMinDistance; }
        ITF_INLINE f32 getSpeedMultiplierMaxDistance() const { return m_speedMultiplierMaxDistance; }
        ITF_INLINE f32 getSpeedMultiplierMinValue() const { return m_speedMultiplierMinValue; }
        ITF_INLINE f32 getSpeedMultiplierMaxValue() const { return m_speedMultiplierMaxValue; }
        ITF_INLINE f32 getSpeedMultiplierAcceleration() const { return m_speedMultiplierAcceleration; }
        ITF_INLINE f32 getSpeedMultiplierDeceleration() const { return m_speedMultiplierDeceleration; }

        ITF_INLINE bbool getConsiderHeadAsFirstBodyPart() const { return m_considerHeadAsFirstBodyPart; }
        ITF_INLINE DestructibleMode getDestructibleMode() const { return m_destructibleMode; }
        ITF_INLINE bbool getBroadcastHitToPart() const { return m_broadcastHitToPart; }
        ITF_INLINE bbool getBroadcastEventsToPart() const { return m_broadcastEventsToPart; }

        ITF_INLINE f32 getAttackMinDistance() const { return m_attackMinDistance; }
        ITF_INLINE f32 getAttackMaxDistance() const { return m_attackMaxDistance; }
        ITF_INLINE const StringID& getDeathAnimation() const { return m_deathAnimation; }
        ITF_INLINE const StringID& getDeathBhv() const { return m_deathBhv; }
        ITF_INLINE const StringID& getAttackAnimation() const { return m_attackAnimation; }


        virtual void onTemplateLoaded();

        ITF_INLINE bbool getUsePolyline() const { return m_usePolyline; }

    private:

        StringID            m_headPolyline;
        BodyPartBaseTemplateList m_bodyParts;
        f32                 m_bodyPartsZSpacing;
        bbool               m_drawHeadBelow;
        Path                m_gameMaterial;

        // SPRITE STUFF
        Path                m_atlasPath;

        // NODE STUFF
        u32                 m_prevNodeCount;    // how many prev nodes to remember (for SpikyMoray)

        // TRAJECTORY STUFF
        f32                 m_sampleLength;
        f32                 m_acceleration; // default acceleration; can be overriden by the nodes

        // ALL THIS BELOW IS AI STUFF
        Vec2d               m_targetEvaluationOffset;   // used both to blend the speed multiplier, and for the close range attack -> both should be moved to AI

        bbool               m_useSpeedMultiplier;       // onTemplatedLoaded
        f32                 m_speedMultiplierMinDistance;
        f32                 m_speedMultiplierMaxDistance;
        f32                 m_speedMultiplierMinValue;
        f32                 m_speedMultiplierMaxValue;
        f32                 m_speedMultiplierAcceleration;
        f32                 m_speedMultiplierDeceleration;

        f32                 m_attackMinDistance;
        f32                 m_attackMaxDistance;

        bbool               m_considerHeadAsFirstBodyPart;
        DestructibleMode    m_destructibleMode;
        bbool               m_broadcastHitToPart;
        bbool               m_broadcastEventsToPart;

        StringID            m_deathAnimation;
        StringID            m_deathBhv;
        StringID            m_attackAnimation;

        // onTemplateLoaded
        bbool               m_usePolyline;
    };

    ITF_INLINE const Ray_SnakeAIComponent_Template* Ray_SnakeAIComponent::getTemplate() const
    {
        return static_cast<const Ray_SnakeAIComponent_Template*>(m_template);
    }

    ITF_INLINE u32 BodyPartBase::getBodyPartCount() const { return m_parentComponent->m_bodyParts.size(); }
};

#endif //_ITF_RAY_SNAKEAICOMPONENT_H_
