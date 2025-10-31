#ifndef _ITF_ANIMATIONCOOKER_H_
#define _ITF_ANIMATIONCOOKER_H_

namespace ITF
{
    class DepCollection;

    class animSkeletonCooker
    {
    public:

        static bbool cookFile(const String& _szPlatform,const String& _szFilename);
        static bbool cook(const String& _szPlatform,const String& _szFilename, bbool _RaiseError);
        static bbool cookForced(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError, bbool _forceCook);
};

    class animPatchBankCooker
    {
    public:


        static bbool cookFile(const String& _szPlatform,const String& _szFilename);
        static bbool cook(const String& _szPlatform,const String& _szFilename, bbool _RaiseError);
        static bbool cookForced(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError, bbool _forceCook);
    };

    class animTrackCooker
    {
    public:
        static bbool cookFile(const String& _szPlatform,const String& _szFilename);
        static bbool cook(const String& _szPlatform,const String& _szFilename, bbool _RaiseError);
        static bbool cookForced(const String& _szPlatform,const String& _szFilename, bbool _bRaiseError, bbool _forceCook);
        static bbool dependencyFiles( const String& cacheKey, const String& _vPlatform,const String& _szFilename,DepCollection& _vDependencies );
    };

}

#endif //_ITF_ANIMATIONCOOKER_H_
