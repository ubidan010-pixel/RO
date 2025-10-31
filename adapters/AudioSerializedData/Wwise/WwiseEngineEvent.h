#ifndef __WWISEENGINEEVENT_H__
#define __WWISEENGINEEVENT_H__


#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#include "core/serializer/ZSerializerObject.h"
#include "core/StringID.h"



namespace	ITF
{
	class	WwiseEngineEvent
	{


	public:
			DECLARE_SERIALIZE()
			WwiseEngineEvent() : m_engineEventType(AUDIO_ENGEVT_INVALID)
			{
			}

			inline StringID getGUID() const
			{
				return m_guid;
			}

			inline ITF::AudioEngineEvent getEngineEventType() const
			{
				return m_engineEventType;
			}

	private:
			StringID	            m_guid;
			ITF::AudioEngineEvent	m_engineEventType;
	};
}

#endif // __WWISEENGINEEVENT_H__
