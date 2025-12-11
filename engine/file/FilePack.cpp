#include "precompiled_engine.h"

#ifndef _ITF_FILEPACK_H_
#include "engine/file/FilePack.h"
#endif //_ITF_FILEPACK_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/BundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

#ifndef _ITF_BUNDLEPROFILER_H_
#include "engine/file/bundleProfiler.h"
#endif //_ITF_BUNDLEPROFILER_H_

#ifndef _ITF_BUNDLEMANAGER_H_
#include "engine/file/BundleManager.h"
#endif //_ITF_BUNDLEMANAGER_H_

#define ITF_NO_BUNDLE_TRACKING

namespace ITF
{
    Filepack::Filepack(BundleFile* _bundleparent,u64 _globalPositionOnFile, u64 _Length,u64 _compressedSize,const String& filename)
    {
        m_CurrentPosition       = 0;
        m_BundleParent          = _bundleparent;
        m_BundleParent->TrackReference( this );
        m_Length                = _Length;
        m_globalPositionOnFile  = _globalPositionOnFile;
        m_compressedSize        = _compressedSize;
        m_pBuffer               = NULL;
        m_szFilename            = filename;
    }

    Filepack::~Filepack()
    {
        m_BundleParent->UntrackReference( this );
        SF_DEL_ARRAY(m_pBuffer);
    }

    bbool Filepack::read(void *pBuffer, u32 size)
    {
        u32 readlocal;
        return read(pBuffer,size,&readlocal);
    }


    bbool Filepack::readFromCompressed(u8* _pBufferDst,u32 _bytesToRead,u32 _lenghtCompressed)
    {
        BUNDLEMANAGER->lockAccess();
        ITF_ASSERT(m_CurrentPosition + _bytesToRead <= _lenghtCompressed);

        u32 readlocal;

#ifndef ITF_NO_BUNDLE_TRACKING
        LOG( "[FILEPACK][READ_COMPRESSED][%s] %s %s (%d)", ThreadSettings::getCurrentThreadName(), StringToUTF8(m_BundleParent->getFilename()).get(), m_szFilename.isEmpty() ? "Unknown file" : StringToUTF8(m_szFilename).get(), _bytesToRead );
#endif
        {
            PROFILE_BUNDLE(m_globalPositionOnFile,_bytesToRead,getLength(),m_szFilename,m_BundleParent->getFilename());
            m_BundleParent->fileRead(m_globalPositionOnFile,_pBufferDst,_bytesToRead,readlocal);
        }

        ITF_ASSERT(readlocal==_bytesToRead);
        m_CurrentPosition += readlocal;
        BUNDLEMANAGER->unlockAccess();
        return btrue;
    }


    bbool Filepack::read(void* _buffer,u32 _bytesToRead,u32* _readBytes)
    {
        // if (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID)
        // {
        //     LOG("Bundle read on main thread of %s", StringToUTF8(m_szFilename).get());
        // }


        BUNDLEMANAGER->lockAccess();
        u32 readlocal;

        ITF_ASSERT(_bytesToRead <= m_Length);
        ITF_ASSERT(m_CurrentPosition + _bytesToRead <= m_Length);

        if (m_pBuffer)
        {
            ITF_Memcpy(_buffer,m_pBuffer+m_CurrentPosition,_bytesToRead);
            readlocal = _bytesToRead;
        }
        else
        {
#ifndef ITF_NO_BUNDLE_TRACKING
            LOG( "[FILEPACK][READ][%s] %s %s (%d)", ThreadSettings::getCurrentThreadName(), StringToUTF8(m_BundleParent->getFilename()).get(), m_szFilename.isEmpty() ? "Unknown file" : StringToUTF8(m_szFilename).get(), _bytesToRead );
#endif
            PROFILE_BUNDLE(m_globalPositionOnFile + m_CurrentPosition,_bytesToRead,this->getLength(),m_szFilename,m_BundleParent->getFilename());
            m_BundleParent->fileRead(m_globalPositionOnFile + m_CurrentPosition,_buffer,_bytesToRead,readlocal);
        }

        m_CurrentPosition += readlocal;
        if (_readBytes)
            *_readBytes = readlocal;
        ITF_ASSERT(m_CurrentPosition <= m_Length);
        BUNDLEMANAGER->unlockAccess();
        return btrue;
    }

    void Filepack::setBuffer(const u8* _pBuffer)
    {
        m_pBuffer = _pBuffer;
    }

    u64 Filepack::seek(i64 _seekPos,u16 _from)
    {
        switch (_from)
        {
            case 0:
                m_CurrentPosition = _seekPos;
                break;
            case 1:
                m_CurrentPosition += _seekPos;
                break;
            default:
                ITF_ASSERT(0);
                break;
        }
        ITF_ASSERT( m_CurrentPosition <= m_Length );
        return m_CurrentPosition;
    }

    void Filepack::waitTobeReady()
    {
        if (!isCompressed())
            return;
    }

}
