#ifndef _ITF_LOGICDATAUTILS_H_
#define _ITF_LOGICDATAUTILS_H_

#ifdef ITF_SUPPORT_LOGICDATABASE

namespace ITF
{
class LogicDataTable;
class LogicDataProperty;


struct LogicDataMemoryShared
{
    LogicDataMemoryShared():m_countSafeArray(0),m_countProperties(0),m_countElementArray(0),m_countLogicTable(0),m_countLogicDataProperty(0),

        m_stringLenAllocation(0),
        m_positionInSafeArrayMemory(0),m_positionInPropertiesMemory(0),m_positionInLogicTable(0),m_positionLogicDataProperty(0),
        m_positionLinearString(0),m_positionVect2D(0),m_positionDataNumber(0),m_positionVect3D(0),m_positionInElementArray(0),m_linearElementArray(0),
        m_linearCharArrayMemory(NULL),m_linearPropertyArrayMemory(NULL),m_linearSafeArrayMemory(NULL),m_linearLogicTable(NULL),
        m_linearString(NULL),m_linearLogicDataProperty(NULL)
    {

        ITF_MemSet(m_linearMemory,0,sizeof(m_linearMemory));

    };

    void destroy();

    u32 m_countSafeArray;
    u32 m_countProperties;
    u32 m_countLogicTable;
    u32 m_countLogicDataProperty;
    u32 m_stringLenAllocation;//in U16 len
    u32 m_countElementArray;

    u32 m_positionDataNumber;
    u32 m_positionInElementArray;
    u32 m_positionInSafeArrayMemory;
    u32 m_positionInPropertiesMemory;
    u32 m_positionInLogicTable;
    u32 m_positionLogicDataProperty;
    u32 m_positionLinearString;
    u32 m_positionVect2D;
    u32 m_positionVect3D;

    SafeArray<LogicDataProperty*>*  m_linearSafeArrayMemory;
    LogicDataTable*                 m_linearLogicTable;
    char**                          m_linearCharArrayMemory;
    u16*                            m_linearString;

    u8*                             m_linearPropertyArrayMemory;
    LogicDataProperty*              m_linearLogicDataProperty;
    UINT_PTR*                       m_linearElementArray;
    u8*                             m_linearMemory[eLogicDataTypeMax];

    ArchiveMemory                   m_archive[eLogicDataTypeMax];

    void serialize(ArchiveMemory& _archive);
};

class LogicDataLink
{
public:
    virtual ~LogicDataLink();
    LogicDataTable* m_pLogicDataTable;
    LogicDataMemoryShared m_MemoryShared;


 };

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF

#endif // _ITF_LOGICUTILSs_H_
#endif //ITF_SUPPORT_LOGICDATABASE
