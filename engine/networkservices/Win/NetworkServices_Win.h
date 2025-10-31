#ifdef ITF_WINDOWS
#ifndef _ITF_NETWORKSERVICES_WIN_H_
#define _ITF_NETWORKSERVICES_WIN_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

namespace ITF
{
    class NetworkServices_Win : public NetworkServices
    {

    public:

        NetworkServices_Win();
        ~NetworkServices_Win();

        virtual u32 update();
        virtual void initialize(u32 dwMinUsers, u32 dwMaxUsers, bbool  bRequireOnlineUsers, u32 dwSignInPanes);
        virtual void terminate();

    protected:

    private:

    };

} // namespace ITF


#endif //_ITF_NETWORKSERVICES_WIN_H_

#endif // ITF_WINDOWS
