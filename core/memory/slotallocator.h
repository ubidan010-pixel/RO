#ifndef _ITF_SLOTALLOCATOR_H_
#define _ITF_SLOTALLOCATOR_H_


namespace ITF
{

#define MEM_ALIGNMENT_4     4
#define MEM_ALIGNMENT_8     8
#define MEM_ALIGNMENT_16    16
#define MEM_ALIGNMENT_32    32
#define MEM_ALIGNMENT_64    64
#define MEM_ALIGNMENT_128   128
#define MEM_ALIGNMENT_256   256
#define MEM_ALIGNMENT_512   512
#define MEM_ALIGNMENT_1K    1024
#define MEM_ALIGNMENT_2K    2*1024
#define MEM_ALIGNMENT_4K    4*1024
#define MEM_ALIGNMENT_8K    8*1024
#define MEM_ALIGNMENT_16K   16*1024
#define MEM_ALIGNMENT_32K   32*1024

#define ITF_PAGE_READWRITE      0x04     
#define ITF_PAGE_NOCACHE        0x200     
#define ITF_PAGE_WRITECOMBINE   0x400
#define ITF_LARGE_PAGES         0x20000000  

//#define USE_WARNINGS_STATS 

extern  ITF_INLINE u32    mem_align_size(u32 _size,u32 _alignment);
extern  ITF_INLINE bbool  mem_IsPtrAligned(void *_ptr,u32 _alignment);

//------------------------------------------------------------------------------------------------------------------------

class Blob;

class memSettings
{
public:
    memSettings(u32 _memoryType,u32 _slotSize,u32 _slotCount,u32 _alignment,u32 _protectFlags,u32* _poolID = NULL)
    {
        m_MemoryType    = _memoryType;
        m_slotSize      = _slotSize;
        m_slotCount     = _slotCount;
        m_alignment     = _alignment;
        m_protectFlags  = _protectFlags;
        m_poolID        = _poolID;
    }

    u32     m_MemoryType;
    u32     m_slotSize;
    u32     m_slotCount;
    u32     m_alignment;
    u32     m_protectFlags;
    u32*    m_poolID;//this pointer refer to an pool id used in the global scope
};

//------------------------------------------------------------------------------------------------------------------------


class slotAllocator
{
public:

    uPtr create(u32 _memoryType,u32 _sizeSlotByte,u32 _slotCount,u32 _alignmentITF,u32 _protectITF);
    void* allocate();
    bbool  deallocate(void* _pMemory);

    bbool ITF_INLINE matchProtectFlags(u32 _protect)            const              {return (_protect == m_protectFlagsPlatform);}
    bbool ITF_INLINE matchProtectFlagsITF(u32 _protectITF)      const              {return (_protectITF == m_protectFlagsITF);}
       
    ITF_INLINE bbool matchAlignmentAttribute(u32 _alignmentAttributePlatform) const {return (_alignmentAttributePlatform == m_AlignmentAtributePlatform);}
    ITF_INLINE bbool matchAlignmentITF(u32 _alignmentITF)                     const {return (_alignmentITF == m_AlignmentITF);}       
    
    ITF_INLINE u32 getSizeSlot() {return m_sizeSlotByte;}
    ITF_INLINE bbool isInRange(uPtr pAddress) { return ((pAddress>= uPtr(m_pMemoryStartRange)) && (pAddress<m_MemoryEndRange));}

    static void createCriticalSection();
    static void destroyCriticalSection();

#ifndef ITF_FINAL
    void dumpStats(Blob& _blob) const;
#endif //ITF_FINAL


protected:

    uPtr    m_globalMemorySize;
    u32     m_sizeSlotByte;
    u32     m_slotCount;
    u32     m_protectFlagsITF;
    u32     m_protectFlagsPlatform;
    u8*     m_pMemoryStartRange;
    uPtr    m_MemoryEndRange;
    u32     m_AlignmentITF;
    u32     m_AlignmentAtributePlatform;

    SafeArray<u32>  m_freeIndexes;

    //TODO use a more faster way
    static ITF_THREAD_CRITICAL_SECTION     m_csSlot;

};


} // namespace ITF
			

#endif // _ITF_SLOTALLOCATOR_H_
