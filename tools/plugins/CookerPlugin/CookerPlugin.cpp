// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_COOKERPLUGIN_H_
#include "tools/plugins/CookerPlugin/CookerPlugin.h"
#endif //_ITF_COOKERPLUGIN_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_BLOBUTILS_H_
#include "core/blobUtils.h"
#endif //_ITF_BLOBUTILS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_TEXTURECOOKER_H_
#include "tools/plugins/CookerPlugin/textureCooker.h"
#endif //_ITF_TEXTURECOOKER_H_

#ifndef _ITF_TEXTURECOOKERHRES_H_
#include "tools/plugins/CookerPlugin/textureCookerHRes.h"
#endif //_ITF_TEXTURECOOKERHRES_H_

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

#ifndef _ITF_ATLASCOOKER_H_
#include    "tools/plugins/CookerPlugin/atlasCooker.h"
#endif // _ITF_ATLASCOOKER_H_

#ifndef _ITF_DEP_PARSER_H_
#include "tools/plugins/CookerPlugin/XmlDepParser.h"
#endif //_ITF_DEP_PARSER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_VLDMANAGER_H_
#include "core/vldManager.h"
#endif //_ITF_VLDMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FRIEZECOOKER_H_
#include "engine/display/frieze/FriezeCooker.h"
#endif //_ITF_FRIEZECOOKER_H_

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

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATIONLOADTRACK_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadTrack.h"
#endif //_ITF_ANIMATIONLOADTRACK_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_LOADING_SEQUENCE_COOKER_H_
#include "tools/plugins/CookerPlugin/LoadingSequenceCooker.h"
#endif //_ITF_LOADING_SEQUENCE_COOKER_H_

namespace ITF
{

CookerPlugin*    CookerPlugin::m_pInstance = NULL;


CookerPlugin::CookerPlugin()
{
    Synchronize::createCriticalSection(&m_cs);
    m_showDebug = bfalse;
    m_pInstance = this;
    m_cookFactory = newAlloc(mId_System,cookerFactory());
    m_cookFactory->init();

	m_platform = FILESERVER->getPlatform();

#ifdef MODE_HRESTEXTURE_SUPPORTED
    m_cookFactory->registerCookerHRes("png",textureCookerHRes::cookFile,Versioning::textureVer());
    m_cookFactory->registerCookerHRes("tga",textureCookerHRes::cookFile,Versioning::textureVer());
#endif //MODE_HRESTEXTURE_SUPPORTED

    m_cookFactory->registerCooker("act",actCooker::cookFile,actCooker::actDependenciesFile,Versioning::logicDatabaseCacheVer());
    m_cookFactory->registerCooker("loa",actCooker::cookFile,actCooker::actDependenciesFile,Versioning::logicDatabaseCacheVer());
	m_cookFactory->registerCooker("gmt",actCooker::cookFile,actCooker::dependenciesFile,Versioning::logicDatabaseCacheVer());
    m_cookFactory->registerCooker("isg",actCooker::cookFile,actCooker::dependenciesFile,Versioning::logicDatabaseCacheVer());
    m_cookFactory->registerCooker("imt",actCooker::cookFile,actCooker::dependenciesFile,Versioning::logicDatabaseCacheVer());
	m_cookFactory->registerCooker("fxk",actCooker::cookFile,actCooker::dependenciesFile,Versioning::logicDatabaseCacheVer());
	m_cookFactory->registerCooker("atl",atlasCooker::cookFile,NULL,Versioning::atlasVer());
    // m_cookFactory->registerCooker("wav",soundCooker::cookFile,soundCooker::dependencyFiles,Versioning::soundVer());
    m_cookFactory->registerCooker("ptc",animSkeletonCooker::cookFile,NULL,Versioning::animSkeletonVer());
    m_cookFactory->registerCooker("pbk",animPatchBankCooker::cookFile,NULL,Versioning::animPatchBankVer());
    m_cookFactory->registerCooker("anm",animTrackCooker::cookFile,animTrackCooker::dependencyFiles,Versioning::animTrackVer());
    m_cookFactory->registerCooker("skl",animSkeletonCooker::cookFile,NULL,Versioning::animSkeletonVer());
    m_cookFactory->registerCooker("fcg",FriseConfigCooker::cookFile,XmlDepParser::fcgDependencyFiles,Versioning::friezeConfigVer());
    m_cookFactory->registerCooker("isc",sceneCooker::cookFile,XmlDepParser::iscDependencyFiles,Versioning::sceneVer());
    m_cookFactory->registerCooker("tsc",NULL,XmlDepParser::iscDependencyFiles,Versioning::sceneVer());
    m_cookFactory->registerCooker("isd",NULL,XmlDepParser::isdDependencyFiles,NULL);
    m_cookFactory->registerCooker("bse",LoadingSequenceCooker::cook,NULL,NULL);

    m_cookFactory->registerCooker("tga",textureCooker::cookFile,textureCooker::dependencyFiles,Versioning::textureVer(),btrue);
    m_cookFactory->registerCooker("png",textureCooker::cookFile,textureCooker::dependencyFiles,Versioning::textureVer(),btrue);

    //options 
    soundCooker::useBatch = bfalse;

    AnimSkeleton::m_funcCookerSkeleton                      = animSkeletonCooker::cookForced;
    AnimPatchBank::m_funcCookerPatchBank                    = animPatchBankCooker::cookForced;
    AnimTrack::m_funcCookerTrack                            = animTrackCooker::cookForced;
    AnimationDependenciesMap::s_computeDependenciesCallback = AnimationLoadTrack::fillDependencies;

    if (RESOURCE_MANAGER)
    {
        RESOURCE_MANAGER->registerHResCooking(CookerPlugin::cookHRes);
        RESOURCE_MANAGER->registerCooking(CookerPlugin::requestCooking);
        RESOURCE_MANAGER->registerNeededCooking(CookerPlugin::requestNeededCooking);
    }

    if (FILESERVER)
        FILESERVER->register_addVersionExtension(CookerPlugin::addVersionExtension);
}

void CookerPlugin::addVersionExtension(const String& _szFilename,const String& _szPlatform,String& _szVersionExtension)
{
    if (m_pInstance)
        m_pInstance->m_cookFactory->getExtensionVersion(_szFilename,_szPlatform,_szVersionExtension);
}

CookerPlugin::~CookerPlugin()
{
    Synchronize::destroyCriticalSection(&m_cs);
    m_pInstance = NULL;
    SF_DEL(m_cookFactory);
}

void CookerPlugin::cookHRes(const String& _filename)
{
 return m_pInstance->cookHResInternal(FilePath::normalizePath(_filename));
}

void CookerPlugin::cookHResInternal(const String& _filename)
{
    m_cookFactory->cookHRes(_filename);
}

void CookerPlugin::requestCooking(const String& _filename)
{
    return m_pInstance->requestCookingInternal(FilePath::normalizePath(_filename));
}

bbool CookerPlugin::requestNeededCooking(const String& _filename,bbool _HResVersion)
{
    return m_pInstance->requestNeededCookingInternal(FilePath::normalizePath(_filename),_HResVersion);
}


bbool CookerPlugin::requestNeededCookingInternal(const String& _filename,bbool _HResVersion)
{
    
#ifdef   MODE_HRESTEXTURE_SUPPORTED
    if (_HResVersion)
    {
        return m_cookFactory->fileNeedsCook(FILESERVER->getHResVersionName(_filename),FILESERVER->getCookedHResVersionName(_filename),	m_platform);
    }
#endif //MODE_HRESTEXTURE_SUPPORTED
    
    return m_cookFactory->fileNeedsCook(_filename,FILESERVER->getCookedName(_filename),	m_platform);
}

void CookerPlugin::requestCookingInternal(const String& _filename)
{
    //cook now!
    csAutoLock cs(m_cs);

    m_cookFactory->cook(m_platform,_filename);
}

void  CookerPlugin::getCookFile(String& _result)
{
    m_cookFactory->getCurrentlyCookedFileName(_result);
}


const bbool CookerPlugin::isCooking() const
{
    return m_cookFactory->isCooking();
}

void CookerPlugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
    String name, val;

    _blob.extractString(name);

    if ( name == "cook" )
    {
        LOG("COOK ORDER RECEIVE\n");

        u32 countFile = 0;
        countFile = _blob.extractInt32();
        String filename;
        for (u32 u=0;u<countFile;++u)
        {
          _blob.extractString(filename);

          LOG("cook :%S",filename.cStr());
          requestCooking(filename);
        }

       
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString("idle");
        PLUGINGATEWAY->sendPacket(blob, _peer, _senderID);
    }
}

bbool CookerPlugin::cudaIsEnable()
{
    return textureCooker::cudaIsEnable();
}

void CookerPlugin::update()
{
    if (m_cookFactory)
        m_cookFactory->update();

#ifndef ITF_FINAL
    if (m_showDebug)
    {
        char txt[256];
        sprintf_s(txt,sizeof(txt),
            "Cook time: %.3Lf s", m_cookFactory->getGlobalCookingTime());
        GFX_ADAPTER->drawDBGText(String(txt), 10.f, 50.0f);
    }
#endif //ITF_FINAL

    if (m_editorSenderID == 0)
        return;

    
}

void CookerPlugin::onKey( i32 _key, InputAdapter::PressStatus _keyStatus )
{
    if (_keyStatus == InputAdapter::Pressed)
    {
        switch (_key)
        {
        case 'K':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                toggleShowDebug();
            break;

        }
    }
}
} // namespace ITF
