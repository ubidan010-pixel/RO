#include "precompiled_engine.h"

#include <xonline.h>

#ifndef _ITF_NETWORKSERVICES_X360_H_
#include "engine/NetworkServices/X360/NetworkServices_x360.h"
#endif //_ITF_NETWORKSERVICES_X360_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_SYSTEMADAPTER_X360_H_
#include "adapters/SystemAdapter_x360/SystemAdapter_x360.h"
#endif //_ITF_SYSTEMADAPTER_X360_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#define MESSAGE_NO_SIGNED_IN_PLAYER_TITLE L"No gamer profile"
#define MESSAGE_NO_SIGNED_IN_PLAYER L"You are not signed in. In order to save progress, you will need to be signed in.\nDo you want to sign in now?"
#define MESSAGE_PLAYER_SIGNED_OUT_TITLE L"Sign-in change"
#define MESSAGE_PLAYER_SIGNED_OUT L"A sign-in change occurred.\nYou are now being returned to the title screen."
#define MESSAGE_YES L"Yes"
#define MESSAGE_NO L"No"
#define MESSAGE_OK L"Ok"

//#define _WANT_SOMEONE_SIGNEDIN

#include "../make/Techno/x360/rayman.spa.h"


namespace ITF
{
    static LPCWSTR g_pYESNOLabels[]=
    {
        MESSAGE_YES,
        MESSAGE_NO
    };

    static LPCWSTR g_pOKLabel[]=
    {
        MESSAGE_OK
    };

    static DWORD g_acceptedNotifications[]=
    {
        XN_SYS_SIGNINCHANGED,
        XN_SYS_UI,
    };

    NetworkServices_x360::NetworkServices_x360() :
        m_lastMessageSystemStatus(0),
        m_systemMessageListener(g_acceptedNotifications, ARRAYSIZE(g_acceptedNotifications)),
        m_QuestionableSigninChangeNotifReceived(0),
        m_QuestionableSigninChangeNotifReceivedTick(0)
        {
        }

        void NetworkServices_x360::initialize( u32 dwMinUsers,u32 dwMaxUsers,bbool bRequireOnlineUsers,u32 dwSignInPanes )
        {
            // Sanity check inputs
            ITF_ASSERT( dwMaxUsers <= 4 && dwMinUsers <= dwMaxUsers );
            ITF_ASSERT( dwSignInPanes <= 4 && dwSignInPanes != 3 );

            static_cast<SystemAdapter_X360*>(SYSTEM_ADAPTER)->registerSystemMessageListener(&this->m_systemMessageListener);

            m_bSystemUIShowing = bfalse;
            m_bSigninMessageBoxShowing = bfalse;
            m_bNeedToShowSignInUI = bfalse;
            m_bRequireOnlineUsers = bfalse;
            m_bSigninUIWasShown = bfalse;
            m_bPlayWithoutProfile = bfalse;
            m_mustShowReturnToMainScreen = bfalse;
            m_showingReturnToMainScreenMessageBox = bfalse;

            // Assign variables
            m_dwMinUsers = dwMinUsers;
            m_dwMaxUsers = dwMaxUsers;
            m_bRequireOnlineUsers = bRequireOnlineUsers;
            m_dwSignInPanes = dwSignInPanes;
            m_pwstrButtons[0] = L"Ok";
            m_pwstrButtons[1] = L"Cancel";

            // Query who is signed in
            querySigninStatus();
            m_lastCheckedSignedInUserMask = m_dwSignedInUserMask;
            m_lastCheckedOnlineUserMask = m_dwOnlineUserMask;

            ITF_VERIFY(XPresenceInitialize(4)==ERROR_SUCCESS);
        }

        void NetworkServices_x360::terminate()
        {
            static_cast<SystemAdapter_X360*>(SYSTEM_ADAPTER)->unregisterSystemMessageListener(&this->m_systemMessageListener);
        }

        //--------------------------------------------------------------------------------------
        void NetworkServices_x360::askForSignIn( u32 maxUsers )
        {
            m_bNeedToShowSignInUI = btrue;
            if(maxUsers >= 1 && maxUsers <= 4)
                m_dwSignInPanes = maxUsers;
        }

        //--------------------------------------------------------------------------------------
        bbool NetworkServices_x360::isShowingMessageBox()
        {
            return m_bSigninMessageBoxShowing || 
                m_showingReturnToMainScreenMessageBox ||
                m_bSystemUIShowing;
        }

        //--------------------------------------------------------------------------------------
        // Name: QuerySigninStatus()
        // Desc: Query signed in status of all users.
        //--------------------------------------------------------------------------------------
        void NetworkServices_x360::querySigninStatus()
        {
            m_dwSignedInUserMask = 0;
            m_dwOnlineUserMask = 0;

            // Count the signed-in users
            m_dwNumSignedInUsers = 0;
            m_dwFirstSignedInUser = U32_INVALID;

            LOG(" ------ querySigninStatus ------ ");
            for( UINT nUser = 0;
                nUser < XUSER_MAX_COUNT;
                nUser++ )
            {
                XUSER_SIGNIN_STATE State = XUserGetSigninState( nUser );

                LOG("user %d, %s", nUser, State == eXUserSigninState_NotSignedIn  ?  "NotSigned": (State == eXUserSigninState_SignedInToLive ? "Live": "Local"));
                if( State != eXUserSigninState_NotSignedIn )
                {
                    // Check whether the user is online
                    BOOL bUserOnline =
                        State == eXUserSigninState_SignedInToLive;

                    m_dwOnlineUserMask |= bUserOnline << nUser;

                    // If we want Online users only, only count signed-in users
                    if( !m_bRequireOnlineUsers || bUserOnline )
                    {
                        m_dwSignedInUserMask |= ( 1 << nUser );

                        if( m_dwFirstSignedInUser == U32_INVALID )
                        {
                            m_dwFirstSignedInUser = nUser;
                        }

                        ++m_dwNumSignedInUsers;
                    }
                }
            }

#ifdef _WANT_SOMEONE_SIGNEDIN
            // check to see if we need to invoke the signin UI
            m_bNeedToShowSignInUI = !areUsersSignedIn() && !m_bPlayWithoutProfile &&!SYSTEM_ADAPTER->isTrialMode();
#endif

        }

        void NetworkServices_x360::checkPlayerStatusChanges()
        {
            // Query who is signed in
            u32 signInChange = m_dwSignedInUserMask ^ m_lastCheckedSignedInUserMask;
            u32 onlineSignInChange = m_dwOnlineUserMask ^ m_lastCheckedOnlineUserMask;

            m_lastCheckedSignedInUserMask = m_dwSignedInUserMask;
            m_lastCheckedOnlineUserMask = m_dwOnlineUserMask;

            bbool mustShowTrcMessage=bfalse;

            for (u32 index=0; index < GAMEMANAGER->getMaxPlayerCount(); index++)
            {    
                //Check profile sign-in changes
                if (signInChange & (1<<index))
                {
                    if (GAMEMANAGER->userSignedInOut(index, isUserSignedIn(index), bfalse))
                        mustShowTrcMessage = btrue;
                }
                if (onlineSignInChange & (1<<index))
                {
                    if (GAMEMANAGER->userSignedInOut(index, isUserOnline(index), btrue))
                        mustShowTrcMessage = btrue;
                }
            }
            if (mustShowTrcMessage)
            {
                m_mustShowReturnToMainScreen = btrue;
            }

        }

        //--------------------------------------------------------------------------------------
        // Name: Update()
        // Desc: Does required per-frame processing for signin
        //--------------------------------------------------------------------------------------
        u32 NetworkServices_x360::update()
        {
            m_lastMessageSystemStatus = 0;
            const u32 dwTimeBeforeTrustingSignInChangedNotif = 1000;

            // Check for system notifications
            DWORD dwNotificationID;
            ULONG_PTR ulParam;
            while (m_systemMessageListener.popMessage(dwNotificationID, ulParam))
            {
                switch( dwNotificationID )
                {
                case XN_SYS_SIGNINCHANGED:
                    
                    LOG(" XN_SYS_SIGNINCHANGED => %d ", ulParam);
                    querySigninStatus();

                    if( m_dwSignedInUserMask == 0 )
                    {
                        ++m_QuestionableSigninChangeNotifReceived;
                        if( m_QuestionableSigninChangeNotifReceived == 1 )
                        {
                            // If a second signin changed notification arrives within dwTimeBeforeTrustingSignInChangedNotif,
                            // trust it instead of this one
                            m_QuestionableSigninChangeNotifReceivedTick = GetTickCount();
                        }
                        else if( ( m_QuestionableSigninChangeNotifReceived > 1 )  && 
                            ( GetTickCount() - m_QuestionableSigninChangeNotifReceivedTick <= dwTimeBeforeTrustingSignInChangedNotif ) )
                        {
                            m_lastMessageSystemStatus |= SIGNIN_USERS_CHANGED;
                        }
                    }
                    else
                    {
                    m_lastMessageSystemStatus |= SIGNIN_USERS_CHANGED;
                    }

                    // Reset dwQuestionableSigninChangeNotifReceived to 0 if the notification is valid
                    if( m_lastMessageSystemStatus & SIGNIN_USERS_CHANGED )
                    {
                        m_QuestionableSigninChangeNotifReceived = 0;
                        LOG(" XN_SYS_SIGNINCHANGED => ACCEPTED ", ulParam);
                    } 
                    else
                    {
                        LOG(" XN_SYS_SIGNINCHANGED => IGNORED ", ulParam);
                    }

                    break;

                case XN_SYS_UI:
                    m_lastMessageSystemStatus |= SYSTEM_UI_CHANGED;
                    m_bSystemUIShowing = static_cast<BOOL>( ulParam );

#ifdef _WANT_SOMEONE_SIGNEDIN
                    // check to see if we need to invoke the signin UI
                    m_bNeedToShowSignInUI = !areUsersSignedIn() && !m_bPlayWithoutProfile;
#endif
                    SYSTEM_ADAPTER->setSystemPauseFlag(m_bSystemUIShowing);
                    break;

                default:
                    ITF_ASSERT_MSG(bfalse, "Unhandled message");
                    break;

                } // switch( dwNotificationID )
            } // if( XNotifyGetNext() )


            if( ( m_QuestionableSigninChangeNotifReceived >= 1 )  && 
                ( GetTickCount() - m_QuestionableSigninChangeNotifReceivedTick > dwTimeBeforeTrustingSignInChangedNotif ) )
            {
                m_lastMessageSystemStatus |= SIGNIN_USERS_CHANGED;
                m_QuestionableSigninChangeNotifReceived = 0;
                LOG(" XN_SYS_SIGNINCHANGED => ACCEPTED WITH TIMEOUT", ulParam);
            }

            // check for player sign in/out and LIVE connection/disconnection
            if (( m_lastMessageSystemStatus & SIGNIN_USERS_CHANGED ) != 0)
            {
                checkPlayerStatusChanges();
            }

            //
            if (!isShowingMessageBox() && m_mustShowReturnToMainScreen)
            {

                u32 dwResult;

                ZeroMemory( &m_Overlapped, sizeof( XOVERLAPPED ) );   

                static LocalisationId titleID, messageID, okID;
                titleID.value   = 4501;
                messageID.value = 4502;
                okID.value      = 4061;

                static const String TitleText = LOCALISATIONMANAGER->getText(titleID);
                static const String MessageText = LOCALISATIONMANAGER->getText(messageID);
                static const String ButtonText = LOCALISATIONMANAGER->getText(okID);

                g_pOKLabel[0] = (LPWSTR)ButtonText.cStr();

                dwResult = XShowMessageBoxUI( XUSER_INDEX_ANY,
                    (LPWSTR)TitleText.cStr(),   // Message box title
                    (LPWSTR)MessageText.cStr(), // Message
                    ARRAYSIZE(g_pOKLabel),                                  // Number of buttons
                    g_pOKLabel,                                             // Button captions
                    0,                                                      // Button that gets focus
                    XMB_ERRORICON,                                          // Icon to display
                    &m_MessageBoxResult,                                    // Button pressed result
                    &m_Overlapped );

                if( dwResult != ERROR_IO_PENDING )
                {
                    ITF_ASSERT_MSG( 0,"Failed to invoke message box UI, error %d\n", dwResult );
                } else
                {
                    m_mustShowReturnToMainScreen = bfalse;
                    m_showingReturnToMainScreenMessageBox = btrue;
                }
            }

            if (m_showingReturnToMainScreenMessageBox && XHasOverlappedIoCompleted(&m_Overlapped))
            {
                m_showingReturnToMainScreenMessageBox = bfalse;
            }

#ifdef _WANT_SOMEONE_SIGNEDIN
            //If the sign-in UI was shown but still noone is logged in, we show a warning to ask for loggin in
            //if the user says ok, we show the sign-in UI. If no, we play without profile

            if( !isShowingMessageBox() && m_bSigninUIWasShown && !areUsersSignedIn() && !m_bPlayWithoutProfile )
            {
                u32 dwResult;

                ZeroMemory( &m_Overlapped, sizeof( XOVERLAPPED ) );


                dwResult = XShowMessageBoxUI( XUSER_INDEX_ANY,
                    MESSAGE_NO_SIGNED_IN_PLAYER_TITLE,   // Message box title
                    MESSAGE_NO_SIGNED_IN_PLAYER, // Message
                    ARRAYSIZE(g_pYESNOLabels),// Number of buttons
                    g_pYESNOLabels,             // Button captions
                    0,                          // Button that gets focus
                    XMB_ERRORICON,              // Icon to display
                    &m_MessageBoxResult,        // Button pressed result
                    &m_Overlapped );

                if( dwResult != ERROR_IO_PENDING )
                {
                    ITF_FATAL_ERROR( "Failed to invoke message box UI, error %d\n", dwResult );
                }

                m_bSystemUIShowing      = btrue;
                m_bSigninMessageBoxShowing    = btrue;
            }

            // Wait until the message box is discarded, then either exit or show the signin UI again
            if( m_bSigninMessageBoxShowing && XHasOverlappedIoCompleted( &m_Overlapped ) )
            {
                m_bSigninMessageBoxShowing = bfalse;

                if( XGetOverlappedResult( &m_Overlapped, NULL, TRUE ) == ERROR_SUCCESS )
                {
                    switch( m_MessageBoxResult.dwButtonPressed )
                    {
                    case 0:     // Reboot to the launcher
                        showSignInUI();
                        m_bSigninUIWasShown = bfalse;
                        break;

                    case 1:     // Show the signin UI again
                        m_bPlayWithoutProfile = btrue;
                        break;
                    }
                }
                else
                    m_bPlayWithoutProfile = btrue;
            }
#endif

            // Check to see if we need to invoke the signin UI
            if( !isShowingMessageBox() && m_bNeedToShowSignInUI )
            {
                u32 flag = 0;
                if(m_dwSignInPanes==1)      flag |= XSSUI_FLAGS_ALLOW_SIGNOUT;
                if(m_bRequireOnlineUsers)   flag |= XSSUI_FLAGS_SHOWONLYONLINEENABLED;

                u32 ret = XShowSigninUI(m_dwSignInPanes, flag);
                if( ret != ERROR_SUCCESS )
                {
                    ITF_ASSERT_MSG(0, "Failed to invoke signin UI, error %d\n", ret );
                }
                else
                {
                    m_bSystemUIShowing  = btrue;
                    m_bSigninUIWasShown = btrue;
                    m_bNeedToShowSignInUI = bfalse;
                }
            }

            return m_lastMessageSystemStatus;
        }

        //--------------------------------------------------------------------------------------
        // Name: CheckPrivilege()
        // Desc: Test to see if a user has a required privilege
        //--------------------------------------------------------------------------------------
        bbool NetworkServices_x360::checkPrivilege( u32 dwController, XPRIVILEGE_TYPE priv )
        {
            bbool bResult;
            return ( XUserCheckPrivilege( dwController, priv, &bResult ) == ERROR_SUCCESS ) && bResult;
        }

        //--------------------------------------------------------------------------------------
        // Update presence info for the player connected
        //--------------------------------------------------------------------------------------
        void NetworkServices_x360::setPresenceInfo_GameType( GameType _gameType , u32 _padIndex)
        {
            u32 gameType=(_gameType==Ranked)? X_CONTEXT_GAME_TYPE_RANKED : X_CONTEXT_GAME_TYPE_STANDARD;
            setPresenceInfo_Custom(X_CONTEXT_GAME_TYPE, gameType, _padIndex);
        }

        void NetworkServices_x360::setPresenceInfo_GameMode( u32 _gameMode, u32 _padIndex )
        {
            setPresenceInfo_Custom(X_CONTEXT_GAME_MODE, _gameMode, _padIndex);
        }

        void NetworkServices_x360::setPresenceInfo_State( u32 _state, u32 _padIndex )
        {
            setPresenceInfo_Custom(X_CONTEXT_PRESENCE, _state, _padIndex);
        }

        void NetworkServices_x360::setPresenceInfo_Custom( u32 _context, u32 _value, u32 _padIndex )
        {
            if(isUserSignedIn(_padIndex))
            {
                XUserSetContext(_padIndex, _context, _value);     
            }
        }

        bbool NetworkServices_x360::getUserFromControllerIndex( u32 dwController , User* user)
        {
            if(isUserSignedIn(dwController) && user != NULL)
            {
                XUSER_SIGNIN_INFO info ;
                if ( XUserGetSigninInfo(dwController, XUSER_GET_SIGNIN_INFO_OFFLINE_XUID_ONLY , &info) == ERROR_SUCCESS )
                {
                    user->id = info.xuid;
                    user->isValid = btrue;
                    return btrue;
                }
            }

            if(user)
                user->isValid = bfalse;
            return bfalse;
        }

        bbool NetworkServices_x360::compareUsers( const User * user1, const User* user2 )
        {
            if( !user1 || !user2 || !user1->isValid || !user2->isValid )
                return bfalse;

            return IsEqualXUID ( user1->id  , user2->id );
        }


} // namespace ITF

