#ifndef _ITF_PACKCONFIGCOOKER_H_
#define _ITF_PACKCONFIGCOOKER_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Cooks a PackConfig. ONLY CALLED FROM SPECIAL CASE IN ASSETCOOKER
    class packConfig_Cooker
    {
    public:
        static bbool cook(const String& _szPlatform,const String& _szFilename);
    };
}

#endif
