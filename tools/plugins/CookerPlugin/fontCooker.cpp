#include "core/enginecommon.h"

#include "fontCooker.h"

#ifndef _ITF_FONT_H_
#include "engine/display/font.h"
#endif //_ITF_FONT_H_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

namespace ITF
{
    //------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool fontCooker::dependenciesFile( const String& cacheKey, const String& _vPlatform,const String& _szFilename,DepCollection& _vDependencies )
    {
        if ( _vDependencies.load( cacheKey, _szFilename, _vPlatform ) )
        {
            _vDependencies.filterExt();
            return true;
        }
        Font::dependenciesFile(_szFilename,_vDependencies);
        _vDependencies.save( cacheKey, _szFilename, _vPlatform );
        _vDependencies.filterExt();
        return btrue;
    }



    //------------------------------------------------------------------------------------------------------------------------------------------------------
}