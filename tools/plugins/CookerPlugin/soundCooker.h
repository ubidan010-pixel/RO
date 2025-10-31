#ifndef _ITF_SOUNDCOOKER_H_
#define _ITF_SOUNDCOOKER_H_

namespace ITF
{

class DepCollection;

class soundCooker
{
public:
    static bbool cookFile(const String& _vPlatform,const String& _szFilename);
    static bbool compress(const String& _vPlatform,const String& _szFilename,bbool _bUseTemporaryConfig);
    static bbool binarize(const String& _szFilename);
    static bbool dependencyFiles( const String& cacheKey, const String& platform, const String& filename, DepCollection& dependencies );

    static bbool useBatch;
};

}

#endif //_ITF_SOUNDCOOKER_H_
