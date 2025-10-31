#include "precompiled_GFXAdapter_GCM.h"

#include "Report_GCM.h"

namespace ITF
{
namespace GCM
{

    ReportManager::ReportManager(u32 _nbMaxReport, u32 _rsxLocation)
        : m_nbIndices(_nbMaxReport)
        , m_rsxLocation(_rsxLocation)
    {
        ITF_ASSERT(_rsxLocation != CELL_GCM_LOCATION_LOCAL || _nbMaxReport<=2048);
        ITF_ASSERT(_rsxLocation == CELL_GCM_LOCATION_LOCAL || _rsxLocation == CELL_GCM_LOCATION_MAIN);
        for(u32 i = _nbMaxReport; i!=0; --i)
        {
            m_freeIndices.push_back(i-1u);
        }
        if (_rsxLocation == CELL_GCM_LOCATION_MAIN) // main memory -> we need to reserve it
        {
            m_vramMainReportArea.MainReportAlloc(_nbMaxReport * sizeof(CellGcmReportData));
        }
    }

    u32 ReportManager::allocIndex()
    {
        if (m_freeIndices.empty())
        {
            ITF_ASSERT(0);
            return InvalidReportIndex;
        }
        u32 newIdx = m_freeIndices.back();
        m_freeIndices.pop_back();
        return newIdx;
    }

    void ReportManager::freeIndex(u32 _index)
    {
        ITF_ASSERT(_index<=m_nbIndices);
        // check it's not yet released
        ITF_ASSERT(m_freeIndices.end() == std::find(m_freeIndices.begin(), m_freeIndices.end(), _index));
        m_freeIndices.push_back(_index);
    }

    CellGcmReportData * ReportManager::getAddressFromIndex(u32 _index) const
    {
        return cellGcmGetReportDataAddressLocation(_index, m_rsxLocation);
    }

    void Report::init()
    {
        ITF_ASSERT(!isValid());
        ITF_ASSERT(reportAddr == NULL);
        reportIdx = ReportManager::get().allocIndex();
        if (reportIdx!=ReportManager::InvalidReportIndex)
        {
            reportAddr = ReportManager::get().getAddressFromIndex(reportIdx);
            memset(reportAddr, 0, sizeof(*reportAddr));
        }
    }

    void Report::release()
    {
        if (isValid())
        {
            ReportManager::get().freeIndex(reportIdx);
            reportIdx = ReportManager::InvalidReportIndex;
        }
    }




} // GCM
} // ITF
