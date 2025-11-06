#ifndef _ITF_DDSFORMAT_H_
#define _ITF_DDSFORMAT_H_

namespace ITF
{

    struct DDSCAPS2
    {
        DWORD dwCaps;
        DWORD dwCaps2;
        DWORD Reserved[2];

        void swapToBigEndian()
        {
            Endian::swapBigEndian(dwCaps,(u8*)&dwCaps);
            Endian::swapBigEndian(dwCaps2,(u8*)&dwCaps2);

        }
    };

    struct DDPIXELFORMAT
    {
        DWORD dwSize;
        DWORD dwFlags;
        DWORD dwFourCC;
        DWORD dwRGBBitCount;
        DWORD dwRBitMask;
        DWORD dwGBitMask;
        DWORD dwBBitMask;
        DWORD dwABitMask;

        enum : DWORD
        {
            ALPHAPIXELS = 0x1, // Texture contains alpha data; dwRGBAlphaBitMask contains valid data.
            ALPHA = 0x2,       // Used in some older DDS files for alpha channel only uncompressed data (dwRGBBitCount contains the alpha channel bitcount; dwABitMask contains valid data).
            FOURCC = 0x4,      // dwFourCC contains valid data.
            RGB = 0x40,        // Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks(dwRBitMask, dwGBitMask, dwBBitMask) contain valid data.
            YUV = 0x200,       // Used in some older DDS files for YUV uncompressed data(dwRGBBitCount contains the YUV bit count; dwRBitMask contains the Y mask, dwGBitMask contains the U mask, dwBBitMask contains the V mask)
            LUMINANCE = 0x2000 // Used in some older DDS files for single channel color uncompressed data(dwRGBBitCount contains the luminance channel bit count; dwRBitMask contains the channel mask).Can be combined with DDPF_ALPHAPIXELS for a two channel DDS file.
        };

        void swapToBigEndian()
        {

            Endian::swapBigEndian(dwSize,(u8*)&dwSize);
            Endian::swapBigEndian(dwFlags,(u8*)&dwFlags);
            Endian::swapBigEndian(dwFourCC,(u8*)&dwFourCC);
            Endian::swapBigEndian(dwRGBBitCount,(u8*)&dwRGBBitCount);
            Endian::swapBigEndian(dwGBitMask,(u8*)&dwGBitMask);
            Endian::swapBigEndian(dwBBitMask,(u8*)&dwBBitMask);
            Endian::swapBigEndian(dwABitMask,(u8*)&dwABitMask);

        }
    };

    struct DDS_USER
    {
        DWORD dwLodBias;
        DWORD dwAlphaBias;
        DWORD dwbHasOneColor;
        DWORD dwAnisotropy;
        DWORD dwReserved[7];
    };

    enum DDS_FLAGS : DWORD
    {
        DDSD_CAPS       = 0x1,      // Required in every.dds file.
        DDSD_HEIGHT     = 0x2,      // Required in every.dds file.
        DDSD_WIDTH      = 0x4,      // Required in every.dds file.
        DDSD_PITCH      = 0x8,      // Required when pitch is provided for an uncompressed texture.
        DDSD_PIXELFORMAT= 0x1000,   // Required in every.dds file.
        DDSD_MIPMAPCOUNT= 0x20000,  // Required in a mipmapped texture.
        DDSD_LINEARSIZE = 0x80000,  // Required when pitch is provided for a compressed texture.
        DDSD_DEPTH      = 0x800000  // Required in a depth texture.
    };

    struct DDS_HEADER
    {
        DWORD dwMagic;
        DWORD dwSize;
        DWORD dwFlags;
        DWORD dwHeight;
        DWORD dwWidth;
        DWORD dwPitchOrLinearSize;
        DWORD dwDepth;
        DWORD dwMipMapCount;

        union
        {
            DDS_USER    dwUser;
            DWORD       dwReserved1[11];
        };

        DDPIXELFORMAT format;
        DDSCAPS2 ddsCaps;
        DWORD dwReserved2;

        void swapToBigEndian()
        {
            Endian::swapBigEndian(dwMagic,(u8*)&dwMagic);
            Endian::swapBigEndian(dwSize,(u8*)&dwSize);
            Endian::swapBigEndian(dwFlags,(u8*)&dwFlags);
            Endian::swapBigEndian(dwHeight,(u8*)&dwHeight);
            Endian::swapBigEndian(dwWidth,(u8*)&dwWidth);
            Endian::swapBigEndian(dwPitchOrLinearSize,(u8*)&dwPitchOrLinearSize);
            Endian::swapBigEndian(dwDepth,(u8*)&dwDepth);
            Endian::swapBigEndian(dwMipMapCount,(u8*)&dwMipMapCount);

            for (u32 i=0;i<11;++i)
            {
                Endian::swapBigEndian(dwReserved1[i],(u8*) &dwReserved1[i]);          
            }

            format.swapToBigEndian();
            ddsCaps.swapToBigEndian();

            Endian::swapBigEndian(dwReserved2,(u8*)&dwReserved2);

        }
    };
} // namespace ITF

#endif // _ITF_DDSFORMAT_H_
