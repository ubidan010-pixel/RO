#include "precompiled_inputadapter_WII.h"

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_



namespace ITF
{
	//Static members (used by static callback)
	WiiSpeaker::Info WiiSpeaker::_info[WPAD_MAX_CONTROLLERS] ;
	OSAlarm	WiiSpeaker::SpeakerAlarm;
	s16	WiiSpeaker::AudioBuffer[WPAD_MAX_CONTROLLERS][WII_SPEAKER_SAMPLES_PER_AUDIO_PACKET];
	
	// Main update, called by an Alarm timer (see DOC for more explcanation)
	void WiiSpeaker::Update(OSAlarm *alarm, OSContext *context)
	{
		#pragma unused(alarm, context)
		u8		data[WII_SPEAKER_AUDIO_PACKET_MAX_LEN+4];
		u32		flag;
		BOOL	adv = FALSE;
		BOOL	intr;

		for(s32 chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++)
		{
		    const s32 i_SampleCount = AXRmtGetSamples(chan, AudioBuffer[chan], WII_SPEAKER_SAMPLES_PER_AUDIO_PACKET);
		    
		    ITF_ASSERT(i_SampleCount<=WII_SPEAKER_SAMPLES_PER_AUDIO_PACKET);
			
			if(i_SampleCount>0)
			{
			    const s32 i_PacketSize = WENCGetEncodeBufferSize(i_SampleCount);
			    
			    ITF_ASSERT(i_PacketSize<=WII_SPEAKER_AUDIO_PACKET_MAX_LEN);
			    ITF_ASSERT(0<i_PacketSize);
			    
			    
				adv = TRUE;

				if(_info[chan].state > State_OFF)
				{
					intr = OSDisableInterrupts();
					if(WPADCanSendStreamData(chan))
					{
						flag = (u32) WENC_FLAG_CONT;

						switch(_info[chan].state)
						{
						case State_PLAY_FIRST:
							_info[chan].state = State_PLAYING;
							_info[chan].u_MuteState = Mute_OFF;
							flag = (u32) WENC_FLAG_FIRST;

						/* no break on purpose! */
						case State_PLAYING:
							WENCGetEncodeData
							(
								&_info[chan].encInfo,
								flag,
								(const s16 *) AudioBuffer[chan],
								i_SampleCount,
								data
							);
	                        
	                        WPADSendStreamData(chan, data, i_PacketSize);
							break;
						}
					}

					OSRestoreInterrupts(intr);
				}
			}
			
			if(_info[chan].u_UserAlive)
			    _info[chan].u_UserAlive--;
	            
	        if(_info[chan].state == State_PLAYING)
		    {
	            const u32 u_MuteState = _info[chan].u_MuteState;
		            
	            if(u_MuteState == Mute_OFF)
	            {
	                if(_info[chan].u_UserAlive==0)
	                {
	                    _info[chan].u_MuteState=Mute_IN_PROGRESS;
	                    WPADControlSpeaker(chan, WPAD_SPEAKER_MUTE, MuteOnCallback);
	                }
	            }
	            else if(u_MuteState == Mute_ON)
	            {
	                if(_info[chan].u_UserAlive)
	                {
	                    _info[chan].u_MuteState=Mute_IN_PROGRESS;
	                    WPADControlSpeaker(chan, WPAD_SPEAKER_MUTE_OFF, MuteOffCallback);
	                }
	            }
			 
			}
		}

		if(adv)
		{
			AXRmtAdvancePtr(WII_SPEAKER_SAMPLES_PER_AUDIO_PACKET);
		}
	}

	void WiiSpeaker::ProcessStart()
	{
		if(_isAlarmOn) return;

		for(u32 chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++)
		{
			if(_info[chan].state == State_HOME_MENU) return;
			
			_info[chan].u_UserAlive = 0;
			_info[chan].u_MuteState = Mute_OFF;
		}

		OSCreateAlarm(&SpeakerAlarm);
		OSSetPeriodicAlarm(&SpeakerAlarm, OSGetTime(), WPAD_STRM_INTERVAL, Update);

		_isAlarmOn = btrue;
	}

	void WiiSpeaker::ProcessStop()
	{
		if(!_isAlarmOn) return;

		for(int chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++)
		{
			if(_info[chan].state != State_OFF && _info[chan].state != State_HOME_MENU) return;
		}

		OSCancelAlarm(&SpeakerAlarm);

		_isAlarmOn = bfalse;
	}

	//Home button process
	void WiiSpeaker::HomeButtonStart()
	{
		for(int chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++)
		{
			if(_info[chan].state > State_OFF)
			{
				_info[chan].state = State_HOME_MENU;
				_info[chan].u_MuteState = Mute_OFF;

				WPADControlSpeaker(chan, WPAD_SPEAKER_OFF, OffCallback);
			}
		}

		ProcessStop();
	}

	void WiiSpeaker::HomeButtonFinish()
	{
		for(int chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++)
		{
			if(_info[chan].state == State_HOME_MENU)
			{
				_info[chan].state = State_OFF;

				TurnOn(chan);
			}
		}

		ProcessStart();
	}

	//Start/Stop speaker
	void WiiSpeaker::TurnOn(s32 _chan)
	{
		if(_info[_chan].state == State_OFF)
		{
			_info[_chan].state = State_TURNING_ON;
			_info[_chan].u_MuteState = Mute_OFF;

			WPADControlSpeaker(_chan, WPAD_SPEAKER_ON, OnCallback);
		}
	}

	void WiiSpeaker::TurnOff(s32 _chan)
	{
		if(_info[_chan].state > State_OFF)
		{
			_info[_chan].state = State_TURNING_OFF;
			_info[_chan].u_MuteState = Mute_OFF;

			WPADControlSpeaker(_chan, WPAD_SPEAKER_OFF, OffCallback);
		}
	}

	void WiiSpeaker::WaitForShutdown(s32 _chan)
	{
		while(_info[_chan].state != State_OFF)
		{
		};
	}

	void WiiSpeaker::Connect(s32 _chan)
	{
		if(_info[_chan].state != State_HOME_MENU)
		{
			_info[_chan].state = State_OFF;
			TurnOn(_chan);

			// make sure the alarm is working
			ProcessStart();
		}
	}

	void WiiSpeaker::Disconnect(s32 _chan)
	{
		if(_info[_chan].state != State_HOME_MENU)
		{
			_info[_chan].state = State_OFF;
			TurnOff(_chan);
		}
	}


	// Callback  
	void WiiSpeaker::OffCallback(s32 chan, s32 result)
	{
		/* only set the state to off if the last action was to turn it off */
		if(_info[chan].state == State_TURNING_OFF) _info[chan].state = State_OFF;
	}

	void WiiSpeaker::PlayCallback(s32 chan, s32 result)
	{
		if(result == WPAD_ERR_NONE)
		{
			if(_info[chan].state == State_PLAY_START)
			{
				_info[chan].state = State_PLAY_FIRST;
				ITF_MemSet(&_info[chan].encInfo, 0, sizeof(WENCInfo));
			}
		}
	}

	void WiiSpeaker::OnCallback(s32 chan, s32 result)
	{
		if(result == WPAD_ERR_NONE)
		{
			/* only set the wiimote speaker to play if the last action was to turn it on */
			if(_info[chan].state == State_TURNING_ON)
			{
				_info[chan].state = State_PLAY_START;
				_info[chan].u_MuteState = Mute_OFF;
				WPADControlSpeaker(chan, WPAD_SPEAKER_PLAY, PlayCallback);
			}
		}
	}

	void WiiSpeaker::MuteOnCallback(s32 chan, s32 result)
	{
		if(result == WPAD_ERR_NONE)
		{
		    if(_info[chan].state == State_PLAYING)
		    {
	    		if(_info[chan].u_MuteState == Mute_IN_PROGRESS) 
	    		    _info[chan].u_MuteState = Mute_ON;
		    }
		}
	}

	void WiiSpeaker::MuteOffCallback(s32 chan, s32 result)
	{
		if(result == WPAD_ERR_NONE)
		{
		    if(_info[chan].state == State_PLAYING)
		    {
	    		if(_info[chan].u_MuteState == Mute_IN_PROGRESS) 
	    		    _info[chan].u_MuteState = Mute_OFF;
		    }
		}
	}

}