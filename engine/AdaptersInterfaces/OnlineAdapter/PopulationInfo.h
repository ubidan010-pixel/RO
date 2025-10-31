#pragma once

#include "core/String8.h"

namespace ITF
{
    struct PopulationInfo
    {
        PopulationInfo() {}

        String8 m_name;
        String8 m_subject;
        String8 m_object;
        String8 m_spaceId;
    };
};
