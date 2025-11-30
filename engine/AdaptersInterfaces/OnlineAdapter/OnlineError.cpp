#include "precompiled_engine.h"

#include "engine/AdaptersInterfaces/OnlineAdapter/OnlineError.h"

namespace ITF
{

namespace
{
    const char* type_strings[] =
    {
        "LOGIC BUG",
        "Success",
        "Network Error",
        "First Party Error",
        "Ubisoft Error",
        "Timeout Error",
        "Cancelled",
    };
    ITF_ASSERT_STATIC(ITF_ARRAY_SIZE(type_strings) == OnlineError::ErrorTypeCount, "The array containing the strings for online error types doesn't have the proper size");

    const char* subtype_strings[] =
    {
        "None",
        "Maintenance",
        "Not Initialized",
        "Authentication",
        "Ubisoft Connect",
        "News"
    };
    ITF_ASSERT_STATIC(ITF_ARRAY_SIZE(subtype_strings) == OnlineError::ErrorSubtypeCount, "The array containing the strings for online error subtypes doesn't have the proper size");
}

String8 OnlineError::toString() const
{
    String8 result;
    result.setTextFormat("%s (subtype %s, code %d)", type_strings[m_type], subtype_strings[m_subtype], m_code);
    return result;
}

String8 OnlineError::typeToString(OnlineError::ErrorType _type)
{
    ITF_ASSERT(_type >= 0);
    ITF_ASSERT(_type < OnlineError::ErrorTypeCount);
    return String8(type_strings[_type]);
}

String8 OnlineError::subtypeToString(OnlineError::ErrorSubtype _subtype)
{
    ITF_ASSERT(_subtype >= 0);
    ITF_ASSERT(_subtype < OnlineError::ErrorSubtypeCount);
    return String8(subtype_strings[_subtype]);
}


} // namespace ITF
