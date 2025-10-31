#include "precompiled_GFXAdapter_PS5.h"

#include "adapters/GFXAdapter_PS5/Texture_PS5.h"
#include "adapters/GFXAdapter_PS5/RenderTarget_PS5.h"

namespace ITF
{
    namespace Gnmp
    {
        void RenderTargetRelocHandle::relocateAddress(VRAMAddress _vramAddr)
        {
            m_rt.setBaseAddress(_vramAddr);
        }

        VRAMAddress RenderTargetRelocHandle::resolveAddress() const
        {
            return getAddress();
        }

        void RTCMaskRelocHandle::relocateAddress(VRAMAddress _vramAddr)
        {
            m_rt.setCmaskAddress(_vramAddr);
        }

        VRAMAddress RTCMaskRelocHandle::resolveAddress() const
        {
            return getAddress();
        }

        void RTFMaskRelocHandle::relocateAddress(VRAMAddress _vramAddr)
        {
            m_rt.setFmaskAddress(_vramAddr);
        }

        VRAMAddress RTFMaskRelocHandle::resolveAddress() const
        {
            return getAddress();
        }
    }

    void PS5RenderTarget::buildTexture()
    {
        SF_DEL(asTexture);
        asTexture = new (MemoryId::mId_GfxAdapter) PS5Texture;
        asTexture->initFromRenderTarget(*this, bfalse);
    }

    PS5RenderTarget::~PS5RenderTarget()
    {
        SF_DEL(asTexture);
    }
}

