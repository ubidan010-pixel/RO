#ifndef ITF_CORE_MACROS_H_
#define ITF_CORE_MACROS_H_

#pragma once

// $GB 2025/25/02: temporarily removed until compilation of new platforms
// we should rather modify the calls or anyway avoid defines
#if 0 // !defined(ITF_MICROSOFT)
    #define _snprintf		snprintf
    #define sprintf_s		sprintf
    #define _vsnprintf_s	vsnprintf
    #define vsprintf_s		vsprintf
#endif // ITF_MICROSOFT
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL) && !defined(ITF_RETAIL)
// ONLY USE FOR INTERNAL ARTIST
// ENABLE FOR QUICK REVIEW SUPPORTED SAMPLER IN EDITOR
//#define DAN_TEXTURE_SELECTOR
#endif
// Everything that is not windows is a console for the moment
#if !defined(ITF_WINDOWS)
    #define ITF_CONSOLE 1
    #ifdef ITF_FINAL
        #define ITF_CONSOLE_FINAL 1
    #endif
#endif

// $GB 2025/25/02: rework on what is supported in final/windows only
#if defined(ITF_FINAL)
    #define ITF_DISABLE_WARNING 1
    #define ITF_DISABLE_LOG 1
    #define ITF_DISABLE_DEBUGINFO 1
    //Temporay enable for final mode
    #define ITF_SUPPORT_CHEAT 1
#else
    #define ITF_SUPPORT_CHEAT 1
    // $GB 2025/25/02: Should be renamed to ITF_RASTER_ENABLED
    #define RASTER_ENABLED 1
    #if !defined(ROBOT_COMPILATION)
        // $GB 2025/25/02: SUPPORT_STATS_MANAGER is for programmers
        #define SUPPORT_STATS_MANAGER 1
    #endif

    #if !defined(ROBOT_COMPILATION)
        // $GB 2025/25/02: Keep asserts for programmers in not final.
        #define ASSERT_ENABLED 1
    #endif

    #if defined(ITF_WINDOWS)
        // $GB 2025/25/02: Support of editor and debug features only on windows in not final or retail
        #define ITF_SUPPORT_EDITOR 1
        #define ITF_SUPPORT_DEBUGFEATURE 1
        #define ITF_ENABLE_EDITOR_KEYBOARD 1
        // $GB 2025/25/02: Support of network connexion for development tools
        #define ITF_SUPPORT_NET 1
        #if !defined(ROBOT_COMPILATION)
            // $GB 2025/25/02: memory tracking only for programmers local builds (NB: ROBOT_COMPILATION not yet set on build machines)
            // STACK ALLOCATION info
            #define ITF_TRACKING_MEMORY_ENABLE 1
            // CATEGORY ALLOCATION info
            #define ITF_CATEGORY_MEMORY_ENABLE 1
        #endif
    #endif
#endif

// Some options coming from ITF_SUPPORT_EDITOR support (XML,LOGICDATABASE SUPPORT,LUA..)
#if ITF_SUPPORT_EDITOR
    #define ITF_SUPPORT_LOGICDATABASE 1
    #define ITF_SUPPORT_LUA 1
    #define ITF_SUPPORT_HOTRELOAD 1  //must define ITF_SUPPORT_LOGICDATABASE
    #define ITF_SUPPORT_XML 1
    #define ITF_SUPPORT_BLOBEVENT 1
    #define ITF_SUPPORT_COOKING 1
    #define ITF_SUPPORT_PLUGIN 1
    #define ITF_DISABLE_VIDEO_EDITOR 1
#endif  //!defined (ITF_FINAL)

#if defined (ITF_PS3) || defined(ITF_PS5) || defined(ITF_NINTENDO) || defined(ITF_XBOX_SERIES)
#define ITF_SUPPORT_XML                 1 // Used by shaders for technique descriptors
#endif

// $GB 2025/25/02: All recent platforms support multithread and shaders
#define ITF_SUPPORT_MULTITHREADED 1
#define ANIMUSESHADER 1

// $GB 2025/25/02: removed support for network services and tracking:
// $GS 2025/25/11: support should work now =^.^=
#ifdef ITF_WINDOWS
#define ITF_SUPPORT_UPLAY 1

// $GS : need to tell apart UPLAY from PC / STEAM / LUNA / GFN later
#define ITF_UPLAYPC 1
#endif

#define ITF_SUPPORT_NETWORKSERVICES 1
#define ITF_SUPPORT_UBISERVICES 1
#define ITF_SUPPORT_ONLINETRACKING 1

// $GB 2025/25/02: ENABLE_ALLOCATOR was for a custom allocator for X360. Probably obsolete.
#ifdef ITF_X360
#define ENABLE_ALLOCATOR 1
//#define ITF_CATEGORY_MEMORY_ENABLE
//#define ITF_TRACKING_MEMORY_ENABLE
#endif //ITF_X360

#if defined(_MSC_VER)
    #define ITF_MSC_VER _MSC_VER
#endif

#if defined(_MSC_VER) && defined(_M_AMD64) && !defined(_M_ARM64EC)
    #ifndef ITF_AMD64_CPU
        #define ITF_AMD64_CPU 1
    #endif
#endif

#if defined(ITF_PS5)
    #ifndef ITF_AMD64_CPU
        #define ITF_AMD64_CPU 1
    #endif
#endif

#if defined(ITF_NX) || defined(ITF_OUNCE)
    #ifndef ITF_ARM64_CPU
        #define ITF_ARM64_CPU 1
    #endif
#endif

#if defined(__clang__)
    #ifndef ITF_CLANG
        #define ITF_CLANG 1
    #endif
#endif

//#define OSDK_DLL_SUPPORT
// $GB 2025/25/02: What is it?
#ifdef _DEBUG
    #define OSDK_DEBUG_VERSION 1
#else
    #define OSDK_RETAIL_VERSION 1
#endif

// $GB 2025/25/02: ANIMUSEGEOMETRYSHADER is an old define for 3DS
#if defined(ITF_CTR)
// ANIMUSEGEOMETRYSHADER
#define ANIMUSEGEOMETRYSHADER
#endif

// $GB 2025/25/02: EXPORT_ASSERTS is to activate the export of asserts to a web page.
// It was allowing to get logs of asserts happening in tools for users, without bothering them with popups.
// Deactivate as we do not have the infrastructure anymore.
// Also see ASSERT_ENABLED define that is not set on robot builds.
#if 0
    #if defined(ASSERT_ENABLED) && defined(ITF_WINDOWS) && defined(ROBOT_COMPILATION) && !defined(_DEBUG)
        #define EXPORT_ASSERTS 1
    #endif // ASSERT_ENABLED && ITF_WINDOWS && ROBOT_COMPILATION && !defined(_DEBUG)
#endif

#include "core/AssertManager.h"

// Prevent definition of "min" and "max"
#ifndef NOMINMAX
#define NOMINMAX
#endif

// To suppress warnings for "unused" variables
#define ITF_UNUSED(x) (void)(x);

//
#define ITF_ALLOCATE_STATIC_UNINITIALIZED_ARRAY(_variable, _type, _count) u32 _variable##_Room[(sizeof(_type)*_count+3)/4]; _type *_variable = (_type*)_variable##_Room
#define ITF_ALLOCATE_STATIC_UNINITIALIZED_VARIABLE(_variable, _type) u32 _variable##_Room[(sizeof(_type)+3)/4]; _type &_variable = *(_type*)_variable##_Room

#if !defined(ITF_MICROSOFT)
    #define ITF_POSIX_FRIENDLY 1
    #define ITF_PURE_POSIX 0
    #define ITF_POSIX_FILE_SYSTEM 1
#endif

#if defined(ITF_MICROSOFT)
    #define SPRINTF_S(buf_, size_, format_, ...)            sprintf_s(buf_, size_, format_, __VA_ARGS__)
    #define SPRINTF_S_TRUNC(buf_, size_, format_, ...)      FormatTruncate( buf_, size_, format_, __VA_ARGS__);
    #define SPRINTF_S_TRUNC_VALIST(buf_, size_, format_, list_) VFormatTruncate( buf_, size_, format_, list_);
    #define SSCANF_S(buf_, format_,...)                    sscanf_s(buf_, format_, __VA_ARGS__)
#else
    #define SPRINTF_S(buf_, size_, format_,...)            sprintf(buf_, format_, ## __VA_ARGS__)
    #define SPRINTF_S_TRUNC(buf_, size_, format_,...)      FormatTruncate( buf_, size_, format_, __VA_ARGS__)
    #define SPRINTF_S_TRUNC_VALIST(buf_, size_, format_, list_) VFormatTruncate( buf_, size_, format_, list_);
    #define SSCANF_S(buf_, format_, ...)                    sscanf(buf_, format_, ## __VA_ARGS__)
#endif // !ITF_MICROSOFT

// $GB 2025/28/02: replace __DEBUG_BREAK by ITF_DEBUG_BREAK
#if defined(ITF_MICROSOFT)
    #define ITF_DEBUG_BREAK() __debugbreak()
#elif defined(ITF_PS5)
    #include <libdbg.h>
    #define ITF_DEBUG_BREAK() SCE_BREAK()
#elif defined(ITF_NINTENDO)
    #include <nn/diag/diag_Break.h>
    #include <nn/diag/diag_Debugger.h>
    #define ITF_DEBUG_BREAK() NN_DIAG_BREAK()
#endif

// $GB 2025/25/02: Obsolete, do not use anymore, just use inline.
#define ITF_INLINE              inline

// $GB 2025/25/02: ITF_RESTRICT as a define to handle differences that could exist between compilers.
// While restrict is C99 keyword, there is no such thing in C++.
// Anyway __restrict is available on GCC/Clang/MSVC in C++ but subtle propagation rules can change.
#define ITF_RESTRICT __restrict

// $GB 2025/25/02: ITF_FORCE_INLINE also set for other compilers (ref code from wikipedia).
// This change came with a remove of excessive force inline in the code.
#if defined(_MSC_VER)
    #define ITF_FORCE_INLINE __forceinline
#elif defined(__GNUC__)
    #define ITF_FORCE_INLINE inline __attribute__((__always_inline__))
#elif defined(__clang__ )
    #if __has_attribute(__always_inline__)
        #define ITF_FORCE_INLINE inline __attribute__((__always_inline__))
    #else
        #define ITF_FORCE_INLINE inline
    #endif
#else
    #define ITF_FORCE_INLINE inline
#endif

#if defined(_MSC_VER)
    #define ITF_ASSUME(cond)                            __assume(cond)
    #define ITF_DISABLE_WARNING_PUSH                    __pragma(warning(push))
    #define ITF_DISABLE_WARNING_ID(id_)                __pragma(warning(disable:id_))
    #define ITF_DISABLE_WARNING_POP                     __pragma(warning(pop))
    #pragma warning( error : 4715 )
#elif defined(__GNUC__) || defined(__clang__)
    #define ITF_ASSUME(cond)                            do { if (!(cond)) __builtin_unreachable(); } while (0)
    #define ITF_DISABLE_WARNING_PUSH                    /*#pragma GCC diagnostic push*/
    #define ITF_DISABLE_WARNING_ID(id_)
    #define ITF_DISABLE_WARNING_POP                     /*#pragma GCC diagnostic pop*/
#else
    #define ITF_ASSUME(cond)                            do {} while (0)
    #define ITF_DISABLE_WARNING_PUSH
    #define ITF_DISABLE_WARNING_ID(id_)
    #define ITF_DISABLE_WARNING_POP
#endif

#if defined(ASSERT_ENABLED) && !defined(ITF_DISABLE_LOG)
    #if defined(EXPORT_ASSERTS)

        #define ITF_ASSERT_EXPORT(condition, forcecrash, ... )                                                                  \
        do {                                                                                                                       \
            ITF_DISABLE_WARNING_PUSH                                                                                            \
            ITF_DISABLE_WARNING_ID(4127)                                                                                        \
                                                                                                                                \
            if(!(condition))                                                                                                    \
            {                                                                                                                   \
                if(forcecrash)                                                                                                  \
                {                                                                                                               \
                    int *crash = NULL;                                                                                          \
                    (*crash)++;                                                                                                 \
                }                                                                                                               \
            }                                                                                                                   \
                                                                                                                                \
            ITF_DISABLE_WARNING_POP                                                                                             \
        } while(0)

        #define ITF_ASSERT_CRASH(cond, ...)                 ITF_ASSERT_EXPORT(cond, true, __VA_ARGS__ )
        #define ITF_ASSERT_MSG_FILTER(cond, flag, ...)      ITF_ASSERT_EXPORT(cond, false, __VA_ARGS__ )
        #define ITF_VERIFY_MSG_FILTER(cond, flag, ...)      ITF_ASSERT_EXPORT(cond, false, __VA_ARGS__ )
        #define ITF_ASSERT_FILTER(cond, flag)               ITF_ASSERT_EXPORT(cond, false, NULL)
        #define ITF_VERIFY_FILTER(cond, flag)               ITF_ASSERT_EXPORT(cond, false, NULL)

    #else // !EXPORT_ASSERTS
        // Version if supporting AssertManager::OutputDebugMsg
        #if defined(ITF_MICROSOFT)
            #define ITF_ASSERT_IGNORABLE(condition, verify, filter, ... )                                                           \
            do {                                                                                                                       \
                ITF_DISABLE_WARNING_PUSH                                                                                            \
                ITF_DISABLE_WARNING_ID(4127)                                                                                        \
                                                                                                                                    \
                if(!(condition))                                                                                                    \
                {                                                                                                                   \
                    static int iSkipCount = 0;                                                                                      \
                    static bool bSkipAlways = false;                                                                                \
                                                                                                                                    \
                    if(!bSkipAlways)                                                                                                \
                    {                                                                                                               \
                        if(ITF::AssertManager::isFlagEnabled(filter))                                                               \
                        {                                                                                                           \
                            switch(ITF::AssertManager::CallAssert(#condition, __FILE__, __LINE__, __VA_ARGS__ ))                    \
                            {                                                                                                       \
                            case ITF::AssertManager::Break:                                                                         \
                                ITF_DEBUG_BREAK();                                                                                    \
                                break;                                                                                              \
                            case ITF::AssertManager::IgnoreAlways:                                                                  \
                                bSkipAlways = true;                                                                                 \
                                ++iSkipCount;                                                                                       \
                                break;                                                                                              \
                            default:                                                                                                \
                                ++iSkipCount;                                                                                       \
                                break;                                                                                              \
                            }                                                                                                       \
                        }                                                                                                           \
                    }                                                                                                               \
                    else ++iSkipCount;                                                                                              \
                                                                                                                                    \
                    ITF::AssertManager::OutputDebugMsg(#condition, __FILE__, __LINE__, iSkipCount, __VA_ARGS__ );                   \
                }                                                                                                                   \
                                                                                                                                    \
                ITF_DISABLE_WARNING_POP                                                                                             \
            } while(0)
        #else
            #define ITF_ASSERT_IGNORABLE(condition, verify, filter, ... )                                                           \
            do {                                                                                                                       \
                ITF_DISABLE_WARNING_PUSH                                                                                            \
                ITF_DISABLE_WARNING_ID(4127)                                                                                        \
                                                                                                                                    \
                if(!(condition))                                                                                                    \
                {                                                                                                                   \
                    static unsigned char bSkipAlways = 0;                                                                                      \
                    if(!bSkipAlways)                                                                                                \
                    {                                                                                                               \
                        if(ITF::AssertManager::isFlagEnabled(filter))                                                               \
                        {                                                                                                           \
                            switch(ITF::AssertManager::CallAssert(#condition, __FILE__, __LINE__, __VA_ARGS__ ))                    \
                            {                                                                                                       \
                            case ITF::AssertManager::Break:                                                                         \
                                ITF_DEBUG_BREAK();                                                                                    \
                                break;                                                                                              \
                            case ITF::AssertManager::IgnoreAlways:                                                                  \
                                bSkipAlways = 1;                                                                                    \
                                break;                                                                                              \
                            default:                                                                                                \
                                break;                                                                                              \
                            }                                                                                                       \
                        }                                                                                                           \
                    }                                                                                                               \
                }                                                                                                                   \
                                                                                                                                    \
                ITF_DISABLE_WARNING_POP                                                                                             \
            } while(0)
        #endif // Not support AssertManager::OutputDebugMsg
        #define ITF_ASSERT_CRASH(condition, ... )                                                                               \
        do {                                                                                                                       \
            ITF_DISABLE_WARNING_PUSH                                                                                            \
            ITF_DISABLE_WARNING_ID(4127)                                                                                        \
                                                                                                                                \
            if(!(condition))                                                                                                    \
            {                                                                                                                   \
                ITF::AssertManager::displayAssert(#condition, __FILE__, __LINE__, __VA_ARGS__ );                                \
                ITF::AssertManager::OutputDebugMsg(#condition,__FILE__,__LINE__,0,__VA_ARGS__);                                 \
                                                                                                                                \
                ITF_DEBUG_BREAK();                                                                                                \
                                                                                                                                \
                int *crash = NULL;                                                                                              \
                (*crash)++;                                                                                                     \
            }                                                                                                                   \
                                                                                                                                \
            ITF_DISABLE_WARNING_POP                                                                                             \
        } while(0)

        #define ITF_ASSERT_MSG_FILTER(cond, flag, ... )     ITF_ASSERT_IGNORABLE(cond, false, flag, __VA_ARGS__ )
        #define ITF_VERIFY_MSG_FILTER(cond, flag, ... )     ITF_ASSERT_IGNORABLE(cond, true, flag, __VA_ARGS__ )
        #define ITF_ASSERT_FILTER(cond, flag)               ITF_ASSERT_IGNORABLE(cond, false, flag, NULL )
        #define ITF_VERIFY_FILTER(cond, flag)               ITF_ASSERT_IGNORABLE(cond, true, flag, NULL )
    #endif // !EXPORT_ASSERTS
#else // !ASSERT_ENABLED

#if !defined(ITF_FINAL)
    #define ITF_ASSERT_CRASH(condition, ... )                                                                                   \
        do {                                                                                                                       \
            ITF_DISABLE_WARNING_PUSH                                                                                            \
            ITF_DISABLE_WARNING_ID(4127)                                                                                        \
                                                                                                                                \
            if(!(condition))                                                                                                    \
            {                                                                                                                   \
                int *crash = NULL;                                                                                              \
                (*crash)++;                                                                                                     \
            }                                                                                                                   \
                                                                                                                                \
            ITF_DISABLE_WARNING_POP                                                                                             \
        } while(0)
#else
    #define ITF_ASSERT_CRASH(condition, ... )              do {} while(0)
#endif
    #define ITF_ASSERT_MSG_FILTER(cond, flag, ...)         do {} while(0)
    #define ITF_VERIFY_MSG_FILTER(cond, flag, ...)         (!!(cond))
    #define ITF_ASSERT_FILTER(cond, flag)                  do {} while(0)
    #define ITF_VERIFY_FILTER(cond, flag)                  (!!(cond))
#endif // !ASSERT_ENABLED

#define ITF_ASSERT_MSG(cond, ...)                           ITF_ASSERT_MSG_FILTER(cond, ITF::AssertManager::m_FlagAll, __VA_ARGS__ )
#define ITF_ASSERT(cond)                                    ITF_ASSERT_FILTER(cond, ITF::AssertManager::m_FlagAll)
#define ITF_VERIFY_MSG(cond, ...)                           ITF_VERIFY_MSG_FILTER(cond, ITF::AssertManager::m_FlagAll, __VA_ARGS__ )
#define ITF_VERIFY(cond)                                    ITF_VERIFY_FILTER(cond, ITF::AssertManager::m_FlagAll)

#if defined(ITF_SONY)
    #if defined(ASSERT_ENABLED)
        #define ITF_VERIFY_SCE_CALL_NAMED(call_, name_) [&](){ int res_ = (call_); ITF_ASSERT_MSG(res_ >= SCE_OK, "SCE call failed %s with error code 0x%08x", name_, res_); }()
        #define ITF_ASSERT_SCE_RESULT_NAMED(res_, name_) ITF_VERIFY_SCE_CALL_NAMED(res_, name_)
    #else
        #define ITF_VERIFY_SCE_CALL_NAMED(call_, name_) call_
        #define ITF_ASSERT_SCE_RESULT_NAMED(res_, name_) do { (void)sizeof(res_); } while(0)
    #endif
    #define ITF_VERIFY_SCE_CALL(call_) ITF_VERIFY_SCE_CALL_NAMED(call_, #call_)
    #define ITF_ASSERT_SCE_RESULT(res_) ITF_ASSERT_SCE_RESULT_NAMED(res_, #res_)
#elif defined(ITF_NINTENDO)
    #if defined(ASSERT_ENABLED)
        #define ITF_VERIFY_NN_CALL_NAMED(call_, name_) [&](){ ::nn::Result res_ = (call_); ITF_ASSERT_MSG(res_.IsSuccess(), "NN call failed %s with error code 0x%08x", name_, res_.GetInnerValueForDebug()); }()
        #define ITF_ASSERT_NN_RESULT_NAMED(res_, name_) ITF_VERIFY_NN_CALL_NAMED(res_, name_)
    #else
        #define ITF_VERIFY_NN_CALL_NAMED(call_, name_) call_
        #define ITF_ASSERT_NN_RESULT_NAMED(res_, name_) do { (void)sizeof(res_); } while(0)
    #endif
    #define ITF_VERIFY_NN_CALL(call_) ITF_VERIFY_NN_CALL_NAMED(call_, #call_)
    #define ITF_ASSERT_NN_RESULT(res_) ITF_ASSERT_NN_RESULT_NAMED(res_, #res_)
#elif defined(ITF_MICROSOFT)
    #if defined(ASSERT_ENABLED)
        #define ITF_VERIFY_HR_CALL_NAMED(call_, name_) [&](){ \
                HRESULT hr_ = (call_); \
                if (FAILED(hr_)) \
                { \
                    char hresultMsg_[200]; \
                    ITF::AssertManager::HResultToStringBuf(hr_, hresultMsg_); \
                    ITF_ASSERT_MSG(SUCCEEDED(hr_), "HRESULT failed %s. %s", name_, hresultMsg_); \
                } \
            }()
        #define ITF_ASSERT_HR_RESULT_NAMED(res_, name_) ITF_VERIFY_HR_CALL_NAMED(res_, name_)
    #else
        #define ITF_VERIFY_HR_CALL_NAMED(call_, name_) call_
        #define ITF_ASSERT_HR_RESULT_NAMED(res_, name_) do { (void)sizeof(res_); } while(0)
    #endif
    #define ITF_VERIFY_HR_CALL(call_) ITF_VERIFY_HR_CALL_NAMED(call_, #call_)
    #define ITF_ASSERT_HR_RESULT(res_) ITF_ASSERT_HR_RESULT_NAMED(res_, #res_)
#endif

#define ITF_AUTO_TEST(c) ITF_ASSERT_CRASH((c), "unit/auto test failure")

// This assert can only be used with constant expr, and it is checked at compilation time
// The msg parameter is only here to allow programmers to display a string next to their test
// When the assert fails, it is considered as a "divide by zero" error by the compiler
//#define ITF_ASSERT_STATIC(expr, msg) { enum  { Static_Assert__ = 1/(int)(expr) }; }

#define ITF_ASSERT_STATIC(expr, msg) static_assert(expr, msg)

#if !defined(ITF_DISABLE_WARNING)
#define ITF_WARNING(_baseobject_, _assertion_, ...) ITF_WARNING_CATEGORY(Default,_baseobject_,_assertion_,__VA_ARGS__)
#define ITF_WARNING_PARAM(_baseobject_, _parameter_, _assertion_, ...) ITF_WARNING_PARAM_CATEGORY(Default,_baseobject_,_parameter_,_assertion_, __VA_ARGS__)
    #define ITF_WARNING_CATEGORY(_category,_baseobject_, _assertion_, ...) do\
            {\
                ITF_DISABLE_WARNING_PUSH\
                ITF_DISABLE_WARNING_ID(4127)\
                if (!(_assertion_))\
                {\
                    ITF_ASSERT_MSG_FILTER(_assertion_, ITF::AssertManager::m_FlagWarningAndError, __VA_ARGS__ );\
                    WarningManager::addWarning(WarningCategory_##_category,_baseobject_, NULL, __VA_ARGS__ );\
                }\
                ITF_DISABLE_WARNING_POP\
            } while(WarningManager::returnFalse())
    #define ITF_WARNING_PARAM_CATEGORY(_category, _baseobject_, _parameter_, _assertion_, ...) do { \
                ITF_DISABLE_WARNING_PUSH\
                ITF_DISABLE_WARNING_ID(4127)\
                if (!(_assertion_))\
                {\
                    ITF_ASSERT_MSG_FILTER(_assertion_, ITF::AssertManager::m_FlagWarningAndError, _parameter_ ":" __VA_ARGS__ );\
                    WarningManager::addWarning(WarningCategory_##_category,_baseobject_, _parameter_, __VA_ARGS__ );\
                }\
                ITF_DISABLE_WARNING_POP\
            } while (WarningManager::returnFalse())
    #define VALIDATE_COMPONENT_PARAM(_parameter_, _assertion_,...) VALIDATE_COMPONENT_PARAM_CATEGORY(LD,_parameter_, _assertion_,__VA_ARGS__)
    #define VALIDATE_COMPONENT_PARAM_CATEGORY(_category_,_parameter_, _assertion_,...)  do { \
            ITF_DISABLE_WARNING_PUSH\
            ITF_DISABLE_WARNING_ID(4127)\
            if (!(_assertion_))\
                {\
                _isComponentValidated=bfalse;\
                ITF_ASSERT_MSG_FILTER(_assertion_, ITF::AssertManager::m_FlagWarningAndError, _parameter_ ":" __VA_ARGS__ );\
                WarningManager::addWarning(WarningCategory_##_category_, GetActor(), _parameter_,__VA_ARGS__ );\
                }\
            ITF_DISABLE_WARNING_POP\
            } while (WarningManager::returnFalse())
    #define VALIDATE_BEHAVIOR_PARAM(_parameter_, _assertion_, ...)  VALIDATE_BEHAVIOR_PARAM_CATEGORY(LD,_parameter_,_assertion_,__VA_ARGS__)
    #define VALIDATE_BEHAVIOR_PARAM_CATEGORY(_category_,_parameter_, _assertion_, ...)  do { \
        ITF_DISABLE_WARNING_PUSH\
        ITF_DISABLE_WARNING_ID(4127)\
        if (!(_assertion_))\
            {\
            _isBehaviorValidated=bfalse;\
            ITF_ASSERT_MSG_FILTER(_assertion_, ITF::AssertManager::m_FlagWarningAndError, _parameter_ ":" __VA_ARGS__ );\
            WarningManager::addWarning(WarningCategory_##_category_,m_actor, _parameter_,  __VA_ARGS__ );\
            }\
        ITF_DISABLE_WARNING_POP\
        } while (WarningManager::returnFalse())
    #define VALIDATE_ACTION_PARAM(_parameter_, _assertion_, ...)  do { \
        ITF_DISABLE_WARNING_PUSH\
        ITF_DISABLE_WARNING_ID(4127)\
        if (!(_assertion_))\
            {\
            _isActionValidated=bfalse;\
            ITF_ASSERT_MSG_FILTER(_assertion_, ITF::AssertManager::m_FlagWarningAndError, _parameter_ ":" __VA_ARGS__ );\
            WarningManager::addWarning(WarningCategory_LD,m_actor, _parameter_, __VA_ARGS__ );\
            }\
        ITF_DISABLE_WARNING_POP\
        } while (WarningManager::returnFalse())
    #define VALIDATE_PARAM_CATEGORY(_category_,_parameter_, _assertion_, ...)  do { \
        ITF_DISABLE_WARNING_PUSH\
        ITF_DISABLE_WARNING_ID(4127)\
        if (!(_assertion_))\
                        {\
                        _isComponentValidated=bfalse;\
                        ITF_ASSERT_MSG_FILTER(_assertion_, ITF::AssertManager::m_FlagWarningAndError, _parameter_ ":" __VA_ARGS__ );\
                        WarningManager::addWarning(WarningCategory_##_category_,NULL, _parameter_, __VA_ARGS__ );\
                        }\
                        ITF_DISABLE_WARNING_POP\
                } while (WarningManager::returnFalse())
    #define VALIDATE_PARAM(_parameter_, _assertion_, ...)  VALIDATE_PARAM_CATEGORY(LD,_parameter_,_assertion_,__VA_ARGS__)
#else
    #define ITF_WARNING(_baseobject_, _assertion_, _messageFormat_,...) ((void)0)
    #define ITF_WARNING_PARAM(_baseobject_, _parameter_, _assertion_, _messageFormat_,...) ((void)0)
    #define ITF_WARNING_CATEGORY(_category,_baseobject_, _assertion_, _messageFormat_, ...) ((void)0)
    #define ITF_WARNING_PARAM_CATEGORY(_category, _baseobject_, _parameter_, _assertion_, _messageFormat, ...) ((void)0)
    #define VALIDATE_COMPONENT_PARAM(_parameter_, _assertion_, _messageFormat_,...) ((void)0)
    #define VALIDATE_COMPONENT_PARAM_CATEGORY(_category_,_parameter_, _assertion_,_messageFormat,...) ((void)0)
    #define VALIDATE_BEHAVIOR_PARAM(_parameter_, _assertion_, _messageFormat_,...) ((void)0)
    #define VALIDATE_BEHAVIOR_PARAM_CATEGORY(_category_,_parameter_, _assertion_, ...) ((void)0)
    #define VALIDATE_ACTION_PARAM(_parameter_, _assertion_, _messageFormat_,...) ((void)0)
    #define VALIDATE_PARAM(_parameter_, _assertion_, _messageFormat_,...) ((void)0)
    #define VALIDATE_PARAM_CATEGORY(_category_,_parameter_, _assertion_, _messageFormat_,...) ((void)0)
#endif

#define ITF_COMPILE_TIME_ASSERT(name, x) static_assert((x), #name)

#define SF_DEL(p)           { if (p) { delete (p);    (p) = 0;} }
#define SF_DEL_ARRAY(p)     { if (p) { delete [] (p); (p) = 0;} }
#define SF_RELEASE(p)       { if (p) { (p)->Release();(p) = 0;} }

#define ITF_MALLOC(expr)    use_new[]_instead
#define ITF_FREE(expr)      use_delete[]_instead

#define ITF_INVALID_OBJREF      0 // temporary to avoid static initialization order issues, TODO replace ITF_INVALID_OBJREF everywhere


// Array size giving an error if the object is not an array (with a pointer for instance)
template <typename T, unsigned N>
char(&_arrayOfChar_(T(&)[N]))[N]; // this is declaring a function, _arrayOfChar_, taking a reference on an array of T, and returning a reference of a char [N] array
#define ITF_ARRAY_SIZE(_array_) (ITF::uSize(sizeof(_arrayOfChar_(_array_))))

#define ITF_MASK(sizeInBits)    i64((i64(1) << sizeInBits) - 1)

namespace ITF
{
    const float MTH_PI              =   3.14159265359f;
    const float MTH_BYPI            =   1.f / MTH_PI;
    const float MTH_2PI             =   MTH_PI * 2.f;
    const float MTH_BY2PI           =   1.f / (MTH_PI * 2.f);
    const float MTH_PIBY2           =   MTH_PI * 0.5f;
    const float MTH_PIBY3           =   MTH_PI / 3.f;
    const float MTH_PIBY4           =   MTH_PI * 0.25f;
    const float MTH_PIBY6           =   MTH_PI / 6.f;
    const float MTH_PIBY8           =   MTH_PI * 0.125f;
    const float MTH_3PIBY8          =   3.f * MTH_PIBY8;
    const float MTH_DEGTORAD        =   MTH_PI / 180.f;
    const float MTH_RADTODEG        =   180.f * MTH_BYPI;
    const float MTH_EPSILON         =   1.e-5f;
    const float MTH_EPSILON_2       =   MTH_EPSILON*MTH_EPSILON;
    const float MTH_BIG_EPSILON     =   1.e-3f;
    const float MTH_HUGE            =   1.e+30f;
    const float MTH_EULER           =   2.71828182845904523536f;
    const float MTH_SQRT2           =   1.4142135623730950488f;
    const float MTH_SQRT2BY2        =   MTH_SQRT2 *0.5f;
    const float MTH_16BY9           =   16.f / 9.f;
    const float MTH_9BY16           =   9.f / 16.f;

} // namespace ITF

#if !defined(ITF_WINDOWS) && !defined(MAKEFOURCC)
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
        ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) |   \
        ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
#endif

#define F32_ALMOST_EQUALS(v1, v2, vEpsilon) (f32_Abs((v1) - (v2)) <= (vEpsilon))

#define ITF_MEMCOPY ITF_Memcpy

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Selector.h"

#ifndef ITF_FINAL
    #ifdef ITF_WINDOWS
        // Avoid including <windows.h> for IsDebuggerPresent
        extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
        #define ISDEBUGGER_PRESENT (::IsDebuggerPresent()!=0)
    #elif defined(ITF_SONY)
        #define ISDEBUGGER_PRESENT (::sceDbgIsDebuggerAttached()!=0)
    #elif defined(ITF_NINTENDO)
        #define ISDEBUGGER_PRESENT (nn::diag::IsDebuggerAttached())
    #else
        #define ISDEBUGGER_PRESENT (false)
    #endif
#else
    #define ISDEBUGGER_PRESENT (false)
#endif

#ifndef ITF_FINAL
#define ITF_SUPPORT_NINTENDO_PROFILER
#endif

#ifndef ITF_ENABLE_LOG
    #ifndef ITF_RELEASE
        #define ITF_ENABLE_LOG 1
    #else
        #define ITF_ENABLE_LOG 0
    #endif
#endif
#if defined(ITF_PS5)
    #define ITF_OUTPUT_LOG_TO_CONSOLE(buffer) \
        do { \
            ::fprintf(stderr, "%s\n", (buffer)); \
            ::fflush(stderr); \
        } while(0)
#elif defined(ITF_NINTENDO)
    #define ITF_OUTPUT_LOG_TO_CONSOLE(buffer) \
        do { \
            NN_LOG("%s\n", (buffer)); \
            std::cout << (buffer) << '\n'; \
            std::cout.flush(); \
        } while(0)
#elif defined(ITF_WINDOWS)
    #define ITF_OUTPUT_LOG_TO_CONSOLE(buffer) \
        do { \
            std::cout << (buffer) << '\n'; \
            std::cout.flush(); \
        } while(0)
#else
    #define ITF_OUTPUT_LOG_TO_CONSOLE(buffer) \
        do { \
            ::fprintf(stderr, "%s\n", (buffer)); \
            ::fflush(stderr); \
        } while(0)
#endif

#endif //ITF_CORE_MACROS_H_
