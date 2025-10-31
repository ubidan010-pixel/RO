#ifndef _ITF_WAR_GAMEPLAYTYPES_H_
#define _ITF_WAR_GAMEPLAYTYPES_H_

namespace ITF
{
    enum War_EReceivedHitType
    {
        WAR_RECEIVEDHITTYPE_NORMALHIT       = 0,
        ENUM_FORCE_SIZE_32(War_EReceivedHitType)
    };

    enum War_EHitLevel
    {
        WAR_HITLEVEL_LEVEL1     = 0,
        WAR_HITLEVEL_DEATH      = 2,
        ENUM_FORCE_SIZE_32(War_EHitLevel)
    };

    enum War_EPunchType
    {
        WAR_PUNCHTYPE_NORMAL                = 0,
        WAR_PUNCHTYPE_CROUCHKICK            = 1,
        WAR_PUNCHTYPE_JCVD                  = 2,
        WAR_PUNCHTYPE_ACCROBATIC_TORNADO    = 3,
        WAR_PUNCHTYPE_UTURNKICK             = 5,
        WAR_PUNCHTYPE_UTURNJUMPKICK         = 6,
        WAR_PUNCHTYPE_EARTHQUAKE            = 7,
        ENUM_FORCE_SIZE_32(War_EPunchType)
    };

    //const GameMode GAMEMODE_UNKNOWN       = 0;
    const GameMode WAR_GAMEMODE_NORMAL      = 1;

    //const Faction FACTION_UNKNOWN         = U32_INVALID;
    const Faction WAR_FACTION_NEUTRAL       = 0;
    const Faction WAR_FACTION_ALLY          = 1;
    const Faction WAR_FACTION_ENEMY         = 2;
    const u32     WAR_FACTION_COUNT         = 3;

} // ITF

#endif // _ITF_WAR_GAMEPLAYTYPES_H_

