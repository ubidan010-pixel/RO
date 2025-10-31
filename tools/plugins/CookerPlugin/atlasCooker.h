#ifndef _ITF_ATLASCOOKER_H_
#define _ITF_ATLASCOOKER_H_

namespace ITF
{

class atlasCooker
{
public:
    static bbool atlasCooker::cookFile(const String& _vPlatform,const String& _relativeFilename);

	static bbool atlasCooker::cookFile(const String& _vPlatform,const String& _relativeFilename,bbool _bRaiseError);
};

}

#endif //_ITF_ATLASCOOKER_H_