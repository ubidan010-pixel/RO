#ifndef _ITF_DEP_PARSER_H_
#define _ITF_DEP_PARSER_H_

namespace ITF
{

class DepCollection;
class DepCollector;

class XmlDepParser
{
public:
    static bbool iscDependencyFiles( const String& cacheKey, const String& platform, const String& filename, DepCollection& col );
    static bbool isdDependencyFiles( const String& cacheKey, const String& platform, const String& filename, DepCollection& col );
    static bbool fcgDependencyFiles( const String& cacheKey, const String& platform, const String& filename, DepCollection& col );
};

}

#endif //_ITF_DEP_PARSER_H_
