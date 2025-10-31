#ifndef _ITF_REPORTISSUEUTILS_H_
#define _ITF_REPORTISSUEUTILS_H_


namespace ITF
{

    class Pickable;

    class reportIssueUtils
    {
     public:

#ifdef ITF_WINDOWS
        static void generateTrac();
        static void generateCrashDump(const wchar_t* szApplicationDirectory,const wchar_t* _fullpath,const wchar_t* _shortname,u32 clVersion);
#endif // ITF_WINDOWS
        static bbool investigateMapLoadingStuck(Pickable* pick);
        static u32 clVersion;
        static bbool m_investigateMapStuck;
    protected:

        static void generateCommon(String& videoPath,bbool tracMode);

        
    };

}

#endif //_ITF_REPORTISSUEUTILS_H_
