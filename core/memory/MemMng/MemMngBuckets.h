#ifndef ITF_CORE_MEMMNGBUCKETS_H_
#define ITF_CORE_MEMMNGBUCKETS_H_

#pragma once

namespace ITF
{

namespace MemMngBuckets
{
	bool			Init();
	void			Close();

    bool            BucketSystemAvailable();
	bool			IsBucketAddress(void * pData);
	bool			IsBucketRangeAddress(void * pData);

	void*			Alloc(u32 iSize, u32 uAlign);
	void			Free(void *pData);
	uPtr			MSize(void * pData);

	void			FullCheckValidity();
	void			DumpStats();
}

}

#endif // ITF_CORE_MEMMNGBUCKETS_H_
