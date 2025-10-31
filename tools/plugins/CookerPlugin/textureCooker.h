#ifndef _ITF_TEXTURECOOKER_H_
#define _ITF_TEXTURECOOKER_H_

namespace ITF
{

class DepCollection;

class textureCooker
{
public:

    static bbool cookFile(const String& _vPlatform,const String& _szFilename);
    static bbool compress(const String& _vPlatform,const String& _szFilename,bbool _bUseTemporaryConfig);
    static bbool binarize(const String& _szFilename);
    static bbool cudaIsEnable();
    static bbool dependencyFiles( const String& cacheKey, const String& _vPlatform,const String& _szFilename,DepCollection& _vDependencies );
};

}

#endif //_ITF_TEXTURECOOKER_H_