#ifndef _ITF_3DS_GRAPHICSMEMORYALLOCATOR_H_
#define _ITF_3DS_GRAPHICSMEMORYALLOCATOR_H_

#ifdef ITF_CTR
#include <nn/gx.h>
#include <nn/gx/CTR/gx_CommandAccess.h>
#include <nn/fnd/fnd_ExpHeap.h>

#ifndef _ITF_3DS_VRAMALLOCATOR_H_
#include "adapters/GFXAdapter_3DS_GR/VramAllocator.h"
#endif //_ITF_3DS_VRAMALLOCATOR_H_


namespace ITF
{

class DeviceMemoryAllocator
{
public:
	void Initialise(u32 offset, u32 size);
	void Finalize();

	void* Alloc(size_t size, s32 alignment = nn::fnd::HeapBase::DEFAULT_ALIGNMENT);
	void Free(void* ptr);
	
    void FlushContent();

	nn::fnd::ThreadSafeExpHeap m_deviceHeap;
    void* m_startAddr;
    size_t m_size;
};


class GraphicsMemoryAllocator
{
public:
	static void Initialise();
	static void Finalize();
	
	static void SetDeviceMemoryAllocator(DeviceMemoryAllocator* _pDeviceAlloc);

public:
	static void*   Allocate(GLenum area, GLenum aim, GLuint id, GLsizei size);
	static void    Deallocate(GLenum area, GLenum aim, GLuint id, void* addr);
	static GLenum  GetAvailableVRAMBankRegion(GLenum aim, GLsizei size);
	
	static void*   DevMemAlloc(size_t size, int alignement = nn::fnd::HeapBase::DEFAULT_ALIGNMENT);
	static void*   VramAAlloc(size_t size, int alignement = nn::fnd::HeapBase::DEFAULT_ALIGNMENT);
	static void*   VramBAlloc(size_t size, int alignement = nn::fnd::HeapBase::DEFAULT_ALIGNMENT);

    static void   DevMemFree(void* addr);
    static void   VramAFree(void* addr);
    static void   VramBFree(void* addr);

    static void FlushDevMemContent();

public:
	static const int ALIGNMENT_SYSTEM_BUFFER     = 4;
	static const int ALIGNMENT_VERTEX            = 8;
	static const int ALIGNMENT_TEXTURE           = 128;
	static const int ALIGNMENT_RENDER_BUFFER     = 64;    
	static const int ALIGNMENT_DISPLAY_BUFFER    = 16;
	static const int ALIGNMENT_3D_COMMAND_BUFFER = 16;
private:
	static DeviceMemoryAllocator *m_pDeviceAlloc;
	static VramAllocator m_vramA;
	static VramAllocator m_vramB;
};



inline void DeviceMemoryAllocator::Initialise(u32 offset, u32 size)
{
	uptr devmemaddr = nn::os::GetDeviceMemoryAddress();
	
    //NN_LOG("GX memory : %p-%p\n", devmemaddr + offset, devmemaddr + offset + size);
	m_deviceHeap.Initialize(devmemaddr + offset, size);
    
    m_startAddr = (void*)devmemaddr;
    m_size = size;
}

inline void DeviceMemoryAllocator::Finalize()
{
	m_deviceHeap.Finalize();
}

inline void* DeviceMemoryAllocator::Alloc(size_t size, s32 alignment)
{
	void* result = m_deviceHeap.Allocate(size, alignment);
	GFX_ASSERT(result != NULL, "no more memory on device allocator\n");
	return result;
}

inline void DeviceMemoryAllocator::Free(void* ptr)
{
	GFX_ASSERT(ptr != NULL, "Freeing a null pointer\n");
	m_deviceHeap.Free(ptr);
}

inline void DeviceMemoryAllocator::FlushContent()
{
    nngxUpdateBuffer(m_startAddr, m_size);
}

inline void GraphicsMemoryAllocator::SetDeviceMemoryAllocator(DeviceMemoryAllocator* _pDeviceAlloc)
{
	m_pDeviceAlloc = _pDeviceAlloc;
}

inline void* GraphicsMemoryAllocator::DevMemAlloc(size_t size, int alignement)
{
    return m_pDeviceAlloc->Alloc(size, alignement);
}

inline void* GraphicsMemoryAllocator::VramAAlloc(size_t size, int alignement)
{
    return m_vramA.Alloc(size);
}

inline void* GraphicsMemoryAllocator::VramBAlloc(size_t size, int alignement)
{
    return m_vramB.Alloc(size);
}

inline void GraphicsMemoryAllocator::DevMemFree(void* addr)
{
    m_pDeviceAlloc->Free(addr);
}

inline void GraphicsMemoryAllocator::VramAFree(void* addr)
{
    m_vramA.Free(addr);
}

inline void GraphicsMemoryAllocator::VramBFree(void* addr)
{
    m_vramB.Free(addr);
}

inline void GraphicsMemoryAllocator::FlushDevMemContent()
{
    m_pDeviceAlloc->FlushContent();
}

typedef GraphicsMemoryAllocator GFXMEM;

} // namespace ITF

#endif // ITF_CTR
#endif // _ITF_3DS_GRAPHICSMEMORYALLOCATOR_H_