/*   SCE CONFIDENTIAL                                       */
/*   PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004 */
/*   Copyright (C) 2006 Sony Computer Entertainment Inc.    */
/*   All Rights Reserved.                                   */

#ifndef __GTF_UTILS_H__
#define __GTF_UTILS_H__

#include "../GCM_API.h"
#include "gtfloader.h"

#ifdef CELL_GTF_DEBUG // {

#define dprintf(x) printf x; fflush(stdout);

static inline void cellGtfDebugPrintSurface( CellGcmSurface* surface ) 
{
	printf( "********** surface *********\n" );
	printf( "type:             0x%x\n", surface->type );
	printf( "antialias:        0x%x\n", surface->antialias );

	printf( "colorFormat:      0x%x\n", surface->colorFormat);
	printf( "colorTarget:      0x%x\n", surface->colorTarget);
	printf( "colorLocation[0]: 0x%x\n", surface->colorLocation[0] );
	printf( "colorLocation[1]: 0x%x\n", surface->colorLocation[1] );
	printf( "colorLocation[2]: 0x%x\n", surface->colorLocation[2] );
	printf( "colorLocation[3]: 0x%x\n", surface->colorLocation[3] );
	printf( "colorOffset[0]:   0x%x\n", surface->colorOffset[0] );
	printf( "colorOffset[1]:   0x%x\n", surface->colorOffset[1] );
	printf( "colorOffset[2]:   0x%x\n", surface->colorOffset[2] );
	printf( "colorOffset[3]:   0x%x\n", surface->colorOffset[3] );
	printf( "colorPitch[0]:    0x%x\n", surface->colorPitch[0] );
	printf( "colorPitch[1]:    0x%x\n", surface->colorPitch[1] );
	printf( "colorPitch[2]:    0x%x\n", surface->colorPitch[2] );
	printf( "colorPitch[3]:    0x%x\n", surface->colorPitch[3] );
	printf( "depthFormat:      0x%x\n", surface->depthFormat );
	printf( "depthLocation:    0x%x\n", surface->depthLocation );
	printf( "depthOffset:      0x%x\n", surface->depthOffset );
	printf( "depthPitch:       0x%x\n", surface->depthPitch );
	printf( "width:            0x%x\n", surface->width );
	printf( "height:           0x%x\n", surface->height );
	printf( "x:                0x%x\n", surface->x );
	printf( "y:                0x%x\n", surface->y );
	fflush(stdout);
}

static inline void cellGtfDebugPrintTexture( CellGcmTexture* tex ) 
{
	printf( "********** texture ***************\n" );
	printf( "Tex.format   : 0x%x (0x%x)", (uint32_t)tex->format, (tex->format & ~(0x20|0x40))); 
	// swizzle?
	if( tex->format & CELL_GCM_TEXTURE_LN ) printf( " Linear" );
	else printf( " Swizzle" );
	if( tex->format & CELL_GCM_TEXTURE_UN ) printf( " Unnormalize" );
	else printf( " Normalize" );
	printf( "\n" );
	printf( "Tex.mipmap   : 0x%x\n", (uint32_t)tex->mipmap ); 
	printf( "Tex.dimension: 0x%x\n", (uint32_t)tex->dimension ); 
	printf( "Tex.cubemap  : 0x%x\n", (uint32_t)tex->cubemap ); 
	printf( "Tex.remap    : 0x%x\n", (uint32_t)tex->remap ); 
	printf( "Tex.width    : 0x%x\n", (uint32_t)tex->width ); 
	printf( "Tex.height   : 0x%x\n", (uint32_t)tex->height ); 
	printf( "Tex.depth    : 0x%x\n", (uint32_t)tex->depth ); 
	printf( "Tex.pitch    : 0x%x\n", (uint32_t)tex->pitch ); 
	printf( "Tex.location : 0x%x\n", (uint32_t)tex->location ); 
	printf( "Tex._padding : 0x%x\n", (uint32_t)tex->_padding ); 
	printf( "Tex.offset   : 0x%x\n", (uint32_t)tex->offset ); 
	fflush(stdout);
}

#else   // } {

#define dprintf(x)

static inline void cellGtfDebugPrintSurface( CellGcmSurface* surface ) 
{
	(void) surface;	// do nothing
}

static inline void cellGtfDebugPrintTexture( CellGcmTexture* tex ) 
{
	(void) tex; // do nothing
}
#endif  // }


// Prototypes
int			cellGtfUtilIsValidGtf( CellGtfFileHeader* header );
int			cellGtfUtilCheckMemoryLocation( void* mem, uint8_t loc );
uint32_t	cellGtfUtilGetSurfaceBytePerPixel(const CellGcmSurface* surface, const uint8_t buffer_flag);
uint32_t	cellGtfUtilGetSurfaceSize(const CellGcmSurface* surface, const uint8_t buffer_flag);
uint32_t	TexCoordToSwizzledIndex( uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height, uint32_t depth );

#endif // __GTF_UTILS_H__
