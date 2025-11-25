#include "precompiled_engine.h"

#ifndef _ITF_NETWORKSERVICES_XBOXSERIES_H_
#include "engine/NetworkServices/Xboxseries/NetworkServices_xboxseries.h"
#endif //_ITF_NETWORKSERVICES_XBOXSERIES_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#include "adapters/SystemAdapter_xboxseries/SystemAdapter_xboxseries.h"

#ifdef ITF_SUPPORT_NETWORKSERVICES

namespace ITF
{
	NetworkServices_xboxseries::NetworkServices_xboxseries():
        m_platformErrorCode(0)
    {
	}

    NetworkServices_xboxseries::~NetworkServices_xboxseries()
	{
	}

    void NetworkServices_xboxseries::initialize( u32 dwMinUsers,u32 dwMaxUsers,bbool bRequireOnlineUsers,u32 dwSignInPanes )
    {
	}
        
    void NetworkServices_xboxseries::terminate()
    {
    }

    u32 NetworkServices_xboxseries::update()
    {
        return 0;
    }

    u32 NetworkServices_xboxseries::getPlatformSpecificErrorCode()
    {
        return m_platformErrorCode;
    }
} // namespace ITF

#endif

