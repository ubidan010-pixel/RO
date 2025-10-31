#ifndef ITF_PRECOMPILED_GFXADAPTER_PS5_H_
#define ITF_PRECOMPILED_GFXADAPTER_PS5_H_

#include "core/enginecommon.h"
#include "core/memory/memory.h"

#include "core/error/ErrorHandler.h"


// In order for any of the asserts in Agc headers to work (ex. registerstructs.h) SCE_AGC_DEBUG must be defined before including those headers.
// Since the extra validation adds an extra cost, we only define SCE_AGC_DEBUG for Debug builds.
#if defined(ITF_DEBUG) && !defined(ROBOT_COMPILATION) && !defined(SCE_AGC_DEBUG)
#define SCE_AGC_DEBUG
#endif

#include <kernel.h>
#include <agc.h>
#include <agc/core.h>
#include <agc/gnmp/drawcommandbuffer.h>
#include <agc/gnmp/rendertarget.h>
#include <agc/gnmp/buffer.h>
#include <agc/gnmp/shader.h>
#include <agc/gnmp/depthrendertarget.h>

#endif //ITF_PRECOMPILED_GFXADAPTER_PS5_H_
