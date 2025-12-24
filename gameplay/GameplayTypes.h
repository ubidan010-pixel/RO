#ifndef _ITF_GAMEPLAYTYPES_H_
#define _ITF_GAMEPLAYTYPES_H_

namespace ITF
{
    typedef u32 Faction;
    const Faction FACTION_UNKNOWN = U32_INVALID;

    typedef u32 GameMode;
    const GameMode GAMEMODE_UNKNOWN = U32_INVALID;

    enum ECharacterSize
    {
        CHARACTERSIZE_SMALL,
        CHARACTERSIZE_NORMAL,
        CHARACTERSIZE_BIG,
    };

    #define s_RegionName_Friction ITF_GET_STRINGID_CRC(friction,889942831)
    #define s_RegionName_Swim ITF_GET_STRINGID_CRC(swim,1258662562)
    #define s_RegionName_GroundFriction ITF_GET_STRINGID_CRC(groundFriction,2379881549)

    enum EContextIconType
    {
        ContextIconType_Invalid = -1,
        ContextIconType_Select = 0,
        ContextIconType_Back = 1,
        ContextIconType_Delete = 2,
        ContextIconType_RayHome = 3,
        ContextIconType_Upload = 4,
        ContextIconType_Download = 5,
        ContextIconType_Count,
    };

    enum EContextIcon
    {
        ContextIcon_Invalid = -1,
        ContextIcon_Select,
        ContextIcon_Continue,
        ContextIcon_Enter,
        ContextIcon_Skip,
        ContextIcon_Back,
        ContextIcon_Retry,
        ContextIcon_Delete,
        ContextIcon_RayHome,
        ContextIcon_Controls,
        ContextIcon_SpeedUp,
        ContextIcon_Upload,
        ContextIcon_Download,
        ContextIcon_Confirm,
        ContextIcon_Cancel,
        ContextIcon_Count,
    };

} // ITF

#endif // _ITF_GAMEPLAYTYPES_H_

