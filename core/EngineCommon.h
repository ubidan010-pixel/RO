#ifndef _ENGINECOMMON_H_
#define _ENGINECOMMON_H_

#ifdef _DEBUG
#define ITF_UNITTEST    1
#else //_DEBUG
#define ITF_UNITTEST    0
#endif //_DEBUG


#ifndef ITF_CONSOLE_FINAL
#define ITF_DEBUG_LEVEL 3               // 0 : no debug information
                                    // 1 : debug information + low framerate footprint
                                    // 2 : debug information + mid framerate footprint
                                    // 3 : debug information + high framerate footprint
#else
#define ITF_DEBUG_LEVEL 0  
#endif // ITF_FINAL

#if !defined(ITF_FINAL) && (defined(ITF_WINDOWS) || (ITF_DEBUG_LEVEL > 0))
#define ENABLE_FILE_REGISTERING
#endif // !defined(ITF_FINAL) && (ITF_DEBUG_LEVEL > 0)

// Std C includes
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "core/developer_prefs.h"

#if defined (ITF_CTR)
    #include "CTRSpecificConstants.h"
#elif defined (ITF_VITA)
    #include "VITASpecificConstants.h"
#elif defined (ITF_WII)
    #include "WiiSpecificConstants.h"
#elif defined (ITF_NINTENDO)
    #include "DefaultSpecificConstants.h"
    // DrawPrimitive needs to handle dynamic VB
    #define VBMANAGER_USE_DYNAMICRING_VB
#else
    #include "DefaultSpecificConstants.h"
#endif

#ifndef DEVELOPER_FRED
    #define LOG_FRED(_format, ...) ((void)0)
#endif // DEVELOPER_FRED

#ifndef DEVELOPER_JAY
    #define LOG_JAY(_format, ...) ((void)0)
#endif // DEVELOPER_JAY

#ifndef DEVELOPER_NEIL
    #define LOG_NEIL(_format, ...) ((void)0)
#endif // DEVELOPER_NEIL

#ifndef DEVELOPER_NEIL
#define LOG_NEIL(_format, ...) ((void)0)
#endif // DEVELOPER_NEIL

#ifndef DEVELOPER_YOUS
#define LOG_YOUS(_format, ...) ((void)0)
#endif // DEVELOPER_YOUS

#ifndef DEVELOPER_OLIV
    #define LOG_OLIV(_format, ...) ((void)0)
#endif // DEVELOPER_OLIV

#ifndef DEVELOPER_SMATHIEU
    #define LOG_SMATHIEU(_format, ...) ((void)0)
#endif

#ifndef DEVELOPER_BEN
#define LOG_BEN(_format, ...) ((void)0)
#endif // DEVELOPER_BEN

#ifndef DEVELOPER_GUILL_THE_WHITE
#define LOG_GUILL_THE_WHITE(_format, ...) ((void)0)
#endif // DEVELOPER_GUILL_THE_WHITE

#include "core/types.h"
#include "core/memory/memory.h"

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif // _ITF_STRING_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_ARCHIVEMEMORY_H_
#include "core/ArchiveMemory.h"
#endif //_ITF_ARCHIVEMEMORY_H_

#ifndef ITF_MAP_H_
#include "core/container/extendedMap.h"
#endif //ITF_MAP_H_

#ifndef ITF_VECTOR_H_
#include "core/container/extendedVector.h"
#endif //ITF_VECTOR_H_

#ifndef ITF_LIST_H_
#include "core/container/extendedList.h"
#endif //ITF_LIST_H_

#ifdef ITF_WINDOWS
    #include <winsock2.h>
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <memory.h>
    #include <stdarg.h>

    #define ITF_WINSOCK2
#endif // ITF_WINDOWS

#ifndef _ITF_STRING8_H_
    #include "core/String8.h"
#endif // _ITF_STRING8_H_

#ifndef _ITF_STRINGID_H_
    #include "core/StringID.h"
#endif // _ITF_STRINGID_H_

#define ITF_NOT_IMPLEMENTED()    ITF_ASSERT(0)

#ifdef ITF_WINDOWS
#define NOT_IMPLEMENTED_ON_WINDOWS    ITF_ASSERT(0);
#define NOT_IMPLEMENTED_ON_X360
#define NOT_IMPLEMENTED_ON_PS3
#define NOT_IMPLEMENTED_ON_VITA
#define NOT_IMPLEMENTED_ON_GCM
#define NOT_IMPLEMENTED_ON_PS5
#define NOT_IMPLEMENTED_ON_NX
#define NOT_IMPLEMENTED_ON_NINTENDO
#define NOT_IMPLEMENTED_ON_XBOXSERIES
#elif defined (ITF_X360)
#define NOT_IMPLEMENTED_ON_WINDOWS
#define NOT_IMPLEMENTED_ON_X360     ITF_ASSERT(0);
#define NOT_IMPLEMENTED_ON_PS3
#define NOT_IMPLEMENTED_ON_GCM
#define NOT_IMPLEMENTED_ON_VITA
#define NOT_IMPLEMENTED_ON_PS5
#define NOT_IMPLEMENTED_ON_NX
#define NOT_IMPLEMENTED_ON_OUNCE
#define NOT_IMPLEMENTED_ON_NINTENDO
#define NOT_IMPLEMENTED_ON_XBOXSERIES
#elif defined (ITF_PS3)
ITF_INLINE void notSupportedOnPS3()
{
    //#undef printf
    //printf("NOT IMPLEMENTED PS3\n");
    ITF_ASSERT(0);
}

#define NOT_IMPLEMENTED_ON_WINDOWS
#define NOT_IMPLEMENTED_ON_X360
#define NOT_IMPLEMENTED_ON_VITA
#define NOT_IMPLEMENTED_ON_PS3      {notSupportedOnPS3();}
#define NOT_IMPLEMENTED_ON_GCM      //{notSupportedOnPS3();}
#define NOT_IMPLEMENTED_ON_PS5
#define NOT_IMPLEMENTED_ON_NX
#define NOT_IMPLEMENTED_ON_OUNCE
#define NOT_IMPLEMENTED_ON_NINTENDO
#define NOT_IMPLEMENTED_ON_XBOXSERIES
#elif defined (ITF_VITA)
#define NOT_IMPLEMENTED_ON_WINDOWS
#define NOT_IMPLEMENTED_ON_X360  
#define NOT_IMPLEMENTED_ON_PS3
#define NOT_IMPLEMENTED_ON_VITA ITF_ASSERT(0);
#define NOT_IMPLEMENTED_ON_GCM
#define NOT_IMPLEMENTED_ON_PS5
#define NOT_IMPLEMENTED_ON_NX
#define NOT_IMPLEMENTED_ON_OUNCE
#define NOT_IMPLEMENTED_ON_NINTENDO
#define NOT_IMPLEMENTED_ON_XBOXSERIES
#elif defined (ITF_PS5)
#define NOT_IMPLEMENTED_ON_WINDOWS
#define NOT_IMPLEMENTED_ON_X360
#define NOT_IMPLEMENTED_ON_PS3
#define NOT_IMPLEMENTED_ON_VITA
#define NOT_IMPLEMENTED_ON_GCM
#define NOT_IMPLEMENTED_ON_PS5 ITF_ASSERT(0);
#define NOT_IMPLEMENTED_ON_NX
#define NOT_IMPLEMENTED_ON_OUNCE
#define NOT_IMPLEMENTED_ON_NINTENDO
#define NOT_IMPLEMENTED_ON_XBOXSERIES
#elif defined (ITF_NX)
#define NOT_IMPLEMENTED_ON_WINDOWS
#define NOT_IMPLEMENTED_ON_X360
#define NOT_IMPLEMENTED_ON_PS3
#define NOT_IMPLEMENTED_ON_VITA
#define NOT_IMPLEMENTED_ON_GCM
#define NOT_IMPLEMENTED_ON_PS5
#define NOT_IMPLEMENTED_ON_NX ITF_ASSERT(0);
#define NOT_IMPLEMENTED_ON_OUNCE
#define NOT_IMPLEMENTED_ON_NINTENDO ITF_ASSERT(0);
#define NOT_IMPLEMENTED_ON_XBOXSERIES
#elif defined (ITF_OUNCE)
#define NOT_IMPLEMENTED_ON_WINDOWS
#define NOT_IMPLEMENTED_ON_X360
#define NOT_IMPLEMENTED_ON_PS3
#define NOT_IMPLEMENTED_ON_VITA
#define NOT_IMPLEMENTED_ON_GCM
#define NOT_IMPLEMENTED_ON_PS5
#define NOT_IMPLEMENTED_ON_NX
#define NOT_IMPLEMENTED_ON_OUNCE ITF_ASSERT(0);
#define NOT_IMPLEMENTED_ON_NINTENDO ITF_ASSERT(0);
#define NOT_IMPLEMENTED_ON_XBOXSERIES
#elif defined (ITF_XBOX_SERIES)
#define NOT_IMPLEMENTED_ON_WINDOWS
#define NOT_IMPLEMENTED_ON_X360
#define NOT_IMPLEMENTED_ON_PS3
#define NOT_IMPLEMENTED_ON_VITA
#define NOT_IMPLEMENTED_ON_GCM
#define NOT_IMPLEMENTED_ON_PS5
#define NOT_IMPLEMENTED_ON_NX
#define NOT_IMPLEMENTED_ON_OUNCE
#define NOT_IMPLEMENTED_ON_NINTENDO
#define NOT_IMPLEMENTED_ON_XBOXSERIES ITF_ASSERT(0);
#endif

//#define USE_PICKABLE_TRAJECTORIES

#endif // _ENGINECOMMON_H_
