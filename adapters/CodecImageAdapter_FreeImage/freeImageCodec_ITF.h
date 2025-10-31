#ifndef _FREEIMAGECODEC_ITF_H_
#define _FREEIMAGECODEC_ITF_H_

#ifndef _ITF_CODECIMAGEADAPTER_H_
#include "engine/AdaptersInterfaces/codecImageAdapter.h"
#endif //_ITF_CODECIMAGEADAPTER_H_

namespace  ITF
{

class Texture;

class FreeImageCodec : public CodecImageAdapter 
{
public:
    FreeImageCodec();
    ~FreeImageCodec();
    

    Texture* load(const RawDataContainer& data, Texture* result,const String& _szFilename);
protected:

private:

};    


} // namespace ITF

#endif