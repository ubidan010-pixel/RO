#include "precompiled_core.h"

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_


namespace ITF
{

    bbool Directory::HasDot(const FILE_FIND_DATA& t_Data)
    {
        if(wcscmp(t_Data.cFileName, L".") && wcscmp(t_Data.cFileName, L".."))
            return btrue;

        return bfalse;
    }

    HANDLE Directory::FindFirstFileStr(const String& _pz_FileName, FILE_FIND_DATA *_pt_Data)
    {

        return FindFirstFile((wchar_t *) _pz_FileName.cStr(), _pt_Data);
    }


    bbool Directory::getFilesMatch(ITF_VECTOR<String> &vAbsoluteFiles,const String& rootDir,const String& _wildcard)
    {
        WIN32_FIND_DATA FindFileData;
        HANDLE hFind;
        String filepath = rootDir+_wildcard;

        hFind = FindFirstFileEx((LPCWSTR) filepath.cStr(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0 );

        if(hFind == INVALID_HANDLE_VALUE) return bfalse;

        do
        {
            if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            {

            }
            else
            {

                String tmp((u16*)FindFileData.cFileName);
                vAbsoluteFiles.push_back(rootDir+tmp);
            }

        } while(FindNextFile(hFind, &FindFileData));


        FindClose(hFind);

        return btrue;
    }


    bbool Directory::DirectoryExists(const String& _fulldirName)
    {
        DWORD r = ::GetFileAttributes((LPCWSTR)_fulldirName.cStr());
        if(r != INVALID_FILE_ATTRIBUTES)
        {
            return ((r & FILE_ATTRIBUTE_DIRECTORY) > 0);
        }
        else
        {
            return bfalse;
        }
    }

    
    //this function is not recursive
    bbool Directory::getDirectories(const String& rootDir,ITF_VECTOR<String>& list)
    {
        HANDLE            h_Find;
        FILE_FIND_DATA t_Data;

        h_Find = FindFirstFileStr(rootDir+"*.*", &t_Data);

        if(h_Find == INVALID_HANDLE_VALUE) return bfalse;

        do
        {
            if((t_Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(t_Data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            {

                if (HasDot(t_Data))
                {
                    String tmp((u16*)t_Data.cFileName);
                    String directory= rootDir+tmp;
                    list.push_back(directory);
                }
            }
           

        } while(ITF_FindNextFile(h_Find, &t_Data));

        ITF_FindClose(h_Find);

        return (list.size()>0);
    }


    bbool Directory::deleteDirectory(const String& _szDirectory)
    {
       return (RemoveDirectoryW((LPCWSTR)_szDirectory.cStr())==S_OK);
    }

    //this function is not recursive
    void Directory::deleteDirectories(const String& rootDir)
    {
        HANDLE            h_Find;
        FILE_FIND_DATA t_Data;

        h_Find = FindFirstFileStr(rootDir+"/*.*", &t_Data);

        if(h_Find == INVALID_HANDLE_VALUE) return;

        do
        {
            if((t_Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(t_Data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            {

                if (HasDot(t_Data))
                {
                    String tmp((u16*)t_Data.cFileName);
                    String directory= rootDir+"/"+tmp;
                    deleteDirectories(directory);

                    deleteDirectory(directory);
                }

            }
            else
            {

                String tmp((u16*)t_Data.cFileName);
                String file= rootDir+"/"+tmp;
				FileManager::get().deleteFile(file);
            }
            

        } while(ITF_FindNextFile(h_Find, &t_Data));

        ITF_FindClose(h_Find);

        deleteDirectory(rootDir);

    }


    u64  Directory::getLastTimeWriteAccess(const String& _szDirectory)
    {
        HANDLE   hDirectory = CreateFile((LPCWSTR)_szDirectory.cStr(), GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS , NULL);

        FILETIME ftCreate, ftAccess, ftWrite;

        u64 writeTime = 0;

        // Retrieve the file times for the file.
        if (GetFileTime(hDirectory, &ftCreate, &ftAccess, &ftWrite))
        {
            writeTime = (u64(ftWrite.dwHighDateTime) << 32) | u64(ftWrite.dwLowDateTime);
        }

        CloseHandle(hDirectory);

        return writeTime;
    }





}