// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef ITF_FINAL

#ifndef _ITF_TESTISCPLUGIN_H_
#include "Plugin_TestIsc.h"
#endif //_ITF_TESTISCPLUGIN_H_

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

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_TESTMANAGER_H_
#include "engine/test/testmanager.h"
#endif //_ITF_TESTMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILE_MANAGER_LOGGER_ITF_H_
#include "adapters/FileManager_ITF/FileManagerLogger_ITF.h"
#endif //_ITF_FILE_MANAGER_LOGGER_ITF_H_

using namespace std;

namespace ITF
{
    extern u32 resourcePhysicalLifeTime;

    Plugin_TestISC::Plugin_TestISC()
    : _requestExplore( false )
    {
		_peer = NULL;
        _senderID=0;
    }

    Plugin_TestISC::~Plugin_TestISC()
    {
   
    }

    void    Plugin_TestISC::update()
    {
        if ( _requestExplore && !CameraRunStateTest::isRunning() )
        {
            ITF::Blob blobToSend( ITF::BLOB_EXTRACT_AND_WRITE_ENABLED );

            _requestExplore = false;
            blobToSend.pushString( getPluginName() );
            blobToSend.pushString( CameraRunStateTest::isAborted() ? "aborted" : "explored" );
            PLUGINGATEWAY->sendPacket( blobToSend, _peer, _senderID );
        }
    }


    void Plugin_TestISC::onLog(const String& /*_szExeName*/,const String& /*_szLogText*/)
    {
    }

    void    Plugin_TestISC::receive(Blob& _blob,const NETPeer* peer, u32 senderID)
    {
		String name, val;

        _blob.extractString(name);
	    
	    if(!_peer)
	    {
	    	_peer=peer;
	    	_senderID=senderID;
	    }
        if ( name == "loadisc" )
        {
	    	SYSTEM_ADAPTER->setSilentMode(btrue);
	    	SYSTEM_ADAPTER->SetProcessDumpOnCrash(bfalse);
	    	_blob.extractString(worldpath);
	    	worldpath.toLower();
            ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
            blobToSend.pushString(getPluginName());
            blobToSend.pushString("loadinginprogress");


            PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);	
            SCENE_MANAGER->addListener(this);
            LoadInfo loadInfo;
            loadInfo.m_mapPath          = worldpath;
            loadInfo.m_forceLoadResources   = btrue;
            loadInfo.m_disconnectFromCells  = btrue;
            loadInfo.m_bAsyncLoad           = btrue;

            GAMEMANAGER->loadGameplayMap(worldpath, btrue, bfalse);
        }
	    else if(name == "ping")
	    {
	    	ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
	    	blobToSend.pushString(getPluginName());
	    	blobToSend.pushString("pong");
	    	PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);	
	    }
        else if ( name == "init" )
        {
            resourcePhysicalLifeTime = 0;

            ITF_ASSERT_CRASH( FILEMANAGER != NULL, "Invalid file manager" );
#ifndef ITF_FINAL
            FILEMANAGER->getLogger().Flush();
#endif

            ITF::Blob blobToSend( ITF::BLOB_EXTRACT_AND_WRITE_ENABLED );
        }
        else if ( name == "explore" )
        {
            CameraRunStateTest::m_camSpeedPercent = _blob.extractFloat32();
            CameraRunStateTest::startNewTest( bfalse, bfalse );
            _requestExplore = true;
        }
        else if ( name == "list_scene_resources" )
        {
            SafeArray< ActorRef >   actors;
            SafeArray< ObjectRef >  frizes;
            {
                Scene*              rootScene = CURRENTWORLD->getRootScene();
                const PickableList& actorList = rootScene->getActors();
                const PickableList& frizeList = rootScene->getFrises();

                for ( u32 i = 0; i < actorList.size(); ++i )
                {
                    Actor* actor = static_cast< Actor* >( actorList[i] );

                    if ( actor != NULL )
                        actor->registerContainedPickables( actors, frizes );
                }
                for ( u32 i = 0; i < frizeList.size(); ++i )
                {
                    Frise* frize = reinterpret_cast< Frise* >( frizeList[i] );

                    if ( frize != NULL )
                        frizes.push_back( frize->getRef() );
                }
            }
            ListResources( actors, frizes );
        }
    }


	void Plugin_TestISC::onSceneAsyncLoadFinished(const Scene* _pScene)
	{
		if(_peer)
		{	
			if(_pScene->getPath().getString() == worldpath)		
			{
				ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
				blobToSend.pushString(getPluginName());
				blobToSend.pushString("sceneloaded");
				PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);
				// test save on premake
			}
		}
	}

	void Plugin_TestISC::OnFileMissing(const String& _actorFileName, Path& _luaPath )
	{

	}

	void Plugin_TestISC::OnRefMissing(const char* filename,const String& reference)
	{

	}

	void Plugin_TestISC::OnDataError(const String& _str,Pickable* pickable)
	{
		if(_peer)
		{
			ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
			blobToSend.pushString(getPluginName());
			blobToSend.pushString("actordataerror");
			blobToSend.pushString(_str);
			PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);
		}
	}

	void Plugin_TestISC::OnMapLoaded(Scene* rootScene)
	{
		if(_peer)
		{
			if(rootScene->getPath().getString() == worldpath)		
			{
				ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
				blobToSend.pushString(getPluginName());
				blobToSend.pushString("worldisrunning");
				PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);
				SCENE_MANAGER->removeListener(this);
			}
		}
		
	}

	void Plugin_TestISC::OnFatalError(const char* msg)
	{
		if(_peer)
		{
			ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
			blobToSend.pushString(getPluginName());
			blobToSend.pushString("fatalerror");
			blobToSend.pushString(msg);
            PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);
		}
	}

	void Plugin_TestISC::OnWarning(const char* msg)
	{
		if(_peer)
		{
			ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
			blobToSend.pushString(getPluginName());
			blobToSend.pushString("warning");
			blobToSend.pushString(msg);
			PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);
		}
	}

    void Plugin_TestISC::OnFileOpened(const char* filename, const char* threadId, u32 frameId)
    {
        if(_peer)
        {
            ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
            blobToSend.pushString(getPluginName());
            blobToSend.pushString("fileopened");
            blobToSend.pushString(filename);
            blobToSend.pushString(threadId);
            blobToSend.pushUInt32(frameId);
            PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);
        }
    }

    void Plugin_TestISC::OnWorldUpdated()
    {
        if ( !CameraRunStateTest::isRunning() )
            return;

        ObjectGroup* grps = WORLD_MANAGER->getUpdateGroups();

        if ( grps == NULL )
            return;

        ITF::Blob blobToSend( ITF::BLOB_EXTRACT_AND_WRITE_ENABLED );

        blobToSend.pushString( getPluginName() );
        for ( u32 i = 0; i < grps->GetElements().size(); ++i )
        {
            Pickable* ptr = grps->GetElements()[i];

            if ( grps->GetElements()[i]->IsClassCRC( ITF_GET_STRINGID_CRC(Frise,2579114664) ) )
            {
                RefObjectResources( blobToSend, ptr->getRef() );
                continue;
            }
            if ( grps->GetElements()[i]->IsClassCRC( ITF_GET_STRINGID_CRC(Actor,2546623115) ) )
            {
                RefObjectResources( blobToSend, ptr->getRef() );
                continue;
            }
        }
        blobToSend.pushString( "end" );
        PLUGINGATEWAY->sendPacket( blobToSend, _peer, _senderID );
    }

    void Plugin_TestISC::RefObjectResources( ITF::Blob& blobToSend, const ObjectRef& ref )
    {
        Pickable*   ptr = reinterpret_cast< Pickable* >( ref.getObject() );
        if (!ptr || !ptr->getWorld())
            return;
        World* pWorld = ptr->getWorld();
        const Pickable::CellUpdateData& cellUpdateData = ptr->getLastCellUpdateData();
    
        const Vec2d& cellSize = pWorld->getCellsSize(cellUpdateData.m_layerIndex);
        i32 parseCellsX  = (i32)floor(((cellUpdateData.m_aabb.getMin().m_x + fabs(World::m_fminCellXCoord))/cellSize.m_x));
        i32 parseCellsY  = (i32)floor(((cellUpdateData.m_aabb.getMin().m_y + fabs(World::m_fminCellYCoord))/cellSize.m_y));
        i32 endParseCellsX  = (i32)ceil(((cellUpdateData.m_aabb.getMax().m_x + fabs(World::m_fminCellXCoord))/cellSize.m_x));
        i32 endParseCellsY  = (i32)ceil(((cellUpdateData.m_aabb.getMax().m_y + fabs(World::m_fminCellYCoord))/cellSize.m_y));

        if (parseCellsX < 0)
            parseCellsX = 0;

        if (endParseCellsX > pWorld->m_horizCellsCount[cellUpdateData.m_layerIndex]-1)
            parseCellsX = pWorld->m_horizCellsCount[cellUpdateData.m_layerIndex]-1;

        if (parseCellsY < 0)
            parseCellsY = 0;

        if (endParseCellsY > pWorld->m_vertCellsCount[cellUpdateData.m_layerIndex]-1)
            endParseCellsY = pWorld->m_vertCellsCount[cellUpdateData.m_layerIndex]-1;

        i32 startParseCellsX = parseCellsX;

        for (; parseCellsY <= endParseCellsY; parseCellsY++)
            for (parseCellsX = startParseCellsX; parseCellsX <= endParseCellsX; parseCellsX++)
                ListObjectResources( blobToSend, ref, CURRENTWORLD->getCellIndexAtPos(cellUpdateData.m_layerIndex, parseCellsX, parseCellsY ) );
    }

    void Plugin_TestISC::ListObjectResources( ITF::Blob& blobToSend, const ObjectRef& ref, i32 cellId /* = -1 */ )
    {
        Pickable* ptr = reinterpret_cast< Pickable* >( ref.getObject() );

        if ( ptr == NULL )
            return;
        RecurseResources( blobToSend, ptr->getResourceGroup_Slow()->getResourcesList(), cellId );
    }

    void Plugin_TestISC::ListResources( SafeArray< ActorRef >& actors, SafeArray< ObjectRef >& frizes )
    {
        ITF::Blob blobToSend( ITF::BLOB_EXTRACT_AND_WRITE_ENABLED );

        blobToSend.pushString( getPluginName() );
        for ( u32 i = 0; i < actors.size(); ++i )
            ListObjectResources( blobToSend, actors[i] );
        for ( u32 i = 0; i < frizes.size(); ++i )
            ListObjectResources( blobToSend, frizes[i] );

        blobToSend.pushString( "listing_done" );
        PLUGINGATEWAY->sendPacket( blobToSend, _peer, _senderID );
    }

    void Plugin_TestISC::RecurseResources( ITF::Blob& blobToSend, const ResourceGroup::ResourceList& list, i32 cellId )
    {
        static PathChar_t buffer[PATH_C_BUFFERSIZE];

        for ( u32 i = 0; i < list.size(); ++i )
        {
            Resource* res = list[i].getResource();

            if ( res == NULL )
                continue;

            const ResourceGroup* grp = res->getGroup();

            if ( grp != NULL )
            {
                RecurseResources( blobToSend, grp->getResourcesList(), cellId );
                continue;
            }

            StringID::StringIdValueType id = res->getPath().getStringID().GetValue();

            if ( _resourceSet.find( id ) == _resourceSet.end() )
            {
                blobToSend.pushString( "declare_resource" );
                blobToSend.pushUInt64( id );
                res->getPath().getString( buffer );
                blobToSend.pushString( String( buffer ) );
                blobToSend.pushInt32( cellId );
                ITF_VERIFY( _resourceSet.insert( set< StringID::StringIdValueType >::value_type( id ) ).second );
            }
            else
            {
                blobToSend.pushString( "list_resource" );
                blobToSend.pushUInt64( id );
                blobToSend.pushInt32( cellId );
            }
        }
    }
} // namespace ITF

#endif // ITF_FINAL
