#include "precompiled_GFXAdapter_GCM.h"

#include "../GCM_API.h"

#include <sys/memory.h>
#include "VRAMTools.h"

namespace ITF
{
    namespace GCM
    {

        namespace VRAMAllocator
        {

            VRAMAddress AllocRSXMapableMainMemory(u32 _size)
            {
                VRAM_ALLOC_ASSERT(_size<256*1024*1024);
                VRAM_ALLOC_ASSERT(_size == GetAboveAlign(_size, 1024u*1024u)); // 1M block only for RSX mapped memory
                sys_addr_t addr;
                ITF_VERIFY(CELL_OK == sys_memory_allocate(_size, SYS_MEMORY_PAGE_SIZE_1M, &addr));
                return VRAMAddress(addr);
            }

            void AllocRSXMappedMainMemory(u32 _size, VRAMAddress & _ea, VRAMOffset & _rsxOffset, bool _reportArea)
            {
                _ea = AllocRSXMapableMainMemory(_size);
                _rsxOffset = 0;
                if (_ea)
                {
                    if (_reportArea)
                    {
                        static u32 reportAreaStart = 0x0e000000u;
                        ITF_ASSERT(reportAreaStart < 0x0f000000u); // Out of IO report area
                        _rsxOffset = reportAreaStart;
                        if (CELL_OK == cellGcmMapEaIoAddress(_ea, reportAreaStart, _size))
                        {
                            _rsxOffset = reportAreaStart;
                            reportAreaStart += _size;
                        }
                    }
                    else
                    {
                        ITF_VERIFY(CELL_OK == cellGcmMapMainMemory(_ea, _size, &_rsxOffset));
                    }
                }
            }

            void FreeRSXMappedMainMemory(VRAMAddress _ea)
            {
                ITF_VERIFY(CELL_OK == cellGcmUnmapEaIoAddress(_ea));
                ITF_VERIFY(CELL_OK == sys_memory_free(sys_addr_t(_ea)));
            }

            //////////////////////////////////////////////////////////////////////////
            // TileAreaAllocator
            //////////////////////////////////////////////////////////////////////////

            TileAreaAllocator::TileAreaAllocator()
            {
                for(u32 i = 0; i<NbMaxTileArea; ++i)
                {
                    m_tileAreaUsed[i] = 0;
                }
            }

            u32 TileAreaAllocator::allocUnusedIndex()
            {
                for(u32 i = 0; i<NbMaxTileArea; ++i)
                {
                    if (!m_tileAreaUsed[i])
                    {
                        m_tileAreaUsed[i] = 1;
                        return i;
                    }
                }
                ITF_ASSERT(0); // out of tile areas
                return InvalidTileIndex;
            }

            // return the ref count after the operation
            u32 TileAreaAllocator::incRefIndex(u32 _idx)
            {
                ITF_ASSERT(_idx < NbMaxTileArea); // out of tile areas
                ITF_ASSERT(m_tileAreaUsed[_idx]>0); // releasing a tile not used !
                m_tileAreaUsed[_idx]++;
                return m_tileAreaUsed[_idx];
            }

            u32 TileAreaAllocator::releaseIndex(u32 _idx)
            {
                ITF_ASSERT(_idx < NbMaxTileArea); // out of tile areas
                ITF_ASSERT(m_tileAreaUsed[_idx]>0); // releasing a tile not used !
                m_tileAreaUsed[_idx]--;
                return m_tileAreaUsed[_idx];
            }

            //////////////////////////////////////////////////////////////////////////
            //  ZCullMemoryManager
            //////////////////////////////////////////////////////////////////////////

            ZCullMemoryManager::ZCullBindInfo::ZCullBindInfo()
            {
                reset();
            }

            ZCullMemoryManager::ZCullBindInfo::ZCullBindInfo(int offset,int width,int height,int zFormat,int aaFormat)
            {
                m_width			= width;
                m_height		= height;
                m_offset		= offset;
                m_zformat		= zFormat;
                m_aaformat		= aaFormat;
            }

            void ZCullMemoryManager::ZCullBindInfo::reset()
            {
                m_width			= 0;
                m_height		= 0;
                m_offset		= 0;
                m_zformat		= CELL_GCM_ZCULL_Z24S8;
                m_aaformat		= CELL_GCM_SURFACE_CENTER_1;
            }

            bool ZCullMemoryManager::findDuplicate(u32 _rsxOffset)
            {
                // loop through all the bind to make sure there no duplicate
                for(u32 i = 0; i < ZCULL_MAX_INDEX; i++)
                {
                    ZCullBindInfo & compareBindInfo = m_zbindInfos[i];
                    if (_rsxOffset == compareBindInfo.m_offset)
                    {
                        // found a z cull region bound to the same surface, 
                        // cant have two surface bound to one z cull region
                        return true;
                    }
                }
                return false;
            }

            u32 ZCullMemoryManager::bindRegion(int _offset, int _width, int _height, int _zFormat, int _aaFormat)
            {
                // check the size of the surface we want to bind to a z cull region
                ITF_ASSERT_MSG((_width * _height) <= ZCULL_MAX_SIZE_IN_PIXEL, "Cant bind this surface to a z cull region (max z cull size=2048*1536)\n");

                // construct the bind info
                ZCullBindInfo bindInfo(_offset, _width, _height, _zFormat, _aaFormat);

                // check if we try to bind two time the same surface
                if (findDuplicate(_offset))
                {
                    ITF_ASSERT_MSG(0, "Warning: Try to map a cellGcmSurface different z cull region index\n");
                    return InvalidRegionIndex;
                }

                // get a new z cull region index
                u32 zCullIndex = getUnusedZCullIndex();	
                if(zCullIndex == InvalidRegionIndex)
                {
                    return InvalidRegionIndex;
                }

                //bind the z cull to the surf
                int res = cellGcmBindZcull( zCullIndex,
                                            bindInfo.m_offset,
                                            GetAboveAlign(bindInfo.m_width, u32(ZCULL_WIDTH_ALIGMENT)),
                                            GetAboveAlign(bindInfo.m_height, u32(ZCULL_HEIGTH_ALIGMENT)),
                                            0, 
                                            bindInfo.m_zformat,
                                            bindInfo.m_aaformat, 
                                            CELL_GCM_ZCULL_LESS, 
                                            CELL_GCM_ZCULL_LONES,
                                            CELL_GCM_SCULL_SFUNC_LESS,
                                            0x80,
                                            0xff);
                if (res != CELL_OK)
                {
                    ITF_ASSERT_MSG(0, "Warning: Failed to map ZCull region\n");
                    return InvalidRegionIndex;
                }

                m_zbindInfos[zCullIndex] = bindInfo;
                return zCullIndex;
            }

            void ZCullMemoryManager::unBindRegion(u32 _index)
            {
                ITF_ASSERT_MSG(_index < ZCULL_MAX_INDEX, "Warning: Try to unbind a z cull region with an invalid index");
                // check if the index is not already freed
                if (m_zbindInfos[_index].m_offset == 0)
                {
                    ITF_ASSERT_MSG(0, "Warning: Try to unbind a z cull region that as not been bound");
                    return;
                }

                m_zbindInfos[_index].reset();
                cellGcmUnbindZcull(_index);
            }

            u32 ZCullMemoryManager::getUnusedZCullIndex()
            {
                for(u32 i = 0; i < ZCULL_MAX_INDEX; i++)
                {
                    if (m_zbindInfos[i].m_offset == 0)
                    {
                        return i;
                    }
                }
                return InvalidRegionIndex;
            }

        } // namespace VRAMAllocator
    }
}
