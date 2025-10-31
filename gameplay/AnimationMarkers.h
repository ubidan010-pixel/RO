#ifndef _ITF_ANIMATIONMARKERS_H_
#define _ITF_ANIMATIONMARKERS_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

namespace ITF
{
    // Tells us when an AI action is finished --we can't always rely on isAnimFinished()
    #define MRK_AI_Stop ITF_GET_STRINGID_CRC(MRK_AI_Stop,3634767425)

    // Tells us that we should shake the camera: CAMERA->addShake("QuakeN")
    #define MRK_Cam_Shake_00 ITF_GET_STRINGID_CRC(MRK_Cam_Shake_00,3907227817)
    #define MRK_Cam_Shake_01 ITF_GET_STRINGID_CRC(MRK_Cam_Shake_01,3754214614)
    #define MRK_Cam_Shake_02 ITF_GET_STRINGID_CRC(MRK_Cam_Shake_02,1607881029)
    #define MRK_Cam_Shake_03 ITF_GET_STRINGID_CRC(MRK_Cam_Shake_03,45334320)
};

#endif //_ITF_ANIMATIONMARKERS_H_

