#ifndef _ITF_LOADING_SEQUENCE_COOKER_H_
#define _ITF_LOADING_SEQUENCE_COOKER_H_

namespace ITF
{

class LoadingSequenceCooker
{
public:
    static bbool cook( const String& _vPlatform,const String& _relativeFilename );
};

}

#endif  // _ITF_LOADING_SEQUENCE_COOKER_H_
