#ifndef ITF_CORE_MEMMNGPAGE_H_
#define ITF_CORE_MEMMNGPAGE_H_

#pragma once

#if defined(ITF_PS5)
#include "MemMngPage_PS5.h"
#elif defined(ITF_NINTENDO)
#include "MemMngPage_Nintendo.h"
#else
#error Platform not supported
#endif

#endif // ITF_CORE_MEMMNGPAGE_H_
