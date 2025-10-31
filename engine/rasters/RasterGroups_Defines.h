

#ifdef ITF_DEFINE_RASTERGROUP_IDX
    #define ITF_DECLARE_RASTERGROUP(group_) RasterGroupIdx_##group_
    #define ITF_DECLARE_RASTERGROUPS(...) enum RasterGroupIdx_t : unsigned { __VA_ARGS__ }
#elif !defined(ITF_DEFINE_RASTERGROUP_STRINGS)
    // first generate indexes that will allow to bit shift
    #define ITF_DEFINE_RASTERGROUP_IDX
    #include "engine/rasters/RasterGroups_Defines.h"
    #undef ITF_DEFINE_RASTERGROUP_IDX
    #define ITF_DECLARE_RASTERGROUP(group_) RasterGroup_##group_ = 1 << RasterGroupIdx_##group_
    #define ITF_DECLARE_RASTERGROUPS(...) enum RasterGroup_t : unsigned { __VA_ARGS__ }
#else
    #define ITF_DECLARE_RASTERGROUP(group_) #group_
    #define ITF_DECLARE_RASTERGROUPS(...) static const char* RasterGroup_Names[]={ __VA_ARGS__ }
#endif

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Declare your raster groups here
    ITF_DECLARE_RASTERGROUPS(
        ITF_DECLARE_RASTERGROUP(Frame),
        ITF_DECLARE_RASTERGROUP(WholeFrame),
        ITF_DECLARE_RASTERGROUP(ActorUpdate),
        ITF_DECLARE_RASTERGROUP(ComponentUpdate),
        ITF_DECLARE_RASTERGROUP(Animation),
        ITF_DECLARE_RASTERGROUP(Physics),
        ITF_DECLARE_RASTERGROUP(Misc),
        ITF_DECLARE_RASTERGROUP(END));
}

#undef ITF_DECLARE_RASTERGROUP
#undef ITF_DECLARE_RASTERGROUPS
