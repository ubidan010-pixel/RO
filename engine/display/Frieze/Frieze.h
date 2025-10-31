#ifndef _ITF_FRISE_H_
#define _ITF_FRISE_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_FRIEZE_DRAW_H_
#include "engine/display/Frieze/FriezeDraw.h"
#endif //_ITF_FRIEZE_DRAW_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

#ifdef ITF_WII
#define OPTIM_COMPUTE_LIGHT
#endif // ITF_WII

namespace ITF
{
    struct VertexAnim;
    class FriseConfig;
    class FriseTextureConfig;
    class FriezePlugin;

    class   AABBBuilder
    {
    public:
        AABBBuilder() {reset();}
        void    reset() {m_isReset = btrue;}
        void    set(const Vec2d& _value) {m_aabb = _value; m_isReset = bfalse;}
        void    set(const AABB& _value) {m_aabb = _value; m_isReset = bfalse;}
        void    grow(const Vec2d& _value) {if (m_isReset) set(_value); else m_aabb.grow(_value);}
        void    grow(const AABB& _value) {if (m_isReset) set(_value); else m_aabb.grow(_value);}
        bbool   isValid() const {return !m_isReset;}
        const AABB& get() const {ITF_ASSERT(!m_isReset); return m_aabb;}
        const Vec2d& getMin() const {ITF_ASSERT(!m_isReset); return m_aabb.getMin();}
        const Vec2d& getMax() const {ITF_ASSERT(!m_isReset); return m_aabb.getMax();}
    private:
        AABB    m_aabb;
        bbool   m_isReset;
    };

    struct MeshElementRouter
    {
        enum MeshElementTableID
        {
            MeshElementTable_Static = 0,
            MeshElementTable_Dynamic,
            MeshElementTable_Count , // must remain the last element
            ENUM_FORCE_SIZE_32(0)
        };

        static const u8     MeshElementTable_FlagDefault = 0;
        static const u8     MeshElementTable_FlagAnimated = 1;
        static const u8     MeshElementTable_FlagFill = 2;

        MeshElementTableID  m_tableID;
        u32                 m_indexInTable;
    };

    struct FillingParams
    {
        FillingParams()
            : m_cosAngle(0.f)
            , m_sinAngle(0.f)
            , m_scaleX(0.f)
            , m_scaleY(0.f)
        {
        }
        f32 m_cosAngle;
        f32 m_sinAngle;
        f32 m_scaleX;
        f32 m_scaleY;
    };

    typedef ITF_MAP<u32, MeshElementRouter> ConfigToMeshElem;

    struct FriezeRecomputeData_Static
    {
        FriezeRecomputeData_Static();
        u32                  m_edgeListCount;
        u32                  m_edgeListCountMax;
        f32                  m_texRatio;
        bbool                m_anim;
        f32                  m_uvXsign;
        f32                  m_animSyncCur;
        f32                  m_animSyncXCur;
        f32                  m_animSyncYCur;
        bbool                m_loopWanted;
        bbool                m_alphaBorderWanted;
        f32                  m_heightScale;
        f32                  m_widthScale;
        f32                  m_zVtxUp;
        f32                  m_zVtxDown;        
        bbool                m_edgeRunStartBuildVB;
        f32                  m_flexibility;
        f32                  m_heightScaleInv;
        bbool                m_isUvFlipY;
        bbool                m_isHoleVisual;
        ConfigToMeshElem     m_ConfigToMeshElemTable;
        u32                  m_MeshElementTable_ElemCount[MeshElementRouter::MeshElementTable_Count];
        f32                  m_uvXoffSet;
        f32                  m_uvYoffSet;
    };

    class Frise : public Pickable
    {
    public:

        ITF_DECLARE_NEW_AND_DELETE_OPERATORS_CATEGORY(mId_Frieze);
        DECLARE_OBJECT_CHILD_RTTI(Frise, Pickable,2579114664);
        DECLARE_SERIALIZE()

        // Mesh
        typedef SafeArray<u16, 128>             MeshAnim_IndexList;
        typedef SafeArray<VertexPNC3T, 512>     MeshAnim_VertexList;

        typedef SafeArray<u16, 128>             MeshStatic_IndexList;
        typedef SafeArray<VertexPCT, 512>       MeshStatic_VertexList;

        typedef SafeArray<u16, 128>             MeshOverlay_IndexList;
        typedef SafeArray<VertexPCBT, 512>      MeshOverlay_VertexList;

		enum PivotMode
		{
			PivotMode_Invalid = 0,
			PivotMode_Center,
			PivotMode_TopLeft,
			PivotMode_TopRight,
			PivotMode_BottomLeft,
			PivotMode_BottomRight,
            PivotMode_UserCustom,
			ENUM_FORCE_SIZE_32(0)
		};

        enum BuildFriezeMethode
        {
            InRoundness = 0,
            InGeneric,
            InExtremity, 
            InArchitecture,
            InFluid,
            InPipe,
            InExtremitySimple,
            InString,
            InAtlas,
            InOverlay,
            BuildFriezeCount,
            ENUM_FORCE_SIZE_32(BuildFriezeMethode)
        };
    
        enum LockTexture
        {            
            LockTexture_Top = 0,
            LockTexture_Right = 1,
            LockTexture_Left = 2,
            LockTexture_Bottom = 3,
            LockTexture_None = 4,
            LockTexture_Switch = 5,
            ENUM_FORCE_SIZE_32(LockTexture)
        };

        static const u32    FriezeUpdateFlag_None = 0;
        static const u32    FriezeUpdateFlag_NeeedsConfig = 1;
        static const u32    FriezeUpdateFlag_NeeedsRotation = 3;    // includes NeeedsConfig flag

#ifdef ITF_WINDOWS
        ITF_VECTOR<Vec3d>   EditorWireframeMesh; // pt1, pt2, pt3, etc.
#endif

        Frise                               (   );
        virtual ~Frise                      (   );

        u32             addObjectInGroupForUpdate(ObjectGroup* _listToFill, bbool _fromParent);
        void            update              (   f32 _ellasped );
        void            draw                (   );
        void            clearComputedData   (   );
        virtual void    setTaggedForDeletion();

        static void     RemoveFromUpdateAnimList(const ResourceID _id); 
        static void     UpdateAllAnimations (   f32 _ellapsed );
        static void     DrawAllAnimations   (   );
		void			offsetCollisionData(PolyLine* pol, const Vec2d& _offset);

        void            preCheckPoint();
        void            postCheckPoint();
        Matrix44    	getTransformMatrix  (bbool _useTranslation = btrue) const;
        Vec3d			transformPos		(const Matrix44& _matrix, const Vec3d& _pos) const;
        Vec2d			transformPos		(const Matrix44& _matrix, const Vec2d& _pos) const;        
		Vec3d			transformPos		(const Vec3d& _pos, bbool _useTranslation = btrue) const;
        Vec2d			transformPos		(const Vec2d& _pos, bbool _useTranslation = btrue) const;

        virtual void    onLoaded(HotReloadType _hotReload = HotReloadType_None);   
        virtual void    onEvent(class Event* _event);
        virtual void    requestDestruction  (   );
	    virtual void    rotate (   f32 _deltaAngle, bbool _applyToInitialValue, const Vec3d* _pOrigin = NULL );

        virtual Vec3d   getAnchorPos        (   ) const;
        ITF_INLINE bbool isDynamic          (   ) const {return m_bIsDynamic;}

        ITF_INLINE  FriseConfig*    getConfig   (   ) const     {return (FriseConfig*)m_ConfigResourceID.getResource();}
        virtual     Pickable*       clone       (   Scene* _pDstScene, bbool _callOnLoaded) const;
        static      FriseConfig*    loadFriezeConfig(const Path& _path, bbool _reload);        

        void                        getNearestEdgeFromPos(const Vec2d& _pos, PolyLineEdge& _bestEdge, i32& _bestIndex, f32& _bestRatio) const;
        void                        setUserCustomPivot(const Vec2d& _pivot);

        virtual void                setDepth    (   f32 _depth);
        virtual const Path*         getTemplatePath() const;
	    virtual void				offsetPosition          (const Vec3d& _delta, bbool _applyToInitialPosition );
	    virtual  void               setPos                      ( const Vec3d& _pos );
	    virtual  void				set2DPos                    ( const Vec2d& _pos );

		PivotMode           getPivotMode            (   ) const {return m_pivotMode;}
		void                setPivotMode            ( PivotMode _mode ) {m_pivotMode = _mode; recomputeData();}
        void                repairPivot             ();
        virtual     void    onParentChanged (ObjectRef _changedObject, PickableEditorChangeType _type);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set dynamic transform. Actual transform will be used when updated/rebuilt. Frieze must be dynamic.
        /// @param _m : Dynamic transform. Must be rotation+translation. No scale.
        void            setDynamicTransform(const GMatrix44 &_m);
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to dynamic transform
        const GMatrix44 *getDynamicTransform(void) const {return m_pFriezeFullData? &(m_pFriezeFullData->m_dynamicTransform) : NULL;}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// immediately apply dynamic tranform. Frieze must be dynamic.
        /// *CAUTION* Should not be called in normal gameplay code. Exception is in onEditorMove
        void            applyDynamicTransform(void);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// add a fluid shaper. it forces the shape of the fluid
        /// @param _layerIndex
        /// @param _shaper
        void            registerFluidShaper(u32 _layerIndex, FluidShaper *_shaper);
        void            unregisterFluidShaper(u32 _layerIndex, FluidShaper *_shaper);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get wave generators that influence this frieze. Only valid for strings
        const SafeArray<ObjectRef>* getStringWaveGenerators() const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// setFluidLayerCollisionHeightMultiplier
        /// @param _layerIndex : layer index. If no such layer, do nothing
        /// @param _multiplier : the multiplier to set
        void setFluidLayerCollisionHeightMultiplier(u32 _layerIndex, f32 _multiplier);

        ///////////////////////////////////////////////////////////////////////////////////////////
		void			forceRecomputeData();
		ITF_INLINE void recomputeData() {m_querriedRecomputeData = btrue;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool           canConnectToOtherFrieze() const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get collision data in world space. returned data is selected according to dynamic state.
        ITF_INLINE const SafeArray<PolyLine*>* getCollisionData_WorldSpace()const { if (isDynamic()) return m_pFriezeFullData ? &m_pFriezeFullData->m_dynamicCollisionData : NULL; else return m_pFriezeFullData ? &m_pFriezeFullData->m_staticCollisionData : NULL; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to static collisions in local space
        const ITF_VECTOR<PolyPointList>* getStaticCollisionData_LocalSpace() const {return m_pFriezeFullData ? &m_pFriezeFullData->m_staticCollisionsLocalSpace : NULL;}


        ResourceGroup*  getResourceGroup();
		void			deleteOwnedPolyline(PolyLine* _pol);

		void            invertPoints				(	);
        void            mirror                      ( bbool _onX, bbool _onY, const Vec2d& _pivot );
        virtual bbool   isZForced                   (   ) const;
        void            setFriseConfig              (   const Path& _name );
        const Path&     getConfigNamePath           (   ) const { return m_ConfigName; }    
        void            setGlobalColor              (   const Color& _color);
        void            setGlobalColorToMesh        (   const Color& _color);
        void            setFog                      (   const Color& _color);
        static void     clearUsedConfigs            (   ) {m_configsUsedThisFrame.clear();}        
        void            applyColorFactors           (   );
		void            computeFinalColor           (   );
        ObjectRef       getVisualPoly               (   ) const { return m_visualPoly; }
        static void     checkConnections            (   PickableList& _friseList, ITF_VECTOR<FriezeConnectionResult>& _results );
        void            preloadFileDependencies     (   );
        void            addLight                    (   const LightInfo &_light );
        bbool           getRealTimeLightning        (   )const { return m_realTimeLighting; }
        bbool           getLightComputed            (   )const { return m_lightComputed; }
        void            setRealTimeLightning        (   const bbool _set ) { m_realTimeLighting = _set; }
        const Color&    getColorFactor              (   )const { return m_colorFactor; }
        const Color&    getColorFog                 (   )const { return m_colorFog; }
        void            setColorFactor              (   const Color& _color );
        void            setColorFog                 (   const Color& _color );
        void            setColorComputerTagId       (   u32 _tagid  )   {  m_colorComputerTagId = _tagid;}
        const u32       getColorComputerTagId       (   ) const {   return m_colorComputerTagId;    }

		void			computeDynamicDisplayMatrix	(Matrix44& _dest) const;
		void			computeStaticDisplayMatrix	(Matrix44& _dest) const;

        ITF_Mesh*       getStaticMesh               (   ) { return m_pMeshStaticData ? &m_pMeshStaticData->m_mesh : NULL; }       

        virtual void    onSceneActive();
        virtual void    onSceneInactive();
		virtual void    onBecomeActive();
		virtual void    onBecomeInactive();
        virtual void    notifyParentsAndChildren (PickableEditorChangeType _type );
        
        void    scale(const Vec2d & _deltaScale, bbool _applyToInitialValue);
        void    scale( const Vec2d& _center, const Vec2d & _deltaScale, bbool _applyToInitialValue );

	    bbool           erasePosAt              (   u32 _index );
        void            clearPoints()   { m_pointsList.clear(); }
        f32             getEdgeScaleAtRatio(u32 _index, f32 _ratio) const { return m_pointsList.getPointScaleAt(_index) +(m_pointsList.getPointScaleAt(_index+1)-m_pointsList.getPointScaleAt(_index))*_ratio;}
		void            registerPhysic();
		void            unregisterPhysic();
virtual void            onAddedToScene(Scene* _pScene);
virtual void            onRemovedFromScene(Scene* _pScene,bbool _forDeletion);
    ObjBinding*         getBinding() { return &m_binding; }
    const ObjBinding*   getBinding() const { return &m_binding; }
	ITF_INLINE u32		getPosCount() const {return m_pointsList.getPosCount();}
	ITF_INLINE const PolyPointList&		getPointsData() const {return m_pointsList;}
	ITF_INLINE PolyLineEdge::HoleMode  getHoleAt(   u32 _index  ) const {return getPointsData().getHoleAt(_index);}
    ITF_INLINE const Vec2d&  getPosAt           (   u32 _index  ) const { return getPointsData().getPosAt(_index);}
    ITF_INLINE const PolyLineEdge&  getEdgeAt	(   u32 _index  ) const { return getPointsData().getEdgeAt(_index);}
    ITF_INLINE bbool				isLooping( ) const { return getPointsData().isLooping();}
	ITF_INLINE void rotatePoint(u32 _index, f32 _deltaAngle, const Vec3d& _rotationCenter) {m_pointsList.rotatePoint(_index, _deltaAngle, _rotationCenter, getDepth()); recomputeData();}
	ITF_INLINE void switchTextureAt ( u32 _index) {m_pointsList.switchTextureAt(_index);}
    ITF_INLINE void setHoleAt (   u32 _index, PolyLineEdge::HoleMode _setHole ) {m_pointsList.setHoleAt(_index, _setHole);}
	ITF_INLINE void setPosAt (const Vec2d& _pos, u32 _i ) {m_pointsList.setPosAt(_pos, _i ); recomputeData();}
	ITF_INLINE void addPoint (const Vec2d& _pos, i32 _at = -1 ) {m_pointsList.addPoint(_pos, _at ); recomputeData();}
	ITF_INLINE void transferEdgeProperties ( u32 _index, const PolyLineEdge& _edg ) {m_pointsList.transferEdgeProperties(_index, _edg);}
	ITF_INLINE void setLoop (   bbool _loop) {m_pointsList.setLoop(_loop); recomputeData();}
    ITF_INLINE void     forceLoop (   bbool _loop) {m_pointsList.forceLoop(_loop); recomputeData();}
    ITF_INLINE void  checkLoop( ) {m_pointsList.checkLoop();}
    ITF_INLINE void switchTexturePipeExtremity() { m_switchTexturePipeExtremity = ( m_switchTexturePipeExtremity +1) %4;}
    ITF_INLINE void     addEdge( const PolyLineEdge& _edge) { m_pointsList.addEdge(_edge);}
    static const MeshElementRouter* getRoutingElement(u8 _flags, u32 _textureConfigIndex);
    void                createElementInRoutingTable (   MeshElementRouter::MeshElementTableID _tableID, u32& _index, u8 _flags, u32 _textureConfigIndex ) const;

    ITF_INLINE f32      getPointScaleAt         (   u32 _index  ) const { return m_pointsList.getPointScaleAt(_index);  }
    ITF_INLINE void                setPointScaleAt         (   u32 _index, f32 _scale ) { m_pointsList.setPointScaleAt( _index, _scale); recomputeData(); }
    ITF_INLINE void                resetPointScaleAt       (   u32 _index ) { setPointScaleAt( _index, 1.f ); }
    ITF_INLINE void                resetAllPointsScale     (   ) { m_pointsList.resetAllPointsScale(); recomputeData(); }

    ITF_INLINE bbool	isFilling               (   ) const { return m_isFilling;}

    // Mesh Static
    void    clearMeshStaticData            (   );
    void    buildMesh_Static            (   const MeshStatic_VertexList& _vertexTempList, const MeshStatic_IndexList* _indexTempList, const Matrix44* _pMatrixTransfo );      
    void    addIndex_Static_Quad        (   MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, u16& _indexVtxDown, u16& _indexVtxUp, bbool _flip ) const;
    void    buildVB_Static_Quad         (   MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, Vec2d* _pos, Vec2d* _uv, u32* _color, bbool _flip ) const;

	Vec3d					inverseTransform(const Vec3d& _v) const;
	Vec2d					inverseTransform(const Vec2d& _v) const;
#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorCreated( Pickable* _original = NULL);
        void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
        void                setglobalAlphaFade(f32 _fade)   { m_globalAlphaFade = _fade; }
        void                setDynamic(bbool _b)            { m_isDynamicFrieze = _b; }
        void                setAsFirstPoint( u32 _indexOffset);
        ITF_INLINE ObjectRefList & getEventListeners() { return m_eventListeners; }
        ITF_INLINE void addEventListener(ObjectRef val) 
        { 
            if (m_eventListeners.find(val) == -1)
                m_eventListeners.push_back(val);
        }
        ITF_INLINE void removeEventListener(ObjectRef val) 
        { 
            i32 index = m_eventListeners.find(val);
            if (index != -1)
                m_eventListeners.eraseNoOrder(index);
        }

        static void     freeResourceGroup( ResourceGroup* _group);

    private:
        struct FreeVBData
        {
            u32                     m_timeToGo;
            class ITF_VertexBuffer* m_buffer;
        };
        struct FreeIBData
        {
            i32                     m_timeToGo;
            class ITF_IndexBuffer*  m_buffer;
        };
        struct FreeGroupData
        {
            i32                     m_timeToGo;
            class ResourceGroup*    m_group;
        };
        static SafeArray<FreeVBData>    m_VBDataToFree;
        static SafeArray<FreeIBData>    m_IBDataToFree;
        static SafeArray<FreeGroupData> m_groupDataToFree;
        static void     FreeVB              ( class ITF_VertexBuffer* _buffer   );
        static void     FreeIB              ( class ITF_IndexBuffer* _buffer  );
        static void     flushGraphicData();

		void             convertFromOldDataFormat();
        void            computeOrientedGlobalAABB(AABBBuilder& _aabb, u32 _colIndex) const;

        f32               m_globalAlphaFade;
        u32              m_updateFlags;

        void            clearCollisionData          (   );
        void            setupCollisionPolyline      (PolyLine* _col) const;
        void            addStaticCollisionPolyline  (   PolyLine* _col );
        void            addDynamicCollisionPolyline (   PolyLine* _col );
        void            makeDynamicCollisionsFromStatic();
        void            lightProcess();
        void            udpateCollisionData(f32 _elapsed);

        ResourceID      m_resourceGroup;

        //////////////
        // serialize  
        //////////////
		PolyPointList m_pointsList;
		PivotMode	m_pivotMode;
        Vec2d       m_customUserPivot;
        Path        m_ConfigName;
#ifdef ITF_SUPPORT_EDITOR
        Path        m_editConfigName;
#endif
        u32         m_switchTexturePipeExtremity;

        Color       m_colorFactor;
        Color       m_colorFog;
        u32         m_colorComputerTagId;
        bbool       m_renderInTarget;

        //  uv
        f32     m_uvXoffSet;
        f32     m_uvYoffSet;
        bool    m_uvX_flip;
        bool    m_uvY_flip;
        f32     m_uvY_tile;
        
        // fill
        Vec2d   m_fillOffSetUv;

        //  vertex anim        
        f32     m_animGlobalSync;

        bbool   m_isDynamicFrieze; //This flag is similar to m_isDynamic, but can set by the property editor. When it changes, it will be used to set m_isDynamic.

        // atlas
        u32     m_atlasIndex;

		f32         m_thickness;

        LockTexture m_lockTexture;

        /////////////////
        // no serialize
        /////////////////

        static FriezeRecomputeData_Static m_recomputeData;

#ifdef SKIP_NODRAW_UPDATE
        f32     m_skippedDt;
#endif // SKIP_NODRAW_UPDATE
        ResourceID  m_ConfigResourceID;
        bbool   m_regionRegistered;
        f32     m_timeCur;
        Angle   m_animGlobalRotCur;
        Color   m_LightColorComputed;
        bbool   m_isFilling;

        SafeArray<LightInfo>  m_lights;
        bbool m_realTimeLighting;
        bbool m_lightComputed;

        static ITF_VECTOR<ResourceID> m_configsUsedThisFrame;
        static ITF_VECTOR<ResourceID> m_configsNeedingAnimDraw;
                                                          
        struct edgeRun
        {
            edgeRun()
                : m_idEdgeStart(0)
                , m_edgeCount(0)
                , m_idTex(-1)
                , m_coeff(1.f)
                , m_idZone(0)
            {
            }

            u32 m_idEdgeStart;
            u32 m_edgeCount;
            i32 m_idTex;
            u32 m_idZone;
            f32 m_coeff;

        };
       
        struct MeshFluidData
        {
            DataFluid       m_data;
            AABBBuilder     m_AABB;
        };
        MeshFluidData* m_pMeshFluidData;

        struct MeshOverlayData
        {
            ITF_Mesh        m_mesh;
            AABBBuilder     m_AABB;
            Vec2d           m_angle;
            Vec2d           m_syncVtx[8];
            Vec2d           m_sync[8];
        };
        MeshOverlayData* m_pMeshOverlayData;

        struct MeshAnimData
        {
            ITF_Mesh        m_mesh;
            SafeArray<u32>  m_elemConfig_IndexList;
            AABBBuilder     m_AABB;
        };
        MeshAnimData* m_pMeshAnimData;

        struct MeshStaticData
        {                            
            ITF_Mesh        m_mesh;
            SafeArray<u32>  m_elemConfig_IndexList;
            AABBBuilder     m_AABB;
        };
        MeshStaticData* m_pMeshStaticData;

        struct FriezeFullData
        {
            // dynamic
            AABB        m_AABB_Mesh_Dynamic;
            AABB        m_AABB_Anim_Dynamic;        
            AABB        m_AABB_Overlay_Dynamic;
            GMatrix44   m_dynamicTransform;

            //  collision
            ITF_VECTOR<PolyPointList>	m_staticCollisionsLocalSpace;
            SafeArray<PolyLine*>		m_staticCollisionData;
            SafeArray<PolyLine*>		m_dynamicCollisionData;
            AABBBuilder                 m_AABB_Collision;
                       
            void init();
        };
        FriezeFullData* m_pFriezeFullData;

        void        createFriezeFullData();
        void        deleteFriezeFullData();

        // visual Polyline
        ObjectRef               m_visualPoly;
        
        enum CollisionDouble
        {
            None = 0,
            Double,
            DoubleAndLoop,
            Count,
            ENUM_FORCE_SIZE_32(CollisionDouble)
        };

        struct collisionRun
        {
            collisionRun()
                : m_idEdgeRunStart(0)
                , m_edgeRunCount(0)
                , m_idEdgeStart(0)
                , m_edgeCount(0)
                , m_edgeCountStartOffset(0)
                , m_edgeCountStopOffset(0)
                , m_collisionOffsetLast(-1.f)
                , m_collisionOffsetNext(-1.f)
                , m_index(0)
                , m_collisionDouble(None)
            {
            }

            u32 m_idEdgeRunStart;
            u32 m_edgeRunCount;
            u32 m_idEdgeStart;
            u32 m_edgeCount;
            u32 m_edgeCountStartOffset;
            u32 m_edgeCountStopOffset;

            f32 m_collisionOffsetLast;
            f32 m_collisionOffsetNext;

            u32 m_index;
            CollisionDouble m_collisionDouble;

        };

        struct CollisionByGameMat
        {
            i32 m_startId;
            i32 m_stopId;
            Path m_gameMat;

        };

        struct edgeFrieze
        {
            edgeFrieze()
                : m_pos(Vec2d::Zero)
                , m_sight(Vec2d::Zero)
                , m_sightNormalized(Vec2d::Zero)
                , m_normal(Vec2d::Zero)
                , m_interUp(Vec2d::Zero)
                , m_interDown(Vec2d::Zero)
                , m_cornerNormal(Vec2d::Zero)
                , m_cornerAngle(0.f)
                , m_build(btrue)
                , m_snap(bfalse)
                , m_norm(0.f)
                , m_startCoeff(0.f)
                , m_normUv(0.f)
                , m_holeMode(PolyLineEdge::Hole_None)
                , m_heightStart(1.f)
                , m_heightStop(1.f)
                , m_scaleCur(1.f)
                , m_scaleInit(1.f)
                , m_switchTexture(bfalse)
                , m_stopCoeff(1.f)
                , m_idPoint(0)
            {
                m_points[0] = m_points[1] = m_points[2] = m_points[3] = Vec2d::Zero;
            }

            Vec2d       m_pos;
            Vec2d       m_sight;
            Vec2d       m_sightNormalized;
            Vec2d       m_normal;
            Vec2d       m_interUp;
            Vec2d       m_interDown;
            Vec2d       m_cornerNormal;
            f32         m_cornerAngle;
            bbool       m_build;
            bbool       m_snap;
            f32         m_norm;
            f32         m_startCoeff;
            f32         m_normUv;
            PolyLineEdge::HoleMode       m_holeMode;
            f32         m_scaleCur;
            f32         m_scaleInit;
            f32         m_heightStart;
            f32         m_heightStop;
            bbool       m_switchTexture;
            Vec2d       m_points[4];
            f32         m_stopCoeff;
            u32         m_idPoint;
        };

        struct CornerRounder
        {
            CornerRounder()
                : m_edgeCur(NULL)
                , m_edgeLast(NULL)
                , m_edgeRun(NULL)
                , m_config(NULL)
                , m_indexVtxUp(NULL)
                , m_indexVtxDown(NULL)
                , m_uvXsign(NULL)
                , m_uvDown(NULL)
                , m_uvUp(NULL)
                , m_uvDownXOffSet(NULL)
                , m_uvUpXOffSet(NULL)
                , m_indexVtxCenter(NULL)
                , m_indexVtxCorner(NULL)
                , m_uvCorner(NULL)
                , m_zVtxCorner(0.f)
                , m_colorCorner(0)
                , m_startPos(Vec2d::Zero)
                , m_stopPos(Vec2d::Zero)
                , m_centerPos(Vec2d::Zero)
                , m_cornerPos(Vec2d::Zero)
                , m_uvXoffsetCorner(NULL)
                , m_edgeCountCur(NULL)
            {
            }

            const edgeFrieze* m_edgeCur;
            const edgeFrieze* m_edgeLast;
            const edgeRun* m_edgeRun;
            FriseConfig* m_config;
            u16* m_indexVtxUp;
            u16* m_indexVtxDown;
            f32* m_uvXsign;
            Vec2d *m_uvDown;
            Vec2d *m_uvUp;
            f32* m_uvUpXOffSet;
            f32* m_uvDownXOffSet;

            u16* m_indexVtxCenter;
            u16* m_indexVtxCorner;
            Vec2d* m_uvCorner;
            f32* m_uvXoffsetCorner;
            f32  m_zVtxCorner;
            u32 m_colorCorner;
            Vec2d m_startPos;
            Vec2d m_stopPos;
            Vec2d m_centerPos;
            Vec2d m_cornerPos;

            u32* m_edgeCountCur;

        };

        //dynamic transform
        bbool m_bIsDynamic; //dynamic flag is used to know if the frieze is dynamic at a given point of the code
        

        ITF_MAP<FriseTextureConfig*, MeshStatic_IndexList>     m_mapTextureToMeshStaticIndexList;

#ifdef DEVELOPER_JAY_FRIEZE
        void    debugDrawInt                (   Vec2d pos2d, u32 _i, Color _color );
        void    debugDrawFloat              (   Vec2d pos2d, f32 _f, Color _color );
        void    debugDrawEdgeList           (   ITF_VECTOR<edgeFrieze>& _edgeList );
        void    debugDrawEdgeRun            (   ITF_VECTOR<edgeRun>& _edgeRunList );       
        u32     m_recomputeCount;
#endif

        void    preProperties               (   );
        void    postloadProperties          (   );        
        void    renderFrieze                (   );        
        void    updateMeshElemFromConfig    (   ITF_Mesh* _pmesh, SafeArray<u32>& _elemconfigIndexlist    );
		virtual bbool	updatePhysicalReady (	);
		virtual void    unloadResources		(	);
        void    createNewPolylineVisual     (   const ITF_VECTOR<edgeFrieze>& _edgeList  );
        void    clearVisualPolyline         (   );                
        void    makeDynamicGraphicAABB      (   );
        void    computeDynamicAABB(const AABB &_src, AABB &_dst, const Matrix44 &_m);
        void    executeRecomputeData        (   );
        void    initDatas                   (   );
        i32     getFirstNoFillTextureIndex  (   const FriseConfig* config ) const;
        void    checkDataConsistency        (   );
        void    updateAABB                  (   FriseConfig* _config);

        // Edges In Loop
        u32     findLastBuildEdgeInLoop     (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdge ) const;

        // Edges
        bbool   skipEdgeFrieze              (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idToSkip, u32 _idToUpdate, u32 _idLast ) const;        
        void    finalizeEdgeFriezeList      (   const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        bbool   updateEdgeFriezeVector      (   edgeFrieze& _edgeToUpdate, Vec2d _vector ) const;
        void    copyEdgeFromPolyline        (   ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        bbool   buildEdgeCorner             (   FriseConfig* config, edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, f32 _cornerLongMax, f32& _dotSight ) const;
        bbool   skipWrongEdge               (   edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, f32& _dotSight ) const;    
        void    setNormEdge                 (   edgeFrieze& _edge ) const;
        bbool   skipSmallEdge               (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeFrieze& _edgeLast, edgeFrieze& _edgeCur, f32 _normMin, i32 _idEdgeLastLast ) const;
        void    snapEdge                    (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeToSnap, u32 _idEdgeToUpdate ) const;
        bbool   findNextBuildEdge           (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 &_idEdge ) const;
        bbool   findPreviousBuildEdge       (   ITF_VECTOR<edgeFrieze>& _edgeList, i32 &_idEdge ) const;
        void    buildEdgeList               (   ITF_VECTOR<edgeFrieze>& _edgeList, bbool _snapEdgeLast = btrue ) const;
        void    snapAllEdge                 (   ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        f32     getEdgeNormMin              (   const FriseConfig* _config ) const;
        f32     getEdgeRunNormMin           (   const FriseConfig* _config ) const;
        void    updateIdEdgeList            (   u32& _idCurEdge, u32& _idLastEdge, u32* _idLastEdgeValidList ) const;
        bbool   insertNewEdge               (   ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge ) const;
        bbool   setOffsetEdge               (   edgeFrieze& curEdge, edgeFrieze& lastEdge, Vec2d& _posInit, f32 _offset, f32 longMax, f32 longMaxSqr) const;
        bbool   isEdgeWithHoleCollision     (   const edgeFrieze& _edge ) const;
        bbool   isEdgeWithHoleVisual        (   const edgeFrieze& _edge ) const;
        bbool   isEdgeWithHole              (   const edgeFrieze& _edge ) const;
        bbool   updateEdgeFriezePosition    (   edgeFrieze& _edge, Vec2d& _pos ) const;
        void    subtractOffsetExtremity     (   ITF_VECTOR<edgeFrieze>& _edgeList, f32 _offset ) const;
        bbool   isEdgeRunWithSwitchTexture  (   edgeRun& _edgeRun, ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        void    setScaleNewEdge             (   edgeFrieze& _edgeNew, edgeFrieze& _edgeLast, edgeFrieze& _edgeNext ) const;
        Vec2d   getPosStartOnNormalEdge     (   const edgeFrieze* _edge, f32 _offset = 1.f ) const;
        Vec2d   getPosStopOnNormalEdge      (   const edgeFrieze* _edge, f32 _offset = 1.f ) const;
        Vec2d   getPosOnCornerEdge          (   const edgeFrieze* _edge, f32 _offset = 1.f ) const;
        bbool   setScaleAverage             (   edgeFrieze& _edgeCur, edgeFrieze& _edgeLast ) const;
        bbool   isEdgeRun                   (   ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge ) const;
        bbool   getIdLastLastEdgeBuild      (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idLastEdge, u32& _idLastLastEdge ) const;
        void    buildEdgePoints             (   const FriseConfig* _config, edgeFrieze& _edgeCur) const;
        bbool   setIntersectionUp           (   const edgeFrieze& _edgeLast, edgeFrieze& _edgeCur) const;
        bbool   setIntersectionDown         (   const edgeFrieze& _edgeLast, edgeFrieze& _edgeCur) const;
        bbool   skipBadIntersection         (   const edgeFrieze& _edge, const Vec2d& _interLast, const Vec2d& _interCur) const;
        void    snapEdgesPointsWithInter    (   edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, edgeFrieze* edgeLast2, bbool _allEdgeSnaped) const;
        Vec2d   getEdgeBorderLeft           (   const edgeFrieze& _edge) const;
        Vec2d   getEdgeBorderRight          (   const edgeFrieze& _edge) const;
        void    updateEdgeHeight            (   edgeFrieze& _edgeCur, edgeFrieze& _edgeLast) const;
        bbool   isHeightEqual               (   const edgeFrieze& _edgeLast, const edgeFrieze& _edgeCur) const;
        void    snapEdgeListPointsWithInter (   const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        u32     getCountBuildEgdes          (   ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        u32     getCountSnapEgdes           (   ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        bbool   isScaleEqual                (   const edgeFrieze& _edgeLast, const edgeFrieze& _edgeCur) const;
        bbool   isSnapAllEdgeInBubble       (   ITF_VECTOR<edgeFrieze>& _edgeList) const;
        f32     getNormMinToSnap            (   ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge, f32 _normMinPoint, f32 _normMinUv) const;
        void    setPosStartOnEdgeWithHoleVisual  (   const FriseConfig* _config, const edgeFrieze& edge, Vec2d& _down, Vec2d& _up) const;
        void    setPosStopOnEdgeWithHoleVisual( const FriseConfig* _config, const edgeFrieze& edge, Vec2d& _down, Vec2d& _up, u32 _idPoint) const;
        u32     getIdEdgeNext               (   u32 _id) const;
        u32     getIdEdgeLast               (   u32 _id) const;
                
        // Uvs
        void    setEdgeRunCoeffUv           (   FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const;
        bbool   setEdgeRunListCoeffUv       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    orderEdgeRunListByTextureId (   ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        f32     getUvX_Start                (   ) const;
        u32     getCornerStepNb             (   f32 _angleFan, f32 _step, f32 _scale ) const;
        void    setCornerRounderDatas       (   FriseConfig* _config, const edgeFrieze* _edge, f32 _uvCoeff, u32& _stepNb, f32& _stepCornerAngle, f32& _uvXStep, f32& _arcBy2, f32 _uvXsign ) const;
        f32     getCornerSquareArcBy2       (   f32 _cornerAngle, f32 _uvCoeff, Vec2d _vector, f32 _uvXsign ) const;
        void    setEdgeRunNormUv            (   FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const;
        u32     getIdEdgeStartWithoutSnap   (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const;
        f32     getStepCornerCoeff          (   f32 _uvCoeff, f32 _angle ) const;
        bbool   flipEdge                    (   f32 _offset ) const;
        void    updateUvs                   (   const edgeRun& _edgeRun, const edgeFrieze* _edge, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign, f32 _uvUpXOffSet, f32 _uvDownXOffSet ) const;
        void    updateEdgeCornerUvs         (   CornerRounder& _corner, f32& _vtxUvX, f32 _uvLeft, f32 _uvRight ) const;
        f32     getCornerRounderUv          (   const FriseConfig* config, f32 _angle, f32 _uvCoeff, f32 _height, f32 _scale ) const;
        f32     getUvCornerBy2              (   const FriseConfig* _config, edgeRun& _edgeRun, edgeFrieze* _edgeCur, edgeFrieze* _edgeLast) const;
        void    setEdgeNormUv               (   edgeFrieze* _edge, f32 _ratio, edgeFrieze* _edgeLast, edgeFrieze* _edgeNext) const;
        f32     getCoeffScaleStepFanToBlend (   const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast) const;

        // In Roundness
        void    buildFrieze_InRoundness     (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static ) const;    
        void    buildVB_InRoundness_Static  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList ) const;
        void    buildRoundness_Static       (   FriseConfig* config, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, struct UvsByAngle_InRoundness* _uvTab, Vec2d& _vCenter, Vec2d& _vStart, Vec2d& _vStop, f32 _angle, u32 _vtxColorCenter, u32 _vtxColorCorner ) const;
        void    buildCollision_InRoundness  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        bbool   buildEdgeRunList_InRoundness(   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        u32     buildRoundness_getIndexUv   (   f32 _angle ) const;
        i32     getTexIdBySlope_InRoundness (   Vec2d _sight, FriseConfig* _pConfig ) const;
        u32     getIdStartEdge_InRoundness  (   ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* _pConfig ) const;
        bbool   isEdgeRun_InRoundness       (   ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const;
        bbool   isAngleBreak_InRoundness    (   edgeFrieze& _edge ) const;

        // In Generic
        void    buildFrieze_InGeneric       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const;
        bbool   buildEdgeRunList_InGeneric  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildCollision_InGeneric    (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildVB_InGeneric           (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const;

        // Build Fill
        f32     getFillOffSet               (   const FriseConfig* config, const edgeRun* _edgeRun ) const;
        bbool   buildFilling                (   const ITF_VECTOR<edgeFrieze>& _edgeList, const ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList ) const;
        bbool   buildFilling_VB             (   const FriseConfig* config, SafeArray<Vec2d>& filling, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList )const;
        void    setFillingSafe              (   const FriseConfig* config, const SafeArray<Vec2d>& _fillingOld, SafeArray<Vec2d>& _fillingSafe ) const;
        void    buildVB_FillGradient        (   ITF_VECTOR<edgeFrieze>& _edgeList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList )const;
        void    buildFilling_Points         (   const FriseConfig* config, SafeArray<Vec2d>& filling, const ITF_VECTOR<edgeFrieze>& _edgeList, const ITF_VECTOR<edgeRun>& _edgeRunList ) const;

        // Build Outline
        void    buildOutline_EdgeRunWithCornerRounder   ( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRun, f32 _offset, f32 _stepFan, u32 _edgeCountStartOffset = 0, u32 _edgeCountStopOffset = 0, bbool _isOutlineCollision = btrue ) const;
        void    buildOutline_StartEdgeRunCornerRounder      ( const FriseConfig* config, SafeArray<Vec2d>& _outline,  const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRunLast, const edgeRun* _edgeRunCur, f32 _offsetLast, f32 _offsetCur, f32 _stepFan, bbool _isOutlineCollision = btrue ) const;        
        void    buildOutline_CornerRounder  (   SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offset, f32 _stepFan ) const;
        void    buildOutline_CornerSquare   (   SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offset ) const;
        void    buildOutline_EdgeRunWithCornerSquare ( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRun, f32 _offset, u32 _edgeCountStartOffset = 0, u32 _edgeCountStopOffset = 0, bbool _isOutlineCollision = btrue) const;
        void    buildOutline_StartEdgeRunCornerSquare ( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRunLast, const edgeRun* _edgeRunCur, f32 _offsetCur, f32 _offsetLast, bbool _isOutlineCollision = btrue) const;
        f32     getCornerEdgeNorm           (   f32 _angle, f32 _radius ) const;
        void    setPos_CornerSquareBlending (  Vec2d& _posStart, Vec2d& _posStop, Vec2d& _posSquare, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offsetCur, f32 _offsetLast ) const;
        void    buildOutline_CornerSquareBlending_startEdgeRun( SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeCur, f32 _offsetCur, f32 _offsetLast ) const;
        void    buildOutline_CornerSquareBlending_stopEdgeRun( SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeCur, f32 _offsetCur, f32 _offsetNext ) const;
        void    buildOutline_initPosCornerRounderBlending( const edgeFrieze* _edgeLast, const edgeFrieze* _edgeCur, Vec2d &_p1, Vec2d &_p2, f32 _offsetCur = 1.f, f32 _offsetLast = 1.f) const;
        void    buildOutline_CornerRounderBlending( SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeLast, const edgeFrieze* _edgeCur, f32 _offsetLast, f32 _offsetCur, f32 _stepFan, i32 _countOffset = 0, Vec2d* _posCorner = NULL) const;
        void    buildOutline_CornerSquareBlending( SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offsetCur, f32 _offsetLast ) const;
        bbool   buildOutline_isStartEdgeRunWithRounder( const FriseConfig* _config ) const;
        bbool   buildOutlineFromHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur, bbool _isOutlineCollision ) const;
        bbool   buildOutlineFromHoleVisual( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur ) const;
        bbool   buildOutlineFromHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur ) const;
        void    buildOutline_PosStartOnEdgeWithHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio) const;
        void    buildOutline_PosStopOnEdgeWithHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio, u32 _idPoint) const;

        void    buildOutline_PosStopOnEdgeWithHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio, u32 _idPoint) const;
        void    buildOutline_PosStartOnEdgeWithHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio) const;


        // Mesh Common
        void    setVtxColor                 (   const FriseTextureConfig& _texConfig, u32& _vtxColorUp, u32& _vtxColorDown ) const;
        void    setVtxColorWithExtrem       (   const FriseTextureConfig& _texConfig, u32& _vtxColorUp, u32& _vtxColorDown, u32& _vtxColorUpExt, u32& _vtxColorDownExt, bbool _forceAlpha = bfalse ) const;
        void    setMeshElement              (   ITF_MeshElement& _elem, const FriseConfig* _friseConfig, u32 _indexTableSize, u32 _idTexConfig ) const;        
        void    setQuadPos_StartExtremity   (   edgeFrieze& _edge, Vec2d* _pos, f32 _visualOffset, f32 _heightMid, f32 _width, bbool _isSnaped ) const;
        void    setQuadPos_StopExtremity    (   edgeFrieze& _edge, Vec2d* _pos, f32 _visualOffset, f32 _heightMid, f32 _width, bbool _isSnaped ) const;
        void    setQuadColor_StartExtremity (   u32* _color, u32 _vtxColorDown, u32 _vtxColorUp, u32 _vtxColorDownExtremity, u32 _vtxColorUpExtremity ) const;        
        void    setQuadColor_StopExtremity  (   u32* _color, u32 _vtxColorDown, u32 _vtxColorUp, u32 _vtxColorDownExtremity, u32 _vtxColorUpExtremity ) const;
        void    setVtxBorderLeftPosDown     (   const edgeFrieze& _edge, Vec3d& _vtxPos) const;
        void    setVtxBorderLeftPosUp       (   const edgeFrieze& _edge, Vec3d& _vtxPos) const;
        void    setVtxBorderRightPosDown    (   const edgeFrieze& _edge, Vec3d& _vtxPos) const;
        void    setVtxBorderRightPosUp      (   const edgeFrieze& _edge, Vec3d& _vtxPos) const;
        void    setVtxPosUp                 (   const Vec2d& _pos, Vec3d& _vtxPos) const;
        void    setVtxPosDown               (   const Vec2d& _pos, Vec3d& _vtxPos) const;
        void    setVtxPos                   (   const Vec2d& _pos, Vec3d& _vtxPos, f32 _z) const;
        void    setFlipQuadWithAlphaBorder  (   bbool& _flip, u32 _colorLeft, u32 _colorRight) const;

        // Mesh Anim
        void    addIndex_Anim               (   MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, u16 _index ) const;        
        void    clearMeshAnimData              (   );
        void    buildMesh_Anim              (   MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList* _indexTempList, const Matrix44* _pMatrixTransfo );        
        void    buildVB_Anim_EdgeRun        (   FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign ) const;
        void    buildVB_Anim_CornerRounder  (   MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, CornerRounder& _corner ) const;
        void    buildVB_Anim_CornerSquare   (   MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, CornerRounder& _corner ) const;
        void    setVtxAnim                  (   VertexAnim& _anim, VertexPNC3T& _vtx ) const;
        void    setVtxAnimNull              (   VertexPNC3T& _vtx ) const;
        void    setVtxAnimOnEdge            (   VertexAnim& _anim, const edgeFrieze& _edge, VertexPNC3T& _vtx ) const;
        void    setVtxAnimOnEdge            (   VertexAnim& _anim, Vec2d& _vector, VertexPNC3T& _vtx ) const;
        void    setVtxAnimOnCorner          (   VertexAnim& _anim, const edgeFrieze& _edge, VertexPNC3T& _vtx ) const; 
        void    copyVtxAnim                 (   VertexPNC3T& _vtxTo, VertexPNC3T& _vtxFrom ) const;
        void    addIndex_Anim_Quad          (   MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, u16& _indexVtxDown, u16& _indexVtxUp, bbool _flip ) const;
        void    buildVB_Anim_Quad           (   VertexAnim& _anim, MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, Vec2d* _pos, Vec2d* _uv, u32* _color, bbool _flip, bbool _vtxAnimStart, bbool _vtxAnimStop ) const;
        void    buildVB_Anim_CornerRounderBlending( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, CornerRounder& _corner ) const;
        void    buildVB_Anim_Corner         (   MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, CornerRounder& _corner ) const;
        void    buildVB_Anim_EdgeRunCorner  (   FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList& _indexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32& _uvUpXOffSet, f32& _uvDownXOffSet, f32 _uvXsign ) const;
        void    buildVB_Anim_addVtxOnStartPosEdge( VertexAnim& _anim, const edgeFrieze& edge, MeshAnim_VertexList& _vertexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown ) const;
        void    buildVB_Anim_addVtxOnStopPosEdge( VertexAnim& _anim, const edgeFrieze& edge, MeshAnim_VertexList& _vertexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown ) const;
        GFX_Vector4    getvParamsMeshAnim   (   const VertexAnim* _anim );
#ifdef ITF_WII
        void    reorderVertexAndIndexListByAnim( MeshAnim_VertexList& _vertexTempList, MeshAnim_IndexList* _indexTempList, u32 _indexTempListSize) const;
        u16     getIndexLastWithoutVtxAnim  (   u16 _indexStop, MeshAnim_VertexList& _vertexTempList) const;
        bbool   isVtxAnim                   (   VertexPNC3T& _vtx) const;
#endif //ITF_WII

        //
        void    buildVB_Static_Corner(   MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, CornerRounder& _corner ) const;
        void    buildVB_Static_CornerSquare (   MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, Vec2d& _uvSquare, f32 _stepUvX, Vec2d& _vSquare, Vec2d& _vStop, u16 _indexVtxCenter, u16& _indexVtxSquare, f32 _angle, u32 _color );
        void    buildVB_Static_EdgeRunCorner( FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32& _uvUpXOffSet, f32& _uvDownXOffSet, f32 _uvXsign ) const;
        void    buildVB_Static_EdgeRun      (   FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign ) const;        
        void    buildVB_Static_CornerRounder(   MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, CornerRounder& _corner ) const;
        void    buildVB_Static_CornerSquare (   MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, CornerRounder& _corner ) const;
        void    buildVB_Static_CornerRounderBlending( MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, CornerRounder& _corner ) const;

        // Collision
        void    initCollisionData           (   u32 _collisionWantedCount );
        f32     getCollisionOffSet          (   const FriseConfig* config, const edgeRun* _edgeRun, collisionRun& _colRun ) const;
        f32     getCollisionOffSetSimple    (   const FriseConfig* config, const edgeRun* _edgeRun ) const;
        const Path& getCollisionGameMaterial(   const FriseConfig* _friseConfig, i32 _idTex ) const;        
        bbool   isEdgeRunCollisionable      (   const FriseConfig* _friseConfig, const edgeRun& _edgeRun, const edgeFrieze& _edge ) const;
        void    buildCollision_EdgeRunList  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        bbool   flipCollision               (   ) const;
        bbool   isEdgeRunWithCollisionHole  (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun, u32& _idEdgeRunStart, u32& _edgeCountStart, u32 _edgeRunListCount ) const;
        void    getIdEdgeRunStartCollision  (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, u32& _idEdgeRunStart, u32& _idEdgeCountStart  ) const;
        void    initCollisionRun            (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, u32 _idEdgeRunStart, u32 _idEdgeStart, u32 _edgeCountStartOffset ) const;
        bbool   checkStartCollisionRun      (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, bbool& _build, u32& _idEdgeRunOld, u32 _idEdgeRun, u32 _idEdge, u32 _edgeCountStartOffset ) const;
        bbool   checkStopCollisionRun       (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList, collisionRun& _collisionRun, bbool& _build, u32 _edgeCountStopOffset, u32 _idEdgeRun );
        bbool   buildCollisionRunList       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    addPointsCollision          (   const FriseConfig* _config, SafeArray<Vec2d>& collision, ITF_VECTOR<CollisionByGameMat>& _collisionList, collisionRun& _colRun );
        void    finalizeCollisionList       (   );
        void    finalizeCollisionRun        (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, u32 _edgeCountStopOffset, u32 _idEdgeRun ) const;
        void    verifyCollision             (   ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList ) const;
        bbool   AddPointInCollisionData     (   const FriseConfig* _config, Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32& _indexPoint, u32 _indexCollisionData, Path& _gameMat );
        void    AddPointInCollisionDataWithGMat(   const FriseConfig* _config, Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32& _indexPoint, Path& _gameMat, u32 _indexCollisionData );
        bbool   skipEdgeParallelInCollisionData( const FriseConfig* _config, Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32 _indexPoint, u32 _indexCollisionData, Path& _gameMat );
        bbool   isEmptyCollision            (   u32 _index ) const;
        i32     getIdEmptyCollision         (   );
        void    createCollisionData         (   );        
        void    initCollision               (   u32 _index );
        void    buildCollision_ExtremityWithCoord( const FriseConfig* config, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun* _edgeRunCur, collisionRun& _colRun, f32 _offsetHeightCur, f32 _offsetHeightLast, u32 _startEdgeRun = U32_INVALID ) const;
        void    buildCollision_CornerSquare_StartEdgeRun( const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, SafeArray<Vec2d>& _outline, edgeFrieze* _edgeCur, u32 _idEdgeStart, f32 _offset, collisionRun& _collisionRun ) const;
        void    buildCollision_CornerSquare_StopEdgeRun( const FriseConfig* _config, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeStop, f32 _offset, collisionRun& _collisionRun ) const;
        u32     buildCollisionRunListDouble   (   ITF_VECTOR<collisionRun>& _collisionRunList, FriseConfig* _config ) const;
        void    setCollisionRunDoubleOffset (   collisionRun& _collisionRunDouble ) const;
        bbool   addPoinLastToCloseCollisionDouble( collisionRun& _colRun ) const;
        bbool   isParallelEdges             (  const  Vec2d& _posCur, const Vec2d& _posLast, const Vec2d& _posLastOld, f32 _distMax) const;
        void    setCollisionExtremityGameMaterial( collisionRun& _colRun, bbool _startExtremity);
        Vec2d   buildCollision_getCornerExtremityWithCoord( const FriseTextureConfig& _texConfig, collisionRun& _colRun, edgeFrieze* _edgeCur, edgeFrieze* _edgeLast) const;

        //  In Extremity
        void    buildFrieze_InExtremity     (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const;
        bbool   buildEdgeRunList_InExtremity(   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildVB_InExtremity         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim, f32 _extremityRatio ) const;
        void    buildCollision_InExtremity  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        f32     getExtremityLong            (   f32 _ratio ) const;

        // In Extremity Simple
        void    buildFrieze_InExtremitySimple(  ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const;

        //  In Archi
        void    buildFrieze_InArchi         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static ) const;
        bbool   buildEdgeRunList_InArchi    (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        u32     getZoneId_InArchi           (   const Vec2d& _sight ) const;
        u32     getIdStartEdge_InArchi      (   ITF_VECTOR<edgeFrieze>& _edgeList, f32 _nornMin ) const;
        void    buildVB_InArchi_Static      (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList ) const;
        void    buildCollision_InArchi      (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildCollision_InArchi_CornerSquare( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        bbool   insertNewEdge_InArchi       (   ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* config, u32 idCurEdge, u32 idLastEdge ) const;
        bbool   isEdgeRun_InArchi           ( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const;
        Frise::edgeFrieze   buildNewEdge_InArchi        (   ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* config, u32 idCurEdge, u32 idLastEdge) const;
        void    insertNewEdgeInEdgeList     (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeFrieze& _newEdge, u32 _index ) const;
        void    buildVB_Static_CornerSquareExt( FriseConfig* config, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, Vec2d* _uv, u32 _colorUp, u32 _colorDown, bbool _flipEdge, f32 _varianceForced = -1.f ) const;

        // In Fluid
        void    buildFrieze_InFluid         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList );
        bbool   buildEdgeRunList_InFluid    (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildEdgeFluidList          (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        i32     getIdStartEdge_InFluid      (   ITF_VECTOR<edgeFrieze>& _edgeList );
        bbool   isEdgeValid_InFluid         (   edgeFrieze& _edge );
        void    buildCollision_InFluid      (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildCollision_EdgeFluidList(   FriseConfig* _config, u32 _sideIndex );
        void    setEdgeFluidVertexColors    (   FriseTextureConfig& _texConfig, ITF_VECTOR<EdgeFluid>& _edgeFluidList ) const;
        void    initDataFluid               (   );
        void    clearMeshFluidData          (   );
        
        // In String
        void    buildFrieze_InString        (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList );
        void    buildCollision_InString     (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        
        // In Pipe
        void    buildFrieze_InPipe          (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static ) const;
        bbool   insertNewEdge_InPipe        (   ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* config, u32 idCurEdge, u32 idLastEdge ) const;
        u32     getZoneId_InPipe            (   const Vec2d& _sight ) const;
        u32     getIdStartEdge_InPipe       (   ITF_VECTOR<edgeFrieze>& _edgeList, f32 _nornMin ) const;
        bbool   checkEdgeIsValid_InPipe     (   const edgeFrieze& _edge, f32 _nornMin ) const;
        bbool   buildEdgeRunList_InPipe     (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildVB_InPipe_Static       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList ) const;
        void    buildVB_Static_CornerSquareExt_InPipe( const FriseConfig* config, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, Vec2d* _uv, u32 _colorUp, u32 _colorDown, bbool _flipEdge ) const;
        bbool   isEdgeRun_InPipe            (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const;
        
        void    buildCollision_InPipe       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildCollision_InPipe_CornerSquare( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );

        
        void    buildVB_Static_addVtxOnStartPosEdge( const FriseConfig* _config, const edgeFrieze& edge, MeshStatic_VertexList& _vertexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, edgeFrieze* _edgeLast = NULL ) const;
        void    buildVB_Static_addVtxOnStopPosEdge(    const edgeFrieze& edge, MeshStatic_VertexList& _vertexTempList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown ) const;
        
        void    setFillParams               (   const FriseConfig* _config, FillingParams& _params ) const;
        void    setFillUv                   (   FillingParams& _params, Vec2d& _uv, Vec2d& _pos) const;

        // In Atlas
        void    buildFrieze_InAtlas         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim, u32& _atlasIndex ) const;
        bbool   setTextureData_InAtlas      (   const FriseConfig* _config, Vec2d* _uvUp, Vec2d* _uvDown, bbool& _isUvAtlas, u32& _atlasIndex) const;
        void    setEdgeRunCoeffUv_InAtlas   (   const ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun) const;
        void    buildVB_InAtlas             (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun, Vec2d* _uvUp, Vec2d* _uvDown, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const;
        f32     setRatioFixed_InAtlas       (   const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, Vec2d* _uvUp, Vec2d* _uvDown, bbool _isUvAtlas ) const;
        void    MatchEdgesWithRatioFixed( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _longWanted) const;

        // In Overlay
        void    buildVB_InOverlay           (   const FriseConfig* _config, const FriseTextureConfig& _texConfig, const ITF_VECTOR<edgeFrieze>& _edgeList, MeshOverlay_VertexList*& _vertexTempList_Overlay, MeshOverlay_IndexList*& _indexTempList_Overlay ) const;
        void    buildFrieze_InOverlay       (   ITF_VECTOR<edgeFrieze>& _edgeList, MeshOverlay_VertexList*& _vertexTempList_Overlay, MeshOverlay_IndexList*& _indexTempList_Overlay) const;
        void    clearMeshOverlayData        (   );
        void    buildMesh_Overlay           (   MeshOverlay_VertexList* _vertexTempList, MeshOverlay_IndexList* _indexTempList, const Matrix44* _pMatrixTransfo );
        void    buildVB_Overlay_Quad        (   MeshOverlay_VertexList* _vertexTempList, MeshOverlay_IndexList* _indexTempList, Vec2d* _pos, Vec2d* _uv, u32* _color, u32 _idAnim ) const;
        void    initOverlay                 (   const VertexAnim* _anim );
        void    setMeshOverlayDataMatrix    (   const VertexAnim* _anim );

private:	// line data
		void				getStaticMeshAABB(AABB& _dest);
		void				getAnimMeshAABB(AABB& _dest);
        void				getOverlayMeshAABB(AABB& _dest);

		void				executeRecomputeLineData();
		void				postDataLoad();
		void				switchCollisionsToGlobalSpace();

        //
        void                onEvent_Djembe(Event *_event);
        void                update_Djembe(f32 _elapsed);
        void                buildFrieze_Djembe(MeshStatic_VertexList& _vertexTempList_Anim, MeshStatic_IndexList* _indexTempList_Anim);

        u32                 m_querriedRecomputeData : 1,
                            m_disablePhysic : 1,
                            m_physicRegistered : 1,
                            m_needUpdateMeshElemFromConfig : 1,
                            m_needUpdate : 1;
	    ObjBinding      	m_binding;    
        bbool               m_collisionInGlobalSpace;

        FriezePlugin        *m_friezePlugin;

        ObjectRefList       m_eventListeners;

#ifdef OPTIM_COMPUTE_LIGHT
        // Philippe Optim colors 
		u32					m_ComputeLightCounter;
#endif // OPTIM_COMPUTE_LIGHT
    };

} // namespace ITF

#endif  // _ITF_FRISE_H_
