#include "precompiled_SourceControlAdapter_Perforce.h"

#ifndef ITF_FINAL

#ifndef _ADAPTERSOURCECONTROL_PERFORCE_H_
#	include "adapters/Adapter_SourceControl_Perforce/SourceControlAdapter_Perforce.h"
#endif //_ADAPTERSOURCECONTROL_PERFORCE_H_

#ifndef _ITFCLIENTUSER_PERFORCE_H_
#	include "adapters/Adapter_SourceControl_Perforce/ITFClientUser.h"
#endif
#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_
#ifndef _ITFCLIENTAPI_PERFORCE_H_
#	include "adapters/Adapter_SourceControl_Perforce/ITFClientApi.h"
#endif
namespace  ITF
{
	SourceControlAdapter_Perforce::SourceControlAdapter_Perforce()
	{
		//m_ProjectRootDirectory= FILESERVER->getWorkingDepot();
	}

	SourceControlAdapter_Perforce::~SourceControlAdapter_Perforce()
	{
	}

	bbool SourceControlAdapter_Perforce::checkout(const String& _filenameRelative)
	{
	
		DWORD dwAttributes = GetFileAttributes((LPCWSTR)_filenameRelative.cStr());
		if(!(dwAttributes & FILE_ATTRIBUTE_READONLY))
		{
			return true;
		}
		if(FILESERVER->getWorkingDepot().getLen()==0)
		{
			WarningManager::addWarning(WarningCategory_Default, NULL,NULL,"[PERFORCE] No project root defined, cannot checkout files");
			return false;
		}
		
		ITFClientUser itf_P4User;
		ITFClientApi itf_P4Api;
		Error e;

		StrBuf sb1;
		ProjectDir = String8(FILESERVER->getWorkingDepot().cStr());
		sb1.Set(ProjectDir.cStr());
		itf_P4Api.SetCwd(&sb1);
		itf_P4Api.Init(&e);
		if(e.Test())
            return bfalse;
        char * argv[1];		
        
        char* file = _filenameRelative.getCharCopy();		        
        argv[0]=file;
       
		itf_P4Api.SetArgv( 1, argv );		
		itf_P4Api.Run("edit",&itf_P4User);
		itf_P4Api.Final(&e);
		
        SF_DEL_ARRAY(file);
		return true;
	}

    bbool SourceControlAdapter_Perforce::add(const String& _filenameRelative)
    {
        if(FILESERVER->getWorkingDepot().getLen()==0)
        {
            WarningManager::addWarning(WarningCategory_Default, NULL,NULL,"[PERFORCE] No project root defined, cannot checkout files");
            return false;
        }

        ITFClientUser itf_P4User;
        ITFClientApi itf_P4Api;
        Error e;

        StrBuf sb1;
        ProjectDir = String8(FILESERVER->getWorkingDepot().cStr());
        sb1.Set(ProjectDir.cStr());
        itf_P4Api.SetCwd(&sb1);
        itf_P4Api.Init(&e);
        if(e.Test())
            return false;
        char * argv[1];		

        char* file = _filenameRelative.getCharCopy();		        
        argv[0]=file;

        itf_P4Api.SetArgv( 1, argv );		
        itf_P4Api.Run("add",&itf_P4User);
        itf_P4Api.Final(&e);

        SF_DEL_ARRAY(file);
        return true;
    }

} // namespace ITF

#endif //ITF_FINAL