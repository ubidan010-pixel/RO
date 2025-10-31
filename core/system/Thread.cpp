#include "precompiled_core.h"

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

namespace ITF
{
    ThreadSettings   ThreadSettings::m_settings[eThreadId_count] = {

     ThreadSettings(                // 
        "MainThread",               // name
        Thread_HardwareThread0,     // HardwareThreadId
        Thread_Priority_Normal,     // Thread_Priority
        16384,                      // m_maxObjectRef
        16,							// m_maxString
        16,							// m_maxString8
        65536,                      // m_stackSize
        eThreadId_mainThread
        ),

     ThreadSettings(                // eThreadId_soundStreamer
        "MusicThread",              // name
        Thread_HardwareThread3,     // HardwareThreadId
        Thread_Priority_Highest,     // Thread_Priority
        16,                       // m_maxObjectRef
        4,                         //m_maxString
        4,                         //m_maxString8
        65536,                       //m_stackSize
        eThreadId_soundStreamer
        ),

    ThreadSettings(                 // eThreadId_SCAdapter
        "SCAdapter",                // name
        Thread_HardwareThread0,     // HardwareThreadId
        Thread_Priority_Normal,     // Thread_Priority
        0,                          // m_maxObjectRef
        4,                         //m_maxString
        4,                         //m_maxString8
        65536,                      //m_stackSize
        eThreadId_SCAdapter
        ),

    ThreadSettings(                 //eThreadId_TCPReceivePackage
        "TCPReceive_Package",       // name
        Thread_HardwareThread4,     // HardwareThreadId
        Thread_Priority_Normal,     // Thread_Priority
        0,                         // m_maxObjectRef
        0,                         //m_maxString
        0,                         //m_maxString8
        65536,                       //m_stackSize
        eThreadId_TCPReceivePackage
        ),

    ThreadSettings(                 // eThreadId_FileWatcher
        "FileWatcher",              // name
        Thread_HardwareThread0,     // HardwareThreadId
        Thread_Priority_Normal,     // Thread_Priority
        0,                          // m_maxObjectRef
        4,                         //m_maxString
        4,                         //m_maxString8
        65536,                      //m_stackSize
        eThreadId_FileWatcher
        ),

    ThreadSettings(                 // eThreadId_ResourceManager
        "ResourceManager_Run",      // name
        Thread_HardwareThread2,     // HardwareThreadId
#ifndef ITF_WII
        Thread_Priority_AboveNormal,     // Thread_Priority
#else
        Thread_Priority_Normal,
#endif
        1024,                       // m_maxObjectRef
        4,                         //m_maxString
        4,                         //m_maxString8
        65536,                      //m_stackSize
        eThreadId_ResourceManager
        ),

    ThreadSettings (                // eThreadId_SceneManager
        "runAsyncScene",            // name
        Thread_HardwareThread2,     // HardwareThreadId
#ifndef ITF_WII
        Thread_Priority_AboveNormal,     // Thread_Priority
#else
        Thread_Priority_Normal,
#endif
        8192,                       // m_maxObjectRef
        8,							//m_maxString
        8,							//m_maxString8
        65536,                      //m_stackSize
        eThreadId_SceneManager
        ),

    ThreadSettings(                 // eThreadId_VideoCapture
        "VideoCapture1",            // name
        Thread_HardwareThread1,     // HardwareThreadId
        Thread_Priority_Normal,     // Thread_Priority
        0,                          // m_maxObjectRef
        4,                         //m_maxString
        4,                         //m_maxString8
        65536,                      //m_stackSize
        eThreadId_VideoCapture1
        ),

    ThreadSettings(                 // eThreadId_VideoCapture
        "VideoCapture2",            // name
        Thread_HardwareThread2,     // HardwareThreadId
        Thread_Priority_Normal,     // Thread_Priority
        0,                          // m_maxObjectRef
        4,                         //m_maxString
        4,                         //m_maxString8
        65536,                      //m_stackSize
        eThreadId_VideoCapture2
        ),

    ThreadSettings(                 // eThreadId_VideoCapture
        "VideoCapture3",            // name
        Thread_HardwareThread3,     // HardwareThreadId
        Thread_Priority_Normal,     // Thread_Priority
        0,                          // m_maxObjectRef
        4,                         //m_maxString
        4,                         //m_maxString8
        65536,                      //m_stackSize
        eThreadId_VideoCapture3
        ),

    ThreadSettings(                 // eThreadId_VideoCapture
        "VideoCapture4",            // name
        Thread_HardwareThread4,     // HardwareThreadId
        Thread_Priority_Normal,     // Thread_Priority
        0,                          // m_maxObjectRef
        4,                         //m_maxString
        4,                         //m_maxString8
        65536,                      //m_stackSize
        eThreadId_VideoCapture4
        ),

     ThreadSettings(                 // eThreadId_CookBackground
        "cookBackGround",            // name
        Thread_HardwareThread1,     // HardwareThreadId
        Thread_Priority_Highest,     // Thread_Priority
        8192,                       // m_maxObjectRef
        4,                        // m_maxString
        4,                        // m_maxString8
        65536,                      // m_stackSize
        eThreadId_CookBackGround
        ),

        ThreadSettings(             // eThreadId_TaskManager
        "taskManager",              // name
        Thread_HardwareThread3,     // HardwareThreadId
        Thread_Priority_Normal,    // Thread_Priority
        16,                       // m_maxObjectRef
        4,                         // m_maxString
        4,                         // m_maxString8
        32*1024,                      // m_stackSize
        eThreadId_TaskManager
        ),

        ThreadSettings(             // eThreadId_SaveManager
        "saveManager",              // name
        Thread_HardwareThread3,     // HardwareThreadId
        Thread_Priority_Normal,    // Thread_Priority
        16,                       // m_maxObjectRef
        4,                         // m_maxString
        4,                         // m_maxString8
        65536,                      // m_stackSize
        eThreadId_SaveManager
        ),

        ThreadSettings(             // eThreadId_TRCManager
        "TRCManager",               // name
        Thread_HardwareThread3,     // HardwareThreadId
        Thread_Priority_Highest,    // Thread_Priority
        0,                       // m_maxObjectRef
        4,                         // m_maxString
        4,                         // m_maxString8
        65536,                      // m_stackSize
        eThreadId_TRCManager
        ),        

        ThreadSettings(                // 
        "LoadingSequence",               // name
        Thread_HardwareThread2,     // HardwareThreadId
        Thread_Priority_AboveNormal,     // Thread_Priority
        16384,                      // m_maxObjectRef
        16,							// m_maxString
        16,							// m_maxString8
        65536,                      // m_stackSize
        eThreadId_loadingSequence
        )
    };


    void    ThreadSettings::threadStartFrame()
    {
        IdServer::getptr()->updateThreadsData();
        String::updateThreadsData();
        String8::updateThreadsData();
/*
        #ifdef  ITF_DEBUG_STRINGS 
        int index = ThreadInfo::m_threadEngineIndex.getValue();
        ThreadSettings& curSetting = m_settings[index];
        curSetting.m_testStringFrame &= 3;

        switch (curSetting.m_testStringFrame)
        {
        case 0:
            {
            int count = (rand() % 10000) / 50 + 256;
            if (count >= m_testStringsCount)
                count = m_testStringsCount-1;
            i32 i = 0;
            for (; i < count; i++)
            {
                curSetting.m_testStrings[i] = new String("Stress");
                curSetting.m_testStrings[i]->setTextFormat("Stress %d %d", index, i);
            }
            for (; i < m_testStringsCount; i++)
            {
                curSetting.m_testStrings[i] = NULL;
            }
            }
            break;
        case 1:
            for (u32 i = 0; i < m_testStringsCount; i++)
            {
                if (curSetting.m_testStrings[i])
                {
                    String Compare;
                    Compare.setTextFormat("Stress %d %d", index, i);
                    ITF_ASSERT(Compare == *curSetting.m_testStrings[i]);
                    delete curSetting.m_testStrings[i];
                    curSetting.m_testStrings[i] = NULL;
                }
            }
            break;
        case 2:
            u32 count = (rand() % 10000) / 50 + 100;
            if (count >= m_testStringsCount)
                count = m_testStringsCount-1;
            for (u32 i = 0; i < count; i++)
            {
                curSetting.m_testStrings[i] = new String("Stress");
                curSetting.m_testStrings[i]->setTextFormat("Stress %d %d", index, i);
            }
            for (u32 i = 0; i < count; i++)
            {
                String Compare;
                Compare.setTextFormat("Stress %d %d", index, i);
                ITF_ASSERT(Compare == *curSetting.m_testStrings[i]);
                delete curSetting.m_testStrings[i];
                curSetting.m_testStrings[i] = NULL;
            }
            break;
        }
        curSetting.m_testStringFrame++;

        #ifdef ITF_WINDOWS
            if (index  == eThreadId_ResourceManager)
            {
                // random slowdown
                int amount = rand() & 127;
                sleep(20 + amount);
            }
        #endif
    #endif  // ITF_DEBUG_STRINGS
*/

    }

    const char* ThreadSettings::getCurrentThreadName()
    {
        u32 id = Synchronize::getCurrentThreadId();

        for ( u32 i = 0; i < eThreadId_count; ++i )
            if ( m_settings[i].m_threadID == id )
                return m_settings[i].m_name;
        return NULL;
    }

    eThreadId ThreadSettings::getCurrentThreadId()
    {
        u32 id = Synchronize::getCurrentThreadId();

        for ( u32 i = 0; i < eThreadId_count; ++i )
            if ( m_settings[i].m_threadID == id )
                return static_cast< eThreadId >( i );
        return eThreadId_count;
    }
}
