// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_


#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ONLINETRACKING_PLUGIN_H
#include "OnlineTrackingPlugin.h"
#endif //_ITF_ONLINETRACKING_PLUGIN_H

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef __ITF__STRINGUTILS_H_
#include "StringUtils.h"
#endif

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
	OnlineTrackingPlugin::OnlineTrackingPlugin(void)
	{

		m_editorPeer					= NULL;
		m_lastSentTime					= ELAPSEDTIME;
		m_levelPath						= "not set";
		m_LevelNameTimer				= 0.f; 
		m_sendLevelNameCounter			= 0;
		m_bubbleExitCount				= 0;
		m_currentStep					= 1;
		m_iconSizeFactor				= 1.0f;
		m_camPosY						= 12.0f;
	
	}

	void OnlineTrackingPlugin::clearTrackingData()
	{

		for (u32 i=0;i<4;i++)
		{
			displayed_pos[i].clear();
			displayed_hits[i].clear();
			displayed_death[i].clear();
		}
		
		m_bubbleExitCount	= 0;

		
	}


	void OnlineTrackingPlugin::DisplayBubbleExitCount()
	{

		ITF::String bubble_exit_str;
		bubble_exit_str.setTextFormat("Bubble Exit count : %d" , m_bubbleExitCount);
		GFX_ADAPTER->drawDBGText( bubble_exit_str,150,35,0);
		
	}

	void  OnlineTrackingPlugin::onReceiveTrackingData(Blob& _blob)
	{

		u32 size = _blob.extractUint32();


		Vec3d last_pos[4];// = Vec3d::Zero;
	
		for (u32 i=0;i<size;i++)
		{

			String id,trackingID,tag,attributes,date_created;

			ITF_TrackingData_Positions t_pos;

			_blob.extractString(id);
			_blob.extractString(trackingID);
			_blob.extractString(tag);
			_blob.extractString(attributes);
			_blob.extractString(date_created);

			t_pos.id			= id;
			t_pos.trackingID	= trackingID;

			if (tag.isSameString("PLAYER_POS",tag.getCharCopy()) )
			{

				std::map <String,String> attribs = StringSplitUtils::SplitTrackingAttributes(attributes);

				Vec3d v;
				
				v.set( attribs["P1X"].atof32() ,attribs["P1Y"].atof32(),attribs["P1Z"].atof32() );
				t_pos.pos = v;

				if (!last_pos[0].IsEqual( v ) )
				{
					displayed_pos[0].push_back( t_pos );
					last_pos[0] = v;
				}
				
				v.set( attribs["P2X"].atof32() ,attribs["P2Y"].atof32(),attribs["P2Z"].atof32() );
				t_pos.pos = v;
				
				if (!last_pos[1].IsEqual( v ) )
				{
					displayed_pos[1].push_back( t_pos );
					last_pos[1] = v;
				}
				
				v.set( attribs["P3X"].atof32() ,attribs["P3Y"].atof32(),attribs["P3Z"].atof32() );
				t_pos.pos = v;
				
				if (!last_pos[2].IsEqual( v ) )
				{
					displayed_pos[2].push_back( t_pos );
					last_pos[2] = v;
				}

				v.set( attribs["P4X"].atof32() ,attribs["P4Y"].atof32(),attribs["P4Z"].atof32() );
				t_pos.pos = v;
				
				if (!last_pos[3].IsEqual( v ) )
				{
					displayed_pos[3].push_back( t_pos );
					last_pos[3] = v;
				}
				
			}

			else if (tag.isSameString("PLAYER_HIT",tag.getCharCopy()))
			{

				std::map <String,String> attribs = StringSplitUtils::SplitTrackingAttributes(attributes);

				Vec3d v;

				v.set( attribs["X"].atof32() ,attribs["Y"].atof32(),attribs["Z"].atof32() );

				t_pos.pos = v;

				u32 index = 0;

				if		(attribs["PLAYERID"].isSameString(attribs["PLAYERID"].getCharCopy(),"1"))
				{
					index = 0;
				}
				else if (attribs["PLAYERID"].isSameString(attribs["PLAYERID"].getCharCopy(),"2"))
				{
					index = 1;
				}
				else if (attribs["PLAYERID"].isSameString(attribs["PLAYERID"].getCharCopy(),"3"))
				{
					index = 2;
				}
				else if (attribs["PLAYERID"].isSameString(attribs["PLAYERID"].getCharCopy(),"4"))
				{
					index = 3;
				}
				else 
					{
					ITF_ASSERT("OnlineTracking - PLAYER_HIT : unable to find PLAYER_ID");	
				}

				t_pos.state = ITF_TrackingData_Positions::ITF_TD_Standard;

				if (attribs["PAF"].isSameString(attribs["PAF"].getCharCopy(),"YES"))
				{
					t_pos.state = ITF_TrackingData_Positions::ITF_TD_Paf;
					displayed_hits[index].push_back( t_pos );
					continue;
				}
				if (attribs["OFFSCREEN"].isSameString(attribs["OFFSCREEN"].getCharCopy(),"YES"))
				{
					t_pos.state = ITF_TrackingData_Positions::ITF_TD_Offscreen;
					displayed_hits[index].push_back( t_pos );
					continue;
				}

				if (attribs["DEATH"].isSameString(attribs["DEATH"].getCharCopy(),"YES"))
				{
					displayed_death[index].push_back( t_pos );
				}
				else
				{
					displayed_hits[index].push_back( t_pos );
				}
				
			}

			else if (tag.isSameString("BUBBLE_EXIT",tag.getCharCopy()))
			{
				m_bubbleExitCount++;
			}

			else 
			{
				//LOG("unknown tag : %s",tag.getCharCopy());
			}
			
		}

	}

	void OnlineTrackingPlugin::onReceiveClearData(Blob& _blob) 
	{
		clearTrackingData();
		_blob.clear();
	}

	void  OnlineTrackingPlugin::receive(Blob& _blob, const NETPeer* _peer, u32 _senderID)
	{
		String message;
		_blob.extractString(message);

		if (message == getSendDataCmdName())
		{
			onReceiveTrackingData(_blob);
		}

		else if (message == getAliveCmdName())
		{
			m_lastAlive = CURRENTFRAME;
		}
		else if (message == getClearDataCmdName() )
		{
			onReceiveClearData(_blob);
		}
		else if (message=="PluginId")
		{

		}
		else
		{
			//ITF_ASSERT(0); // unknown command
		}

		_blob.setSeekPos ( _blob.getSize() );

	}

	void  OnlineTrackingPlugin::onCloseApp( )
	{
		ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
		blob.pushString(getPluginName());
		blob.pushString(getCloseCmdName());
		PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
	}

	void OnlineTrackingPlugin::SendLevelNameToTool()
	{

		Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

		ITF_STDSTRING checkPointStr = "";

		if ( checkpoint )
		{
			ObjectPath path;
			SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint,path);	
			path.toString(checkPointStr);
		}

		if (CURRENTWORLD != NULL )
		{
			if (CURRENTWORLD->getRootScene()!=NULL)
			{
				CURRENTWORLD->getRootScene()->getPath().getString(m_levelPath);
			}

			ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
			blob.pushString(getPluginName());
			blob.pushString(getSendLevelNameCmdName());
			blob.pushString(m_levelPath);
			PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);

		}
	    
	}

	void OnlineTrackingPlugin::LaunchTool()
	{
		SYSTEM_ADAPTER->launchExe("UA_OnlineTracking.exe", NULL, btrue, btrue);
		
	}

	void OnlineTrackingPlugin::DisplayPos()
	{
		String s;

		Vec3d v2dpos,last_pos,last_pos3dto2d;

		AABB box_screen = View::getMainView()->getScreenProjAABB();

		
		if (m_camPosY<13.0f)
		{
			m_currentStep = 1;
		}

		else if (m_camPosY<15.0f)
		{
			m_currentStep = 2;
		}
		else if (m_camPosY>15.0f)
		{
			m_currentStep = 4;
		}
	
		for (u32 k=0;k<4;k++)
		{
			std::vector < ITF_TrackingData_Positions >  & pos = displayed_pos[k];
			s.setTextFormat("%d",k+1);

			u32 size = u32(pos.size());

			std::vector<u32> colors;
			colors.push_back(COLOR_WHITE);
			colors.push_back(COLOR_RED|COLOR_GREEN);
			colors.push_back(COLOR_RED|COLOR_BLUE);
			colors.push_back(COLOR_WHITE);

			colors.push_back(COLOR_WHITE);
			colors.push_back(COLOR_GREEN|COLOR_RED);
			colors.push_back(COLOR_GREEN|COLOR_BLUE);
			colors.push_back(COLOR_GREEN);

			bool bDrawLines = !GFX_ADAPTER->hasHideFlags(GFXAdapter::HideFlag_DBGPrimitive);
			
			for (u32 i=1 ; i<size ; i+=m_currentStep)
			{

				if (box_screen.contains(pos[i].pos.truncateTo2D()))
				{

					Vec3d point = pos[i].pos;

					u32 icon = (pos[i].trackingID.atoui32() % 2);

					if (icon==0)
					{
						EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_RedSquare,point ,15.0f*m_iconSizeFactor,0,false,false,colors[k] );
					}
					else
					{
						EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_YellowSquare,point ,15.0f*m_iconSizeFactor,0,false,false,colors[k+4] );
					}
					

					GFX_ADAPTER->compute3DTo2D(pos[i].pos,v2dpos);
					GFX_ADAPTER->drawDBGText( s,v2dpos.getX(),v2dpos.getY(),pos[i].pos.getZ());

					if (bDrawLines)
					{
						last_pos = pos[i-1].pos;
						GFX_ADAPTER->compute3DTo2D(last_pos,last_pos3dto2d);
						GFX_ADAPTER->draw2dLine(last_pos3dto2d.truncateTo2D(),v2dpos.truncateTo2D(),  2.0f, ITFCOLOR_TO_U32(100, 20, 20, 50));
					}

			}

		}

	}

}

	void OnlineTrackingPlugin::DisplayHit()
	{
		String s = "HIT";

		Vec3d v2dpos;

		AABB box_screen = View::getMainView()->getScreenProjAABB();

		for (u32 k=0;k<4;k++)
		{

				std::vector < ITF_TrackingData_Positions >  & pos = displayed_hits[k];

				u32 size = u32(pos.size());

				for (u32 i=0;i<size;i++)
				{

					if (box_screen.contains(pos[i].pos.truncateTo2D()))
					{
					
						if (pos[i].state == ITF_TrackingData_Positions::ITF_TD_Paf)
						{
							s = "PAF";
							EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_Hit_Receive,pos[i].pos ,60.0f*m_iconSizeFactor,0,false,0,COLOR_GREEN );
						}
						else if (pos[i].state == ITF_TrackingData_Positions::ITF_TD_Offscreen)
						{
							s = "OFFSCREEN";
							EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_Hit_Receive,pos[i].pos ,60.0f*m_iconSizeFactor,0,false,0,COLOR_BLUE );
						}
						else
						{
							s = "HIT";
							EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_Hit_Receive,pos[i].pos ,600.0f );
						}
	
						GFX_ADAPTER->compute3DTo2D(pos[i].pos,v2dpos);
						GFX_ADAPTER->drawDBGText( s,v2dpos.getX(),v2dpos.getY(),pos[i].pos.getZ());

					}

				}

			}

	}

	void OnlineTrackingPlugin::DisplayDeath()
	{
		String s = "DEATH";
		Vec3d v2dpos;
		AABB box_screen = View::getMainView()->getScreenProjAABB();

		for (u32 k=0;k<4;k++)
		{
				std::vector < ITF_TrackingData_Positions >  & pos = displayed_death[k];
				u32 size = u32(pos.size());

				for (u32 i=0;i<size;i++)
				{

					if (box_screen.contains(pos[i].pos.truncateTo2D()))
					{
					
						Vec3d point = pos[i].pos;

						EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_Death,point ,80.0f*m_iconSizeFactor,0,false,false,COLOR_WHITE);	

						GFX_ADAPTER->compute3DTo2D(pos[i].pos,v2dpos);
						GFX_ADAPTER->drawDBGText( s,v2dpos.getX(),v2dpos.getY(),pos[i].pos.getZ());

					}

				}

			}

	}

	void OnlineTrackingPlugin::update()
	{
#ifdef ITF_SUPPORT_ONLINETRACKING
		if (CONFIG->m_enableonlineTracking==btrue)
			{

				if ( GAMEMANAGER->isPlaying() == btrue)
				{
					m_camPosY = -CAMERA->getCorrectedPos().getY();
					
					m_iconSizeFactor = 10.0f/m_camPosY;
					//LOG("m_iconSizeFactor : %.1f",m_iconSizeFactor);
					DisplayPos();
					DisplayHit();
					DisplayDeath();		
					//DisplayBubbleExitCount();
				}

			}
#endif //ITF_SUPPORT_ONLINETRACKING
	}


	void OnlineTrackingPlugin::onToolConnect( const NETPeer* _peer, u32 _senderID )
	{
			Plugin::onToolConnect(_peer, _senderID);
			SendLevelNameToTool();

	}

} // namespace ITF

