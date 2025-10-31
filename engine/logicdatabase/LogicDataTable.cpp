#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_LOGICDATABASE

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_LOGICDATAUTILS_H_
#include "engine/logicdatabase/logicdatautils.h"
#endif //_ITF_LOGICDATAUTILS_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

namespace ITF
{
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataProperty * LogicDataTable::add(const String8& _name,const String8& _value)
{
    String8 lowerkey = _name;
    lowerkey.toLower();
    LogicDataProperty * dataProperty = newAlloc(mId_LogicDataBase ,LogicDataProperty(_value));
    allocateProperties();
    (*m_Properties)[lowerkey] = dataProperty;
    return dataProperty;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataProperty *  LogicDataTable::add(const String8& _name,const f32& _value)
{
    String8 lowerkey = _name;
    lowerkey.toLower();
    LogicDataProperty * dataProperty =  newAlloc(mId_LogicDataBase , LogicDataProperty(_value));
    allocateProperties();
    (*m_Properties)[lowerkey] = dataProperty;
    return dataProperty;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataProperty *  LogicDataTable::add(const String8& _name,LogicDataTable* _value)
{
    String8 lowerkey = _name;
    lowerkey.toLower();
    allocateProperties();
    (*m_Properties)[lowerkey] = _value;//new  LogicDataProperty(_value);
    return _value;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

u8 LogicDataTable::getU8(const char*  _name,u8 default_value) const
{
    if (!m_PropertiesKey)
        return 0;

    for (u32 index = 0;index < m_PropertiesCount;++index )
    {
        if (m_PropertiesKey[index] && _strcmpi(_name,m_PropertiesKey[index]) == 0)
        {
            const LogicDataProperty* properties = m_PropertiesValue[index];

            if (properties->type != eLogicDataNumber)
                return default_value;

            return (u8)static_cast<i32>(properties->fData);
            //old return static_cast<u8>(properties->fData);
        }
    }

    return default_value;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

u32  LogicDataTable::getU32(const char*  _name,u32 default_value) const
{
    if (!m_PropertiesKey)
        return 0;

    for (u32 index = 0;index < m_PropertiesCount;++index )
    {
        if (m_PropertiesKey[index] && _strcmpi(_name,m_PropertiesKey[index]) == 0)
        {
            const LogicDataProperty* properties = m_PropertiesValue[index];

            if (properties->type != eLogicDataNumber)
                return default_value;
            
            return (u32)static_cast<i32>(properties->fData);
            //old return static_cast<u32>(properties->fData);
        }
    }

    return default_value;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

i32  LogicDataTable::getI32(const char*  _name,i32 default_value) const
{
    if (!m_PropertiesKey)
        return 0;

    for (u32 index = 0;index < m_PropertiesCount;++index )
    {
        if (m_PropertiesKey[index] && _strcmpi(_name,m_PropertiesKey[index]) == 0)
        {
            const LogicDataProperty* properties = m_PropertiesValue[index];

            if (properties->type != eLogicDataNumber)
                return default_value;

            return static_cast<i32>(properties->fData);
        }
    }

    return default_value;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

f32 LogicDataTable::getF32(const char* _name,f32 default_value) const
{
    if (!m_PropertiesKey)
        return 0.0f;

    for (u32 index = 0;index < m_PropertiesCount;++index )
    {
        if (m_PropertiesKey[index] && _strcmpi(_name,m_PropertiesKey[index]) == 0)
        {
            const LogicDataProperty* properties = m_PropertiesValue[index];

            if (properties->type != eLogicDataNumber)
                return default_value;

            return static_cast<f32>(properties->fData);
        }
    }

    return default_value;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::getVec2d(const char* _name,Vec2d& _retvalue) const
{
    if (!m_PropertiesKey)
        return;

    
    for (u32 index = 0;index < m_PropertiesCount;++index )
    {
        if (m_PropertiesKey[index] && _strcmpi(_name,m_PropertiesKey[index]) == 0)
        {
            const LogicDataProperty* properties = m_PropertiesValue[index];

            if (properties->type != eLogicDataVector2d)
                return;

            _retvalue =  (*(Vec2d*)(properties->binary.pBuffer));
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::getVec3d(const char* _name,Vec3d& _retvalue) const
{

    if (!m_PropertiesKey)
        return;

    for (u32 index = 0;index < m_PropertiesCount;++index )
    {
        if (m_PropertiesKey[index] && _strcmpi(_name,m_PropertiesKey[index]) == 0)
        {
            const LogicDataProperty* properties = m_PropertiesValue[index];

            if (properties->type == eLogicDataVector2d)
            {
                Vec2d vec2d = (*(Vec2d*)(properties->binary.pBuffer));
                _retvalue =  vec2d.to3d();
            }
            else
            {
                if (properties->type != eLogicDataVector3d)
                    return;

                _retvalue =  (*(Vec3d*)(properties->binary.pBuffer));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::getColor(const char* _name,Color& _color) const
{
    String colorS;

    getString(_name,colorS);

    if ( !colorS.isEmpty() )
    {
        u32 colorN;

        sscanf(StringConverter(colorS).getChar(),"0x%08x",&colorN);

        _color = Color(ITFCOLOR_FROM_ARGB(colorN));
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::getString(const char* _name,String& _retvalue) const
{
    if (!m_PropertiesKey)
        return;

    for (u32 index = 0;index < m_PropertiesCount;++index )
    {
        if (m_PropertiesKey[index] && _strcmpi(_name,m_PropertiesKey[index]) == 0)
        {
            const LogicDataProperty* properties = m_PropertiesValue[index];

            if (properties->type != eLogicDataString)
                return;

            _retvalue.setText((u16*)properties->string.pwszData);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const LogicDataTable* LogicDataTable::getTable(const char* _name) const
{
    if (!m_PropertiesKey)
        return NULL;

    for (u32 index = 0;index < m_PropertiesCount;++index )
    {
        if (m_PropertiesKey[index] && _strcmpi(_name,m_PropertiesKey[index]) == 0)
        {
            const LogicDataProperty* properties = m_PropertiesValue[index];

            if (properties->type != eLogicDataTable)
                return NULL;

            return static_cast<const LogicDataTable*>(properties);
        }
    }
  
    return NULL;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::setCount(u32 _count) 
{
    m_Count = _count;
    if (_count)
    {
        m_Array = newAlloc(mId_LogicDataBase , SafeArray<LogicDataProperty*>());
        m_Array->resize(_count);

        for (u32 index = 0;index<_count;index++)
            (*m_Array)[index] = NULL;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void LogicDataTable::shrink()
{
    if (m_Array)
    {
        for (u32 index=0;index<(*m_Array).size();)
        {
            if ((*m_Array)[index] == NULL)
            {
                (*m_Array).eraseNoOrder(index);
            }
            else
            {
                LogicDataProperty* pProperties = (*m_Array)[index];
                pProperties->setIndex(index);
                index++;
            }
        }

        m_Count = (*m_Array).size();
    }


}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::setIndex(u32 _index,LogicDataProperty* _property)
{
    ITF_ASSERT(m_Array);
    if (m_Array)
    {
        (*m_Array)[_index] = _property;
        _property->setIndex(_index);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::serializeTableMemoryTracking(LogicDataBase* _pDataBaseServer,LogicDataMemoryShared& _memoryShared)
{
    if (m_Properties)
    {
        mapLogicDataProperty::const_iterator iter =  m_Properties->begin();
        for (;iter!=m_Properties->end();++iter)
        {
            LogicDataProperty* pProperty = (*iter).second;
            u32 type = pProperty->getType();

            if (type == eLogicDataTable)
            {
                _memoryShared.m_countLogicTable++;
                ((LogicDataTable*)(pProperty))->serializeTableMemoryTracking(_pDataBaseServer,_memoryShared);
            }
            else
            {
                _memoryShared.m_countLogicDataProperty++;
                pProperty->serializeTracking(_memoryShared);
            }
        }


        _memoryShared.m_countProperties += m_Properties->size();
    }
    
    if (m_Count)
    {
        _memoryShared.m_countSafeArray++;
        _memoryShared.m_countElementArray += m_Count;
    }

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::serializeTableWrite(LogicDataBase* _pDataBaseServer,ArchiveMemory& am)
{
    am.serialize(m_Count);

    //save this table
    u32 size = m_Properties ? m_Properties->size() : 0;
    am.serialize(size);

    if (m_Properties)
    {
        mapLogicDataProperty::const_iterator iter =  m_Properties->begin();
        for (;iter!=m_Properties->end();++iter)
        {
            //save the key
            const String8& key = (*iter).first;
            //key.serialize(am);

            LogicDataBase::keyID keyid = _pDataBaseServer->addKeyString(key);
            am.serialize(keyid);

            //save the value
            LogicDataProperty* pProperty = (*iter).second;
            u32 type = pProperty->getType();

            u32 indexArray = pProperty->getIndexArray();
            am.serialize(type);
            am.serialize(indexArray);

            if (type == eLogicDataTable)
            {
                ((LogicDataTable*)(pProperty))->serializeTableWrite(_pDataBaseServer,am);
            }
            else
            {
                pProperty->serializeWrite(am);
            }
    }
    }
   
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool LogicDataTable::serializeTableRead(LogicDataBase* _pDataBaseServer,ArchiveMemory& am,LogicDataMemoryShared& _memoryShared)
{
    am.serialize(m_Count);

    u32 size;
    am.serialize(size);

    if (m_Count)
    {
        _memoryShared.m_positionInSafeArrayMemory++;

        m_ArrayLogicDataRead = (_memoryShared.m_linearElementArray+_memoryShared.m_positionInElementArray);
        _memoryShared.m_positionInElementArray += m_Count;
    }

    m_PropertiesValue  = new (_memoryShared.m_linearPropertyArrayMemory +_memoryShared.m_positionInPropertiesMemory*sizeof(LogicDataProperty)) LogicDataProperty*[size];
    m_PropertiesKey    = new (_memoryShared.m_linearCharArrayMemory     +_memoryShared.m_positionInPropertiesMemory) const char*[size];
    
    _memoryShared.m_positionInPropertiesMemory+=size;

    m_PropertiesCount  = size;

    LogicDataBase::keyID keyid;
    u32 indexArray;
    eLogicDataType type;

    for (u32 index = 0; index<size ;++index)            
    {
        am.serialize(keyid);
        am.serialize((u32&)type);
        am.serialize(indexArray);

        const char* keyPtr = _pDataBaseServer->findKey(keyid);
        if (!keyPtr)//propagate the information to the top
            return bfalse;
 
        LogicDataProperty* pLogicDataProperty = NULL;

        switch (type)
        {
            case eLogicDataTable:
                {
                    LogicDataTable* pLogicDataTable = new (_memoryShared.m_linearLogicTable+_memoryShared.m_positionInLogicTable) LogicDataTable(0); 
                    ++_memoryShared.m_positionInLogicTable;
                    pLogicDataTable->serializeTableRead(_pDataBaseServer,am,_memoryShared);
                    pLogicDataProperty = pLogicDataTable;
                }
                break;

            default:
                pLogicDataProperty = new (_memoryShared.m_linearLogicDataProperty+_memoryShared.m_positionLogicDataProperty) LogicDataProperty(type); 
                ++_memoryShared.m_positionLogicDataProperty;
                pLogicDataProperty->serializeRead(am,_memoryShared);
                break;

        }

        if (indexArray != U32_INVALID)
        {
            (m_ArrayLogicDataRead[indexArray]) =(UINT_PTR) pLogicDataProperty;
        }

        m_PropertiesValue[index] = pLogicDataProperty;
        m_PropertiesKey[index]   = keyPtr;
    }

    return btrue;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTable::destroy()
{
    if (m_Properties)
    {
        mapLogicDataProperty::iterator iter =  m_Properties->begin();

        for (;iter!=m_Properties->end();++iter)
        {
            (*iter).second->destroy();
            SF_DEL((*iter).second);
        }
    }


    for (u32 index = 0;index <m_PropertiesCount;++index)
    {
        SF_DEL(m_PropertiesValue[index]);
        SF_DEL_ARRAY(m_PropertiesKey[index]);
    }

    SF_DEL(m_Array);

    SF_DEL(m_Properties);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF


#endif  //ITF_SUPPORT_LOGICDATABASE
