#pragma once

#include "core/Macros.h"

namespace ITF
{

// Describes an error from the engine online stack.
class OnlineError
{
public:

    // The high level type of an OnlineError.
    // This gives information about the origin of the error.
    enum ErrorType
    {
        Invalid,        // Internal use only: used to construct an OnlineError without parameters (transient state)
        Success,        // Useful to return from network APIs that they succeeded, in fact.

        Network,        // Network connection problem (e.g., disconnected cable, no IP)
        FirstParty,     // First Party issue (e.g., invalid account)
        UbiServer,      // Ubisoft Server issue (e.g., wrong config, maintenance)
        Timeout,        // Unknown exact source (e.g., slow connection)
        Cancelled,      // Online request was canceled

        ErrorTypeCount  // Internal use: keep it last my friend
    };

    // Detailed information for an ErrorType.
    // This gives additional information about the error type.
    enum ErrorSubtype
    {
        None,                   // No specific subtype

        Maintenance,            // Ubisoft servers under maintenance / FeatureSwitches are off
        NotInitialized,         // APIs cannot be used yet; missing inits
        Authentication,         // Ubiservices auth errors
        Connect,                // Connect launching errors
        News,                   // Errors in fetching news
        ErrorSubtypeCount       // KEEP IT LAST (Internal use)
    };


    // -------------------------------------------------------------------------

    OnlineError()
        : m_type(ErrorType::Invalid),
          m_subtype(ErrorSubtype::None),
          m_code(0)
    {
    }

    OnlineError(ErrorType _type, u32 _code = 0)
        : OnlineError(_type, ErrorSubtype::None, _code)
    {
    }

    explicit OnlineError(ErrorType _type, ErrorSubtype _subtype, u32 _code = 0)
        : m_type(_type),
          m_subtype(_subtype),
          m_code(_code)
    {
        ITF_ASSERT_CRASH(_type != Invalid, "OnlineError::Invalid shouldn't be used directly (transient state)");

        ITF_ASSERT_CRASH((_subtype == Maintenance && _type == UbiServer) || _subtype != Maintenance, "Maintenance is a subtype of UbiServer only");

#ifdef ITF_ORBIS
        // Make sure we always have an error code for first party PS4 errors
        ITF_ASSERT_CRASH((_type != FirstParty) || (_code != 0), "No Error code set for low-level PS4 online error!");
#endif
    }

    bool operator==(const OnlineError &_rhs) const
    {
        return (m_type == _rhs.m_type) && (m_subtype == _rhs.m_subtype) && (m_code == _rhs.m_code);
    }

    ErrorType getType() const { return m_type; }
    ErrorSubtype getSubType() const { return m_subtype; }
    u32 getCode() const { return m_code; }

    String8 toString() const;

    static String8 typeToString(OnlineError::ErrorType _type);
    static String8 subtypeToString(OnlineError::ErrorSubtype _subtype);


private:
    ErrorType m_type;
    ErrorSubtype m_subtype;
    u32 m_code; // Platform specific error code
};


} // namespace ITF
