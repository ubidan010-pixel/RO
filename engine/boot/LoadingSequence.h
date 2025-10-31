#ifndef _ITF_LOADING_SEQUENCE_H_
#define _ITF_LOADING_SEQUENCE_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

namespace ITF
{

class String;

class LoadingSequence
{
public:
    struct PrefetchResource
    {
        PrefetchResource(): refType( 0 ), type( static_cast< Resource::ResourceType >( -1 ) ) {}

        u8                      refType;
        StringID                resource;
        Resource::ResourceType  type;
    };

private:
    typedef ITF_LIST< PrefetchResource > ResRef;

    struct SequenceLoadInfo
    {
        String                  filename;
        bool                    setStatic;
        TemplateClientHandler*  handler;
    };

public:
    static void load( const String& filename, bool setStatic, TemplateClientHandler* hdl = NULL );
    static void load( const ITF_VECTOR< Path >& files, ITF_VECTOR< PrefetchResource >& prefetched, TemplateClientHandler* hdl = NULL );
    static void loadAsync( const String& filename, bool setStatic, TemplateClientHandler* hdl = NULL );
    static void releaseResources();
    static void releaseResources( ITF_VECTOR< PrefetchResource >& prefetched );
    static bool isRunning() { return m_asyncThread != NULL; }
    static bool isLoaded();
    static void disableDelayRelease();

#ifdef ITF_WINDOWS
    static bbool cookSequence( const String& platform, const String& filename );
#endif //ITF_WINDOWS

private:
    static u32 WINAPI loadProc( void* data );

    static Thread* volatile         m_asyncThread;
    static SequenceLoadInfo         m_info;
    static TemplateClientHandler    m_clientHandler;
    static ResRef                   m_resources;
    static bool                     m_delayRelease;
};

}

#endif  //_ITF_LOADING_SEQUENCE_H_
