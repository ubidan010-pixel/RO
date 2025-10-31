#ifndef _ITF_FRIEZE_DRAW_H_
#define _ITF_FRIEZE_DRAW_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_FLUIDSHAPER_H_
#include "engine/display/Frieze/FluidShaper.h"
#endif //_ITF_FLUIDSHAPER_H_


namespace ITF
{
    class EdgeProcessData
    {
    public:
        i32         m_id;
        i32         m_indexStart;
        i32         m_indexEnd;

        Vec2d       m_posStart;
        Vec2d       m_posEnd;
        Vec2d       m_posOffset;
        i32         m_count;

        Vec2d       m_normal;

        EdgeProcessData() : m_id(-1), m_indexStart(0), m_indexEnd(0)
        {}
    };

#define EDGEFLUID_MAIN_DATA      0x1
#define EDGEFLUID_VISUAL         0x2
#define EDGEFLUID_COLLISION      0x4

    struct EdgeData
    {
        f32         m_level;
        f32         m_pos;

        i32         m_way; // 0 -> normal, -1 perpendicular back, 1 perpendicular front
        bbool       m_cutUV;

        f32         m_heightScale;
        f32         m_heightPos; // 1-> base, less move on edge
        f32         m_posZ;

        EdgeData()
        : m_level(1.f)
        , m_pos(0.f)
        , m_way(0)
        , m_cutUV(bfalse)
        , m_heightScale(1.f)
        , m_heightPos(1.f)
        {
        }
    };

    typedef SafeArray<Vec2d, NB_FLUID_LEVELS> FluidHeightMap;
    typedef SafeArray<u8,NB_FLUID_LEVELS> FluidColorMap;
    typedef SafeArray<u8,NB_FLUID_LEVELS> FluidGameMaterialMap;

    class EdgeFluid
    {
    public:
        FixedArray<Vec2d, 4>                m_pos;
        FixedArray<Vec2d, 4>                m_uv;
        FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> m_colors;
        static const u32                    maxCollisionProcessCount=2;
        
        class Texture*                      m_texture;
        u32                                 m_blendMode;
        Vec2d                                m_uvAnimTrans;

        u32                                 m_meshLevel;
        f32                                 m_depth;
        FluidHeightMap                      m_heightLevelSet[MAX_FLUID_LAYER_COUNT];
        FluidColorMap                       m_fluidColorMap;
        FluidGameMaterialMap                m_fluidGameMaterialMap;
        u32                                 m_lastIndex;

        f32                                 m_norm;
        f32                                 m_height;
        f32                                 m_angle;
        Vec3d                               m_delta;
        Vec2d                               m_deltaUV;

        EdgeProcessData                     m_collisionProcesses[maxCollisionProcessCount]; //collisions, for each side (not necessarily used)
        EdgeProcessData                     m_visualProcess;
        AABB                                m_aabb;

        EdgeFluid() 
            : m_texture(NULL)
            , m_meshLevel(8)
            , m_lastIndex(NB_FLUID_LEVELS-1)
            , m_blendMode(2)
            , m_uvAnimTrans(Vec2d::Zero)
            , m_delta(Vec3d::Zero)
            , m_deltaUV(Vec2d::Zero)
        {
            m_pos.resize(4);
            m_uv.resize(4);
            m_colors.resize(GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT);
            m_aabb.invalidate();
        }

        void        Draw(EdgeData *_data, const Vec2d& _pivot, ITF_VECTOR<FluidFriseLayer> &_layerInfos);
        void        updateData(class FriseConfig *config, u32 _mask);
        void        fillPolyline(PolyLine * polyline, class AABBBuilder *aabb, u32 _mask, u32 _subIndex);
        Vec3d       getShift(f32 percent) const { return m_delta*percent; } 
    };

    struct ActorInfo
    {
        ActorInfo() { m_prevPos = Vec2d::Zero; m_prevInfluence = 0.f; m_mass=0.f; m_radius = 1.f; m_waterMultiplier = 1.f;m_queryPosition = bfalse; }
        ObjectRef   m_ref;
        Vec2d       m_prevPos;
        f32         m_prevInfluence;
        f32         m_mass;
        f32         m_radius;
        f32         m_waterMultiplier;
        bbool       m_queryPosition;
    };

    struct EdgeFluidLevel
    {
        EdgeFluid       *m_edgeFluid;
        EdgeData        m_data;
        Color           m_color;
        Vec2d            m_pivot;
        ITF_VECTOR<FluidFriseLayer> m_layerInfos;

        f32 getDepth() const
        {
            if (m_data.m_way == 0)
                return m_edgeFluid->m_depth + m_edgeFluid->getShift(m_data.m_pos).m_z;
            else
                return m_edgeFluid->m_depth + m_data.m_pos;
        }
        f32 getTrueDepth() const
        {
            if (m_data.m_way == 0)
                return m_edgeFluid->m_depth + m_edgeFluid->getShift(m_data.m_pos).m_z;
            else
                return m_edgeFluid->m_depth +  m_data.m_posZ + m_data.m_pos*m_data.m_heightScale*m_edgeFluid->m_height;
        }
    };

    class DataFluid
    {
    public:
        DataFluid() : m_isCushion(bfalse), m_weightMultiplier(1.f) {}
        ~DataFluid();

        void                        setFrieze(class Frise * _frieze);
        class Frise *               getFrieze();

        void                        deleteFluidShapers();

        void                        addActor(ObjectRef _actorRef, const Vec2d& _speed, f32 _radius, f32 _waterMultiplier, f32 _weight, bbool _queryPosition );
        void                        removeActor(ObjectRef _actorRef, const Vec2d& _speed);
        
        void                        addStringWaveGenerator(ObjectRef _actorRef);
        void                        removeStringWaveGenerator(ObjectRef _actorRef);
        const SafeArray<ObjectRef>& getStringWaveGenerators() const {return m_stringWaveGenerators;}

        void                        sendPerturbation(const Vec2d& _pos, f32 _alteration, f32 _radius, bbool radiusRandom, f32 _mass);

        void                        computeEdgeFluidLevels(u32 _levelsFront, u32 _levelBack, bbool _cutUV);
        void                        addEdgeFluidLevelPerpendicular(i32 _way, f32 _heightScale, f32 _heightPos, f32 _posZ, f32 _z); // _cutUV = true
        void                        computeAABB(class AABBBuilder *aabb);

        ITF_VECTOR<EdgeFluid>&      getEdgeFluid()       { return m_edgeFluidList; }
        ITF_VECTOR<EdgeFluidLevel>& getEdgeFluidLevels() { return m_edgeFluidListLevels; }

        void                        ProcessEdgeFluidList_InFluid(f32 _elapsed);
        void                        ProcessEdgeFluidList_InString(f32 _elapsed);
        void                        checkActorInfluence();
        void                        handlePropagation(Frise *frieze, FriseConfig *config);
        void                        handleAbsorptionAtEdgeStartEnd_String(FriseConfig *config);
        void                        checkElevationMapInitialization(FriseConfig *config);
        void                        processVirtualPerturbers(Frise *frieze, FriseConfig *_config);
        void                        updateCollisionPolyLine_InFluid(Frise *frieze, FriseConfig *config);
        void                        updateCollisionPolyLine_InString(Frise *frieze, FriseConfig *config);
        void                        updateVisualPolyLine(Frise *frieze, FriseConfig *config);
        void                        clear();

        void                        setIsCushion(bbool _isCushion) {m_isCushion = _isCushion;}
        bbool                       isCushion() const {return m_isCushion;}

        void                        setWeightMultiplier(f32 _multiplier) {m_weightMultiplier = _multiplier;}
        void                        setLayers(const ITF_VECTOR<FluidFriseLayer> &_layerInfos) {m_layerInfos = _layerInfos;}

        void                        addFluidShaper(u32 _layerIndex, FluidShaper *_shaper);
        void                        removeFluidShaper(u32 _layerIndex, FluidShaper *_shaper);

        void                        setLayerCollisionHeightMultiplier(u32 _layerIndex, f32 _multiplier);
        u32                         getLayerCount() const {return m_layerInfos.size();}
        void                        resetAllLayerSettings();

    private:
        void                        computeInfluence(Vec2d _pos, f32 speedNorm, f32 _raidus, f32 * prevInfluence, f32 way , bbool _radiusRandom, f32 _mass);
        ITF_INLINE f32              computeFluidHeight_InString(FriseConfig *_config, EdgeFluid &_edge, u32 _edgeIndex, i32 _indexForCollision, u32 _sideIndex, StringID::StringIdValueType &gameMaterialID);
        void                        applyLocalInfluence(Frise *_frieze, FriseConfig *_config, f32 _deltaHeight, i32 _edgeIndex, i32 _referencePointIndex, i32 _startIndex, i32 _endIndex, i32 _indexStep, bbool _radiusRandom, i32 _influenceRadiusIndices);
        void                        updateUV(f32 _elapsed);

        ObjectRef                   m_frieze;
        ITF_VECTOR<EdgeFluid>       m_edgeFluidList;
        ITF_VECTOR<EdgeFluidLevel>  m_edgeFluidListLevels;
        SafeArray<ActorInfo>        m_contactActorsInfo;
        SafeArray<ObjectRef>        m_stringWaveGenerators;
        bbool                       m_isCushion; //cushion is malleable solid
        f32                         m_weightMultiplier;
        ITF_VECTOR<FluidFriseLayer> m_layerInfos;
        SafeArray<FluidShaper*>     m_virtualPerturbers[MAX_FLUID_LAYER_COUNT];
        u8         getColorIndexAt_InString(const EdgeFluid *_edge, u32 _edgeIndex, i32 _posIndex) const;
        u8         getGameMaterialIndexAt_InString(const EdgeFluid *_edge, u32 _edgeIndex, i32 _posIndex) const;
    };
} // namespace ITF

#endif  // _ITF_FRIEZE_DRAW_H_
