#include "precompiled_engine.h"

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

namespace ITF
{
    const DepthRange DepthRange::InvalidDepthRange;

    DepthRange::DepthRange()
        : m_referenceDepth(F32_INFINITY)
    {

    }

    DepthRange::DepthRange( f32 _depth )
    {
        m_referenceDepth = floorf(_depth + 0.5f);
    }

} // ITF