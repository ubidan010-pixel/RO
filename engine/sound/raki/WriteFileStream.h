#ifndef _ITF_WRITEFILESTREAM_H_
#define _ITF_WRITEFILESTREAM_H_


#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_


#include "RakiEngine/Data/Stream/SeekableStream.h"


namespace ITF
{


    class WriteFileStream 
        : public raki::SeekableStream
    {
    public:

        WriteFileStream();
        virtual ~WriteFileStream();

        bool open( const String & _path );
        void close();

        // from SeekableStream
        virtual const u64 stream( void * _data, const u64 _size );
        virtual const u64 seek( const u64 _size, const SeekType _seekType );
        virtual const u64 getCurrentPos() const;
        virtual const u64 getSize() const;


    private:

        File * m_file;
        u64 m_size;

    };



} // namespace ITF 


#endif // _ITF_WRITEFILESTREAM_H_