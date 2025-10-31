#include "precompiled_FileManager_ITF.h"

#ifndef ITF_FINAL

#ifndef _ITF_FILE_MANAGER_LOGGER_ITF_H_
#include "adapters/FileManager_ITF/FileManagerLogger_ITF.h"
#endif //_ITF_FILE_MANAGER_LOGGER_ITF_H_

#ifndef _ITF_TESTISCPLUGIN_H_
#include "tools/plugins/Plugin_TestIsc/Plugin_TestIsc.h"
#endif

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

namespace ITF
{

bool FileManagerLogger_ITF::m_logFiles = false;

FileManagerLogger_ITF::FileManagerLogger_ITF()
{
    Synchronize::createCriticalSection( &m_cs );
}

FileManagerLogger_ITF::~FileManagerLogger_ITF()
{
    Synchronize::destroyCriticalSection( &m_cs );
}

void FileManagerLogger_ITF::LogOpenedFile( const String& filename )
{
    if ( !m_logFiles )
        return;
    LogOpenedFile( filename, GetPlugin() );
}

void FileManagerLogger_ITF::LogTag( const String& tag )
{
    if ( !m_logFiles )
        return;

    String tagMsg( "tag:" );

    tagMsg += tag;
    LogOpenedFile( tagMsg, GetPlugin() );
}

void FileManagerLogger_ITF::Flush()
{
    Plugin_TestISC* plug = GetPlugin();

    if ( plug != NULL )
        Flush( plug );
}

Plugin_TestISC* FileManagerLogger_ITF::GetPlugin() const
{
    Plugin_TestISC* plug = NULL;
#ifdef ITF_SUPPORT_EDITOR
    if ( PLUGINGATEWAY != NULL )
    {
        plug = reinterpret_cast< Plugin_TestISC* >( PLUGINGATEWAY->getPluginByName( Plugin_TestISC::getPluginName() ) );
        if ( plug != NULL && plug->_peer == NULL )
            plug = NULL;
    }
#endif
    return plug;
}

void FileManagerLogger_ITF::LogOpenedFile( const String& filename, Plugin_TestISC* plug )
{
    csAutoLock      lock( m_cs );
    StringConverter converter( filename );
    const char*     threadId = ThreadSettings::getCurrentThreadName();

    if ( threadId == NULL )
        threadId = "UnknownThread";
    if ( plug != NULL )
    {
        Flush( plug );
        plug->OnFileOpened( converter.getChar(), threadId, CURRENTFRAME );
        return;
    }

    FileSlot slot;

    memset( slot.filepath, 0, _MAX_PATH );
    memcpy( slot.filepath, converter.getChar(), converter.getCharLength() );
    slot.threadId = threadId;
    slot.frameId = CURRENTFRAME;
    m_files.push_back( slot );
}

void FileManagerLogger_ITF::Flush( Plugin_TestISC* plug )
{
    csAutoLock lock( m_cs );

    ITF_ASSERT_CRASH( plug != NULL, "Invalid plugin pointer" );
    if ( m_files.size() )
    {
        for ( u32 i = 0; i < m_files.size(); ++i )
            plug->OnFileOpened( m_files[i].filepath, m_files[i].threadId, m_files[i].frameId );
        m_files.clear();
    }
}

}

#endif //ITF_FINAL
