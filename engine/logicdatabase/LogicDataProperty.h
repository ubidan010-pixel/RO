#ifndef _ITF_LOGICDATAPROPERTY_H_
#define _ITF_LOGICDATAPROPERTY_H_
namespace ITF
{

enum eLogicDataType
{
    eLogicDataNumber    = 0, 
    eLogicDataString    = 1, 
    eLogicDataTable     = 2,
    eLogicDataVector2d  = 3,
    eLogicDataVector3d  = 4,
    eLogicDataTypeMax  ,
    ENUM_FORCE_SIZE_32(eLogicDataType)
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class LogicDataProperty;
class LogicDataTable;
struct LogicDataMemoryShared;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class LogicDataProperty
{
public:
    LogicDataProperty() {m_indexArray = U32_INVALID;type = eLogicDataNumber;};
    LogicDataProperty(eLogicDataType _type):type(_type){m_indexArray = U32_INVALID;};
    virtual void destroy();

    void serializeWrite(ArchiveMemory& _Archive);
    void serializeRead(ArchiveMemory& _Archive,LogicDataMemoryShared& _memoryShared);

    void serializeTracking(LogicDataMemoryShared& _memoryShared);

    LogicDataProperty(const String& _value)           
    {
        m_indexArray = U32_INVALID;
        u32 len = _value.getLen();
        string.pwszData = newAlloc(mId_LogicDataBase,u16[len+1]);
        ITF_Memcpy(string.pwszData,_value.cStr(),len*sizeof(u16));
        string.pwszData[len] = 0;
        string.cbData = len;
        type = eLogicDataString;
    };

    LogicDataProperty(const f32&    _value)             {m_indexArray = U32_INVALID;fData = _value;            type = eLogicDataNumber;}
 
    template<class T>  LogicDataProperty(eLogicDataType _type,const T&  _value)   
    {
        m_indexArray = U32_INVALID;
        type = _type;
        binary.size = sizeof(T);
        binary.pBuffer =  newAlloc(mId_LogicDataBase, T());        
        (*(T*)(binary.pBuffer)) = _value;
    }

    const ITF_INLINE eLogicDataType getType()   {return type;}
    const u32 getIndexArray()                   {return m_indexArray;}
    void  setIndex(u32 _index)                  {m_indexArray = _index;}

protected:

    friend class LogicDataTable;
    eLogicDataType type;
    u32 m_indexArray;

    union {

        struct  
        {
            u32 size;
            void* pBuffer;
        } binary;

        f32 fData;

        struct {
            u32 cbData;
            u16* pwszData;
        }string;

    };
};


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF

#endif // _ITF_LOGICDATAPROPERTY_H_
