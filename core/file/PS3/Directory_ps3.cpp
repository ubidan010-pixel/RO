#include "precompiled_core.h"

#include  "core/file/Directory.h"

namespace ITF
{
    bbool Directory::HasDot(const FILE_FIND_DATA& t_Data)
    {
        NOT_IMPLEMENTED_ON_PS3
        /*
        if(strcmp(t_Data.cFileName, ".") && strcmp(t_Data.cFileName, ".."))
            return btrue;
        */
        return bfalse;
    }

    HANDLE Directory::FindFirstFileStr(const String& _pz_FileName, FILE_FIND_DATA *_pt_Data)
    {
        NOT_IMPLEMENTED_ON_PS3
       // StringConverter str(_pz_FileName);
        //return FindFirstFile(str.getChar(), _pt_Data);
        return INVALID_HANDLE_VALUE;
    }

    bbool Directory::DirectoryExists(const String& _fulldirName)
    {
        NOT_IMPLEMENTED_ON_PS3
            /*
        StringConverter str(_fulldirName);
        DWORD r = GetFileAttributes(str.getChar());
        if(r != U32_INVALID)
        {
            return ((r & FILE_ATTRIBUTE_DIRECTORY) > 0);
        }
        else
        {
            return bfalse;
        }*/

        return bfalse;
    }


}