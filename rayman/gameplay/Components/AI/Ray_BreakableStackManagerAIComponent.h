#ifndef _ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_
#define _ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

#ifndef _ITF_ANIMATIONATLAS_H_
#include "gameplay/AI/Utils/AnimationAtlas.h"
#endif //_ITF_ANIMATIONATLAS_H_

// Polylines
#define POLY_LEFT           0 
#define POLY_TOP            1
#define POLY_RIGHT          2
#define POLY_BOTTOM         3

// Misc
#define BLOCK_HIT_POINTS    2
#define OFFSET_COL_PHANTOM  0.0f

// State
#define BLOCK_STATE_STATIC                  0
#define BLOCK_STATE_SHAKING_BEFORE_FALL     1
#define BLOCK_STATE_FALLING                 2
#define BLOCK_STATE_EXPULSE                 3
#define BLOCK_STATE_DISABLED                4

// Param
#define EXPULSE_MIN_RANDOM_Z    4.0f
#define EXPULSE_MAX_RANDOM_Z    6.0f
#define EXPULSE_RANDOM_ANGLE    MTH_PIBY6
#define EXPULSE_RANDOM_POWER    9.0f
#define TIME_SHAKE_ON_HIT       0.2f

// Explosive
#define TIME_FOR_CHAINE_EXPLODE        0.5f

// HighLight Explode
#define HL_DURATION_ON      0.1f
#define HL_DURATION_OFF     0.4f
#define HL_DURATION_COEF    0.25f
#define HL_STATE_OFF        0
#define HL_STATE_ON         1

// Particles
#define PAR_COUNT_MAX_BY_BOX       4
#define PAR_PERCENT_START_ALPHA    0.75f
#define PAR_GRAVITY                -15.0f
#define PAR_ROTATION               MTH_PI * 0.015f
#define PAR_LIFE_TIME              2.0f
#define PAR_EXPULSE_POWER_MIN      0.0f
#define PAR_EXPULSE_POWER_MAX      10.0f
#define PAR_EXPULSE_Y_OFFSET       MTH_PIBY3
#define PAR_EXPULSE_ANGLE          MTH_PIBY6
#define PAR_SCALE_LITTLE_FRAG      0.3f


//#define _ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_ 1
//#define _ITF_BSE_DEBUG_ 


namespace ITF
{
    //*****************************************************************************
    // InfoElement (struct) // For creation only
    //*****************************************************************************

    struct InfoHeadElement
    {
        InfoHeadElement()
        : m_width(2)
        , m_height(2)
        , m_hitPoint(2)
        , m_blockStatic(bfalse)
        , m_explosive(bfalse)
        , m_radiusExplosive(1.0f)
        , m_timeBeforeExplode(0.0f)
        {
            // none
        }

        ~InfoHeadElement() {}

        u32  m_width;
        u32  m_height;
        u32  m_hitPoint;
        Path m_gmat;
        bbool m_blockStatic;

        bbool m_explosive;
        f32   m_radiusExplosive;
        f32   m_timeBeforeExplode;

    };

    struct FragmentsList
    {
        DECLARE_SERIALIZE();

        FragmentsList() {}
        ~FragmentsList() {}

        ITF_VECTOR<u32> m_fragments;
    };


    struct InfoElement
    {
        DECLARE_SERIALIZE();

        InfoElement()
        : m_isOccupied(btrue)
        , m_reward(0)
        {
            // none
        }

        ~InfoElement() {}

        bbool m_isOccupied;
        AnimationAtlas m_anim;
        AnimationAtlas m_animBreak;
        FragmentsList  m_fragments;
        u32 m_reward;
    };

    struct InfoElementList
    {
        DECLARE_SERIALIZE();

        ITF_VECTOR<InfoElement> m_elements;
    };

    typedef ITF_VECTOR<InfoElementList> InfoElementGrid;


    struct FxData
    {
        DECLARE_SERIALIZE();

        StringID m_fxFall;
        StringID m_fxHit;
        StringID m_fxSteam;

        StringID m_soundFall;
        StringID m_soundHit;
        StringID m_soundDestroy;

    };


    //*****************************************************************************


    class FXControllerComponent;
    class LinkComponent;
    class PhysPhantom;

    class Ray_BreakableStackManagerAIComponent : public Ray_AIComponent
    {
    public:

        //*****************************************************************************
        // Generator Particles
        //*****************************************************************************


        class ParticlesGenerator
        {
        public:

            struct Particle
            {
                Particle()
                : m_angle(0.0f)
                , m_scale(1.0f)
                , m_time(0.0f)
                , m_timeMax(0.0f)
                , m_alpha(1.0f)
                , m_atlasIndex(0)
                , m_explodeToRight(bfalse)
                , m_explodeToLeft(bfalse)
                {
                    // none
                }

                virtual ~Particle() {}

                Vec2d m_dir;

                Vec2d m_startPos;
                Vec2d m_curPos;

                f32   m_alpha;
                f32   m_angle;
                f32   m_scale;
                f32   m_time;
                f32   m_timeMax;

                u32  m_atlasIndex;
                bbool m_explodeToRight;
                bbool m_explodeToLeft;


            };

            ParticlesGenerator()
            {
                // none
            }

            virtual ~ParticlesGenerator()
            {
                // none
            }

            void update(f32 _dt );
            void addParticles( u32 _indexAtlas, Vec2d _dir, Vec2d _pos, f32 _scale = 1.0f);

            ITF_VECTOR<Particle> m_particles;

        };


        //*****************************************************************************
        // PolylineBox
        //*****************************************************************************

        class PolylineBox
        {
        public:

            PolylineBox();
            virtual ~PolylineBox();

            ProceduralPolyline  m_procPoly;
            SafeArray<Vec2d>    m_points;
        };

        //*****************************************************************************
        // Box
        //*****************************************************************************

        struct NeighborBox;

        class Box
        {
        public:
            Box();
            virtual ~Box();

            void updateOnOffPolylineBox(NeighborBox& _neighborBox, f32 _depth);
            void updateConnectionBox(NeighborBox& _neighborBox);
            void desactiveBox();
            void activeBox(f32 _depth);
#ifdef _ITF_BSE_DEBUG_
            void debugDisplayConnection( PolylineBox& _prev, PolylineBox& _next ); // Debug
#endif // _ITF_BSE_DEBUG_

            // Atlas / Anim
            AnimationAtlas       m_animation; 
            AnimationAtlas       m_animationBreak;            
            mutable AnimationAtlasPlayer m_animPlayer;            
            FragmentsList        m_fragments;

            Vec2d           m_drawRelativePos;
            bbool           m_isOccupied; 
            bbool           m_canFall;
            u32             m_handleFX;

            // Expulse
            f32             m_targetExpulseZ;
            f32             m_currentExpulseZ;
            f32             m_angle;
            Vec2d           m_impulse;
            Vec2d           m_startAbsoluteExpulsePos;

            u32             m_reward;

            PhysPhantom*    m_phantom;
            bbool           m_physWorldRegistred;

            ITF_VECTOR<PolylineBox> m_procPolyList;


            ParticlesGenerator  m_generator;

        };

        //*****************************************************************************
        // Neighbor (struct)
        //*****************************************************************************

        struct NeighborBox
        {
            NeighborBox()
                : boxA(NULL)
                , boxB(NULL)
                , boxC(NULL)
                , boxD(NULL)
                , boxE(NULL)
                , boxF(NULL)
                , boxG(NULL)
                , boxH(NULL)
            {
                // none
            }

            ~NeighborBox() {}

            Box* boxA;
            Box* boxB;
            Box* boxC;
            Box* boxD;
            Box* boxE;
            Box* boxF;
            Box* boxG;
            Box* boxH;
        };


        //*****************************************************************************
        // Block
        //*****************************************************************************

        class Block
        {
        public:

            Block();
            virtual ~Block();

            void            Init(Actor* _owner, IEventListener* _eventListener, u32 _posRow, u32 _posCol, f32 _edgeSize, const InfoHeadElement& _info, const FxData& _fxData, PhysShapePolygon& _physShape, Color _color = Color::green());
            const Vec2d     getRelativeCenterPos() const;

			ITF_INLINE bbool isDesactivate() const
			{
				return (m_state == BLOCK_STATE_DISABLED || m_state == BLOCK_STATE_EXPULSE);
			}
            void            getLocalBoxNeighbor(u32 _globalIndexRow, u32 _globalIndexCol, NeighborBox& _neighborBox);
            bbool           checkEmptyRow();
            void            resetConnection();
            void            invalidFall();
            void            spawnReward(Actor* _sender, ObjectRef _receiver, Vec3d _offset, u32 _type);

            Ray_BreakableStackManagerAIComponent::Box* getGlobalBox(u32 _rowGlobal, u32 _colGlobal);

            u32   m_width;
            u32   m_height;
            u32   m_posRow;
            u32   m_posCol;
            Vec2d m_originRelativePos;
            u32   m_state;
            f32   m_timeShake;
            f32   m_timeExpulse;

            f32   m_countDownHit;
            i32   m_hitPoints;
            u32   m_handleFX;
            u32   m_heightOffset;
            f32   m_alpha;
            f32   m_edgeSize;

            bbool m_updated;
            bbool m_canFall;
            bbool m_explodeToRight;
            bbool m_shaking;
            bbool m_blockStatic;

            // Explode (special blocks)
            bbool m_explosive;
            f32   m_radiusExplosive;
            f32   m_timeBeforeExplode;

            bbool m_mustExplode;
            f32   m_timeExplode;
            Vec2d m_dirExplode;
            u32   m_levelExplode;

            f32   m_timeBlink;
            u32   m_stateBlink;
            u32   m_countBlink;

            ObjectRef m_receiverReward;

            ActorRef m_spawnerRef;

            FxData m_fxData;

            ITF_VECTOR<ITF_VECTOR<Box> > m_listBox;

            //Debug
            Color m_colorDebug;
        };

    

        //*****************************************************************************

        //typedef u32 IdBlock;
        //typedef ITF_VECTOR<ITF_VECTOR<IdBlock> > BreakableStackGrid;
        
        DECLARE_OBJECT_CHILD_RTTI(Ray_BreakableStackManagerAIComponent, Ray_AIComponent,1979560305)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BreakableStackManagerAIComponent();
        virtual ~Ray_BreakableStackManagerAIComponent();

        static const Actor* getManager(Vec2d _pos);

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onDepthChanged( f32 _oldZ, f32 _newZ );


        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event );

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR
        virtual void        Draw();

        // Services
                AABB        getMinMaxBox();
                void        createBlocks(Actor* _spawner, u32 _checkPointRow, u32 _checkPointCol);
                void        createCreditsBlocks(InfoHeadElement& _info, FxData& _fxData, InfoElementGrid& _gridElement, u32 _row, u32 _col);
                Vec2d       getSnapPos(Vec2d _spawnerPos) const;
                u32         getWidth() {return m_width;}
                u32         getHeight() {return m_height;}
				void		setCreditsMap(bbool _value) { m_isCreditsMap = _value;}

    private:

                void        postLoadProperties();

                bbool       canReceiveHit( HitStim* _hit );
                void        receiveHit( const PhysContactsContainer& _contacts, Vec2d _direction,  u32 _level, ObjectRef _sender, ObjectRef _receiverReward);
                void        gridInit();
                void        setGridIdBlock(u32 _indexBlock, u32 _row, u32 _col);
                void        setNextGridIdBlock(u32 _indexBlock, u32 _row, u32 _col);

                void        updateConnection();
                void        updateGravity();
                void        updateGravityTree();

                void        updateBlock(f32 _dt);
                void        updateParticles( f32 _dt );
                void        updatePosPolyline();
                void        updatePolyline(f32 _dt);
                void        updatePosBallistics(Block* _block, f32 _dt);
                void        updateOffset(f32 _dt);

				ITF_INLINE void switchGrid()
				{
					m_currentGrid = m_nextGrid;
				}

                void        drawAtlas();
                void        drawParticles(Box* _box);

                bbool       boxCanFall(u32 _row, u32 _col);

                void        startFallBlock(Block& _block, bbool _immediateFall = bfalse);
                void        requestFallBlock(Block& _block);
                void        clearConnection();
                bbool       addBlockInGrid(Block* _block);
                void        removeAllDisabledBlock();

                bbool       gridZoneIsFree(Block* _block);
                u32         getIndexBlock(Block& _block);
                Box*        getBox( u32 _row, u32 _col );
                Block*      getBlock( u32 _row, u32 _col );
                void        getGlobalBoxNeighbor(u32 _globalIndexRow, u32 _globalIndexCol, NeighborBox& _neighborBox);
                void        hitBlock(Block& _block, Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender, ObjectRef _receiverReward);
                bbool       searchDestroyBlock(ObjectRef _phantom ,Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender, ObjectRef _receiverReward);
                void        destroyBlock(Block& _block, Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender, ObjectRef _receiverReward);
                void        sendEventToPolyline(Event* _event, ObjectRef _polylineRef);
                void        receiveQueryBreakable( class Ray_EventBreakableQuery* _query );
                void        spawnBlockActor(Block& _block, u32 _type);
                void        growAABB();

				ITF_INLINE Vec2d getPosManager() const
				{
					return m_actor->getBoundWorldInitialPos().truncateTo2D() + m_offset;
				}

                void        playSound(const StringID& _sound, Vec2d _pos);
                u32         playFx(StringID _name, Vec3d _pos);
                void        setFxPos(u32 _handle, Vec3d _pos);
                void        setFxAngle(u32 _handle, f32 _angle);
                void        stopFX(u32 _handle);

                Vec2d       getShake(f32 _intensX, f32 _freqX, f32 _intensY, f32 _freqY, bbool _dephase = bfalse);

                void        activateAllBox(bbool _active);

                void        checkContacts(Block* _block);
                bbool       getCanBlockHits( const PhysContactsContainer& _contacts );
                Block*      searchBlockbyPhysCollidable(ObjectRef _collidable);

    public:

        static SafeArray<Ray_BreakableStackManagerAIComponent*> m_managerList;


    private:
        ITF_INLINE const class Ray_BreakableStackManagerAIComponent_Template * getTemplate() const;
        
        FXControllerComponent* m_fxControllerComponent;
        LinkComponent* m_linkComponent;
       
        //BreakableStackGrid  m_currentGrid;
        //BreakableStackGrid  m_nextGrid;

		SafeArray<u32>		m_currentGrid;
		SafeArray<u32>		m_nextGrid;


        u32                 m_width;
        u32                 m_height;
        f32                 m_gravityFall;

        ITF_VECTOR<Block*>  m_listBlock;
        SafeArray<u32>      m_listIndexFree;

        AtlasDynamicObject  m_atlasParticles;
        AtlasDynamicObject  m_atlasObject;
        f32                 m_atlasOffset;
        f32                 m_timer;
		u32					m_countAtlasUv;

        bbool               m_iceMode;
        f32                 m_speedIceMode;

        bbool               m_syncAnim;

        Vec2d               m_offset;

        f32                 m_edgeSize;

        u32                 m_curGridAnimTrame;

        PhysShapePolygon    m_physShapeBox;

		SafeArray<Vec2d> m_listUV;

		// Special credits
		bbool				m_isCreditsMap;
		u32					m_countLumCredits;
    };

    //-------------------------------------------------------------------------------------
    class Ray_BreakableStackManagerAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BreakableStackManagerAIComponent_Template,Ray_AIComponent_Template,2521435442);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BreakableStackManagerAIComponent);

    public:


        Ray_BreakableStackManagerAIComponent_Template();
        ~Ray_BreakableStackManagerAIComponent_Template();


        ITF_INLINE f32 getTimeShakeBeforeFall() const   { return m_timeShakeBeforeFall; }
        ITF_INLINE f32 getCountDownHit() const          { return m_countDownHit; }
        ITF_INLINE f32 getGravityBallistics() const     { return m_gravityBallistics; }
        ITF_INLINE f32 getTimeExpulse() const           { return m_timeExpulse; }
        ITF_INLINE Path getAtlasPath() const            { return m_atlasPath; }
        ITF_INLINE Path getAtlasParticlesPath() const   { return m_atlasParticlesPath; }

        ITF_INLINE f32 getEdgeSize() const              { return m_edgeSize; }
        ITF_INLINE ITF::Faction getFaction() const      { return m_faction; }

    private:

        // Misc
        f32 m_timeShakeBeforeFall;
        f32 m_countDownHit;

        // Ballistics
        f32 m_gravityBallistics;
        f32 m_timeExpulse;

        // Atlas
        Path m_atlasPath;
        Path m_atlasParticlesPath;
        // Edge
        f32 m_edgeSize;

        // Faction
        Faction m_faction;

    };

    ITF_INLINE const Ray_BreakableStackManagerAIComponent_Template * Ray_BreakableStackManagerAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BreakableStackManagerAIComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_