#ifndef _ITF_ZSERIALIZER_DEP_H_
#define _ITF_ZSERIALIZER_DEP_H_

#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif //SERIALIZEROBJECTPARSER_H

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/FilePath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF
{

template< typename T >
class CSerializerDep : public T
{
public:
    CSerializerDep()
    : m_pathTags( NULL )
    {
    }

    void setStringTags( const char** tags ) { m_pathTags = tags; }

    virtual void Serialize( const char* tag, String& str )
    {
        ITF_ASSERT_MSG( validatePathTag( tag ), "The tag %s should be a path", tag );
        T::Serialize( tag, str );
        if ( str.strstr( '.', btrue ) != NULL && validatePath( str ) )
            m_paths.insert( str );
    }

    virtual void Serialize( const char* tag, Path& _path )
    {
        T::Serialize( tag, _path );
        if ( validatePath( _path ) )
            m_paths.insert( _path );
    }

    virtual void Serialize( const char* tag, struct LocalisationId& id )
    {
        T::Serialize( tag, id );
        m_loc.insert( id );
    }

    const ITF_SET< Path >&           getDependencyPaths() const { return m_paths; }
    const ITF_SET< LocalisationId >& getDependencyLoc() const { return m_loc; }

private:
    bool validatePathTag( const char* tag ) const
    {
        if ( m_pathTags == NULL )
            return true;
        for ( u32 i = 0; m_pathTags[i] != NULL; ++i )
            if ( !strcmp( m_pathTags[i], tag ) )
                return bfalse;
        return btrue;
    }

    bool validatePath( const String& path )
    {
        // Removing anm files without directory (in the case of animation path and animation filename separated in act file)
        if ( FilePath::getFilename( path ).isEmpty() )
            return false;
        if ( FilePath::getExtension( path ) == "anm" && FilePath::getDirectory( path ).isEmpty() )
            return false;
        return true;
    }

    bool validatePath( const Path& path )
    {
        PathString_t pathName;

        path.getString(pathName);

        return validatePath( pathName );
    }

    ITF_SET< Path >              m_paths;
    ITF_SET< LocalisationId >    m_loc;
    const char**                 m_pathTags;
};

}

#endif //_ITF_ZSERIALIZER_DEP_H_
