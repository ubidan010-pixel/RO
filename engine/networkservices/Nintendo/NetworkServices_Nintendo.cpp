#include "precompiled_engine.h"

#ifndef _ITF_NETWORKSERVICES_NINTENDO_H_
#include "engine/NetworkServices/Nintendo/NetworkServices_Nintendo.h"
#endif //_ITF_NETWORKSERVICES_NINTENDO_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef ITF_SYSTEMADAPTER_NINTENDO_H_
#include "adapters/SystemAdapter_Nintendo/SystemAdapter_Nintendo.h"
#endif //ITF_SYSTEMADAPTER_NINTENDO_H_

#ifdef ITF_SUPPORT_NETWORKSERVICES
namespace ITF
{
    NetworkServices_Nintendo::NetworkServices_Nintendo()
    {
    }

    NetworkServices_Nintendo::~NetworkServices_Nintendo()
    {
    }

    void NetworkServices_Nintendo::initialize(u32 dwMinUsers, u32 dwMaxUsers, bbool bRequireOnlineUsers, u32 dwSignInPanes)
    {
    }

    void NetworkServices_Nintendo::terminate()
    {
    }

    u32 NetworkServices_Nintendo::update()
    {
        return 0;
    }

    u32 NetworkServices_Nintendo::getPlatformSpecificErrorCode()
    {
        return m_platformErrorCode;
    }
} // namespace ITF

#endif
