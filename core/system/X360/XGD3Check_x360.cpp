#include "precompiled_core.h"

#ifndef _ITF_XGD3CHECK_H_
#include "core/system/x360/xgd3Check_x360.h"
#endif //_ITF_XGD3CHECK_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

namespace ITF
{
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    u32 xgd3Check::init()
    {
        DWORD dwResult = XSecurityCreateProcess( CIV_DEFAULT_THREAD );

        if ( dwResult == ERROR_DISK_CORRUPT)
        {
            XShowAntiPiracyUI( TRUE ); // Show message box and terminate the title
            return dwResult;
        }

        if( dwResult != ERROR_SUCCESS )
        {
            XSecurityCloseProcess();
            return dwResult;
        }

        m_bInit = btrue;
        return dwResult;
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef ITF_SUPPORT_CHEAT
    void xgd3Check::injectError(u32 _hashError)
    {
        XSecurityInjectErrors(_hashError,0);
    }
#endif //ITF_SUPPORT_CHEAT

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    u32 WINAPI threadLoop(void* pParams)
    {
        //this will never exit
        XShowAntiPiracyUI( TRUE );
        return 0;
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void xgd3Check::showPiracy()
    {
        m_thread = THREADMANAGER->createThread(threadLoop, this, ThreadSettings::m_settings[eThreadId_TaskManager]);//use the task manager as thread id
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void xgd3Check::poll()
    {
        if (!m_checkStarted)
            return;

        XSECURITY_FAILURE_INFORMATION FailureInformation = {0};
        FailureInformation.dwSize = sizeof( XSECURITY_FAILURE_INFORMATION );
        u32 dwResult = XSecurityGetFailureInfo( &FailureInformation );

        if (dwResult == ERROR_FILE_CORRUPT || FailureInformation.dwFailedHashes > 0)
        {
            showPiracy();
        }

        if (FailureInformation.fComplete)
        {
            m_checkStarted = bfalse;
        }

        if( dwResult != ERROR_SUCCESS )
        {
            return;  // This is not considered a CIV failure. Possibilities include I/O contention, out of memory or locking.
        }
        else
        {
            m_checkStarted = bfalse;
        }

        if (!m_checkStarted)
        {
            shutdown();//as soon as it's done ,we close
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void xgd3Check::startCheck()
    {
        if (!m_bInit)
            return;

        // Call XSecurityVerify periodically, when title is not reading from the disc
        u32 dwResult = XSecurityVerify( 200,  // Cap the execution time to 200 ms
            NULL,
            NULL ); // No completion routine


        if ( dwResult == ERROR_DISK_CORRUPT)
        {
            showPiracy();
            return;  // This is a CIV failure. Take appropriate anti-piracy action.
        }

        if( dwResult == ERROR_ACCESS_DENIED )
            return;  // This is not considered a CIV failure, it just indicates that inspection is in progress.


        m_checkStarted = btrue;
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void xgd3Check::shutdown()
    {
        XSecurityCloseProcess();
        m_bInit = bfalse;
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
} 