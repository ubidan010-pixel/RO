/*   SCE CONFIDENTIAL                                       */
/*   PLAYSTATION(R)3 Programmer Tool Runtime Library 160.008 */
/*   Copyright (C) 2006 Sony Computer Entertainment Inc.    */
/*   All Rights Reserved.                                   */

#include "precompiled_GFXAdapter_GCM.h"

#include "gtfloader.h"
#include "gtfutils.h"


// Function Name: cellGTFReadInfoFromMemory
// 
// Description:
//  Read header part of GTF texture, and returns them.
//
// Return Value:
//   Returns  0 on success.
//   Returns -1 on failure.
// 
int cellGtfReadFileHeaderFromMemory( const uint8_t* gtfptr, CellGtfFileHeader* header )
{
	uint32_t index = 0;

	// Error check
	ITF_ASSERT( header != NULL );
	if(gtfptr == NULL || header == NULL) return -1;

	// Read File Header
	memcpy( header, &gtfptr[index], sizeof(CellGtfFileHeader));
	
	// Simple header validation 
	if( ! cellGtfUtilIsValidGtf( header )) {
		return -1;
	}

	return 0;
}

// Function Name: cellGTFReadAttributeFromMemory
// 
// Description:
//  Read header part of GTF texture, and returns them.
//
// Return Value:
//   Returns  0 on success.
//   Returns -1 on failure.
// 
int cellGtfReadTextureAttributeFromMemory( const uint8_t* gtfptr, uint32_t id, 
                    CellGtfTextureAttribute* attrib, CellGcmTexture * tex)
{
	uint32_t index = 0;
	int ret = 0;	// return value
	CellGtfFileHeader header;

	// Error check
	if(gtfptr == NULL || attrib == NULL) return -1;

	// Read header part
	memcpy( &header, &gtfptr[index], sizeof(CellGtfFileHeader));
	index += sizeof(CellGtfFileHeader);

	// Simple file validation 
	if( ! cellGtfUtilIsValidGtf( &header )) {
		return -1;
	}

	// Read Texture Attribute
	uint32_t count;
	for( count=0; count<header.numTexture; count++ ) {
		memcpy(attrib, &gtfptr[index], sizeof(CellGtfTextureAttribute));
		index += sizeof(CellGtfTextureAttribute);
        memcpy(tex, &gtfptr[index], sizeof(CellGcmTexture));
        index += sizeof(CellGcmTexture);

		// if ID matches, then break out this loop
		if( id == attrib->id ) {
			break;
		}
	}

	if( count == header.numTexture ) {
		fprintf( stderr, "ERROR: ID %d not found\n", id );
		ret = -1;
	}

	return ret;
}

//
// Function Name: cellGtfLoadFromMemory
// 
// Description:
//   Read file and load texture specified by parameter id.
//
// Return Value:
//   Returns  0 on success.
//   Returns -1 on failure.
//
// Note:
//   Parameter attrib is a pointer to pointer because this
//   function changes pointer to points to different location
//   in memory.
// 
int cellGtfLoadFromMemory( const uint8_t* gtfptr, uint32_t id, 
		                CellGtfTextureAttribute** attrib, CellGcmTexture ** tex)
{
	// Auto variables
	int i;
	uint32_t index = 0;

	CellGtfFileHeader* header;

	// Simple error check
	ITF_ASSERT( attrib != NULL );
	if( gtfptr == NULL || attrib == NULL ) return -1;

	// Read File Header
	header = (CellGtfFileHeader*) &gtfptr[index];
	index += sizeof(CellGtfFileHeader);
	
	// Validation 
	if( ! cellGtfUtilIsValidGtf( header )) {
		return -1;
	}

	// Search texture attribute that matchs id 
	for( i=0; i<(int)header->numTexture; i++ ) {
		
		// read texture attribute one by one
		*attrib = (CellGtfTextureAttribute*) &gtfptr[index];
		index += sizeof(CellGtfTextureAttribute);
        *tex = (CellGcmTexture*) &gtfptr[index];
        index += sizeof(CellGcmTexture);

		// if ID matches, then break out this loop
		if( id == (*attrib)->id ) {
			break;
		}
	}

	// if no matching id, return error
	if( i == (int)header->numTexture ) {
		return -1;
	}

	// Seek to texture data
	index = (*attrib)->offsetToTex;

	return 0;
}
