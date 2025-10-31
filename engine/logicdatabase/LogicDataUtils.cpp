#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_LOGICDATABASE

#ifndef _ITF_LOGICDATAPROPERTY_H_
#include "engine/logicdatabase/LogicDataProperty.h"
#endif //_ITF_LOGICDATAPROPERTY_H_

#ifndef _ITF_LOGICDATATABLE_H_
#include "engine/logicdatabase/LogicDataTable.h"
#endif //_ITF_LOGICDATATABLE_H_

#ifndef _ITF_LOGICDATAUTILS_H_
#include "engine/logicdatabase/LogicDataUtils.h"
#endif //_ITF_LOGICDATAUTILS_H_


namespace ITF
{
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataLink::~LogicDataLink()
{
   m_MemoryShared.destroy(); 
   SF_DEL(m_pLogicDataTable);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataMemoryShared::serialize(ArchiveMemory& _archive)
{
    _archive.serialize(m_countSafeArray);
    _archive.serialize(m_countProperties);
    _archive.serialize(m_countLogicTable);
    _archive.serialize(m_countLogicDataProperty);
    _archive.serialize(m_countElementArray);
    _archive.serialize(m_stringLenAllocation);


    if (_archive.isReading())
    {
        m_linearPropertyArrayMemory     = newAlloc(mId_LogicDataBase, u8[m_countProperties*sizeof(LogicDataProperty)]);
        m_linearCharArrayMemory         = newAlloc(mId_LogicDataBase, char*[m_countProperties]);

        m_linearLogicDataProperty       = newAlloc(mId_LogicDataBase, LogicDataProperty[m_countLogicDataProperty]);
        m_linearLogicTable              = newAlloc(mId_LogicDataBase, LogicDataTable[m_countLogicTable]);
        m_linearString                  = newAlloc(mId_LogicDataBase, u16[m_stringLenAllocation]);
        m_linearElementArray            = newAlloc(mId_LogicDataBase, UINT_PTR[m_countElementArray]);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataMemoryShared::destroy()
{
    SF_DEL_ARRAY(m_linearPropertyArrayMemory);

    SF_DEL_ARRAY(m_linearSafeArrayMemory);
    SF_DEL_ARRAY(m_linearLogicTable);
    SF_DEL_ARRAY(m_linearCharArrayMemory);
    SF_DEL_ARRAY(m_linearString);


    SF_DEL_ARRAY(m_linearLogicDataProperty);

    SF_DEL_ARRAY(m_linearElementArray);

    for (u32 index=0;index<eLogicDataTypeMax;++index)
        SF_DEL_ARRAY(m_linearMemory[index]);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF

#endif //ITF_SUPPORT_LOGICDATABASE

