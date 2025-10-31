#ifndef ITF_MEMORYPROXY_H_
#define ITF_MEMORYPROXY_H_

#pragma once

#if defined(ITF_WINDOWS) || defined(ITF_XBOX_SERIES)
#include "core/memory/memoryProxyWIN.h"
#elif defined(ITF_X360)
#include "core/memory/memoryProxyX360.h"
#elif defined(ITF_IPAD)
#include "core/memory/memoryProxyiPad.h"
#elif defined(ITF_CTR)
#include "core/memory/memoryProxyCTR.h"
#elif defined(ITF_WII)
#include "core/memory/memoryProxyWII.h"
#elif defined(ITF_PS3)
#include "core/memory/memoryProxyPS3.h"
#elif defined(ITF_VITA)
#include "core/memory/memoryProxyVITA.h"
#else
#include "core/memory/memoryProxyGeneric.h"
#endif


#endif //_ITF_MEMORYPROXY_H_
