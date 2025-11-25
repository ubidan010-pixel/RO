#ifndef _ITF_NETWORKSERVICES_
#define _ITF_NETWORKSERVICES_

#ifdef ITF_SUPPORT_NETWORKSERVICES

namespace ITF
{
class NetworkServices
{
public:

    class User
    {
    public:
        User() : isValid(bfalse),id(0) {}        

        bbool  isValid; 
        u64    id; 
    };

    NetworkServices();
    virtual ~NetworkServices();

	virtual u32 update() { return 0; }
	virtual void initialize(u32 dwMinUsers,u32 dwMaxUsers, bbool  bRequireOnlineUsers,u32 dwSignInPanes ) {}
	virtual void terminate() {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// check if player at given controller index is signed in
    /// @param dwController
	virtual bbool     isUserSignedIn( u32 dwController ) { return true; }


    ///////////////////////////////////////////////////////////////////////////////////////////  
    enum GameType
    {
        Ranked,
        Standard
    };
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Set presence information so that other users know what you do
    virtual void setPresenceInfo_GameType(GameType _gameType, u32 _padIndex) {}
    virtual void setPresenceInfo_GameMode(u32 _gameMode, u32 _padIndex) {}
    virtual void setPresenceInfo_State(u32 _state, u32 _padIndex) {}
    virtual void setPresenceInfo_Custom(u32 _context, u32 _value, u32 _padIndex) {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Open the Sign in UI for the number of specified users 
    /// @param _maxUsers The maximum number of players that can sign in
    virtual void askForSignIn( u32 maxUsers = 1 ) {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Return if we are in a sign in process ... 
    virtual bbool isUserSignInProcessRunning() const { return bfalse; }


    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Get the user ID
    virtual bbool getUserFromControllerIndex( u32 dwController, User* user ) { return bfalse; }
    virtual bbool compareUsers(const User * user1, const User* user2) { return bfalse; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// NetworkAdapter typical API
    enum ENetworkStatus
    {
        ENetworkStatus_Preparing,
        ENetworkStatus_Ready,
        ENetworkStatus_Error,
    };

    inline ENetworkStatus getNetworkStatus() { return m_networkStatus; }
    inline bool isNetworkReady() { return (getNetworkStatus() == ENetworkStatus_Ready); }

    // mostly used for Nintendo platforms
    virtual u32 getPlatformSpecificErrorCode() { return 0; }

    virtual void requestRefresh() {}
    const String8& getIPAddress() const { return m_ipAddress; }

protected:
    virtual void setNetworkStatus(ENetworkStatus _networkStatus);
    String8 m_ipAddress;

private:
    ENetworkStatus m_networkStatus;
};


} // namespace ITF

#endif // ITF_SUPPORT_NETWORKSERVICES

#endif //_ITF_NETWORKSERVICES_
