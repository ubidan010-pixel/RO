#ifndef _ITF_ACTCOOKER_H_
#define _ITF_ACTCOOKER_H_

namespace ITF
{

class DepCollection;

class actCooker
{
public:
    static bbool cookFile(const String& _vPlatform,const String& _szFilename);

    static bbool actDependenciesFile( const String& cacheKey, const String& _vPlatform, const String& _szFilename, DepCollection& _vDependencies );
    static bbool dependenciesFile( const String& cacheKey, const String& _vPlatform, const String& _szFilename, DepCollection& _vDependencies );
};

}

#endif //_ITF_ACTCOOKER_H_
