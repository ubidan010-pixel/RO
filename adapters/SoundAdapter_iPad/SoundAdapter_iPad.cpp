#include "precompiled_SoundAdapter_iPad.h"

#ifndef _ITF_SOUNDADAPTER_H_IPAD_
#include "adapters/SoundAdapter_iPad/SoundAdapter_iPad.h"
#endif //_ITF_SOUNDADAPTER_H_IPAD_

#ifndef _ITF_SOUNDENGINE_IPAD_H_
#include "adapters/SoundAdapter_iPad/SoundEngine_iPad.h"
#endif //_ITF_SOUNDENGINE_IPAD_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_H_
#include "engine/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

namespace ITF
{
	bbool ReadChunkData(File * _f, void * buffer, DWORD buffersize, DWORD bufferoffset)
    {
        ITF_ASSERT(_f);
        _f->seek(bufferoffset,begin);
        u32 dwRead;
        return (_f->read(buffer, buffersize, &dwRead) ? btrue : bfalse);
    }
	
	bbool FindChunk(File * _f, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
    {
        
        ITF_ASSERT(_f);
        _f->seek(0,begin);
		
        DWORD dwChunkType;
        DWORD dwChunkDataSize;
        DWORD dwRIFFDataSize = 0;
        DWORD dwFileType;
        DWORD bytesRead = 0;
        DWORD dwOffset = 0;
        bbool result = btrue;
        while (result)
        {
            u32 dwRead;
            result = _f->read(&dwChunkType, sizeof(DWORD), &dwRead);
            if (!result)
                return bfalse;
            result = _f->read(&dwChunkDataSize, sizeof(DWORD), &dwRead);
			if (!result)
                return bfalse;
			
            switch (dwChunkType)
            {
				case fourccRIFF:
					dwRIFFDataSize = dwChunkDataSize;
					dwChunkDataSize = 4;
					result = _f->read(&dwFileType, sizeof(DWORD), &dwRead);
					if (!result)
						return bfalse;
					break;
					
				default:
					_f->seek(dwChunkDataSize,current);
            }
			
            dwOffset += sizeof(DWORD) * 2;
			
            if (dwChunkType == fourcc)
            {
                dwChunkSize = dwChunkDataSize;
                dwChunkDataPosition = dwOffset;
                return btrue;
            }
			
            dwOffset += dwChunkDataSize;
			
            if (bytesRead >= dwRIFFDataSize) return bfalse;

        }

        return btrue;

    }
	
	struct SoundResourceData
	{
		void	*data;
		ALuint	size;
		ALenum	format;
		ALsizei sampleRate;
		ALuint	bufferId;
	};
	
	struct WAVEFORMATEX
	{
		WORD        wFormatTag;         /* format type */
		WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
		DWORD       nSamplesPerSec;     /* sample rate */
		DWORD       nAvgBytesPerSec;    /* for buffer estimation */
		WORD        nBlockAlign;        /* block size of data */
		WORD        wBitsPerSample;     /* number of bits per sample of mono data */
		WORD        cbSize;             /* the count in bytes of the size of */
		/* extra information (after cbSize) */
	};
	
	bbool SoundAdapter_iPad::init()
	{
		m_isRunning = bfalse;
		SoundAdapter::init();
		
		OSStatus status = AudioSessionInitialize(NULL, NULL, NULL, NULL);
		if (status)
		{
			LOG("<< SoundAdapter >> Error on init");
			return bfalse;
		}
		else
		{
			UInt32 category = kAudioSessionCategory_AmbientSound;
			status = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
			if (status)
			{
				LOG("<< SoundAdapter >> Error setting audio session category");
				return bfalse;
			}
			else
			{
				status = AudioSessionSetActive(true);
				if (status)
				{
					LOG("<< SoundAdapter >> Error setting audio session active");
					return bfalse;
				}
			}
		}
		
		m_alDevice = alcOpenDevice(NULL);
		if (m_alDevice != NULL)
		{
			m_alContext = alcCreateContext(m_alDevice, 0);
			if (m_alContext)
			{
				alcMakeContextCurrent(m_alContext);
			}
			else
			{
				LOG("<< SoundAdapter >> Error creating OpenAL context");
				return bfalse;
			}
		}
		else
		{
			LOG("<< SoundAdapter >> Error creating OpenAL device");
			return bfalse;
		}

		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
		
		for(ALuint sourceIndex = 0; sourceIndex < MAX_SOURCES; ++sourceIndex)
		{
			alGenSources(1, &m_sources[sourceIndex].source);
			m_sources[sourceIndex].free = true;
		}
		
		m_isRunning = btrue;
		return m_isRunning;
	}

	bbool SoundAdapter_iPad::isRunning() const
	{
		return m_isRunning;
	}

	void SoundAdapter_iPad::shutdown()
	{
		m_isRunning = bfalse;
		alcDestroyContext(m_alContext);
		alcCloseDevice(m_alDevice);
	}

	bbool SoundAdapter_iPad::loadSound(Sound * _sound, const char * _path)
	{
        File* fileSound = FILEMANAGER->openFile(_path,ITF_FILE_ATTR_READ);;
        if (!fileSound)
        {
            LOG("<< SoundAdapter >> Unable to open file");
            return bfalse;
        }

		        //Locate the 'RIFF' chunk in the audio file and check the file type.

        DWORD dwChunkSize;
        DWORD dwChunkPosition;

        //check the file type, should be fourccWAVE
        FindChunk(fileSound,fourccRIFF,dwChunkSize, dwChunkPosition );

        DWORD filetype;
        ReadChunkData(fileSound,&filetype,sizeof(DWORD),dwChunkPosition);

        if (filetype != fourccWAVE)
            return bfalse;

		SoundResourceData*	resourceData = newAlloc(mId_AudioEngine,SoundResourceData());
		WAVEFORMATEX		wavFormat;
        //Locate the 'fmt ' chunk and copy its contents into a WAVEFORMATEXTENSIBLE structure.
        FindChunk(fileSound,fourccFMT, dwChunkSize, dwChunkPosition );
        ReadChunkData(fileSound, &wavFormat, dwChunkSize, dwChunkPosition);

        FindChunk(fileSound,fourccDATA,dwChunkSize, dwChunkPosition );
        resourceData->size = dwChunkSize;
		resourceData->data = malloc(dwChunkSize);
		if(wavFormat.nChannels > 1)
		{
			resourceData->format = wavFormat.wBitsPerSample == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
		}
		else
		{
			resourceData->format = wavFormat.wBitsPerSample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
		}
		resourceData->sampleRate = wavFormat.nSamplesPerSec;

		
		ReadChunkData(fileSound, resourceData->data, dwChunkSize, dwChunkPosition);
		
		alGenBuffers(1, &resourceData->bufferId);
		alBufferData(resourceData->bufferId, resourceData->format, resourceData->data, resourceData->size, resourceData->sampleRate);
		
        _sound->m_adapterImplementationData = resourceData;
        _sound->setPhysicalLoaded(btrue);
#ifndef ITF_FINAL
        _sound->setMemoryUsage(dwChunkSize);
#endif
        FILEMANAGER->closeFile(fileSound);
		
        return btrue;
	}

	void SoundAdapter_iPad::unloadSound(Sound * _sound)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}

	SourceHandle SoundAdapter_iPad::createSource(const SourceInfo * _params)
	{
		for(ALuint sourceIndex = 0; sourceIndex < MAX_SOURCES; ++sourceIndex)
		{
			if(m_sources[sourceIndex].free)
			{
				SoundResourceData* data = reinterpret_cast<SoundResourceData*>(_params->m_sound->m_adapterImplementationData);
				ALuint source = m_sources[sourceIndex].source;
				
				alSourcei(source, AL_BUFFER, data->bufferId);
				alSourcei(source, AL_LOOPING, _params->m_loop);
				alSourcef(source, AL_GAIN, _params->m_volume);
				alSourcef(source, AL_PITCH, _params->m_pitch);
				
				m_sources[sourceIndex].free = false;
				return sourceIndex;
			}
		}
		return MAX_SOURCES;
	}

	bbool SoundAdapter_iPad::cancelStream(SoundStreamHandle _handle)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
		return false;
	}

	void SoundAdapter_iPad::play(const SourceHandle _source)
	{
		if(_source < MAX_SOURCES)
		{
			alSourcePlay(m_sources[_source].source);
		}
	}

	void SoundAdapter_iPad::stop(const SourceHandle _source)
	{
		if(_source < MAX_SOURCES)
		{
			alSourceStop(m_sources[_source].source);
		}
	}
	
	void SoundAdapter_iPad::resume(const SourceHandle _source)
	{
		if(_source < MAX_SOURCES)
		{
			alSourcePlay(m_sources[_source].source);
		}		
	}
	
	void SoundAdapter_iPad::getPlayingVoices(SafeArray<SourceHandle> &_playingVoices)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}
	
	void SoundAdapter_iPad::pushBuffer(const SourceHandle _source, const SoundBuffer * _buffer)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}

	void SoundAdapter_iPad::setLooping(const SourceHandle _source)
	{
		if(_source < MAX_SOURCES)
		{
			alSourcei(m_sources[_source].source, AL_LOOPING, AL_TRUE);
		}
	}

	void SoundAdapter_iPad::setVolume(const SourceHandle _source, const f32 _volume)
	{
		if(_source < MAX_SOURCES)
		{
			alSourcef(m_sources[_source].source, AL_GAIN, _volume);
		}
	}

	f32 SoundAdapter_iPad::getVolume(const SourceHandle _source) const
	{
		if(_source < MAX_SOURCES)
		{
			return m_sources[_source].params->m_volume;
		}
		return 0.f;
	}

    void SoundAdapter_iPad::setOutputVolumes(const SourceHandle _source, f32 * _volumes, u32 _sourceChannels, u32 _destChannels)
	{
	}

	void SoundAdapter_iPad::setPitch(const SourceHandle _source, f32 _pitch)
	{
		if(_source < MAX_SOURCES)
		{
			alSourcef(m_sources[_source].source, AL_PITCH, _pitch);
		}
	}

	bbool SoundAdapter_iPad::isPlaying(const SourceHandle _source)
	{
		if(_source < MAX_SOURCES)
		{
			ALenum state;
			alGetSourcei(m_sources[_source].source, AL_SOURCE_STATE, &state);
			return (state == AL_PLAYING);
		}
		return false;
	}
	
	bbool SoundAdapter_iPad::isStreamingSource(const SourceHandle _source)
	{
		return bfalse;
	}

	void SoundAdapter_iPad::setMasterVolume(f32 _volume)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}

	void SoundAdapter_iPad::toggleDebug()
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}

	bbool SoundAdapter_iPad::isDebug()
	{
#ifdef _DEBUG
		return true;
#else
		return false;
#endif
	}

	void SoundAdapter_iPad::destroySource(const SourceHandle _source)
	{
		if(_source < MAX_SOURCES)
		{
			m_sources[_source].free = true;
		}
	}

	void SoundAdapter_iPad::emptySource(const SourceHandle _source)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}

	void SoundAdapter_iPad::update(f32 _dt)
	{
		ALfloat orientation[6] =
		{
			0.f, 0.f, -1.f,				// direction vector
			0.f, 1.f, 0.f				// up vector
		};
		alListenerfv(AL_ORIENTATION, orientation);
	}

	SourceHandle SoundAdapter_iPad::createStreamingSourceHandle(SoundStreamerDataSource *_dataSource)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
		return NULL;
	}

	SoundStreamerDataSource* SoundAdapter_iPad::openStreamFile(const String &_filename)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
		return NULL;
	}

	void addDataSourceToStream(SoundStreamHandle _stream, SoundStreamerDataSource* _dataSource, bbool _isLooping, SoundStreamExhaustedCallback _reachEndOfStreamCallback)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}
	
	SoundStreamHandle SoundAdapter_iPad::createStream(SoundStreamerDataSource* _dataSource, SourceHandle _soundSourceHandle)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
		return NULL;
	}
	
	void SoundAdapter_iPad::addDataSourceToStream(SoundStreamHandle _stream, SoundStreamerDataSource* _dataSource, bbool _isLooping, SoundStreamExhaustedCallback _reachEndOfStreamCallback)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}
	
	void  SoundAdapter_iPad::rewindDataSource(SoundStreamHandle _stream, SoundStreamerDataSource* _dataSource)
	{
		ITF_ASSERT_MSG(0, "Not implemented");
	}

    void  SoundAdapter_iPad::setFilter(const SourceHandle _source,FilterType _filterType, f32 _cutoff, f32 _oneOverQ)
    {
        ITF_ASSERT_MSG(0, "Not implemented");
    }
    void SoundAdapter_iPad::setBusVolume(StringID _bus, Volume _volume, f32 _time)
    {
        ITF_ASSERT_MSG(0, "Not implemented");
    }
    void SoundAdapter_iPad::setBusFilter(StringID _bus,FilterType _filterType, f32 _cutoff, f32 _oneOverQ)
    {
        ITF_ASSERT_MSG(0, "Not implemented");
    }
    void SoundAdapter_iPad::enableBusReverb(StringID _bus,bbool _enable)
    {
        ITF_ASSERT_MSG(0, "Not implemented");
    }
    void SoundAdapter_iPad::setBusReverbPreset(StringID _bus,ReverbPreset _preset)
    {
        ITF_ASSERT_MSG(0, "Not implemented");
    }
} // namespace ITF
