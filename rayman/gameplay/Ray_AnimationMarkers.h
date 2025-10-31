#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#define _ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

namespace ITF
{

// Tells us when to send the hit stim
#define MRK_Perform_Hit ITF_GET_STRINGID_CRC(MRK_Perform_Hit,3609949161)
#define MRK_Perform_Hit_Start ITF_GET_STRINGID_CRC(MRK_Perform_Hit_Start,1540319322)
#define MRK_Perform_Hit_Stop ITF_GET_STRINGID_CRC(MRK_Perform_Hit_Stop,4042576674)
#define MRK_Prepare_Hit ITF_GET_STRINGID_CRC(MRK_Prepare_Hit,4094847275)
#define MRK_Prepare_Hit_Start ITF_GET_STRINGID_CRC(MRK_Prepare_Hit_Start,3231032146)
#define MRK_Prepare_Hit_Stop ITF_GET_STRINGID_CRC(MRK_Prepare_Hit_Stop,3496397681)
#define MRK_HitArea_Start ITF_GET_STRINGID_CRC(MRK_HitArea_Start,2127564399)
#define MRK_HitArea_End ITF_GET_STRINGID_CRC(MRK_HitArea_End,2572252850)
#define MRK_Permanent_Hit ITF_GET_STRINGID_CRC(MRK_Permanent_Hit,4771022)
#define MRK_ComboHit_Next ITF_GET_STRINGID_CRC(MRK_ComboHit_Next,2121265408)
#define MRK_Crush_Hit ITF_GET_STRINGID_CRC(MRK_Crush_Hit,4127241626)
#define MRK_Finish_Anim ITF_GET_STRINGID_CRC(MRK_Finish_Anim,1478727137)

// Tells us when to send the rope impulse when climbing
#define MRK_Force_Impulse ITF_GET_STRINGID_CRC(MRK_Force_Impulse,2586928088)

// Tells us when the uturn is finished
#define MRK_UTurn_Finish ITF_GET_STRINGID_CRC(MRK_UTurn_Finish,2940797116)

// Tells us when to flip the character
#define MRK_Flip_Actor ITF_GET_STRINGID_CRC(MRK_Flip_Actor,233356407)

// Tells us when we can interrupt an action and move again
#define MRK_Allow_Move ITF_GET_STRINGID_CRC(MRK_Allow_Move,3125360241)

// Tells us that it will start a UTurn
#define MRK_Perform_UTurn ITF_GET_STRINGID_CRC(MRK_Perform_UTurn,727287548)

#define MRK_Death ITF_GET_STRINGID_CRC(MRK_Death,1201978210)

#define MRK_UpHitApex_Start ITF_GET_STRINGID_CRC(MRK_UpHitApex_Start,2800804982)
#define MRK_UpHitApex_Stop ITF_GET_STRINGID_CRC(MRK_UpHitApex_Stop,3826169485)

#define MRK_SpawnFX ITF_GET_STRINGID_CRC(MRK_SpawnFX,1765115524)

};

#endif //_ITF_RAY_ANIMATIONMARKERS_H_

