#include "precompiled_core.h"

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef ITF_FINAL
#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif //_ITF_WARNINGMANAGER_H_
#endif // ITF_FINAL

#ifdef ITF_WII
#include <wctype.h>
#endif

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

// include for towlower
#include <wctype.h>

namespace ITF
{
    String  FilePath::normalizePath(const String& path)
    {
        String ret = path;
        normPath(ret);
        return ret;
    }
    
    void FilePath::normPath(const u16 *_path, u16 _destBuffer[256])
    {
        const u16 *pSrc = _path;
        u16 val;
        u16 *pDst = _destBuffer;


        do 
        {
            val = *pSrc;
            pSrc++;
            if (val>=(u16)L'A' && val<=(u16)L'Z')
                val += (u16)L'a'-(u16)L'A';
            else if (val == (u16)L'\\')
                val = L'/';
            *pDst = val;
            pDst++;
        } while (val);
    }

    void FilePath::splitPathAndNormalize(const u16 *_path, u16 _destDir[256], u16 _destBasename[], u32 _destBasenameBufferSize)
    {
        u16 *destDirPos = _destDir;
        u16 *destBasenamePos = _destBasename;
        u16 *destBasenameEnd = _destBasename + _destBasenameBufferSize;
        u16 val;
        u16 * lastSlash = NULL;
        u16 lastChar = 0;
        bbool mustEmptyDir = bfalse;

        //copy directory.NB : we handle double slash
        do 
        {
            val = *_path;
            _path++;
            if (val>=(u16)L'A' && val<=(u16)L'Z')
                val += L'a'-L'A';
            else if (val==(u16)L'\\')
                val = L'/';
            if (val==(u16)L'/')
            {
                if (lastChar==(u16)L'/')
                {
                    continue;
                }
                lastSlash = destDirPos;
            }

            lastChar = val;
            *destDirPos = val;
            destDirPos++;
        } while (val);
        if (lastSlash)
        {
            _path = lastSlash+1;
        }
        else
        {
            mustEmptyDir = btrue;
            _path = _destDir;
        }

        //copy filename
        do 
        {
            val = *_path;
            _path++;
            *destBasenamePos = val;
            destBasenamePos++;
            if (destBasenamePos==destBasenameEnd)
            {
                destBasenameEnd[-1]=0;
                if (lastSlash)
                {
                    lastSlash[1] = 0;
                }
                if (mustEmptyDir)
                    _destDir[0] = 0;
                return;
            }
        } while (val);
        if (lastSlash)
        {
            lastSlash[1] = 0;
        }
        if (mustEmptyDir)
            _destDir[0] = 0;
    }

    void FilePath::splitPathAndNormalize(const char *_path, char _destDir[256], char _destBasename[], u32 _destBasenameBufferSize)
    {
        char *destDirPos = _destDir;
        char *destBasenamePos = _destBasename;
        char *destBasenameEnd = _destBasename + _destBasenameBufferSize;
        char val;
        char * lastSlash = NULL;
        char lastChar = 0;
        bbool mustEmptyDir = bfalse;

        //copy directory.NB : we handle double slash
        do 
        {
            val = *_path;
            _path++;
            if (val>='A' && val<='Z')
                val += 'a'-'A';
            else if (val=='\\')
                val = '/';
            if (val=='/')
            {
                if (lastChar=='/')
                {
                    continue;
                }
                lastSlash = destDirPos;
            }

            lastChar = val;
            *destDirPos = val;
            destDirPos++;
        } while (val);
        if (lastSlash)
        {
            _path = lastSlash+1;
        }
        else
        {
            mustEmptyDir = btrue;
            _path = _destDir;
        }

        //copy filename
        do 
        {
            val = *_path;
            _path++;
            *destBasenamePos = val;
            destBasenamePos++;
            if (destBasenamePos==destBasenameEnd)
            {
                destBasenameEnd[-1]=0;
                if (lastSlash)
                {
                    lastSlash[1] = 0;
                }
                if (mustEmptyDir)
                    _destDir[0] = 0;
                return;
            }
        } while (val);
        if (lastSlash)
        {
            lastSlash[1] = 0;
        }
        if (mustEmptyDir)
            _destDir[0] = 0;
    }


    void FilePath::normPath(String& path)
    {
        path.replace('\\', '/');
        path.toLower();
    }

    void FilePath::normPath(String8& path)
    {
        path.replace('\\', '/');
        path.toLower();
    }

    bbool FilePath::isNormalized(const String& path)
    {
        
        StringConverter str(path);

        if(strrchr(str.getChar(), '\\') != NULL)
        {
            return bfalse;
        }

        return btrue;
    }

    bbool FilePath::isValid( const String& path )
    {
        // check for space in path
        return path.strstr(' ') == NULL;
    }

    bbool FilePath::isFilePath( const String& path )
    {
        //ITF_ASSERT(isNormalized(path));

        if (path.isEmpty())
        {
            return false;
        }

        StringConverter str(path);
        const char* sStr = str.getChar();
        const u32   uSize = str.getCharLength();

        return sStr[uSize-1] != Separator || sStr[uSize-1] != '\\';
    }

    // This function compute the path before the next to last separator
    bbool  FilePath::getParentDirectory(const String& path, String &parent)
    {
        ITF_ASSERT(isNormalized(path));

        StringConverter str(getDirectory(path));
        const char*     pPath = str.getChar();
        const u32       uSize = str.getCharLength();
        u32             uSubSize = 0;

        // Reverse loop on all char off the path
        // Start at n-1 because we want to ignore the last separator
        for(i32 i = (uSize - 1 - 1); i > 0; --i)
        {
            // We found the Separator between path and it's parent
            if(Separator == pPath[i])
            {
                uSubSize = u32(i)+1; // + 1 because we include the separator
                break;
            }
        }

        // Root reached
        if(uSubSize == 0)
        {
            return bfalse;
        }

        char pParent[_MAX_DIR];
#ifdef ITF_PS3
        NOT_IMPLEMENTED_ON_PS3
#else
#ifdef ITF_CTR
        strcpy(pParent, pPath);
#endif
#endif//ITF_PS3
        pParent[uSubSize] = 0;
        parent = String(pParent);
        return btrue;
    }

    void FilePath::getParentDirList(const String& path, ITF_VECTOR<String> & _list)
    {
        String pathCopy(path);
        String curParent;

        _list.clear();
        while(getParentDirectory(pathCopy, curParent))
        {
            _list.push_back(curParent);
            pathCopy = curParent;
        }
    }

    String  FilePath::getDirectory(const String& path)
    {
        String workPath = normalizePath(path);

        i32 lastSlash = workPath.rfind('/');

        if (lastSlash == String::npos)
        {
            return String::emptyString;
        }

#ifndef ITF_FINAL
        if(*(workPath.cStr()) == '/')
        {
            ITF_WARNING(NULL, bfalse, "Bad path: '%s'", StringToUTF8(path).get());
            return String(workPath.cStr()+1,lastSlash);
        }
#endif // ITF_FINAL
        return String(workPath,lastSlash+1);
    }

     /// C:/test/bla.txt     =>  bla.txt
    String  FilePath::getFilename(const String& path)
    {
        ITF_ASSERT(isNormalized(path));
        i32 lastSlash = path.rfind('/');

        if (lastSlash == String::npos)
            return path;

        return String(path.cStr()+lastSlash+1);
    }

    static const u16* u16rchr(const u16* str, u16 ch)
    {
        const u16* lastOccurrence = nullptr;
        while (*str)
        {
            if (*str == ch)
            {
                lastOccurrence = str;
            }
            ++str;
        }
        return lastOccurrence;
    }


    /// C:/test/bla.txt     =>  bla.txt
    String  FilePath::getFilenameWithoutExtension(const String& _path)
    {   
        ITF_ASSERT(isNormalized(_path));

        const u16 *path = _path.cStr();
        const u16 *basenamePos = path;
        const u16 *pathPos = path;

        //iterate to find basename
        u16 currentChar;
        do
        {
            currentChar = *pathPos;
            pathPos++;
            if (currentChar==(u16)L'/' || currentChar==(u16)L'\\')
            {
                basenamePos = pathPos;
            }

        } while (currentChar);


        //find dot. If exists, isolate basename without extension in path
        const u16 *dotPos = u16rchr(basenamePos, L'.');
        if (dotPos)
        {
            return _path.substr(u32(basenamePos-path), u32(dotPos-basenamePos));
        }
        else
        {
            //otherwise, return basename
            if (basenamePos == path)
            {
                //if basename is path, just return input (optimization for string dictionary);
                return _path;
            }

            return basenamePos;
        }
    }


    String  FilePath::getExtension(const String& path)
    {       
        ITF_ASSERT(isFilePath(path));

        String filename = getFilename(path);

        i32 dotPos = filename.rfind('.');
        if (dotPos>=0)
        {
            i32 l = filename.getLen()-(dotPos+1);
            return filename.substr((u32)dotPos+1,l);
        }
        else
        {
            return "";
        }
    }

    String FilePath::getChangedExtension(const String & path, const String & _newExtension)
    {
        String str = path;
        changeExtension(str, _newExtension);
        return str;
    }

    void FilePath::changeExtension(String & path, const String & _newExtension)
    {       
        normPath(path);

        i32 dotPos = path.rfind('.');
        if (dotPos >= 0)
        {
            if (_newExtension.isEmpty() || _newExtension.cStr()[0] != L'.')
                dotPos++;
            path.truncate(dotPos);
            path += _newExtension;
        } else
        {
            if (!_newExtension.isEmpty() && _newExtension.cStr()[0] != L'.')
                path += L'.';
            path += _newExtension;
        }
    }

    void FilePath::changeExtension(String8 & path, const char * _newExtension)
    {       
        normPath(path);

        i32 dotPos = path.rfind('.');
        if (dotPos >= 0)
        {
            if (_newExtension==NULL || _newExtension[0] != '.')
                dotPos++;
            path.truncate(dotPos);
            if (_newExtension!=NULL)
                path += _newExtension;
        } else if (_newExtension!=NULL && _newExtension[0]!='\0') // add it only if not NULL
        {
            if (_newExtension[0]!='.')
                path += '.';
            path += _newExtension;
        }
    }


    /// Change path extension

    String  FilePath::getDrive(const String& path)
    {
     
        ITF_ASSERT(isNormalized(path));

    #ifdef ITF_PS3
            wchar_t szDrive[_MAX_DRIVE];
            szDrive[0] = 0;
            const wchar_t * wszPath = (wchar_t *)path.cStr();
            if (wszPath && wszPath[0]==L'/')
            {
                const wchar_t * secondSlash = wcschr(wszPath+1, L'/');
                if (secondSlash!=0)
                {
                    wcsncpy(szDrive, wszPath, secondSlash-wszPath);
                    return String((u16 *)szDrive);
                }
            }
            return "";
    #else
        #if defined(ITF_MICROSOFT)
            wchar_t szDrive[_MAX_DRIVE];
            szDrive[0] = 0;
            _wsplitpath((const wchar_t*)path.cStr(), szDrive,  NULL,  NULL, NULL);
            String szResult((u16*)szDrive);
            return szResult;
        #endif //!defined(ITF_IPAD) && !defined(ITF_CTR)
            return String::emptyString;
    #endif
    }

    bbool FilePath::isAbsolute(const String& _path)
    {
        u32 dataLength = FileServer::get().getDataDepot().getLen();
        if (dataLength==0)
        {
            return bfalse;
        }

        if (_path.getLen()<dataLength)
        {
            return bfalse;
        }
        const u16 *p1 = _path.cStr();
        const u16 *p2 = FileServer::get().getDataDepot().cStr();

        for (u32 i=0; i<dataLength; i++)
        {
            u16 d1 = towlower(p1[i]);
            u16 d2 = towlower(p2[i]);
            if (d1==(u16)L'\\')
                d1 = (u16)L'/';
            if (d2==(u16)L'\\')
                d2 = (u16)L'/';
            if (d1!=d2)
                return bfalse;
        }
        return btrue;
    }

    bbool FilePath::transformToRelative( String& _path )
    {
        if ( isAbsolute(_path) )
        {             
            u32 dataLength = FileServer::get().getDataDepot().getLen();
            u32 totalLength = _path.getLen();
            _path = _path.substr(dataLength, totalLength-dataLength);
            return btrue;
        }
        return bfalse;
    }

} // namespace ITF
