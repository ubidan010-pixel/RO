#ifndef _ITF_FONTCOOKER_H_
#define _ITF_FONTCOOKER_H_

namespace ITF
{

class DepCollection;

class fontCooker
{
public:
//    static bbool cookFile(const String& _vPlatform,const String& _szFilename); //not supported yet

    static bbool dependenciesFile( const String& cacheKey, const String& _vPlatform,const String& _szFilename,DepCollection& _vDependencies );
};

}

#endif //_ITF_FONTCOOKER_H_