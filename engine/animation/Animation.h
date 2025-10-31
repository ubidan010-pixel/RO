#ifndef _ITF_ANIMATION_H_
#define _ITF_ANIMATION_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_SAFEARRAY_H_
#include "core/containter/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_ANIMATIONDRAW_H_
#include    "engine/animation/AnimationDraw.h"
#endif // _ITF_ANIMATIONDRAW_H_

#ifndef _ITF_ANIMATIONPATCHES_H_
#include    "engine/animation/AnimationPatches.h"
#endif // _ITF_ANIMATIONPATCHES_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_


#define ANIMATION_FPS 60.f
#define ANIMATION_FRAMERATE (1.f/ANIMATION_FPS)

namespace ITF
{    
    class ResourceGroup;
    class ITF_Mesh;
    class AnimTrack;
    class AnimTrackBML;
    class AnimTrackPolyline;
    class AnimMarkerEvent;
    class ITF_IndexBuffer;
    class AnimTrackExt;
    class AnimSkeleton;
    class AnimTemplate;
    class AnimPatchPointDyn;

    class TrackSet
    {
    public:
        NamesObjectRefMap           m_nameToTrack;

        void                        setParentResourceGroup(ResourceGroup* _parent) ;
        void                        addTrack(const StringID &_name, const String &_path, const Path & _dependencyFile);
        
        bbool                       getTrackResource(const StringID &_name, ResourceID &_res, bbool _useDefault = bfalse) const;
        AnimTrack *                 getTrack(const StringID &_name, bbool _useDefault = bfalse) const;
        void                        destroy();
        const AABB &                getGlobalBoundingBox() const { return m_globalBoundingBox; }

        TrackSet();
        ~TrackSet();

    protected:
        ResourceID                  m_resourceGroup;
        ResourceID                  m_parentResourceGroup;
        static  i32                 s_trackCount;
        AABB                        m_globalBoundingBox;
    };

#ifndef ITF_SUPPORT_MULTITHREADED
    #define CUR_FRAME_DATA_INDEX  0
    #define PREV_FRAME_DATA_INDEX 1
#else
    #define PREV_FRAME_DATA_INDEX 2
#endif

	/////////////////////////////////////////////////////////////////////////////////////
	///Instance information for each character  (position, bone position/orientation, current lighting...)
    struct AnimFrameInfo
    {
        typedef SafeArray<char> BonesArray;

        const AnimTrackExt* m_pTrackExt;
        f32                 m_fFrame;
        f32                 m_percent;
        bbool               m_useVisuals;
        BonesArray          m_bonesUsed;

        AnimFrameInfo() : m_pTrackExt(NULL)
                        , m_fFrame(0.f)
                        , m_percent(1.f)
                        , m_useVisuals(btrue) {}
    };
    VECTAGSET(AnimFrameInfo, ITF::MemoryId::mId_VectorAnimation)


#define SAFEARRAY_CACHE_TRCKBONE_PASCUT SafeArray<AnimTrackBonePASCut,8,MemoryId::mId_SafeArrayCache>
    struct AnimFrameCache
    {
        const AnimTrackExt* m_pTrackExt;
		SAFEARRAY_CACHE_TRCKBONE_PASCUT m_cashTab;
    };

    struct AnimBMLInfo
    {
        AnimTrackBML*   m_bml;
        SafeArray<char> m_templateUsed;

        AnimBMLInfo() : m_bml(NULL) {}
        bool operator==(const AnimBMLInfo& bmlIfo) const
        {
            return m_bml == bmlIfo.m_bml && m_templateUsed == bmlIfo.m_templateUsed;
        }
    };

    struct AnimFrameData
    {
        SafeArray<AnimBoneDyn>          m_bonesList;
#ifndef NO_RUNTIME_LOCK
        ITF_VECTOR<AnimLock>            m_locked;
#endif
        ITF_VECTOR<AnimBMLInfo>         m_bmlInfo;
        SafeArray<const AnimTrackExt*>  m_trackExtList;
        ITF_VECTOR<StringID>            m_polyRefList;
        SafeArray<Vec2d>                m_polylineDyn;

        void clear() {
            m_bonesList.clear();
#ifndef NO_RUNTIME_LOCK
            m_locked.clear();
#endif
            m_bmlInfo.clear();
            m_trackExtList.clear();
            m_polyRefList.clear();
            m_polylineDyn.clear();
        }
    };

    struct IndexSize
    {
        u32     m_index;
        u32     m_size;
    };

    class AnimInfo
    {
    public:
        AnimInfo();
        ~AnimInfo();

        Vec3d                       m_Pos;
        f32                         m_Angle;
        Vec2d                       m_Scale;
        AABB                        m_scissor;
#ifndef ANIMATION_FIX_ABB
        AABB                        m_patchAABB;
#endif
        AABB                        m_visualAABB;
        u32                         m_renderPassFrontierPatchIndex;
        bbool                       m_renderPass;

        bbool                       m_useWorking;
        bbool                       m_useRootRotation;
        bbool                       m_IsLookingRight;
        bbool                       m_canonizeTransitions;

        AnimGlobalData              m_globalData;
        Color                       m_color;
        Color                       m_selfIllumColor;
        Color                       m_fogColor;

        ITF_VECTOR<AnimFrameInfo>   m_frameInfo;
        ITF_VECTOR<AnimFrameCache>  m_frameCache;
        u32                         m_hDiv;
        u32                         m_vDiv;

        u32                         m_state;
        int                         m_vertexFormat;
        bbool                       m_forceBMLRecompute;

#ifndef ITF_SUPPORT_MULTITHREADED
        static const u32            FRAMEDATA_NB = 2;
#else
        static const u32            FRAMEDATA_NB = 3;
        u32                         m_currFrameDataIndex; // vary between 0 and 1 -> opposite to working
#endif
        AnimFrameData               m_frameDataList[FRAMEDATA_NB];
        f32                         m_prevPercent;
        bbool                       m_minimalBonesUpdate;

        SafeArray<AnimBoneDyn>          m_tmpTemplateBonesList;
#ifdef ANIMUSEGEOMETRYSHADER
        SafeArray<AnimBoneRenderable>   m_finalBoneList;
#endif
        u32                             m_patchPointDynAlloc;
        SafeArray<AnimPatchPointDyn>    m_patchPointDyn;
        u32                             m_templateBufferFreeIndex;
        KeyArray<IndexSize, uPtr>             m_templateToPatchPointDyn; // with void * as id

        u32                             m_polylineDynAlloc;
        KeyArray<u32>                   m_polylineToPolylineDyn;

        SafeArray<LightInfo,2>          m_lights;

        AnimSkeleton *                  getFrameSkeleton(i32 _index = 0) const;
        AnimTrack *                     getFrameTrackReal(i32 _index = 0) const;
        const AnimTrackExt *            getFrameTrackExt(i32 _index = 0) const;
        f32                             getFrame(i32 _index = 0) const { return m_frameInfo[_index].m_fFrame; }


        void                            clearTemplatesBuffer();
        AnimPatchPointDyn *             getTemplatePatchPointBuffer(AnimTemplate* _template, bbool _allowAddOnBuffer);

#ifndef ITF_SUPPORT_MULTITHREADED
        AnimFrameData *                 getCurrentFrameData()  { return &m_frameDataList[CUR_FRAME_DATA_INDEX]; }
        AnimFrameData *                 getWorkingFrameData()  { return &m_frameDataList[CUR_FRAME_DATA_INDEX]; }
#else
        AnimFrameData *                 getCurrentFrameData()  { return &m_frameDataList[m_currFrameDataIndex]; }
        AnimFrameData *                 getWorkingFrameData()  { return m_useWorking ? &m_frameDataList[1-m_currFrameDataIndex] : getCurrentFrameData(); }
#endif
        AnimFrameData *                 getPreviousFrameData() { return &m_frameDataList[PREV_FRAME_DATA_INDEX]; }

        AnimSkeleton *                  getCurrentSkeleton(i32 _index = 0);
        AnimSkeleton *                  getWorkingSkeleton(i32 _index = 0);

        AnimTrack *                     getCurrentTrackReal(i32 _index = 0);
        AnimTrack *                     getWorkingTrackReal(i32 _index = 0);

       const AnimTrackExt *             getCurrentTrackExt(i32 _index = 0) { return getCurrentFrameData()->m_trackExtList[_index]; }
       const AnimTrackExt *             getWorkingTrackExt(i32 _index = 0) { return getWorkingFrameData()->m_trackExtList[_index]; }



        SafeArray<AnimBoneDyn> &        getCurrentBoneList() { return getCurrentFrameData()->m_bonesList; }
        SafeArray<AnimBoneDyn> &        getWorkingBoneList() { return getWorkingFrameData()->m_bonesList; }

        ITF_VECTOR<StringID>&           getCurrentPolyList() { return getCurrentFrameData()->m_polyRefList; }
        ITF_VECTOR<StringID>&           getWorkingPolyList() { return getWorkingFrameData()->m_polyRefList; }

        void                            copyWorkingToCurrent();

        void                            setPrev(f32 percent);
        bbool                           ComputeBonesFromLocalToWorld(bbool _forceCompute);
        void                            ComputePolylinePoints();
        void                            SetFrame();
        void                            clearPrev();
        Vec2d                           ComputePointFromLocalToWorld(const Vec2d &_point, AnimTrack* _baseTrack) const;

#ifndef ITF_FINAL
        bbool                           verifyJob();
#endif //!ITF_FINAL

    private:
        friend class AnimLightComponent;
        friend class AnimTrackPolyline;

        Vec2d *                         getPolylinePointBuffer(const StringID & polyId, bbool forReading);
        void                            updateCurFrameData();
    };

	////////////////////////////////////////////////////////////////////////////////
	///Instance of a AnimScene for drawing (meshes...) . includes AnimInfo
    class AnimMeshScene
    {
    public:
        AnimMeshScene();
        ~AnimMeshScene();

        SafeArray<ITF_Mesh*, MAX_MESHES>    m_pMesh;
        int                                 m_vertexFormat;

        AnimDraw                            m_pAnimDraw;
        AnimPatches                         m_pAnimPatches;
        AnimInfo                            m_AnimInfo;

        static AnimMeshScene *  AllocNewMesh(int VtxFormat, ITF_IndexBuffer *_staticIndex, u32 _bufferSize = 0);
        
        bbool                   AllocAddMesh(); //Creates graphic meshes (for each texture...)
        bbool                   AllocAddMesh(ITF_IndexBuffer *_staticIndex, u32 _bufferSize = 0);

#ifndef ITF_FINAL
        ObjectRef                           m_owner;
#endif //!ITF_FINAL
    };

} // namespace ITF



#endif //_ITF_ANIMATION_H_

