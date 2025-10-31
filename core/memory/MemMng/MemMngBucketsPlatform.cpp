#include "precompiled_core.h"
#include "MemMngPrivate.h"
#include "MemMngBucketsPlatform.h"

namespace ITF
{

namespace MemMngBucketsPlatform
{

#if MEMMNG_BUCKET_BUILT_ON_MALLOC==0

    // address mapping info
    void * baseAddress = 0; // init by Init
    void * limitAddress = 0; // init by Init 

    MemoryPageManager * g_bucketPageMgr;

    void Init()
    {
        static MemoryPageManager memoryPageMgr;
        g_bucketPageMgr = &memoryPageMgr;
        g_bucketPageMgr->Init(MemoryPageManager::InitParams()
                            .setPageAlignment(BUCKET_PAGE_SIZE_GRANULARITY)
                            .setAreaSize(BUCKET_MEMORY_ADRESSABLE));
        baseAddress = g_bucketPageMgr->GetAreaBegin();
        limitAddress = g_bucketPageMgr->GetAreaEnd();
    }

    void Close()
    {
        g_bucketPageMgr->Close();
    }

#else // MEMMNG_BUCKET_BUILT_ON_MALLOC!=0

    // Address mapping info (set by Init)
    void * baseAddress = 0;
    void * limitAddress = 0;

    void Init()
    {
        MemMngStd::GetRange(baseAddress, limitAddress);
    }

    void Close()
    {

    }

#endif

} // namespace MemMngBucketsPlatform

} // namespace ITF
