#include "precompiled_GFXAdapter_DX12.h"

#include "BlendState_DX12.h"

namespace ITF::DX12
{
    static GFX_BLENDMODE getGFXBlendMode(BlendState blendState)
    {
        for (int mode = GFX_BLEND_UNKNOWN + 1; mode <= GFX_BLEND_NUMBER; ++mode)
        {
            GFX_BLENDMODE blendMode = static_cast<GFX_BLENDMODE>(mode);
            BlendState testBlend(blendMode);

            if (testBlend.getKeyAsUInt() == blendState.getKeyAsUInt())
            {
                return blendMode;
            }
        }
        return GFX_BLEND_UNKNOWN;
    }

    BlendState::BlendDescKey BlendState::build(GFX_BLENDMODE _blend)
    {
        BlendDescKey blendKey{};
        blendKey.blendEnable = false;
        blendKey.renderTargetWriteMask =
            D3D12_COLOR_WRITE_ENABLE_RED |
            D3D12_COLOR_WRITE_ENABLE_GREEN |
            D3D12_COLOR_WRITE_ENABLE_BLUE |
            D3D12_COLOR_WRITE_ENABLE_ALPHA;

        switch (_blend)
        {
        case GFX_BLEND_COPY:
            blendKey.blendEnable = false;

            blendKey.srcBlend = D3D12_BLEND_ONE;
            blendKey.destBlend = D3D12_BLEND_ZERO;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        default:
        case GFX_BLEND_UNKNOWN:
            ITF_ASSERT_MSG(false, "Incorrect blend mode");
            //missing break intentionally
        case GFX_BLEND_ALPHA:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_SRC_ALPHA;
            blendKey.destBlend = D3D12_BLEND_INV_SRC_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_ALPHAPREMULT:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_ONE;
            blendKey.destBlend = D3D12_BLEND_INV_SRC_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_ALPHADEST:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_DEST_ALPHA;
            blendKey.destBlend = D3D12_BLEND_INV_DEST_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_ALPHADESTPREMULT:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_ONE;
            blendKey.destBlend = D3D12_BLEND_SRC_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_ADD:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_ONE;
            blendKey.destBlend = D3D12_BLEND_ONE;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_ADDALPHA:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_SRC_ALPHA;
            blendKey.destBlend = D3D12_BLEND_ONE;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_SUBALPHA:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_SRC_ALPHA;
            blendKey.destBlend = D3D12_BLEND_ONE;
            blendKey.blendOp = D3D12_BLEND_OP_REV_SUBTRACT;
            break;

        case GFX_BLEND_SUB:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_ZERO;
            blendKey.destBlend = D3D12_BLEND_INV_SRC_COLOR;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_MUL:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_ZERO;
            blendKey.destBlend = D3D12_BLEND_SRC_COLOR;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_ALPHAMUL:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_ZERO;
            blendKey.destBlend = D3D12_BLEND_SRC_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_IALPHAMUL:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_ZERO;
            blendKey.destBlend = D3D12_BLEND_INV_SRC_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_IALPHA:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_INV_SRC_ALPHA;
            blendKey.destBlend = D3D12_BLEND_SRC_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_IALPHAPREMULT:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_ONE;
            blendKey.destBlend = D3D12_BLEND_SRC_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_IALPHADEST:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_INV_DEST_ALPHA;
            blendKey.destBlend = D3D12_BLEND_DEST_ALPHA;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_IALPHADESTPREMULT:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_INV_DEST_ALPHA;
            blendKey.destBlend = D3D12_BLEND_ONE;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_MUL2X:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_DEST_COLOR;
            blendKey.destBlend = D3D12_BLEND_SRC_COLOR;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_ALPHATOCOLOR:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_DEST_ALPHA;
            blendKey.destBlend = D3D12_BLEND_ZERO;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_IALPHATOCOLOR:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_INV_DEST_ALPHA;
            blendKey.destBlend = D3D12_BLEND_ZERO;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;
            break;

        case GFX_BLEND_SETTOCOLOR:
            blendKey.blendEnable = true;

            blendKey.srcBlend = D3D12_BLEND_DEST_COLOR;
            blendKey.destBlend = D3D12_BLEND_ZERO;
            blendKey.blendOp = D3D12_BLEND_OP_ADD;

            break;
        }

        // Some blend factors can be invalid for alpha, filter it
        switch (_blend)
        {
        case GFX_BLEND_SUB:
        case GFX_BLEND_MUL:
        case GFX_BLEND_MUL2X:
        case GFX_BLEND_SETTOCOLOR:
            blendKey.srcBlendAlpha = D3D12_BLEND_ONE;
            blendKey.destBlendAlpha = D3D12_BLEND_ZERO;
            blendKey.blendOpAlpha = D3D12_BLEND_OP_ADD;
            break;

        default:
            blendKey.srcBlendAlpha = blendKey.srcBlend;
            blendKey.destBlendAlpha = blendKey.destBlend;
            blendKey.blendOpAlpha = blendKey.blendOp;
            break;
        }

        return blendKey;
    }

    D3D12_BLEND_DESC BlendState::buildBlendDesc() const
    {
        D3D12_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;

        for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
        {
            desc.RenderTarget[i].BlendEnable = m_key.blendEnable;
            desc.RenderTarget[i].SrcBlend = static_cast<D3D12_BLEND>(m_key.srcBlend);
            desc.RenderTarget[i].DestBlend = static_cast<D3D12_BLEND>(m_key.destBlend);
            desc.RenderTarget[i].BlendOp = static_cast<D3D12_BLEND_OP>(m_key.blendOp);
            desc.RenderTarget[i].SrcBlendAlpha = static_cast<D3D12_BLEND>(m_key.srcBlendAlpha);
            desc.RenderTarget[i].DestBlendAlpha = static_cast<D3D12_BLEND>(m_key.destBlendAlpha);
            desc.RenderTarget[i].BlendOpAlpha = static_cast<D3D12_BLEND_OP>(m_key.blendOpAlpha);
            desc.RenderTarget[i].RenderTargetWriteMask = m_key.renderTargetWriteMask;
        }

        return desc;
    }

    const char* BlendState::getName() const
    {
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        GFX_BLENDMODE gfxBlendMode = getGFXBlendMode(*this);
        switch (gfxBlendMode)
        {
        case GFX_BLEND_COPY:               return "Copy";
        case GFX_BLEND_ALPHA:              return "Alpha";
        case GFX_BLEND_ALPHAPREMULT:       return "AlphaPremult";
        case GFX_BLEND_ALPHADEST:          return "AlphaDest";
        case GFX_BLEND_ALPHADESTPREMULT:   return "AlphaDestPremult";
        case GFX_BLEND_ADD:                return "Add";
        case GFX_BLEND_ADDALPHA:           return "AddAlpha";
        case GFX_BLEND_SUBALPHA:           return "SubAlpha";
        case GFX_BLEND_SUB:                return "Sub";
        case GFX_BLEND_MUL:                return "Mul";
        case GFX_BLEND_ALPHAMUL:           return "AlphaMul";
        case GFX_BLEND_IALPHAMUL:          return "IAlphaMul";
        case GFX_BLEND_IALPHA:             return "IAlpha";
        case GFX_BLEND_IALPHAPREMULT:      return "IAlphaPremult";
        case GFX_BLEND_IALPHADEST:         return "IAlphaDest";
        case GFX_BLEND_IALPHADESTPREMULT:  return "IAlphaDestPremult";
        case GFX_BLEND_MUL2X:              return "Mul2x";
        case GFX_BLEND_ALPHATOCOLOR:       return "AlphaToColor";
        case GFX_BLEND_IALPHATOCOLOR:      return "IAlphaToColor";
        case GFX_BLEND_SETTOCOLOR:         return "SetToColor";
        case GFX_BLEND_UNKNOWN:
        default:                           return "Unknown";
        };
    #else
        return "Unknown";
    #endif
    }

}
