#include "precompiled_engine.h"

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef SERIALIZEROBJECTBINARY_H 
#include "core/serializer/ZSerializerObjectBinary.h"
#endif //SERIALIZEROBJECTBINARY_H

#ifndef _ITF_FILETRACKER_H_
#include "engine/file/FileTracker.h"
#endif //_ITF_FILETRACKER_H_

namespace ITF
{

//------------------------------------------------------------------------------------------------------------------------------

bbool readSceneBinary(Scene* _pScene, const LoadInfo& _info)
{
    PathString_t pathName;
    _info.m_mapPath.getString(pathName);
    String filenameCooked   = FILESERVER->getCookedName(pathName);
    File* readFile = FILEMANAGER->openFile(filenameCooked, ITF_FILE_ATTR_READ);
    if (readFile)
    {
        const u32 length = (u32)readFile->getLength();
        u8* pBuffer = newAlloc(mId_Temporary, u8[length]);

        readFile->read(pBuffer, length);
        FILEMANAGER->closeFile(readFile);
        readFile = NULL;

        ArchiveMemory am(pBuffer, length);
        CSerializerObjectBinary serializeBin(&am);
        _pScene->Serialize(&serializeBin, ESerialize_Data_Load);

        SF_DEL_ARRAY(pBuffer);

        return   btrue;
    }

    return bfalse;
}

bbool XMLAll::readMapFolder(Scene* _pScene, const LoadInfo& _info, bbool _bcookMode  /*bfalse*/)
{
    FILETRACKERSCOPE(_info.m_mapPath.getString());
    const bbool bBinary = _bcookMode ? _info.m_bBinary : FILESERVER->isBinaryMode();

    bbool result = bfalse;
    if (bBinary)
    {
        result = readSceneBinary(_pScene,_info);
    }
#ifdef ITF_SUPPORT_XML
    else
    {
        result =  XMLAll::readScene(_pScene, _info.m_mapPath);
    }
#endif  //ITF_SUPPORT_XML

    if(result)
    {
        _pScene->setPath(_info.m_mapPath);
    }

    return result;
}



#ifdef ITF_SUPPORT_XML

//------------------------------------------------------------------------------------------------------------------------------

u32 XMLAll::readCoreConfig (XMLConfig* _config, const String& _szXMLFile)
{
    PugiXMLWrap XML;

    // set default values
    _config->m_showMouseCursor = btrue;

    // read config file
  
    if (!XML.openFile(_szXMLFile, bfalse,PugiXMLWrap::RaiseError))
        return 0;

    _config->m_screenWidth      = XML.readIntAttribute("Screen/Width", 800);
    _config->m_screenHeight     = XML.readIntAttribute("Screen/Height", 600);
    _config->m_fullScreen       = XML.readIntAttribute("Screen/FullScreen", 0);
    _config->m_waitVBL          = XML.readIntAttribute("Screen/WaitVBL", 1);
    _config->m_disableRendering = XML.readIntAttribute("Screen/DisableRendering", 0);
    _config->m_logTimings       = XML.readIntAttribute("Screen/LogTimings", 0);
    _config->m_BGColor[0]       = XML.readFloatAttribute("BGColor/R", 0.1f);
    _config->m_BGColor[1]       = XML.readFloatAttribute("BGColor/G", 0.2f);
    _config->m_BGColor[2]       = XML.readFloatAttribute("BGColor/B", 0.4f);
    _config->m_zMin             = XML.readFloatAttribute("Space/ZMin", 0.f);
    _config->m_zMax             = XML.readFloatAttribute("Space/ZMax", 100.f);
    _config->m_urlServer        = XML.readAttribute("Server/url", "127.0.0.1");
    _config->m_PortServer       = XML.readIntAttribute("Server/Port", 3456);
    _config->m_szMapFolder      = XML.readAttribute("DB/folder", "error", PugiXMLWrap::RaiseError);
    _config->m_BGMusic          = XML.readAttribute("BGMusic/music");
    _config->m_editorZoomFactor = XML.readFloatAttribute("Editor/ZoomFactor", 0.2f);
    _config->m_editorZoomFactor = std::max(_config->m_editorZoomFactor, 0.05f);
    _config->m_editorZoomFactor = std::min(_config->m_editorZoomFactor, 0.95f);
    _config->m_editorMoveDepthFactor = XML.readFloatAttribute("Editor/DepthFactor", 0.1f);
    _config->m_editorMoveDepthFactor = std::max(_config->m_editorMoveDepthFactor, 0.01f);
    _config->m_editorMoveDepthFactor = std::min(_config->m_editorMoveDepthFactor, 0.95f);
    _config->m_debugVideoCapture = XML.readIntAttribute("Debug/VideoCapture", 0);
    _config->m_audioVideoCapture = XML.readIntAttribute("Debug/AudioCapture", 0);
#ifdef ITF_WINDOWS
    _config->m_XInputDevicesOnly = XML.readIntAttribute("PC/XInputDevicesOnly", 1);
#endif // ITF_WINDOWS

#ifdef ITF_SUPPORT_NETWORKSERVICES
    _config->m_enableNetwork = XML.readIntAttribute("NetworkServices/enableNetwork", _config->m_enableNetwork);
#endif // ITF_SUPPORT_NETWORKSERVICES
#ifdef ITF_SUPPORT_UPLAY
    _config->m_enableUplay = XML.readIntAttribute("NetworkServices/enableUplay", _config->m_enableUplay);
#endif // ITF_SUPPORT_UPLAY
#ifdef ITF_SUPPORT_EDITOR
    _config->m_SourceControlProvider    = XML.readAttribute("Editor/SCM","");
#endif // ITF_SUPPORT_EDITOR
    _config->m_noWarnings               = XML.readIntAttribute("Debug/nowarnings",_config->m_noWarnings);
    _config->m_HResDirectory            = XML.readAttribute("HRes/Directory","");

#ifndef ITF_FINAL
    _config->m_ProfileFPS               = XML.readFloatAttribute("Profile/CaptureBelowFPS", 40);
#endif // ITF_FINAL


#if defined(EXPORT_ASSERTS)
    strcpy_s(AssertManager::assertServerInsertPage, 512, XML.readAttribute("Server/AssertDB", ""));
#endif // EXPORT_ASSERTS
    XML.exitElement();
    
    return 1;
}

u32 XMLAll::readProjectConfig (XMLConfig* _config, const String& _szXMLFile)
{
    PugiXMLWrap XML;

    // read config file

    if (!XML.openFile(_szXMLFile, bfalse,PugiXMLWrap::RaiseError))
        return 0;

    _config->m_projectID = XML.readAttribute("Project/ID", "Rayman");
    _config->m_allowDebug = XML.readIntAttribute("Game/AllowDebug", bfalse);
    if (_config->m_allowDebug)
    {
        _config->m_startWithCheatsOn = XML.readIntAttribute("Game/StartWithCheatsOn", bfalse);
    }
    else
    {
#ifdef ITF_DEMO
        _config->m_startWithCheatsOn = XML.readIntAttribute("Game/StartWithCheatsOn", bfalse);
#else
        _config->m_startWithCheatsOn = bfalse;
#endif
    }
    _config->m_pressStartToJoin = XML.readIntAttribute("Game/PressStartToJoin", bfalse);
    _config->m_minimalEngineVersion = XML.readIntAttribute("Engine/minVersion", 0);


    if (XML.enterElement("ActorFolder"))
    {    
        do
        {
            ActorLoadPath loadPath;
            loadPath.m_path = XML.readAttribute("path");
            if (!loadPath.m_path.isEmpty())
            {
                loadPath.m_path = FilePath::normalizePath(loadPath.m_path);
                StringConverter pth(loadPath.m_path);
                u32 len = u32(strlen(pth.getChar()));
                if (len)
                {
                    if ((pth.getChar())[len-1] != '/')
                        loadPath.m_path += '/';
                    loadPath.m_recursive = (bbool)XML.readIntAttribute("recursive", 0);                
                    _config->m_actorLoadPaths.push_back(loadPath);
                }
            }
        } while (XML.nextSameElement());
    }

    if (XML.enterElement("ActorAlias"))
    {    
        do
        {
            ActorAlias alias;
            alias.m_oldName = XML.readAttribute("oldName");
            alias.m_newName = XML.readAttribute("newName");
            alias.m_oldName.toLower();
            alias.m_newName.toLower();
            _config->m_actorAliases.push_back(alias);
        } while (XML.nextSameElement());
    }


    XML.exitElement();

    return 1;
}

void   XMLAll::loadSceneAfterFxBank (Scene* _pScene, const String& _afterFxFile)
{
    PugiXMLWrap XML;

    if (!FILEMANAGER->fileExists(_afterFxFile))
        return;

    if (!XML.openFile(_afterFxFile, bfalse, PugiXMLWrap::LogWarning))
        return;

    if (XML.enterElement("AfterFxGroup", PugiXMLWrap::LogWarning))
    {    
        do
        {            
            _pScene->getWorld()->CreateFxManager();
 
            AFX_Group* newAFXGroup = newAlloc(mId_GfxAdapter, AFX_Group);

            newAFXGroup->setDepth(XML.readFloatAttribute("depth", 0.f));
            newAFXGroup->setBeginRank(XML.readFloatAttribute("startrank", -1000.f));
            newAFXGroup->setRenderRank(XML.readFloatAttribute("renderrank", 1000.f));
            newAFXGroup->SetRenderInTarget(XML.readIntAttribute("renderintarget", 1));
            newAFXGroup->SetFinalBlend(XML.readIntAttribute("finalblend", 1));

            if (XML.enterElement("AfterFx"))
            {    
                do
                {
                    AFTERFX_Type type;
                    String afxName = XML.readAttribute("fxtype", "null");
                    type = AFTERFX::getTypeByName(afxName);

                    if (type != AFX_None)
                    {
                        AFTERFX *newAFX = newAlloc(mId_GfxAdapter, AFTERFX);
                        
                        const char *texName = "";
                        String8 str;
                        for (u32 i = 0; i < 4;i++)
                        {
                            str.setTextFormat("customTex%d", i+1);
                            texName = XML.readAttribute(str.cStr() , "");
                        
                            if ( texName[0] )
                                 newAFX->LoadTexture(Path(texName), i);
                        }

                        newAFX->loadResources();

                        newAFX->setType(type);
                        newAFX->setParamfAt(0, XML.readFloatAttribute("paramf1", 1.f));
                        newAFX->setParamfAt(1, XML.readFloatAttribute("paramf2", 1.f));
                        newAFX->setParamfAt(2, XML.readFloatAttribute("paramf3", 1.f));
                        newAFX->setParamfAt(3, XML.readFloatAttribute("paramf4", 1.f));
                        newAFX->setParamfAt(4, XML.readFloatAttribute("paramf5", 1.f));
                        newAFX->setParamfAt(5, XML.readFloatAttribute("paramf6", 1.f));
                        newAFX->setParamfAt(6, XML.readFloatAttribute("paramf7", 1.f));
                        newAFX->setParamfAt(7, XML.readFloatAttribute("paramf8", 1.f));
                        newAFX->setParamiAt(0, XML.readIntAttribute("parami1", 0));
                        newAFX->setParamiAt(1, XML.readIntAttribute("parami2", 0));
                        newAFX->setParamiAt(2, XML.readIntAttribute("parami3", 0));
                        newAFX->setParamiAt(3, XML.readIntAttribute("parami4", 0));
                        
                        newAFXGroup->addAFXtoGroup(newAFX);
                    }
                    
                
                } while (XML.nextSameElement());
            }
                    
            _pScene->getWorld()->getAFXManager()->addFxGroup(newAFXGroup);

        } while (XML.nextSameElement());
    }
}
#endif //ITF_SUPPORT_XML

} // namespace ITF

