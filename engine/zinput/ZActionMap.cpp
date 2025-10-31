#include "precompiled_engine.h"

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{
void ZAction::Reset()
{
    ITF_ASSERT(ACTIONMAP_MAX_PLAYER == 4);

    const u32 u32_Count = m_inputs.size();
    for (u32 inputId = 0; inputId < u32_Count; ++inputId)
    {
        ZInput& curZInput = m_inputs[inputId];
        curZInput.m_match[0] = bfalse;
        curZInput.m_match[1] = bfalse;
        curZInput.m_match[2] = bfalse;
        curZInput.m_match[3] = bfalse;
    }

    m_matchs[0] = 0;
    m_matchs[1] = 0;
    m_matchs[2] = 0;
    m_matchs[3] = 0;

    m_axis[0] = 0.f;
    m_axis[1] = 0.f;
    m_axis[2] = 0.f;
    m_axis[3] = 0.f;
}

BEGIN_SERIALIZATION(ZInput)
    SERIALIZE_MEMBER("control",m_control);
    SERIALIZE_MEMBER("query", m_query);
    SERIALIZE_MEMBER("axis_range", m_axisRange);
    SERIALIZE_MEMBER("threshold", m_threshold);
    SERIALIZE_MEMBER("delay", m_delay);
END_SERIALIZATION()

BEGIN_SERIALIZATION(ZAction)
    SERIALIZE_MEMBER("name",m_action);
    SERIALIZE_MEMBER("inverted",m_inverted);
    SERIALIZE_MEMBER("scale",m_scale);
    SERIALIZE_CONTAINER_OBJECT("input", m_inputs);
END_SERIALIZATION()

}//namespace ITF
