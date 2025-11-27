// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

namespace ITF
{
    cookerFactory* cookerFactory::m_pInstance = NULL;
    bbool cookerFactory::m_bForceVersioningCheck = bfalse;
    bbool cookerFactory::m_bForceMode = bfalse;

    u32 WINAPI Cook_Run(void* user)
    {
        cookerFactory* cookFactory = (cookerFactory*)(user);
      
        cookFactory->runInBackground();
        return 0;
    }

    cookerFactory::cookerFactory():m_allowExit(bfalse), m_globalCookingTime(0.),m_isCooking(bfalse)
    {
        ITF_ASSERT_CRASH( m_pInstance == NULL, "Only one cookerFactory at a time" );
        m_pInstance = this;
    }

    void cookerFactory::init(bbool modeAsync)
    {
        m_bModeAsync = modeAsync;

        if (modeAsync)
        {
        
            Synchronize::createEvent(&m_cookEvent,NULL);
            Synchronize::resetEvent(&m_cookEvent);

            Synchronize::createEvent(&m_cookEventExit,NULL);
            Synchronize::resetEvent(&m_cookEventExit);

			Synchronize::createCriticalSection(&m_cs);
			Synchronize::createCriticalSection(&m_csCooking);
            Synchronize::createCriticalSection(&m_csCookingNaming);

            ITF_MemoryBarrier();
            THREADMANAGER->createThread(Cook_Run, this, ThreadSettings::m_settings[eThreadId_CookBackGround]);


        }

        if (RESOURCE_MANAGER)
        {
            ResourceManager::m_cookingGroupID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);
            m_cookingGroup = (ResourceGroup*)ResourceManager::m_cookingGroupID.getResource();
        }
    }

    void cookerFactory::getDependencyFiles( const String& _filename, const String& _platform, DepCollection& _dependencies )
    {
        String                                  ext = FilePath::getExtension(_filename);
        ITF_MAP<String,cookerFilter*>::iterator iter = m_Factory.find(ext);

        if (iter!=m_Factory.end())
        {
            cookerFilter* ptrCookerFilter = ((*iter).second);

            ptrCookerFilter->callDependencies( m_cacheKey, _platform, _filename, _dependencies );
        }
    }

    void cookerFactory::getDependencyFiles( const String& _filename, const String& _platform, DepCollector& _dependencies )
    {
        DepCollection   col;
#ifdef ITF_WINDOWS
        u32             timer = GetTickCount();
#endif
        col.destroyFilters();
        getDependencyFiles( _filename, _platform, col );
        if ( _dependencies.getFilters() != NULL )
        {
            col.initFilters( _dependencies.getFilters() );
            col.filterExt();
        }
#ifdef ITF_WINDOWS
        timer = GetTickCount() - timer;
        _dependencies.addStat( FilePath::getExtension( _filename ), timer );
#endif
        _dependencies.add( col );
    }

    void cookerFactory::resolveDependencyFiles( const String& _platform, DepCollector& _dependencies )
    {
        String current;

        while (_dependencies.getNextProcessing(current))
            getDependencyFiles( current, _platform, _dependencies );
    }

    void  cookerFactory::getCurrentlyCookedFileName(String& _result)
    {
        csAutoLock cs(m_csCookingNaming);
        _result = m_currentFileBeingCooked;
    }

	void cookerFactory::cookInternal(cookerFilter* ptrCookerFilter,const String& _platform,const String& _filename)
	{
		csAutoLock cs(m_csCooking);
		beginCooking(_filename);
		bbool needDeletion = ptrCookerFilter->call(_platform,_filename);
		endCooking();

		if (needDeletion)
			cookerFactory::deleteOldVersion(_filename,_platform);

	}

    void   cookerFactory::beginCooking(const String& _szFilename)
    {
        csAutoLock cs(m_csCookingNaming);
        m_currentFileBeingCooked = _szFilename;
        m_isCooking =btrue;
    }

    void   cookerFactory::endCooking()
    {
        csAutoLock cs(m_csCookingNaming);
        m_currentFileBeingCooked ="";
        m_isCooking =bfalse;
    }


    void cookerFactory::runInBackground()
    {
        while (!m_allowExit)
        {
            ThreadSettings::threadStartFrame();

            Synchronize::waitEvent(&m_cookEvent);

            const String& platform = FILESERVER->getPlatform();

            f64  beginCook = SYSTEM_ADAPTER->getTime();
            String currentFile;
            {
                csAutoLock cs(m_cs);
                currentFile = m_currentCook; 
            }
            
            if (!currentFile.isEmpty())
            {
                String ext = FilePath::getExtension(currentFile);
                ext.toLower();

                ITF_MAP<String,cookerFilter*> ::iterator iter = m_Factory.find(ext);
                if (iter!=m_Factory.end())
                {
                    cookerFilter* ptrCookerFilter = ((*iter).second);
					cookInternal(ptrCookerFilter,platform,currentFile);

#ifdef ITF_SUPPORT_COOKING
                    //tell the engine the resource need to be reloaded
                    Resource* pResource = RESOURCE_MANAGER->getResourceByPath_slow(currentFile);
                    if (pResource)
                    {

                        //update the cooked ready flags
                        if (pResource->isPhysicalExist())
                            pResource->cancelDefault();
                    }
#endif //ITF_SUPPORT_COOKING
                }
            }

            {
                csAutoLock cs(m_cs);
                m_currentCook.clear();
            }
            m_globalCookingTime += SYSTEM_ADAPTER->getTime() - beginCook;


            Synchronize::resetEvent(&m_cookEvent);
        }

        Synchronize::setEvent(&m_cookEventExit);
    }


    cookerFactory::~cookerFactory()
    {
        if (m_bModeAsync)
        {
            ITF_MemoryBarrier();
            {
                csAutoLock cs(m_cs);
                m_currentCook.clear();
            }

            m_allowExit = btrue;
            Synchronize::setEvent(&m_cookEvent);

            //exit done ? wait until is done
            Synchronize::waitEvent(&m_cookEventExit);

            Synchronize::destroyCriticalSection(&m_cs);
			Synchronize::destroyCriticalSection(&m_csCooking);
            Synchronize::destroyCriticalSection(&m_csCookingNaming);
			
            for (ITF_MAP<String,cookerFilter*> ::iterator iter = m_Factory.begin();iter!=m_Factory.end();++iter)
            {
                delete (*iter).second;
            }


            for (ITF_MAP<String,cookerFilter*> ::iterator iter = m_FactoryHRes.begin();iter!=m_FactoryHRes.end();++iter)
            {
                delete (*iter).second;
            }
        }

        if (RESOURCE_MANAGER)
            RESOURCE_MANAGER->removeUserToResourceLogicalData(ResourceManager::m_cookingGroupID);
        m_pInstance = NULL;
    }


    void cookerFactory::update()
    {
        csAutoLock cs(m_cs);

        if (m_listCookPending.size() && m_currentCook.isEmpty())
        {
            //send the last for cooking 
            m_currentCook =  m_listCookPending.front();
            m_listCookPending.erase(m_listCookPending.begin());

            Synchronize::setEvent(&m_cookEvent);
        }
        
    }

    bbool cookerFactory::isCookable(const String& _filename)
    {
        String ext = FilePath::getExtension(_filename);
        ext.toLower();

        ITF_MAP<String,cookerFilter*> ::const_iterator iter = m_Factory.find(ext);

        if (iter!=m_Factory.end())
        {
            const cookerFilter* pFilter = (*iter).second;
            return (pFilter->getCookInformation().m_funcCook != NULL);
        }

        return bfalse;
    }

    bbool cookerFactory::directCook(const String& _platform,const String& _filename)
    {
        String ext = FilePath::getExtension(_filename);
        ext.toLower();

        ITF_MAP<String,cookerFilter*> ::iterator iter = m_Factory.find(ext);
        if (iter!=m_Factory.end())
        {
            cookerFilter* ptrCookerFilter = ((*iter).second);
            return ptrCookerFilter->call(_platform,_filename);

        }
            
        return bfalse;
    }


    void cookerFactory::cookHRes(const String& _filename)
    {
        String ext = FilePath::getExtension(_filename);
        ITF_MAP<String,cookerFilter*> ::iterator iter = m_FactoryHRes.find(ext);
        if (iter!=m_FactoryHRes.end())
        {
            cookerFilter* ptrCookerFilter = ((*iter).second);
            cookInternal(ptrCookerFilter,"PC",_filename);
        }
    }

    void cookerFactory::cook(const String& _platform,const String& _filename)
    {        
        f64  beginCook = SYSTEM_ADAPTER->getTime();
        String ext = FilePath::getExtension(_filename);

        ITF_MAP<String,cookerFilter*> ::iterator iter = m_Factory.find(ext);
        if (iter!=m_Factory.end())
        {
            cookerFilter* ptrCookerFilter = ((*iter).second);
  
            if (ptrCookerFilter->getCookInformation().m_delayedCook)
            {
                csAutoLock cs(m_cs);
                m_listCookPending.push_back(_filename);
            }
            else
            {
				cookInternal(ptrCookerFilter,_platform,_filename);
            }
        }
        m_globalCookingTime += SYSTEM_ADAPTER->getTime() - beginCook;
    }


    bbool cookerFactory::skipVersioningPlatform(const String& _szPlatform)
    {
        if (m_bForceVersioningCheck == btrue)
            return bfalse;

        return (_szPlatform == "PC" || _szPlatform == "EMUWII");
    }

    void cookerFactory::getExtensionVersion(const String& _filename,const String& _platform,String& _dst)
    {
        //add the version extension only on PC version
        if (!skipVersioningPlatform(_platform))
            return;

        String ext = FilePath::getExtension(_filename);
        ext.toLower();

        ITF_MAP<String,cookerFilter*> ::iterator iter = m_Factory.find(ext);
        if (iter!=m_Factory.end())
        {
            cookerFilter* ptrCookerFilter = ((*iter).second);
            _dst.setTextFormat(".V%05d",ptrCookerFilter->getCookInformation().m_version);
        }
    }

    void cookerFactory::registerCookerHRes(const String& _extension, func_cook  _funcCook,u32 _version)
    {
        ITF::String lower_extention = _extension;
        lower_extention.toLower();

        ITF_ASSERT_CRASH(m_FactoryHRes.find(lower_extention) == m_FactoryHRes.end(), "Can't register twice the same extension");

        cookerFilter* pCookerFilter = newAlloc(mId_System, cookerFilter(_extension,_funcCook,NULL,_version,bfalse));
        m_FactoryHRes[lower_extention] = pCookerFilter;
    }

    void cookerFactory::registerCooker(const String& _extension, func_cook  _funcCook,funcDependencies _funcDependencies,u32 _version,bbool _delayedCook /*= bfalse*/)
    {
        ITF::String lower_extention = _extension;
        lower_extention.toLower();

        ITF_ASSERT_CRASH(m_Factory.find(lower_extention) == m_Factory.end(), "Can't register twice the same extension");

        cookerFilter* pCookerFilter = newAlloc(mId_System, cookerFilter(_extension,_funcCook,_funcDependencies,_version,_delayedCook));
        m_Factory[lower_extention] = pCookerFilter;
    }

	String cookerFactory::getPattern() const
	{
		String tmp,pattern;
		u32 count = 0;
		const u32 factorySize = m_Factory.size();
		ITF_MAP<String,cookerFilter*> ::const_iterator iter = m_Factory.begin();
		for (iter;iter!=m_Factory.end();iter++)
		{	
			tmp.setStringFormat("*.%ls",((*iter).first).cStr());
			
			count++;
			if (count!=factorySize)
				tmp+=";";

			pattern +=tmp;
			
		}

		return pattern;
	}

    void cookerFactory::deleteOldVersion(const String& _filename,const String& _platform)
    {
        //delete old files related to _filename;
        String cookedname = FILESERVER->getCookedNamePlatform(_filename,_platform,bfalse);
        String cookednameNew = FILESERVER->getCookedNamePlatform(_filename,_platform);

        ITF_VECTOR<String> files;
        String star("*");
        String coo = FilePath::getFilenameWithoutExtension(cookedname)+"*.*";
        Directory::getFilesMatch(files,FilePath::getDirectory(cookedname),coo);

        for ( ITF_VECTOR<String>::iterator iter = files.begin();iter!=files.end();++iter)
        {
            if (!(cookednameNew == *iter))//little dirty,no better way
                FILEMANAGER->deleteFile(*iter);
        }
    }

    u32 cookerFactory::getVersionFile(const String& _szFilename)
    {
        if (!FILEMANAGER->fileExists(_szFilename))
            return 0;

        u32 lenght = 0;
        u8 * pBuffer = NULL;
        File* file = FILEMANAGER->openFile(_szFilename,ITF_FILE_ATTR_READ);

        if (!file)
            return 0;

        u32 version = 0;           
        lenght = sizeof(version);
        pBuffer = new_array(u8,lenght,mId_System);
        file->read(pBuffer,lenght);
        ArchiveMemory rd(pBuffer,lenght);
        rd.serialize(version);

        FILEMANAGER->closeFile(file);

        delete[] pBuffer;
        return version;
    }

    bbool cookerFactory::fileNeedsCook(const String & _file, const String & _cookFile,const String& _platform, u32 _version)
    {
        if(m_bForceMode)
            return btrue;

        // $GB added support of alternative source file names for resources
        String realFileName = ResourceManager::getExistingAlternativeFileNameForCooking(_file, FILESERVER->getPlatform());

        u64 timeBin     = FILEMANAGER->getLastTimeWriteAccess(_cookFile);
        u64 timeWrite   = FILEMANAGER->getLastTimeWriteAccess(realFileName);

        if (timeBin != timeWrite)
            return btrue;

        if (_version != U32_INVALID && !skipVersioningPlatform(_platform))
        {
            u32 version = getVersionFile(_cookFile);
            if (version != _version)
                return btrue;
        }
      
        return bfalse;
    }

} // namespace ITF
