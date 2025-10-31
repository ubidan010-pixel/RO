#ifndef __MEM_LOG_WII_H__
#define __MEM_LOG_WII_H__

#include "core/memory/WII/MEM_consts_WII.h"

namespace ITF
{

typedef void (*MEM_tfn_LogCallback) (const char *);

#ifdef _HEAP_DEBUG_

#define MEM_LogStringSize 512
#define	MEM_LogPrint(ptLogCB,...)\
	do{\
	char buffTmp[MEM_LogStringSize];\
	sprintf(buffTmp,__VA_ARGS__);\
	(*ptLogCB)(buffTmp);\
	}while(0)

void MEM_LogUsage(MEM_tfn_LogCallback _pLogCb,u32 _uFlag);

inline void MEM_LogCallback(const char *_sMsg)
{
	OSReport(_sMsg);
}

inline void MEM_LogUsageDebug()
{
	MEM_LogUsage(MEM_LogCallback,MEM_C_LOG_USAGE_ALL);
}

#else // _HEAP_DEBUG_

#define MEM_LogUsage(_pLogCb,_uFlag)
#define MEM_LogUsageDebug()

#endif // _HEAP_DEBUG_

} // namespace ITF

#endif // __MEM_LOG_WII_H__
