#ifndef _ITF_LOGICDATABASE_H_
#define _ITF_LOGICDATABASE_H_

#ifdef ITF_SUPPORT_LOGICDATABASE

#ifndef _ITF_LOGICDATAPROPERTY_H_
#include "engine/logicdatabase/LogicDataProperty.h"
#endif //_ITF_LOGICDATAPROPERTY_H_

#ifndef _ITF_LOGICDATATABLE_H_
#include "engine/logicdatabase/LogicDataTable.h"
#endif //_ITF_LOGICDATATABLE_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

namespace ITF
{

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class LogicDataTable;
class LogicDataProperty;
class LogicDataLink;
class DepCollection;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct LogicDataTableDependenciesCollector 
{
    LogicDataTableDependenciesCollector() {};
    ~LogicDataTableDependenciesCollector();

    ITF_MAP<String,String>                              m_strValues;
    ITF_MAP<String,f32>                                 m_numValues;
    ITF_VECTOR<LogicDataTableDependenciesCollector*>    m_childDep;
};


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//for a given logic file ,record all its dependencies
class LogicDataTableDependencies
{
public:
    struct Dependency
    {
        String_Uncached m_Filename;      
        u64             m_flushTime;

        void serialize(ArchiveMemory& _Archive)
        {
            m_Filename.serialize(_Archive);
            _Archive.serialize(m_flushTime);
        }
    };

    bbool isUptoDate();
    void addDependency(const String& _name);
    bbool hasDependency(const String& _name);

    void serialize(ArchiveMemory& _Archive);

protected:
    Vector<Dependency> m_Dependencies;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct CompilLogicDataInfo
{
    CompilLogicDataInfo():nodecacheTable(NULL),dependency(NULL) {};

    LogicDataTable* nodecacheTable;
    LogicDataTableDependencies* dependency;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class LogicDataBase :public TemplateSingleton<LogicDataBase>
{
public:
    typedef u32 keyID;

    LogicDataBase();
    virtual ~LogicDataBase() {destroy();}
    void init(const String& _platform,bbool _bFinalMode,bbool _bUseDependencies=btrue);

    void destroy();

    bbool cookFile(const String& _relativePath,const String& _platform);

    LogicDataLink* createFile(const String& _relativePath,const String& _platform = "PC",bbool _iscooking = bfalse);

#ifdef ITF_SUPPORT_LUA
    LogicDataLink* createFileByBuffer(const String& _relativePath,const char * readonlyBuffer);
#else
    LogicDataLink* createFileByBuffer(const String& _relativePath,const char * readonlyBuffer) {return NULL;};
#endif //ITF_SUPPORT_LUA

    LogicDataLink* readFromBuffer(ArchiveMemory& _archive);
    void saveIdToFile(const String& _relativePath,const String& _platform,LogicDataTable* _nodecachetable,ArchiveMemory& aw, bbool _doNotSave = bfalse);
    LogicDataLink* loadIdFromFile(const String& _relativePath,const String& _platform);

    LogicDataTable* getTableFromId(const String& _refname,const String& _platform);
    void flushId(const String& _refname);
    void setRemote(bbool _remote) {m_bRemote = _remote;}
    
    bbool needRebuildId(const String& _refname);
    int includeReference(lua_State *L);

    void setBigEndian(bbool _bigEndian)    {m_bigEndian = _bigEndian;}

    u32  addKeyString(const String8& _key);
    const char* findKey(keyID _key);

    void clear();

    void saveDependencies(const String& _platform);
    void loadDependencies(const String& _platform);

    void disableSaving()    {m_disableSaving = btrue;}

    ITF_INLINE const ITF_MAP<String_Uncached,LogicDataTableDependencies*>& getDependenciesTables() const { return m_DependenciesTables; }
    void serializeStringTable(bbool _bReading,bbool _bDelete,const String& _szPlatform);

    const String& getCurrentRefName()const  { return m_currentRefName; }

    LogicDataLink* loadIdFromFileInternal(const String& _relativePath,const String& _platform);

	void setRaiseError(bbool _raiseError)	{m_bRaiseError = _raiseError;}

    void enableBinarisation()                       {m_enableBinarisation = btrue;}
    const bbool isBinarisationEnable()     const    {return m_enableBinarisation;}

    char* loadActBuffer(const String& _relativePath);
    CompilLogicDataInfo compileBuffer(const String& _vPlatform,const String& _relativePath,char* readBuffer,LogicDataTableDependenciesCollector* _CollectorDependencies=NULL);

#ifdef ITF_WINDOWS
    void dependenciesFile(const String& _vPlatform,const String& file,DepCollection& _depCollection);
#endif //ITF_WINDOWS

protected:
    bbool                                       m_disableSaving;
    bbool                                       m_initDone;
    bbool                                       m_bigEndian;
    bbool                                       m_bRemote;
    bbool										m_bModeFinal;//step,cooking is not allowed on this mode
    bbool										m_bRaiseError;
    bbool                                       m_enableBinarisation;
    LUAHandler*                                 m_luaHandler;

    String                                      m_currentRefName;

    ITF_THREAD_CRITICAL_SECTION                 m_cs;
    ITF_THREAD_CRITICAL_SECTION                 m_csAccess;

    ITF_MAP<keyID,char*>                        m_keyToString;

    ITF_MAP<String_Uncached,LogicDataTableDependencies*> m_DependenciesTables;
    ITF_MAP<String_Uncached,LogicDataLink*>              m_LogicDataTables;

};

#define LOGICDATABASE LogicDataBase::getptr()
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF

#endif // _ITF_LOGICDATABASE_H_
#endif //ITF_SUPPORT_LOGICDATABASE
