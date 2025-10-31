#ifndef VRAMRELOCHDL_H_
#define VRAMRELOCHDL_H_

#pragma once

#include <agc/gnmp/gpustructs.h>
#include "VRAMMemoryTypes.h"
#include "VRAMHandleLinks.h"
#include "PoolContainer.h"

namespace ITF::Gnmp
{
    using sce::Agc::Gnmp::SizeAlign;

    namespace VRAMAllocator
    {
        class ChunkContainer;
        class PoolContainer;
    }

    class VRAMRelocHandle : public VRAMHandleLinks
    {
    public:
        VRAMRelocHandle() { next = 0; prev = 0; }

        virtual ~VRAMRelocHandle();

        void operator = (const VRAMRelocHandle & hdl);
        void operator += (VRAMChunkSize offset); // basic pointer arithmetic

        template <VRAMResourceType = READ_ONLY, VRAMMemoryBus = VRAMMemoryBus::DIRECT>
        void alloc(VRAMChunkSize _size, VramMemoryCategory _category = VRAM_Unknown);
        template <VRAMResourceType = READ_ONLY, VRAMMemoryBus = VRAMMemoryBus::DIRECT>
        void allocAligned(VRAMChunkSize _size, VRAMChunkSize _alignment, VramMemoryCategory _category = VRAM_Unknown);

        // SizeAlign version
        template <VRAMResourceType resType = READ_ONLY, VRAMMemoryBus memBus = VRAMMemoryBus::DIRECT>
        void alloc(SizeAlign _sizeAlign, VramMemoryCategory _category = VRAM_Unknown)
        {
            allocAligned<resType, memBus>(_sizeAlign.m_size, _sizeAlign.m_align, _category);
        }

        // specify the container (the container being itself a garlic or onion one, with a resource type assigned)
        void allocInContainer(VRAMChunkSize size, VRAMAllocator::ChunkContainer * _container, VramMemoryCategory type = VRAM_Unknown);
        void allocAlignedInContainer(VRAMChunkSize size, VRAMChunkSize alignment, VRAMAllocator::ChunkContainer * _container, VramMemoryCategory type = VRAM_Unknown);

        VRAMChunkSize memSize();

    protected:
        virtual void relocateAddress(VRAMAddress vramAddr) = 0;
        virtual VRAMAddress resolveAddress() const = 0;

        VRAMRelocHandle(const VRAMRelocHandle & hdl);

    private:
        void unlinkHandle();
    };

    // simpler version. Keep internally the address for immediate access
    class SimpleVRAMRelocHandle : public VRAMRelocHandle
    {
    public:
        SimpleVRAMRelocHandle();
        SimpleVRAMRelocHandle(const VRAMRelocHandle & hdl);
        virtual ~SimpleVRAMRelocHandle() {}

        void operator = (const VRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
        void operator = (const SimpleVRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
        void operator += (VRAMChunkSize offset) { VRAMRelocHandle::operator += (offset); }

        VRAMAddress getAddress() const { return m_vramAddr; }

    protected:
        virtual void relocateAddress(VRAMAddress vramAddr);
        virtual VRAMAddress resolveAddress() const;

    private:
        VRAMAddress m_vramAddr;
    };

    // Version that contain pointer on locations where to change the address
    // You can this way bind it with another "proprietary" object thus keeping it up to date when relocation occur
    class OffsetPtrRelocHandle : public VRAMRelocHandle
    {
    public:
        OffsetPtrRelocHandle();
        OffsetPtrRelocHandle(VRAMAddress * pAddrToPatch);

        void operator = (const VRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
        void operator = (const OffsetPtrRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
        void operator += (VRAMChunkSize offset) { VRAMRelocHandle::operator += (offset); }

        void setRelocPtr(VRAMAddress * pAddrToPatch);
 
        VRAMAddress getAddress() const { return m_vramAddr; }

    protected:
        virtual void relocateAddress(VRAMAddress vramAddr);
        virtual VRAMAddress resolveAddress() const;

    private:
        VRAMAddress m_vramAddr;
        VRAMAddress * m_pAddrToPatch;
    };

    extern const SimpleVRAMRelocHandle InvalidRelocHandle;

} // namzspace ITF::Gnmp

#endif // VRAMRELOCHDL_H_
