#ifndef _ITF_ANIMATIONSTATS_H_
#define _ITF_ANIMATIONSTATS_H_

#ifndef ITF_FINAL
//#define ENABLE_ANIMATIONSTATS // TODO When all is OK (not a priority)
#endif


namespace ITF
{
#ifdef ENABLE_ANIMATIONSTATS
    class AnimMeshScene;
    class AnimationObject;

    class AnimationStats
    {
    public:
        typedef struct AnimSubSceneData
        {
            u32                 m_mainAnimSubSceneMemory;
            u32                 m_mainAnimSubSceneFullMemory;
            ITF_VECTOR<u32>     m_bonesStatics;
            u32                 m_bonesStaticsMemory;
            ITF_VECTOR<u32>     m_bonesDynamic;
            u32                 m_bonesDynamicMemory;
            ITF_VECTOR<u32>     m_bonesTDynamic;
            u32                 m_bonesTDynamicMemory;
            ITF_VECTOR<u32>     m_patches;
            u32                 m_patchesMemory;
            ITF_VECTOR<u32>     m_patchesPoints;
            u32                 m_patchesPointsMemory;
        } AnimSubSceneData;

        typedef struct TrackData
        {
            u32                 m_mainTrackMemory;
            u32                 m_mainTrackFullMemory;
            ITF_VECTOR<u32>     m_bml;
            u32                 m_bmlMemory;

            ITF_VECTOR<u32>     m_bones;
            u32                 m_bonesMemory;

            ITF_VECTOR<u32>     m_events;
            u32                 m_eventsMemory;
        } TrackData;


        typedef struct AnimantionData
        {
            u32                             m_mainAnimSceneMemory;
            u32                             m_mainAnimSceneFullMemory;
            ITF_VECTOR<TrackData>           m_tracks;
            u32                             m_tracksMemory;
        } AnimantionData;

        typedef struct AnimMeshData
        {
            u32                             m_mainAnimMeshMemory;
            u32                             m_mainAnimMeshFullMemory;
            ITF_VECTOR<u32>                 m_meshes;
        } AnimMeshData;

        static AnimationStats* m_singleton;

        static void createSingleton();
        static void deleteSingleton();
        void     addAnimation(AnimationObject * _as);
        void     removeAnimation(AnimationObject * _as);
        void     computeAnimation(AnimationObject * _as);
        void     addAnimMesh(AnimMeshScene *_an);
        void     removeAnimMesh(AnimMeshScene *_an);
        void     computeAnimMesh(AnimMeshScene *_an);
        void     dumpToFile();

    private:
        u32                                     m_animationMemory;        
        SafeArray<AnimationObject*>             m_animationList;

        u32                                     m_animMeshMemory;        
        SafeArray<AnimMeshScene *>              m_animMeshList;

        ITF_MAP<AnimScene *, AnimantionData>    m_animationDataList;
        ITF_MAP<AnimMeshScene *, AnimMeshData>  m_animMeshDataList;
    };

#define ANIMATIONSTAT_INIT                      AnimationStats::createSingleton()
#define ANIMATIONSTAT_STOP                      AnimationStats::deleteSingleton()
#define ANIMATIONSTAT_ADD_ANIMATION(ptr)        AnimationStats::m_singleton->addAnimation(ptr)
#define ANIMATIONSTAT_DEL_ANIMATION(ptr)        AnimationStats::m_singleton->removeAnimation(ptr)
#define ANIMATIONSTAT_ADD_ANIMINSTANCE(ptr)     AnimationStats::m_singleton->addAnimMesh(ptr)
#define ANIMATIONSTAT_DEL_ANIMINSTANCE(ptr)     AnimationStats::m_singleton->removeAnimMesh(ptr)
#define ANIMATIONSTAT_DUMP                      AnimationStats::m_singleton->dumpToFile();


#else // ENABLE_ANIMATIONSTATS


#define ANIMATIONSTAT_INIT                      ((void)0)
#define ANIMATIONSTAT_STOP                      ((void)0)
#define ANIMATIONSTAT_ADD_ANIMATION(ref)        ((void)0)
#define ANIMATIONSTAT_DEL_ANIMATION(ref)        ((void)0)
#define ANIMATIONSTAT_ADD_ANIMINSTANCE(ref)     ((void)0)
#define ANIMATIONSTAT_DEL_ANIMINSTANCE(ref)     ((void)0)
#define ANIMATIONSTAT_DUMP                      ((void)0)


#endif //ENABLE_ANIMATIONSTATS

#endif  // _ITF_ANIMATIONSTATS_H_
} // namespace ITF
