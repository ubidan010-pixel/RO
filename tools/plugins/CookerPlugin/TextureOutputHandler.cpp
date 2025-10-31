#include "core/enginecommon.h"


#include <nvimage/Image.h>
#include <nvimage/DirectDrawSurface.h>

#include <nvtt/nvtt.h>
#include <io.h>

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#include "tools/plugins/CookerPlugin/TextureOutputHandler.h"

namespace ITF
{

//------------------------------------------------------------------------------------------------------------------------------------
    TextureOutputHandler::TextureOutputHandler(const wchar_t* srcname,const wchar_t* shortname,const char * name,const wchar_t* platform, const f32 _lod_bias, const u32 _alphaBias, bbool _uniqueRGB, u32 _anisotropy ) :
    m_shortname(shortname),m_srcname(srcname),total(0), progress(0), percentage(0), 
    m_platform(platform),verbose(btrue),swapBigEndian(bfalse),tmpBufferSize(0),tmpBuffer(NULL),m_lod_bias(_lod_bias), m_AlphaBias(_alphaBias), m_HasOneColor(_uniqueRGB), m_Anisotropy(_anisotropy)
   {
        m_fp = fopen(name, "wb");

        if (!m_fp)
        {
            ITF_ASSERT_MSG("file open failed on %s",name);
            stream  = NULL;
            return ;
        }
        
        stream = new nv::StdOutputStream(m_fp,false);
    }


 TextureOutputHandler::~TextureOutputHandler()
 { 
     if (m_fp)
     {
         fclose(m_fp);
     }

     delete stream;
     delete[] tmpBuffer;

   
 }

//------------------------------------------------------------------------------------------------------------------------------------

 bool TextureOutputHandler::writeDataBlock(const void * data, int size)
 {
     if (swapBigEndian)
     {
         u32 countSwap = size/sizeof(u32);
         u8* mem = getBufferSize(size);
         u8* currentMem = mem;
         u32* pSource = (u32*) data;

         for (u32 i=0;i<countSwap;++i)
         {       
             swapBigEndianDXT(*pSource,currentMem);
             currentMem+=sizeof(u32);
             pSource++;
         }

         return writeData(mem,size);
     }
     else
     {
         return writeData(data,size);
     }

 }

//------------------------------------------------------------------------------------------------------------------------------------

 bool TextureOutputHandler::writeDataHeader(const void * data, int size)
 {
     ITF_ASSERT(size==sizeof(DDS_HEADER));
     DDS_HEADER header;

     ITF_Memcpy(&header,data,sizeof(header));
     ITF_Memcpy(&header.dwUser.dwLodBias,&m_lod_bias,sizeof(f32));
     ITF_Memcpy(&header.dwUser.dwAlphaBias,&m_AlphaBias,sizeof(u32));
     header.dwUser.dwbHasOneColor = m_HasOneColor?1:0;
     header.dwUser.dwAnisotropy = (int)m_Anisotropy;

     if (swapBigEndian)
         header.swapToBigEndian();
    return writeData(&header,size);
 }
//------------------------------------------------------------------------------------------------------------------------------------

 bool TextureOutputHandler::writeDataRGBA(const void * data, int size)
 {
     if (swapBigEndian)
     {
         u32 countSwap = size/sizeof(u32);
         u8* mem = getBufferSize(size);
         u8* currentMem = mem;
         u32* pSource = (u32*) data;

         for (u32 i=0;i<countSwap;++i)
         {       
             Endian::swapBigEndian(*pSource,currentMem);
             currentMem+=sizeof(u32);
             pSource++;
         }

         return writeData(mem,size);
     }
     else
     {
         return writeData(data,size);
     }
 }

//------------------------------------------------------------------------------------------------------------------------------------
 // Output data.
 bool TextureOutputHandler::writeData(const void * data, int size)
 {
     nvDebugCheck(stream != NULL);
     stream->serialize(const_cast<void *>(data), size);

     progress += size;
     //verbose only if you get an issue
/*     int p = int((100 * progress) / total);

     if (verbose && p != percentage)
     {
         nvCheck(p >= 0);

         percentage = p;
#undef printf
         printf("\r%S %d%% ==> %S//%S", m_srcname,percentage,m_platform,m_shortname);

         //printf("\r%d%%",percentage);
         fflush(stdout);
     }
*/
     return true;
 }

//------------------------------------------------------------------------------------------------------------------------------------
}