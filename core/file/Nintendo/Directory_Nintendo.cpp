#include "precompiled_core.h"

#include "core/file/Directory.h"
#include "core/file/Path.h"
#include "core/file/FileServer.h"

namespace ITF
{
    bbool Directory::HasDot(const FILE_FIND_DATA& t_Data)
    {
        NOT_IMPLEMENTED_ON_NINTENDO
        return bfalse;
    }

    HANDLE Directory::FindFirstFileStr(const String& _pz_FileName, FILE_FIND_DATA* _pt_Data)
    {
        NOT_IMPLEMENTED_ON_NINTENDO
        return static_cast<HANDLE>(-1);
    }

    bbool Directory::DirectoryExists(const String& _fulldirName)
    {
        NOT_IMPLEMENTED_ON_NINTENDO
        return bfalse;
    }
}
