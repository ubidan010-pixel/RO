#include "precompiled_engine.h"

#ifndef ITF_FINAL

#ifndef _ITF_PROFILERMANAGER_H_
#include "engine/profiler/ProfilerManager.h"
#endif //_ITF_PROFILERMANAGER_H_

namespace ITF
{
    ProfilerManager::ProfilerManager()
        :   m_enableMonitoring(bfalse),m_profileSerialized(0),m_enableMonitoringRuntime(bfalse),
            m_startLoadingTime(0.0f),m_lastLoadingTime(0.0f),m_startOnSceneLoading(0.0f),m_lastOnSceneLoading(0.0f),
            m_GlobalAllocCount(0),m_GlobalFreeCount(0)
    {
        profileScope::initFrequency();
        clear();
    }

    ProfilerManager::~ProfilerManager()
    {
        for (u32 index = 0;index<profileScope_Max;++index)
        {
            m_listProfilerDataType[index].clear();
        }

        m_listProfilerStatesData.clear();
    }

    void ProfilerManager::clear()
    {      
        if(!m_enableMonitoring)
            return;

        for (u32 index = 0;index<profileScope_Max;++index)
        {
            m_listProfilerDataType[index].clear();
        }

        clearArchive();

    }

    void ProfilerManager::clearArchive()
    {
        m_recordArchive.clear();
        m_recordArchive.reserve(100*1024);
        updateArchiveHeader(bfalse);
        m_profileSerialized = 0;
    }

    
    void ProfilerManager::addObject(profileScope* _ps)
    {
        if(!m_enableMonitoring)
           return;

        m_profileSerialized++;
        _ps->serializeWrite(m_recordArchive);
    }

    void ProfilerManager::setEnableUpdateObject( ObjectRef _objRef, bbool _state)
    {
        MapObjRefProfilerStates::const_iterator iter = m_listProfilerStatesData.find(_objRef);

        if (iter!=m_listProfilerStatesData.end())
        {
            ProfilerStatesData* stateData = (*iter).second;
            stateData->enableUpdate = _state;
        }
        else
        {
            ProfilerStatesData* stateData = newAlloc(mId_Map , ProfilerStatesData());
            stateData->enableUpdate = _state;
            m_listProfilerStatesData[_objRef] = stateData;
        }
    }

    void  ProfilerManager::dumpFreedAllocationLoad()
    {
#ifdef ITF_CATEGORY_MEMORY_ENABLE
        memoryStatsManager::get().dumpLoadAllocation();
#endif //ITF_CATEGORY_MEMORY_ENABLE
    }

    void  ProfilerManager::setEnableBreakObject( ObjectRef _objRef)
    {

        MapObjRefProfilerStates::const_iterator iter = m_listProfilerStatesData.find(_objRef);

        if (iter!=m_listProfilerStatesData.end())
        {
            ProfilerStatesData* stateData = (*iter).second;
            stateData->enableBreak= btrue;
        }
        else
        {
            ProfilerStatesData* stateData = newAlloc(mId_Map , ProfilerStatesData());
            stateData->enableBreak = btrue;
            m_listProfilerStatesData[_objRef] = stateData;
        }
    }

    bbool   ProfilerManager::isEnableBreakObject( ObjectRef _objRef)
    {

        MapObjRefProfilerStates::iterator iter = m_listProfilerStatesData.find(_objRef);
        if (iter!=m_listProfilerStatesData.end())
        {
            bbool res  = (*iter).second->enableBreak;
            (*iter).second->enableBreak = bfalse;
            return res;
        }

        return bfalse;
    }



    void    ProfilerManager::setEnableEventObject( ObjectRef _objRef)
    {
        
        MapObjRefProfilerStates::const_iterator iter = m_listProfilerStatesData.find(_objRef);

        if (iter!=m_listProfilerStatesData.end())
        {
            ProfilerStatesData* stateData = (*iter).second;
            stateData->enableEvent = btrue;
        }
        else
        {
            ProfilerStatesData* stateData = newAlloc(mId_Map , ProfilerStatesData());
            stateData->enableEvent = btrue;
            m_listProfilerStatesData[_objRef] = stateData;
        }

    }
        
    bbool   ProfilerManager::isEnableEventObject( ObjectRef _objRef)
    {
        MapObjRefProfilerStates::const_iterator iter = m_listProfilerStatesData.find(_objRef);
        if (iter!=m_listProfilerStatesData.end())
        {
            return (*iter).second->enableEvent;
        }

        return bfalse;
    }

    bbool ProfilerManager::isEnableUpdateObject( ObjectRef _objRef)
    {

        MapObjRefProfilerStates::const_iterator iter = m_listProfilerStatesData.find(_objRef);
        if (iter!=m_listProfilerStatesData.end())
        {
            return (*iter).second->enableUpdate;
        }

        return btrue;
    }

    void ProfilerManager::setEnableDrawObject( ObjectRef _objRef, bbool _state)
    {
        MapObjRefProfilerStates::const_iterator iter = m_listProfilerStatesData.find(_objRef);

        if (iter!=m_listProfilerStatesData.end())
        {
            ProfilerStatesData* stateData = (*iter).second;
            stateData->enableDraw = _state;
        }
        else
        {
            ProfilerStatesData* stateData = newAlloc(mId_Map , ProfilerStatesData());
            stateData->enableDraw = _state;
            m_listProfilerStatesData[_objRef] = stateData;
        }

    }

    const ProfilerStatesData* ProfilerManager::getProfilerStatesData(ObjectRef _objRef) const
    {
        MapObjRefProfilerStates::const_iterator iter = m_listProfilerStatesData.find(_objRef);
        if (iter!=m_listProfilerStatesData.end())
        {
            return (*iter).second;
        }
        return NULL;
    }

    bbool ProfilerManager::isEnableDrawObject( ObjectRef _objRef)
    {  MapObjRefProfilerStates::const_iterator iter = m_listProfilerStatesData.find(_objRef);

        if (iter!=m_listProfilerStatesData.end())
        {
            return (*iter).second->enableDraw;
        }

        return true;
    }


    void  ProfilerManager::updateArchiveHeader(bbool brewind)
    {
        if (brewind)
            m_recordArchive.seek(0);

        m_recordArchive.serialize(m_profileSerialized);
    }

    void ProfilerManager::startProfileLoading()
    {
        m_startLoadingTime = static_cast<f32>(SYSTEM_ADAPTER->getTime());


#ifdef ITF_CATEGORY_MEMORY_ENABLE
        memoryStatsManager::get().resetProfileCounter();
#endif //ITF_CATEGORY_MEMORY_ENABLE
    }

    void ProfilerManager::getInfoLoadingTime(f32& _lastLoadingTime,u32& _GlobalAllocCount,u32 &_GlobalFreeCount)
    {
        _lastLoadingTime = m_lastLoadingTime;

#ifdef ITF_CATEGORY_MEMORY_ENABLE
        _GlobalAllocCount = m_GlobalAllocCount;
        _GlobalFreeCount  = m_GlobalFreeCount;
#endif //ITF_CATEGORY_MEMORY_ENABLE
    }

    void ProfilerManager::stopProfileLoading()
    {
        m_lastLoadingTime = static_cast<f32>(SYSTEM_ADAPTER->getTime()-m_startLoadingTime);

        ITF_UNUSED(m_GlobalAllocCount);
        ITF_UNUSED(m_GlobalFreeCount);
#ifdef ITF_CATEGORY_MEMORY_ENABLE
        m_GlobalAllocCount = memoryStatsManager::get().getGlobalProfileAllocCount();
        m_GlobalFreeCount  = memoryStatsManager::get().getGlobalProfileFreeCount();
#endif //ITF_CATEGORY_MEMORY_ENABLE
    }


    void ProfilerManager::startProfileOnSceneLoading()
    {
        m_startOnSceneLoading = SYSTEM_ADAPTER->getTime();
    }

    void ProfilerManager::stopProfileOnSceneLoading()
    {
        m_lastOnSceneLoading = static_cast<f32>(SYSTEM_ADAPTER->getTime()-m_startOnSceneLoading);
    }

}

#endif //!ITF_FINAL
