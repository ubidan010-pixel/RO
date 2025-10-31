#ifndef VRAMRELOCHDL_H_
#define VRAMRELOCHDL_H_

#include "VRAMAllocator/VRAMMemoryTypes.h"
#include "VRAMAllocator/VRAMHandleLinks.h"
#include "VRAMAllocator/PoolContainer.h"

namespace ITF
{
namespace GCM
{
    namespace VRAMAllocator
    {
        class ChunkContainer;
    }

    class VRAMRelocHandle : public VRAMHandleLinks
    {
    public:
        VRAMRelocHandle() { next = 0; prev = 0; }

        virtual ~VRAMRelocHandle();

        void operator = (const VRAMRelocHandle & hdl);
        void operator += (u32 offset); // basic pointer arithmetic

        // generic -> try first the local memory, then the main
        void Alloc(VRAMChunkSize size, VramMemoryType type = VRAM_Unknown);
        void AllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type = VRAM_Unknown);

        // specify the memory area
        void LocalAlloc(VRAMChunkSize size, VramMemoryType type = VRAM_Unknown);
        void LocalAllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type = VRAM_Unknown);
        void MainAlloc(VRAMChunkSize size, VramMemoryType type = VRAM_Unknown);
        void MainAllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type = VRAM_Unknown);

        void MainReportAlloc(VRAMChunkSize size, VramMemoryType type = VRAM_Unknown);
        void MainReportAllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type = VRAM_Unknown);

        VRAMChunkSize MemSize();

        // do not call unless you know what you are doing
        virtual void RelocateAddress(VRAMAddress vramEA, VRAMOffset vramOffset, VRAMLocation location) = 0;
        virtual void ResolveAddress(VRAMAddress & vramEA, VRAMOffset & vramOffset, VRAMLocation & location) const = 0;

    protected:
        VRAMRelocHandle(const VRAMRelocHandle & hdl);

    private:
        void UnlinkHandle();

        template <void (VRAMAllocator::PoolContainer::* AllocMethod)(VRAMChunkSize, VramMemoryType, VRAMAllocator::ChunkContainer * &, VRAMAllocator::ChunkIdx &)>
        void TplAlloc(VRAMChunkSize size, VramMemoryType type);

        template <void (VRAMAllocator::PoolContainer::* AllocAlignedMethod)(VRAMChunkSize, VRAMChunkSize, VramMemoryType, VRAMAllocator::ChunkContainer * &, VRAMAllocator::ChunkIdx &)>
        void TplAllocAligned(VRAMChunkSize size, VRAMChunkSize alignment, VramMemoryType type);
    };

    // simpler version. Keep EA, RSX Offset, and location
    class SimpleVRAMRelocHandle : public VRAMRelocHandle
    {
    public:
        SimpleVRAMRelocHandle();
        SimpleVRAMRelocHandle(const VRAMRelocHandle & hdl);
        virtual ~SimpleVRAMRelocHandle() {}

        void operator = (const VRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
        void operator = (const SimpleVRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
        void operator += (u32 offset) { VRAMRelocHandle::operator += (offset); }

        VRAMAddress GetAddress() const { return m_vramEA; }
        VRAMOffset GetOffset() const { return m_vramOffset; }
        VRAMLocation GetLocation() const { return m_location; }

    protected:
        virtual void RelocateAddress(VRAMAddress vramEA, VRAMOffset vramOffset, VRAMLocation location);
        virtual void ResolveAddress(VRAMAddress & vramEA, VRAMOffset & vramOffset, VRAMLocation & location) const;

    private:
        VRAMAddress m_vramEA;
        VRAMOffset m_vramOffset;
        VRAMLocation m_location;
    };

    // Version that contain pointer on locations where to change RSX offset or location
    // You can this way bind it with a CellGCMTexture thus keeping it up to date
    class OffsetPtrRelocHandle : public VRAMRelocHandle
    {
    public:
        OffsetPtrRelocHandle();
        OffsetPtrRelocHandle(VRAMOffset * pOffset, VRAMLocation * pLocation);

        void operator = (const VRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
        void operator = (const OffsetPtrRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
        void operator += (u32 offset) { VRAMRelocHandle::operator += (offset); }

        void SetRelocPtr(VRAMOffset * pOffset, VRAMLocation * pLocation);
 
        VRAMAddress GetAddress() const { return m_vramEA; }
        VRAMOffset GetOffset() const { return m_pVramOffset?*m_pVramOffset:0; }
        VRAMLocation GetLocation() const { return m_pLocation?*m_pLocation:0; }

    protected:
        virtual void RelocateAddress(VRAMAddress vramEA, VRAMOffset vramOffset, VRAMLocation location);
        virtual void ResolveAddress(VRAMAddress & vramEA, VRAMOffset & vramOffset, VRAMLocation & location) const;

    private:
        VRAMAddress m_vramEA;
        VRAMOffset * m_pVramOffset;
        VRAMLocation * m_pLocation;
    };

    extern const SimpleVRAMRelocHandle InvalidRelocHandle;
} //GCM
} // ITF

#endif // VRAMRELOCHDL_H_
