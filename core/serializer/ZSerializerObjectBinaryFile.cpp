
#include "precompiled_core.h"
#include "ZSerializerObjectBinaryFile.h"
#include "ZSerializerFile.h"

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_


namespace ITF
{

CSerializerObjectBinaryFile::CSerializerObjectBinaryFile() : CSerializerObjectBinary()
{
    m_serializer = NULL;
    m_bDestroySerializer = btrue;
}

bbool CSerializerObjectBinaryFile::open( const Path& _file, bbool _read )
{
    PathString_t fileName;

    _file.getString(fileName);

    return open( FILESERVER->getCookedName( fileName ), _read );
}

bbool CSerializerObjectBinaryFile::open( const Path& _file, const String& _szPlatform, bbool _read )
{
    PathString_t fileName;

    _file.getString(fileName);

    return open( FILESERVER->getCookedNamePlatform( fileName, _szPlatform ), _read );
}

bool CSerializerObjectBinaryFile::open( const String& cookedName, bbool _read )
{
    m_properties |= ESerializerProperty_Binary;

    if (_read)
    {
        m_read = btrue;

        File* pFile = FILEMANAGER->openFile(cookedName,ITF_FILE_ATTR_READ);

        if (!pFile)
        {
            return false;
        }

        u32 length = (u32) pFile->getLength();

        ArchiveMemory* pArchiveMemory = newAlloc(mId_Archive, ArchiveMemory(length, length ,btrue));
        pFile->read(pArchiveMemory->getPtrForWrite(),length);
        pArchiveMemory->rewindForReading();
        m_serializer = pArchiveMemory;

        FILEMANAGER->closeFile(pFile);
        return true;
    }


    ITF_ASSERT_MSG(0,"write mode not supported yet");
    return false;
}

} // namespace ITF
