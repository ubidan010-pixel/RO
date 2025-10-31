#ifndef _ITF_MEMORYPROXYX360_H_
#define _ITF_MEMORYPROXYX360_H_

namespace ITF
{

class MemoryProxy
{

public:

static u32 getITFAlignmentPhysicalFromAttribute(u32 _platformAttribute)
{
    switch (_platformAttribute)
    {
        case XALLOC_PHYSICAL_ALIGNMENT_4:
            return MEM_ALIGNMENT_4;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_8: 
            return MEM_ALIGNMENT_8;
            break;

        case XALLOC_PHYSICAL_ALIGNMENT_16://XALLOC_ALIGNMENT_DEFAULT
            return MEM_ALIGNMENT_16;
            break;

        case XALLOC_PHYSICAL_ALIGNMENT_32: 
            return MEM_ALIGNMENT_32;
            break;

        case XALLOC_PHYSICAL_ALIGNMENT_64:  
            return MEM_ALIGNMENT_64;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_128:       
            return MEM_ALIGNMENT_128;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_256:      
            return MEM_ALIGNMENT_256;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_512:        
            return MEM_ALIGNMENT_512;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_1K:      
            return MEM_ALIGNMENT_1K;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_2K:       
            return MEM_ALIGNMENT_2K;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_DEFAULT:
        case XALLOC_PHYSICAL_ALIGNMENT_4K:                              
            return MEM_ALIGNMENT_4K;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_8K:               
            return MEM_ALIGNMENT_8K;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_16K: 
            return MEM_ALIGNMENT_16K;
            break;
        case XALLOC_PHYSICAL_ALIGNMENT_32K:
            return MEM_ALIGNMENT_32K;
            break;
        default:
            ITF_ASSERT(0);        
            break;

    }

    return MEM_ALIGNMENT_4;
}

static ITF_INLINE u32 getITFAttributeProtectFlagsFromX360(u32 _protectX360)
{
    u32 res = 0;
    if (_protectX360 & XALLOC_MEMPROTECT_WRITECOMBINE)
        return res |= ITF_PAGE_WRITECOMBINE;

    if (_protectX360 & XALLOC_MEMPROTECT_READWRITE)
        return res |= ITF_PAGE_READWRITE;

    if (_protectX360 & XALLOC_MEMPROTECT_NOCACHE)
        return res |= ITF_PAGE_NOCACHE;

    return res;
}

static u32 getAlignmentPhysicalFromITF(u32 _platformITF)
{
    switch (_platformITF)
    {
        case MEM_ALIGNMENT_4:
            return XALLOC_PHYSICAL_ALIGNMENT_4;
            break;
        case MEM_ALIGNMENT_8:
            return XALLOC_PHYSICAL_ALIGNMENT_8;
            break;
        case MEM_ALIGNMENT_16:
            return XALLOC_PHYSICAL_ALIGNMENT_16;
            break;
        case MEM_ALIGNMENT_32:
            return XALLOC_PHYSICAL_ALIGNMENT_32;
            break;
        case MEM_ALIGNMENT_64:
            return XALLOC_PHYSICAL_ALIGNMENT_64;
            break;
        case MEM_ALIGNMENT_128:
            return XALLOC_PHYSICAL_ALIGNMENT_128;
            break;
        case MEM_ALIGNMENT_256:
            return XALLOC_PHYSICAL_ALIGNMENT_256;
            break;
        case MEM_ALIGNMENT_512:
            return XALLOC_PHYSICAL_ALIGNMENT_512;
            break;
        case MEM_ALIGNMENT_1K:
            return XALLOC_PHYSICAL_ALIGNMENT_1K;
            break;
        case MEM_ALIGNMENT_2K:
            return XALLOC_PHYSICAL_ALIGNMENT_2K;
            break;
        case MEM_ALIGNMENT_4K:
            return XALLOC_PHYSICAL_ALIGNMENT_4K;
            break;
        case MEM_ALIGNMENT_8K:
            return XALLOC_PHYSICAL_ALIGNMENT_8K;
            break;
        case MEM_ALIGNMENT_16K:
            return XALLOC_PHYSICAL_ALIGNMENT_16K;
            break;       
        case MEM_ALIGNMENT_32K:
            return XALLOC_PHYSICAL_ALIGNMENT_32K;
            break;
    }

    return MEM_ALIGNMENT_16;
}

    static ITF_INLINE u32 getITFAttributeProtectFlags(u32 _protect)
    {
        u32 res = 0;
        if (_protect & XALLOC_MEMPROTECT_WRITECOMBINE)
            return res |= ITF_PAGE_WRITECOMBINE;

        if (_protect & XALLOC_MEMPROTECT_READWRITE)
            return res |= ITF_PAGE_READWRITE;

        if (_protect & XALLOC_MEMPROTECT_NOCACHE)
            return res |= ITF_PAGE_NOCACHE;

        return res;
    }


    static ITF_INLINE u32 getAttributeProtectFlagsFromITF(u32 _memory,u32 _protectITF)
    {
        u32 protect = 0;

        if (_protectITF & ITF_PAGE_WRITECOMBINE)
            protect = XALLOC_MEMPROTECT_WRITECOMBINE;

        if (_protectITF & ITF_LARGE_PAGES)
            protect = XALLOC_MEMPROTECT_WRITECOMBINE_LARGE_PAGES;

        if (_protectITF & ITF_PAGE_READWRITE)
            protect = XALLOC_MEMPROTECT_READWRITE;

        if (_protectITF & ITF_PAGE_NOCACHE)
            protect = XALLOC_MEMPROTECT_NOCACHE;

        return protect;
    }

    static void* memoryHeapAllocFunction(size_t _size,size_t _align,u32 _protectFlagsITF)
    {
        if (_size<4*1024)
        {
            return VirtualAlloc(NULL,_size, MEM_COMMIT|MEM_NOZERO,_protectFlagsITF);
        }

        return VirtualAlloc(NULL,_size, MEM_COMMIT|MEM_LARGE_PAGES|MEM_NOZERO,_protectFlagsITF);
    }
    
    
    static void* memoryPhysicalAllocFunction(size_t _size,size_t _align,u32 _protectFlagsITF)
    {
        void* pMemory = XPhysicalAlloc(_size,MAXULONG_PTR,_align,_protectFlagsITF);    

        ITF_ASSERT((u32) pMemory>=GPU_CPU_VIRTUAL_ADDRESS);
        /* seems x360 allocate more that real align requirement
#ifdef _DEBUG
        u32 _memsize = XPhysicalSize(pMemory);
        u32 newsize_aligned = mem_align_size(_size,_align);
        ITF_ASSERT(_memsize == newsize_aligned);
#endif //_DEBUG*/
        return pMemory;
    }

    static void memoryPhysicalFreeFunction(void* _ptr)
    {
        XPhysicalFree(_ptr);
    }
    
};

}

#endif //_ITF_MEMORYPROXYX360_H_