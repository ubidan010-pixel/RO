#ifndef ITF_RAKIINCLUDES_
#define ITF_RAKIINCLUDES_

#pragma once

// ITF specific

#if defined ( ITF_WINDOWS )
    #define RAKI_PLATFORM_WIN32
    // TODO : Remove RAKI_PLATFORM_WIN32 and replace by RAKI_PLATFORM_WIN64
    #define RAKI_PLATFORM_WIN64
#elif defined ( ITF_XBOX_SERIES )
    #define RAKI_PLATFORM_XBOX_SERIES
#elif defined ( ITF_X360 )
    #define RAKI_PLATFORM_XB360
#elif defined ( ITF_PS3 )
    #define RAKI_PLATFORM_PS3
#elif defined ( ITF_WII )
    #define RAKI_PLATFORM_WII
#elif defined ( ITF_CTR )
    #define RAKI_PLATFORM_CTR
#elif defined ( ITF_PS5 )
    #define RAKI_PLATFORM_PS5
#elif defined ( ITF_NINTENDO )
    #define RAKI_PLATFORM_NINTENDO
    #if defined ( ITF_OUNCE )
        #define RAKI_PLATFORM_OUNCE
    #elif defined ( ITF_NX )
        #define RAKI_PLATFORM_NX
    #endif
#endif 


#if defined ( _DEBUG )
    #define RAKI_TARGET_DEBUG
#elif defined ( ITF_FINAL )
    #define RAKI_TARGET_FINAL
#else
    #define RAKI_TARGET_RELEASE
#endif 

#if defined ( RAKI_PLATFORM_WIN32 ) || defined ( RAKI_PLATFORM_WIN64 ) || defined ( RAKI_PLATFORM_XB360 )
#define RAKI_USING_XAUDIO2
#include <XAudio2.h>
#endif

#include "RakiEngine/System/RakiDebug.h"
#include "RakiEngine/System/RakiNew.h"
#include "RakiEngine/System/RakiTypes.h"
#include "RakiEngine/System/RakiThread.h"
#include "RakiEngine/System/RakiTime.h"
#include "RakiEngine/System/RakiSingleton.h"

#include "RakiEngine/System/Memory/RakiMemory.h"

#endif // ITF_RAKIINCLUDES_
