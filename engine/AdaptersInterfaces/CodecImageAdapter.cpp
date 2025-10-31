#include "precompiled_engine.h"

#ifndef _ITF_CODECIMAGEADAPTER_H_
#include "engine/AdaptersInterfaces/CodecImageAdapter.h"
#endif //_ITF_CODECIMAGEADAPTER_H_

namespace ITF
{
void RawDataContainer::release(void)
{
    if(mData)
    {
        delete [] mData;
        mData = 0;
    }
}

CodecImageAdapter::CodecImageAdapter(const String& name)
    : d_identifierString(name)
{    
}

CodecImageAdapter::~CodecImageAdapter()
{
}

const String& CodecImageAdapter::getIdentifierString() const
{
    return d_identifierString;
}

const String& CodecImageAdapter::getSupportedFormat() const
{
    return d_supportedFormat;
}

} // namespace ITF
