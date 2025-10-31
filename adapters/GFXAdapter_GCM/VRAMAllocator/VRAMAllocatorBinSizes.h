#ifndef VRAMALLOCATORBINSIZES_H_
#define VRAMALLOCATORBINSIZES_H_

namespace ITF
{
    namespace GCM
    {
        namespace VRAMAllocator
        {

        // Bin size array to get fast access to the list of free blocks in a size range
        // Crafted to get common texture size (see code below)
        u32 BinSizes[] =
        {
            0, // keep 0 here
            16,
            32,
            48,
            64,
            80,
            96,
            112,
            128,
            192,
            256,
            384,
            512,
            640,
            768,
            896,
            1024,
            1152,
            1280,
            1408,
            1536,
            1664,
            1792,
            2048,
            2176,
            2432,
            2560,
            2688,
            2816,
            2944,
            3072,
            3200,
            3456,
            3584,
            4096,
            4224,
            4864,
            4992,
            5120,
            5248,
            5504,
            5632,
            5760,
            6144,
            6272,
            6912,
            7168,
            8192,
            9600,
            9728,
            9856,
            10240,
            11008,
            11136,
            11392,
            12288,
            13696,
            13824,
            14336,
            16384,
            19200,
            19328,
            19456,
            20480,
            21888,
            22016,
            22144,
            22528,
            24576,
            27392,
            27648,
            38272,
            38400,
            38912,
            43776,
            43904,
            44032,
            45056,
            54656,
            54784,
            76544,
            76800,
            87424,
            87552,
            88064,
            109312,
            152960,
            153088,
            174848,
            175104,
            218496,
            305920,
            349568,
            349696,
            436992,
            611712,
            699136,
            873856,
            1223424,
            1398144,
            1747712,
            2446720,
            2796288,
            3495296,
            4893440,
            5592448,
            6990592,
            9786752,
            0xFFFFFFFF // keep 4Go-1 here (to avoid a special "last" case)
        };

        const u32 NumberOfBinSizes = sizeof(BinSizes)/sizeof(BinSizes[0]);
        } // namespace VRAMAllocator
    } // namespace GCM
} // namespace ITF

// code used to generate values (after 128)
#if 0
int AboveAlign(int s, int align)
{
    return ((s + (align-1)) & ~(align-1));
}

int CalcTileCompliantSize(int w, int h, int tileTexelSize, int tileByteSize)
{
    w = AboveAlign(w, tileTexelSize)/tileTexelSize;
    h = AboveAlign(h, tileTexelSize)/tileTexelSize;

    return w*h*tileByteSize;
}

int CalcSize(int w, int h, int tileTexelSize, int tileByteSize)
{
    int totalSize = w*h;
    for(int i = 0; i<9 && (w>1 || h>1); ++i)
    {
        w = std::max(w >> 1, 1);
        h = std::max(h >> 1, 1);
        totalSize += CalcTileCompliantSize(w, h, tileTexelSize, tileByteSize);
    }
    return AboveAlign(totalSize, 128);
}

typedef std::set<int> SetSize;

int main(int argc, const char * argv[])
{
    SetSize setSize;

    // DXT1
    for (int i=1; i<12; i++)
    {
        for (int j=1; j<12; j++)
        {
            setSize.insert(CalcSize(1<<i, 1<<j, 4, 8));
        }
    }

    // DXT5
    for (int i=1; i<12; i++)
    {
        for (int j=1; j<12; j++)
        {
            setSize.insert(CalcSize(1<<i, 1<<j, 4, 16));
        }
    }

    // RGBA8
    for (int i=1; i<12; i++)
    {
        for (int j=1; j<12; j++)
        {
            setSize.insert(CalcSize(1<<i, 1<<j, 1, 4));
        }
    }

    // RGB 16 bit
    for (int i=1; i<12; i++)
    {
        for (int j=1; j<12; j++)
        {
            setSize.insert(CalcSize(1<<i, 1<<j, 1, 2));
        }
    }

    for(SetSize::iterator it = setSize.begin();
        it!=setSize.end();
        ++it)
    {
        printf("%d,\n", *it);
    }
}
#endif



#endif // VRAMALLOCATORBINSIZES_H_
