#include "precompiled_GFXAdapter_WII.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <revolution.h>
#include <revolution/sc.h>
#include <revolution/mtx.h>

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_
#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_
#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#include "core/memory/memory_WII.h"

//#pragma global_optimizer off
//#pragma optimization_level 0

namespace ITF
{
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))
                
//----------------------------------------------------------------------------//

void GFXAdapter_WII::loadTextureTGA(const void *_buffer, u32 _bufferSize, unsigned _target, int _level, bool& _useAlpha, bool _is2D, u32 *_width, u32 *_height, unsigned _forceformat)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::loadTextureTGA not implemented");
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_WII::loadFromFileInMemory(Texture *_t, const void* _buffer, u32 _contentSize)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::loadFromFileInMemory not implemented");
    return btrue;
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::GDK_images_TileForWii(void *_dst, const void *_src, const u16 W, const u16 H, const u32 _mipMapLevels)
{
          u32* curDst = (      u32*)_dst;
    const u32* curSrc = (const u32*)_src;

    u32 curW = W;
    u32 curH = H;
    u32 levelsize = curW * curH;

    for(u32 i=0 ; i<=_mipMapLevels ; ++i)
    {
        GDK_buffer_TileForWii(curDst, curSrc, curW, curH, 32);

        curDst += levelsize;
        curSrc += levelsize;

        curW >>= 1;
        curH >>= 1;
        levelsize >>= 2;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::GDK_buffer_TileForWii
(
    void*       _pv_DestImg,
    const void* _pv_SrcImg,
    u32         _u32_Width,
    u32         _u32_Height,
    u32         _u32_BPP
)
{

	const u32		tileSize = 32;	
	u32 			nbXTiles, nbYTiles, nbTexelsPerTileX = 0, nbTexelsPerTileY = 0;		
	u8				u8_BytesPerColor = 0;
	u8			*	currARTilePtr = NULL;
	u8			*	currGBTilePtr = NULL;
	u8			*	currTilePtr = NULL;
	u32 			Ix, Iy; // Image x and y;
	u32 			tileY, tileX;
	const u8	*	srcImage = NULL;
	u32 			tx, ty; // Tile x and y
	u32				u32_Color = 0;
	u16				u16_Color = 0;
	
	// Set tiles and color size according to BPP (if palette, color size is not used)
	switch (_u32_BPP)
	{
		case 32:
			nbTexelsPerTileX = 4;
			nbTexelsPerTileY = 4;
			u8_BytesPerColor = 4;
			break;
			
		case 16:
			nbTexelsPerTileX = 4;
			nbTexelsPerTileY = 4;
			u8_BytesPerColor = 2;
			break;
			
		case 8:
			nbTexelsPerTileX = 8;
			nbTexelsPerTileY = 4;
			break;
			
		default:
			return;
	}
	
	nbXTiles = std::max((u32) 1, (u32)(_u32_Width  / nbTexelsPerTileX));
	nbYTiles = std::max((u32) 1, (u32)(_u32_Height / nbTexelsPerTileY));
	
#ifndef ITF_FINAL
	u32 tiledWidth  = nbXTiles * nbTexelsPerTileX;
	u32 tiledHeight = nbYTiles * nbTexelsPerTileY;
	if((tiledWidth != _u32_Width) || (tiledHeight != _u32_Height))
	{
		ITF_WARNING(NULL, bfalse, "Bad texture size %d x %d, should be at least %d x %d", _u32_Width, _u32_Height, tiledWidth, tiledHeight);
		ITF_MemSet(_pv_DestImg, 0xFFFFFFFF, _u32_Width*_u32_Height*u8_BytesPerColor);
		return;
	}
#endif // ITF_FINAL

	// Destination pointers for mipmap 0
	currARTilePtr = (u8 *) _pv_DestImg;
	currGBTilePtr = (u8 *) _pv_DestImg + tileSize;
	currTilePtr = (u8 *) _pv_DestImg;
	
	// Set source pointers
	srcImage = (u8 *) _pv_SrcImg;	

	// For each tile
	for (tileY = 0; tileY < nbYTiles; tileY++)
	{
		for (tileX = 0; tileX < nbXTiles; tileX++)
		{
			// Fill the tiles texel by texel				
			for (ty = 0 ; ty < nbTexelsPerTileY ; ty++)
			{
				for (tx = 0 ; tx < nbTexelsPerTileX ; tx++)
				{
					Ix = (tileX * nbTexelsPerTileX + tx);
					Iy = (tileY * nbTexelsPerTileY + ty);
					
					ITF_ASSERT_MSG(Ix < _u32_Width && Iy < _u32_Height, "GFXAdapter_WII::GDK_buffer_TileForWii : bad sizes.");

					// Fill dest buffer according to BPP
					switch (_u32_BPP)
					{		
						case 32:
							u32_Color = *(u32 *)(srcImage + u8_BytesPerColor * (Iy * _u32_Width + Ix));
							
							*currARTilePtr = (u8)((u32_Color & 0xFF000000) >> 24);
							*(currARTilePtr + 1) = (u8)((u32_Color & 0x00FF0000) >> 16);
							*currGBTilePtr = (u8)((u32_Color & 0x0000FF00) >> 8);
							*(currGBTilePtr + 1) = (u8)((u32_Color & 0x000000FF) >> 0);
							break;
							
						case 16:
							u16_Color = *(u16 *)(srcImage + u8_BytesPerColor * (Iy * _u32_Width + Ix));
							
							*currTilePtr = (u8)(u16_Color >> 8);
							*++currTilePtr = (u8)(u16_Color);
							break;
							
						case 8:
							*currTilePtr = *(srcImage + _u32_Width * Iy + Ix);
							break;
							
						default:
							*currARTilePtr = 0xFF;
							*(currARTilePtr + 1) = 0xFF;
							*currGBTilePtr = 0x00;
							*(currGBTilePtr + 1) = 0x00;
							break;
					}
					
					currARTilePtr += 2;
					currGBTilePtr += 2;
					currTilePtr++;
				}
			}
			
			currARTilePtr += tileSize;
			currGBTilePtr += tileSize;
		}
	}
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_WII::loadTextureCooked(Texture* _texture, const char* _path)
{
    String filenameCooked = FILESERVER->getCookedName(_path);

    File* fileTexture = FILEMANAGER->openFile(filenameCooked,ITF_FILE_ATTR_READ);
    ITF_ASSERT(fileTexture);

    if (fileTexture)
    {
	    u32 sizeFile = (u32) fileTexture->getLength();

        // Check file validity
#ifndef ITF_FINAL
        if(sizeFile <= sizeof(ITF::DDS_HEADER))
        {
            ITF_WARNING(NULL, bfalse, "Bad file size: %s", _path);
            fileTexture = FILEMANAGER->closeFile(fileTexture);
            return bfalse;
        }
#endif // ITF_FINAL

	    u8 *rawBuf = (u8 *)Memory::alignedMallocCategory(sizeFile, 32, MemoryId::mId_GfxAdapter);
	    ITF_ASSERT_MSG(rawBuf,"LOAD Texture Failed out of memory %s",_path);

	    if (!rawBuf)
        {
            fileTexture = FILEMANAGER->closeFile(fileTexture);
	        return bfalse;
        }

	    u32 bytesRead = 0;
	    ITF_VERIFY(fileTexture->read(rawBuf,sizeFile,&bytesRead));
	    fileTexture = FILEMANAGER->closeFile(fileTexture);

	    DDS_HEADER header;
	    ITF_Memcpy(&header,rawBuf,sizeof(header));

        // Check header
#ifndef ITF_FINAL
        if(memcmp(&header.dwMagic, " SDD", 4) != 0)
        {
            ITF_WARNING(NULL, bfalse, "Bad file header: %s", _path);
            Memory::alignedFree(rawBuf);
            return bfalse;
        }
#endif // ITF_FINAL

        Size siz((f32) header.dwWidth,(f32) header.dwHeight);
	    setTextureSize(_texture, siz);
	    
	    _texture->m_mipMapLevels = header.dwMipMapCount;

        // Texture LOD parameters
        f32 min_lod         = 0.f;
        f32 max_lod         = 0.f;
        f32 lod_bias        = 0.f;
        GXTexFilter min_flt = GX_LIN_MIP_LIN;
        GXTexFilter max_flt = GX_LINEAR;
        GXBool biasclamp    = GX_FALSE;
        GXBool edgelod      = GX_TRUE;
        GXBool bUseMimap    = _texture->m_mipMapLevels ? GX_TRUE : GX_FALSE;
        u32 AlphaBias       = 0;
        bbool bHasColors    = btrue;
        GXAnisotropy aniso;
        switch(header.dwUser.dwAnisotropy)
        {
            case 1 : aniso  = GX_ANISO_2;
                break;
            case 2 : aniso  = GX_ANISO_1;
                break;
            default : aniso  = GX_ANISO_4;
                break;
        }

        if(bUseMimap)
        {
            min_flt  = GX_LIN_MIP_LIN;
            max_lod  = _texture->m_mipMapLevels;

            ITF_Memcpy(&lod_bias, &header.dwUser.dwLodBias, sizeof(f32));
            ITF_Memcpy(&AlphaBias, &header.dwUser.dwAlphaBias, sizeof(u32));
            bHasColors = header.dwUser.dwbHasOneColor ? bfalse: btrue;

            if((lod_bias < -4.f) || (lod_bias > 3.99f))
            {
                ITF_WARNING(NULL, bfalse, "Unsupported lod bias value %f for %s", lod_bias, StringConverter(filenameCooked).getChar());
                lod_bias = 0.f;
            }
        }

		lod_bias -= 0.70f;

        ITF_ASSERT((min_lod>=0.f) && (max_lod>=0.f));
        ITF_ASSERT((min_lod<=10.f) && (max_lod<=10.f));
        ITF_ASSERT((min_lod<=max_lod));

        if(!bHasColors && max_lod<1.f)
            bHasColors = btrue;

        if(AlphaBias && max_lod<1.f)
            AlphaBias = 0;
    
	    switch (header.format.dwFourCC)
	    {
	        case MAKEFOURCC('D', 'X', 'T', '1'):
	            _texture->m_pixFormat = Texture::PF_DXT1;
	            break;
#ifndef ITF_FINAL
	        case MAKEFOURCC('D', 'X', 'T', '2'):
                {
	            StringConverter conv(filenameCooked);
                char* ctrs = (char*)conv.getChar();
	            ITF_ERROR(NULL, bfalse,"Texture %s use DXT2 format : %dx%d", ctrs, header.dwWidth, header.dwHeight);
	            _texture->m_pixFormat = Texture::PF_DXT2;
	            break;
                }
	        case MAKEFOURCC('D', 'X', 'T', '3'):
                {
	            StringConverter conv(filenameCooked);
                char* ctrs = (char*)conv.getChar();
	            ITF_ERROR(NULL, bfalse,"Texture %s use DXT3 format : %dx%d", ctrs, header.dwWidth, header.dwHeight);
	            _texture->m_pixFormat = Texture::PF_DXT3;
	            break;
                }
	        case MAKEFOURCC('D', 'X', 'T', '4'):
                {
	            StringConverter conv(filenameCooked);
                char* ctrs = (char*)conv.getChar();
	            ITF_ERROR(NULL, bfalse,"Texture %s use DXT4 format : %dx%d", ctrs, header.dwWidth, header.dwHeight);
	            _texture->m_pixFormat = Texture::PF_DXT4;
	            break;
                }
	        case MAKEFOURCC('D', 'X', 'T', '5'):
                {
	            StringConverter conv(filenameCooked);
                char* ctrs = (char*)conv.getChar();
	            ITF_ERROR(NULL, bfalse,"Texture %s use DXT5 format : %dx%d", ctrs, header.dwWidth, header.dwHeight);
	            _texture->m_pixFormat = Texture::PF_DXT5;
	            break;
                }
#endif
	        case MAKEFOURCC('C', 'M', 'P', 'A'):
	            _texture->m_pixFormat = Texture::PF_CMPA;
	            break;
	        default:
	            {	        
	            _texture->m_pixFormat = Texture::PF_RGBA;	            
	            #ifndef ITF_FINAL
	            StringConverter conv(filenameCooked);
                char* ctrs = (char*)conv.getChar();
	            ITF_WARNING(NULL, bfalse,"Texture %s use RGBA format : %dx%d", ctrs, header.dwWidth, header.dwHeight);
	            #endif
	            }
	            break;
	    }

	    //loadFromMemory(rawBuf, sizeo,_texture, PixFormat ,sizeFile,btrue,header.dwMipMapCount);
	 
	    unsigned char* currentBuffer = (unsigned char*)((u8*)rawBuf+sizeof(ITF::DDS_HEADER));
	    unsigned long bufferSize = sizeFile - sizeof(ITF::DDS_HEADER);
	    unsigned long finalRGBSize = bufferSize / 2;
	    unsigned long finalALPHASize = finalRGBSize;
        
        // LockRendering to avoid calling GXInitTexObj (in loading thread) between GXBegin and GXEnd (in main thread)
	    switch(_texture->m_pixFormat)
	    {
	        case Texture::PF_CMPA:
            {
                u32 decAlpha = 0;
                u32 decRGB = 0;
                u32 texSurf = (_texture->getSizeX() * _texture->getSizeY()) / 2;
                if(!bHasColors)
                {
                    for(u32 i = 0; i < _texture->m_mipMapLevels; ++i)
                    {
                        decRGB += texSurf;
                        texSurf /= 4;
                    }
                    finalRGBSize -= decRGB;
                }

                if(AlphaBias)
                {
                    decAlpha = texSurf;
                    finalALPHASize -= decAlpha;
                }
#ifndef ITF_FINAL
                sprintf(_texture->m_dbgNfo, "%c%c", decRGB?'C':' ', decAlpha?'A':' ');
#endif
                MEM_M_PushExt(MEM_C_MEM2_ALLOC);
                _texture->m_adapterimplementationData = (u32)Memory::alignedMallocCategory(finalRGBSize+finalALPHASize, 32, MemoryId::mId_Textures);
                MEM_M_PopExt();

                // copy RGB
                ITF_Memcpy(  (u8*)_texture->m_adapterimplementationData, (u8*)currentBuffer + decRGB, finalRGBSize);

                // Copy alpha
                ITF_Memcpy(  (u8*)_texture->m_adapterimplementationData + finalRGBSize, (u8*)currentBuffer + bufferSize/2 + decAlpha, finalALPHASize);

		        DCFlushRange((u8*)_texture->m_adapterimplementationData, finalRGBSize + finalALPHASize);

                // texture obj
                LockRendering();

                // RGB chanel
                if(bHasColors)
                {
            	    GXInitTexObj(&_texture->m_TexObj, (u8*)_texture->m_adapterimplementationData,
            	                 _texture->getSizeX(), _texture->getSizeY(), GX_TF_CMPR, GX_REPEAT, GX_REPEAT, bUseMimap);
            	    GXInitTexObjLOD(&_texture->m_TexObj,
                                    min_flt, max_flt, min_lod, max_lod, lod_bias, biasclamp, edgelod, aniso);
                }
                else
                {
            	    GXInitTexObj(&_texture->m_TexObj, (u8*)_texture->m_adapterimplementationData,
            	                 _texture->getSizeX() >> _texture->m_mipMapLevels, _texture->getSizeY() >> _texture->m_mipMapLevels, GX_TF_CMPR, GX_REPEAT, GX_REPEAT, GX_FALSE);
            	    GXInitTexObjLOD(&_texture->m_TexObj,
                                    min_flt, max_flt, 0.f, 0.f, lod_bias, biasclamp, edgelod, aniso);
                }

                // alpha chanel
                if(!AlphaBias)
                {
        	        GXInitTexObj(&_texture->m_TexObjA, (u8*)_texture->m_adapterimplementationData + finalRGBSize,
        	                    _texture->getSizeX(), _texture->getSizeY(), GX_TF_CMPR, GX_REPEAT, GX_REPEAT, bUseMimap);
        	        GXInitTexObjLOD(&_texture->m_TexObjA,
                                    min_flt, max_flt, min_lod, max_lod, lod_bias, biasclamp, edgelod, aniso);
                }
                else
                {
        	        GXInitTexObj(&_texture->m_TexObjA, (u8*)_texture->m_adapterimplementationData + finalRGBSize,
            	                _texture->getSizeX()/2, _texture->getSizeY()/2, GX_TF_CMPR, GX_REPEAT, GX_REPEAT, bUseMimap);
            	    GXInitTexObjLOD(&_texture->m_TexObjA,
                                    min_flt, max_flt, min_lod, max_lod-1.f, lod_bias, biasclamp, edgelod, aniso);
                }
                UnlockRendering();
                bufferSize = finalRGBSize + finalALPHASize;
                break;
            }
	        case Texture::PF_DXT1:
            {
                MEM_M_PushExt(MEM_C_MEM2_ALLOC);
                _texture->m_adapterimplementationData = (u32)Memory::alignedMallocCategory(bufferSize, 32, MemoryId::mId_Textures);
                MEM_M_PopExt();

                ITF_Memcpy((u8*)_texture->m_adapterimplementationData, currentBuffer, bufferSize);
    		    DCFlushRange((u8*)_texture->m_adapterimplementationData, bufferSize);

                LockRendering();
            	GXInitTexObj(&_texture->m_TexObj, (u8*)_texture->m_adapterimplementationData,
            	             _texture->getSizeX(), _texture->getSizeY(), GX_TF_CMPR, GX_REPEAT, GX_REPEAT, bUseMimap);
            	GXInitTexObjLOD(&_texture->m_TexObj,
                                min_flt,       // min_filt
                                max_flt,       // mag_filt
                                min_lod,       // min_lod    
                                max_lod,       // max_lod    
                                lod_bias,      // lod_bias   
                                biasclamp,     // bias_clamp 
                                edgelod,       // do_edge_lod 
                                aniso);        // max_aniso  	                
                UnlockRendering();                                            
    		    break;
            }            
            default :
            {
                MEM_M_PushExt(MEM_C_MEM2_ALLOC);
                _texture->m_adapterimplementationData = (u32)Memory::alignedMallocCategory(bufferSize, 32, MemoryId::mId_Textures);
                MEM_M_PopExt();
    		    GDK_images_TileForWii((u8*)_texture->m_adapterimplementationData, currentBuffer, header.dwWidth, header.dwHeight, _texture->m_mipMapLevels);
    		    DCFlushRange((u8*)_texture->m_adapterimplementationData, bufferSize);

                LockRendering();
            	GXInitTexObj(&_texture->m_TexObj, (u8*)_texture->m_adapterimplementationData,
            	             _texture->getSizeX(), _texture->getSizeY(), GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, bUseMimap);
            	GXInitTexObjLOD(&_texture->m_TexObj,
                                min_flt,       // min_filt
                                max_flt,       // mag_filt
                                min_lod,       // min_lod    
                                max_lod,       // max_lod    
                                lod_bias,      // lod_bias   
                                biasclamp,     // bias_clamp 
                                edgelod,       // do_edge_lod
                                aniso);        // max_aniso
                UnlockRendering();
    		    break;
		    }
        }

		Memory::alignedFree(rawBuf);

	    generateTexture(_texture);

		Size TexSize(header.dwWidth, header.dwHeight);
	    setTextureSize(_texture, TexSize);
	    _texture->m_datasizeX = (u32)TexSize.d_width;
	    _texture->m_datasizeY = (u32)TexSize.d_height;
	    updateCachedScaleValues(_texture);

#ifndef ITF_FINAL
        _texture->setMemoryUsage(bufferSize);
#endif //ITF_FINAL
    }
        
    RESOURCE_MANAGER->addResourceAsLoaded(_texture);
    return btrue;
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_WII::loadTexture(Texture* _t, const char* _path, u32 _alhpa)
{
	if(_t->isCooked())
		return loadTextureCooked(_t, _path);
	return btrue;
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::SetTexture(int _Sampler, Texture* _Texture, bbool _linearFiltering)
{
	if (!_Texture || _Texture->m_adapterimplementationData == 0)
	{
		m_Texture = NULL;
		return;
	}
	
	GXTexMapID  texMapId = (GXTexMapID) (GX_TEXMAP0 + _Sampler);	   

    m_Texture = _Texture;
	
    GXInitTexObjWrapMode(&_Texture->m_TexObj,  m_TexWrapS, m_TexWrapT);			    	
	GXLoadTexObj(&_Texture->m_TexObj, texMapId);    
	if(_Texture->m_pixFormat == Texture::PF_CMPA)
	{
	    GXInitTexObjWrapMode(&_Texture->m_TexObjA, m_TexWrapS, m_TexWrapT);    	
    	GXLoadTexObj(&_Texture->m_TexObjA, (GXTexMapID) (GX_TEXMAP1 + _Sampler));
	}
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::SetTextureBind(int _Sampler, unsigned int _Bind, bbool _linearFiltering)
{
	if (!_Bind)
	{
		m_Texture = NULL;
		return;
	}

    SetTexture(_Sampler, (Texture*)_Bind, _linearFiltering);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::lockTexture(Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::lockTexture not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::unlockTexture(Texture* _tex)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::unlockTexture not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::generateTexture(Texture* _texture)
{
    // NOTHING
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::cleanupTexture(Texture* _texture)
{
    cleanupBaseTexture(_texture);
    
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::cleanupRenderTarget(renderTarget* _target)
{
	if (_target)
	    cleanupBaseTexture(_target->getTexture());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::cleanupBaseTexture(Texture* _texture)
{
	if (!_texture) return;

    // Check if a normal tex is associated to this texture
    ITF_MAP<Texture*,Texture*>::iterator it = m_NormalTex.find(_texture);
    if(it != m_NormalTex.end())
    {
        RESOURCE_MANAGER->removeUserToResourceData(it->second);
        cleanupBaseTexture(it->second);
        m_NormalTex.erase(it);
    }

    // Check if it's an AFX_RT's texture
    for(u32 i=0 ; i<AFX_MAX_FULL_RT ; ++i)
    {
        if(mp_AFX_RT[i] && (mp_AFX_RT[i]->getTexture() == _texture))
        {
            mp_AFX_RT[i] = NULL;
            break;
        }
    }
    for(u32 i=AFX_MAX_FULL_RT ; i<AFX_MAX_RT ; ++i)
    {
        if(mp_AFX_RT[i] && (mp_AFX_RT[i]->getTexture() == _texture))
        {
            _texture->m_adapterimplementationData = 0;
            mp_AFX_RT[i] = NULL;
        }
    }

    // Free the texture
    LockRendering();
    u8* pdata = (u8*)_texture->m_adapterimplementationData;
    if(pdata)
	    Memory::alignedFree(pdata);
	_texture->m_adapterimplementationData = 0;
    GXInvalidateTexAll();
    UnlockRendering();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setTextureSize(Texture* _texture, const Size& _size)
{
    _texture->setSize((u32)_size.d_width, (u32)_size.d_height);
}

//----------------------------------------------------------------------------//

unsigned char* GFXAdapter_WII::grabTexture(Texture* _texture)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::grabTexture not implemented");
	return NULL;
}

 
//----------------------------------------------------------------------------//

u32 GFXAdapter_WII::getMaxTextureSize()
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::getMaxTextureSize not implemented");
    return 1;
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format,const u32 _buffer_sizeload,bbool bCooked, u32 _mipmapCount)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::loadFromMemory");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::saveToMemory(void* _buffer, Texture* _Tex)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::saveToMemory not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setSamplerState(u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value)
{
	GXTexWrapMode *pt_WrapMode = &m_TexWrapS;
	switch(_type)
	{
	case GFXSAMP_ADDRESSU:
		pt_WrapMode = &m_TexWrapS;
		break;
	case GFXSAMP_ADDRESSV:
		pt_WrapMode = &m_TexWrapT;
		break;
	}

	*pt_WrapMode = GX_REPEAT;
	switch (_value)
	{
	case GFXTADDRESS_WRAP:
		*pt_WrapMode = GX_REPEAT;
		break;
	case GFXTADDRESS_MIRROR:
		*pt_WrapMode = GX_MIRROR;
		break;
	case GFXTADDRESS_CLAMP:
		*pt_WrapMode = GX_CLAMP;
		break;
	default:
	    ITF_ASSERT_MSG(0, "Unknown wrap mode (%d)", _value);
	    break;
	}
}

//----------------------------------------------------------------------------//

GXTexFmt GFXAdapter_WII::PixFormat_TO_CopyTexFmt(Texture::PixFormat _format)
{
    switch(_format)
    {
    case Texture::PF_RGBA:
        return GX_TF_RGBA8;

    case Texture::PF_CMPA:
        return GX_TF_RGB565;

    case Texture::PF_LA8:
        return GX_CTF_RG8;

    default:
        ITF_ASSERT("Unsupported format for CopyTex.");
        break;
    }

    return GX_TF_RGBA8;
}

GXTexFmt GFXAdapter_WII::PixFormat_TO_GXTexFmt(Texture::PixFormat _format)
{
    switch(_format)
    {
    case Texture::PF_RGBA:
        return GX_TF_RGBA8;

    case Texture::PF_CMPA:
        return GX_TF_RGB565;

    case Texture::PF_LA8:
        return GX_TF_IA8;
        
    default:
        ITF_ASSERT("Unsupported format for dynamic texture.");
        break;
    }

    return GX_TF_RGBA8;
}

Texture::PixFormat  GFXAdapter_WII::GXTexFmt_TO_PixFormat(GXTexFmt _format)
{
    switch(_format)
    {
    case GX_TF_RGBA8:
        return Texture::PF_RGBA;
    case GX_TF_RGB565:
        return Texture::PF_CMPA;
    case GX_TF_IA8:
        return Texture::PF_LA8;
    default:
        ITF_ASSERT("Unsupported format");
        break;
    }

    return Texture::PF_RGBA;
}

void GFXAdapter_WII::createDynamicTexture(Texture *_p_Tex, u16 _W, u16 _H, Texture::PixFormat _format)
{
    ITF_ASSERT(_p_Tex);
    ITF_ASSERT(_W && _H);

    unsigned long rgbBufferSize = GXGetTexBufferSize(_W, _H, PixFormat_TO_GXTexFmt(_format), GX_FALSE, 0);
    unsigned long aBufferSize = 0;
    if(_format == Texture::PF_CMPA)
    {
        aBufferSize = GXGetTexBufferSize(_W, _H, GX_TF_I8, GX_FALSE, 0);
    }
    unsigned long bufferSize = rgbBufferSize + aBufferSize;

    MEM_M_PushExt(MEM_C_MEM2_ALLOC);
    _p_Tex->m_adapterimplementationData = (u32)Memory::alignedMallocCategory(bufferSize, 32, MemoryId::mId_GfxAdapter);
    MEM_M_PopExt();

    ITF_MemSet((u8*)_p_Tex->m_adapterimplementationData, 0, bufferSize);
    DCFlushRange((u8*)_p_Tex->m_adapterimplementationData, bufferSize);

    initializeDynamicTexture(_p_Tex, _W, _H, _format, rgbBufferSize, aBufferSize);

#ifndef ITF_FINAL
    _p_Tex->setMemoryUsage(bufferSize);
#endif //ITF_FINAL
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::initializeDynamicTexture(Texture *_p_Tex, u16 _W, u16 _H, Texture::PixFormat _format, unsigned long _rgbBufferSize, unsigned long _aBufferSize)
{
    ITF_ASSERT(_p_Tex);
    ITF_ASSERT(_W && _H);

    _p_Tex->m_mipMapLevels  = 0;
    _p_Tex->m_pixFormat     = _format;

    GXTexFmt texFmt = PixFormat_TO_GXTexFmt(_format);

    GXInitTexObj
    (
        &_p_Tex->m_TexObj,
        (u8*)_p_Tex->m_adapterimplementationData,
        _W,
        _H,
        texFmt,
        GX_CLAMP,
        GX_CLAMP,
        GX_FALSE
    );
    GXInitTexObjLOD
    (
        &_p_Tex->m_TexObj,
        GX_LINEAR,      // min_filt
        GX_LINEAR,      // mag_filt
        0.f,            // min_lod    
        0.f,            // max_lod    
        0.f,            // lod_bias   
        GX_FALSE,       // bias_clamp 
        GX_FALSE,       // do_edge_lod 
        GX_ANISO_1      // max_aniso
    );

    if(_aBufferSize)
    {
        GXInitTexObj
        (
            &_p_Tex->m_TexObjA,
            (u8*)_p_Tex->m_adapterimplementationData + _rgbBufferSize,
            _W,
            _H,
            GX_TF_I8,
            GX_CLAMP,
            GX_CLAMP,
            GX_FALSE
        );
        GXInitTexObjLOD
        (
            &_p_Tex->m_TexObjA,
            GX_LINEAR,      // min_filt
            GX_LINEAR,      // mag_filt
            0.f,            // min_lod    
            0.f,            // max_lod    
            0.f,            // lod_bias   
            GX_FALSE,       // bias_clamp 
            GX_FALSE,       // do_edge_lod 
            GX_ANISO_1      // max_aniso
        );
    }

    generateTexture(_p_Tex);

    Size TexSize(_W, _H);
    setTextureSize(_p_Tex, TexSize);
    _p_Tex->m_datasizeX = (u32)TexSize.d_width;
    _p_Tex->m_datasizeY = (u32)TexSize.d_height;
    updateCachedScaleValues(_p_Tex);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::clearRenderTarget(renderTarget* _target, u32 _color)
{
    // Not essential
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::initialiseRenderTarget(renderTarget* _target)
{
    Texture* p_Tex = _target->getTexture();
    if(p_Tex->m_adapterimplementationData) return;

    u16 w = _target->m_area[2] - _target->m_area[0];
    u16 h = _target->m_area[3] - _target->m_area[1];
    if(!w || !h) return;

    createDynamicTexture(p_Tex, w, h, (w >= (u16)mv_StaticUtilScreenSize.m_x) ? Texture::PF_RGBA : Texture::PF_CMPA);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::enableRenderTarget(renderTarget* _target)
{
    LockRendering();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::disableRenderTarget(renderTarget* _target)
{
    if(_target)
    {
        Texture *p_Tex = _target->getTexture();
        if(p_Tex && p_Tex->m_adapterimplementationData)
        {
            copyCurrentColorBuffer(p_Tex, _target->m_area[0], _target->m_area[1], _target->m_area[2], _target->m_area[3]);
        }
    }

    UnlockRendering();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::resizeRenderTarget(renderTarget* _target, const Size& _size)
{
    _target->clean();

    _target->m_area[0] = 0;
    _target->m_area[1] = 0;
    _target->m_area[2] = (int)_size.d_width;
    _target->m_area[3] = (int)_size.d_height;

    initialiseRenderTarget(_target);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear)
{
    // Enable render target
    ITF_ASSERT(_rt);
    _rt->set();

    // Projection
    setOrthoView(xmin, xmax, ymin, ymax);

    // Viewport
    if(Rmode->field_rendering)
        GXSetViewportJitter((f32)_rt->m_area[0], (f32)_rt->m_area[1], (f32)_rt->m_area[2], (f32)_rt->m_area[3], -1.f, 1.f, VIGetNextField());
    else
        GXSetViewport((f32)_rt->m_area[0], (f32)_rt->m_area[1], (f32)_rt->m_area[2], (f32)_rt->m_area[3], -1.f, 1.f);
    GXSetScissor(_rt->m_area[0], _rt->m_area[1], _rt->m_area[2], _rt->m_area[3]);

    // Immediate clear
    if(_mustClear)
    {
        setAlphaBlend(GFX_BLEND_COPY);
        drawScreenQuad(xmin, ymin, xmax, ymax, 0.f, _rt->getBackGroundColor(), btrue);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::startImpostorAlphaRendering(renderTarget* _rt)
{
    // Disable render target = copy RGB to texture (pass color)
    // + set alpha bg color for pass alpha
    ITF_ASSERT(_rt);
    f32 f32_Alpha = ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_A(_rt->getBackGroundColor()));
    clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, Color(f32_Alpha,f32_Alpha,f32_Alpha,f32_Alpha).getAsU32());
    _rt->unSet();

    // Reset clear color
    clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, CONFIG->m_BGColor[0], CONFIG->m_BGColor[1], CONFIG->m_BGColor[2], 0.f);

    // Active alpha pass + enable render target (pass alpha)
    m_CurrentRTPass = RT_PASS_ALPHA;
    _rt->set();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::stopImpostorRendering(renderTarget* _rt)
{
    // Disable render target = copy to texture
    ITF_ASSERT(_rt);
    _rt->unSet();

    m_CurrentRTPass = RT_PASS_COLOR;

    /// restore 3d View.
    CAMERA->apply();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::copyCurrentColorBuffer(u32 _rt)
{
    // Copy frame buffer
    ITF_ASSERT((_rt < AFX_MAX_RT) && mp_AFX_RT[_rt]);
    mp_AFX_RT[_rt]->set();
    mp_AFX_RT[_rt]->unSet();
}

void GFXAdapter_WII::copyCurrentColorBuffer(Texture *_p_Tex, u16 l, u16 t, u16 w, u16 h)
{
    // Copy frame buffer
    ITF_ASSERT(_p_Tex && _p_Tex->m_adapterimplementationData);

    GXSetZMode(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);

    GXSetCopyFilter(GX_FALSE, NULL, GX_FALSE, NULL);

    GXSetTexCopySrc(l, t, w, h);

    if((m_CurrentRTPass == RT_PASS_ALPHA) && (_p_Tex->m_pixFormat == Texture::PF_CMPA))
    {
        GXSetTexCopyDst(_p_Tex->getSizeX(), _p_Tex->getSizeY(), GX_CTF_G8, GX_FALSE);
        GXCopyTex(GXGetTexObjData(&_p_Tex->m_TexObjA), mb_ClearOnCopy ? GX_TRUE : GX_FALSE);
    }
    else
    {
        GXSetTexCopyDst(_p_Tex->getSizeX(), _p_Tex->getSizeY(), PixFormat_TO_CopyTexFmt(_p_Tex->m_pixFormat), GX_FALSE);
        GXCopyTex(GXGetTexObjData(&_p_Tex->m_TexObj), mb_ClearOnCopy ? GX_TRUE : GX_FALSE);
    }

    GXPixModeSync();
    GXInvalidateTexAll();

    GXSetCopyFilter(Rmode->aa, Rmode->sample_pattern, m_VFilterEnable, Rmode->vfilter);

    renderContext_SetDefault();

    // Reset clear color
    if(mb_ClearOnCopy)
        clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, CONFIG->m_BGColor[0], CONFIG->m_BGColor[1], CONFIG->m_BGColor[2], 0.f);
}

} // namespace ITF
