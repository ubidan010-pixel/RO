#include "core/enginecommon.h"

#ifndef _ITF_TEXTURECONFIGURATION_H_
#include "tools/plugins/CookerPlugin/TextureConfiguration.h"
#endif //_ITF_TEXTURECONFIGURATION_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

namespace ITF
{
    //------------------------------------------------------------------------------------------------------------------------------------------------------------

    TextureConfiguration* TextureConfiguration::openTextureConfiguration(const String& _szPlatform, const String& _szFilename)
    {
        PugiXMLWrap XML;
        if (!XML.openFile(_szFilename, bfalse, PugiXMLWrap::DoNothing))
            return bfalse;

        String targetName;
        String stringTmp;

        String useMipmap;
        {
            if (XML.enterElement("TextureConfiguration"))
            {
                const char fallbackPlatform[] = "PC"; // we assume to prefer to fallback on "PC" platform if nothing is found
                TextureConfiguration* fallbackTextureConfiguration = nullptr;
                do
                {
                    TextureConfiguration* pTextureConfiguration = new TextureConfiguration();
                    pTextureConfiguration->setCompressionMode(XML.readAttribute("CompressionMode", "DXT3"));
                    pTextureConfiguration->setWrapMode(XML.readAttribute("WrapMode", "WrapMode_Repeat"));
                    pTextureConfiguration->setMipmapFilter(XML.readAttribute("Filter", "MipmapFilter_Box"));
                    pTextureConfiguration->setAutoDegrad(XML.readBoolAttribute("AllowDegradation", true));
                    pTextureConfiguration->setLodBias(XML.readFloatAttribute("LodBias", 0.f));
                    pTextureConfiguration->setAutoDegradAlpha(XML.readBoolAttribute("AllowDegradationAlpha", false));
                    stringTmp = XML.readAttribute("OneColMode", "auto");
                    if (stringTmp == "auto")
                        pTextureConfiguration->setOneColMode(TextureConfiguration::eAUTO);
                    else if (stringTmp == "yes")
                        pTextureConfiguration->setOneColMode(TextureConfiguration::eYES);
                    else if (stringTmp == "no")
                        pTextureConfiguration->setOneColMode(TextureConfiguration::eNO);

                    stringTmp = XML.readAttribute("Anisotropy", "max");
                    if (stringTmp == "max")
                        pTextureConfiguration->setAnisotropy(TextureConfiguration::eMAX);
                    else if (stringTmp == "low")
                        pTextureConfiguration->setAnisotropy(TextureConfiguration::eLOW);
                    else if (stringTmp == "none")
                        pTextureConfiguration->setAnisotropy(TextureConfiguration::eNONE);

                    targetName = XML.readAttribute("TargetName", "");
                    pTextureConfiguration->setMipmapStart(XML.readIntAttribute("MipmapLevelStart", 0));

                    useMipmap = XML.readAttribute("UseMipmap", "False");
                    if (useMipmap == "False")
                        pTextureConfiguration->setUseMipmap(bfalse);

                    if (_szPlatform.strstr((u16*)targetName.cStr()))
                    {
                        return pTextureConfiguration;
                    }
                    else
                    {
                        if (targetName == fallbackPlatform)
                        {
                            fallbackTextureConfiguration = pTextureConfiguration;
                        }
                        else
                        {
                            SF_DEL(pTextureConfiguration);
                        }
                    }
                } while (XML.nextSameElement());

                if (fallbackTextureConfiguration != nullptr)
                {
                    return fallbackTextureConfiguration;
                }
                SF_DEL(fallbackTextureConfiguration);
                XML.exitElement();
            }
        }

        return nullptr;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------
    nvtt::Format TextureConfiguration::getnvttFormat() const
    {
        const String& compressionModeString = getCompressionMode();
        if (compressionModeString == "DXT1")
            return nvtt::Format_DXT1;
        else
            if (compressionModeString == "DXT3")
                return nvtt::Format_DXT3;
            else
                if (compressionModeString == "DXT5")
                    return nvtt::Format_DXT5;
                else
                    return nvtt::Format_RGBA;
    }
    //------------------------------------------------------------------------------------------------------------------------------------------------------------
    nvtt::WrapMode TextureConfiguration::getnvttWrapMode() const
    {
        const String& wrapModeString = getWrapMode();
        if (wrapModeString == "WrapMode_Repeat")
            return  nvtt::WrapMode_Repeat;
        else
            if (wrapModeString == "WrapMode_Clamp")
                return nvtt::WrapMode_Clamp;
            else
                if (wrapModeString == "WrapMode_Mirror")
                    return  nvtt::WrapMode_Mirror;
                else
                {
                    ITF_ASSERT(0);
                }

        return nvtt::WrapMode_Repeat;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------

    nvtt::MipmapFilter TextureConfiguration::getnvttMipMapFilter() const
    {
        //Filter
        const String& mipmapFilterString = getMipmapFilter();
        if (mipmapFilterString == "MipmapFilter_Box")
            return nvtt::MipmapFilter_Box;
        else
            if (mipmapFilterString == "MipmapFilter_Triangle")
                return nvtt::MipmapFilter_Triangle;
            else
                if (mipmapFilterString == "MipmapFilter_Kaiser")
                    return nvtt::MipmapFilter_Kaiser;
                else
                {
                    ITF_ASSERT(0);
                }

        return  nvtt::MipmapFilter_Box;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------

    // The list of supported formats for texture conversion extracted for texconv help
    static const char* gs_texConvFormats[] =
    {
        "DXT1",
        "DXT2",
        "DXT3",
        "DXT4",
        "DXT5",
        "RGBA",
        "R32G32B32A32_FLOAT",
        "R32G32B32A32_UINT",
        "R32G32B32A32_SINT",
        "R32G32B32_FLOAT",
        "R32G32B32_UINT",
        "R32G32B32_SINT",
        "R16G16B16A16_FLOAT",
        "R16G16B16A16_UNORM",
        "R16G16B16A16_UINT",
        "R16G16B16A16_SNORM",
        "R16G16B16A16_SINT",
        "R32G32_FLOAT",
        "R32G32_UINT",
        "R32G32_SINT",
        "R10G10B10A2_UNORM",
        "R10G10B10A2_UINT",
        "R11G11B10_FLOAT",
        "R8G8B8A8_UNORM"
        "R8G8B8A8_UNORM_SRGB",
        "R8G8B8A8_UINT",
        "R8G8B8A8_SNORM",
        "R8G8B8A8_SINT",
        "R16G16_FLOAT",
        "R16G16_UNORM",
        "R16G16_UINT",
        "R16G16_SNORM",
        "R16G16_SINT",
        "R32_FLOAT",
        "R32_UINT",
        "R32_SINT",
        "R8G8_UNORM",
        "R8G8_UINT",
        "R8G8_SNORM",
        "R8G8_SINT",
        "R16_FLOAT",
        "R16_UNORM",
        "R16_UINT",
        "R16_SNORM",
        "R16_SINT",
        "R8_UNORM",
        "R8_UINT",
        "R8_SNORM",
        "R8_SINT",
        "A8_UNORM",
        "R9G9B9E5_SHAREDEXP",
        "R8G8_B8G8_UNORM",
        "G8R8_G8B8_UNORM",
        "BC1_UNORM",
        "BC1_UNORM_SRGB",
        "BC2_UNORM",
        "BC2_UNORM_SRGB",
        "BC3_UNORM",
        "BC3_UNORM_SRGB",
        "BC4_UNORM",
        "BC4_SNORM",
        "BC5_UNORM",
        "BC5_SNORM",
        "B5G6R5_UNORM",
        "B5G5R5A1_UNORM",
        "B8G8R8A8_UNORM",
        "B8G8R8X8_UNORM",
        "R10G10B10_XR_BIAS_A2_UNORM",
        "B8G8R8A8_UNORM_SRGB",
        "B8G8R8X8_UNORM_SRGB",
        "BC6H_UF16",
        "BC6H_SF16",
        "BC7_UNORM",
        "BC7_UNORM_SRGB",
        "AYUV",
        "Y410",
        "Y416",
        "YUY2",
        "Y210",
        "Y216",
        "B4G4R4A4_UNORM",
        "A4B4G4R4_UNORM",
        "BGRA",
        "BGR",
        "FP16",
        "FP32",
        "BPTC",
        "BPTC_FLOAT",
        "BC3n",
        "DXT5nm",
        "RXGB"
    };

String TextureConfiguration::getTexConvFormatOption() const
{
    String formatOption = " -f ";

    auto & compMode = getCompressionMode();

    for (const char * format : gs_texConvFormats)
    {
        if (compMode == format)
        {
            return formatOption + format;
        }
    }

    return formatOption + "RGBA";
}

String TextureConfiguration::getTexConvWrapModeOption() const
{
    const String& wrapModeString = getWrapMode();
    if (wrapModeString == "WrapMode_Repeat")
        return String(" -wrap");
    else
        if (wrapModeString == "WrapMode_Clamp")
            return String();
        else
            if (wrapModeString == "WrapMode_Mirror")
                return String(" -mirror");
            else
            {
                ITF_ASSERT(0);
            }

    return String(" -wrap");
}
}
