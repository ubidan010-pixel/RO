#ifndef VRAMTOOLS_H_
#define VRAMTOOLS_H_

#include "VRAMMemoryTypes.h"
#include "../Tools_GCM.h"

namespace ITF
{
    namespace GCM
    {
        namespace VRAMAllocator
        {
            enum
            {
                // offset area for reports
                ReportIOOffsetStart = 0x0E000000u,
                ReportIOOffsetEnd = 0x0F000000u
            };
            

            VRAMAddress AllocRSXMapableMainMemory(u32 _size);

            void AllocRSXMappedMainMemory(u32 _size, VRAMAddress & _ea, VRAMOffset & _rsxOffset, bool _reportArea = false);

            void FreeRSXMappedMainMemory(VRAMAddress _ea);

            // simple allocator
            // could be more complex and effectively bind tile ?
            class TileAreaAllocator : public TemplateSingleton<TileAreaAllocator>
            {
            public:
                TileAreaAllocator();

                u32 allocUnusedIndex(); // return u32(-1) if no more are available
                u32 incRefIndex(u32 _idx); // return the ref count after the operation ()
                u32 releaseIndex(u32 _idx); // return the ref count after the operation. If 0, it's user reponsability to unbind the region

                enum 
                {
                    InvalidTileIndex = u32(-1)
                };

            private:
                enum 
                {
                    NbMaxTileArea = 15
                };
                u32 m_tileAreaUsed[NbMaxTileArea];
            };

            class ZCullMemoryManager : public TemplateSingleton<ZCullMemoryManager>
            {
            public:
                enum
                {
                    InvalidRegionIndex = u32(-1)
                };
                u32 bindRegion(int _offset, int _width, int _height, int _zFormat, int _aaFormat);
                void unBindRegion(u32 _index);

            protected:
                class ZCullBindInfo
                {
                public:
                    ZCullBindInfo();
                    ZCullBindInfo(int offset,int width,int height,int zFormat,int aaFormat);
                    void reset();
                    u32 m_width;
                    u32 m_height;
                    u32 m_offset;
                    u32 m_zformat;
                    u32 m_aaformat;
                };

                u32 getUnusedZCullIndex();
                bool findDuplicate(u32 _rsxOffset);

            private:
                enum
                {
                    ZCULL_MAX_INDEX = 8u,
                    ZCULL_WIDTH_ALIGMENT = 64u,
                    ZCULL_HEIGTH_ALIGMENT = 64u,
                    ZCULL_MAX_SIZE_IN_PIXEL = 2048u*1536u
                };

                ZCullBindInfo m_zbindInfos[ZCULL_MAX_INDEX];
            };
        }        
    }
}

#endif  // VRAMTOOLS_H_
