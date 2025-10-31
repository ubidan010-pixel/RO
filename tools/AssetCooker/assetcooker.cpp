#include "core/enginecommon.h"

#include <stdio.h>
#include <tchar.h>

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_COMMANDARGS_H_
#include "engine/common/CommandArgs.h"
#endif //_ITF_COMMANDARGS_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_


#ifndef _ITF_ANIMATIONCOOKER_H_
#include    "tools/plugins/CookerPlugin/AnimationCooker.h"
#endif // _ITF_ANIMATIONCOOKER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_GFXADPATER_COOKER_H
#include "adapters/GFXAdapter_Cooker/GFXAdapter_Cooker.h"
#endif //_ITF_GFXADPATER_COOKER_H

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_UNPACKVIDEOCAPTURE_H_
#include "tools/plugins/CookerPlugin/unpackVideoCapture.h"
#endif //_ITF_UNPACKVIDEOCAPTURE_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_GAMEPLAYFACTORYFILLER_H_
#include "gameplay/GameplayFactoryFiller.h"
#endif //_ITF_GAMEPLAYFACTORYFILLER_H_

#ifndef _ITF_RAY_GAMEINTERFACE_H_
#include "rayman/gameplay/Ray_GameInterface.h"
#endif //_ITF_RAY_GAMEINTERFACE_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_INITFACTORY_H_
#include "initfactory/initfactory.h"
#endif //_ITF_INITFACTORY_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_COOKERPLUGIN_H_
#include "tools/plugins/CookerPlugin/CookerPlugin.h"
#endif //_ITF_COOKERPLUGIN_H_

#ifndef _ITF_TEXTURECOOKER_H_
#include "tools/plugins/CookerPlugin/textureCooker.h"
#endif //_ITF_TEXTURECOOKER_H_

#ifndef _ITF_SOUNDCOOKER_H_
#include "tools/plugins/CookerPlugin/soundCooker.h"
#endif //_ITF_SOUNDCOOKER_H_

#ifndef _ITF_SCENECOOKER_H_
#include "tools/plugins/CookerPlugin/sceneCooker.h"
#endif //_ITF_SCENECOOKER_H_

#ifndef _ITF_FILEPACKER_H_
#include "tools/plugins/CookerPlugin/filePacker.h"
#endif //_ITF_FILEPACKER_H_

#ifndef _ITF_ACTCOOKER_H_
#include "tools/plugins/CookerPlugin/actCooker.h"
#endif //_ITF_ACTCOOKER_H_

#ifndef _ITF_ATLASCOOKER_H_
#include    "tools/plugins/CookerPlugin/atlasCooker.h"
#endif // _ITF_ATLASCOOKER_H_

#ifndef _ITF_TCPADAPTER_H_
#include "engine/AdaptersInterfaces/TCPAdapter.h"
#endif //_ITF_TCPADAPTER_H_

#ifndef _ITF_TCPADAPTER_ITF_H_
#include "adapters/TCPAdapter_ITF/TCPAdapter_ITF.h"
#endif //_ITF_TCPADAPTER_ITF_H_


#ifndef _ITF_FRIEZECOOKER_H_
#include "engine/display/frieze/FriezeCooker.h"
#endif //_ITF_FRIEZECOOKER_H_

#ifndef _ITF_PACKCONFIGCOOKER_H_
#include "tools/plugins/CookerPlugin/packConfigCooker.h"
#endif

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif  //_ITF_REWARDADAPTER_H_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#ifndef _ITF_DEP_PARSER_H_
#include "tools/plugins/CookerPlugin/XmlDepParser.h"
#endif //_ITF_DEP_PARSER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ADAPTERSOURCECONTROL_PERFORCE_H_
#include "adapters/Adapter_SourceControl_Perforce/SourceControlAdapter_Perforce.h"
#endif //_ADAPTERSOURCECONTROL_PERFORCE_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#include <iostream>
#include <fstream>
#include <string>

namespace ITF
{

//--------------------------------------------------------------------------------------------------------

void cookAllScenes(ITF_VECTOR<String>& _vPlatform,const ITF_VECTOR<String>& _vFiles)
{
    LOG("Cooking scenes...\n");
    LUA_HANDLER->init();
    const String8 path("GameManager/game.lua");
    LUA_HANDLER->executeFile(path.cStr());
    for (ITF_VECTOR<String>::const_iterator iterName = _vFiles.begin();iterName!=_vFiles.end();++iterName)
    {
        String file = (*iterName);
        file = FilePath::normalizePath(file);
        String ext = FilePath::getExtension(file);

        if (ext == "isc" || ext == "tsc")
        {
            for (ITF_VECTOR<String>::const_iterator iterPlatform = _vPlatform.begin();iterPlatform!=_vPlatform.end();++iterPlatform)
            {
                if (file.strstr((u16*)L"scene")!=NULL)
                   sceneCooker::cookFile(*iterPlatform, file);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------

void init()
{    
    Singletons::get().setFileManager(new FileManager_ITF());
}


//--------------------------------------------------------------------------------------------------------


bbool addPlatform(ITF_VECTOR<String>& _vPlatform,const String& _szPlatform)
{
    String platformL = _szPlatform;
    platformL.toUpper();

    if (platformL == "EMUWII" || platformL == "FINALPC" || platformL == "PC" || platformL == "X360" || platformL == "PS3" || platformL == "IPAD" || platformL == "WII" || platformL == "CTR") 
    {
        ITF_VECTOR<String>::iterator iter = find(_vPlatform.begin(),_vPlatform.end(),_szPlatform);

        if (iter==_vPlatform.end())
        {
            _vPlatform.push_back(_szPlatform);

            String tmpDir;
            tmpDir.setTextFormat("tmpDirectory_%S",_szPlatform.cStr());
            Directory::create(tmpDir);

            return btrue;
        }
    }


    return bfalse;
}


//--------------------------------------------------------------------------------------------------------



    class cookerSettings
    {
    public:

        cookerSettings()
        {
            Synchronize::createEvent(&m_endEvent,"endEvent");
        };

        ITF_VECTOR<String>              m_Files;
        ITF_VECTOR<String>              m_vPlatform;
        String                          m_workingDirectory;
        ITF_THREAD_EVENT                m_endEvent;
        
    };
    
    cookerSettings* g_pCookerSettings       = NULL;
    cookerFactory*  g_pCookerFactory        = NULL;
    bbool           g_canProcessPackConfig  = bfalse;


    void processPackInfo(const String& _filename)
    {
        //build the full name
        String file = g_pCookerSettings->m_workingDirectory+_filename;
        file = FilePath::normalizePath(file);
        file.toLower();
        for (ITF_VECTOR<String>::const_iterator iterPlatform = g_pCookerSettings->m_vPlatform.begin();iterPlatform != g_pCookerSettings->m_vPlatform.end();++iterPlatform)
        {
            if (g_canProcessPackConfig)
            {
                packConfig_Cooker::cook(*iterPlatform, file);
            }
        }
        
    }
    void nextFileRelative(const String& _filename)
    {
        //String is not empty
        //quick eject:
        if (FilePath::getFilename(_filename) == ".svn")
            return;

        //build the full name
        String file = _filename;
        file = FilePath::normalizePath(file);
        file.toLower();

		//String directory = FilePath::getDirectory(_filename);

        for (ITF_VECTOR<String>::const_iterator iterPlatform = g_pCookerSettings->m_vPlatform.begin();iterPlatform != g_pCookerSettings->m_vPlatform.end();++iterPlatform)
        {
            g_pCookerFactory->cook(*iterPlatform,file);
        }
    }

    class SystemAdapter_Cooker : public SystemAdapter
    {
    public:

        SystemAdapter_Cooker()
        {
            LARGE_INTEGER ticksPerSec = { 0 };
            QueryPerformanceFrequency( &ticksPerSec );
            m_QPFTicksPerSec = ticksPerSec.QuadPart;
            m_baseTime = 0;
            m_stopTime = 0;
        }

        virtual String  userName()const  {return "UserCooker";}
        virtual String  dateTime() const {return "";}


        LARGE_INTEGER GetAdjustedCurrentTime()
        {
            LARGE_INTEGER time;
            if( m_stopTime != 0 )
                time.QuadPart = m_stopTime;
            else
                QueryPerformanceCounter( &time );
            return time;
        }

        void   queryPerformanceCounter(LARGE_INTEGER *counter)  {};
        void   queryPerformanceFrequency(LARGE_INTEGER *counter) {};

        f64 getTime()
        {
            LARGE_INTEGER curtime = GetAdjustedCurrentTime();
            f64 time = ( f64 )( curtime.QuadPart - m_baseTime ) / ( f64 )m_QPFTicksPerSec;
            return time;
        }

        virtual void    getTime(Time & _time) const {

            time_t currentTime;
            ::time(&currentTime);
            ::tm* t = ::localtime(&currentTime);
            _time.m_year = 1900 + t->tm_year;       // tm years are since 1900
            _time.m_month = 1 + t->tm_mon;          // tm month: 0-11 we needs 1-12
            _time.m_day = t->tm_mday;
            _time.m_hour = t->tm_hour;
            _time.m_minute = t->tm_min;
            _time.m_second = t->tm_sec;

        }

        virtual const  char*  getPlatformName() {return "UserCookerPC";}

        virtual i32 messageBox( const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT _silentValue )
        {
            u32 windowType  = 0;

            if (_type & ITF_MSG_ICONERROR)
                windowType |= MB_ICONERROR;

            if (_type & ITF_MSG_ICONWARNING)
                windowType |= MB_ICONWARNING;

            if (_type & ITF_MSG_OK)
                windowType |= MB_OK;

            if (_type & ITF_MSG_YESNOCANCEL)
                windowType |= MB_YESNOCANCEL;

            if (_type & ITF_MSG_ICONQUESTION)
                windowType |= MB_ICONQUESTION;

            if (_type & ITF_MSG_YESNO)
                windowType |= MB_YESNO;

			if(IsSilent())
			{
				if (_type & ITF_MSG_OK)				
					return ITF_IDYES;				
				else
					return _silentValue;
			}
			
				 
            u32 result = ::MessageBox(NULL, (LPCWSTR)_msg.cStr(), (LPCWSTR)_title.cStr(), windowType);

            switch (result)
            {
                case IDYES:
                    return ITF_IDYES;
                case IDNO:
                    return ITF_IDNO;
                case IDCANCEL:
                    return ITF_IDCANCEL;
                default:
                    return ITF_IDYES;
            }

        }

    private:
        i64 m_QPFTicksPerSec; 
        i64 m_stopTime;
        i64 m_baseTime;
    };

    u32 parseDirectory( cookerSettings* _cookerSettings)
    {
        
        ITF_VECTOR<String> vFiles;

        //.Packinfo files short circuit normal cooking
        Directory::searchFiles(vFiles,_cookerSettings->m_workingDirectory,"*.packconfig",btrue,processPackInfo);
        if (vFiles.size()==0)
        {
            Directory::searchFiles(vFiles,_cookerSettings->m_workingDirectory,"*.*",btrue,nextFileRelative);
        }

       
        return 0;
    }


void extractFilesFromList(String fileName,ITF_VECTOR<String> &vFiles)
{
    using namespace std;
    string line;
    ifstream myfile (StringConverter(fileName).getChar());
    if (myfile.is_open())
    {
        while ( myfile.good() )
        {
            getline (myfile,line);
            if ( !line.empty() )
                vFiles.push_back(String(line.c_str()));
        }
        myfile.close();
    }
}

void retrieveFilesFromDir( ITF_VECTOR< String >& vDirs, const String& rootDir, const String& sWildcard, bbool recursive )
{
    String                          fmtRootDir( rootDir );
    String                          currentDir;
    ITF_VECTOR< String >::size_type i = 0;
    ITF_VECTOR< String >::size_type size = vDirs.size();
    ITF_VECTOR< String >::size_type dirFound = 0;

    if ( fmtRootDir.cStr()[fmtRootDir.getLen() - 1] != '/' && fmtRootDir.cStr()[fmtRootDir.getLen() - 1] != '\\' )
        fmtRootDir += '/';
    while ( i < size - dirFound )
    {
        currentDir = fmtRootDir;
        currentDir += vDirs[i];
        if ( Directory::exists( currentDir ) )
        {
            if ( currentDir.cStr()[currentDir.getLen() - 1] != '/' && currentDir.cStr()[currentDir.getLen() - 1] != '\\' )
                currentDir += '/';
            Directory::searchFiles( vDirs, currentDir, sWildcard, recursive );
            vDirs.erase( vDirs.begin() + i );
            ++dirFound;
            continue;
        }
        ++i;
    }
    for ( i = 0; i < vDirs.size(); ++i )
        vDirs[i].replace( rootDir, "" );
}

int cook_main(int argc, _TCHAR* argv[])
{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
    ITF::memoryStatsManager::disable();
#endif  //ITF_CATEGORY_MEMORY_ENABLE
    // Force exe dir to be current directory if not debugging
    {
        Singletons::init();
        PLUGINGATEWAY->createSingleton();

        PLUGINGATEWAY->addPlugin(CookerPlugin::getPluginName(), new CookerPlugin());

        g_pCookerSettings = new cookerSettings;

        wchar_t strExePath[MAX_PATH]; 
        const DWORD uSize = GetModuleFileName(NULL, strExePath, MAX_PATH); 

		FILESERVER->setApplicationDirectory(FilePath::getDirectory((u16*) strExePath));

    #ifndef ITF_FINAL
        if (!IsDebuggerPresent()) //use the working directory set in your ide
    #endif //ITF_FINAL
        {
            if(uSize > 0)
            {
                for(int i = (uSize - 1); i > 0; --i)
                { 
                    if('\\' == strExePath[i]) 
                    { 
                        strExePath[i] = '\0';
                        ITF_VERIFY_MSG(SetCurrentDirectory(strExePath) == TRUE, "Could not set the current directory"); 
                        break; 
                    } 
                } 
            }
        }
    }

    using namespace ITF;
    u32 sleepTime = 2000;
    DWORD tickStart = GetTickCount();

    SINGLETONS.setResourceManager(new ResourceManager());

    g_pCookerFactory = cookerFactory::getInstance();

#ifndef ITF_FINAL
    AnimTrackResource::m_loadRessourcesOnActor = bfalse;
#endif //ITF_FINAL

#ifndef ITF_FINAL
    WarningManager::initialize();
#endif

    if (argc >=2)
    {

        SINGLETONS.setSystemAdapter(new SystemAdapter_Cooker());

        String cmdLine((u16*)argv[1]);
        CommandArgs args("directory",cmdLine);

        bbool modeTask = bfalse;

        // Runtime assert activation by command line
        String szArg; 
        if(args.find("assert",szArg))
        {
            FlagType flag = 0;
            if(szArg == "-1")
                flag = AssertManager::m_FlagAll;
            else
                flag = szArg.atoui32();

            AssertManager::setFlag(flag);
            LOG("[ApplicationFramework] Assertions enabled by command line with flag : 0x%x", flag);
        }

        ITF_ASSERT(!args.isSet("break"));

        if (args.isSet("task"))
        {
            modeTask = btrue;
        }

        String re;
        ITF_VECTOR<String> vPlatform;
        bbool useTemporaryConfig = bfalse;

        String szDirectory;
        if (args.find("directory",szDirectory))
        {
            String workingDirectory = szDirectory;
            String subDirectory;
            String destinationDirectory = "";

            if ( workingDirectory.cStr()[workingDirectory.getLen() - 1] != '\\' && workingDirectory.cStr()[workingDirectory.getLen() - 1] != '/' )
                workingDirectory += '/';
            FILESERVER->setWorkingDepot(workingDirectory);

            init();

            SINGLETONS.setGFXAdapter(new GFXAdapter_Cooker());
            FILEMANAGER->mountFileDevice("MainDrive", DATA_FOLDER, ePriorityFileDevice::ePriorityFileDevice_HDD);
            ALIASMANAGER->loadDefault();


			FILESERVER->rebuildCookDepot("PC");//lock the file server
			
            LOGICDATABASE->init(FILESERVER->getPlatform(),bfalse);

            // we now have a project root, get its config
            if (!XMLAll::readProjectConfig(CONFIG, "project.xml"))
            {
                ITF_FATAL_ERROR("project.xml read failure");
            }
            
            LOG("Cook starting");
            LOG("Cook from directory:%S",szDirectory.cStr());
            LOG("AnimSkeleton version:%d",Versioning::animSkeletonVer());
            LOG("AnimPatchBank version:%d",Versioning::animPatchBankVer());
            LOG("AnimTrack version:%d",Versioning::animTrackVer());
            LOG("LogicDatabase version:%d",Versioning::logicDatabaseCacheVer());
            LOG("FriezeConfig version:%d",Versioning::friezeConfigVer());
            LOG("CUDA enable:%s",CookerPlugin::cudaIsEnable() ? "true" : "false");

            ITF_VECTOR<String>  vFiles;
            ITF_VECTOR<String>  vDepFiles;
            String              sDepOut;
            String tmp;

            if (args.find("processDir",tmp))
            {
                subDirectory = tmp+"\\";
            }

            bbool _bRaiseError          = bfalse;
            bbool cookScenes            = bfalse;
            bbool generateFilePack      = bfalse;
            bbool bCookFileList         = bfalse;
            bbool restorecapture        = bfalse;
            bbool generateDependencies  = bfalse;
            u32   qualityJpg            = 0;

            // Game singletons must be created once project is known (via CONFIG).
            GameInterface* gameInterface = new Ray_GameInterface();
            SINGLETONS.setGameInterface(gameInterface);
            gameInterface->registerObjects();
            SINGLETONS.setFeedbackFXManager(newAlloc(mId_Singleton, ITF::FeedbackFXManager));
            gameInterface->init();
#ifndef ITF_FINAL
            SINGLETONS.setSourceControlAdapter(newAlloc(mId_Editor,SourceControlAdapter_Perforce));
#endif //ITF_FINAL

            if (args.find("raiseerror",tmp))
            {
                _bRaiseError = btrue;
                LOG("Raise Error enable");
            }

            if (args.isSet("silent"))
            {
                SYSTEM_ADAPTER->setSilentMode(btrue);
            }
            
            bool bCookAll = bfalse;
            if (args.find("all",tmp))
            {
                bCookAll = btrue;
            }

            if (args.find("files",tmp))
            {
                bCookAll = btrue;
                bCookFileList = btrue;

                vFiles.clear();
                extractFilesFromList(tmp,vFiles);
            }
        
            bbool cookAct = bfalse;
            bbool cookGmt = bfalse;
            bbool cookIsg = bfalse;
            bbool cookFxk = bfalse;
            bbool cookFrt = bfalse;
            bbool cookAtl = bfalse;
            bbool cookWav = bfalse;
            bbool cookTga = bfalse;
            bbool cookPng = bfalse;
            bbool cookIsc = bfalse;
            bbool cookTsc = bfalse;
            bbool cookPtc = bfalse;
            bbool cookSkl = bfalse;
            bbool cookPbk = bfalse;
            bbool cookAnm = bfalse;
            bbool cookFcg = bfalse;
            bbool cookFile = bfalse;

            if (bCookAll || args.find("act",tmp))
            {
                cookAct = btrue;
                cookGmt = btrue;
                cookIsg = btrue;
                cookFxk = btrue;
                cookFrt = btrue;

                LOGICDATABASE->enableBinarisation();
            }

			if (bCookAll || args.find("gmt",tmp))
			{
                cookGmt = btrue;
			}

			if (bCookAll || args.find("atlas",tmp))
			{
                cookAtl = btrue;
			}

            if (bCookAll || args.find("sounds",tmp))
            {
                cookWav = btrue;
            }

            if (bCookAll || args.find("textures",tmp))
            {
                cookTga = btrue;
                cookPng = btrue;
            }

            /* disable scene cooking*/
            /*
            if (bCookAll || args.find("scenes",tmp))
            {
                cookScenes = btrue;
                cookIsc = btrue;
                cookTsc = btrue;
            }
            if(cookIsc)
                g_pCookerFactory->registerCooker("isc",sceneCooker::cookFile,XmlDepParser::iscDependencyFiles,Versioning::sceneVer());
            if(cookTsc)
                g_pCookerFactory->registerCooker("tsc",sceneCooker::cookFile,XmlDepParser::iscDependencyFiles,Versioning::sceneVer());
            */

            if (bCookAll || args.find("animations",tmp))
            {
                cookPtc = btrue;
                cookSkl = btrue;
                cookPbk = btrue;
                cookAnm = btrue;
                g_canProcessPackConfig = btrue;
            }            

            if (bCookAll || args.find("fcg",tmp))
            {
                cookFcg = btrue;
            }

            if(cookAct)
				g_pCookerFactory->registerCooker("act",actCooker::cookFile,NULL,Versioning::logicDatabaseCacheVer());
            if(cookGmt)
				g_pCookerFactory->registerCooker("gmt",actCooker::cookFile,NULL,Versioning::logicDatabaseCacheVer());
            if(cookIsg)
                g_pCookerFactory->registerCooker("isg",actCooker::cookFile,NULL,Versioning::logicDatabaseCacheVer());
            if(cookFxk)
                g_pCookerFactory->registerCooker("fxk",actCooker::cookFile,NULL,Versioning::logicDatabaseCacheVer());
            if(cookFrt)
                g_pCookerFactory->registerCooker("frt",actCooker::cookFile,NULL,Versioning::logicDatabaseCacheVer());

            if(cookAtl)
				g_pCookerFactory->registerCooker("atl",atlasCooker::cookFile,NULL,Versioning::atlasVer());

            if(cookWav)
                g_pCookerFactory->registerCooker("wav",soundCooker::cookFile,NULL,Versioning::soundVer());

            if(cookTga)
                g_pCookerFactory->registerCooker("tga",textureCooker::cookFile,NULL,Versioning::textureVer());
            if(cookPng)
                g_pCookerFactory->registerCooker("png",textureCooker::cookFile,NULL,Versioning::textureVer());
            
            if(cookPtc)
                g_pCookerFactory->registerCooker("ptc",animSkeletonCooker::cookFile,NULL,Versioning::animSkeletonVer());
            if(cookSkl)
                g_pCookerFactory->registerCooker("skl",animSkeletonCooker::cookFile,NULL,Versioning::animSkeletonVer());
            if(cookPbk)
                g_pCookerFactory->registerCooker("pbk",animPatchBankCooker::cookFile,NULL,Versioning::animPatchBankVer());
            if(cookAnm)
                g_pCookerFactory->registerCooker("anm",animTrackCooker::cookFile,NULL,Versioning::animTrackVer());

            if(cookFcg)
                g_pCookerFactory->registerCooker("fcg",FriseConfigCooker::cookFile,XmlDepParser::fcgDependencyFiles,Versioning::friezeConfigVer());

			if (args.find("force",tmp))
			{
                g_pCookerFactory->setForceMode(tmp.atoi32()==1 ? btrue : bfalse);
			}

            if (args.find("pack",tmp))
            {
                g_canProcessPackConfig = btrue;
                g_pCookerFactory->registerCooker("packconfig",packConfig_Cooker::cook,NULL,1);
            }

            if (args.find("restorecapture",tmp))
            {
                restorecapture = btrue;
                destinationDirectory = tmp;
            }

            if (args.find("qualityjpg",tmp))
            {
                qualityJpg = tmp.atoui32();
            }

            if (args.find("packfile",tmp))
                generateFilePack = btrue;

            if (args.find("platform",tmp))
            {
                String8 pattern(tmp.cStr());
                pattern.toUpper();

                const char sep =  ',';
                const char* current = pattern.cStr();
                while (true)
                {
                   char * pos = pattern.strstr(sep);
                   if (pos == 0)
                       break;

                   addPlatform(vPlatform,String(current ,pos-current));

                   while (*pos == sep)
                       pos++;

                   pattern = pos;
                   current = pattern.cStr();
                   
                }

                if (*current!=NULL)
                {
                    addPlatform(vPlatform,pattern);
                }
               
            }
            else //default is PC
            {
                addPlatform(vPlatform,"PC");
            }

            String textPlatform = "";
            for (u32 i = 0;i<vPlatform.size();i++)
            {
                textPlatform+=vPlatform[i];
                textPlatform += " ";
            }

            LOG("Cook for platform: %S",textPlatform.cStr());

            if (args.find("temporaryConfig",tmp))
                useTemporaryConfig = btrue;

          
            if (args.find("file",tmp))
            {
                String file = FilePath::normalizePath(tmp);
                String ext = FilePath::getExtension(file);
                if (ext == "tga" || ext == "png")
                {
                    for (ITF_VECTOR<String>::iterator iterPlatform = vPlatform.begin();iterPlatform!=vPlatform.end();++iterPlatform)
                    {
                        textureCooker::compress(*iterPlatform,file,useTemporaryConfig);   
                    }
                  
                    sleepTime = 0;
                }

                cookFile = btrue;
            }


            //use the cooker background thread settings
            String working_path = workingDirectory+subDirectory;
            g_pCookerSettings->m_workingDirectory = working_path;
            g_pCookerSettings->m_vPlatform = vPlatform;

            sDepOut = workingDirectory;
            vDepFiles.clear();
            if (args.find("depfiles",tmp))
            {
                generateDependencies = btrue;
                extractFilesFromList(tmp,vDepFiles);
            }

            if (args.find("depfile", tmp))
            {
                generateDependencies = btrue;
                vDepFiles.push_back(tmp);
            }

            if (args.find("depout", tmp))
                sDepOut = tmp;

            if (!cookFile)
            {
                if (bCookFileList)
                {

                    for (ITF_VECTOR<String>::iterator iterFile = vFiles.begin();iterFile!=vFiles.end();++iterFile)
                    {
                        //build the full name
                        String file = *iterFile;
                        file = FilePath::normalizePath(file);
                        file.toLower();

                        for (ITF_VECTOR<String>::iterator iterPlatform = vPlatform.begin();iterPlatform!=vPlatform.end();++iterPlatform)
                        {
                            g_pCookerFactory->cook(*iterPlatform,file);
                        }
                    }
                }
                else
                if (!restorecapture && !generateFilePack)
                {
    			    parseDirectory(g_pCookerSettings);
                }
            }

            if (restorecapture)
                unpackVideoCapture::restore(szDirectory,destinationDirectory,qualityJpg);

            if (generateFilePack)
            {

                wchar_t  workingDirectory[_MAX_PATH];
                _wgetcwd(workingDirectory,_MAX_PATH);

                String strWorkingDirectory;
                strWorkingDirectory.setTextFormat("%ls/",workingDirectory);

                Directory::searchFilesRelative(vFiles,g_pCookerSettings->m_workingDirectory,"*.*",btrue,NULL);

                FILEMANAGER->mountFileDevice("ExeDrive", strWorkingDirectory, ePriorityFileDevice::ePriorityFileDevice_HDD);
                for (ITF_VECTOR<String>::iterator iter = vPlatform.begin();iter!=vPlatform.end();++iter)
                {
                    FilePacker::make(szDirectory,*iter,vFiles);
                }
            }

        }

    }
    else
    {
        LOG("Missing arguments...");
        getchar();
        return 0;

    }

    LOG("\nCook done\n");


#ifndef ITF_FINAL
    WarningManager::terminate();
#endif


    DWORD tickEnd = GetTickCount();
    DWORD timeToCook = tickEnd-  tickStart; //in ms
    LOG("Time to cook: %0.02f seconds",timeToCook*0.001f);
    //to let time to read the status..

    if (RESOURCE_MANAGER)
    {
        RESOURCE_MANAGER->setImmediateFlushMode(btrue);
    }

    Sleep(sleepTime);
    SF_DEL(g_pCookerSettings);
    SF_DEL(g_pCookerFactory);


    Singletons::get().destroy();
    return 0;
}

};

//--------------------------------------------------------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{
    return ITF::cook_main(argc, argv);
}

//--------------------------------------------------------------------------------------------------------

#if defined(ITF_WIN32) && !defined(ITF_FINAL)
// Fake methods to enable linking
void ITF::ObjectGroup::update( ITF::f32 /*_elapsed*/ )
{
}

void ITF::ObjectGroup::draw(SafeArray<class LightComponent*>& _activeLights)
{
}
#endif
