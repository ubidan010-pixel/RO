#ifndef _ITF_LOGICDATATABLE_H_
#define _ITF_LOGICDATATABLE_H_
namespace ITF
{


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class LogicDataTable;
class LogicDataProperty;
class LogicDataBase;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class LogicDataTable : public LogicDataProperty
{
public:

    typedef ITF_MAP<String8,LogicDataProperty*> mapLogicDataProperty;
    //for writing ,the map must removed!
    LogicDataTable():m_Count(0),m_Array(NULL),m_PropertiesCount(0),m_ArrayLogicDataRead(NULL) 
    {
        type = eLogicDataTable;m_Properties = NULL;m_PropertiesKey=NULL;

#ifndef ITF_FINAL
        m_TimeToLoad =  0.0f;
#endif //!ITF_FINAL
    }
    LogicDataTable(u32):m_Count(0),m_Array(NULL),m_PropertiesCount(0),m_ArrayLogicDataRead(NULL)
    {
        type = eLogicDataTable;m_Properties = NULL;m_PropertiesKey=NULL;

#ifndef ITF_FINAL
        m_TimeToLoad =  0.0f;
#endif //!ITF_FINAL
    };

    virtual void destroy();

    ITF_INLINE void allocateProperties() {if (!m_Properties) m_Properties = newAlloc(mId_LogicDataBase , mapLogicDataProperty());}
    void setCount(u32 _count);
    void shrink();
    void setIndex(u32 _index,LogicDataProperty* _property);
    LogicDataProperty * add(const String8& _name,const String8& _value);
    LogicDataProperty * add(const String8& _name,const f32& _value);
    LogicDataProperty * add(const String8& _name,LogicDataTable* _value);
   
    template<class  T> LogicDataProperty * add(eLogicDataType _type,const String8& _name,const T& _value)
    {
        String8 lowerkey = _name;
        lowerkey.toLower();
        LogicDataProperty * dataProperty = newAlloc(mId_LogicDataBase ,LogicDataProperty(_type,_value));
        allocateProperties();
        (*m_Properties)[lowerkey] = dataProperty;
        return dataProperty;
    }

    u8      getU8(const char* _name,u8 default_value) const;
    f32     getF32(const char* _name,f32 default_value) const;
    u32     getU32(const char* _name,u32 default_value) const;
    i32     getI32(const char* _name,i32 default_value) const;

    void    getString(const char* _name,String& _retvalue) const;
    void    getVec2d(const char* _name,Vec2d& _retvalue) const;
    void    getVec3d(const char* _name,Vec3d& _retvalue) const;
    void    getColor(const char* _name,Color& _color) const;
    const   LogicDataTable* getTable(const char* _name) const;
    const   LogicDataTable* getTable(const u32 _index) const;

    const u32 getSize()  const {return m_Properties->size();}
    const u32 getCount() const {return m_Count;}

    const bbool getIsEmpty() const { return m_PropertiesCount == 0; }
    
    const LogicDataProperty* getIndexArrayRead(u32 _index) const {  return (LogicDataProperty *)(m_ArrayLogicDataRead[_index]);}

    void serializeTableWrite(LogicDataBase* _pDataBaseServer,ArchiveMemory& am);
    bbool serializeTableRead(LogicDataBase* _pDataBaseServer,ArchiveMemory& am,LogicDataMemoryShared& _memoryShared);
    void serializeTableMemoryTracking(LogicDataBase* _pDataBaseServer,LogicDataMemoryShared& _memoryShared);

#ifndef ITF_FINAL
    void setTimeToLoad(f32 _timeToLoad) {m_TimeToLoad = _timeToLoad;}
#endif //!ITF_FINAL
 
protected:

#ifndef ITF_FINAL
    f32 m_TimeToLoad;
#endif //!ITF_FINAL

    u32 m_Count;

    const char**           m_PropertiesKey;
    LogicDataProperty**    m_PropertiesValue;
    u32                    m_PropertiesCount;

    mapLogicDataProperty*  m_Properties;
    SafeArray<LogicDataProperty*>*       m_Array;


    UINT_PTR*                       m_ArrayLogicDataRead;//SHARED MEMORY DON'T DELETE DIRECTLY


};
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF

#endif // _ITF_LOGICDATATABLE_H_
