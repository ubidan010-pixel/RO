#ifndef MEMMNGBUCKETS_H__
#define MEMMNGBUCKETS_H__

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
	u32			    MSize(void * pData);

	void			FullCheckValidity();
	void			DumpStats();
}

}

#endif // MEMMNGBUCKETS_H__
