#include "precompiled_CodecImageAdapter_FreeImage.h"

#ifndef _FREEIMAGECODEC_ITF_H_
    #include "adapters/CodecImageAdapter_FreeImage/freeImageCodec_ITF.h"
#endif // _FREEIMAGECODEC_ITF_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef FREEIMAGE_H
    #include "FreeImage.h"
#endif // FREEIMAGE_H

#include "core/ByteEndian.h"

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

namespace
{
    void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
    {
        LOG("FreeImage error ( %s ): %s" , FreeImage_GetFormatFromFIF(fif), message);
    }
}

namespace  ITF
{
FreeImageCodec::FreeImageCodec()
    : CodecImageAdapter("FreeImageCodec - FreeImage based image codec")
{
    FreeImage_Initialise(true);
    FreeImage_SetOutputMessage(&FreeImageErrorHandler);

    // Getting extensions
    for (int i = 0; i < FreeImage_GetFIFCount(); ++i)
    {
        String exts(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));

        // Replace commas with spaces
         StringConverter name(exts);
        
        char* text = (char*)name.getChar();
        for (size_t i = 0; i < exts.getLen(); ++i)
            if (text[i] == ',')
               text[i] = ' ';
 
         // Add space after existing extensions
        if (!d_supportedFormat.isEmpty())
            d_supportedFormat += ' ';

        d_supportedFormat += text;
    }
}

FreeImageCodec::~FreeImageCodec()
{
    FreeImage_DeInitialise();
}

Texture* FreeImageCodec::load(const RawDataContainer& data, Texture* result,const String& _szFilename)
{

    int len = (int)data.getSize();
    FIMEMORY *mem = 0;
    FIBITMAP *img = 0;
    Texture *retval = 0;

    {
        mem = FreeImage_OpenMemory((BYTE*)data.getDataPtr(), len);
        if (mem == 0)
            LOG("Unable to open memory stream, FreeImage_OpenMemory failed");

        FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(mem, len);

        if (fif == FIF_UNKNOWN) // it may be that it's TARGA or MNG
        {
            fif = FIF_TARGA;
            img = FreeImage_LoadFromMemory(fif, mem, 0);

            if (img == 0)
            {
                fif = FIF_MNG;
                img = FreeImage_LoadFromMemory(fif, mem, 0);
            }
        }
        else
            img = FreeImage_LoadFromMemory(fif, mem, 0);

        if (img == 0)
            LOG("Unable to load image, FreeImage_LoadFromMemory failed");

         FIBITMAP *newImg = FreeImage_ConvertTo32Bits(img);
         if (newImg == 0)
              LOG("Unable to convert image, FreeImage_ConvertTo32Bits failed");

        FreeImage_Unload(img);
        img = newImg;
        newImg = 0;

        // convert that to RGBA.
        //
        // It is now:
        // RED_MASK        0x00FF0000
        // GREEN_MASK    0x0000FF00
        // BLUE_MASK    0x000000FF
        // ALPHA_MASK    0xFF000000
        //
        // It should be:
        // RED_MASK        0x000000FF
        // GREEN_MASK    0x0000FF00
        // BLUE_MASK    0x00FF0000
        // ALPHA_MASK    0xFF000000

        unsigned int pitch = FreeImage_GetPitch(img);
        unsigned int height = FreeImage_GetHeight(img);
        unsigned int width = FreeImage_GetWidth(img);
        u32          rawBufSize = width * height << 2;
        unsigned char *rawBuf = new_array(unsigned char,rawBufSize,mId_System);

        // convert the bitmap to raw bits (top-left pixel first) 
        FreeImage_ConvertToRawBits(rawBuf, img, pitch, 32,
            FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);

        // We need to convert pixel format a little
        // NB: little endian only - I think(!)


        for (unsigned int i = 0; i < height; ++i)
        {
            for (unsigned int j = 0; j < width; ++j)
            {
                unsigned int p = *(((unsigned int*)(rawBuf + i * pitch)) + j);

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

                unsigned int r = (p >> 16) & 0x000000FF;
                unsigned int b = (p << 16) & 0x00FF0000;
                p &= 0xFF00FF00;
                p |= r | b;
#else
                Endian::hideEndian<unsigned int>(p,(u8*) &p);
#endif // FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
                // write the adjusted pixel back
                *(((unsigned int*)(rawBuf + i * pitch)) + j) = p;
            }
        }


        FreeImage_Unload(img);
        img = 0;
        GFX_ADAPTER->generateTexture(result);
        Size size(width, height);
        GFX_ADAPTER->setTextureSize(result, size);
        GFX_ADAPTER->loadFromMemory(rawBuf, size,result, Texture::PF_RGBA,0,bfalse,0);//temp 1 = Texture::PF_RGBA
        result->m_alpha = 1;
        delete [] rawBuf;
        retval = result;
    }

    if (mem != 0) FreeImage_CloseMemory(mem);
    RESOURCE_MANAGER->addResourceAsLoaded(retval);
    return retval;
}

} // namespace ITF
