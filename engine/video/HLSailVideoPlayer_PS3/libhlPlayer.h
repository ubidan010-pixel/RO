/* SCE CONFIDENTIAL
 * PlayStation(R)3 Programmer Tool Runtime Library 360.001
 * Copyright (C) 2010 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef	__hlPlayer_H__
#define	__hlPlayer_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/prx.h>
#include <cell/sysmodule.h>
#include <cell/cell_fs.h>
#include <sys/ppu_thread.h>
#include <sys/event.h>
#include <assert.h>
#include <sys/memory.h>

#include <cell/spurs.h>
#include <sys/spu_initialize.h>

struct VideoFrameInfo
{
	uint8_t* buffer;
	uint32_t width;
	uint32_t height;
    uint32_t syncTag;
    bool RGBAFrameHint;  
};

struct VideoPlayerInit
{
    CellSpurs* pSharedSpurs;
    bool RGBAOutput;
	uint32_t width;
	uint32_t height;
};

#ifdef __cplusplus
// The C++ API is only available to C++
#include "SailAllocator.h"

namespace cell {
namespace Sail {
    class CustomSource;
}}

// Shared Structure between the C & C++ interfaces.
typedef struct { //TODO: Make this C only so it can be shared between the two.
	cell::Sail::CustomSource* FileReplacement;
	cell::Sail::memallocator* MemoryReplacement;
} CustomAllocators;
#include "libhlPlayerApi.hpp"

#endif

// The C API is available to both C & C++
#include "libhlPlayerApi.h"


#endif
