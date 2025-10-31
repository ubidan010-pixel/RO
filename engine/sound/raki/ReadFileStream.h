#ifndef _ITF_READFILESTREAM_H_
#define _ITF_READFILESTREAM_H_


#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_


#include "RakiEngine/Data/Stream/SeekableStream.h"


namespace ITF
{
    class File;

    class ReadFileStream 
        : public raki::SeekableStream
    {
    public:

        ReadFileStream();
        virtual ~ReadFileStream();

        void openInStream( const String & _path );
        bool open( const String & _path );
        void close();

        // from SeekableStream
        virtual const u64 stream( void * _data, const u64 _size );
        virtual const u64 seek( const u64 _size, const SeekType _seekType );
        virtual const u64 getCurrentPos() const;
        virtual const u64 getSize() const;


    private:

        String m_pathForOpenFileAtFirstStream;
        u64 m_seekForopenFileAtFirstStream;
        File * m_file;
        u64 m_size;

    };



} // namespace ITF 


#endif // _ITF_READFILESTREAM_H_