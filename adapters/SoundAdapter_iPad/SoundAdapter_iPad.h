#ifndef _ITF_SOUNDADAPTER_H_IPAD_
#define _ITF_SOUNDADAPTER_H_IPAD_

#ifdef ITF_IPAD

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <OpenAl/oalStaticBufferExtension.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

#define MAX_SOURCES 32

namespace ITF
{
	class SoundAdapter_iPad : public SoundAdapter
	{
		public:
			bbool						init();
			bbool						isRunning() const;
			void						shutdown();
			bbool						loadSound(Sound * _sound, const char * _path);
			void						unloadSound(Sound * _sound);
			SourceHandle				createSource(const SourceInfo * _params);
			bbool						cancelStream(SoundStreamHandle _handle);
			void						play(const SourceHandle _source);
			void						resume(const SourceHandle _source);
			void						getPlayingVoices(SafeArray<SourceHandle> &_playingVoices);
			void						stop(const SourceHandle _source);
			void						pushBuffer(const SourceHandle _source, const SoundBuffer * _buffer);
			void						setLooping(const SourceHandle _source);
			void						setVolume(const SourceHandle _source, const f32 _volume);
			f32							getVolume(const SourceHandle _source) const;
			void						setOutputVolumes(const SourceHandle _source, f32 * _volumes, u32 _sourceChannels, u32 _destChannels);
			void						setPitch(const SourceHandle _source, f32 _pitch);
            void                        setFilter(const SourceHandle _source,FilterType _filterType, f32 _cutoff, f32 _oneOverQ);
            void                        setBusVolume(StringID _bus, Volume _volume, f32 _time);
            void                        setBusFilter(StringID _bus,FilterType _filterType, f32 _cutoff, f32 _oneOverQ);
            void                        enableBusReverb(StringID _bus,bbool _enable);
            void                        setBusReverbPreset(StringID _bus,ReverbPreset _preset);
            bbool						isPlaying(const SourceHandle _source);
			bbool						isStreamingSource(const SourceHandle _source);
			void						setMasterVolume(f32 _volume);


			void						toggleDebug();
			bbool						isDebug();
			void						destroySource(const SourceHandle _source);
			void						emptySource(const SourceHandle _source);
			void						update(f32 _dt);
			SourceHandle				createStreamingSourceHandle(SoundStreamerDataSource *_dataSource);
			SoundStreamerDataSource		*openStreamFile(const String &_filename);
			SoundStreamHandle			createStream(SoundStreamerDataSource* _dataSource, SourceHandle _soundSourceHandle);
			void						addDataSourceToStream(SoundStreamHandle _stream, SoundStreamerDataSource* _dataSource, bbool _isLooping, SoundStreamExhaustedCallback _reachEndOfStreamCallback);
			void						rewindDataSource(SoundStreamHandle _stream, SoundStreamerDataSource* _dataSource);
		
		private:
			struct SoundSource
			{
				ALuint			source;
				ALboolean		free;
				SourceInfo*	params;
			};
		
			SoundSource					m_sources[MAX_SOURCES];
			ALCcontext*					m_alContext;
			ALCdevice*					m_alDevice;
			bbool						m_isRunning;
	};
} // namespace ITF

#endif //ITF_IPAD

#endif //_ITF_SOUNDADAPTER_H_IPAD_
