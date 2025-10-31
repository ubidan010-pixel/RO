#ifndef _ITF_TRCMESSAGES_WII_H_
#define _ITF_TRCMESSAGES_WII_H_

namespace ITF
{

#define MAX_LANGUAGE        7

    // Hard coded messages for handling fatal error or disc error (all important messages have to be found in the exe)
    class TRCMessages_WII
    {
    public:
        enum TRCLanguageWII
        {
            TRC_LANG_JAPANESE=0,
            TRC_LANG_ENGLISH=1,
            TRC_LANG_GERMAN=2,
            TRC_LANG_FRENCH=3,
            TRC_LANG_SPANISH=4,
            TRC_LANG_ITALIAN=5,
            TRC_LANG_DUTCH=6,
            TRC_LANG_SIMP_CHINESE,
            TRC_LANG_TRAD_CHINESE,
            TRC_LANG_KOREAN
        };

        static u32   getLanguageIndex();

        static char* ms_szDvdWrongDisc[MAX_LANGUAGE];
        static char* ms_szDvdNoDisc[MAX_LANGUAGE];
        static char* ms_szDvdFatal[MAX_LANGUAGE];
        static char* ms_szDvdRetry[MAX_LANGUAGE];
        static char* ms_szDvdMotorOff[MAX_LANGUAGE];
        static char* ms_szDvdReadingDisc[MAX_LANGUAGE];
        static char* ms_szWiiChannelInstallFailed[MAX_LANGUAGE];
        static char* ms_szNandSystemCorrupt[MAX_LANGUAGE];
        static char* ms_szNandContentFilesCorrupt[MAX_LANGUAGE];
        static char* ms_szNandUnknownError[MAX_LANGUAGE];
    };
} //namespace ITF

#endif //_ITF_TRCMESSAGES_WII_H_
