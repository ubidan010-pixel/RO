////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file eallog.h  Engine Abstraction Layer Logging Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ealdef.h>
#include <cstdarg>

/// This module covers the logging reference API.
/*! \addtogroup Log
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210)
/// See ealdef.h for more information.
#define EAL_LOG_VERSION             601

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef eal_u32         eal_log_level;      ///< Logging levels

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Logging levels
///
/// Note from the EAL Committee to implementers:
///          Please advise that despite the general understanding of EAL Log levels so far, the
///          enum values are not ordered by "verbosity level"; they are only bit masks each
///          representing a "log channel", thus multiple levels can be specified at the same time.
///          While this differs from typical log levels in other systems, for some odd reason this
///          was the actual intention behind the original design, and you will need to check each
///          bit value that interests your implementation.
///          --> We strongly discourage combining multiple level flags together.
enum eal_log_levels
{
    EAL_LOG_NONE        = 0x00000000,       ///< Level 0: No logging
    EAL_LOG_DEBUG       = 0x00000010,       ///< Level 1: Used to display debugging information (NEW in 5.00)
    EAL_LOG_INFO        = 0x00000001,       ///< Level 2: Used to display general information
    EAL_LOG_WARNING     = 0x00000002,       ///< Level 3: Used to display warnings
    EAL_LOG_ERROR       = 0x00000004,       ///< Level 4: Used to display errors
    EAL_LOG_CRITICAL    = 0x00000008        ///< Level 5: Used to display critical errors (NEW in 5.00); also consider using \ref EalLogAssert2
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Assert options
enum eal_log_assert_opt
{
    EAL_LOG_ASSERT_NORMAL       = 0x00000000,   ///< Normal assert (always display, can be skipped)
    EAL_LOG_ASSERT_ONCE         = 0x00000001,   ///< Caller recommend showing the assert only once
    EAL_LOG_ASSERT_DO_NOT_SKIP  = 0x00000002    ///< Caller recommend never skipping the assert
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Initializes the library.
    \return true if initialization succeeded

    \param Version      Must be equal to current version = EAL_LOG_VERSION

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to EalLogLibInit(), so
      EalLogLibShutdown() is called the exact same number of times.
    - Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - The library will be initialized and all APIs calls will be allowed.
    - EallogLibShutdown() must be called even if false is returned.

    \sa EalLogLibShutdown
*/
EAL_DLL_ENTRY bool EalLogLibInit(eal_u32 Version = EAL_LOG_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Shuts down the library after usage.
    \PRECONDITIONS
    - Must be called once for every call to EalLogLibInit().

    \POSTCONDITIONS
    - You are not allowed to call any other library member after calling EalLogLibShutdown().

    \sa EalLogLibInit
*/
EAL_DLL_ENTRY void EalLogLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Queries if a log will be displayed or not based on its Tag & Log Level.
    This function is used for optimization purpose. Log that does not need to be displayed, will
    not build up its string, which may be costly when snprintf or similar functions are used.

    \return true    If the caller should call EalLogOutput to display the log, or false if it is
                    100% that EalLogOutput would discard the log.

    \param Tag      The tag identifying the origin of the call. See ealdef.h.
    \param Level    The level of importance of the output, see \ref eal_log_levels for valid values.
                    We strongly discourage combining multiple level flags together.

    \PRECONDITIONS
    - Level must be a non-zero value built from values in \ref eal_log_levels.
*/
EAL_DLL_ENTRY bool EalLogIsEnabled(eal_id Tag, eal_log_level Level);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Sends an output to the logging subsystem.
    This is intended for smaller, pre-formatted strings (less than 1 KiB).
    \param Tag      The tag identifying the origin of the call. See ealdef.h.
    \param Level    The level of importance of the output, see \ref eal_log_levels for valid values.
                    We strongly discourage combining multiple level flags together.
    \param pMessage A valid string containing the message to print. Can be a dynamic string.
                    String can contain the '\n' character for EOL.
    \param pFile    A string containing the filename (can be NULL)
    \param Line     The line number of the log line in the calling file

    \PRECONDITIONS
    - pMessage cannot be NULL

    \sa EalLogOutputWithSize, EalLogOutputWithFormatVa, EalLogIsEnabled
*/
EAL_DLL_ENTRY void EalLogOutput(eal_id          Tag,
                                eal_log_level   Level,
                                const char*     pMessage,
                                const char*     pFile,
                                eal_u32         Line);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Sends an output to the logging subsystem using a vprintf-style format string and an argument list.
    This is intended for smaller strings (less than 1 KiB).
    \param Tag      The tag identifying the origin of the call. See ealdef.h.
    \param Level    The level of importance of the output, see \ref eal_log_levels for valid values.
                    We strongly discourage combining multiple level flags together.
    \param pFile    A string containing the filename (can be NULL)
    \param Line     The line number of the log line in the calling file
    \param pMessageFormat   A valid string containing the message format to print. Can be a dynamic string.
                            String can contain the '\n' character for EOL.
    \param MessageArgs      Additional arguments to be sent to the output device

    \PRECONDITIONS
    - pMessageFormat cannot be NULL

    \sa EalLogOutput, EalLogOutputWithSize, EalLogIsEnabled
*/

EAL_DLL_ENTRY void EalLogOutputWithFormatVa(eal_id Tag,
                                            eal_log_level Level,
                                            const char* pFile,
                                            eal_u32 Line,
                                            const char* pMessageFormat,
                                            va_list MessageArgs);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Assert handler (deprecated).
    \deprecated DEPRECATED since version 2.00. Please use \ref EalLogAssert2 instead.
    This function will be removed in version 5.00.
    Implementers must still support this function until then, while clients should avoid it as
    soon as possible.
    Please contact mailto:gearsupport@ubisoft.com if you think this will be a problem.

    \return true if the caller should BREAK in the debugger
    \return false if the assert should be ignored. In that case, do not break.

    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pCondition   A string containing the condition that failed (can be NULL).
    \param pMessage     A string containing extra information on the error (can be NULL).
    \param pFile        A string containing the faulty filename (can be NULL)
    \param Line         The line number of the assert line in the calling file
    \param pStatic      Pointer to a static variable of type eal_u32. This variable should be
                        set by default to 0 and never modified. There should be a unique variable
                        per assert function in the code (we recommend declaring a static variable
                        in a macro before calling \ref EalLogAssert).
                        This variable is used by the supplier to implement "always skip features".
                        This variable can be NULL. In that case, the supplier "always skip" feature
                        will not work for that specific assert.

    \POSTCONDITIONS
    - This function will inform the user of the assert and potentially prompt him on the action to
      be taken.
    - This function SHOULD NOT HALT. Halt should be done on the caller side (we want the
      debugger to stop at the line causing the assert, and that can only be done using a macro)

    \sa EalLogAssert2
*/
EAL_DEPRECATED("Deprecated; use EalLogAssert2 instead")
EAL_DLL_ENTRY bool EalLogAssert(eal_id      Tag,
                                const char* pCondition,
                                const char* pMessage,
                                const char* pFile,
                                eal_u32     Line,
                                eal_u32*    pStatic);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Assert handler.
    This function is an assert handler that takes care of informing the user that an assert with
    a failing condition was hit. The handler will then request to the user if the code should
    break or not. The break instruction itself must be called on the client side if the function
    return true.

    \return true if the caller should BREAK in the debugger
    \return false if the assert should be ignored. In that case, do not break.

    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pCondition   A string containing the condition that failed (can be NULL).
    \param pMessage     A string containing extra information on the error (can be NULL).
    \param pFile        A string containing the faulty filename (can be NULL)
    \param Line         The line number of the assert line in the calling file
    \param Options      0 by default. Can be used to inform the handler that the assert should
                        wither be displayed once (\ref EAL_LOG_ASSERT_ONCE) or never skipped
                        (\ref EAL_LOG_ASSERT_DO_NOT_SKIP).
    \param pStatic      Pointer to a static variable of type eal_u32. This variable should be
                        set by default to 0 and never modified. There should be a unique variable
                        per assert function in the code (we recommend declaring a static variable
                        in a macro before calling \ref EalLogAssert2).
                        This variable is used by the supplier to implement "always skip features".
                        This variable can be NULL. In that case, the supplier "always skip" feature
                        will not work for that specific assert.

    \POSTCONDITIONS
    - This function will inform the user of the assert and potentially prompt him on the action to
      be taken.
    - This function SHOULD NOT HALT. Halt should be done on the caller side (we want the
      debugger to stop at the line causing the assert, and that can only be done using a macro)
*/
EAL_DLL_ENTRY bool EalLogAssert2(eal_id             Tag,
                                 const char*        pCondition,
                                 const char*        pMessage,
                                 const char*        pFile,
                                 eal_u32            Line,
                                 eal_log_assert_opt Options,
                                 eal_u32*           pStatic);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Associate a Tag (or eal id) to a human readable string.
    This function should be called early in the initialization of any client using any EAL API, in
    order to associate a numerical Eal Id with a readable string - in release & debug target.
    Strings are used by tracking tools, display tools, and any form of resource management or for
    leak investigation.

    \param Tag          The tag (or eal id) to link with a string.
    \param pTagName     A static & valid string. String copy may not be done, so the string must
                        be valid through the entire execution of the program.

    \PRECONDITIONS
    - We recommend calling this function early, before using the tag in any other eal function
    - It is legal to call this function with the same Tag/TagName pair many times (in case of
      multiple inits or instances).
*/
EAL_DLL_ENTRY void EalLogSetTagName(eal_id Tag, const char* pTagName);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Return a human readable version of a tag (eal id)

    \return A valid, static human readable string. The string cannot be null (empty at worst)
    \param Tag  The tag from which we want a string description of the tag

    \POSTCONDITIONS
    - The string returned must live & stay valid for the entire program execution
    - Tips: Since eal_ids are defined by product in ealdef.h, it is easy to AT LEAST return
      the correct product name for theses, when tags are not sets.
*/
EAL_DLL_ENTRY const char* EalLogGetTagName(eal_id Tag);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Sends an output to the logging subsystem, giving the exact amount of characters to send.
    This is intended for very large, pre-formatted strings (1 KiB and more), such as JSON messages.
    NEW in 6.00.

    \return             Zero if the log operation was successful; an implementation-specific error
                        code otherwise. This error code is for debugging purposes only. Do not use
                        this error code to make decisions at game runtime in middleware code. If
                        the log has failed, move along. Note that if the log is filtered by the
                        implementation, it is still considered a success.
    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param Level        The level of importance of the output, see \ref eal_log_levels for valid
                        values. We strongly discourage combining multiple level flags together.
    \param Reserved     Reserved for future use; must be 0 for now.
    \param pMessage     A valid string containing the message to print. Can be a dynamic string.
                        String can contain the '\n' character for EOL.
                        String does not have to be null-terminated but must contain at least the
                        number of valid characters as indicated by MessageSize.
    \param MessageSize  The number of characters from pMessage to output. It can be assumed that no
                        null-termination character '\0' is present before (pMessage + MessageSize)
                        is reached.
    \param pFile        A string containing the path of the source file where this log was issued
                        (optional; can be NULL).
    \param Line         The line number of the log line in the source file where this log was issued
                        (optional; can be 0).

    \PRECONDITIONS
    - pMessage cannot be NULL
    - pMessage can be empty (""), in which case MessageSize must be zero
    - pMessage contains at least the number of valid characters as indicated by MessageSize

    \POSTCONDITIONS
    - If pMessage contains a null-termination character '\0' before (pMessage + MessageSize) is reached, the behavior is undefined

    \sa EalLogOutput, EalLogOutputWithFormatVa, EalLogIsEnabled
*/
EAL_DLL_ENTRY eal_s32 EalLogOutputWithSize(eal_id          Tag,
                                           eal_log_level   Level,
                                           eal_u64         Reserved,
                                           const char*     pMessage,
                                           eal_size_t      MessageSize,
                                           const char*     pFile,
                                           eal_u32         Line);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL Log functions.
struct eal_log_dll_interface
{
    bool (*pEalLogLibInit)              (eal_u32            Version);
    void (*pEalLogLibShutdown)          ();
    void (*pEalLogOutput)               (eal_id             Tag,
                                        eal_log_level       Level,
                                        const char*         pMessage,
                                        const char*         pFile,
                                        eal_u32             Line);

    // BEGIN DEPRECATED in V200
    bool (*pEalLogAssert)               (eal_id             Tag,
                                        const char*         pCondition,
                                        const char*         pMessage,
                                        const char*         pFile,
                                        eal_u32             Line,
                                        eal_u32*            pStatic);
    // END DEPRECATED

    bool (*pEalLogIsEnabled)            (eal_id             Tag,
                                        eal_log_level       Level);
    bool (*pEalLogAssert2)              (eal_id             Tag,
                                        const char*         pCondition,
                                        const char*         pMessage,
                                        const char*         pFile,
                                        eal_u32             Line,
                                        eal_log_assert_opt  Options,
                                        eal_u32*            pStatic);
    void (*pEalLogOutputWithFormatVa)   (eal_id             Tag,
                                        eal_log_level       Level,
                                        const char*         pFile,
                                        eal_u32             Line,
                                        const char*         pFormat,
                                        va_list             Args);
    void (*pEalLogSetTagName)           (eal_id             Tag,
                                        const char*         pTagName);
    const char* (*pEalLogGetTagName)    (eal_id             Tag);
    eal_s32 (*pEalLogOutputWithSize)    (eal_id             Tag,
                                        eal_log_level       Level,
                                        eal_u64             Reserved,
                                        const char*         pMessage,
                                        eal_size_t          MessageSize,
                                        const char*         pFile,
                                        eal_u32             Line);
};

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initialization code and must be done before any
    call to any EAL function.
    \param Interface    Structure containing valid pointers to the engine EAL functions.
    \sa EalLogDllPopulateInterface
*/
void EalLogDllInitInterface(const eal_log_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the engine side. It is used to fill up the Interface structure.
    This function is provided by the ealdll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so it will dynamically connect its internal EAL
    function calls to the one in your engine.
    \param Interface    A structure that will be filled up with pointers to local EAL functions.
    \sa EalFileDllMemInterface
*/
void EalLogDllPopulateInterface(eal_log_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */
