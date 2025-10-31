#include "precompiled_engine.h"

#ifndef _ITF_BOOT_LOADER_H_
#include "engine/boot/BootLoader.h"
#endif //_ITF_BOOT_LOADER_H_

namespace ITF
{

BootLoader::UpdateProc  BootLoader::m_proc = NULL;
u32                     BootLoader::m_jmpId = 0;

void BootLoader::init( UpdateProc proc )
{
    ITF_ASSERT( m_proc == NULL );
    m_proc = proc;
    m_jmpId = 0;
}

void BootLoader::update()
{
    ITF_ASSERT( m_proc != NULL );
    if ( m_proc == NULL )
        return;

    bbool ret = btrue;

    while ( ret )
    {
        ret = m_proc( m_jmpId );
        if ( ret )
            ++m_jmpId;
    }
}

}
