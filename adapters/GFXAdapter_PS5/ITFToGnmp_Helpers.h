#ifndef ITF_GFX_TO_GNMP_HELPERS_H_
#define ITF_GFX_TO_GNMP_HELPERS_H_

#pragma once

#include "Gnmp_Helpers.h"
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#include "engine/display/Texture.h"

namespace ITF
{
    inline Gnmp::WrapMode itfToGnmp(GFX_TEXADRESSMODE _texAddressMode)
    {
        switch (_texAddressMode)
        {
        case GFXTADDRESS_WRAP:
            return Gnmp::kWrapModeWrap;
        case GFXTADDRESS_MIRROR:
            return Gnmp::kWrapModeMirror;
        case GFXTADDRESS_CLAMP:
            return Gnmp::kWrapModeClampLastTexel;
        case GFXTADDRESS_BORDER:
            return Gnmp::kWrapModeClampBorder;
        default:
            break;
        }
        ITF_ASSERT_CRASH(false, "Invalid primitive type %d", static_cast<int>(_texAddressMode));
        return Gnmp::kWrapModeWrap;
    }

    inline Texture::PixFormat gnmpToITFTexFormat(Gnmp::DataFormat format)
    {
        Gnmp::SurfaceFormat gnmSurfFormat = format.getSurfaceFormat();

        switch (gnmSurfFormat)
        {
            case Gnmp::kSurfaceFormatBc1:
                return Texture::PF_DXT1;
            case Gnmp::kSurfaceFormatBc2:
                return Texture::PF_DXT2;
            case Gnmp::kSurfaceFormatBc3:
                return Texture::PF_DXT5;
            case Gnmp::kSurfaceFormat8_8_8_8:
            {
                if (format.getChannel(3) != Gnmp::kTextureChannelW)
                    return Texture::PF_RGB;
                else
                    return Texture::PF_RGBA;
            }
            case Gnmp::kSurfaceFormat8_8:
                return Texture::PF_LA8;
            case Gnmp::kSurfaceFormat8:
            {
                if (format.getChannel(3) == Gnmp::kTextureChannelX)
                    return Texture::PF_A8;
                else
                    return Texture::PF_L8;
            }
            default:
            {
                ITF_ASSERT(0); // not supported format
            }
            break;
        }
        return Texture::PF_RGBA;
    }

    inline Gnmp::DataFormat itfToGnmpTexFormat(Texture::PixFormat _pixformat)
    {
        Gnmp::DataFormat pformat;
        pformat = Gnmp::kDataFormatR8G8B8A8Unorm;
        switch (_pixformat)
        {
        case Texture::PF_RGB:
            ITF_ASSERT(0);
            break;

        case Texture::PF_DXT1:
            pformat = Gnmp::kDataFormatBc1Unorm;
            break;
        case Texture::PF_DXT2:
        case Texture::PF_DXT3:
            pformat = Gnmp::kDataFormatBc2Unorm;
            break;
        case Texture::PF_DXT4:
        case Texture::PF_DXT5:
            pformat = Gnmp::kDataFormatBc3Unorm;
            break;
        case Texture::PF_RGBA:
            pformat = Gnmp::kDataFormatR8G8B8A8Unorm;
            break;
        case Texture::PF_L8:
            pformat = Gnmp::kDataFormatR8Unorm;
            break;
        case Texture::PF_A8:
            pformat = Gnmp::kDataFormatA8Unorm;
            break;
        default:
            {
                ITF_ASSERT_CRASH(false, "Unknown texture format");
            }
            break;
        }
        return pformat;
    }

    inline Gnmp::PrimitiveType itfToGnmpPrimitiveType(PRIMITIVETYPE _type)
    {
        switch (_type)
        {
        case GFX_TRIANGLES:
            return Gnmp::kPrimitiveTypeTriList;
        case GFX_POINTS:
            return Gnmp::kPrimitiveTypePointList;
        case GFX_LINES:
            return Gnmp::kPrimitiveTypeLineList;
        case GFX_LINE_STRIP:
            return Gnmp::kPrimitiveTypeLineStrip;
        case GFX_TRIANGLE_STRIP:
            return Gnmp::kPrimitiveTypeTriStrip;
        case GFX_TRIANGLE_FAN:
            return Gnmp::kPrimitiveTypeTriFan;
        default:
            break;
        }

        ITF_ASSERT_CRASH(false, "Invalid primitive type %d", static_cast<int>(_type));

        return Gnmp::kPrimitiveTypeTriList;
    }

}

#endif // ITF_GFX_TO_GNMP_HELPERS_H_
