#pragma once

#include "DX12/Helpers.h"

#include "core/utility.h"
#include "core/container/ExtendedVector.h"

namespace ITF::DX12
{
    class RenderTargetViewDescriptorPool
    {
    public:
        explicit RenderTargetViewDescriptorPool(ux _nbMaxRTV = 64);
        RenderTargetViewDescriptorPool(const RenderTargetViewDescriptorPool&) = delete;

        RenderTargetViewDescriptorPool & operator = (const RenderTargetViewDescriptorPool&) = delete;

    private:

        Vector<ux> m_unusedIndex;
    };
}
