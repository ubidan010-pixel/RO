/*   SCE CONFIDENTIAL                                       */
/*   PLAYSTATION(R)3 Programmer Tool Runtime Library 160.008 */
/*   Copyright (C) 2006 Sony Computer Entertainment Inc.    */
/*   All Rights Reserved.                                   */

#include "precompiled_GFXAdapter_GCM.h"

#include <sys/fixed_addr.h> // for memory map

#include "gtfloader.h"
#include "gtfutils.h"

// 
// Helper function
//
// Returns true if it's linear format
// 
static inline uint8_t isLinear( uint8_t format ) 
{
	return (format & CELL_GCM_TEXTURE_LN);
}

// 
// Helper function 
// 
// Returns number of byte per texel. 
// 
static uint32_t getComponentSize(uint8_t format)
{
	switch(format) {
	case CELL_GCM_TEXTURE_B8:
		return 1;
	case CELL_GCM_TEXTURE_A1R5G5B5:
	case CELL_GCM_TEXTURE_A4R4G4B4:
	case CELL_GCM_TEXTURE_R5G6B5:
	case CELL_GCM_TEXTURE_D1R5G5B5:
	case CELL_GCM_TEXTURE_G8B8:
	case CELL_GCM_TEXTURE_R6G5B5:
	case CELL_GCM_TEXTURE_DEPTH16:
	case CELL_GCM_TEXTURE_DEPTH16_FLOAT:
	case CELL_GCM_TEXTURE_X16:
	case CELL_GCM_TEXTURE_R5G5B5A1:
		return 2;
	case CELL_GCM_TEXTURE_A8R8G8B8:
	case CELL_GCM_TEXTURE_DEPTH24_D8:
	case CELL_GCM_TEXTURE_DEPTH24_D8_FLOAT:
	case CELL_GCM_TEXTURE_Y16_X16:
	case CELL_GCM_TEXTURE_X32_FLOAT:
	case CELL_GCM_TEXTURE_D8R8G8B8:
	case CELL_GCM_TEXTURE_Y16_X16_FLOAT:
		return 4;
	case CELL_GCM_TEXTURE_W16_Z16_Y16_X16_FLOAT:
		return 8;
	case CELL_GCM_TEXTURE_W32_Z32_Y32_X32_FLOAT:
		return 16;
	case CELL_GCM_TEXTURE_COMPRESSED_HILO8: // not supported yet
	case CELL_GCM_TEXTURE_COMPRESSED_HILO_S8:
		return 0;
	case CELL_GCM_TEXTURE_COMPRESSED_DXT1:	// should not get here
	case CELL_GCM_TEXTURE_COMPRESSED_DXT23:
	case CELL_GCM_TEXTURE_COMPRESSED_DXT45:
	default:
		return 0;
	}
}

// 
// Helper function
//
// Returns Size of image given width/height/format.
//
static uint32_t getImageSize(uint32_t w, uint32_t h, uint8_t format)
{
	uint32_t size = 0;

	// clear linear/normalize bit to get raw texture bits
	format &= ~(CELL_GCM_TEXTURE_LN|CELL_GCM_TEXTURE_UN);

	switch(format) {
	case CELL_GCM_TEXTURE_COMPRESSED_DXT1:
		size = ((w+3)/4) * ((h+3)/4) * 8;
		break;
	case CELL_GCM_TEXTURE_COMPRESSED_DXT23:
	case CELL_GCM_TEXTURE_COMPRESSED_DXT45:
		size = ((w+3)/4) * ((h+3)/4) * 16;
		break;
	default:
		size = w * h * getComponentSize(format);
	}
	
	ITF_ASSERT( size > 0 );

	return size;
}
// 
// Function name: TexCoordToSwizzledIndex
//
// Description:
//   Calculate the swizzle index of a tex coord .
//   
//   
// Return Value: ndUInt
//   
//   
//   
uint32_t TexCoordToSwizzledIndex( uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height, uint32_t depth )
{
	uint32_t bit;
	uint32_t coord;

	bit = 1;
	coord = 0;
	do
	{
		width >>= 1;
		height >>= 1;
		depth >>= 1;

		if ( width )
		{
			coord += (x & 1) ? bit : 0;
			bit += bit;
		}

		if ( height )
		{
			coord += (y & 1) ? bit : 0;
			bit += bit;
		}

		if ( depth )
		{
			coord += (z & 1) ? bit : 0;
			bit += bit;
		}

		x >>= 1;
		y >>= 1;
		z >>= 1;
	} while ( x + y + z );
	return coord;
}
// 
// Function name: cellGtfCalculateMemorySize
//
// Description:
//   Calculate necessary memory size for texture by given texture attribute.
//   The result calculated size is returned by a pointer passed as size.
//   
// Return Value:
//   Returns  0 on success.
//   Returns -1 on failure.
//   
int cellGtfCalculateMemorySize(CellGtfTextureAttribute* attrib, CellGcmTexture* tex, uint32_t *size)
{
	uint32_t w, h;	
	uint32_t pitch;

	// Error check
	if( attrib == NULL ) return -1;
	if( size == NULL ) return -1;

	ITF_ASSERT( attrib->textureSize > 0 );

	// initialize size to 0 
	*size = 0;
	pitch = tex->pitch;

	// for each cubemap face
	for( uint32_t face=0; face<(tex->cubemap?6:1); face++ ) {

		w = tex->width;
		h = tex->height;

		// for each mipmap level
		for( uint32_t mip=0; mip<tex->mipmap; mip++ ) {
			
			// if linear texture
			if( isLinear(tex->format)) {
				*size += getImageSize(pitch, h, tex->format);
			}
			else {	// swizzle/S3TC
				*size += getImageSize(w, h, tex->format);
			}
			
			// down to next mipmap
			if( w != 1 ) w /= 2;
			if( h != 1 ) h /= 2;
		}

		// Align to 128 byte boundary
		*size = (*size+127)&~127;
	}

	return 0;
}


//
// Function Name: cellGtfUtilIsValidGtf
//
// Description:
//   Validate GTF file by examining GTF header.
// 
// Return Value:
//   Returns 1 if file is valid GTF
//   Returns 0 if file is not valid GTF
//
int cellGtfUtilIsValidGtf( CellGtfFileHeader* header ) 
{
	if( header->size == 0 || header->numTexture == 0 ) {
		return 0;
	}
	// XXX: more error check
	return 1;
}


//
// Function Name: cellGtfUtilCheckMemoryLocation
//
// Description:
//   Checks if @mem is in main memory or local memory @loc.
//   
// 
// Return Value:
//   Returns  0 if memory location is on loc.
//   Returns -1 if memory location is not on loc.
//
int cellGtfUtilCheckMemoryLocation( void* mem, uint8_t loc ) 
{
	//
	// Main memory EA is between TEXT_SEGMENT_BASE_ADDR and RSX_FB_BASE_ADDR. 
	// Local meomry EA is between RSX_FB_BASE_ADDR and RAW_SPU_BASE_ADDR.
	// 
	// NOTE: 
	//   Memory map information obtained from <sys/fixed_addr.h>
	//
	switch( loc ) {
	case CELL_GCM_LOCATION_MAIN:
		return 
		   ((uint32_t)mem>=TEXT_SEGMENT_BASE_ADDR && (uint32_t)mem<RSX_FB_BASE_ADDR)?
		   0 : -1;
	case CELL_GCM_LOCATION_LOCAL:
		return
		   ((uint32_t)mem>=RSX_FB_BASE_ADDR && (uint32_t)mem<RAW_SPU_BASE_ADDR)?
		   0 : -1;
	default:
		return -1;
	}
}

// 
// Function Name: cellGtfUtilGetSurfaceBytePerPixel
//
// Description:
//
// Return Value:
//   Returns byte per pixel for the surface.
//   0 on failure.
//
uint32_t cellGtfUtilGetSurfaceBytePerPixel(const CellGcmSurface* surface, const uint8_t buffer_flag)
{
	uint32_t byte_per_pixel = 0;

	if( buffer_flag < CELL_GTF_ALL_COLOR_BUFFER ) { 
		//
		// color buffer case
		//
		switch(	surface->colorFormat ) {
		case CELL_GCM_SURFACE_B8:
			byte_per_pixel = 1;
			break;
		case CELL_GCM_SURFACE_X1R5G5B5_Z1R5G5B5:
		case CELL_GCM_SURFACE_X1R5G5B5_O1R5G5B5:
		case CELL_GCM_SURFACE_R5G6B5:
		case CELL_GCM_SURFACE_G8B8:
			byte_per_pixel = 2;
			break;
		case CELL_GCM_SURFACE_X8B8G8R8_Z8B8G8R8:
		case CELL_GCM_SURFACE_X8B8G8R8_O8B8G8R8:
		case CELL_GCM_SURFACE_A8B8G8R8:
		case CELL_GCM_SURFACE_X8R8G8B8_Z8R8G8B8:
		case CELL_GCM_SURFACE_X8R8G8B8_O8R8G8B8:
		case CELL_GCM_SURFACE_A8R8G8B8:
		case CELL_GCM_SURFACE_F_X32:
			byte_per_pixel = 4;
			break;
		case CELL_GCM_SURFACE_F_W16Z16Y16X16:
			byte_per_pixel = 8;
			break;
		case CELL_GCM_SURFACE_F_W32Z32Y32X32:
			byte_per_pixel = 16;
			break;
		default:
			ITF_ASSERT_MSG(0, "Unknown color Format\n");
			byte_per_pixel = 0;
		}
	}
	else  {
		//
		// depth buffer case
		//
		switch( surface->depthFormat ) {
		case CELL_GCM_SURFACE_Z16:
			byte_per_pixel = 2;
			break;
		case CELL_GCM_SURFACE_Z24S8:
			byte_per_pixel = 4;
			break;
		default:
			ITF_ASSERT_MSG(0, "Unknown depth Format"); 
			byte_per_pixel = 0;
		}
	}
	return byte_per_pixel;
}

// 
// Function Name: cellGtfUtilSurfaceSize
//
// Description:
//   Calculate the size necessary for GTF to save surface.
//
// Return Value:
//   Size of surface on success.
//   0 on failure.
//
uint32_t cellGtfUtilGetSurfaceSize(const CellGcmSurface* surface, const uint8_t buffer_flag)
{
	uint32_t pitch = 0;

	if( buffer_flag < CELL_GTF_ALL_COLOR_BUFFER ) { 
		//
		// color buffer case
		//
		uint32_t buf_index = (uint32_t)log2((float)buffer_flag);
		pitch = surface->colorPitch[buf_index];
	}
	else  {
		//
		// depth buffer case
		//
		pitch = surface->depthPitch;
	}

	uint32_t bpp = cellGtfUtilGetSurfaceBytePerPixel(surface, buffer_flag);
	return (bpp * pitch * surface->height);
}
