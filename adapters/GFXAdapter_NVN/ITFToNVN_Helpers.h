#pragma once

#include "core/Macros.h"
#include "core/utility.h"
#include "NVN/Helpers.h"
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#include "engine/display/Texture.h"

namespace ITF
{
    inline nvn::MinFilter itfToNVNMinFilter(bool _isBilinear)
    {
        return _isBilinear ? nvn::MinFilter::LINEAR_MIPMAP_LINEAR : nvn::MinFilter::NEAREST_MIPMAP_NEAREST;
    }

    inline nvn::MagFilter itfToNVNMagFilter(bool _isBilinear)
    {
        return _isBilinear ? nvn::MagFilter::LINEAR : nvn::MagFilter::NEAREST;
    }

    inline nvn::WrapMode itfToNVN(GFX_TEXADRESSMODE _texAddressMode)
    {
        switch (_texAddressMode)
        {
        case GFXTADDRESS_WRAP:
            return nvn::WrapMode::REPEAT;
        case GFXTADDRESS_MIRROR:
            return nvn::WrapMode::MIRRORED_REPEAT;
        case GFXTADDRESS_CLAMP:
            return nvn::WrapMode::CLAMP;
        case GFXTADDRESS_BORDER:
            return nvn::WrapMode::CLAMP_TO_BORDER;
        default:
            break;
        }
        ITF_ASSERT_CRASH(false, "Invalid primitive type %d", toUnderlying(_texAddressMode));
        return nvn::WrapMode::REPEAT;
    }

    inline Texture::PixFormat nvnToITFTexFormat(nvn::Format _format)
    {
        switch (_format)
        {
        case nvn::Format::RGBA_DXT1:
            return Texture::PF_DXT1;
        case nvn::Format::RGBA_DXT3:
            return Texture::PF_DXT3;
        case nvn::Format::RGBA_DXT5:
            return Texture::PF_DXT5;
        case nvn::Format::RGBA8:
            return Texture::PF_RGBA;
        case nvn::Format::RGB8:
            return Texture::PF_RGB;
        case nvn::Format::RG8:
            return Texture::PF_LA8;
        case nvn::Format::R8:
            return Texture::PF_L8;
        default:
            break;
        }
        ITF_ASSERT_CRASH(false, "Unknown nvn texture format %d", toUnderlying(static_cast<nvn::Format::Enum>(_format)));
        return Texture::PF_RGBA;
    }

    inline nvn::Format itfToNvnTexFormat(Texture::PixFormat _pixformat)
    {
        nvn::Format pformat = nvn::Format::RGBA8;

        switch (_pixformat)
        {
        case Texture::PF_RGB:
            ITF_ASSERT(0);
            break;

        case Texture::PF_DXT1:
            pformat = nvn::Format::RGBA_DXT1;
            break;
        case Texture::PF_DXT2:
        case Texture::PF_DXT3:
            pformat = nvn::Format::RGBA_DXT3;
            break;
        case Texture::PF_DXT4:
        case Texture::PF_DXT5:
            pformat = nvn::Format::RGBA_DXT5;
            break;
        case Texture::PF_RGBA:
            pformat = nvn::Format::RGBA8;
            break;
        case Texture::PF_L8:
            pformat = nvn::Format::R8;
            break;
        case Texture::PF_A8:
            ITF_ASSERT(0); // need more efforts (swizzle)
            pformat = nvn::Format::R8;
            break;
        default:
            ITF_ASSERT_CRASH(false, "Unknown texture ITF texture format %d", toUnderlying(_pixformat));
            break;
        }
        return pformat;
    }

    inline nvn::DrawPrimitive itfToNvnPrimitiveType(PRIMITIVETYPE _type)
    {
        switch (_type)
        {
        case GFX_TRIANGLES:
            return nvn::DrawPrimitive::TRIANGLES;
        case GFX_POINTS:
            return nvn::DrawPrimitive::POINTS;
        case GFX_LINES:
            return nvn::DrawPrimitive::LINES;
        case GFX_LINE_STRIP:
            return nvn::DrawPrimitive::LINE_STRIP;
        case GFX_TRIANGLE_STRIP:
            return nvn::DrawPrimitive::TRIANGLE_STRIP;
        case GFX_TRIANGLE_FAN:
            return nvn::DrawPrimitive::TRIANGLE_FAN;
        case GFX_QUADS:
            return nvn::DrawPrimitive::QUADS;
        default:
            break;
        }
        ITF_ASSERT_CRASH(false, "Invalid ITF primitive type %d", toUnderlying(_type));
        return nvn::DrawPrimitive::TRIANGLES;
    }
}
