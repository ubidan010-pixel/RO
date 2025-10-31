#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_LOGICDATABASE

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_LOGICDATAUTILS_H_
#include "engine/logicdatabase/logicdatautils.h"
#endif //_ITF_LOGICDATAUTILS_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#ifndef _ITF_FILECACHEMANAGER_H_
#include "core/AdaptersInterfaces/FileCacheManager.h"
#endif //_ITF_FILECACHEMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

namespace ITF
{

    LogicDataTableDependenciesCollector::~LogicDataTableDependenciesCollector()
    {
        for ( u32 i = 0; i < m_childDep.size(); ++i )
            delete m_childDep[i];
    }

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void LogicDataBase::clear()
    {
        ITF_MAP<String_Uncached,LogicDataLink*>::iterator iter = m_LogicDataTables.begin();

        for (;iter!=m_LogicDataTables.end();++iter)
        {
            SF_DEL((*iter).second);
        }


        m_LogicDataTables.clear();


        ITF_MAP<String_Uncached,LogicDataTableDependencies*>::iterator iterDepencies = m_DependenciesTables.begin();

        for (;iterDepencies!=m_DependenciesTables.end();++iterDepencies)
        {
            SF_DEL((*iterDepencies).second);
        }

        m_DependenciesTables.clear();
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTableDependencies::addDependency(const String& _name)
{
    Dependency dep;
    dep.m_Filename = FilePath::normalizePath(_name);
    dep.m_flushTime =  FILECACHEMANAGER->getLastTimeWriteAccess(_name);
    m_Dependencies.push_back(dep);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool LogicDataTableDependencies::hasDependency(const String& _name)
{
    for (Vector<Dependency>::const_iterator it = m_Dependencies.begin(); it != m_Dependencies.end(); ++it)
    {
        if (it->m_Filename == _name)
            return btrue;
    }
    return bfalse;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool LogicDataBase::cookFile(const String& _relativePath,const String& _platform)
{
    createFile(_relativePath,_platform,btrue);
#if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
    if (m_enableBinarisation)
        return TEMPLATEDATABASE->serializeClassTemplate(_relativePath,_platform);
#endif //#if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)

    return btrue;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

char* LogicDataBase::loadActBuffer(const String& _relativePath)
{
    File* fileLoading = FILECACHEMANAGER->openFile(_relativePath);
    if (!fileLoading)
        return NULL;
    
    u32 fileSize = (u32)fileLoading->getLength();
    char * readBuffer = newAlloc(mId_Temporary, char[fileSize+1]);
    u32 sizeRead = 0;
    fileLoading->read(readBuffer,fileSize,&sizeRead);
    ITF_ASSERT(fileSize == sizeRead);
    readBuffer[fileSize] = '\0';

    FILECACHEMANAGER->closeFile(fileLoading);
    fileLoading = NULL;
    
    return readBuffer;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//parse the file and fill the appropriate cache 
LogicDataLink* LogicDataBase::createFile(const String& _relativePath,const String& _platform, bbool _iscooking /*bfalse*/)
{
	if (m_bModeFinal)
		return NULL;

    if (m_bRemote)
    {
       bbool res = FILEMANAGER->remoteCook(_relativePath);
       ITF_ASSERT(res);

       if (res)
       {
           serializeStringTable(btrue,btrue,_platform);
           return loadIdFromFileInternal(_relativePath,_platform);
       }

       
    }
#ifdef ITF_SUPPORT_LUA

	//we have to cook the file...

    String keyname = _relativePath;
    keyname.toLower();
    
    //open the act file
    char * readBuffer = loadActBuffer(_relativePath);

    if (readBuffer)
    {
        ///////////////////////////////////////////////////////////////////////////
        CompilLogicDataInfo compilInfo = compileBuffer(_platform,_relativePath,readBuffer);
        //clean all previous

        ITF_MAP<String_Uncached,LogicDataTableDependencies*>::iterator iterDependency = m_DependenciesTables.find(keyname);
        if (iterDependency != m_DependenciesTables.end())
        {
            SF_DEL((*iterDependency).second);
            m_DependenciesTables.erase(iterDependency);
        }


        ITF_MAP<String_Uncached,LogicDataLink*>::iterator iterLogic = m_LogicDataTables.find(keyname);
        if (iterLogic != m_LogicDataTables.end())
        {
            SF_DEL((*iterLogic).second);
            m_LogicDataTables.erase(iterLogic);
        }

        bbool hasError = m_luaHandler->hasError();
        SF_DEL(m_luaHandler);

        {
            csAutoLock cs(m_cs);
            m_DependenciesTables[keyname]   = compilInfo.dependency;
        }

        //don t save in write bin mode
        //if (!m_enableBinarisation)
            saveDependencies(_platform);
        
        //An error occurs don't save anything
        if (hasError)
        {
            //delete the file must be cached
            String cookFile         = FILESERVER->getCachedNamePlatform(_relativePath,_platform);
            FILEMANAGER->deleteFile(cookFile);
            return NULL;
        }

        //each you reload a cache ,you save the new cache file..       
     //    if (!m_enableBinarisation)
        {
            ArchiveMemory aw;
            saveIdToFile(_relativePath,_platform,compilInfo.nodecacheTable,aw);
        }

        compilInfo.nodecacheTable->destroy();
        SF_DEL( compilInfo.nodecacheTable);

        //don t save in write bin mode
       // if (!m_enableBinarisation)
            serializeStringTable(bfalse,bfalse,_platform);

        m_currentRefName.clear();
        
        if (!_iscooking)
        {
            LogicDataLink* pLogicDataLink = loadIdFromFile(_relativePath,_platform);
            return pLogicDataLink;
        }
        
        return NULL;
    }

#endif //ITF_SUPPORT_LUA

    return NULL;
    
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataTableDependencies::serialize(ArchiveMemory& _Archive)
{
    m_Dependencies.serialize(_Archive);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool LogicDataTableDependencies::isUptoDate()
{
    for (Vector<Dependency>::iterator iterDependency = m_Dependencies.begin();iterDependency!=m_Dependencies.end();++iterDependency)
    {
        const Dependency& myDependency = *iterDependency;
        u64 currentFlushTime = FILECACHEMANAGER->getLastTimeWriteAccess(myDependency.m_Filename);
    
        //the file is different from the original one
        if (currentFlushTime != myDependency.m_flushTime)
        {
            return bfalse;
        }
        
    }

    return btrue;
}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool LogicDataBase::needRebuildId(const String& _refname)
{
#ifndef ITF_WINDOWS
    //we don t use any dependencies on console
    return bfalse;
#else

    //Look in the dependencies table
    String keyname = _refname;
    keyname.toLower();

    ITF_MAP<String_Uncached,LogicDataTableDependencies*>::const_iterator iterDependency = m_DependenciesTables.find(keyname);

    if (iterDependency != m_DependenciesTables.end())
    {
        return !((*iterDependency).second)->isUptoDate();
    }

    return bfalse;
#endif //ITF_WINDOWS
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataBase::saveIdToFile(const String& _relativePath,const String& _platform,LogicDataTable* _nodecachetable, ArchiveMemory& aw,bbool _doNotSave)
{
#ifndef ITF_WINDOWS
    ITF_ASSERT_MSG(0,"LogicDataBase::saveIdToFile is allowed only on win32 target");
#else

    if (m_disableSaving)
        return;

    if (_nodecachetable)
    {
        LogicDataMemoryShared memoryShared;

         _nodecachetable->serializeTableMemoryTracking(this,memoryShared);
        
        memoryShared.serialize(aw);

        //SWAP IT ?
        for (u32 index=0;index<eLogicDataTypeMax;index++)
        {
            u32 size = memoryShared.m_archive[index].getSize();
            aw.serialize(size);

            if (m_bigEndian)
            {
                u32* sourceData = (u32*) memoryShared.m_archive[index].getData();
                if (index == eLogicDataNumber || index == eLogicDataVector2d || index == eLogicDataVector3d) //float,vector2d,vector3d
                {
                    u32* current = (u32*) sourceData;
                    for (u32 i = 0;i<size/sizeof(u32);i++)
                    {
                        u32 val;
                        Endian::swapBigEndian(*current,(u8*)&val);
                        *current = val;
                        current++;
                    }
                }
            }

            aw.serializeBlock8(memoryShared.m_archive[index].getData(),size);
        }

        _nodecachetable->serializeTableWrite(this,aw);

        if (!_doNotSave)
        {
            String filename = FILESERVER->getCachedNamePlatform(_relativePath,_platform);

            File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
            if (pFile)
            {
                pFile->write(aw.getData(),aw.getSize());
                FILEMANAGER->closeFile(pFile);
                pFile = NULL;
            }
        }
    }
#endif 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataLink* LogicDataBase::readFromBuffer(ArchiveMemory& _archive)
{

    LogicDataTable* pLogicDataTableRead = newAlloc(mId_LogicDataBase,LogicDataTable);

    //allocate the SafeArray pool

    LogicDataLink* pLogicDataLink = newAlloc(mId_LogicDataBase,LogicDataLink);
    LogicDataMemoryShared& memoryShared = pLogicDataLink->m_MemoryShared;
    memoryShared.serialize(_archive);

    u32 size = 0;

    for (u32 index =0 ;index<eLogicDataTypeMax;index++)
    {
        _archive.serialize(size);
        u8* pMemory = newAlloc(mId_LogicDataBase,u8[size]);
        _archive.serializeBlock8(pMemory,size);

        //assign the linear memory
        memoryShared.m_linearMemory[index] = pMemory;
    }

    bbool bresult = pLogicDataTableRead->serializeTableRead(this,_archive,memoryShared);

    if (bresult)
    {
        ITF_ASSERT(memoryShared.m_countSafeArray == memoryShared.m_positionInSafeArrayMemory);
        ITF_ASSERT(memoryShared.m_countProperties == memoryShared.m_positionInPropertiesMemory);
        ITF_ASSERT(memoryShared.m_countElementArray == memoryShared.m_positionInElementArray);
    }
    else
    {
        if (SYSTEM_ADAPTER)
            ITF_FATAL_ERROR("String cache not valid,Full act recooking required");
    }

    pLogicDataLink->m_pLogicDataTable = pLogicDataTableRead;
    return pLogicDataLink;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataLink* LogicDataBase::loadIdFromFileInternal(const String& _relativePath,const String& _platform)
{

#ifdef ENABLE_LOG
    f32 startTime = 0;
    if (SYSTEM_ADAPTER)
        startTime = static_cast<f32>(SYSTEM_ADAPTER->getTime());
#endif //ENABLE_LOG

    String filename = FILESERVER->getCachedNamePlatform(_relativePath,_platform);    
	bbool needRebuild = bfalse;

	if (!m_bModeFinal)
		needRebuild = needRebuildId(_relativePath);

    if (!needRebuild)
    {
        File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_READ);

        if(pFile)
        {
            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            pFile->read(pBuffer,lenght);

            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            ArchiveMemory rd(pBuffer,lenght);
            SF_DEL_ARRAY(pBuffer);

            LogicDataLink* pLogicDataLink = readFromBuffer(rd);

            String keyname = _relativePath;
            keyname.toLower();
            {
                csAutoLock cs(m_cs);
                m_LogicDataTables[keyname]      = pLogicDataLink;
            }


#ifdef ENABLE_LOG
            f32 timeToLoad = 0;
            if (SYSTEM_ADAPTER)
                timeToLoad = static_cast<f32>(SYSTEM_ADAPTER->getTime()-startTime);
            pLogicDataTableRead->setTimeToLoad(timeToLoad);
            LOG("open LogicDataTable %ls %0.02f ms",_refname.cStr(),timeToLoad*1000.0f);
#endif //ENABLE_LOG
            
            return pLogicDataLink;
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataLink* LogicDataBase::loadIdFromFile(const String& _relativePath,const String& _platform)
{ 
    String keyname = _relativePath;
    keyname.toLower();

    ITF_MAP<String_Uncached,LogicDataTableDependencies*>::const_iterator iterDependency = m_DependenciesTables.find(keyname);


    //not found in the dependencies,must be build
    if (iterDependency == m_DependenciesTables.end())
    {
        return NULL;
    }


    return loadIdFromFileInternal(_relativePath,_platform);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataBase::LogicDataBase()
:   m_initDone(bfalse)
,   m_disableSaving(bfalse)
,   m_bigEndian(bfalse)
,   m_luaHandler(NULL)
,   m_bRemote(bfalse)
,	m_bModeFinal(bfalse)
,	m_bRaiseError(btrue)
{
	
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataBase::init(const String& _platform,bbool _bFinalMode,bbool _bUseDependencies)
{
    if(m_initDone)
        return;

	m_bModeFinal = _bFinalMode;
    m_enableBinarisation = bfalse;

    Synchronize::createCriticalSection(&m_cs);
    Synchronize::createCriticalSection(&m_csAccess);

#if !defined( ITF_CONSOLE )
    if (_bUseDependencies)
    {
        serializeStringTable(btrue,bfalse,_platform);
        loadDependencies(_platform);
    }
#endif //ITF_CONSOLE
    
    m_initDone = btrue;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataBase::destroy()
{
    if(!m_initDone)
        return;

    Synchronize::destroyCriticalSection(&m_cs);
    Synchronize::destroyCriticalSection(&m_csAccess);

    clear();

    for (ITF_MAP<keyID,char*>::iterator iter = m_keyToString.begin();iter!= m_keyToString.end();++iter)
    {
        SF_DEL_ARRAY((*iter).second);
    }
    
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataBase::saveDependencies(const String& _platform)
{
    if (m_disableSaving)
        return;

    ArchiveMemory aw;

    u32 version = Versioning::logicDatabaseCacheVer();

    aw.serialize(version);
    u32 size = m_DependenciesTables.size();
    aw.serialize(size);

    ITF_MAP<String_Uncached,LogicDataTableDependencies*>::const_iterator iterDependency = m_DependenciesTables.begin();
    for (;iterDependency!=m_DependenciesTables.end();++iterDependency)
    {
        LogicDataTableDependencies* pDependencies = (*iterDependency).second;
        const String& filename = (*iterDependency).first;
        filename.serialize(aw);
        pDependencies->serialize(aw);
    }

    if (!aw.isReading())
    {
        String filename = FILESERVER->getCachedNamePlatform("dependencies",_platform);

        File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
        if (pFile)
        {
            pFile->write(aw.getData(),aw.getSize());
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataBase::loadDependencies(const String& _platform)
{
    String filename = FILESERVER->getCachedNamePlatform("dependencies",_platform);

    if (FILEMANAGER->fileExists(filename))
    {
        File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_READ);
        if (pFile)
        {
            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            pFile->read(pBuffer,lenght);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            ArchiveMemory rd(pBuffer,lenght);
            SF_DEL_ARRAY(pBuffer);

            u32 version;
            rd.serialize(version);
            //the version doesn't match,all the logic database must be flushed
            if (version != Versioning::logicDatabaseCacheVer())
                return;

            u32 sizeDependencies = 0;
            rd.serialize(sizeDependencies);
            String filename;

            for (u32 index =0; index<sizeDependencies; ++index)
            {
                filename.serialize(rd);
                LogicDataTableDependencies* pDependency = newAlloc(mId_LogicDataBase, LogicDataTableDependencies);
                pDependency->serialize(rd);

                m_DependenciesTables[filename] = pDependency;
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicDataTable* LogicDataBase::getTableFromId(const String& _refname,const String& _platform)
{
    csAutoLock cs(m_csAccess);
    String keyname = _refname;
    keyname.toLower();

    {
        csAutoLock cs(m_cs);
        ITF_MAP<String_Uncached,LogicDataLink*>::const_iterator iter = m_LogicDataTables.find(keyname);

        if (iter!=m_LogicDataTables.end())
        {
            return ((*iter).second)->m_pLogicDataTable;
        }
     
    }

    LogicDataLink* pLogicDataLink = loadIdFromFile(_refname,_platform);

    //try to create...
    if (!pLogicDataLink)
        pLogicDataLink = createFile(_refname,_platform);

#ifdef _DEBUG
    if (!pLogicDataLink)
    {
        String8 txt;
        txt.setTextFormat("ref not found : %ls" ,_refname.cStr());
        ITF_ASSERT_MSG(0,txt.cStr());
    }
#endif //_DEBUG

    if (!pLogicDataLink)
        return NULL;

    return pLogicDataLink->m_pLogicDataTable;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* LogicDataBase::findKey(keyID _key)
{
    ITF_MAP<keyID,char*> ::iterator iter = m_keyToString.find(_key);

    if (iter != m_keyToString.end())
    {
        return (*iter).second;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
u32  LogicDataBase::addKeyString(const String8& _key)
{
    keyID valKey = StringID::StrToCRC(_key.cStr());
    ITF_MAP<keyID,char*> ::iterator iter = m_keyToString.find(valKey);

    if (iter == m_keyToString.end())
    {
        //allocate new string
        char* copyKey = _key.getCharCopy();
        m_keyToString[valKey] = copyKey;
    }
     
    return valKey;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataBase::serializeStringTable(bbool _bReading,bbool _bDelete,const String& _szPlatform)
{
    const String relativeName = "string";

	String filename = FILESERVER->getCachedNamePlatform(relativeName,_szPlatform);

#ifndef ITF_WINDOWS
    if (_bDelete)
    {
        FILEMANAGER->deleteFile(filename);

        for (ITF_MAP<keyID,char*>::iterator iter = m_keyToString.begin();iter!= m_keyToString.end();++iter)
        {
            SF_DEL_ARRAY((*iter).second);
        }

        m_keyToString.clear();
    }
#endif //ITF_WINDOWS

    if (_bReading)
    {
        if (FILEMANAGER->fileExists(filename))
        {
            File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_READ);

            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            pFile->read(pBuffer,lenght);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            ArchiveMemory am(pBuffer,lenght);

            u32 _size = 0;
            am.serialize(_size);

            ITF_ASSERT(!m_keyToString.size());

        
            for (u32 index = 0;index<_size;++index)
            {                    
                u32 keyid,len;
                am.serialize((u32&)keyid);
                am.serialize(len);
                char* pBuffer = newAlloc(mId_LogicDataBase, char[len]);//including trail 
                am.serializeBlock8((u8*)pBuffer,len);
                m_keyToString[keyid] = pBuffer;
            }

            SF_DEL_ARRAY(pBuffer);
        }
        else
        {
            //file not ready ,doesn't matter
            return;
        }
    }
    else
    {
        ArchiveMemory aw;
        u32 _size = m_keyToString.size();
        aw.serialize(_size);

        for (ITF_MAP<keyID,char*> ::const_iterator iter = m_keyToString.begin();iter!=m_keyToString.end();++iter)        
        {
            keyID     keyid             = (*iter).first;
            const char*   keyvalue      = (*iter).second;

            aw.serialize((u32&)keyid);
            u32 len = u32(strlen(keyvalue)+1);//save the 0 trail
            aw.serialize(len);
            aw.serializeBlock8((u8*)keyvalue,len);
        }

        File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
        if (pFile)
        {
            pFile->write(aw.getData(),aw.getSize());
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogicDataBase::flushId(const String& _refname)
{
    String keyname = _refname;
    keyname.toLower();

    {
        csAutoLock cs(m_cs);
        ITF_MAP<String_Uncached,LogicDataLink*>::iterator iter = m_LogicDataTables.find(keyname);

        if (iter!=m_LogicDataTables.end())
        {
            SF_DEL((*iter).second);
            m_LogicDataTables.erase(iter);
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef ITF_WINDOWS
static void computeDependencies( LogicDataTableDependenciesCollector* collectDependencies, DepCollection& _depCollection, String AnimationPath )
{
    static const char* lineIdsTag[] =
    {
        "lineid",
        "lineid2",
        "lineid3",
        "lineid4",
        "lineid5",
        "lineid6",
        NULL
    };

    ITF_ASSERT_CRASH( collectDependencies != NULL, "Invalid collector instance" );

    static String anm = ".anm";
    static String tryTobeAFile = ".";
    ITF_MAP<String,String>::const_iterator  strIt;
    ITF_MAP<String,f32>::const_iterator     numIt;

    //look for path animation;
    strIt = collectDependencies->m_strValues.find( "animationpath" );
    if ( strIt != collectDependencies->m_strValues.end() )
        AnimationPath = strIt->second;
    
    //find all animation path folder and update them
    for ( strIt = collectDependencies->m_strValues.begin(); strIt != collectDependencies->m_strValues.end(); ++strIt )
        if ( strIt->second.strstr( tryTobeAFile.cStr() ) )
        {
            if ( !AnimationPath.isEmpty() && strIt->second.strstr( anm.cStr() ) )
            {
                _depCollection.add( AnimationPath + strIt->second );
                continue;
            }
            _depCollection.add( strIt->second );
        }

    Path locPath;
    PathString_t pathFilename;

    //resolve loc dependencies
    for ( u32 i = 0; lineIdsTag[i] != NULL; ++i )
    {
        numIt = collectDependencies->m_numValues.find( lineIdsTag[i] );
        if ( numIt != collectDependencies->m_numValues.end() )
        {
            LocalisationId id;

            id =(u32) numIt->second;
            locPath = LOCALISATIONMANAGER->getAudioPath(id);
            if ( !locPath.isEmpty() )
            {
                locPath.getString(pathFilename);
                _depCollection.add( pathFilename );
            }
        }
    }

    ITF_MAP< String, String >::const_iterator archetypeIt = collectDependencies->m_strValues.find( "archetype" );
    ITF_MAP< String, String >::const_iterator typeIt = collectDependencies->m_strValues.find( "type" );
    ITF_VECTOR< Path >                       fxDep;

    FEEDBACKFX_MANAGER->getDependencies( archetypeIt != collectDependencies->m_strValues.end() ? archetypeIt->second : StringID::Invalid, typeIt != collectDependencies->m_strValues.end() ? typeIt->second : StringID::Invalid, fxDep );
    for ( u32 i = 0; i < fxDep.size(); ++i )
    {
        fxDep[i].getString(pathFilename);
        _depCollection.add( pathFilename );
    }

    for ( u32 i = 0; i < collectDependencies->m_childDep.size(); ++i )
        computeDependencies( collectDependencies->m_childDep[i], _depCollection, AnimationPath );
}

void LogicDataBase::dependenciesFile(const String& _vPlatform,const String& file,DepCollection& _depCollection)
{
    char * readBuffer = loadActBuffer(file);

    if (readBuffer)
    {
        //String anm = ".anm";
        ///////////////////////////////////////////////////////////////////////////
        LogicDataTableDependenciesCollector collectDependencies;
        CompilLogicDataInfo compilInfo =compileBuffer(_vPlatform,file,readBuffer,&collectDependencies);

        if (compilInfo.nodecacheTable)
        {
            compilInfo.nodecacheTable->destroy();
            SF_DEL(compilInfo.nodecacheTable);
            SF_DEL(compilInfo.dependency);
        }

        SF_DEL( m_luaHandler);

        String AnimationPath;

        computeDependencies( &collectDependencies, _depCollection, AnimationPath );
    }
}
#endif //ITF_WINDOWS

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // namespace ITF

#endif //ITF_SUPPORT_LOGICDATABASE

