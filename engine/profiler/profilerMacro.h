#ifndef _ITF_PROFILERMACRO_H_
#define _ITF_PROFILERMACRO_H_

#pragma once

#include "core/ArchiveMemoryWriter.h"

#include "core/BaseObject.h"

#ifdef ITF_FINAL
#define PROFILER_UPDATE_PICKABLE(__this,__pickableRef)  {}

#define PROFILER_DRAW_PICKABLE(__this,__pickableRef)    {}

#define PROFILER_DRAWGPU(__pickableRef)    {}

#define PROFILER_TYPECOUNT u64

namespace ITF
{
    // Empty implementation of a simple helper class.
    class SimpleProfileTimer
    {
    public:
        SimpleProfileTimer() = default;

        void restart() {}

        PROFILER_TYPECOUNT getStartTime() const { return (PROFILER_TYPECOUNT)(0); }

        f64 getDurationSinceStart() const { return 0.f; }
    };
} // namespace ITF

#else
#ifdef ITF_X360
#include "engine/profiler/profilerMacro_x360.h"
#elif defined(ITF_WINDOWS) || defined(ITF_XBOX_SERIES)
#include "engine/profiler/profilerMacro_win.h"
#elif defined(ITF_WII)
#include "engine/profiler/profilerMacro_wii.h"
#elif defined(ITF_PS3)
#include "engine/profiler/profilerMacro_ps3.h"
#elif defined(ITF_PS5)
#include "engine/profiler/profilerMacro_ps5.h"
#elif defined(ITF_NINTENDO)
#include "engine/profiler/profilerMacro_Nintendo.h"
#else //Other console must create their own macro
#include "engine/profiler/profilerMacro_default.h"
#endif //ITF_X360


#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_


//-------------------------------------------------------------------------------------------------

namespace ITF
{

    // A simple helper class to measure elapsed time in milliseconds using the profiler macros.
    class SimpleProfileTimer
    {
    public:
        SimpleProfileTimer()
        {
            PROFILER_QUERYPERFORMANCECOUNTER(m_start);
        }

        void restart()
        {
            PROFILER_QUERYPERFORMANCECOUNTER(m_start);
        }

        PROFILER_TYPECOUNT getStartTime() const
        {
            return m_start;
        }

        f64 getDurationSinceStart() const // elapsed time in seconds since construction or last restart
        {
            PROFILER_TYPECOUNT current = {};
            PROFILER_QUERYPERFORMANCECOUNTER(current);
            
            return PROFILER_CONVERT_TOMS(PROFILER_DELTA(current, m_start), PROFILER_GETFREQUENCY()) / 1000.;
        }

    private:
        PROFILER_TYPECOUNT m_start = {};
    };

    class Pickable;

    enum profileScopeEnum
    {
        profileScope_Update     = 0,
        profileScope_Draw       = 1,
        profileScope_DrawGPU    = 2,
        profileScope_Max        = 3,

        ENUM_FORCE_SIZE_32(0)
    };

    struct profileScope
    {
        profileScope():m_skipUpdate(0),m_skipDraw(0),m_skipThisScope(bfalse) {};

        static void initFrequency();
        static f64 m_profilerFrequency;

        profileScope(const Pickable* pPickable,ObjectRef _ref,profileScopeEnum _typeScope);

        ITF_INLINE bbool skipDraw() const
        {
            return m_skipDraw?btrue:bfalse;
        }

        ITF_INLINE bbool skipUpdate() const
        {
            return m_skipUpdate?btrue:bfalse;
        }


        void serializeWrite(ArchiveMemoryWriter& _mem);
    

        ITF_INLINE void serializeRead(ArchiveMemory& _mem,String8& _friendlyName,String& _filename,f32& timeMS)
        {
            _mem.serialize((u32&) m_typeScope);
            _mem.serialize((u32&) m_ref);
            _mem.serialize((f32&) timeMS);
            _mem.serialize((u32&) m_typePickable);
            _mem.serialize((u32&) m_updateType);
            _mem.serialize((u32&) m_skipUpdate);
            _mem.serialize((u32&) m_skipDraw);

            if (m_typePickable == BaseObject::eActor)
            {
                _mem.serialize((u32&) m_nbComponents);
                _mem.serialize((u32&) m_nbBones);
            }
            else
            {
                m_nbComponents  = 0;
                m_nbBones       = 0;
            }
       

            if (m_typePickable != BaseObject::eInvalid)
            {
                _friendlyName.serialize(_mem);
                _mem.padOn32();
            }
        }

       ~profileScope();
   
        const Pickable* m_pPickable;
        profileScopeEnum m_typeScope;
        u32     m_ref;
        u32     m_skipUpdate;
        u32     m_skipDraw;
        bbool   m_skipThisScope; 
        bbool   m_useEvent;
        u32     m_typePickable;
        u32     m_nbComponents;
        u32     m_nbBones;
        f32     m_timeMS;
        u32     m_updateType;
        PROFILER_TYPECOUNT  m_highTimerStart;
        PROFILER_TYPECOUNT  m_highTimerEnd;
        PROFILER_TYPECOUNT  m_delta;
    };


}

#define PROFILER_UPDATE_PICKABLE(__this,__pickableRef)  profileScope pS(__this,__pickableRef,profileScope_Update);if(pS.skipUpdate()) return;

#define PROFILER_DRAW_PICKABLE(__this,__pickableRef)    profileScope pS(__this,__pickableRef,profileScope_Draw);if(pS.skipDraw()) return;

#define PROFILER_DRAWGPU(__pickableRef)    profileScope pS(NULL,__pickableRef,profileScope_DrawGPU);

#endif //ITF_FINAL

namespace ITF
{
    // Time in a scope filling a variable
    class SimpleProfileTimerScope
    {
    public:
        explicit SimpleProfileTimerScope(f64& _timeInSeconds)
            : m_timeInSeconds(_timeInSeconds)
        {
        }

        ~SimpleProfileTimerScope()
        {
            m_timeInSeconds = f64(getDurationSinceStart());
        }

        auto getStartTime() const
        {
            return m_timer.getStartTime();
        }

        f64 getDurationSinceStart() const
        {
            return m_timer.getDurationSinceStart();
        }

    protected:
        SimpleProfileTimer m_timer;
        f64& m_timeInSeconds;
    };
}

#endif //_ITF_PROFILERMACRO_H_
