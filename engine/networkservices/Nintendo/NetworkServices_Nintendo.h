#ifdef ITF_NINTENDO
#ifndef _ITF_NETWORKSERVICES_NINTENDO_H_
#define _ITF_NETWORKSERVICES_NINTENDO_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifdef ITF_SUPPORT_NETWORKSERVICES
namespace ITF
{
    class NetworkServices_Nintendo : public NetworkServices
    {

    public:

        NetworkServices_Nintendo();
        ~NetworkServices_Nintendo();

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Init
        virtual void initialize(u32 dwMinUsers, u32 dwMaxUsers, bbool  bRequireOnlineUsers, u32 dwSignInPanes);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// termination
        virtual void terminate();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// check if player at given controller index is signed in
        /// @param dwController
        virtual bbool     isUserSignedIn(u32 dwController) { return m_npOnline; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// update
        u32    update();

    protected:
    private:
    };

} // namespace ITF

#endif // ITF_SUPPORT_NETWORKSERVICES
#endif //_ITF_NETWORKSERVICES_NINTENDO_H_
#endif //ITF_NINTENDO
