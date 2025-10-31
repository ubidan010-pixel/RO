#include "precompiled_core.h"

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifdef ITF_WINDOWS
    #include <io.h>
    #include <direct.h>
    #include <errno.h>
#endif //ITF_WINDOWS

namespace ITF
{
    bbool Directory::create(const String& dirName)
    {
#ifndef ITF_X360
        StringConverter str(dirName);
        return Directory::createRecursive(str.getChar());
#else
        String finalPath;
        if(FILEMANAGER->TranslatePath(finalPath, dirName,0))
        {
            finalPath.replace('/','\\');
            StringConverter str(finalPath);
            return Directory::createRecursive(str.getChar());
        }
        return bfalse;
#endif //ITF_X360
    }

    //TODO make it as an adapter ?
#if !defined (ITF_MICROSOFT)
    bbool Directory::createRecursive(const char* path)
    {
        ITF_ASSERT_CRASH(false, "Not implemented");
        return bfalse;
    }

    bbool  Directory::ITF_FindNextFile(HANDLE _h_Find, FILE_FIND_DATA *_pt_Data)
    {
        ITF_ASSERT_CRASH(false, "Not implemented");
        return bfalse;
    }

    void  Directory::ITF_FindClose(HANDLE _h_Find)
    {
        ITF_ASSERT_CRASH(false, "Not implemented");
    }

#else
    bbool Directory::createRecursive(const char* path)
    {
        int len;
        char *new_path;
        bbool ret = btrue;

        new_path = newAlloc(mId_Temporary,char[strlen(path)+1]);
        strcpy(new_path,path);

        len = int(strlen(new_path));
        while (len && new_path[len - 1] == '\\')
            new_path[len - 1] = 0;

        //TODO cross platform call
        while (!::CreateDirectoryA(new_path,NULL))
        {
            CHAR *slash;
            DWORD last_error = GetLastError();
            if (last_error == ERROR_ALREADY_EXISTS)
                break;

            if (last_error != ERROR_PATH_NOT_FOUND)
            {
                ret = bfalse;
                break;
            }

            slash = strrchr(new_path,'\\');
            if (!slash)
            {
                slash = strrchr(new_path,'/');
                if (!slash)
                {
                    ret = bfalse;
                    break;
                }
            }

            len = int(slash - new_path);
            new_path[len] = 0;
            if (!createRecursive(new_path))
            {
                ret = bfalse;
                break;
            }
            new_path[len] = '\\';
        }
        SF_DEL_ARRAY(new_path);
        return ret;
    }

    bbool  Directory::ITF_FindNextFile(HANDLE _h_Find, FILE_FIND_DATA *_pt_Data)
    {
        return FindNextFile(_h_Find, _pt_Data);
    }

    void  Directory::ITF_FindClose(HANDLE _h_Find)
    {
        FindClose(_h_Find);
    }
#endif //ITF_PS3


    bbool Directory::exists(const String& _fulldirName)
    {
        return DirectoryExists(_fulldirName);
    }

    bbool Directory::searchFiles(ITF_VECTOR<String> &vFiles,const String& rootDir,const String& wildcard,bbool brecursive,cbNextFileRelative _funcNextFileRelative)
    {
#ifndef ITF_WINDOWS
        ITF_ASSERT(0); //parsing a hw directory on console must be avoided,move to file manager to ensure bundle usage
#endif //ITF_WINDOWS

        ITF_VECTOR<String> vRelativesFiles;
        bbool b = searchFilesRelative(vRelativesFiles, rootDir, wildcard, brecursive,_funcNextFileRelative);

        ITF_VECTOR<String>::const_iterator ite = vRelativesFiles.begin();
        while(ite != vRelativesFiles.end())
        {
            String fullPath;
            fullPath =  rootDir;
            fullPath += *ite;

            vFiles.push_back(fullPath);

            ++ite;
        }

        return b;

    }

    bbool Directory::searchFile(char* foundPath, const char* rootDir, const String& toFind, bbool _recursive)
    {
#ifdef ITF_WINDOWS
        DirectoryChangeScope directoryChange;
#endif //ITF_WINDOWS
        bbool bResult = searchFileInternal(foundPath,rootDir,toFind,_recursive);
        return bResult;
    }


    bbool Directory::searchFileInternal(char* foundPath, const char* rootDir, const String& toFind, bbool _recursive)
    {
        // Looks for a file in a folder and all of its subfolders
        // Yes, it's oldschool way, but it works fine & fast
        // Be vigilant this function is not threadsafe

        #ifdef ITF_WINDOWS
            String findMe(toFind);
            findMe.toUpper();

            { // PROCESS ALL FILES WITHIN CURRENT DIR
                struct _finddata_t c_file;
                intptr_t hFile;
                const char* szPath = rootDir;
                int len = int(strlen(foundPath));
                if (len > 0 && foundPath[len-1] != '/')
                    strcat(foundPath, "/");
                strcat(foundPath, szPath);
                if (_chdir(szPath))
                {
                    ITF_ERROR("Can't change current directory to %s", szPath);
                    return bfalse;
                }
                if( (hFile = _findfirst( "*.*", &c_file )) == -1L )
                {
                   return bfalse;
                }
                int iRes =0;
                do
                {
                    char FileExt[33];
                    FileExt[32] = 0;
                    if (!(c_file.attrib & _A_SUBDIR))
                    {
                        String fileName(c_file.name);
                        fileName.toUpper();
                        if (fileName == findMe)
                        {
                            int len = int(strlen(foundPath));
                            if (len > 0 && foundPath[len-1] != '/')
                                strcat(foundPath, "/");
                            return btrue;
                        }
                    }
                    iRes = _findnext( hFile, &c_file );
                    switch (iRes)
                    {
                        case EINVAL:
                        case ENOMEM:
                            ITF_ASSERT_MSG(bfalse, "Error occured while scanning folder: could not perform _findnext instruction.");
                        break; 
                    }
                } while( iRes == 0 );
                _findclose( hFile );
            }
            if (!_recursive)
                return bfalse;

            { // PROCESS ALL SUBFOLDERS
                struct _finddata_t c_file;
                intptr_t hFile;

                if( (hFile = _findfirst( "*.*", &c_file )) == -1L )
                {
                   return bfalse;
                }
                int iRes =0;
                do
                {
                    char FileExt[33];
                    FileExt[32] = 0;
                    if (c_file.attrib & _A_SUBDIR)
                    {
                        if (*c_file.name != '.')
                        {
                            if (searchFileInternal(foundPath, c_file.name, toFind, _recursive))
                                return btrue;
                            if (_chdir(".."))
                            {
                            }
                            else
                            {
                                size_t index = strlen(foundPath);
                                while(index>0)
                                {
                                    if (foundPath[index] == '/')
                                    {
                                        foundPath[index] = 0;
                                        break;
                                    }
                                    index--;
                                }
                            }
                        }
                    }
                    iRes = _findnext( hFile, &c_file );
                    switch (iRes)
                    {
                        case EINVAL:
                        case ENOMEM:
                            ITF_ASSERT_MSG(bfalse, "Error occured while scanning folder: could not perform _findnext instruction.");
                        break; 
                    }
                } while( iRes == 0 );
                _findclose( hFile );
            }
            return bfalse;
        #else //ITF_WINDOWS
            ITF_ASSERT_CRASH(false, "Not implemented");
            return bfalse;
        #endif //ITF_WINDOWS
    }


#if !defined(ITF_MICROSOFT)
    bbool Directory::searchFilesRelative(ITF_VECTOR<String> &vFiles,const String& rootDir,const String& wildcard,bbool brecursive, cbNextFileRelative _funcCB,String subDir/* = ""*/)
    {
        ITF_ASSERT_CRASH(false, "Not implemented");
        return bfalse;
    }
#else
 
    bbool Directory::searchFilesRelative(ITF_VECTOR<String> &vFiles,const String& rootDir,const String& wildcard,bbool brecursive,cbNextFileRelative _funcCB, String subDir/* = ""*/)
    {
        HANDLE            h_Find;
        FILE_FIND_DATA t_Data;

        String searchPath;
        searchPath =  rootDir;
        searchPath += subDir;
        searchPath += wildcard;
        

        h_Find = FindFirstFileStr(searchPath, &t_Data);

        if(h_Find == INVALID_HANDLE_VALUE) return bfalse;

        do
        {
             if((t_Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(t_Data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            {

                if (HasDot(t_Data))
                {
                    String tmp((u16*)t_Data.cFileName);
                    String subDirCpy = subDir;
                    subDirCpy+=tmp;
                    subDirCpy+=FilePath::Separator;
                    searchFilesRelative(vFiles,rootDir,wildcard,brecursive,_funcCB,subDirCpy);
                }
          
            }
             else
             {
                 String filename;
                 filename = subDir;
                 String tmp((u16*)t_Data.cFileName);
                 filename+=tmp;

                 if (_funcCB)
                    _funcCB(filename);

                 vFiles.push_back(filename);
             }

        } while(ITF_FindNextFile(h_Find, &t_Data));

        ITF_FindClose(h_Find);

        return btrue;

    }
#endif // ITF_MICROSOFT
}

