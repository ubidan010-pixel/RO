#ifdef ITF_XBOX_SERIES
#ifndef _ITF_NETWORKSERVICES_XBOXSERIES_H_
#define _ITF_NETWORKSERVICES_XBOXSERIES_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifdef ITF_SUPPORT_NETWORKSERVICES
namespace ITF
{

class NetworkServices_xboxseries: public NetworkServices
{
        
public:

    NetworkServices_xboxseries();
	~NetworkServices_xboxseries();

    virtual u32 update() override;
    virtual void initialize(u32 dwMinUsers, u32 dwMaxUsers, bbool  bRequireOnlineUsers, u32 dwSignInPanes) override;
    virtual void terminate() override;

    virtual u32 getPlatformSpecificErrorCode() override;

protected:

private:
    int m_platformErrorCode;
};
  

} // namespace ITF

#endif // ITF_SUPPORT_NETWORKSERVICES

#endif //_ITF_NETWORKSERVICES_XBOXSERIES_H_

#endif //ITF_XBOX_SERIES
