#ifndef _ITF_RAY_BEZIERTREEAICOMPONENT_H_
#define _ITF_RAY_BEZIERTREEAICOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif // _ITF_SPAWNER_H_

#ifndef _ITF_TWEENINTERPRETER_H_
#include "gameplay/Components/Common/TweenInterpreter.h"
#endif //_ITF_TWEENINTERPRETER_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_


/**********************************************
                AI STUFF
 *********************************************/

#ifndef _ITF_RAY_PLATFORMTREECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_PlatformTreeComponent.h"
#endif //_ITF_RAY_PLATFORMTREECOMPONENT_H_

#ifndef _ITF_RAY_SOFTCOLLISION_H_
#include "rayman/gameplay/AI/Utils/Ray_SoftCollision.h"
#endif //_ITF_RAY_SOFTCOLLISION_H_

namespace ITF
{
    enum PolylineMode
    {
        PolylineMode_Invalid        = -1,
        PolylineMode_None           = 0,
        PolylineMode_Left           = 1,
        PolylineMode_Right          = 2,
        PolylineMode_DoubleSided    = 3,
        ENUM_FORCE_SIZE_32(PolylineMode)
    };

    #define SERIALIZE_POLYLINEMODE(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
        SERIALIZE_ENUM_VAR(PolylineMode_None); \
        SERIALIZE_ENUM_VAR(PolylineMode_Left); \
        SERIALIZE_ENUM_VAR(PolylineMode_Right); \
        SERIALIZE_ENUM_VAR(PolylineMode_DoubleSided); \
    SERIALIZE_ENUM_END();


    class FXControllerComponent;
    class SoundComponent;

    class Ray_BezierTreeAIComponent : public GraphicComponent, IUpdateSoundInputCriteria
    {
        friend class Ray_BezierTree_Plugin;

        DECLARE_OBJECT_CHILD_RTTI(Ray_BezierTreeAIComponent, GraphicComponent,989843734)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BezierTreeAIComponent();
        virtual ~Ray_BezierTreeAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );
        virtual void        Draw();
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR


    private:
        ITF_INLINE const class Ray_BezierTreeAIComponent_Template* getTemplate() const;

        class Branch;
        class Ray_AnemoneAI;

        class Node
        {
            friend class Ray_BezierTree_Plugin;
            DECLARE_SERIALIZE()
        public:
            Node()
                : m_initialPos()
                , m_initialAngle(0.f)
                , m_initialScale(1.f)
                , m_bezierTension(1.f)
                , m_tweenOffset(0.f)
                , m_tplSpawnable(NULL)
                , m_pos(Vec3d::Zero)
                , m_angle(0.f)
                , m_tangent(Vec3d::Zero)
                , m_scale(1.f)
            {}

            void clear();
            void initTransform();
            void initTween( Ray_BezierTreeAIComponent* _owner );
            void initSpawn( Ray_BezierTreeAIComponent* _owner, const Transform3d& _transform, Scene* _scene );
            void update( const Transform3d& _transform, const f32 _dt );

            ITF_INLINE const Transform3d& getInitialTransform() const { return m_initialTransform; }
            ITF_INLINE const Vec3d& getPos() const { return m_pos; }
            ITF_INLINE const f32 getAngle() const { return m_angle; }
            ITF_INLINE f32 getScale() const { return m_scale; }
            ITF_INLINE Vec3d getTangent() const { return m_tangent; }

            ITF_INLINE f32 getInitialScale() const { return m_initialScale; }
            ITF_INLINE void setInitialScale( const f32 _value ) { m_initialScale = _value; }

        private:
            // editor
            Vec3d getInitialTangent() const { Vec3d v(m_bezierTension,0,0); Vec3d_Rotate(&v, m_initialAngle); return v; }
            void setInitialTangent( const Vec3d& _tan ) { m_initialAngle = Vec3d_getAngle(_tan); m_bezierTension = _tan.norm(); }

            // instance-serialized
            Vec3d m_initialPos;
            f32 m_initialAngle;
            f32 m_initialScale;
            f32 m_bezierTension;    // angle + tension -> tangent
            StringID m_tweenSet;
            f32 m_tweenOffset;
            StringID m_spawnableName;

            // runtime
            Transform3d m_initialTransform;
            TweenInterpreter m_tween;
            const class Spawnable* m_tplSpawnable;
            ActorRef m_spawn;

            // all these are in world space
            Vec3d m_pos;
            f32 m_angle;
            Vec3d m_tangent;
            f32 m_scale;
        };
        typedef SafeArray<Node> NodeList;

        class Branch
        {
            enum State
            {
                State_Open,
                State_Closed,
            };

            DECLARE_SERIALIZE()
        public:
            Branch()
                : m_polyline(NULL)
                , m_oneOverTotalInitialLength(0.f)
                , m_oneOverTotalLength(0.f)
                , m_currentLength(0.f)
                , m_polyEdgeCount(U32_INVALID)
                , m_useSharpTip(bfalse)
                , m_fxHandle(U32_INVALID)
                // AI STUFF
                , m_state(State_Open)
                , m_timeRatio(1.f)
                , m_time(1.f)
            {}

            bbool checkNodeCount( Ray_BezierTreeAIComponent* _owner ) const;
            ITF_INLINE bbool useSharpTip() const { return m_useSharpTip; }

            void clear();
            void init( Ray_BezierTreeAIComponent* _owner, const Transform3d& _transform );
            void update( Ray_BezierTreeAIComponent* _owner, const Transform3d& _transform, const f32 _dt, const BezierCurve::Point& _attachPoint );
            void initAABB( Ray_BezierTreeAIComponent* _owner, const f32 _extrusion, AABB& _aabb );
            void updateAABB( Ray_BezierTreeAIComponent* _owner, const f32 _extrusion, AABB& _aabb );
            void draw( Ray_BezierTreeAIComponent* _owner, Texture* _texture, const bbool _flipUV );
            void draw_end( Ray_BezierTreeAIComponent* _owner, Texture* _texture, const bbool _flipUV );
            void draw_startEnd( Ray_BezierTreeAIComponent* _owner, Texture* _texture, const bbool _flipUV );
            void updateFx( Ray_BezierTreeAIComponent* _owner );

            // instance-serialized
            NodeList m_nodes;

            // runtime
            ProceduralPolyline* m_polyline;
            BezierCurve m_initialBezierCurve;   // local
            BezierCurve m_bezierCurve;          // world
            ActorRef m_headActor;


            // AI STUFF
            void initAI( const Ray_AnemoneAI* _ai );
            void forceOpen() { open(); m_time = 1.f; }
            void forceClose() { close(); m_time = 0.f; }
            void open() { m_state = State_Open; }
            void close() { m_state = State_Closed; }
            ITF_INLINE f32 getT() const { return m_time; }

        private:
            void buildCurve( Ray_BezierTreeAIComponent* _owner, BezierCurve& _bezierCurve, const BezierCurve::Point& _firstPoint );
            f32 getScale( const f32 _dist ) const;
            f32 getScaleFromInitialDist( const f32 _dist ) const;

            void initSpawn( Ray_BezierTreeAIComponent* _owner, const Transform3d& _transform, Scene* _scene );
            void updateHead( Ray_BezierTreeAIComponent* _owner, Actor* _headActor );

            u32 getPolylinePointCount( Ray_BezierTreeAIComponent* _owner ) const;
            void updatePolyline( Ray_BezierTreeAIComponent* _owner, const f32 _dt );
            void updatePolyline_left( Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength );
            void updatePolyline_right( Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength );
            void updatePolyline_doubleSided( Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength );
            void updatePolyline_leftExtruded( Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength );
            void updatePolyline_rightExtruded( Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength );
            void updatePolyline_doubleSidedExtruded( Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength );
            void updatePolyline_doubleSidedExtruded_sharpTip( Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength );
            Vec2d getPosExtrapolated( u32& _edgeIndex, u32& _sampleIndex, const f32 _dist ) const;
            void getPosAndNormalExtruded( Ray_BezierTreeAIComponent* _owner, u32& _edgeIndex, u32& _sampleIndex, const f32 _dist, Vec2d& _posOut, Vec2d& _normalOut ) const;

            void renderer_draw( const BezierCurveRenderer& _renderer,
                                const f32 _startDist, const f32 _endDist,
                                const Vec2d& _startUV, const Vec2d& _endUV,
                                Texture* _texture, BezierPatchList& _patches,
                                const BezierCurve& _curve, const bbool _flipUV = bfalse );

            f32 m_oneOverTotalInitialLength;
            f32 m_oneOverTotalLength;
            f32 m_currentLength;

            BezierPatchList m_patches;
            BezierPatchList m_startPatches;
            BezierPatchList m_endPatches;
            SafeArray<f32> m_scales;

            u32 m_polyEdgeCount;    // set at init: must NOT change!
            bbool m_useSharpTip;    // set at init: must NOT change!

            u32 m_fxHandle;


            // AI STUFF
            void updateAI( Ray_BezierTreeAIComponent* _owner, const f32 _dt, Actor* _headActor );
            State m_state;
            f32 m_timeRatio;    // init-time - 1 / duration
            f32 m_time;         // [0, 1] - lerped
        };
        typedef ITF_VECTOR<Branch> BranchList;


        void clear();
        void init();
        void postChangeProperties();
        ITF_INLINE Transform3d getTransform() const { return getWorldInitTransform(m_actor); }
        void initAABB( const Transform3d& _transform );
        void processHit( class PunchStim* _stim );

        // instance-serialized
        BranchList m_branches;
        PolylineMode m_polylineMode;

        bbool m_polylineDisabled;
        SpawneeGenerator m_headSpawner;
        ITF_VECTOR<SpawneeGenerator> m_spawners;

        ITF_INLINE bbool useStartSprite() const { return m_spriteMode == SpriteMode_StartMainEnd; }
        ITF_INLINE bbool useEndSprite() const { return m_spriteMode != SpriteMode_Main; }
        enum SpriteMode
        {
            SpriteMode_Main,
            SpriteMode_MainEnd,
            SpriteMode_StartMainEnd,
            ENUM_FORCE_SIZE_32(SpriteMode)
        };
        BezierCurveRenderer m_bezierRenderer;
        SpriteMode m_spriteMode;
        UVdata m_mainSprite;
        UVdata m_startSprite;
        UVdata m_endSprite;
        f32 m_uvOffset;
        bbool m_flipTexture;    // instance-serialized


        void getAttachPointLocal( const Transform3d& _transform, BezierCurve::Point& _point ) const;
        void getAttachPointWorld( const Transform3d& _transform, BezierCurve::Point& _point ) const;
        AnimLightComponent* m_animComponent;
        u32 m_attachBoneIndex;


        void updateFx( const f32 _dt, const f32 _prevBranchTime );
        virtual void updateSoundInput();
        FXControllerComponent* m_fxController;
        SoundComponent* m_soundComponent;
        u32 m_speedInputIndex;
        f32 m_speedInputValue;



        /*************************************************
                    AI STUFF : OPEN/CLOSE etc
         ************************************************/

        class Ray_AnemoneAI
        {
            friend class Ray_BezierTreeAIComponent;
            //DECLARE_SERIALIZE()
            //DECLARE_VALIDATE_COMPONENT()

        public:
            Ray_AnemoneAI();
            ~Ray_AnemoneAI();

            void        onActorLoaded( Ray_BezierTreeAIComponent* _owner );
            void        initAI();
            void        onEvent( Event* _event );
            void        update( const f32 _dt );
#ifdef ITF_SUPPORT_EDITOR
            virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
                    void        editor_setActivated( bbool _value );
#endif // ITF_SUPPORT_EDITOR

            ITF_INLINE f32 getBranchSpeed() const { return m_branchSpeed; }
            ITF_INLINE f32 getBranchSpeedRandom() const { return m_template->getChildMoveSpeedRandom(); }

            ITF_INLINE bbool isOpen() const { return m_state == State_Open; }

        private:
            ITF_INLINE const class Ray_PlatformTreeComponent_Template* getTemplate() const { return m_template; }

            Actor* m_actor;
            Ray_BezierTreeAIComponent* m_owner;
            const Ray_PlatformTreeComponent_Template* m_template;

            class AnimLightComponent* m_animComponent;
            class AnimatedComponent* m_animatedComponent;
            class PlayerDetectorComponent* m_playerDetector;


            void        tryOpen();
            void        tryClose();
            void        applyStateForced( const bbool _open );

            bbool       m_startActivated;
            f32         m_branchSpeed;


            enum State
            {
                State_Invalid,
                State_Open,
                State_Open_Wiggling,
                State_Closed,
                State_Closed_Wiggling,
                State_Opening_In,
                State_Opening_Loop1,    // wait for branches to open
                State_Opening_Loop2,    // extra timer
                State_Opening_Out,
                State_Closing_In,
                State_Closing_Loop1,    // wait for branches to close
                State_Closing_Loop2,    // extra timer
                State_Closing_Out,
                ENUM_FORCE_SIZE_32(State)
            };

            void        forceChangeState( const State _newState ) { m_state = State_Invalid; changeState(_newState); }
            void        changeState( const State _newState );
            void        updateState( const f32 _dt );

            bbool       allChildrenOpen() const;
            bbool       allChildrenClosed() const;
            bbool       playAnim( const StringID _anim );
            void        setCollisionDisabled( const bbool _disable );

            State       m_state;
            f32         m_timer;

            void        detectWiggle();
            u32         m_playersInsideCount;

            Ray_SoftCollision m_softCollision;
        };

        Ray_AnemoneAI m_ai;
    };


    //---------------------------------------------------------------------------------------------------

    class Spawnable
    {
        DECLARE_SERIALIZE();

    public:
        Spawnable()
            : m_useNodeScale(btrue)
            , m_posOffset(0, 0, 0)
        {
        }

        StringID m_name;
        Path m_actorPath;
        bbool m_useNodeScale;
        Vec3d m_posOffset;
        Angle m_angleOffset;
    };
    typedef ITF_VECTOR<Spawnable> SpawnableList;

    class Ray_BezierTreeAIComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BezierTreeAIComponent_Template, GraphicComponent_Template,1286437410);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BezierTreeAIComponent);

    public:

        Ray_BezierTreeAIComponent_Template()
            : m_lockFirstNode(btrue)
            , m_lockLastNodeScale(btrue)
            , m_polylineMode(PolylineMode_DoubleSided)
            , m_polylineDisableOnTransition(bfalse)
            , m_polylineBeginLength(0.f)
            , m_polylineEndLength(0.f)
            , m_polylineBeginWidth(0.f)
            , m_polylineMidWidth(0.f)
            , m_polylineEndWidth(0.f)
            , m_polylineStartOffset(0.f)
            , m_polylineEndOffset(0.f)
            , m_polylineTessellationLength(1.f)
            , m_polylineTessellationRatio(1.f)
            , m_mainSpriteIndex(0)
            , m_startSpriteIndex(1)
            , m_startSpriteLength(1.f)
            , m_endSpriteIndex(2)
            , m_endSpriteLength(1.f)
            , m_uvStretch(btrue)
            , m_uvAttachToHead(btrue)
            , m_uvScrollSpeed(0.f)
            , m_headAttachOffset(0.f)
        {
            m_ai.getSoftCollision().setRadius(0.f);    // off by default
        }
        virtual ~Ray_BezierTreeAIComponent_Template() {}

        ITF_INLINE bbool getLockFirstNode() const { return m_lockFirstNode; }
        ITF_INLINE bbool getLockLastNodeScale() const { return m_lockLastNodeScale; }
        ITF_INLINE const StringID& getAttachBone() const { return m_attachBone; }
        ITF_INLINE PolylineMode getPolylineMode() const { return m_polylineMode; }
        ITF_INLINE const Path& getGameMaterial() const { return m_gameMaterial; }
        ITF_INLINE f32 getPolylineBeginWidth() const { return m_polylineBeginWidth; }
        ITF_INLINE f32 getPolylineMidWidth() const { return m_polylineMidWidth; }
        ITF_INLINE f32 getPolylineEndWidth() const { return m_polylineEndWidth; }
        ITF_INLINE f32 getPolylineStartOffset() const { return m_polylineStartOffset; }
        ITF_INLINE f32 getPolylineEndOffset() const { return m_polylineEndOffset; }
        ITF_INLINE f32 getPolylineTessellationLength() const { return m_polylineTessellationLength; }
        ITF_INLINE f32 getPolylineTessellationRatio() const { return m_polylineTessellationRatio; }
        ITF_INLINE const BezierCurveRenderer_Template& getBezierRenderer() const { return m_bezierRenderer; }
        ITF_INLINE u32 getMainSpriteIndex() const { return m_mainSpriteIndex; }
        ITF_INLINE u32 getStartSpriteIndex() const { return m_startSpriteIndex; }
        ITF_INLINE f32 getStartSpriteLength() const { return m_startSpriteLength; }
        ITF_INLINE u32 getEndSpriteIndex() const { return m_endSpriteIndex; }
        ITF_INLINE f32 getEndSpriteLength() const { return m_endSpriteLength; }
        ITF_INLINE bbool getUvStretch() const { return m_uvStretch; }
        ITF_INLINE bbool getUvAttachToHead() const { return m_uvAttachToHead; }
        ITF_INLINE f32 getUvScrollSpeed() const { return m_uvScrollSpeed; }
        ITF_INLINE const TweenInterpreter_Template& getTweenInterpreter() const { return m_tweenInterpreter; }
        ITF_INLINE const Path& getHeadActor() const { return m_headActor; }
        ITF_INLINE f32 getHeadAttachOffset() const { return m_headAttachOffset; }
        ITF_INLINE const SpawnableList& getSpawnables() const { return m_spawnables; }
        ITF_INLINE const StringID& getFxName() const { return m_fxName; }
        ITF_INLINE const Ray_PlatformTreeComponent_Template& getAI() const { return m_ai; }
        ITF_INLINE bbool getPolylineDisableOnTransition() const { return m_polylineDisableOnTransition; }

        virtual void onTemplateLoaded();

        f32 interpolatePolylineWidth( const f32 _dist, const f32 _totalLength ) const;

    private:

        bbool m_lockFirstNode;      // if true, first node == actor pos
        bbool m_lockLastNodeScale;  // if true, last node can't be scaled
        StringID m_attachBone;

        // polyline
        PolylineMode m_polylineMode;
        Path m_gameMaterial;
        f32 m_polylineBeginLength;
        f32 m_polylineEndLength;
        f32 m_polylineBeginWidth;
        f32 m_polylineMidWidth;
        f32 m_polylineEndWidth;
        f32 m_polylineStartOffset;
        f32 m_polylineEndOffset;
        f32 m_polylineTessellationLength;
        f32 m_polylineTessellationRatio;    // onTemplateLoaded

        // renderer
        BezierCurveRenderer_Template m_bezierRenderer;
        u32 m_mainSpriteIndex;
        u32 m_startSpriteIndex;
        f32 m_startSpriteLength;
        u32 m_endSpriteIndex;
        f32 m_endSpriteLength;
        bbool m_uvStretch;
        bbool m_uvAttachToHead;
        f32 m_uvScrollSpeed;

        // tween
        TweenInterpreter_Template m_tweenInterpreter;

        // spawner
        Path m_headActor;
        f32 m_headAttachOffset;
        SpawnableList m_spawnables;

        // FX
        StringID m_fxName;

        // AI
        Ray_PlatformTreeComponent_Template m_ai;
        bbool m_polylineDisableOnTransition;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_BezierTreeAIComponent_Template* Ray_BezierTreeAIComponent::getTemplate() const
    {
        return static_cast<const Ray_BezierTreeAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BEZIERTREEAICOMPONENT_H_