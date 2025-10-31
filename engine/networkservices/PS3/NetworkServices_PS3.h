#ifdef ITF_PS3
#ifndef _ITF_NETWORKSERVICES_PS3_H_
#define _ITF_NETWORKSERVICES_PS3_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_
#include <np.h>

namespace ITF
{

class NetworkServices_PS3: public NetworkServices
{
        
public:

    NetworkServices_PS3();
	~NetworkServices_PS3();

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Init
    virtual void initialize(u32 dwMinUsers,u32 dwMaxUsers, bbool  bRequireOnlineUsers,u32 dwSignInPanes );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// termination
    virtual void terminate();

	///////////////////////////////////////////////////////////////////////////////////////////
	/// check if player at given controller index is signed in
	/// @param dwController
	virtual bbool     isUserSignedIn( u32 dwController ) { return m_npOnline; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// update
    u32    update();

	///////////////////////////////////////////////////////////////////////////////////////////
	/// Asynchronous: request a ticket (first call), return true only when ticket has been received
	/// @param npTicket (output) only valid if method returns true
	/// @param npTicketSize (output) only valid if method returns true
	bool getRequestTicket(char **npTicket, size_t *npTicketSize);

    static const SceNpCommunicationId s_communication_id[];
    static const SceNpCommunicationSignature s_communication_sig[];

protected:
	void onNpManagerCallback(int event, int result);
	void onTicketResult(int result);
	friend void npManagerCallback(int event, int result, void* arg);
private:
   	bbool	m_npOnline;
	char*	m_npPool;
	char*	m_npTicket;
	size_t	m_npTicketSize;
};
  

} // namespace ITF

#endif //_ITF_NETWORKSERVICES_PS3_H_
#endif //ITF_PS3
