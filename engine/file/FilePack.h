#ifndef _ITF_FILEPACK_H_
#define _ITF_FILEPACK_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

namespace ITF
{
    class BundleFile;

    class Filepack : public File
    {
    public:
        Filepack(BundleFile* _bundleparent,u64 _globalPositionOnFile, u64 _Length,u64 _compressedSize,const String& filename);
        virtual ~Filepack();

        bbool read(void *pBuffer, u32 size);

        bbool readFromCompressed(u8* _pBufferDst,u32 _bytesToRead,u32 _lenghtCompressed);

        //_from need to be implemented
        u64 seek(i64 _seekPos,u16 _from);

        u64 getLength() {return m_Length;}

        //TOBE implemented
        void close() {ITF_ASSERT(0);}

        const bbool isCompressed() const {return m_compressedSize != 0;}

        void waitTobeReady();
        void setBuffer(const u8* _pBuffer);

        const String& getFilename() const { return m_szFilename; }
        ITF_INLINE u64 getGlobalPositionOnfile() const {return m_globalPositionOnFile;}
        ITF_INLINE BundleFile* getBundleParent() const {return  m_BundleParent;}

    private:

        bbool read(void* _buffer,u32 _bytesToRead,u32* _readBytes);

        u64             m_compressedSize;
        u64             m_Length;
        u64             m_CurrentPosition;//on its own range
        u64             m_globalPositionOnFile;
        const u8*       m_pBuffer;             
        BundleFile*     m_BundleParent;
        String          m_szFilename;
    };



} // namespace ITF

#endif // _ITF_FILEPACK_H_
