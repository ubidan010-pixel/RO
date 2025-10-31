#ifndef ITF_RENDERTARGET_PS5_H_
#define ITF_RENDERTARGET_PS5_H_

#pragma once
#include "Gnmp_Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

namespace ITF
{
    class PS5Texture;

    namespace Gnmp
    {
        class RenderTargetRelocHandle : public Gnmp::VRAMRelocHandle
        {
        public:
            RenderTargetRelocHandle(Gnmp::RenderTarget & _rt) : m_rt(_rt) { relocateAddress(nullptr); }

            void operator = (const VRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
            void operator = (const RenderTargetRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
            void operator += (VRAMChunkSize offset) { Gnmp::VRAMRelocHandle::operator += (offset); }

            VRAMAddress getAddress() const { return (VRAMAddress)m_rt.getBaseAddress(); }

        protected:
            virtual void relocateAddress(VRAMAddress vramAddr);
            virtual VRAMAddress resolveAddress() const;

        private:
            Gnmp::RenderTarget & m_rt;
        };

        class RTCMaskRelocHandle : public Gnmp::VRAMRelocHandle
        {
        public:
            RTCMaskRelocHandle(Gnmp::RenderTarget & _rt) : m_rt(_rt) { relocateAddress(nullptr); }

            void operator = (const VRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
            void operator = (const RTCMaskRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
            void operator += (VRAMChunkSize offset) { Gnmp::VRAMRelocHandle::operator += (offset); }

            VRAMAddress getAddress() const { return (VRAMAddress)m_rt.getCmaskAddress(); }

        protected:
            virtual void relocateAddress(VRAMAddress vramAddr);
            virtual VRAMAddress resolveAddress() const;

        private:
            Gnmp::RenderTarget & m_rt;
        };

        class RTFMaskRelocHandle : public Gnmp::VRAMRelocHandle
        {
        public:
            RTFMaskRelocHandle(Gnmp::RenderTarget & _rt) : m_rt(_rt) { relocateAddress(nullptr); }

            void operator = (const VRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
            void operator = (const RTFMaskRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
            void operator += (VRAMChunkSize offset) { Gnmp::VRAMRelocHandle::operator += (offset); }

            VRAMAddress getAddress() const { return (VRAMAddress)m_rt.getFmaskAddress(); }

        protected:
            virtual void relocateAddress(VRAMAddress vramAddr);
            virtual VRAMAddress resolveAddress() const;

        private:
            Gnmp::RenderTarget & m_rt;
        };
    }

    class PS5RenderTarget : public Gnmp::RenderTarget
    {
    public:

        PS5RenderTarget()
            : Gnmp::RenderTarget{}
            , baseMemHdl(*this)
            , cmaskMemHdl(*this)
            , fmaskMemHdl(*this)
            , m_resHandler(&this->m_agcRenderTarget)
        {}

        ~PS5RenderTarget();

        Gnmp::RenderTargetRelocHandle baseMemHdl;
        Gnmp::RTCMaskRelocHandle cmaskMemHdl;
        Gnmp::RTFMaskRelocHandle fmaskMemHdl;

        void buildTexture(); // create asTexture object
        PS5Texture* asTexture = nullptr;

        Gnmp::AutoResourceHandler<sce::Agc::CxRenderTarget> m_resHandler;

    private:
        // The addresses are changed only by the memHdls. Methods let undefined in order to get link errors.
        void setAddresses(void *baseAddress, void *cmaskAddress, void *fmaskAddress);
        void setBaseAddress(void *baseAddr);
        void setBaseAddress256ByteBlocks(u32 baseAddr256);
        void setCmaskAddress(void *cmaskAddr);
        void setCmaskAddress256ByteBlocks(u32 cmaskAddr256);
        void setFmaskAddress(void *fmaskAddr);
        void setFmaskAddress256ByteBlocks(u32 fmaskAddr256);
        i32 initFromTexture(const Gnmp::Texture *texture, u32 mipLevel);
    };

}


#endif // ITF_RENDERTARGET_PS5_H_
