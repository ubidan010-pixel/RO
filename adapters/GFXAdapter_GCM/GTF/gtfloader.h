/*   SCE CONFIDENTIAL                                       */
/*   PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004 */
/*   Copyright (C) 2006 Sony Computer Entertainment Inc.    */
/*   All Rights Reserved.                                   */

// ITF -> slightly modified

#ifndef GTF_LOADER_H_
#define GTF_LOADER_H_


/* GTF texture structure */
#include "gtftexture.h"

struct CellGcmSurface;

enum {
	CELL_GTF_COLOR_BUFFER_0   = 0x01,
	CELL_GTF_COLOR_BUFFER_1   = 0x02,
	CELL_GTF_COLOR_BUFFER_2   = 0x04,
	CELL_GTF_COLOR_BUFFER_3   = 0x08,
	CELL_GTF_ALL_COLOR_BUFFER = 0x0F,
	CELL_GTF_DEPTH_BUFFER     = 0x10, 
	CELL_GTF_ALL_BUFFER       = 0x1F
};

	/*   Utils   */
int cellGtfCalculateMemorySize(CellGtfTextureAttribute* attrib, struct CellGcmTexture* tex, uint32_t *size);

	/*   Load API from Memory pool   */
int cellGtfReadFileHeaderFromMemory(const uint8_t* gtfptr, 
		                            CellGtfFileHeader* header );

int cellGtfReadTextureAttributeFromMemory(const uint8_t* gtfptr, 
		                                  uint32_t id, 
										  CellGtfTextureAttribute* attribs,
                                          struct CellGcmTexture * tex);

int cellGtfLoadFromMemory(const uint8_t* gtfptr, uint32_t id, 
		                  CellGtfTextureAttribute** attrib);

int cellGtfSaveSurfaceToMem(void* mem, struct CellGcmSurface* surface, const uint8_t flag);

#endif // GTF_LOADER_H_

