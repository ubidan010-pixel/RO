#include "precompiled_GFXAdapter_3DS_GR.h"

#ifndef _ITF_3DS_GRAPHICSMEMORYALLOCATOR_H_
#include "adapters/GFXAdapter_3DS_GR/GraphicsMemoryAllocator.h"
#endif //_ITF_3DS_GRAPHICSMEMORYALLOCATOR_H_

namespace ITF
{
DeviceMemoryAllocator *GraphicsMemoryAllocator::m_pDeviceAlloc=NULL;
VramAllocator GraphicsMemoryAllocator::m_vramA;
VramAllocator GraphicsMemoryAllocator::m_vramB;

void GraphicsMemoryAllocator::Initialise()
{
	//m_deviceMemory.Initialise();
	NN_ASSERT(m_pDeviceAlloc);
	m_vramA.Initialize(nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA), nn::gx::GetVramSize(nn::gx::MEM_VRAMA));
	m_vramB.Initialize(nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB), nn::gx::GetVramSize(nn::gx::MEM_VRAMB));
}

void GraphicsMemoryAllocator::Finalize()
{
	m_vramB.Finalize();
	m_vramA.Finalize();
	//m_deviceMemory.Finalize();
	m_pDeviceAlloc=NULL;
}

void* GraphicsMemoryAllocator::Allocate(GLenum area, GLenum aim, GLuint id, GLsizei size)
{
	if (size == 0)
		return 0;

	int addrAlign = 8;
	VramAllocator::E_AllocDirection allocDirection = VramAllocator::AllocDirection_FromEnd;
	void* resultAddr = NULL;

	switch (aim)
	{
	case NN_GX_MEM_SYSTEM:
		addrAlign = ALIGNMENT_SYSTEM_BUFFER;
		break;
	case NN_GX_MEM_TEXTURE:
		addrAlign = ALIGNMENT_TEXTURE;
		break;
	case NN_GX_MEM_VERTEXBUFFER:
		addrAlign = ALIGNMENT_VERTEX;
		break;
	case NN_GX_MEM_RENDERBUFFER:
		addrAlign = ALIGNMENT_RENDER_BUFFER;
		break;
	case NN_GX_MEM_DISPLAYBUFFER:
		addrAlign = ALIGNMENT_DISPLAY_BUFFER;
		allocDirection = VramAllocator::AllocDirection_FromBeginning;
		break;
	case NN_GX_MEM_COMMANDBUFFER:
		addrAlign = ALIGNMENT_3D_COMMAND_BUFFER;
		break;
	default:
		GFX_ASSERT(false, "Wrong aim\n");
		break;
	}

	switch (area)
	{
	case NN_GX_MEM_FCRAM:
		resultAddr = m_pDeviceAlloc->Alloc(size, addrAlign);
		break;

	case NN_GX_MEM_VRAMA:
		resultAddr = m_vramA.Alloc(size, allocDirection);
		break;

	case NN_GX_MEM_VRAMB:
		resultAddr = m_vramB.Alloc(size, allocDirection);
		break;

	default:
		GFX_ASSERT(false, "Wrong area\n");
		break;
	}
	GFX_ASSERT(resultAddr != NULL, "Allocation failed\n");
	return resultAddr;
}

void GraphicsMemoryAllocator::Deallocate(GLenum area, GLenum aim, GLuint id, void* addr)
{
	(void)aim;  
	(void)id;  
	GFX_ASSERT(addr != NULL, "Deallocating Null address\n");
	switch (area)
	{
	case NN_GX_MEM_FCRAM:
		m_pDeviceAlloc->Free(addr);
		break;

	case NN_GX_MEM_VRAMA:
		m_vramA.Free(addr);
		break;

	case NN_GX_MEM_VRAMB:
		m_vramB.Free(addr);
		break;
	default:
		ITF_ASSERT(false);
		break;
	}
}

GLenum GraphicsMemoryAllocator::GetAvailableVRAMBankRegion(GLenum aim, GLsizei size)
{
	int alignment = 0;

	switch (aim)
	{
	case NN_GX_MEM_SYSTEM:
		alignment = ALIGNMENT_SYSTEM_BUFFER;
		break;
	case NN_GX_MEM_TEXTURE:
		alignment = ALIGNMENT_TEXTURE;
		break;
	case NN_GX_MEM_VERTEXBUFFER:
		alignment = ALIGNMENT_VERTEX;
		break;
	case NN_GX_MEM_RENDERBUFFER:
		alignment = ALIGNMENT_RENDER_BUFFER;
		break;
	case NN_GX_MEM_DISPLAYBUFFER:
		alignment = ALIGNMENT_DISPLAY_BUFFER;
		break;
	case NN_GX_MEM_COMMANDBUFFER:
		alignment = ALIGNMENT_3D_COMMAND_BUFFER;
		break;
	default:
		ITF_ASSERT(false);
		break;
	}

	size += alignment;

	if (m_vramA.CanHandleAllocation(size))
	{
		return NN_GX_MEM_VRAMA;
	}
	else if (m_vramB.CanHandleAllocation(size))
	{
		return NN_GX_MEM_VRAMB;
	}

	return NN_GX_MEM_FCRAM;
}

} // namespace ITF