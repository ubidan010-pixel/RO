#ifndef ITF_CORE_MEMORYBIGALLOC_PS3_H_
#define ITF_CORE_MEMORYBIGALLOC_PS3_H_

namespace ITF
{
    // Big alloc is designed to be used for big allocations (at least > 64K)
    // The usable memory size (MSize) of a block is aligned on the page size.
    // The address returned by allocations is also aligned on the page size.
    // As the page size is at least 64K, alignment parameter is not needed.
    namespace MemMngBigAlloc
    {
        bool			Init();
        void            GetRange(void * & _baseAddress, void * & _limitAddress);
        bool            IsInRange(void * _ptr); // ptr allocated in big alloc
        ux              GetPageSize();
        void*			Alloc(u32 _size); // Alignment is guaranteed to be aligned on page size (ie. at least 64K)
        void			Free(void * _ptr);
        ux	            MSize(void * _ptr);
    }
}  // namespace ITF

#endif // #ifndef ITF_CORE_MEMORYBIGALLOC_PS3_H_
