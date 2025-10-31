#ifndef _ITF_THREAD_H_
#define _ITF_THREAD_H_

#pragma once

#include "core/system/Synchronize.h"

namespace ITF
{

enum Thread_Priority
{
    Thread_Priority_Idle,
    Thread_Priority_Lowest,
    Thread_Priority_BelowNormal,
    Thread_Priority_Normal,
    Thread_Priority_AboveNormal,
    Thread_Priority_Highest,
    Thread_Priority_TimeCritical
};


enum Thread_HardwareThreadId : u32
{
    Thread_HardwareThread0,
    Thread_HardwareThread1,
    Thread_HardwareThread2,
    Thread_HardwareThread3,
    Thread_HardwareThread4,
    Thread_HardwareThread5,
    Thread_HardwareThread6,
    Thread_HardwareThread7,
    Thread_HardwareThread8,
    Thread_HardwareThread9,
    Thread_HardwareThread10,
    Thread_HardwareThread11,
    Thread_HardwareThread12,
    Thread_HardwareThread_Unknown = 0xFFFFFFFF
};

enum eThreadId
{
    eThreadId_mainThread = 0,       // MUST remain the first one with Id 0
    eThreadId_soundStreamer,
    eThreadId_SCAdapter,
    eThreadId_TCPReceivePackage,
    eThreadId_FileWatcher,
    eThreadId_ResourceManager,
    eThreadId_SceneManager,

    eThreadId_VideoCapture1,        // WARNING! Video Capture threads MUST remain contiguous
    eThreadId_VideoCapture2,
    eThreadId_VideoCapture3,
    eThreadId_VideoCapture4,

    eThreadId_CookBackGround,
    eThreadId_TaskManager,
    eThreadId_SaveManager,
    eThreadId_TRCManager,

    eThreadId_loadingSequence,
    
    eThreadId_count                 // MUST remain the last one
};

class ThreadSettings
{
public:

    ThreadSettings()
    {
        m_name          = NULL;
        m_hwId          = Thread_HardwareThread0;
        m_priority      = Thread_Priority_Normal;
        m_maxObjectRef  = 0;
        m_maxString     = 0;
        m_maxString8    = 0;
        m_stackSize     = 64*1024;
        m_engineIndex   = U32_INVALID;
        m_useTLS        = bfalse;
        m_threadID      = U32_INVALID;
    };

    ThreadSettings(const char* _name, Thread_HardwareThreadId _hwId, Thread_Priority _priority, u32 _maxObjectRef, u32 _maxString, u32 _maxString8, u32 _stackSize, eThreadId _engineIndex, bbool _useTLS = btrue )
    {
        m_name = _name;
        m_hwId = _hwId;
        m_priority = _priority;
        m_maxObjectRef = _maxObjectRef;
        m_maxString = _maxString;
        m_maxString8 = _maxString8;
        m_stackSize = _stackSize;
        m_engineIndex = _engineIndex;
        m_useTLS = _useTLS;
        m_threadID = U32_INVALID;
#ifdef ASSERT_ENABLED
        if (!m_useTLS)
        {
            if (m_maxString + m_maxString8 + m_maxObjectRef > 0)
            {
                ITF_DEBUG_BREAK();
            }
        }
#endif
    #ifdef  ITF_DEBUG_STRINGS        
        m_testStringFrame = 0;
    #endif

    }

    static  void            threadStartFrame();

    const char*             m_name;
    Thread_HardwareThreadId m_hwId;
    Thread_Priority         m_priority;
    u32                     m_maxObjectRef;
    u32                     m_maxString;
    u32                     m_maxString8;
    u32                     m_stackSize;
    u32                     m_engineIndex;
    ITF_THREAD_ID           m_threadID;
    bbool                   m_useTLS;
    #ifdef  ITF_DEBUG_STRINGS 
        static const int    m_testStringsCount = 1024;
        class String*       m_testStrings[m_testStringsCount];
        int                 m_testStringFrame;
    #endif

    static const char*  getCurrentThreadName();
    static eThreadId    getCurrentThreadId();

    static ThreadSettings   m_settings[eThreadId_count];
};

}

#if defined (ITF_MICROSOFT)
#include "Microsoft/Thread_microsoft.h"
#elif defined (ITF_PS3)
#include "PS3/Thread_ps3.h"
#elif defined (ITF_X360)
#include "X360/Thread_x360.h"
#elif defined (ITF_IPAD)
#include "iPad/Thread_iPad.h"
#elif defined (ITF_CTR)
#include "CTR/Thread_CTR.h"
#elif defined (ITF_WII)
#include "WII/Thread_WII.h"
#elif defined (ITF_VITA)
#include "VITA/Thread_VITA.h"
#elif defined (ITF_PS5)
#include "PS5/Thread_PS5.h"
#elif defined (ITF_NINTENDO)
#include "Nintendo/Thread_Nintendo.h"
#else
#error Platform not supported
#endif //ITF_WINDOWS


namespace ITF
{
    class ThreadInfo
    {
    public:

        static ThreadLocalStorage   m_threadEngineIndex;
    };
}


#endif // _ITF_THREAD_H_
