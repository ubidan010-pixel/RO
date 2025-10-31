#include "core/enginecommon.h"

#ifndef _ITF_LOADING_SEQUENCE_COOKER_H_
#include "LoadingSequenceCooker.h"
#endif //_ITF_LOADING_SEQUENCE_COOKER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

namespace ITF
{

bbool LoadingSequenceCooker::cook( const String& platform, const String& filename )
{
    String cookFile = FILESERVER->getCookedNamePlatform( filename, platform );

    if ( !cookerFactory::fileNeedsCook( filename, cookFile, platform ) )
        return btrue;

    File* sequence = FILEMANAGER->openFile( filename, ITF_FILE_ATTR_READ );

    if ( sequence == NULL )
        return bfalse;

    u32 length = static_cast< u32 >( sequence->getLength() );
    char* buffer = new char[length];

    if ( !sequence->read( buffer, length ) )
    {
        SF_DEL( sequence );
        return bfalse;
    }

    ITF_VECTOR< String >    paths;
    ITF_SET< String >       uniquePaths;

    {
        char*   ptr = buffer;
        char*   cursor = buffer;
        char*   end = buffer + length;

        while ( cursor < end )
        {
            if ( *cursor == '\r' )
            {
                *cursor = '\0';
                cursor += 1;
                continue;
            }
            if ( *cursor == '\n' )
            {
                *cursor = '\0';
                cursor += 1;
                paths.push_back( ptr );
                ITF_ASSERT_MSG( uniquePaths.insert( ptr ).second, "Duplicated path %s, it will provoc a seek", ptr );
                ptr = cursor;
                continue;
            }
            cursor += 1;
        }
        if ( ptr != end )
        {
            paths.push_back( ptr );
            ITF_ASSERT_MSG( uniquePaths.insert( ptr ).second, "Duplicated path %s, it will provoc a seek", ptr );
        }
        uniquePaths.clear();
    }
    SF_DEL_ARRAY( buffer );
    SF_DEL( sequence );

    ArchiveMemory am;
    u32           size = paths.size();

    am.serialize( size );
    for ( u32 i = 0; i < paths.size(); ++i )
        paths[i].serialize( am );
    sequence = FILEMANAGER->openFile( cookFile, ITF_FILE_ATTR_WRITE | ITF_FILE_ATTR_CREATE_NEW );
    if ( sequence == NULL )
        return bfalse;

    bbool success = sequence->write( am.getData(), am.getSize() );

    SF_DEL( sequence );
    return success;
}

}
