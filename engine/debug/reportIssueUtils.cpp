#include "precompiled_engine.h"

#ifdef ITF_WINDOWS
    #include <shellapi.h>
#endif // ITF_WINDOWS

#ifndef _ITF_REPORTISSUEUTILS_H_
    #include "engine/debug/reportIssueUtils.h"
#endif // _ITF_REPORTISSUEUTILS_H_

#ifdef ITF_SUPPORT_VIDEOCAPTURE
    #ifndef _ITF_VIDEOCAPTURE_H_
        #include "engine/display/videoCapture.h"
    #endif //_ITF_VIDEOCAPTURE_H_
#endif //ITF_SUPPORT_VIDEOCAPTURE

#include "core/UnicodeTools.h"

namespace ITF
{
    u32 reportIssueUtils::clVersion = 0;
    bbool reportIssueUtils::m_investigateMapStuck = bfalse;

#ifdef ITF_WINDOWS
    #define MAX_CHAR_INFO 512

    void reportIssueUtils::generateTrac()
    {
       String videoPath ="";
       generateCommon(videoPath,btrue);

       TCHAR szExePath[MAX_PATH];
       TCHAR szApplicationDirectory[MAX_PATH];
       ::GetCurrentDirectory(MAX_PATH, szApplicationDirectory);

       swprintf_s( szExePath,MAX_PATH ,L"%s\\CrashDumpSender.exe",szApplicationDirectory);
       TCHAR args[MAX_CHAR_INFO];
       swprintf_s(args,MAX_CHAR_INFO,L"TRACMODE %d %s", clVersion, videoPath.wcharCStr());


       ShellExecute(0, NULL,szExePath,args,L"" , SW_NORMAL);
    }

    void reportIssueUtils::generateCrashDump(const wchar_t* szApplicationDirectory,const wchar_t* _fullpath,const wchar_t* _shortname,u32 clVersion)
    {
        String videoPath ="";
        generateCommon(videoPath,bfalse);

        //parameters list
        TCHAR args[MAX_CHAR_INFO];

        swprintf_s(args,MAX_CHAR_INFO,L"%s %s %d %s",_fullpath,_shortname,clVersion, videoPath.wcharCStr());
        TCHAR crashsenderDumpTxt[MAX_PATH];
        swprintf_s( crashsenderDumpTxt,MAX_PATH ,L"%s\\CrashDumpSender.exe",szApplicationDirectory);

        ShellExecute(0, NULL,crashsenderDumpTxt,args,L"" , SW_NORMAL);
    }

    void reportIssueUtils::generateCommon(String& videoPath,bbool tracMode)
    {
        //generate movie :
#ifdef ITF_SUPPORT_VIDEOCAPTURE
        VideoCapture* pVideoCapture = VideoCapture::getInstance();
        if (pVideoCapture && pVideoCapture->videoDebugEnable())
        {
            pVideoCapture->getFileNameCapture(videoPath);
            pVideoCapture->stop();

            if (tracMode)
                pVideoCapture->start(GFX_ADAPTER->getScreenWidth(),GFX_ADAPTER->getScreenHeight());
        }
#endif //ITF_SUPPORT_VIDEOCAPTURE
    }
#endif // ITF_WINDOWS

    bbool reportIssueUtils::investigateMapLoadingStuck(Pickable* _pick)
    {
        #ifndef ITF_FINAL
        if (!m_investigateMapStuck)
            return bfalse;

        String8 message;
        message.setTextFormat("%s object is blocking map loading:", _pick->getUserFriendly().cStr());
        ResourceGroup* grp = _pick->getResourceGroup_Slow();
        if (grp)
        {
            if (!grp->loadingQuerried())
                message += "\n- object resources were not asked to be loaded - ";
            else
                grp->dumpUnloadedResources(message);
        }
        else
            message += "\n- object has no resource group - ";

        ITF_WARNING(_pick, bfalse, message.cStr());

        #ifdef ITF_WINDOWS
            if (MessageBox(NULL, (LPCWSTR)(String(message.cStr()).cStr()), (LPCWSTR)(String(_pick->getUserFriendly().cStr()).cStr()), MB_ICONWARNING|MB_OKCANCEL) == IDCANCEL)
                exit(0);
        #endif 
        #endif
        return btrue;
    }

};

