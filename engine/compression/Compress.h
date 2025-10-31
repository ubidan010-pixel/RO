#ifndef _ITF_COMPRESS_H_
#define _ITF_COMPRESS_H_

namespace ITF
{
    class Compress
    {
public:
        static void Init();
        static void Destroy();

        static bbool compressFile(const String& _szSrcFilename,const String& _szDstFilename);

        static bbool uncompressFile(const String& _szSrcFilename,const String& _szDstFilename);

        static bbool compressFiles(const ITF_VECTOR<String>& _vFilenames,ITF_VECTOR<u64>& _vCompressedSize);
            
        //return 0 if fails,the dest len in otherwise
        static i32 compressBuffer(u8 *target, u32 target_size, const u8 *source, u32 source_size); 
       
        static i32 uncompressBuffer(u8 *target, u32 target_size, const u8 *source, u32 source_size); 
      
        static u32 computeSizeRequired(u32 _sourceSize);

    };

} // namespace ITF

#endif // _ITF_COMPRESS_H_