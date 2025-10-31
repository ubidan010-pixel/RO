#ifndef _ITF_FILEREMOTE_H_
#define _ITF_FILEREMOTE_H_


#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

namespace ITF
{
    class FileRemote : public File
    {
    public:

        FileRemote(u64 _length):m_length(_length),m_LastTimeWriteAccess(0) {ITF_ASSERT(m_length<I32_MAX);m_pData = new u8[(u32)_length];m_currentPosition =0;};
        virtual ~FileRemote() {SF_DEL_ARRAY(m_pData);};

        virtual bbool open(const String& path, u32 attribute,u32 _flagsAttribute = 0);

        virtual bbool read(void *pBuffer, u32 size,u32* sizeRead);

        virtual bbool read(void *pBuffer, u32 size);

        virtual bbool write(const void *pBuffer, u32 size,u32* sizeWritten) {ITF_ASSERT(0);return btrue;}

        virtual bbool write(const void *pBuffer, u32 size) {ITF_ASSERT(0);return btrue;};

        virtual void close();

        virtual u64 getLength() {return m_length;}

        virtual u64 seek(i64 offset, u16 from);

        virtual  FILE_HANDLE getHandle() { ITF_ASSERT(0); return static_cast<FILE_HANDLE>(0);}

        void writeStream(u32 _offset, const u8* _data,u32 _dataSize);
        u64 getLastTimeWriteAccess() {return m_LastTimeWriteAccess;}
        void setLastTimeWriteAccess(u64 _lastTimeWriteAccess) {m_LastTimeWriteAccess = _lastTimeWriteAccess;};

    protected:

        u8* m_pData;
        u64 m_length; 
        u64 m_currentPosition;
        u64 m_LastTimeWriteAccess;
    };

} // namespace ITF






#endif // _ITF_FILEREMOTE_H_
