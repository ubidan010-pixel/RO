#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_RAY_GAMEINTERFACE_H_
#include "rayman/gameplay/Ray_GameInterface.h"
#endif //_ITF_RAY_GAMEINTERFACE_H_

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_


#ifndef _ITF_FILECACHEMANAGER_H_
#include "core/AdaptersInterfaces/FileCacheManager.h"
#endif //_ITF_FILECACHEMANAGER_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_ATLASCOOKER_H_
#include "tools/plugins/CookerPlugin/atlasCooker.h"
#endif // _ITF_ATLASCOOKER_H_

#ifndef _ITF_FILEPACKER_H_
#include "tools/plugins/CookerPlugin/filePacker.h"
#endif //_ITF_FILEPACKER_H_

#ifndef _ITF_LOADING_SEQUENCE_COOKER_H_
#include "tools/plugins/CookerPlugin/LoadingSequenceCooker.h"
#endif //_ITF_LOADING_SEQUENCE_COOKER_H_

#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cookerWrapper.h"

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONLOADTRACK_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadTrack.h"
#endif //_ITF_ANIMATIONLOADTRACK_H_

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_


#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

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

#ifndef _ITF_ANIMATIONCOOKER_H_
#include    "tools/plugins/CookerPlugin/AnimationCooker.h"
#endif // _ITF_ANIMATIONCOOKER_H_

#ifndef _ITF_DEP_PARSER_H_
#include "tools/plugins/CookerPlugin/XmlDepParser.h"
#endif //_ITF_DEP_PARSER_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_GFXADPATER_COOKER_H
#include "adapters/GFXAdapter_Cooker/GFXAdapter_Cooker.h"
#endif //_ITF_GFXADPATER_COOKER_H

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

using namespace System;
using namespace System::IO;

#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>

#include <nvimage/Image.h>
#include <nvimage/DirectDrawSurface.h>

#include <nvtt/nvtt.h>

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FRIEZECOOKER_H_
#include "engine/display/frieze/FriezeCooker.h"
#endif //_ITF_FRIEZECOOKER_H_

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_SCENECOOKER_H_
#include "tools/plugins/CookerPlugin/sceneCooker.h"
#endif //_ITF_SCENECOOKER_H_

#ifndef _ITF_SOUNDMODIFIER_H_
#include "engine/sound/SoundModifier.h"
#endif //_ITF_SOUNDMODIFIER_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif  //_ITF_REWARDADAPTER_H_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_FONTCOOKER_H_
#include "tools/plugins/CookerPlugin/fontCooker.h"
#endif //_ITF_FONTCOOKER_H_

#ifndef _ITF_ENGINESIGNATURE_H_
#include "engine/signature/engineSignature.h"
#endif //_ITF_ENGINESIGNATURE_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

using namespace System::Runtime::InteropServices;


ITF::cookerFactory* g_pFactory = NULL ;

class SystemAdapter_Cooker : public ITF::SystemAdapter
{
public:

    ITF::String  userName()const  {return "UserCooker";}
    ITF::String  dateTime() const {return "";}

    void    getTime(ITF::Time & _time) const {
		time_t currentTime;
		::time(&currentTime);
		::tm* t = ::localtime(&currentTime);
		_time.m_year = 1900 + t->tm_year;       // tm years are since 1900
		_time.m_month = 1 + t->tm_mon;          // tm month: 0-11 we needs 1-12
		_time.m_day = t->tm_mday;
		_time.m_hour = t->tm_hour;
		_time.m_minute = t->tm_min;
		_time.m_second = t->tm_sec;

	};
    
    const  char*  getPlatformName() {return "UserCookerPC";}

    void   queryPerformanceCounter(LARGE_INTEGER *counter) const {};
    void   queryPerformanceFrequency(LARGE_INTEGER *counter) const {};

    ITF::i32 messageBox( const ITF::String& , const ITF::String& , ITF::u32 ,ITF::ITF_MESSAGEBOX_RESULT _result) 
	{
		return _result;
	}

    ITF::i32 messageBox( const ITF::String& _title, const ITF::String& _msg, ITF::u32 _type )
    {
        return -1;
    }
};


void addVersionExtension(const ITF::String& _szFilename,const ITF::String& _szPlatform, ITF::String& _szVersionExtension)
{
	g_pFactory->getExtensionVersion(_szFilename,_szPlatform,_szVersionExtension);

}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

cookerWrapper::cookerWrapper()
{
    m_instance = this;
#ifdef ITF_CATEGORY_MEMORY_ENABLE
    ITF::memoryStatsManager::disable();
#endif //ITF_CATEGORY_MEMORY_ENABLE

    ITF::Singletons::get().setResourceManager(newAlloc(mId_Resource,ITF::ResourceManager));
    ITF::Singletons::get().setFeedbackFXManager(newAlloc(mId_Resource,ITF::FeedbackFXManager));

    onProgress = nullptr;
    g_pFactory = new ITF::cookerFactory();
    g_pFactory->setCacheKey( utils::getITFString( Guid::NewGuid().ToString() ) );
    g_pFactory->init(ITF::bfalse);

    g_pFactory->registerCooker("act",ITF::actCooker::cookFile,ITF::actCooker::actDependenciesFile,ITF::Versioning::logicDatabaseCacheVer());
    g_pFactory->registerCooker("loa",ITF::actCooker::cookFile,ITF::actCooker::actDependenciesFile,ITF::Versioning::logicDatabaseCacheVer());
    g_pFactory->registerCooker("isg",ITF::actCooker::cookFile,ITF::actCooker::dependenciesFile,ITF::Versioning::logicDatabaseCacheVer());
    g_pFactory->registerCooker("imt",ITF::actCooker::cookFile,ITF::actCooker::dependenciesFile,ITF::Versioning::logicDatabaseCacheVer());
    g_pFactory->registerCooker("fxk",ITF::actCooker::cookFile,ITF::actCooker::dependenciesFile,ITF::Versioning::logicDatabaseCacheVer());
    g_pFactory->registerCooker("gmt",ITF::actCooker::cookFile,ITF::actCooker::dependenciesFile,ITF::Versioning::logicDatabaseCacheVer());
    g_pFactory->registerCooker("frt",ITF::actCooker::cookFile,ITF::actCooker::dependenciesFile,ITF::Versioning::logicDatabaseCacheVer());

    g_pFactory->registerCooker("wav",ITF::soundCooker::cookFile,ITF::soundCooker::dependencyFiles,ITF::Versioning::soundVer());
    g_pFactory->registerCooker("ptc",ITF::animSkeletonCooker::cookFile,NULL,ITF::Versioning::animSkeletonVer());
    g_pFactory->registerCooker("pbk",ITF::animPatchBankCooker::cookFile,NULL,ITF::Versioning::animPatchBankVer());
    g_pFactory->registerCooker("anm",ITF::animTrackCooker::cookFile,ITF::animTrackCooker::dependencyFiles,ITF::Versioning::animTrackVer());
    g_pFactory->registerCooker("skl",ITF::animSkeletonCooker::cookFile,NULL,ITF::Versioning::animSkeletonVer());
    g_pFactory->registerCooker("fcg",ITF::FriseConfigCooker::cookFile,ITF::XmlDepParser::fcgDependencyFiles,ITF::Versioning::friezeConfigVer());

    g_pFactory->registerCooker("tga",ITF::textureCooker::cookFile,ITF::textureCooker::dependencyFiles,ITF::Versioning::textureVer(),ITF::btrue);
    g_pFactory->registerCooker("png",ITF::textureCooker::cookFile,ITF::textureCooker::dependencyFiles,ITF::Versioning::textureVer(),ITF::btrue);

    g_pFactory->registerCooker("fnt",NULL,ITF::fontCooker::dependenciesFile,0,ITF::btrue);

    g_pFactory->registerCooker("isc",ITF::sceneCooker::cookFile,ITF::XmlDepParser::iscDependencyFiles,ITF::Versioning::sceneVer());
    g_pFactory->registerCooker("tsc",ITF::sceneCooker::cookFile,ITF::XmlDepParser::iscDependencyFiles,ITF::Versioning::sceneVer());

    g_pFactory->registerCooker("bse",ITF::LoadingSequenceCooker::cook,NULL,NULL);

    //g_pFactory->registerCooker("isc",NULL,ITF::XmlDepParser::iscDependencyFiles,ITF::Versioning::sceneVer());
    //g_pFactory->registerCooker("tsc",NULL,ITF::XmlDepParser::iscDependencyFiles,ITF::Versioning::sceneVer());

    g_pFactory->registerCooker("atl",ITF::atlasCooker::cookFile,NULL,ITF::Versioning::atlasVer());

    ITF::AnimSkeleton::m_funcCookerSkeleton                      = ITF::animSkeletonCooker::cookForced;
    ITF::AnimPatchBank::m_funcCookerPatchBank                    = ITF::animPatchBankCooker::cookForced;
    ITF::AnimTrack::m_funcCookerTrack                            = ITF::animTrackCooker::cookForced;
    //AnimTrack::m_funcCookerTrackForced                    = CookerPlugin::cookForcedAnimation;
    ITF::AnimationDependenciesMap::s_computeDependenciesCallback = ITF::AnimationLoadTrack::fillDependencies;

}

cookerWrapper::~cookerWrapper()
{
    destroy();
    m_instance = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::destroy()
{
    if (ITF::RESOURCE_MANAGER)
    {
        ITF::RESOURCE_MANAGER->stopAsync();
    }
    delete g_pFactory;
}

void callbackLogProxy(const ITF::String& _text)
{
    if (cookerWrapper::m_instance->onLog)
        cookerWrapper::m_instance->onLog( gcnew String((wchar_t*)_text.cStr()));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void cookerWrapper::init(String^ _directoryRoot,String^ _directoryApplication)//tools
{
    if (onProgress != nullptr)
        funcPtr = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(onProgress);

    ITF::ErrorHandler::get().setLogCookerCallback(callbackLogProxy);

    ITF::String root                 = utils::getITFString(_directoryRoot);
    m_DataRoot = _directoryRoot;
  
    ITF::String directoryApplication = utils::getITFString(_directoryApplication);

    ITF::cookerFactory::m_bForceVersioningCheck = true;

	ITF::FileServer* pFileServer = ITF::FILESERVER;
    pFileServer->setWorkingDepot(root);
	pFileServer->setApplicationDirectory(directoryApplication);
    pFileServer->rebuildCookDepot("PC");
   
    ITF::TemplateDatabase* pTemplateDataBase = ITF::TEMPLATEDATABASE;
    pTemplateDataBase->disableModeBinary();

	ITF::WarningManager::initialize();

    ITF::FileManager_ITF *pFileManagerITF = new  ITF::FileManager_ITF();
    ITF::Singletons::get().setFileManager(pFileManagerITF);
    pFileManagerITF->m_bBundleSupported = ITF::bfalse;
    pFileManagerITF->mountFileDevice("MainDrive", root, ITF::ePriorityFileDevice::ePriorityFileDevice_HDD);
    pFileManagerITF->setEnableTrackingRegistering( ITF::btrue);

    ITF::AliasManager::get().loadDefault();

	SystemAdapter_Cooker *pSystemAdapterCooker = new SystemAdapter_Cooker();
	pSystemAdapterCooker->setSilentMode(ITF::btrue);
    ITF::SINGLETONS.setSystemAdapter(pSystemAdapterCooker);

    ITF::LogicDataBase* logicDataBase =  ITF::LOGICDATABASE;

    logicDataBase->init("PC", ITF::bfalse);
    logicDataBase->enableBinarisation();
    
    //ITF::LOGICDATABASE->setRaiseError(ITF::bfalse);

    // we now have a project root, get its config
    //support rayman only (testing)
    ITF::GameInterface* gameInterface = new ITF::Ray_GameInterface();
    ITF::SINGLETONS.setGameInterface(gameInterface);

    ITF::Path gamemanagerIsg = ITF::String("gameconfig/gameconfig.isg");

    ITF::SINGLETONS.setGFXAdapter(new ITF::GFXAdapter_Cooker());

    ITF::SINGLETONS.setLocalisationManager( newAlloc( mId_Singleton, ITF::LocalisationManager ) );
    ITF::LOCALISATIONMANAGER->init(ITF::ITF_LANGUAGE_UNKNOWN, ITF::bfalse);
    ITF::SINGLETONS.setFeedbackFXManager(newAlloc(mId_Singleton, ITF::FeedbackFXManager));
    ITF::FEEDBACKFX_MANAGER->init();
    ITF::SINGLETONS.setRewardManager(newAlloc(mId_Singleton, ITF::RewardManager));

	//pFileServer->register_addVersionExtension(addVersionExtension);
    gameInterface->registerObjects();
    gameInterface->init();

    //compute the engine signature
    ITF::EngineSignature::computeEngineSignature();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::enableCacheFiles()
{
     ITF::FileCacheManager::get().useCache(ITF::btrue);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool cookerWrapper::isCookable(String^ file)
{
    ITF::String str_filename = utils::getITFString(file);
    return g_pFactory->isCookable(str_filename) == ITF::btrue;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::getPattern(String^% destination)
{
	ITF::String pattern = g_pFactory->getPattern();
	destination = gcnew String((wchar_t*)pattern.cStr());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::getCookedName(String^ source,String^% destination)
{
    ITF::String filename = ITF::FILESERVER->getCookedName(utils::getITFString(source));
    destination = gcnew String((wchar_t*)filename.cStr());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::getCookedNamePlatform(String^ source,String^ platform ,String^% destination)
{
    ITF::String filename = ITF::FILESERVER->getCookedNamePlatform(utils::getITFString(source),utils::getITFString(platform));
    destination = gcnew String((wchar_t*)filename.cStr());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::getCookedDirectory(String^ source,String^ platform,String^% destination)
{
	ITF::String directoryDst;
	ITF::FILESERVER->getDirectoryFromCookedFolderPlatform(directoryDst,utils::getITFString(source),utils::getITFString(platform));
	destination = gcnew String((wchar_t*)directoryDst.cStr());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool cookerWrapper::cook(String^ platform,String^ file)
{
    ITF::String str_filename = utils::getITFString(file);
    ITF::String str_platform = utils::getITFString(platform);

    return g_pFactory->directCook(str_platform,str_filename) == ITF::btrue;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::getTopLevelDependencies(String^ platform, String^ file, List<String^>^ _dependencies)
{
    ITF::String         str_filename = utils::getITFString(file);
    ITF::String         str_platform = utils::getITFString(platform);
    ITF::DepCollector   col;

    col.setDefaultFilters();
    g_pFactory->getDependencyFiles(str_filename, str_platform, col);

    ITF::DepCollection::NETIterator it = col.getPendings().getNetIterator();

    while ( it.haveNext() )
    {
        it.get( str_filename );
        _dependencies->Add( gcnew String( ( wchar_t* )str_filename.cStr() ) );
        it.next();
    }
}
void cookerWrapper::getDependencies(String^ platform, String^ file, List<String^>^ _dependencies)
{
    ITF::String         str_filename = utils::getITFString(file);
    ITF::String         str_platform = utils::getITFString(platform);
    ITF::DepCollector   col;

    col.setDefaultFilters();
    g_pFactory->getDependencyFiles(str_filename, str_platform, col);
    g_pFactory->resolveDependencyFiles(str_platform, col);

    ITF::DepCollection::NETIterator it = col.getProcessed().getNetIterator();

    while ( it.haveNext() )
    {
        it.get( str_filename );
        _dependencies->Add( gcnew String( ( wchar_t* )str_filename.cStr() ) );
        it.next();
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::getGameDependencies(String^ platform, List<String^>^ _dependencies)
{
    ITF::String         str_filename  = "common.alias";
    ITF::String         str_platform = utils::getITFString(platform);
    ITF::DepCollector   depCollector;
    ITF::DepCollection  patches;
    const ITF::AliasContainer& aliasContainer = ITF::AliasManager::get().getContainer();

    depCollector.add( str_filename );
    aliasContainer.getDependencies(depCollector);
    aliasContainer.getDependencies(depCollector, str_platform);
    depCollector.setDefaultFilters();
    g_pFactory->resolveDependencyFiles(str_platform, depCollector);

    ITF::DepCollection::NETIterator it = depCollector.getProcessed().getNetIterator();

    while ( it.haveNext() )
    {
        it.get( str_filename );
        _dependencies->Add( gcnew String( ( wchar_t* )str_filename.cStr() ) );
        it.next();
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool cookerWrapper::bundle(String^ _dstFilename,String^ platform,List<String^>^ _listFiles,int _version)
{
    ITF::String** mFiles = new ITF::String*[_listFiles->Count];
    ITF::String str_platform = utils::getITFString(platform);
    ITF::String str_destination = utils::getITFString(_dstFilename);

    for (int index =0;index< _listFiles->Count;index++)
    {
        mFiles[index] = new ITF::String(utils::getITFString(_listFiles[index]));
    }

    ITF::FilePacker::makeForDLL(utils::getITFString(m_DataRoot),str_destination,str_platform,_listFiles->Count,(const ITF::String**)mFiles,(ITF::callbackProgressBuild) funcPtr.ToPointer(),_version);

    for (int index =0;index< _listFiles->Count;index++)
    {
        SF_DEL(mFiles[index]);
    }

    SF_DEL_ARRAY(mFiles);
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool cookerWrapper::buildGlobalFat( String^ destinationFile, String^ platform, String^ bundleOutput, List< String^ >^ bundleFiles )
{
    ITF::String**   mFiles = new ITF::String*[bundleFiles->Count];
    ITF::String     str_platform = utils::getITFString( platform );
    ITF::String     str_destination = utils::getITFString( destinationFile );
    ITF::String     str_output = utils::getITFString( bundleOutput );

    for ( int index = 0; index < bundleFiles->Count; ++index )
        mFiles[index] = new ITF::String( utils::getITFString( bundleFiles[index] ) );

    ITF::FilePacker::makeGlobalFatForDll( utils::getITFString( m_DataRoot ), str_platform, str_destination, str_output, (const ITF::String**)mFiles, bundleFiles->Count );

    for ( int index = 0; index < bundleFiles->Count; ++index )
        SF_DEL( mFiles[index] );
    SF_DEL_ARRAY( mFiles );
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::setCacheKey( String^ key )
{
    g_pFactory->setCacheKey( utils::getITFString( key ) );
}

String^ cookerWrapper::getCacheKey()
{
    return gcnew String( ( wchar_t* )g_pFactory->getCacheKey().cStr() );
}

void cookerWrapper::addCookersOption(String^ option)
{
    ITF::FILESERVER->addCookersOption(utils::getITFString(option));
}

void cookerWrapper::removeCookersOption(String^ option)
{
    ITF::FILESERVER->removeCookersOption(utils::getITFString(option));
}

ITF::StringID::StringIdValueType cookerWrapper::getStringCRC( String^ str )
{
    ITF::String str_destination = utils::getITFString(str);

    return ITF::StringID::StrToCRC( str_destination.cStr() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void cookerWrapper::buildAtlasContainer(String^ dstFile,String^ platform,List<String^>^ _listFiles)
{
    ITF::String str_destination = utils::getITFString(dstFile);
    ITF::String str_platform    = utils::getITFString(platform);

    ITF_VECTOR<ITF::String> vListFiles;
    vListFiles.reserve(_listFiles->Count);

    for (int index = 0;index <_listFiles->Count;index++)
    {
        vListFiles.push_back(ITF::String(utils::getITFString(_listFiles[index])));
    }

    ITF::UVAtlasManager::get().build(str_destination,str_platform,vListFiles);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

String^ cookerWrapper::getAliasName(String^ _alias)
{
    ITF::String str_alias = utils::getITFString(_alias);
    ITF::String pathAlias = ITF::GETPATH_ALIAS(str_alias);    
    return gcnew String((wchar_t*)pathAlias.cStr());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

String^ cookerWrapper::getBundleName(String^ _name, String^ _platform)
{
    String^ fileName = _name + "_" + _platform + ".ipk";
    return fileName;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
