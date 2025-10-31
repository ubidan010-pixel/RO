/*   SCE CONFIDENTIAL                                       */
/*   PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004 */
/*   Copyright (C) 2006 Sony Computer Entertainment Inc.    */
/*   All Rights Reserved.                                   */

#ifndef __GTF_TEXTURE_H__
#define __GTF_TEXTURE_H__

//
// Texture File Header
//
typedef struct
{
	// Version information.
	//   Correspond to dds2gtf converter version.
	uint32_t  version;      

	// Total size of Texture.
	//   Excluding size of header & attribute.
	uint32_t  size;         

	// Number of textures in this file.
	//   
	uint32_t  numTexture;   

} CellGtfFileHeader;


// 
// Attribute for Texture data 
//
// NOTE:
//  It is always true that the number of textures in GTF file 
//  equals the number of CellGtfTextureAttribute in header.
//
typedef struct 
{        
	// Texture ID.
	//   
	uint32_t id;            

	// Offset to texture from begining of file.
	//   
	uint32_t offsetToTex;   

	// Size of texture.
	// 
	uint32_t textureSize;

} CellGtfTextureAttribute;

#endif //  __GTF_TEXTURE_H__

