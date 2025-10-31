#include "precompiled_editor.h"

#include <algorithm>

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_AUTOSAVEMANAGER_H_
#include "editor/AutoSaveManager.h"
#endif //_ITF_AUTOSAVEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

namespace ITF
{

    //----------------------------------------------------------------------------------------------------------------------------------

    struct fileCompareDate
    {

        fileCompareDate():m_filename(NULL),m_date(0) {}
        fileCompareDate(String* _Filename,u64 _date):m_filename(_Filename),m_date(_date)
        {

        };
        String* m_filename;
        u64     m_date;
    };

      
    static  bool        compareDateFiles(fileCompareDate& _p1, fileCompareDate& _p2) { return _p1.m_date > _p2.m_date; }

    void AutoSaveManager::init(i32 _autosavetime)
    {
#ifdef ITF_WINDOWS
        if (FILEMANAGER->fileExists("autosaveenable"))//force enable autosave for any reason
        {
            m_bEnable = btrue;
        }
        else
        if (FILEMANAGER->fileExists("autosavedisable"))//force disable autosave for any reason
        {
            m_bEnable = bfalse;
        }
        else
        {
            switch (_autosavetime)
            {
                case -1:
                     m_bEnable = bfalse;
                    break;
                case 0:
                      m_SaveTimeDelay = TimeForAutoSave;
                    break;
                default:
                      m_bEnable = btrue;  
                      m_SaveTimeDelay = (f32) _autosavetime;
                    break;
            }
        }

        String autoSave = "AutoSave/";

        if(FILEMANAGER->TranslatePath(m_szDirectory, autoSave))
            Directory::create(m_szDirectory);

        ITF_VECTOR<String> files;
        Directory::searchFiles(files,m_szDirectory,"*.isc",bfalse);
        const int maxFilesKeep = 200;
        if (files.size()>maxFilesKeep)
        {
            //start cleaning :)
            ITF_VECTOR<fileCompareDate> vFileCompare;
            vFileCompare.reserve(files.size());
            for (ITF_VECTOR<String>::iterator iter = files.begin();iter!=files.end();iter++)
            {
                FilePath::normPath(*iter);
                FilePath::transformToRelative(*iter);
                u64 dateFile = FILEMANAGER->getLastTimeWriteAccess(*iter);
                vFileCompare.push_back(fileCompareDate(&(*iter),dateFile));
            }

            std::sort(vFileCompare.begin(), vFileCompare.end(), compareDateFiles);

            //keep only the first maxFilesKeep :)

            for (u32 index=maxFilesKeep;index<vFileCompare.size();++index)
            {
                FILEMANAGER->deleteFile(*vFileCompare[index].m_filename);
            }

        }
#endif //ITF_WINDOWS
    }


    //----------------------------------------------------------------------------------------------------------------------------------
    // Every m_SaveTimeDelay we save a backup all scenes in the current world
    //----------------------------------------------------------------------------------------------------------------------------------
    void AutoSaveManager::update(f32 _dt)
    {
        if (!m_bEnable)
            return;

        m_CurrentTime += _dt;
       
        if (CURRENTWORLD && m_CurrentTime >= m_SaveTimeDelay)
        {   
            m_CurrentTime = 0.0f;
            const f32 rs = (f32)(SYSTEM_ADAPTER->getTime());

            SafeArray<World*> worldListToSave;
            worldListToSave.push_back(CURRENTWORLD);

            for(u32 iTab = 0; iTab < EDITOR->getTabsCount(); ++iTab)
            {
                const TabEditInfo& infos = EDITOR->getTabAt(iTab);

                if(World* pWorld = WORLD_MANAGER->getWorldFromUniqueName(infos.m_worldUniqueName))
                    if(worldListToSave.find(pWorld) == -1)
                        worldListToSave.push_back(pWorld);
            }

            SafeArray<StringID> alreadySavedPathCRC; // Used to save a scene once
            String sceneRelPath;

            for(u32 iWorld = 0; iWorld < worldListToSave.size(); ++iWorld)
            {
                World* pWorld = worldListToSave[iWorld];

                for(u32 i = 0; i < pWorld->getSceneCount(); ++i)
                {
                    Scene* pScene = pWorld->getSceneAt(i);
                    ITF_ASSERT(pScene);
                    if(!pScene)
                    {
                        LOG("Current world contains a deleted scene !");
                        continue;
                    }

                    // Do not save groups
                    if(SubSceneActor* pSSA = pScene->getSubSceneActorHolder())
                        if(pSSA->getEmbedScene())
                            continue;

                    pScene->getPath().getString(sceneRelPath);
                    FilePath::normPath(sceneRelPath);

                    // Skip already saved scenes
                    if(alreadySavedPathCRC.find(sceneRelPath) >= 0)
                        continue;

                    TabEditInfo infos;
                    infos.m_worldUniqueName = pWorld->getUniqueName();

#if !defined(ITF_CONSOLE_FINAL)
                    String savefilename = m_szDirectory;
                    String extension = FilePath::getExtension(sceneRelPath);
                    String finalName = FilePath::getFilenameWithoutExtension(sceneRelPath) + "_" + SYSTEM_ADAPTER->dateTime() + "." + extension;
                    savefilename += finalName;

                    EDITOR->fillAssociatedTabList(pScene);
                    if(XMLAll::saveScene(savefilename, pScene, bfalse))
                    {
                        alreadySavedPathCRC.push_back(sceneRelPath);
                    }
                    else
                    {
                        ITF_ASSERT_MSG(0, "Failed to save a scene to xml");
                    }
#endif
                }
            }
   
            if (DEBUGINFO)
            {
                const f32 timetosave = (f32)(SYSTEM_ADAPTER->getTime()-rs);
                String displayMsg;
                displayMsg.setTextFormat("Autosave done in :%0.02f seconds",timetosave);
                DEBUGINFO->addDebugInfoDisplay(displayMsg,5.0f);
            }
        }
    }

//------------------------------------------------------------------------------------------------------------------------
} //namespace ITF
