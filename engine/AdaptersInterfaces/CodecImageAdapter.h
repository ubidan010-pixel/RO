#ifndef _ITF_CODECIMAGEADAPTER_H_
#define _ITF_CODECIMAGEADAPTER_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_


namespace ITF
{

class RawDataContainer
{
public:
    RawDataContainer()
      : mData(0),
        mSize(0)
    {
    }

    ~RawDataContainer(void) 
    { 
        release();
    }

    void setData(unsigned char* data) { mData = data; }

    unsigned char* getDataPtr(void) { return mData; }
    const unsigned char* getDataPtr(void) const { return mData; }

    void setSize(size_t size) { mSize = size; }
    size_t getSize(void) const { return mSize; }
    void release(void);

private:
    unsigned char* mData;
    size_t mSize;
};

class Texture;

class CodecImageAdapter
{
public:

    virtual             ~CodecImageAdapter     (                   );

protected:

    CodecImageAdapter(const String& name);

public:

    const String& getIdentifierString() const;
    const String& getSupportedFormat() const;
    virtual Texture* load(const RawDataContainer& data, Texture* result,const String& _szFilename) = 0;

private:
    String d_identifierString;   //!< display the name of the codec 

protected:
    String d_supportedFormat;   //!< list all image file format supported 

private:
    CodecImageAdapter(const CodecImageAdapter& obj);
    CodecImageAdapter& operator=(CodecImageAdapter& obj);
};

} // namespace ITF

#endif