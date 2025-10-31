#include "precompiled_engine.h"

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Loading_Template)

    BEGIN_SERIALIZATION(Loading_Template)
        SERIALIZE_MEMBER("filename",m_filename);
        SERIALIZE_MEMBER("cameraz",m_cameraz);
    END_SERIALIZATION()

//--------------------------------------------------------------------------------------------------------------------------------------------------------

    bool PrefetchLoa( const Path& path, PrefetchFactory::Params& params )
    {
        return PrefetchTemplate< Loading_Template >( path, params );
    }

    LoadingScreen::~LoadingScreen()
    {
        ITF_ASSERT(!m_pWorld);
    }

    void LoadingScreen::destroy()
    {
        if(m_pWorld)
        {
            ITF_VERIFY(WORLD_MANAGER->deleteWorld(m_pWorld));
            m_pWorld = NULL;
        }
        if (!m_loadingScreenPath.isEmpty())
            TEMPLATEDATABASE->removeTemplateClient(m_loadingScreenPath.getStringID(), this);
    }

    void LoadingScreen::show()
    {
#ifdef ITF_WII
        if(!m_bVisible)
            SYSTEM_ADAPTER_WII->disableHomeButton();
#endif //ITF_WII
        m_bVisible = btrue;
    }

    void LoadingScreen::hide()
    {
#ifdef ITF_WII
        if(m_bVisible)
            SYSTEM_ADAPTER_WII->enableHomeButton();
#endif //ITF_WII
        m_bVisible = bfalse;
    }

    void LoadingScreen::update(f32 _elapsed)
    {
        ITF_ASSERT(m_bVisible);
     
        if(m_pWorld)
        {
            m_pWorld->startFrame();
            const BaseObjectList& objList = m_pWorld->getActiveObjects();
            ID_SERVER->getObjectListV2(objList);
            const u32 uObjCount = objList.m_solvedPointers.size();
            for (u32 index = 0; index < uObjCount; ++index)
            {
                Pickable* pObj = (Pickable*)objList.m_solvedPointers[index];
                ITF_ASSERT(pObj);
                pObj->set2DPos(CAMERA->getPos().truncateTo2D());
                pObj->update(_elapsed);
            }
        }
    }

    void LoadingScreen::draw()
    {
     
        GFX_ADAPTER->setCamera(CAMERA->getX(), CAMERA->getY(), m_LoadingTemplate->getCameraZ(), CAMERA);

        if(m_pWorld && m_pWorld->isPhysicalReady())
        {
            GFX_ADAPTER->clear( GFX_CLEAR_COLOR , 1.0f,1.0f,1.0f, 0);
            const BaseObjectList& objList = m_pWorld->getActiveObjects();
            ID_SERVER->getObjectListV2(objList);
            const u32 uObjCount = objList.m_solvedPointers.size();
            for (u32 index = 0;index < uObjCount; ++index)
            {
                Pickable* pObj = static_cast<Pickable*>(objList.m_solvedPointers[index]);
                pObj->draw();
                pObj->draw2D(); // for saving icon
            }
        }
        else
        {
            GFX_ADAPTER->clear( GFX_CLEAR_COLOR , 0.0f,0.0f,0.0f, 0);   
        }
    }

    void LoadingScreen::load( const Path& loadingScreenPath )
    {
        ITF_ASSERT( m_loadingScreenPath.isEmpty() );
        m_loadingScreenPath = loadingScreenPath;
        TEMPLATEDATABASE->addTemplateClient(m_loadingScreenPath.getStringID(), this);
        m_LoadingTemplate = TEMPLATEDATABASE->getTemplate<Loading_Template>(this, m_loadingScreenPath);
        if (!m_LoadingTemplate)
        {
            ITF_FATAL_ERROR("Couldn't load loadscreen act file: %s", m_loadingScreenPath.getString8().cStr());
        }
        LoadInfo loadInfo;
        loadInfo.m_mapPath = m_LoadingTemplate->getFilename();
        loadInfo.m_disconnectFromCells = btrue;
        loadInfo.m_forceLoadResources = btrue;
        loadInfo.m_disconnectFromCells = btrue;

        ObjectRef worldRef = WORLD_MANAGER->newWorld();
        m_pWorld = static_cast<World*>(GETOBJECT(worldRef));
        ITF_ASSERT(m_pWorld);
        WORLD_MANAGER->loadWorld(worldRef, loadInfo);
        m_pWorld->activeAll();
        m_pWorld->forceLoadResourcesAndDisableUnload();
    }

    ObjectRef LoadingScreen::getRef() const
    {
        if(!m_pWorld)
            return ObjectRef::InvalidRef;
        return m_pWorld->getRef();
    }

} // namespace ITF
