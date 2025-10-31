#ifndef _ITF_COMMON_DEP_FUNCTIONS_H_
#define _ITF_COMMON_DEP_FUNCTIONS_H_

namespace ITF
{
   
template< typename T >
void retrieveLocalisationDependencies( CSerializerDep< T >& serializer, DepCollection& col )
{
    const ITF_SET< Path >& paths = serializer.getDependencyPaths();
    PathString_t fileName;

    for ( ITF_SET< Path >::const_iterator it = paths.begin(); it != paths.end(); ++it )
        if ( !it->isEmpty() )
        {
            it->getString(fileName);
            col.add( fileName );
        }

    const ITF_SET< LocalisationId >&    locs = serializer.getDependencyLoc();
    ITF_VECTOR< String >                texts;
    Path                                path;
    PathString_t pathFilename;

    for ( ITF_SET< LocalisationId >::const_iterator it = locs.begin(); it != locs.end(); ++it )
    {
        path = LOCALISATIONMANAGER->getAudioPath( *it );
        if ( !path.isEmpty() )
        {
            path.getString(pathFilename);
            col.add( pathFilename );
        }
        LOCALISATIONMANAGER->getDependencies( *it, texts );
        for ( u32 j = 0; j < texts.size(); ++j )
            col.add( texts[j] );
        texts.clear();
    }
}

}

#endif  //_ITF_COMMON_DEP_FUNCTIONS_H_
