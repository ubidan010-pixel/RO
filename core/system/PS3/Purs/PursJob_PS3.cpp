#include "precompiled_core.h"

#include "PursJob_PS3.h"

namespace ITF
{

namespace Purs
{
    class SpuBinInfos
    {
    public:
        Purs::JobCode *       m_jobCode;
        const Purs::JobCode   m_origJobCode;

        SpuBinInfos(Purs::JobCode * jobCode)
        : m_jobCode(jobCode),
          m_origJobCode((char*) jobCode->m_eaStart, jobCode->m_size)
        {
        }

        void UpdateCode(char* _pBinStart, u32 _binSize)
        {
            // Release former job binary because it has been allocated in the heap
            if (m_jobCode->m_eaStart != m_origJobCode.m_eaStart)
            {
                Memory::alignedFree((char*) m_jobCode->m_eaStart);
            }

            // Update code
            m_jobCode->UpdateCode(_pBinStart, _binSize);
            LOG("Update code. New value = 0x%llx\n", m_jobCode->m_eaStart);
        }

        typedef Map<u32, SpuBinInfos> SpuJobCodeMap;
        static SpuJobCodeMap * ms_crcToJobCode;
    };


    SpuBinInfos::SpuJobCodeMap * SpuBinInfos::ms_crcToJobCode = NULL;

    #define SPU_BIN_REGISTER_TRACE( exp, ... )    LOG( exp, ##__VA_ARGS__ )

    void RegisterJobCode(const char * jobName, Purs::JobCode * jobCode)
    {
        // Hash map has to be allocated on the head. Otherwise, it would create a race condition in the globals
        if (SpuBinInfos::ms_crcToJobCode == NULL)
        {
            SpuBinInfos::ms_crcToJobCode = new SpuBinInfos::SpuJobCodeMap();
        }

        String8 lowerCaseJobName = jobName;
        lowerCaseJobName.toLower();

        // Create a "CRC to SpuBinInfos" map entry and insert it
        u32 jobNameCrc = CRC::compute32((unsigned char *)lowerCaseJobName.cStr(), lowerCaseJobName.getLen());
        SpuBinInfos jobCodeInfos(jobCode);

        SPU_BIN_REGISTER_TRACE("RegisterJobCode: jobCode = 0x%08X, name = %s, crc = 0x%08X\n", u32(jobCode), jobName, jobNameCrc);

        SpuBinInfos::SpuJobCodeMap::value_type jobCodeHashEntry(jobNameCrc, jobCodeInfos);

        SpuBinInfos::ms_crcToJobCode->insert(jobCodeHashEntry);
    }

    void UpdateJobCode(u32 spuBinCrc, char * spuBin, int newBinSize)
    {
        // If crc is found in map, update the spu binary with the new infos
        SpuBinInfos::SpuJobCodeMap::iterator itJobCode = SpuBinInfos::ms_crcToJobCode->find(spuBinCrc);

        if (itJobCode != SpuBinInfos::ms_crcToJobCode->end())
        {
            SpuBinInfos & jobCodeInfos = itJobCode->second;
            jobCodeInfos.UpdateCode(spuBin, newBinSize);
        }
    }
}
}
