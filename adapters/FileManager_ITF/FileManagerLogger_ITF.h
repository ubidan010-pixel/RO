#ifndef ITF_FINAL
#ifndef _ITF_FILE_MANAGER_LOGGER_ITF_H_
#define _ITF_FILE_MANAGER_LOGGER_ITF_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

namespace ITF
{

class Plugin_TestISC;
class String;

class FileManagerLogger_ITF
{
private:
    struct FileSlot
    {
        char        filepath[_MAX_PATH];
        const char* threadId;
        u32         frameId;
    };

public:
    FileManagerLogger_ITF();
    ~FileManagerLogger_ITF();

    static void enableFileLogging( bool value ) { m_logFiles = value; }
    static bool isFileLoggingEnable() { return m_logFiles; }

    void LogOpenedFile( const String& filename );
    void LogTag( const String& tag );
    void Flush();

private:
    Plugin_TestISC* GetPlugin() const;
    void            LogOpenedFile( const String& filename, Plugin_TestISC* plug );
    void            Flush( Plugin_TestISC* plug );

    static bool                 m_logFiles;
    ITF_VECTOR< FileSlot >      m_files;
    ITF_THREAD_CRITICAL_SECTION m_cs;
};

}

#endif //_ITF_FILE_MANAGER_LOGGER_ITF_H_
#endif //ITF_FINAL
