#include "precompiled_Adapter_WWISE.h"
#ifdef tododisap
#include "adapters/Adapter_WWISE/AudioSDK/WavFile.h"
#include "adapters/Adapter_WWISE/AudioSDK/tools.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"



#ifdef ITF_FINAL
#define MY_READ(_a, _b) readFile(_a, _b)
#else
#define MY_READ(_a, _b) readFile(_a, _b, __FILE__, __LINE__)
#endif

//$4
// ***********************************************************************************************************************
// ***********************************************************************************************************************
//

namespace	AudioSDK
{


#ifdef ITF_FINAL
	inline bool WavReader::readFile(void *_buffer, u32 _size)
#else
	inline bool WavReader::readFile(void *_buffer, u32 _size, const char *_source, int _line)
#endif
	{
		AkUInt32		sizeDone;
		const AKRESULT	res = CAkFileHelpers::ReadBlocking(m_fileHandle, _buffer, m_fileSeek, _size, sizeDone);
		m_fileSeek += _size;
		if(AK_Success == res) return true;

		AUDIO_WARNING("%s(%d) : file reading failure", _source, _line);
		return false;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool WavReader::open(const char *_path)
	{
		// just for converting char -> AkOSChar
		AkOSChar	tmp[1024];
		AudioSDK::safeStringCopy(tmp, _path);

		m_fileSeek = 0;
		if(CAkFileHelpers::OpenBlocking(tmp, m_fileHandle) == AK_Success)
		{
			ChunkHeader chunkHeader;

			// RIFF
			if(!MY_READ(&chunkHeader, sizeof(chunkHeader)) || (chunkHeader.tag != c_tagRIFF))
			{
				AUDIO_WARNING("File %s is not a RIFF", _path);
				close();
				return false;
			}
            
			// WAVEfmt
			if(!MY_READ(&chunkHeader, sizeof(chunkHeader)) || (chunkHeader.tag != c_tagWAVE))
			{
				AUDIO_WARNING("File %s is not a WAVE", _path);
				close();
				return false;
			}
            
            
			
            if(chunkHeader.size != c_tagfmt)
			{
				AUDIO_WARNING("File %s has no \"fmt\" chunk", _path);
				close();
				return false;
			}

			u32 fmtSize = 0;
			if(!MY_READ(&fmtSize, sizeof(fmtSize)))
			{
				close();
				return false;
			}
            m_riffSeek = getSeek()-sizeof(chunkHeader);

			// WAVEFORMATEX
			
			if(fmtSize >= sizeof(waveFormatEx))
			{
				if(!MY_READ(&waveFormatEx.Format, sizeof(waveFormatEx)))
				{
					close();
					return false;
				}
			}
			else if(fmtSize >= sizeof(waveFormatEx.Format))
			{
				if(!MY_READ(&waveFormatEx.Format, sizeof(waveFormatEx.Format)))
				{
					close();
					return false;
				}
			}
			else if(fmtSize == sizeof(AudioSDK::AudioStructs::PCMWaveFormat))
			{
				if(!MY_READ(&waveFormatEx.Format, sizeof(AudioSDK::AudioStructs::PCMWaveFormat)))
				{
					close();
					return false;
				}
			}
			else if(fmtSize == sizeof(AudioSDK::AudioStructs::WaveFormat))
			{
				if(!MY_READ(&waveFormatEx.Format, sizeof(AudioSDK::AudioStructs::WaveFormat)))
				{
					close();
					return false;
				}
			}
			else
			{
				AUDIO_WARNING("Wav format unsupported (%s)", _path);
				close();
				return false;
			}

			return true;
		}

		return false;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WavReader::close()
	{
        if(m_fileHandle)
        {
            CAkFileHelpers::CloseFile(m_fileHandle);
            m_fileHandle = NULL;
        }
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	u32 WavReader::gotoChunk(u32 _chunkID)
	{
        rewind();

        ChunkHeader chunkHeader;
        for(;;)
        {
            if(m_fileSeek & 1)
            {
                if(!MY_READ(&chunkHeader, 1)) return 0;
                continue;
            }

            if(!MY_READ(&chunkHeader, sizeof(chunkHeader))) return 0;

            if(chunkHeader.tag == _chunkID) 
                return chunkHeader.size;
            else
                m_fileSeek += chunkHeader.size;
            
        }

		
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool WavReader::read(void *_buffer, u32 _bufferSize)
	{
        if(m_fileHandle)
        {
            return MY_READ(_buffer, _bufferSize);
        }

        AUDIO_WARNING("file is closed");
		return false;
	}




    u32 WavWriter::write( const void*_buffer, u32 _bufferSize )
    {
        const AKRESULT	res = CAkFileHelpers::WriteBlocking(m_fileHandle, _buffer, _bufferSize);
        if(res==AK_Success)
            return _bufferSize;
        
        AUDIO_WARNING("Write file failure");
        return 0;
    }

    bool WavWriter::open( const char *_path )
    {
        // just for converting char -> AkOSChar
        AkOSChar	tmp[1024];
        AudioSDK::safeStringCopy(tmp, _path);

        if(CAkFileHelpers::OpenFile(tmp, AK_OpenModeWriteOvrwr, false, false, m_fileHandle)==AK_Success)
        {
            ChunkHeader chunkHeader;
            chunkHeader.tag = c_tagRIFF;
            chunkHeader.size= 0;

            write(&chunkHeader, sizeof(chunkHeader));
            m_headerSize = 0;

            chunkHeader.tag = c_tagWAVE;
            m_headerSize += write(&chunkHeader.tag , sizeof(chunkHeader.tag ));
            return true;
        }
        return false;
    }


    void WavWriter::writeHeader( const AudioStructs::WaveFormatExtensible &_val)
    {
        ChunkHeader chunkHeader;
        chunkHeader.tag = c_tagfmt;
        chunkHeader.size= 0;
        if(_val.Format.formatTag == AudioStructs::FORMAT_EXTENSIBLE)
        {
            chunkHeader.size = sizeof(_val);
            m_headerSize += write(&chunkHeader, sizeof(chunkHeader));
            m_headerSize += write(&_val, sizeof(_val));
        }
        else
        {
            chunkHeader.size = sizeof(_val.Format);
            m_headerSize += write(&chunkHeader, sizeof(chunkHeader));
            m_headerSize += write(&_val.Format, sizeof(_val.Format));
        }
        
        chunkHeader.tag = c_tagdata;
        chunkHeader.size = 0;
        m_headerSize += write(&chunkHeader.tag, sizeof(chunkHeader.tag));

        AkUInt32 seek=0;
        if(AK_Success==CAkFileHelpers::GetSeek(m_fileHandle, seek))
            m_dataSizeSeek=seek;
        
        m_dataSize = 0;
        m_headerSize += write(&chunkHeader.size, sizeof(chunkHeader.size));
    }


    void WavWriter::writeData( const void *_buffer, u32 _bufferSize )
    {
        m_dataSize += write(_buffer, _bufferSize);
    }


    void WavWriter::close()
    {
        if(m_fileHandle)
        {
            // data chunk size
            CAkFileHelpers::SetSeek(m_fileHandle, m_dataSizeSeek);
            write(&m_dataSize, sizeof(m_dataSize));

            // RIFF chunk size
            m_headerSize += m_dataSize;
            CAkFileHelpers::SetSeek(m_fileHandle, sizeof(u32));
            write(&m_headerSize, sizeof(m_headerSize));

            CAkFileHelpers::CloseFile(m_fileHandle);
            m_fileHandle = NULL;
        }
    }
}

#endif
