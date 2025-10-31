#include "precompiled_CodecImageAdapter_FreeImage.h"

#include "freeImageCodec_ITF.h"
#include "freeImageCodecModule_ITF.h"

ITF::CodecImageAdapter* createImageCodec(void)
{
  return new ITF::FreeImageCodec();
}

void destroyImageCodec(ITF::CodecImageAdapter* CodecImageAdapter)
{
  delete CodecImageAdapter;
}