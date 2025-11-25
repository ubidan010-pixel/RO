#ifdef ITF_PS5
#ifndef _ITF_NETWORKSERVICES_PS5_H_
#define _ITF_NETWORKSERVICES_PS5_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_
#include <np.h>

namespace ITF
{

class NetworkServices_PS5: public NetworkServices
{
        
public:

    NetworkServices_PS5();
	~NetworkServices_PS5();

    virtual u32 update();
    virtual void initialize(u32 dwMinUsers, u32 dwMaxUsers, bbool  bRequireOnlineUsers, u32 dwSignInPanes);
    virtual void terminate();

    virtual u32 getPlatformSpecificErrorCode();

protected:

private:
    int m_platformErrorCode;
    static void libnetctlCallback(int eventType, void* arg);
};
  

} // namespace ITF

#endif //_ITF_NETWORKSERVICES_PS5_H_
#endif //ITF_PS5
