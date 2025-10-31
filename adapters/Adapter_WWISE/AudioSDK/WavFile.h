#ifndef __WAVFILE_H__
#define __WAVFILE_H__

#include "adapters/Adapter_WWISE/AudioSDK/types.h"
#include "adapters/Adapter_WWISE/WwiseHooks/Files/win32/AkFileHelpers.h"
#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"


#define RIFF_CHUNK_TAG(a, b, c, d)	((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
#define RIFF_CHUNK_TAG_RIFF			(RIFF_CHUNK_TAG('R', 'I', 'F', 'F'))
#define RIFF_CHUNK_TAG_WAVE			(RIFF_CHUNK_TAG('W', 'A', 'V', 'E'))
#define RIFF_CHUNK_TAG_fmt			(RIFF_CHUNK_TAG('f', 'm', 't', ' '))
#define RIFF_CHUNK_TAG_data			(RIFF_CHUNK_TAG('d', 'a', 't', 'a'))

namespace	AudioSDK
{

	//////////////////////////////////////////////////////////////////////////
	//
	class	WavFile
	{


	public:
			struct ChunkHeader
			{
				u32 tag;
				u32 size;
			};

			static const u32	c_tagRIFF = RIFF_CHUNK_TAG_RIFF;
			static const u32	c_tagWAVE = RIFF_CHUNK_TAG_WAVE;
			static const u32	c_tagfmt = RIFF_CHUNK_TAG_fmt;
			static const u32	c_tagdata = RIFF_CHUNK_TAG_data;

			WavFile() :
			m_fileHandle(NULL),
			m_fileSeek(0)
			{
			}

	protected:
			AkFileHandle	m_fileHandle;
			u32				m_fileSeek;
	};

	//////////////////////////////////////////////////////////////////////////
	// ===================================================================================================================
	// ===================================================================================================================
	//
	class WavReader :
		public WavFile
	{


	public:
			WavReader() :
			WavFile(),
			m_riffSeek(0)
			{
                memset(&waveFormatEx, 0, sizeof(waveFormatEx));
			}

			bool	open(const char *_path);
			u32		gotoChunk(u32 _chunkID);	// returns the chunk size
			bool	read(void *_buffer, u32 _bufferSize);
			inline u32 getSeek() const
			{
				return m_fileSeek;
			}

			inline void setSeek(u32 _val)	{ m_fileSeek = _val; }
			inline const AudioStructs::WaveFormatExtensible &
			getWaveFormatEx()
			const
			{
				return waveFormatEx;
			}
			void	close();


	private:
			u32									m_riffSeek;
			AudioStructs::WaveFormatExtensible	waveFormatEx;
#ifdef ITF_FINAL
			inline bool							readFile(void *_buffer, u32 _size);
#else
			inline bool							readFile(void *_buffer, u32 _size, const char *_source, int _line);
#endif
			inline void rewind()	{ m_fileSeek = m_riffSeek; }
	};

	//////////////////////////////////////////////////////////////////////////
	// ===================================================================================================================
	// ===================================================================================================================
	//
	class WavWriter :
		public WavFile
	{


	public:
			WavWriter() : m_headerSize(0), m_dataSizeSeek(0), m_dataSize(0) {}

			bool	open(const char *_path);
			void	writeHeader(const AudioStructs::WaveFormatExtensible &_val);
			void	writeData(const void *_buffer, u32 _bufferSize);
			void	close();


	private:
        u32 m_headerSize;
        u32 m_dataSizeSeek;
        u32 m_dataSize;
        inline u32 write(const void*_buffer, u32 _bufferSize);
	};
}
#endif // __WAVFILE_H__
