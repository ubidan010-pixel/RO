#include "precompiled_FileManager_ITF.h"

#include <commdlg.h>

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#include "adapters/FileManager_ITF/Win/FileManager_ITF_win.h"

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#include <ShlObj.h> // For SHBrowseForFolder

namespace ITF
{
    bbool FileManager_ITF_Win::folderChooser(const String& _title, const String& _startPath, String& _result) const 
    {
        PIDLIST_ABSOLUTE projectItemID;

        //choose file to clone
        HWND hWndParent = (HWND)SYSTEM_ADAPTER->getMainWindow();

        String backslashedDataFolder = DATA_FOLDER;// + _startPath; // because the windows folder cannot go higher than startPath... strange
        backslashedDataFolder.replace('/', '\\');

        FullScreenDialog_ProtectScope fullscreenGuard;

        if(FAILED(SHParseDisplayName((wchar_t*)backslashedDataFolder.cStr(), NULL, &projectItemID, 0, NULL)))
        {
            ITF_ERROR("Could not find project folder");
            return bfalse;
        }

        //choose destination
        BROWSEINFO b;
        TCHAR displayName[MAX_PATH];
        TCHAR outPath[MAX_PATH];

        ITF_MemSet(&b, 0, sizeof(b));

        b.hwndOwner = hWndParent;
        b.lpszTitle = LPCWSTR(_title.cStr());
        b.pszDisplayName = displayName;
        displayName[0]=0;
        b.pidlRoot = projectItemID;
        b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
        
        //IF you trigger an exception in Windows, because of an unimplemented interface, just ignore the exception.
        PIDLIST_ABSOLUTE idlist = SHBrowseForFolder(&b);
        if (!idlist)
            return bfalse;
        if (!SHGetPathFromIDList(idlist, outPath))
            return bfalse;

        _result.setText((u16*)outPath);
        _result.toLower();
        _result.replace('\\', '/');
        _result.replace(DATA_FOLDER, "");
        return btrue;
    }

    bbool FileManager_ITF_Win::fileChooser( bbool _save, String& _defaultAndResult, const wchar_t* _filter /*= NULL*/, const String& _workingPathAbsolute /*= ""*/ ) const
    {
        String defaultFile_Absolute;        
        if(_workingPathAbsolute.isEmpty())
        {
            String directory = FilePath::getDirectory(_defaultAndResult);
            if(!directory.isEmpty())
                TranslatePath(defaultFile_Absolute, _defaultAndResult);
            else
                defaultFile_Absolute = m_lastInitialDir + _defaultAndResult;
        }
        else
            defaultFile_Absolute = _workingPathAbsolute + _defaultAndResult;
        defaultFile_Absolute.replace('/', '\\');

        TCHAR szFile[MAX_PATH];
        ZeroMemory(&szFile, sizeof(szFile));
        if(!_defaultAndResult.isEmpty())
            if(wcscpy_s(szFile, MAX_PATH, (TCHAR*)defaultFile_Absolute.cStr()) != 0)
                return bfalse;

        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = (TCHAR*)_filter;
        ofn.nFilterIndex = 1;
        ofn.Flags =  OFN_NOCHANGEDIR | OFN_EXPLORER;

        BOOL ret = FALSE;

        FullScreenDialog_ProtectScope fullscreenGuard;

        if(_save)
        {
            ofn.Flags |= OFN_OVERWRITEPROMPT;
            ret = GetSaveFileName(&ofn);
        }
        else
        {
            ofn.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;
            ret = GetOpenFileName(&ofn);
        }

        if(ret)
        {
            _defaultAndResult = FilePath::normalizePath(String((u16*)szFile));
            m_lastInitialDir = FilePath::getDirectory(_defaultAndResult);
        }

        return (ret == TRUE);
    }

}
