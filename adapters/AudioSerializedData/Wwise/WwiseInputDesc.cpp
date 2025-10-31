#include "precompiled_AudioSerializedData.h"

#ifdef ITF_SUPPORT_WWISE
#include "adapters/AudioSerializedData/Wwise/WwiseInputDesc.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
#include "core/Macros.h"
#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

namespace	ITF
{
	BEGIN_SERIALIZATION(WwiseInputDesc)

	    SERIALIZE_MEMBER("name", m_inputName);
	    SERIALIZE_MEMBER("WwiseRtpcGUID", m_rtpcGUID);
	    SERIALIZE_MEMBER("IsLocal", m_isLocal);
        SERIALIZE_AUDIO_RTPC_TYPE("RtpcType", m_rtpcType);
	END_SERIALIZATION()

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	WwiseInputDesc::WwiseInputDesc() :
		m_rtpcID(ITF_INVALID_SOUND_RTPC_ID),
		m_type(AUDIO_ITEM_TYPE_INVALID),
		m_isLocal(btrue),
        m_rtpcType(AUDIO_RTPC_INVALID)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseInputDesc::resolve()
	{
        if(m_rtpcGUID.isValid())
        {
            m_rtpcID = AUDIO_ADAPTER->getIDFromGUID(m_rtpcGUID);
         //   m_type = AUDIO_ADAPTER->getAudioTypeFromGUID(m_rtpcGUID);

            switch(m_type)
            {
            case AUDIO_ITEM_WWISE_GAME_PARAMETER:
                switch (m_rtpcType)
                {
                default:
                    m_rtpcType = m_isLocal ? AUDIO_RTPC_LOCAL : AUDIO_RTPC_GLOBAL;
                    break;

                case AUDIO_RTPC_LOCAL:
                    m_isLocal = true;
                    break;

                case AUDIO_RTPC_GLOBAL:
                    m_isLocal = false;
                    break;

                case AUDIO_RTPC_DISTANCE_EMITER_LISTENER:
                    m_isLocal = true;
                    break;
                }
                break;
                
            case AUDIO_ITEM_WWISE_SWITCH_GROUP:
                m_isLocal = true;
                m_rtpcType = AUDIO_RTPC_SWITCH ;
                break;
                

            case AUDIO_ITEM_WWISE_STATE_GROUP:
                m_isLocal = false;
                m_rtpcType = AUDIO_RTPC_STATE;
                break;


            default:
                ITF_WARNING_CATEGORY(Sound, NULL, 0, "Bad Wwise RTPC type \"%d\"", m_type);
                break;
            }           
        }
	}
}
#endif
