#include "core/Color.h"

// .h designed to be included several time (not an 'importable header' as defined in c++ std).
// By default, it defines the warning categories enum values.
// If ITF_DEFINE_WARNINGCATEGORIES_NAMES is defined it create an array of string names of categories (WarningCategory_Names).
// If ITF_DEFINE_WARNINGCATEGORIES_COLORS is defined it create an array of Color of categories (WarningCategory_Colors).

#ifdef ITF_DEFINE_WARNINGCATEGORIES_NAMES
    #define ITF_DEFINE_WARNINGCATEGORY_PAIR(name_, color_) "["#name_"]"
    #define ITF_DECLARE_WARNINGCATEGORIES(...) static const char* WarningCategory_Names[]={__VA_ARGS__}
#elif defined(ITF_DEFINE_WARNINGCATEGORIES_COLORS)
    #define ITF_DEFINE_WARNINGCATEGORY_PAIR(name_, color_) color_
    #define ITF_DECLARE_WARNINGCATEGORIES(...) static const Color WarningCategory_Colors[]={__VA_ARGS__}
#else
    #define ITF_DEFINE_WARNINGCATEGORY_PAIR(name_, color_) WarningCategory_##name_
    #define ITF_DECLARE_WARNINGCATEGORIES(...) enum WarningCategoryIndex {__VA_ARGS__}
#endif

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Declare your warning categories here. First category must be default, and will be shown as empty string
    ITF_DECLARE_WARNINGCATEGORIES(
        ITF_DEFINE_WARNINGCATEGORY_PAIR(Default, Color::red()),
        ITF_DEFINE_WARNINGCATEGORY_PAIR(GPP, Color::violet()),
        ITF_DEFINE_WARNINGCATEGORY_PAIR(LD, Color::orange()),
        ITF_DEFINE_WARNINGCATEGORY_PAIR(Anim, Color::green()),
        ITF_DEFINE_WARNINGCATEGORY_PAIR(Graph, Color::pink()),
        ITF_DEFINE_WARNINGCATEGORY_PAIR(Engine, Color::cyan()),
        ITF_DEFINE_WARNINGCATEGORY_PAIR(Sound, Color::black()),
        ITF_DEFINE_WARNINGCATEGORY_PAIR(L10n, Color::blue()),
        ITF_DEFINE_WARNINGCATEGORY_PAIR(END, Color::black()));
}

#undef ITF_DEFINE_WARNINGCATEGORY_PAIR
#undef ITF_DECLARE_WARNINGCATEGORIES
