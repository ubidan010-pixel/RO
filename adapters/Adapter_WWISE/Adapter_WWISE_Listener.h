#ifndef __ADAPTER_WWISE_LISTENER_H__
#define __ADAPTER_WWISE_LISTENER_H__

#include "adapters/Adapter_WWISE/Adapter_WWISE.h"

namespace	ITF
{
	struct Adapter_WWISE::	ListenerPosition
	{
		bool				m_lockedOnCameraPosition;
		bool				m_lockedOnCameraOrientation;
		AkListenerPosition	m_akPosition;

		ListenerPosition()
		:
		m_lockedOnCameraPosition(true),
		m_lockedOnCameraOrientation(true)
		{
			m_akPosition.SetPosition(0.f,0.f,0.f);
			m_akPosition.SetOrientation(0.f, 1.f, 0.f, 0.f, 0.f, 1.f);

		}
	};
}
#endif // __ADAPTER_WWISE_LISTENER_H__
