#ifndef _ITF_ACTIONMAP_H_
#define _ITF_ACTIONMAP_H_

#ifndef _ITF_INPUTINFO_H_
#include "engine/zinput/ZInputInfo.h"
#endif //_ITF_INPUTINFO_H_

namespace ITF
{
    #define ACTIONMAP_MAX_PLAYER        4

#define ZINPUT_QUERY_AXIS ITF_GET_STRINGID_CRC(AXIS,2106135458)
#define ZINPUT_QUERY_PRESS ITF_GET_STRINGID_CRC(PRESS,2931858408)
#define ZINPUT_QUERY_RELEASE ITF_GET_STRINGID_CRC(RELEASE,3780715669)
#define ZINPUT_QUERY_HOLD ITF_GET_STRINGID_CRC(HOLD,3335884524)
#define ZINPUT_QUERY_ACC ITF_GET_STRINGID_CRC(ACC,1484685828)

    //- ---------------------------------------------------------------
    class ZInput
    {
        DECLARE_SERIALIZE()

    public:

        ZInput() : m_threshold(0.0f), m_delay(0.0), m_axisRange(Vec2d::Zero), 
                m_translatedControl(U32_INVALID),
                m_translatedQuery(SInputInfo::QUERY_UNSET)
            {
            ITF_MemSet(m_timer, 0, sizeof(m_timer));
            ITF_MemSet(m_match, 0, sizeof(m_match));
        }
        StringID                    m_control;
        u32                         m_translatedControl;        // Optimization param

        StringID                    m_query;        
        SInputInfo::EInputQuery     m_translatedQuery;          // Optimization param

        bbool                       m_match[ACTIONMAP_MAX_PLAYER];        // return
        Vec2d                       m_axisRange;
        f32                         m_threshold;
        f32                         m_delay;
        f64                         m_timer[ACTIONMAP_MAX_PLAYER];        // return
    };

    class ZAction
    {
        DECLARE_SERIALIZE()

        ZAction(): m_scale(1.0f), m_inverted(bfalse) { Reset(); }
        void Reset();

        ITF_VECTOR <ZInput>         m_inputs;
        StringID                    m_action;
        bbool                       m_inverted;
        u32                         m_matchs[ACTIONMAP_MAX_PLAYER];       // return
        f32                         m_axis[ACTIONMAP_MAX_PLAYER];         // return
        f32                         m_scale;        
    };

    typedef ITF_VECTOR<ZAction>   ActionMap;
}//namespace ITF

#endif // _ITF_ACTIONMAP_H_
