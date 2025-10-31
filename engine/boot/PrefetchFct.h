#ifndef _ITF_PREFETCH_FCT_H_
#define _ITF_PREFETCH_FCT_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef ITF_MAP_H_
#include "core/container/extendedMap.h"
#endif //ITF_MAP_H_

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif //ITF_TEMPLATECLIENTHANDLER_H

#include "engine/singleton/Singletons.h"

#include "engine/TemplateManager/TemplateDatabase.h"

namespace ITF
{

class PrefetchFactory
{
public:
    struct Params
    {
        Params(): res( NULL ), hdl( NULL ), refType( 0 ), type( static_cast< Resource::ResourceType >( -1 ) ) {}

        enum
        {
            Logical     = 1 << 0,
            Physical    = 1 << 1,
        };

        Resource*               res;
        TemplateClientHandler*  hdl;
        u8                      refType;
        Resource::ResourceType  type;
    };

    typedef bool ( *Callback )( const Path&, Params& params );

private:
    typedef ITF_MAP< String, Callback > Callbacks;

public:
    static bool registerCallback( const String& ext, Callback cb );
    static bool prefetch( const Path& path, Params& params );

private:
    static Callbacks m_callbacks;
};

template< typename T >
bool PrefetchTemplate( const Path& path, PrefetchFactory::Params& params )
{
    ITF_ASSERT_CRASH( params.hdl != NULL, "Invalid template client handler instance" );
    params.hdl->addUsedTemplate( path.getStringID() );
    return TEMPLATEDATABASE->getTemplate< T >( params.hdl, path, true ) != NULL;
}

}

#endif  //_ITF_PREFETCH_FCT_H_
