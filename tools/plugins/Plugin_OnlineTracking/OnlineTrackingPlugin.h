#ifndef _ITF_ONLINETRACKING_PLUGIN_H
#define _ITF_ONLINETRACKING_PLUGIN_H

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_



#define ONLINETRACKINGPLUGIN_LEVELNAME_UPDATE_RATE 5.0f

namespace ITF
{

	// Class predeclarations

	class Blob;


	class ITF_TrackingData
	{
	public:

		String		id;
		String		trackingID;
		String		tag;
		String		attributes;
		String		date_created;

	};

	class ITF_TrackingData_Positions
	{
	public:

		enum TD_State {
			ITF_TD_Standard=0,
			ITF_TD_Paf,
			ITF_TD_Offscreen,
			ITF_TD_Count
		};

		String		id;
		String		trackingID;
		Vec3d		pos;
		void		SetFromTrackingIDAndAttributes(String tid,String attributes);

		TD_State state;

		

	};


	class TrackingData_BubbleExit
	{
		public:

			String		id;
			String		trackingID;
			Vec3d		pos;

			u32			playerIndex;
			
	};

	class OnlineTrackingPlugin : public Plugin
	{

	public:

		static const char*      getPluginName               (   ) { return  "OnlineTrackingPlugin";}
		static const float      getUpdateRate               (   ) { return 0.25f;              } // in seconds
		static const char*      getAliveCmdName             (   ) { return  "Alive";           }
		static const char*      getCloseCmdName             (   ) { return  "Close";           }
		static const char*      getHideCmdName              (   ) { return  "Hide";            }
		static const char*      getSendDataCmdName          (   ) { return  "SendData";        };
		static const char*      getSendLevelNameCmdName     (   ) { return  "SendLevelName";   };

		static const char*      getClearDataCmdName		    (   ) { return  "ClearData";       };
	   
		OnlineTrackingPlugin        (   );

		void					LaunchTool();
	    
		void                    updateWatchedValues     ( Blob& _blob  );
		float                   m_lastSentTime;
		
		// Plugin inherited
		void					receive        ( Blob& _blob, const NETPeer* _peer, u32 _senderID );
		void					update();
		
		void					SendLevelNameToTool();

        void                    onToolConnect(const NETPeer* _peer, u32 _senderID);
        void                    onCloseApp( );

		void					onReceiveTrackingData(Blob& _blob);
		void					onReceiveClearData(Blob& _blob);

		void					clearTrackingData();


		void					DisplayPos(  );
		void					DisplayHit(  );
		void					DisplayDeath( );

		void					DisplayBubbleExitCount();

		f64                     m_lastAlive;
		f32						m_LevelNameTimer;

		f32						m_iconSizeFactor;
		f32						m_camPosY;

		PathString_t            m_levelPath;

		std::vector < ITF_TrackingData_Positions >  displayed_pos[4]; // player 1
		std::vector < ITF_TrackingData_Positions >  displayed_hits[4];
		std::vector < ITF_TrackingData_Positions >  displayed_death[4];

		u32										m_bubbleExitCount;
		u32										m_sendLevelNameCounter;

		u32										m_currentStep;
		
	};

} // namespace ITF


#endif // _ITF_ONLINETRACKING_PLUGIN_H
