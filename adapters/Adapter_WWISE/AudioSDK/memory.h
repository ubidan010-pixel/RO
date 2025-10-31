#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "core/memory/memory.h"

#define WWISE_NEW(_a)           newAlloc(mId_AudioEngine, _a)
#define WWISE_DELETE(_a)        SF_DEL(_a)
#define WWISE_DELETE_ARRAY(_a)  SF_DEL_ARRAY(_a)
#endif // __MEMORY_H__
