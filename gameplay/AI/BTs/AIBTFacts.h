#ifndef _ITF_AIBTFACTS_H_
#define _ITF_AIBTFACTS_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

namespace ITF
{

// Tells us when to send the hit stim
#define AIFact_navTargetPos ITF_GET_STRINGID_CRC(AIFact_NavTargetPoint,3045562591)
#define AIFact_navTargetActor ITF_GET_STRINGID_CRC(AIFact_NavTargetActor,3661844503)
#define AIFact_behindPlayer ITF_GET_STRINGID_CRC(AIFact_BehindPlayer,258248488)
#define AIFact_infrontPlayer ITF_GET_STRINGID_CRC(AIFact_InFrontPlayer,1970724455)

};

#endif //_ITF_AIBTFACTS_H_

