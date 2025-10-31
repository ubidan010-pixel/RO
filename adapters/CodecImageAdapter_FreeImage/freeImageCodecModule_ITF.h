#ifndef _FREEIMAGECODECMODULE_ITF_H_
#define _FREEIMAGECODECMODULE_ITF_H_

#ifndef _ITF_CODECIMAGEADAPTER_H_
#include "engine/AdaptersInterfaces/codecImageAdapter.h"
#endif //_ITF_CODECIMAGEADAPTER_H_

#include "freeImageCodec_ITF.h"

extern "C" ITF::CodecImageAdapter* createImageCodec(void);
extern "C" void destroyImageCodec(ITF::CodecImageAdapter* CodecImageAdapter);

#endif