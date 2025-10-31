#ifndef _ITF_WAR_ANIMATIONMARKERS_H_
#define _ITF_WAR_ANIMATIONMARKERS_H_

namespace ITF
{
    // Tells us when to send the hit stim
    static const StringID War_MRK_Perform_Hit("MRK_Perform_Hit");

    // Tells us when an enemy is ready to be finished off
    static const StringID War_MRK_FinishOff_Start("MRK_FinishOff_Start");
    static const StringID War_MRK_FinishOff_Stop("MRK_FinishOff_Stop");

    //// Tells us when to start ejecting when receiving a hit
    //static const StringID Hit_Impact_Stop("MRK_Hit_Impact_Stop");


    // Tells us when we should lock the move input when doing a uturn
    // so that the animation is not restarted when changing from stand to move
    static const StringID War_MRK_UTurnIntro_Stop("MRK_UTurnIntro_Stop");

    // Tells us when the uturn is finished
    static const StringID War_MRK_UTurn_Stop("MRK_UTurn_Stop");


    // Tells us when we can interrupt an action and move again
    static const StringID War_MRK_Allow_Move("MRK_Allow_Move");

    // Tells us when to send the rope impulse when climbing
    static const StringID War_MRK_Up_Start("MRK_Up_Start");


    // Window to queue a combo punch
    static const StringID War_MRK_PunchComboWindow_Start("MRK_PunchComboWindow_Start");
    static const StringID War_MRK_PunchComboWindow_Stop("MRK_PunchComboWindow_Stop");

};

#endif //_ITF_WAR_ANIMATIONMARKERS_H_

