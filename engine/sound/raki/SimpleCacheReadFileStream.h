#ifndef _ITF_SIMPLECACHEREADFILESTREAM_H_
#define _ITF_SIMPLECACHEREADFILESTREAM_H_


#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_


#include "RakiEngine/Data/Stream/SeekableStream.h"


// this class tries to fill its cache with file data when opening the file 
// after that, it'll first read from its cache, then it'll read ordinarily 

namespace ITF
{
    class File;

    class SimpleCacheReadFileStream 
        : public raki::SeekableStream
    {
    public:

        static void allocateStaticCache( const u64 _cacheSize );
        static void freeStaticCache();

        SimpleCacheReadFileStream();
        virtual ~SimpleCacheReadFileStream();

        bool open( const String & _path );
        void close();

        const u64 streamFromCache( void * _data, const u64 _size );
        const u64 streamFromFile( void * _data, const u64 _size );

        // from SeekableStream
        virtual const u64 stream( void * _data, const u64 _size );
        virtual const u64 seek( const u64 _size, const SeekType _seekType );
        virtual const u64 getCurrentPos() const;
        virtual const u64 getSize() const;


    private:

        File * m_file;
        u64 m_size;
        u64 m_offsetInFile;

        static void * ms_cache;
        static u64 ms_cacheSize;
        static bool ms_cacheIsUsed;


    };



} // namespace ITF 


#endif // _ITF_SIMPLECACHEREADFILESTREAM_H_