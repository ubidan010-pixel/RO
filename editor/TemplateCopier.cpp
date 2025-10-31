#include "precompiled_editor.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifdef ITF_WINDOWS

#ifndef _ITF_TEMPLATECOPIER_H_
#include "editor/TemplateCopier.h"
#endif

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_STRING8_H_
#include "core/string8.h"
#endif //_ITF_STRING8_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DIRECTORY_H_
#include  "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#include <ShlObj.h> // For SHBrowseForFolder

#ifndef _ITF_EDITOR_GUI_TEXTINPUT_H_
#include "editor\GUI.h"
#endif // _ITF_EDITOR_GUI_TEXTINPUT_H_

namespace ITF
{
///////////////////////////////////////////////////////////////////////////////////////////
struct FilenameReplaceDesc
{
    String m_oldName, m_newName;
};
///////////////////////////////////////////////////////////////////////////////////////////
static String backSlashToSlash(const String &_path)
{
    String res(_path);
    res.replace('\\', '/');
    return res;
}

static void replace_Once_IgnoreCase(String &_text, const String &_old, const String &_new)
{
    String text_Lower(_text);
    String old_Lower(_old);
    text_Lower.toLower();
    old_Lower.toLower();

    i32 index=-1;
    text_Lower.strstr(old_Lower.cStr(), bfalse, &index);
    if (index>=0)
    {
        const u16 *afterOldString = _text.cStr()+index+(i32)old_Lower.getLen();
        _text = _text.substr(0, index) + _new + afterOldString;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////
static String renamePath(const String &_path0, const FilenameReplaceDesc &_replace)
{
    String _path = backSlashToSlash(_path0);

    i32 startIndex = _path.rfind((u16)L'/');

    if (startIndex<0)
        startIndex=0;
    else
        startIndex++;

    String directory = _path.substr(0, startIndex);
    String basename = _path.substr(startIndex, (u32)(_path.getLen()-startIndex));

    u32 replaceSourceLen = _replace.m_oldName.getLen();

    if (basename.getLen()>=replaceSourceLen)
    {
        if (basename.substr(0, replaceSourceLen).equals(_replace.m_oldName, btrue) )
        {
            basename = _replace.m_newName+basename.substr(replaceSourceLen, (u32)(basename.getLen()-replaceSourceLen));
            return directory+basename;
        }
    }
    return _path0;
}

///////////////////////////////////////////////////////////////////////////////////////////
static bbool CopyItemFile_Bin(const String &_srcBaseDir, 
                       const String &_destBaseDir, 
                       const String &_srcFilename, 
                       const String &_destFilename,
                       const FilenameReplaceDesc &_filenameReplaceDesc)
{
    bbool isOk = bfalse;
    File *sourceFile = FILEMANAGER->openFile(_srcBaseDir+_srcFilename, ITF_FILE_ATTR_READ);
    HANDLE destFileHandle = INVALID_HANDLE_VALUE;

    
    if (sourceFile)
    {
        String absoluteDestFilename = DATA_FOLDER+renamePath(_destBaseDir+_destFilename,_filenameReplaceDesc);
        destFileHandle = CreateFileW((wchar_t*)absoluteDestFilename.cStr(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,  FILE_ATTRIBUTE_NORMAL, NULL);
    }

    if (sourceFile && (destFileHandle!=INVALID_HANDLE_VALUE))
    {
        DWORD dummy;
        isOk = btrue;
        u32 len = (u32)sourceFile->getLength();
        u8 *l = newAlloc(mId_Temporary, u8[len]);
        sourceFile->read(l, len);
        if (!WriteFile(destFileHandle, l, len, &dummy, NULL))
        {
            isOk = bfalse;
        }
        delete[] l;
        isOk=btrue;
    }

    if (sourceFile)
        FILEMANAGER->closeFile(sourceFile);
    if (destFileHandle!=INVALID_HANDLE_VALUE)
        CloseHandle(destFileHandle);

    return isOk;

}
///////////////////////////////////////////////////////////////////////////////////////////
static bbool CopyItemFileOrDir(const String &_srcBaseDir, 
                        const String &_destBaseDir, 
                        const String &_srcFilename, 
                        const String &_destFilename,
                        const FilenameReplaceDesc &filenameReplaceDesc);

////////////////////////////////////////////////////////////////////////////////////////
//dirs must end with "/"
static bbool CopyItemFile_Text(const String &_srcBaseDir, 
                        const String &_destBaseDir, 
                        const String &_srcFilename, 
                        const String &_destFilename,
                        const FilenameReplaceDesc &_filenameReplaceDesc)
{
    bbool isOk = bfalse;
    File *sourceFile = FILEMANAGER->openFile(_srcBaseDir+_srcFilename, ITF_FILE_ATTR_READ);
    HANDLE destFileHandle = INVALID_HANDLE_VALUE;

    if (sourceFile)
    {
        //Create file. We use OS functions because we don't want path normalization
        String absoluteDestFilename = DATA_FOLDER + renamePath(_destBaseDir+_destFilename, _filenameReplaceDesc);

        destFileHandle = CreateFileW((wchar_t*)absoluteDestFilename.cStr(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,  FILE_ATTRIBUTE_NORMAL, NULL);
    }

    String srcDirSlash = _srcBaseDir;
    String destDirSlash = _destBaseDir;
    srcDirSlash = backSlashToSlash(srcDirSlash);
    destDirSlash = backSlashToSlash(destDirSlash);

    if (sourceFile && (destFileHandle!=INVALID_HANDLE_VALUE))
    {
        u32 fileLen = (u32)sourceFile->getLength();
        char *content8 = newAlloc(mId_Temporary, char[fileLen+1]);
        sourceFile->read(content8, fileLen);
        content8[fileLen] = 0;

        String content(content8);            

        String patternToFind = String("\"")+srcDirSlash;
        String patternToFindLowercase(patternToFind);
        patternToFindLowercase.toLower();


        i32 startIndex=0;

        isOk = btrue;
        for(;;) 
        {
            i32 foundIndex=0;
            String contentLower = content;
            contentLower.toLower();

            u16 *pFound = contentLower.strstr(patternToFindLowercase.cStr(), bfalse, &foundIndex, startIndex);
            if (!pFound)
                break;

            foundIndex += startIndex;
            pFound++;

            String foundPath, replacedPath;

            u16 *pFoundEnd=pFound;
            while(*pFoundEnd!=L'\"' && *pFoundEnd!=0 )
                pFoundEnd++;

            i32 foundEndIndex = i32(pFoundEnd-contentLower.cStr());
            foundPath = content.substr(foundIndex+1, i32(pFoundEnd-pFound));
            replacedPath = foundPath;
            replace_Once_IgnoreCase(replacedPath, srcDirSlash, destDirSlash);
            replacedPath = renamePath(replacedPath, _filenameReplaceDesc);

            LOG("Found %ls => %ls", foundPath.cStr(), replacedPath.cStr());

            content = content.substr(0, foundIndex) + 
                String((u16*)L"\"") + replacedPath + 
                content.substr(foundEndIndex, (i32)contentLower.getLen()-foundEndIndex);

            startIndex = foundIndex+1;

        }


        //Write file
        {
            DWORD dummy;
            String8 content8(content.cStr());
            if (!WriteFile(destFileHandle, content8.cStr(), content8.getLen(), &dummy, NULL))
            {
                isOk = bfalse;
            }            
        }

    }


    if (sourceFile)
        FILEMANAGER->closeFile(sourceFile);
    if (destFileHandle!=INVALID_HANDLE_VALUE)
        CloseHandle(destFileHandle);

    return isOk;

}

///////////////////////////////////////////////////////////////////////////////////////////
String mayAddEndingSlash(const String &_directory)
{
    bbool mustAdd=btrue;
    if (_directory.getLen())
    {
        wchar_t lastChar = (wchar_t)_directory.cStr()[_directory.getLen()-1];
        if (lastChar==L'/' || lastChar==L'\\')
            mustAdd = bfalse;
    }
    if (mustAdd)
        return _directory+String("/");
    return _directory;

}

///////////////////////////////////////////////////////////////////////////////////////////
bbool CopyItemFileOrDir(const String &_srcBaseDir, 
                        const String &_destBaseDir, 
                        const String &_srcFilename, 
                        const String &_destFilename,
                        const FilenameReplaceDesc &_filenameReplaceDesc)
{
    bbool isDirectory = bfalse;
    String combinedSrcName = _srcBaseDir+_srcFilename;
    if (combinedSrcName.getLen())
    {
        wchar_t lastChar = (wchar_t)combinedSrcName.cStr()[combinedSrcName.getLen()-1];
        if (lastChar==L'/' || lastChar==L'\\')
            isDirectory = btrue;
    }

    if (isDirectory)
    {
        //copy directory : recurse for each item
        ITF_VECTOR<String> result_Absolute;
        String rootDir = DATA_FOLDER+_srcBaseDir+_srcFilename;

        //
        WIN32_FIND_DATA FindFileData;
        HANDLE hFind;
        String filepath = rootDir+"*";
        hFind = FindFirstFileEx((LPCWSTR) filepath.cStr(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0 );
        if(hFind == INVALID_HANDLE_VALUE) return bfalse;
        do
        {
            if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
                {

                }
                else
                {
                    String tmp((u16*)FindFileData.cFileName);
                    if (tmp=="." || tmp=="..")
                        continue;
                    result_Absolute.push_back(rootDir+tmp+"/");
                }
            }
            else
            {

                String tmp((u16*)FindFileData.cFileName);
                result_Absolute.push_back(rootDir+tmp);
            }

        } while(FindNextFile(hFind, &FindFileData));
        FindClose(hFind);

        //
        
        for (u32 i=0; i<result_Absolute.size(); i++)
        {
            String a = result_Absolute[i];
            String rel = a.substr(rootDir.getLen(), a.getLen()-rootDir.getLen());

            String absoluteDestFilename = DATA_FOLDER + renamePath(_destBaseDir+_destFilename, _filenameReplaceDesc);
            CreateDirectoryW((wchar_t*)absoluteDestFilename.cStr(), NULL);            

            if (!CopyItemFileOrDir(_srcBaseDir, _destBaseDir, 
                _srcFilename+rel, _destFilename+rel,
                _filenameReplaceDesc))
                return bfalse;
        }
        return btrue;
    }
    else
    {
        //copy files. 2 cases : text or binary.
        String textExtensions[]=
        {
            ".asc",".skl",".pbk",".anm", //anims
            ".act", //actors
            ".fcg", //frises
            ".ilu", ".lua"
        };

        bbool isTextFile=bfalse;
        String extension;
        extension = _srcFilename.strstr((u16*)L".", btrue);
        for (int i=0; i<sizeof(textExtensions)/sizeof(textExtensions[0]); i++)
            if (extension.equals(textExtensions[i], btrue))
            {
                isTextFile = btrue;
                break;
            }
            if (isTextFile)
            {
                LOG("CopyItemFileOrDir : TEXT : %ls=>%ls", (_srcBaseDir+_srcFilename).cStr(), (_destBaseDir+_destFilename).cStr());
                return CopyItemFile_Text(_srcBaseDir, _destBaseDir,
                    _srcFilename, _destFilename,
                    _filenameReplaceDesc);
            }
            else
            {
                LOG("CopyItemFileOrDir : BINARY : %ls=>%ls", (_srcBaseDir+_srcFilename).cStr(), (_destBaseDir+_destFilename).cStr());
                return CopyItemFile_Bin(_srcBaseDir, _destBaseDir,
                    _srcFilename, _destFilename,
                    _filenameReplaceDesc);

            }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////
bbool TemplateCopier::copyTemplateAndDependencies(const String &_templateFile,
                                  const String &_destinationDirectory,
                                  const String &_newTemplateName,
                                  String &_copiedFilename)
{
    //We actually copy the directory itself, so we remove the basename
    String templateSource = _templateFile;
    String templateDest = _destinationDirectory;
    String templateNewName=_newTemplateName;
    FilenameReplaceDesc filenameReplaceDesc;

    templateDest = mayAddEndingSlash(templateDest);
    templateSource = backSlashToSlash(templateSource);
    templateDest = backSlashToSlash(templateDest);
    i32 lastSlashPos = templateSource.rfind(L'/');
    if (lastSlashPos<0)
        return bfalse;
    String srcBasename = templateSource.substr(lastSlashPos+1, templateSource.getLen()-lastSlashPos-1);
    templateSource = templateSource.substr(0, lastSlashPos+1);


    String oldName = templateSource.substr(0, templateSource.getLen()-1);
    u16 *lastSrcSlash = oldName.strstr((u16*)L"/", btrue);
    if (!lastSrcSlash)
        return bfalse;
    oldName =String(lastSrcSlash+1);

    filenameReplaceDesc.m_oldName = oldName;
    filenameReplaceDesc.m_newName = templateNewName;

    FilePath::transformToRelative(templateSource);
    FilePath::transformToRelative(templateDest);

    bbool r = CopyItemFileOrDir(templateSource, 
        templateDest,
        "", 
        "",
        filenameReplaceDesc);

    _copiedFilename = DATA_FOLDER+templateDest+renamePath(srcBasename, filenameReplaceDesc);
    return r;

}
///////////////////////////////////////////////////////////////////////////////////////////
bbool TemplateCopier::copyTemplateAndDependenciesWithGUI()
{
    String userPath("Template/*.*");
    PIDLIST_ABSOLUTE projectItemID;

    //choose file to clone
    HWND hWndParent = (HWND)SYSTEM_ADAPTER->getMainWindow();

    String backslashedDataFolder = DATA_FOLDER;
    backslashedDataFolder.replace('/', '\\');

    if(FAILED(SHParseDisplayName((wchar_t*)backslashedDataFolder.cStr(), NULL, &projectItemID, 0, NULL)))
    {
        ITF_ERROR("Could not find project folder");
        return bfalse;
    }

    if(FILEMANAGER->fileChooser(bfalse, userPath, L"Actor or Frieze config\0*.act;*.fcg\0"))
    {
        //choose destination
        BROWSEINFO b;
        TCHAR displayName[MAX_PATH];
        TCHAR outPath[MAX_PATH];

        ITF_MemSet(&b, 0, sizeof(b));

        b.hwndOwner = hWndParent;
        b.lpszTitle = L"Choose destination directory";
        b.pszDisplayName = displayName;
        displayName[0]=0;
        b.pidlRoot = projectItemID;
        b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN;
        
        //IF you trigger an exception in Windows, because of an unimplemented interface, just ignore the exception.
        PIDLIST_ABSOLUTE idlist = SHBrowseForFolder(&b);
        if (!idlist)
            return bfalse;
        if (!SHGetPathFromIDList(idlist, outPath))
            return bfalse;

        String copiedFilename;

        String newName;
        if (!GUI_TextInput::inputText("New class name", "Enter new class name", newName))
            return bfalse;

        bbool r= copyTemplateAndDependencies(userPath, mayAddEndingSlash(String((u16*)outPath))+newName, newName, copiedFilename); 

        if (r)
        {
            String extension = FilePath::getExtension(copiedFilename);

            FilePath::normPath(copiedFilename);
            if (PLUGINGATEWAY)
                PLUGINGATEWAY->onFileDrop(copiedFilename);
            if (EDITOR)
                EDITOR->onFileDrop(copiedFilename);

        }
        else
        {
            ITF_ERROR("Impossible to copy the archives");
        }

        return r;

    }
    else
    {
        return bfalse;
    }

}

}


#endif
