#include "precompiled_core.h"

#ifndef _ITF_FILEREMOTE_H_
#include "core/file/FileRemote.h"
#endif //_ITF_FILEREMOTE_H_


namespace ITF
{

    bbool FileRemote::open(const String& path, u32 attribute,u32 _flagsAttribute)
    {
        ITF_ASSERT(0);
        return bfalse;
    }

    bbool FileRemote::read(void *pBuffer, u32 size,u32* sizeRead)
    {
        u32 allowedToRead = size;
        if (size+m_currentPosition>m_length)
        {
            i64 difflen = (m_length - m_currentPosition);
            if (difflen>0)
                allowedToRead = (u32) difflen;
            else
            {
                if (sizeRead)
                    *sizeRead = 0;

                return bfalse;
            }
        }

        ITF_Memcpy(pBuffer,m_pData+m_currentPosition,allowedToRead);
        m_currentPosition += allowedToRead;

        if (sizeRead)
            *sizeRead = allowedToRead;

        return btrue;
    }

    u64 FileRemote::seek(i64 offset, u16 from)
    {

        if (from == 0)
        {
            m_currentPosition = offset;
        }
        else if (from == 1)
        {
            m_currentPosition += offset;
        }
        else
        {
            ITF_ASSERT(0);//not supported yet
        }

        return m_currentPosition;
    }


    bbool FileRemote::read(void *pBuffer, u32 size)
    {
        if (size+m_currentPosition>m_length)
        {
            ITF_ASSERT(0);
            return bfalse;
        }

        ITF_Memcpy(pBuffer,m_pData+m_currentPosition,size);
        m_currentPosition+=size;

        return btrue;
    }

    void FileRemote::writeStream(u32 _offset, const u8* _data,u32 _dataSize)
    {
        ITF_Memcpy(m_pData+_offset,_data,_dataSize);
    }

    void FileRemote::close()
    {
        ITF_ASSERT(0);
    };


} // namespace ITF