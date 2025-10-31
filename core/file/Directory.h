#ifndef ITF_DIRECTORY_H_
#define ITF_DIRECTORY_H_

#pragma once

#if defined(ITF_MICROSOFT)
using FILE_FIND_DATA = WIN32_FIND_DATA ;
#endif // ITF_MICROSOFT

#if !defined(ITF_MICROSOFT)
using FILE_FIND_DATA = unsigned int; //fake
#endif // !ITF_MICROSOFT

namespace ITF
{
    typedef void (*cbNextFileRelative)(const String& _filename);

    class Directory
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static bbool create(const String& dirName)
        /// @brief  Create a directory. 
        /// @param  dirName Pathname of the directory. 
        /// @return . 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static bbool create(const String& dirName);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Check if a directory exists
        /// @param  _fulldirName FullPathname of the directory. 
        /// @return btrue if the directory exists
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static bbool exists(const String& _fulldirName);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static bbool searchFiles(const String& dirName,const String& wildcard,bbool brecursive)
        /// @brief  Searches for the first files. 
        /// @param  dirName     Pathname of the directory. 
        /// @param  wildcard    The wildcard. 
        /// @param  brecursive  brecursive. 
        /// @return The found files. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////


        static bbool searchFiles(ITF_VECTOR<String> &vFiles,const String& rootDir,const String& wildcard,bbool brecursive,cbNextFileRelative _func = NULL);
        
        static bbool searchFilesRelative(ITF_VECTOR<String> &vRelativeFiles,const String& rootDir,const String& wildcard,bbool brecursive, cbNextFileRelative _funcCB = NULL,String subDir = "");

        static bbool searchFile(char* foundPath, const char* rootDir, const String& toFind, bbool _recursive);

        static bbool getDirectories(const String& _szRootDir,ITF_VECTOR<String>& list);

        static void deleteDirectories(const String& _szRootDirectory);

        static bbool deleteDirectory(const String& _szDirectory);

        static u64   getLastTimeWriteAccess(const String& _szDir);

#if defined(ITF_MICROSOFT)
        static bbool getFilesMatch(ITF_VECTOR<String> &vAbsoluteFiles,const String& rootDir,const String& _wildcard);
#endif // ITF_WINDOWS

    protected:

        static bbool HasDot(const FILE_FIND_DATA& t_Data);

        static HANDLE   FindFirstFileStr(const String& _pz_FileName, FILE_FIND_DATA *_pt_Data);

        static bbool    DirectoryExists(const String& _fulldirName);


        static bbool  ITF_FindNextFile(HANDLE _h_Find, FILE_FIND_DATA *_pt_Data);
  
        static void  ITF_FindClose(HANDLE _h_Find);

    private:


        static bbool searchFileInternal(char* foundPath, const char* rootDir, const String& toFind, bbool _recursive);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static bool createRecursive(const char* name)
        /// @brief  Creates a recursive directory. 
        /// @param [in]     name    If non-null, the name. 
        /// @return btrue if it succeeds, bfalse if it fails. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static bbool createRecursive(const char* name);
    };

}

#ifdef ITF_WINDOWS

class DirectoryChangeScope
{
public:
    DirectoryChangeScope()
    {
        ::GetCurrentDirectory(MAX_PATH, szWorkindDirectory);
    }


    ~DirectoryChangeScope()
    {
        ::SetCurrentDirectory(szWorkindDirectory);
    }


    TCHAR szWorkindDirectory[MAX_PATH];

};
#endif // ITF_WINDOWS

#endif // ITF_DIRECTORY_H_
