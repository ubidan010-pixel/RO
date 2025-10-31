#ifndef _ITF_PROFILERMANAGER_H_
#define _ITF_PROFILERMANAGER_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#include "engine/profiler/profilerMacro.h"

#ifdef ITF_CATEGORY_MEMORY_ENABLE
#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_
#endif //ITF_CATEGORY_MEMORY_ENABLE

#ifndef ITF_FINAL
namespace ITF
{
    struct ProfilerData
    {
        ProfilerData()
            : objectRef(ITF_INVALID_OBJREF)
            , timeToUpdateReal(0.0f)
            , timeToDrawReal(0.0f)
        {}

        ObjectRef objectRef;
        f32 timeToUpdateReal;//in ms
        f32 timeToDrawReal;//in ms

        PROFILER_TYPECOUNT deltaTimeToUpdate;
        PROFILER_TYPECOUNT deltaTimeToDraw;
    };

    struct ProfilerStatesData
    {
        ProfilerStatesData()
            : enableUpdate(btrue)
            , enableDraw(btrue)
            , enableBreak(bfalse)
            , enableEvent(bfalse)
        {}

        bbool enableUpdate;
        bbool enableDraw;
        bbool enableBreak;
        bbool enableEvent;
    };

    class ProfilerManager : public TemplateSingleton<ProfilerManager>
    {
    public:

        ProfilerManager();
        virtual ~ProfilerManager();

        void                            setEnableMonitoring( bbool _enableMonitoring, bbool _enableMonitoringRuntime ) { m_enableMonitoring = _enableMonitoring; m_enableMonitoringRuntime = _enableMonitoringRuntime;}
        bbool                           isEnableMonitoring( ) { return m_enableMonitoring; }
        bbool                           isEnableMonitoringRuntime( ) { return m_enableMonitoringRuntime; }

        const SafeArray<ProfilerData>&			getListProfilerData(profileScopeEnum _type) const { return m_listProfilerDataType[_type]; }
        void                            addObject(profileScope* _ps);
        void                            clear();


        const ITF_MAP<ObjectRef, ProfilerStatesData*> getListProfilerStatesData() const { return m_listProfilerStatesData; }
        const ProfilerStatesData*       getProfilerStatesData(ObjectRef _objRef) const;
        void                            setEnableUpdateObject( ObjectRef _objRef, bbool _state);
        bbool                           isEnableUpdateObject( ObjectRef _objRef);

        void                            setEnableBreakObject( ObjectRef _objRef);
        bbool                           isEnableBreakObject( ObjectRef _objRef);


        void                            setEnableEventObject( ObjectRef _objRef);
        bbool                           isEnableEventObject( ObjectRef _objRef);

        void                            setEnableDrawObject( ObjectRef _objRef, bbool _state);
        bbool                           isEnableDrawObject( ObjectRef _objRef);

        void                            clearArchive();

        void                            updateArchiveHeader(bbool brewind);
        ArchiveMemoryWriter&            getArchiveMemory()  {return m_recordArchive;}
        u32                             getProfileSerialized() const {return m_profileSerialized;}

        void                            startProfileLoading();
        void                            stopProfileLoading();
        void                            startProfileOnSceneLoading();
        void                            stopProfileOnSceneLoading();

        void                            dumpFreedAllocationLoad();
        
        void                            getInfoLoadingTime(f32& _lastLoadingTime,u32& _GlobalAllocCount,u32& _GlobalFreeCount);
        f32                             getLastOnSceneLoadingTime() {return m_lastOnSceneLoading;}

    private:
        bbool                           m_enableMonitoring;
        bbool                           m_enableMonitoringRuntime;

       // SafeArray<ProfilerData>                         m_listProfilerData;
        SafeArray<ProfilerData>                         m_listProfilerDataType[profileScope_Max];
        typedef  ITF_MAP<ObjectRef, ProfilerStatesData*>  MapObjRefProfilerStates;
        MapObjRefProfilerStates        m_listProfilerStatesData;
        ArchiveMemoryWriter            m_recordArchive;
        u32                            m_profileSerialized;

        f32                            m_startLoadingTime;
        f32                            m_lastLoadingTime;
        f64                            m_startOnSceneLoading; 
        f32                            m_lastOnSceneLoading; 
        u32                            m_GlobalAllocCount;
        u32                            m_GlobalFreeCount;

    };

};
#endif //ITF_FINAL
#endif //_ITF_PROFILERMANAGER_H_
