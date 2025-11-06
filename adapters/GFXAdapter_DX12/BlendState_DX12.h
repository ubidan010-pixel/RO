#pragma once

#include "core/types.h"

#include "engine/AdaptersInterfaces/GFXAdapter.h"

#include "DX12/Helpers.h"


namespace ITF::DX12
{
    class BlendState
    {
    public:

        explicit BlendState(GFX_BLENDMODE _blend = GFX_BLEND_ALPHA)
            : m_key{ build(_blend) }
        {
        }

        BlendState(const BlendState&) = default;

        BlendState& operator=(const BlendState&) = default;

        BlendState& operator=(GFX_BLENDMODE _blend)
        {
            set(_blend);
            return *this;
        }

        void set(GFX_BLENDMODE _blend)
        {
            *this = BlendState(_blend);
        }

        void reset()
        {
            *this = BlendState();
        }

        u32 getKeyAsUInt() const // to be used as map key
        {
            return m_key.value;
        }

        D3D12_BLEND_DESC buildBlendDesc() const;

        bool operator==(const BlendState& other) const
        {
            return m_key.value == other.m_key.value;
        }

        bool operator!=(const BlendState& other) const
        {
            return !(*this == other);
        }

        const char* getName() const;

    private:

        union BlendDescKey
        {
            u32 value;
            struct
            {
                u32 blendEnable : 1;
                u32 srcBlend : 5;
                u32 destBlend : 5;
                u32 blendOp : 3;
                u32 srcBlendAlpha : 5;
                u32 destBlendAlpha : 5;
                u32 blendOpAlpha : 3;
                u32 renderTargetWriteMask : 4;
                u32 rest : 1;
            };
        };

        static BlendDescKey build(GFX_BLENDMODE _blend);

        BlendDescKey m_key;
    };

}
