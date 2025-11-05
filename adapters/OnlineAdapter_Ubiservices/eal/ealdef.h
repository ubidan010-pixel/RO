////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealdef.h  Engine Abstraction Layer Reference API Definitions
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This file contains definitions used by all EAL API files.
//
// Look at the end of the file for documentation on the whole EAL API.
//
// "Simplicity is the ultimate sophistication" - Leonardo Da Vinci
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

/// This module covers the reference API definitions.
/*! \addtogroup EAL
    @{
*/

/// EalDef file version. The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210)
/// See "How to use the library" in the documentation below for more information.
#define EALDEF_VERSION              1454

/// EAL_DLL_IMPLICIT must be defined by the client who requires an implicit and dynamic
/// DLL function linkage with the EAL implementation. Typically not for TG products.
#if !defined(EAL_DLL_ENTRY) && defined(EAL_DLL_IMPLICIT)
#   if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)
#       if defined(_MSC_VER)
#           define EAL_DLL_ENTRY __declspec(dllimport)                      // Microsoft
#       endif
#   else
#       if defined(_MSC_VER)
#           define EAL_DLL_ENTRY __declspec(dllexport)                      // Microsoft
#       elif defined(_GCC)
#           define EAL_DLL_ENTRY __attribute__((visibility("default")))     //  GCC
#       endif
#   endif
#endif

#if !defined(EAL_DLL_ENTRY)
#   define EAL_DLL_ENTRY                                                    // Using explicit linkage
#endif

/// Deprecation attribute
#if defined(EAL_ENABLE_DEPRECATION_WARNINGS)
#   if defined(__clang__) // clang
#       if (__cplusplus >= 201402L) // C++14
#           define EAL_DEPRECATED(...)      [[deprecated(__VA_ARGS__)]]
#       elif __has_extension( attribute_deprecated_with_message )
#           define EAL_DEPRECATED(...)      __attribute__ ((deprecated(__VA_ARGS__)))
#       else
#           define EAL_DEPRECATED(...)      __attribute__ ((deprecated))
#       endif
#   elif defined(_MSC_VER) // msvc
#       if (_MSC_VER >= 1920) && (_MSVC_LANG >= 201402L)
#           define EAL_DEPRECATED(...)      [[deprecated(__VA_ARGS__)]]
#       elif (_MSC_VER >= 1900)
#           define EAL_DEPRECATED(...)      __declspec(deprecated(__VA_ARGS__))
#       else
#           define EAL_DEPRECATED(...)
#       endif
#   elif defined(__GNUC__) // gcc
#       if (__cplusplus >= 201402L) // C++14
#           define EAL_DEPRECATED(...)      [[deprecated(__VA_ARGS__)]]
#       else
#           define EAL_DEPRECATED(...)      __attribute__ ((deprecated))
#       endif
#   else
#       define EAL_DEPRECATED(...)
#   endif
#else
#   define EAL_DEPRECATED(...)
#endif

/// EAL unique ID
typedef int eal_id;                 ///< EAL Id (tag) built with eal_constants values

/// List of EAL constants
enum eal_constants : eal_id
{
    EAL_ID_VOID                     = eal_id(0xFFFFFFFF),                                   ///< Unassigned / void ID
    EAL_ID_TYPE_MASK                = eal_id(0xC0000000),                                   ///< Mask - ID type
    EAL_ID_OWNER_MASK               = 0x3FF00000,                                           ///< Mask - ID owner
    EAL_ID_PRIVATE_MASK             = 0x000FFFFF,                                           ///< Mask - ID private section

    EAL_ID_TYPE_SHIFT               = 30,                                                   ///< Shift - ID type
    EAL_ID_OWNER_SHIFT              = 20,                                                   ///< Shift - ID owner
    EAL_ID_PRIVATE_SHIFT            = 0,                                                    ///< Shift - ID private section

    EAL_ID_TYPE_ENGINE              = (0 << EAL_ID_TYPE_SHIFT),                             ///< ID type for game engines
    EAL_ID_TYPE_TG                  = (1 << EAL_ID_TYPE_SHIFT),                             ///< ID type for TG and Harbour middleware products
    EAL_ID_TYPE_THIRDPARTY          = (2 << EAL_ID_TYPE_SHIFT),                             ///< ID type for third-party products
    EAL_ID_TYPE_INNERSOURCE         = (3 << EAL_ID_TYPE_SHIFT),                             ///< ID type for inner source projects

    EAL_ID_ENGINE_GENERIC           = (EAL_ID_TYPE_ENGINE | (0 << EAL_ID_OWNER_SHIFT)),     ///< Generic engine ID
    EAL_ID_ENGINE_SCIMITAR          = (EAL_ID_TYPE_ENGINE | (1 << EAL_ID_OWNER_SHIFT)),     ///< Scimitar engine ID
    EAL_ID_ENGINE_BLACKSMITH        = (EAL_ID_TYPE_ENGINE | (2 << EAL_ID_OWNER_SHIFT)),     ///< Blacksmith engine ID
    EAL_ID_ENGINE_DUNIA             = (EAL_ID_TYPE_ENGINE | (3 << EAL_ID_OWNER_SHIFT)),     ///< Dunia engine ID
    EAL_ID_ENGINE_YETI              = (EAL_ID_TYPE_ENGINE | (4 << EAL_ID_OWNER_SHIFT)),     ///< Yeti engine ID
    EAL_ID_ENGINE_LYN               = (EAL_ID_TYPE_ENGINE | (5 << EAL_ID_OWNER_SHIFT)),     ///< LYN engine ID
    EAL_ID_ENGINE_ONYX              = (EAL_ID_TYPE_ENGINE | (6 << EAL_ID_OWNER_SHIFT)),     ///< Onyx engine ID
    EAL_ID_ENGINE_SNOWDROP          = (EAL_ID_TYPE_ENGINE | (7 << EAL_ID_OWNER_SHIFT)),     ///< Snowdrop engine ID
    EAL_ID_ENGINE_MOBILETHEATER     = (EAL_ID_TYPE_ENGINE | (8 << EAL_ID_OWNER_SHIFT)),     ///< Mobile theater engine ID
    EAL_ID_ENGINE_VOYAGER           = (EAL_ID_TYPE_ENGINE | (9 << EAL_ID_OWNER_SHIFT)),     ///< Voyager engine ID

    //
    // IMPORTANT: DO NOT ADD ANY ID IF YOU ARE NOT MEMBER OF THE EAL COMMITTEE.
    // If you need an ID, please send a request to World-EALCommittee@ubisoft.com
    //

    EAL_ID_TG_GENERIC               = (EAL_ID_TYPE_TG | ( 0 << EAL_ID_OWNER_SHIFT)),        ///< Generic TG or Harbour product ID
    EAL_ID_TG_GEAR                  = (EAL_ID_TYPE_TG | ( 1 << EAL_ID_OWNER_SHIFT)),        ///< TG Gear product ID
    EAL_ID_TG_DARE                  = (EAL_ID_TYPE_TG | ( 2 << EAL_ID_OWNER_SHIFT)),        ///< TG Dare product ID
    EAL_ID_TG_FIRE                  = (EAL_ID_TYPE_TG | ( 3 << EAL_ID_OWNER_SHIFT)),        ///< TG Fire product ID
    EAL_ID_TG_REALTREE              = (EAL_ID_TYPE_TG | ( 4 << EAL_ID_OWNER_SHIFT)),        ///< TG RealTree product ID
    EAL_ID_TG_REALBLAST             = (EAL_ID_TYPE_TG | ( 5 << EAL_ID_OWNER_SHIFT)),        ///< TG RealBlast product ID
    EAL_ID_TG_OASIS                 = (EAL_ID_TYPE_TG | ( 6 << EAL_ID_OWNER_SHIFT)),        ///< TG Oasis product ID
    EAL_ID_TG_ANIMATION             = (EAL_ID_TYPE_TG | ( 7 << EAL_ID_OWNER_SHIFT)),        ///< TG Animation product ID
    EAL_ID_TG_METRIX                = (EAL_ID_TYPE_TG | ( 8 << EAL_ID_OWNER_SHIFT)),        ///< TG Metrix product ID
    EAL_ID_TG_ORBIT                 = (EAL_ID_TYPE_TG | ( 9 << EAL_ID_OWNER_SHIFT)),        ///< TG Orbit product ID
    EAL_ID_TG_PASSENGER             = (EAL_ID_TYPE_TG | (10 << EAL_ID_OWNER_SHIFT)),        ///< TG Passenger product ID
    EAL_ID_TG_THEATER               = (EAL_ID_TYPE_TG | (11 << EAL_ID_OWNER_SHIFT)),        ///< TG Theater product ID
    EAL_ID_TG_UBISERVICES           = (EAL_ID_TYPE_TG | (12 << EAL_ID_OWNER_SHIFT)),        ///< Harbour UbiServices product ID
    EAL_ID_TG_GATOR                 = (EAL_ID_TYPE_TG | (13 << EAL_ID_OWNER_SHIFT)),        ///< TG Gator product ID
    EAL_ID_TG_NETZ                  = (EAL_ID_TYPE_TG | (14 << EAL_ID_OWNER_SHIFT)),        ///< TG NetZ product ID
    EAL_ID_TG_BLOOMBERG             = (EAL_ID_TYPE_TG | (15 << EAL_ID_OWNER_SHIFT)),        ///< TG Bloomberg product ID
    EAL_ID_TG_PILOT                 = (EAL_ID_TYPE_TG | (16 << EAL_ID_OWNER_SHIFT)),        ///< TG Pilot product ID
    EAL_ID_TG_HERMES                = (EAL_ID_TYPE_TG | (17 << EAL_ID_OWNER_SHIFT)),        ///< TG Hermes product ID
    EAL_ID_TG_ECHO                  = (EAL_ID_TYPE_TG | (18 << EAL_ID_OWNER_SHIFT)),        ///< TG Echo product ID
    EAL_ID_TG_MOTION                = (EAL_ID_TYPE_TG | (19 << EAL_ID_OWNER_SHIFT)),        ///< TG Motion product ID
    EAL_ID_TG_EVIL                  = (EAL_ID_TYPE_TG | (20 << EAL_ID_OWNER_SHIFT)),        ///< TG Evil product ID
    EAL_ID_TG_RDV                   = (EAL_ID_TYPE_TG | (21 << EAL_ID_OWNER_SHIFT)),        ///< Harbour Rendez Vous product ID
    EAL_ID_TG_UPLAY                 = (EAL_ID_TYPE_TG | (22 << EAL_ID_OWNER_SHIFT)),        ///< TG UPlay product ID
    EAL_ID_TG_GRAFFITI              = (EAL_ID_TYPE_TG | (23 << EAL_ID_OWNER_SHIFT)),        ///< TG Graffiti product ID
    EAL_ID_TG_PHOENIX               = (EAL_ID_TYPE_TG | (24 << EAL_ID_OWNER_SHIFT)),        ///< TG Phoenix product ID
    EAL_ID_TG_SCRY                  = (EAL_ID_TYPE_TG | (25 << EAL_ID_OWNER_SHIFT)),        ///< TG Scry product ID
    EAL_ID_TG_ATOM                  = (EAL_ID_TYPE_TG | (26 << EAL_ID_OWNER_SHIFT)),        ///< TG Atom product ID
    EAL_ID_TG_TG2D                  = (EAL_ID_TYPE_TG | (27 << EAL_ID_OWNER_SHIFT)),        ///< TG 2D-renderer product ID
    EAL_ID_TG_FAPROFILER            = (EAL_ID_TYPE_TG | (28 << EAL_ID_OWNER_SHIFT)),        ///< TG Flying Architects Profiler product ID
    EAL_ID_TG_HELIX                 = (EAL_ID_TYPE_TG | (29 << EAL_ID_OWNER_SHIFT)),        ///< TG Helix product ID
    EAL_ID_TG_FATRANSPORT           = (EAL_ID_TYPE_TG | (30 << EAL_ID_OWNER_SHIFT)),        ///< TG Flying Architects Transport product ID
    EAL_ID_TG_REFLEX                = (EAL_ID_TYPE_TG | (31 << EAL_ID_OWNER_SHIFT)),        ///< TG Reflex product ID
    EAL_ID_TG_GDT                   = (EAL_ID_TYPE_TG | (32 << EAL_ID_OWNER_SHIFT)),        ///< TG Gear Data Transport product ID
    EAL_ID_TG_DTEST                 = (EAL_ID_TYPE_TG | (33 << EAL_ID_OWNER_SHIFT)),        ///< TG DTest product ID (mailto:DTest-TG-Support@ubisoft.com)
    EAL_ID_TG_KINO                  = (EAL_ID_TYPE_TG | (34 << EAL_ID_OWNER_SHIFT)),        ///< Anvil Kino product ID
    EAL_ID_TGO_COMMON               = (EAL_ID_TYPE_TG | (35 << EAL_ID_OWNER_SHIFT)),        ///< Harbour common product ID
    EAL_ID_TG_ZEN                   = (EAL_ID_TYPE_TG | (36 << EAL_ID_OWNER_SHIFT)),        ///< Zen product ID (mailto:WW-ZenSupport@ubisoft.com)
    EAL_ID_TG_FACEBUILDER           = (EAL_ID_TYPE_TG | (37 << EAL_ID_OWNER_SHIFT)),        ///< TG FaceBuilder product ID
    EAL_ID_TG_DISSECT               = (EAL_ID_TYPE_TG | (38 << EAL_ID_OWNER_SHIFT)),        ///< Dissect product ID (mailto:World-Dissect2Dev@ubisoft.com)
    EAL_ID_TG_BABEL                 = (EAL_ID_TYPE_TG | (39 << EAL_ID_OWNER_SHIFT)),        ///< Babel product ID (mailto:jessy.gosselin-grant@ubisoft.com)
    EAL_ID_TG_FUSION                = (EAL_ID_TYPE_TG | (40 << EAL_ID_OWNER_SHIFT)),        ///< Fusion product ID (mailto:michel.bouchard@ubisoft.com)
    EAL_ID_TG_SILEXNET              = (EAL_ID_TYPE_TG | (41 << EAL_ID_OWNER_SHIFT)),        ///< Silex-Net product ID (mailto:thomas.minet@ubisoft.com)
    EAL_ID_TG_VANGUARD              = (EAL_ID_TYPE_TG | (42 << EAL_ID_OWNER_SHIFT)),        ///< Harbour Vanguard product ID (mailto:eric.fortin@ubisoft.com)
    EAL_ID_TG_SMOTION               = (EAL_ID_TYPE_TG | (43 << EAL_ID_OWNER_SHIFT)),        ///< TG SMotion product ID (mailto:Reflex-TG-Support@ubisoft.com)
    EAL_ID_TG_TELEMETRY             = (EAL_ID_TYPE_TG | (44 << EAL_ID_OWNER_SHIFT)),        ///< TG Telemetry product ID (mailto:Telemetry-TG-Support@ubisoft.com)
    EAL_ID_TG_UDCC                  = (EAL_ID_TYPE_TG | (45 << EAL_ID_OWNER_SHIFT)),        ///< TG UDCC product ID (mailto:UDCC-TG-Support@ubisoft.com)
    EAL_ID_TG_TGDPCLIENT            = (EAL_ID_TYPE_TG | (46 << EAL_ID_OWNER_SHIFT)),        ///< TGDP Client product ID (mailto:vincent.dore-millet@ubisoft.com)
    EAL_ID_TG_IKTOOLKIT             = (EAL_ID_TYPE_TG | (47 << EAL_ID_OWNER_SHIFT)),        ///< IKToolkit/BionIK product ID (mailto:BionIK-TG-Support@ubisoft.com)
    EAL_ID_TG_RAVEN                 = (EAL_ID_TYPE_TG | (48 << EAL_ID_OWNER_SHIFT)),        ///< Raven product ID (mailto:TBD)

    //
    // IMPORTANT: DO NOT ADD ANY ID IF YOU ARE NOT MEMBER OF THE EAL COMMITTEE.
    // If you need an ID, please send a request to World-EALCommittee@ubisoft.com
    //

    EAL_ID_THIRDPARTY_GENERIC       = (EAL_ID_TYPE_THIRDPARTY | (0 << EAL_ID_OWNER_SHIFT)), ///< Generic third-party product ID
    EAL_ID_THIRDPARTY_SDK           = (EAL_ID_TYPE_THIRDPARTY | (1 << EAL_ID_OWNER_SHIFT)), ///< System SDK product ID
    EAL_ID_THIRDPARTY_BINK          = (EAL_ID_TYPE_THIRDPARTY | (2 << EAL_ID_OWNER_SHIFT)), ///< Bink product ID
    EAL_ID_THIRDPARTY_HAVOK         = (EAL_ID_TYPE_THIRDPARTY | (3 << EAL_ID_OWNER_SHIFT)), ///< Havok product ID
    EAL_ID_THIRDPARTY_ITNETWORK     = (EAL_ID_TYPE_THIRDPARTY | (4 << EAL_ID_OWNER_SHIFT)), ///< ITNetwork product ID
    EAL_ID_THIRDPARTY_TINYXML2      = (EAL_ID_TYPE_THIRDPARTY | (5 << EAL_ID_OWNER_SHIFT)), ///< TinyXML-2 product ID (https://gitlab-ncsa.ubisoft.org/tinyxml2/tinyxml2)
    EAL_ID_THIRDPARTY_LZ4           = (EAL_ID_TYPE_THIRDPARTY | (6 << EAL_ID_OWNER_SHIFT)), ///< LZ4 product ID (https://gitlab-ncsa.ubisoft.org/lz4/lz4)
    EAL_ID_THIRDPARTY_IMGUI         = (EAL_ID_TYPE_THIRDPARTY | (7 << EAL_ID_OWNER_SHIFT)), ///< ImGui (through EAL glue code for Odyssey engine) product ID (https://gitlab-ncsa.ubisoft.org/Odyssey/Engine/Extern/ImGui)

    //
    // IMPORTANT: DO NOT ADD ANY ID IF YOU ARE NOT MEMBER OF THE EAL COMMITTEE.
    // If you need an ID, please send a request to World-EALCommittee@ubisoft.com
    //

    EAL_ID_INNERSOURCE_GENERIC      = (EAL_ID_TYPE_INNERSOURCE | (0 << EAL_ID_OWNER_SHIFT)), ///< Generic inner source project ID
    EAL_ID_INNERSOURCE_AITOOLKIT    = (EAL_ID_TYPE_INNERSOURCE | (1 << EAL_ID_OWNER_SHIFT)), ///< AI Toolkit project ID (mailto:TG-MTLMLFramework@ubisoft.com)
    EAL_ID_INNERSOURCE_USF          = (EAL_ID_TYPE_INNERSOURCE | (2 << EAL_ID_OWNER_SHIFT)), ///< Unified Sound Framework project ID (mailto:UnifiedSoundFramework@ubisoft.com)
    EAL_ID_INNERSOURCE_DRIVE        = (EAL_ID_TYPE_INNERSOURCE | (3 << EAL_ID_OWNER_SHIFT)), ///< Drive project ID (mailto:DriveMiddleware@ubisoft.com)
    EAL_ID_INNERSOURCE_CHOREOGRAPH  = (EAL_ID_TYPE_INNERSOURCE | (4 << EAL_ID_OWNER_SHIFT)), ///< Choreograph (La Forge) project ID (mailto:daniel.holden@ubisoft.com)
    EAL_ID_INNERSOURCE_MLFRAMEWORK  = (EAL_ID_TYPE_INNERSOURCE | (5 << EAL_ID_OWNER_SHIFT)), ///< Machine Learning Framework project ID (mailto:TG-MTLMLFramework@ubisoft.com)
    EAL_ID_INNERSOURCE_RUNN         = (EAL_ID_TYPE_INNERSOURCE | (6 << EAL_ID_OWNER_SHIFT)), ///< Runtime Neural Network project ID (mailto:TG-MTLMLFramework@ubisoft.com)
    EAL_ID_INNERSOURCE_NETCODE      = (EAL_ID_TYPE_INNERSOURCE | (7 << EAL_ID_OWNER_SHIFT)), ///< Netcode project ID (mailto:kris.stasik@ubisoft.com)
    EAL_ID_INNERSOURCE_BARBERSHOP   = (EAL_ID_TYPE_INNERSOURCE | (8 << EAL_ID_OWNER_SHIFT)), ///< Barbershop project ID (mailto:Barbershop-Support@ubisoft.com)
    EAL_ID_INNERSOURCE_AEGIR        = (EAL_ID_TYPE_INNERSOURCE | (9 << EAL_ID_OWNER_SHIFT)), ///< Aegir project ID (mailto:UbisoftConnect-AegirTeam@ubisoft.com)

    //
    // IMPORTANT: DO NOT ADD ANY ID IF YOU ARE NOT MEMBER OF THE EAL COMMITTEE.
    // If you need an ID, please send a request to World-EALCommittee@ubisoft.com
    //
};

/// Timeout value in ms
enum eal_timeout
{
    EAL_NO_WAIT                     = 0,          ///< No timeout
    EAL_INFINITE_WAIT               = 0xffffffff  ///< Wait forever
};

#include <cstddef>
#include <stdint.h> // All current toolchains provide <stdint.h> although some are not fully C99 compliant; <cstdint> requires C++11 support

// Define EAL_USE_STDINT_INT_TYPES unless compiling using Visual C++
#if ( ( !defined(_MSC_VER) ) || defined( __clang__ ) ) && !defined( EAL_USE_STDINT_INT_TYPES )
#   define EAL_USE_STDINT_INT_TYPES
#endif

#ifdef EAL_USE_STDINT_INT_TYPES

// From <stdint.h>
typedef ::int8_t            eal_s8;         ///<  8-bit signed integer
typedef ::uint8_t           eal_u8;         ///<  8-bit unsigned integer
typedef ::int16_t           eal_s16;        ///< 16-bit signed integer
typedef ::uint16_t          eal_u16;        ///< 16-bit unsigned integer
typedef ::int32_t           eal_s32;        ///< 32-bit signed integer
typedef ::uint32_t          eal_u32;        ///< 32-bit unsigned integer
typedef ::int64_t           eal_s64;        ///< 64-bit signed integer
typedef ::uint64_t          eal_u64;        ///< 64-bit unsigned integer

#else

typedef signed char         eal_s8;         ///<  8-bit signed integer
typedef unsigned char       eal_u8;         ///<  8-bit unsigned integer
typedef signed short        eal_s16;        ///< 16-bit signed integer
typedef unsigned short      eal_u16;        ///< 16-bit unsigned integer
typedef signed long         eal_s32;        ///< 32-bit signed integer
typedef unsigned long       eal_u32;        ///< 32-bit unsigned integer
typedef signed long long    eal_s64;        ///< 64-bit signed integer
typedef unsigned long long  eal_u64;        ///< 64-bit unsigned integer

#endif //EAL_USE_STDINT_INT_TYPES

// From <cstddef>
typedef std::size_t         eal_size_t;     ///< Unsigned integer type of the result of the sizeof operator and the alignof operator (used for memory size parameters)
typedef std::ptrdiff_t      eal_ptrdiff_t;  ///< Signed integer type of the result of subtracting two pointers (used for memory offset parameters)

// From <stdint.h>
typedef ::intptr_t          eal_intptr_t;   ///< Signed integer type capable of holding a pointer
typedef ::uintptr_t         eal_uintptr_t;  ///< Unsigned integer type capable of holding a pointer

/*! @} */

/**

\page overview Overview
\brief The basics to know about the Engine Abstraction Layer.

 - \subpage goals --- \copybrief goals
 - \subpage contract --- \copybrief contract
 - \subpage client --- \copybrief client
 - \subpage dll --- \copybrief dll
 - \subpage uuid --- \copybrief uuid
 - \subpage supplier --- \copybrief supplier
 - \subpage conventions --- \copybrief conventions

\page contract Contract Programming
\brief Brief overview of contract programming.

 Design by Contract (DbC) or Programming by Contract is an approach to designing computer
 software. It prescribes that software designers should define formal, precise, and verifiable
 interface specifications for software components, which extend the ordinary definition of
 abstract data types with preconditions, postconditions, and invariants. These specifications
 are referred to as "contracts", in accordance with a conceptual metaphor with the conditions
 and obligations of business contracts.

 The term was coined by Bertrand Meyer in connection with his design of the Eiffel language.
 Programming by contract creates a contract between the software developer and software user -
 in Meyer's terms the supplier and the client. Every feature, or method, starts with a
 precondition that must be satisfied by the client of the routine. And each feature ends with
 postconditions which the supplier guarantees to be true (if and only if the preconditions
 were met).


 \page goals Design Goals & Philosophy
 \brief Description of the design goals of the library.

 - The first goal of the EAL is to provide a standard connection point between middleware and
 the game engine. EAL covers the majority of services that are typically managed by the game
 engine but needed by middleware, such as file access, memory allocation, logging, and so on.\n
 Included in this goal is the desire to be able to connect any mix of targets (debug/retail/etc.)
 - The second goal is to create an API that offers the lowest level of services that cover the
 common denominator between all engines. This also ensures minimal work for supplying an EAL
 for any engine. It is expected that a client will create a more user-friendly layer on top of
 EAL.
 - The third goal is to ensure that the client provides to the engine a rich set of
 information when calling a function. This helps in two ways: first, it helps improve global
 automated management of resources such as memory and CPU; second, this information can also
 be routed to tools. The best example of this concept is the inclusion of tags for almost every
 resource used in the EAL API. With tags, the implementation can always identify which
 middleware is requesting the resource. This can be used, for example, to keep track of all the
 memory used by <a HREF="http://dare" target="_blank">Dare</a>. It can also be used, for example, to ensure all
 jobs created for <a HREF="http://fire" target="_blank">Fire</a> are running on a specific core.

 \page client How to use the library
 \brief Description of how to use the EAL library.

 - You must always call \c EalxxxLibInit() before calling any other function in the library
  (exception made in DLLs), where \c xxx represents the API (Mem, File, Log, and so on).
   If your call to the initialization function fails, it probably means that the supplier does
   NOT support the EAL API version you are using.
   If the client supports a higher version than you are using it will work.
 - EAL is backward compatible up to 3 Major versions. Normally, a supplier is expected to provide
   support for the last 3 Major versions. Minor versions cannot change the interface, they are
   mostly documentation changes.
 - You are allowed to call the initialization  function as many times as you want.
   You should call the same number of shutdowns upon exiting.
 - All calls are thread-safe
 - We recommend using EAL IDs all the time, even in retail, since they have a minimum weight of
   32 bits, yet can still be VERY useful to find problems in retail versions.
 - The term 'NULL' used in comments literally means '0'.
 - The term 'static' in parameter comments refers to variables that stay at the same memory
   location for the whole duration of the program execution. It allows EAL providers to not copy
   the data and simply keep a pointer or reference to the parameter. The typical case is for
   debug strings.
 - The term 'valid' in parameter comments means data properly allocated and initialized. In the
   case of pointers, NULL is not accepted.

 \page supplier How to supply an EAL implementation
 \brief Information on how to supply (implement) EAL services in an engine.

 - Be sure to count the number of times the initialization function is called. Only uninitialize
   when count of calls to Shutdown() is the same.
 - In your implementation of an EAL library, it may happen that you have circular dependencies
   between two modules. A typical case is memory and performance logging: the module must have a
   "2-step" initialization. For the first part you initialize the internals of your library, so
   it is ready to accept calls to its API. Then, for the second part, you initialize sub EAL
   modules. See example code below.
 - You must implement the whole interface, including debug functions, even if you are compiling
   in retail. At some point, you may end up linking a library compiled in debug or release with
   your retail engine for debugging a specific problem.
 - The whole API can be called by many different middlewares in different thread contexts.
   So, all your API should be thread-safe (including the initialization function calls).


 \page dll Using EAL in a DLL
 \brief Information on how to use EAL inside a DLL.

 If you are writing code that is using EAL and that will be in a separate DLL, you will need to
 do the following:
 - Include in your DLL project all the \em ealxxxdll.cpp files corresponding to the services you are
   using.
 - Define \c EAL_IMPORT_DLL in your project.
   \deprecated EAL_DLL is DEPRECATED since version 4.00. Please use EAL_IMPORT_DLL instead.
   EAL_DLL will be removed in version 7.00.
 - Create a DLL initialization function that gets EAL interfaces (\c eal_xxx_dll_interface) as
   found in \em ealxxx.h.
 - Call the \c EalxxDllInitInterface() with that interface.
 - Call the EAL functions as usual in your code, including a call to \c EalxxxLibInit().

 On the engine side:
 - Include in your project the files \em ealxxxdll.cpp corresponding to the services you are
   using.
 - Call \c EalxxxDllPopulateInterface() to populate an interface (\c eal_xxx_dll_interface).
 - Initiate the DLL and provide the interfaces.

 How does it work?
 Really simple. On the DLL side, \em ealxxxdll.cpp contains a global variable of type
 \c eal_xxx_dll_interface. This variable contains a function pointer to all EAL functions.
 When you call \c EalxxxDllInitInterface(), all function pointers are initalized.
 The DLL side also contains code for every EAL function, which simply redirects the call to the
 function pointer.
 The code on the engine side (\c EalxxxDllPopulateInterface) simply fills up the interface
 with the address of all the supplied EAL functions.

 \page uuid EAL IDs
 \brief Description of the EAL Identifier System used through the EAL API.

 EAL IDs are used to identify values for a specific Ubisoft product or engine.
 These IDs, used for all APIs that require tags (such as \c EalMem, \c EalJob, and others),
 serve many purposes. They provide detailed information regarding the origin of the
 calls made to EAL, allowing the implementers to perform specific actions in some cases.
 They are also very valuable for debugging and logging tools.

 Note that all API that requires EAL IDs share tags. If you create a thread with an id,
 this id could be carried up to the memory API, if the implementer of the thread API needs
 to allocate memory for example.
 We highly recommend you associate all the tag you are using with a human readable version
 using EalLogSetTagName in EalLog API. You can retrieve a name using EalLogGetTagName.

 \par TAG Format

 - <b>Bits 30-31</b>: Used to identify the type of product (range 0-3).
    - \c 0x00 ---  Game Engine
    - \c 0x01 ---  Ubisoft internal TG product (or other internal middleware)
    - \c 0x02 ---  Third-party middleware (such as Bink)
    - \c 0x03 ---  Reserved for future use
 - <b>Bits 20-29</b>: Used to identify the product/engine/third-party. The IDs are attributed by the TG.
 If your engine/product/third-party product is not on the list, please contact the TG at
 <a href="mailto:World-EALCommittee@ubisoft.com?subject=Need an ID">World-EALCommittee@ubisoft.com</a> to get an ID.
 - <b>Bits 0-19</b>: Free to use by the product owner. The owner is responsible for managing these bits.

 ID examples:
 \code
 #define FIRE_MEM_VERTEX_BUFFER      (EAL_ID_TG_FIRE | 0x00000001)
 #define FIRE_MEM_TEXTURE            (EAL_ID_TG_FIRE | 0x00000002)
 #define FIRE_MEM_AC_BINARY          (EAL_ID_TG_FIRE | 0x00000003)
 And so on.
 \endcode

 \page conventions Library Conventions
 \brief Programming convention to use for anyone expanding the EAL.

 This section is dedicated to people who would like to extend the EAL library.
 - Line length should not exceed 100 characters.
 - All filenames are in lower case.
 - Do not use default parameters in EAL functions. The only exception to this rule is the
   initialization function because we need the version the client is using.
 - Do not change an existing define/enum value. You can only add new ones.
 - All functions must be implemented in all targets (retail/release/debug). It is possible with
   EAL to use a debug version of a middleware, even though the engine is compiled in release or
   retail. In that case, you will need to link with the debug version of the functions.
 - The version of the library is included in the initialization function.
 - Backward compatibility is preserved for 3 versions, hence any function that is declared
   deprecated must stay for 3 versions. After 3 versions, it can be removed.
 - You CANNOT remove any entry in the DLL interface structure. Deprecated function entries can
   be recycled 3 versions after they were declared deprecated, but they can never be removed.
 - When adding new middleware, engines, and so on, a new version is not mandatory. The essential
   is that the value used is unique and reserved by the EAL team.
 - Each API is independent (such as ealmem and ealfile). Their version can evolve at different
   paces, hence they may be of a different version.
 - In order to deprecate a function, you simply need to add the Doxygen deprecated keyword in
   front of the function comment with the version number in which it became deprecated.
   The function must remain in the library as a deprecated one for at least 3 versions before
   being removed.
**/
