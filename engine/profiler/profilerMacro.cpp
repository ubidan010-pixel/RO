#include "precompiled_engine.h"

#ifndef ITF_FINAL

#ifndef _ITF_PROFILERMACRO_H_
#include "engine\profiler\profilerMacro.h"
#endif //_ITF_PROFILERMACRO_H_

#ifndef _ITF_PROFILERMANAGER_H_
#include "engine\profiler\ProfilerManager.h"
#endif //_ITF_PROFILERMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

//-------------------------------------------------------------------------------------------------

namespace ITF
{

    f64 profileScope::m_profilerFrequency = 1.0f;

    void profileScope::initFrequency()
    {
        profileScope::m_profilerFrequency = PROFILER_GETFREQUENCY();
    }

    profileScope::profileScope(const Pickable* pPickable, ObjectRef _ref,profileScopeEnum _typeScope)
        : m_skipUpdate(0),m_skipDraw(0),m_skipThisScope(bfalse)
    {
        if (!ProfilerManager::get().isEnableMonitoringRuntime())
            return;

        m_ref = _ref.getValue();      
        m_pPickable = pPickable;
        const ProfilerStatesData* profData = ProfilerManager::get().getProfilerStatesData(_ref);
        if(profData)
        {
            m_skipDraw = !profData->enableDraw;
            m_skipUpdate = !profData->enableUpdate;
        }

        if (!ProfilerManager::get().isEnableMonitoring())
            return;

        bbool m_break = profData?profData->enableBreak:bfalse;
        m_useEvent = profData?profData->enableEvent:bfalse;
        m_typeScope = _typeScope;
        m_updateType = pPickable ? (u32)pPickable->getUpdateType(): U32_INVALID;

        if (m_break)
        {
            ITF_DEBUG_BREAK();
            ProfilerManager::get().isEnableBreakObject(_ref); // reset break event.
        }
        
        if (m_skipUpdate && _typeScope == profileScope_Update)
        {
            m_skipThisScope = btrue;
            return;
        }
        else
        if (m_skipDraw && _typeScope == profileScope_Draw)
        {
            m_skipThisScope = btrue;
            return;
        }

        //start record timing
        PROFILER_QUERYPERFORMANCECOUNTER(m_highTimerStart);

        if (m_useEvent)
        {
            PROFILER_BEGIN_EVENT(m_ref);
        }

    }

    void profileScope::serializeWrite(ArchiveMemoryWriter& _mem)
    { 
        _mem.serialize(m_typeScope);
        _mem.serialize( m_ref);
         m_timeMS = static_cast<f32>(PROFILER_CONVERT_TOMS(m_delta,m_profilerFrequency));
        _mem.serialize(m_timeMS);
        u32 _typePickable = m_pPickable ? m_pPickable->getObjectType() : BaseObject::eInvalid;
        _mem.serialize(_typePickable);
        _mem.serialize(m_updateType);
        _mem.serialize(m_skipUpdate);
        _mem.serialize(m_skipDraw);
   
        if (_typePickable == BaseObject::eActor)
        {
            Actor* act =(Actor*)m_pPickable;
            m_nbComponents = act->getUpdateComponentsCount();
            m_nbBones = act->getNbBones();

            _mem.serialize(m_nbComponents);
            _mem.serialize(m_nbBones);
        }

        if (_typePickable != BaseObject::eInvalid)
        {
            const u32 len = m_pPickable->getUserFriendly().getLen();
            _mem.serialize(len);
            _mem.serializeBlock8(m_pPickable->getUserFriendly().cStr(),len);
            _mem.padOn32();
        }

    }

    profileScope::~profileScope()
    {
        if (!ProfilerManager::get().isEnableMonitoring())
            return;

        if (!m_skipThisScope)  //stop record timing
        {
            if (m_useEvent)
            {
                PROFILER_STOP_EVENT();
            }

            //start record timing
            PROFILER_QUERYPERFORMANCECOUNTER(m_highTimerEnd);

            m_delta = PROFILER_DELTA(m_highTimerEnd,m_highTimerStart);
        }
        else
            m_delta = {};

        ProfilerManager::get().addObject(this);//m_pPickable,m_ref,delta,m_typeScope);           
        //record this to profiler
    }

#ifdef ITF_X360
        f64 getProfilerFrequencySeconds()
        {
            LARGE_INTEGER TicksPerSecond;
            QueryPerformanceFrequency( &TicksPerSecond );
            f64 fTicksPersecond = (f64)TicksPerSecond.QuadPart * 1.0f;
            return fTicksPersecond;
        }
#endif //ITF_X360

#ifdef ITF_MICROSOFT
        f64 getProfilerFrequencySeconds()
        {
            LARGE_INTEGER TicksPerSecond;
            QueryPerformanceFrequency( &TicksPerSecond );
            f64 fTicksPersecond = (f64)TicksPerSecond.QuadPart * 1.0f;
            return fTicksPersecond;
        }
#endif //ITF_WINDOWS
}

#endif //!ITF_FINAL
