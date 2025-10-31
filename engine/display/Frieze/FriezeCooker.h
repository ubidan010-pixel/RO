#ifndef _ITF_FRIEZECOOKER_H_
#define _ITF_FRIEZECOOKER_H_

namespace ITF
{
#ifdef ITF_SUPPORT_EDITOR

    class FriseConfigCooker
    {
    public:

        static bbool cookFile(const String& _szPlatform,const String& _szFilename);
        static bbool cook(const String& _szPlatform,const String& _szFilename, bbool _RaiseError);
    };

}


#endif// ITF_SUPPORT_EDITOR
#endif //_ITF_FRIEZECOOKER_H_
