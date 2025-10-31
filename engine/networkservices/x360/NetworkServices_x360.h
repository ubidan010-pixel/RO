#ifdef ITF_X360
#ifndef _ITF_NETWORKSERVICES_X360_H_
#define _ITF_NETWORKSERVICES_X360_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifndef _ITF_SYSTEMMESSAGELISTENER_X360_H_
#include "adapters/SystemAdapter_x360/SystemMessageListener_x360.h"
#endif

namespace ITF
{

//Reference to XBOX LIVE
class NetworkServices_x360: public NetworkServices
{
        
public:
    //copy paste from ATG::signin..
    // Flags that can be returned from Update()
    enum SIGNIN_UPDATE_FLAGS
    {
        SIGNIN_USERS_CHANGED    = 0x00000001,
        SYSTEM_UI_CHANGED       = 0x00000002,
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Construction
    NetworkServices_x360();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Check users that are signed in
    u32    getSignedInUserCount()
    {
        return m_dwNumSignedInUsers;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// getSignedInUserMask
    u32    getSignedInUserMask()
    {
        return m_dwSignedInUserMask;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// isUserSignedIn
    /// @param dwController
    virtual bbool     isUserSignedIn( u32 dwController )
    {
        return ( m_dwSignedInUserMask & ( 1 << dwController ) ) != 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// areUsersSignedIn
    bbool     areUsersSignedIn()
    {
        return ( m_dwNumSignedInUsers >= m_dwMinUsers ) &&
            ( m_dwNumSignedInUsers <= m_dwMaxUsers );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Get the first signed-in user
    u32    getSignedInUser()
    {
        return m_dwFirstSignedInUser;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Check users that are signed into live
    u32    getOnlineUserMask()
    {
        return m_dwOnlineUserMask;
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// isUserOnline
    /// @param dwController
    bbool     isUserOnline( u32 dwController )
    {
        return ( m_dwOnlineUserMask & ( 1 << dwController ) ) != 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Check the presence of system UI
    bbool     isSystemUIShowing() const
    {
        return m_bSystemUIShowing || m_bNeedToShowSignInUI;
    }

    bbool isUserSignInProcessRunning() const
    {
        return isSystemUIShowing();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Function to reinvoke signin UI
    void     showSignInUI()
    {
        m_bNeedToShowSignInUI = TRUE;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Check privileges for a signed-in users
    bbool     checkPrivilege( u32 dwController, XPRIVILEGE_TYPE priv );

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Init
    virtual void initialize(u32 dwMinUsers,u32 dwMaxUsers, bbool  bRequireOnlineUsers,u32 dwSignInPanes );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// termination
    virtual void terminate();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// update
    u32    update();

    ///////////////////////////////////////////////////////////////////////////////////////////
    virtual bbool getUserFromControllerIndex( u32 dwController , User* user);
    virtual bbool compareUsers(const User * user1, const User* user2);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// set rich presence info
    void setPresenceInfo_GameType(GameType _gameType, u32 _padIndex);
    void setPresenceInfo_GameMode(u32 _gameMode, u32 _padIndex);
    void setPresenceInfo_State(u32 _state, u32 _padIndex);
    void setPresenceInfo_Custom(u32 _context, u32 _value, u32 _padIndex);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Open the sign-in dialog for maxUsers 
    virtual void askForSignIn( u32 maxUsers = 1 ) ;

private:
    // Private constructor to prevent instantiation
    void SignIn();

    //check if player status changed, you may have to reboot the title
    void checkPlayerStatusChanges();

    //
    bbool isShowingMessageBox();

    // Parameters
    u32 m_dwMinUsers;             // minimum users to accept as signed in
    u32 m_dwMaxUsers;             // maximum users to accept as signed in
    bbool m_bRequireOnlineUsers;    // online profiles only
    u32 m_dwSignInPanes;          // number of panes to show in signin UI

    // Internal variables
    u32 m_dwSignedInUserMask;           // bitfields for signed-in users
    u32 m_dwFirstSignedInUser;          // first signed-in user
    u32 m_dwNumSignedInUsers;           // number of signed-in users
    u32 m_dwOnlineUserMask;             // users who are online
    u32 m_lastCheckedSignedInUserMask;  // users who were signed in, as last time checked by checkPlayerStatusChanges
    u32 m_lastCheckedOnlineUserMask; //users who were online, as last time checked by checkPlayerStatusChanges
    u32 m_lastMessageSystemStatus;
    u32 m_QuestionableSigninChangeNotifReceived;
    u32 m_QuestionableSigninChangeNotifReceivedTick;

    bbool m_bSystemUIShowing;             // system UI present
    bbool m_bNeedToShowSignInUI;          // invoking signin UI necessary
    bbool m_bSigninMessageBoxShowing;           // is retry signin message box showing?
    bbool m_bSigninUIWasShown;            // was the signin ui shown at least once?
    XOVERLAPPED m_Overlapped;              // message box overlapped struct
    LPCWSTR  m_pwstrButtons[2];             // message box buttons
    MESSAGEBOX_RESULT m_MessageBoxResult;  // message box button pressed
    bbool m_bPlayWithoutProfile;
    bbool m_mustShowReturnToMainScreen;
    bbool m_showingReturnToMainScreenMessageBox;
    SystemMessageListener_x360 m_systemMessageListener;

    void     querySigninStatus();              // Query signed in users

};
  

} // namespace ITF

#endif //_ITF_NETWORKSERVICES_X360_H_
#endif //ITF_X360
