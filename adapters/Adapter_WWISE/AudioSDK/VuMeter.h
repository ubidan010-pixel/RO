#ifndef __VUMETER_H__
#define __VUMETER_H__

#include "adapters/Adapter_WWISE/AudioSDK/types.h"
#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"

namespace	AudioSDK
{
	class	VuMeter : public AudioStructs::WaveDataListener
	{

    public: // CaptureListener surrounding
            virtual ~VuMeter() {}
            virtual void onStart(const AudioStructs::WaveFormatExtensible&_format) ;
            virtual void onStop() ;
            virtual void onBuffer (const void *_data, u32 _bytesRecorded) ;
        
	public:
			enum InputMode
			{
				MODE_MONO	= 0,
				MODE_STEREO,
				MODE_2_1,
				MODE_4_0,
				MODE_5_1,
				MODE_7_1,
				MODE_COUNT,
                MODE_OFF // meter is out of order
			};
			enum
			{
				CHANNEL_MAX = 8
			};

			struct MeterBar
			{
				f32		m_peak;
				f32		m_rms;
				f32		m_peakHold;
				f32		m_peakHoldTime;
				bool	m_clipping;
				i32		m_tailPeakCount;
				f32		m_peakPeakHold;
			};


	public:
			VuMeter();

			//
			// ===========================================================================================================
			//    configure
			// ===========================================================================================================
			//
			inline void setSamplingRate(f32 _val)			{ m_samplingRate = _val; }
			inline void setNumPeakSamplesToClip(i32 _val)	{ m_numPeakSamplesToClip = _val; }
			inline void setdBRange(i32 _val)				{ m_dBRange = _val; }
			inline void setBarCount(i32 _val)				{ m_barCount = _val; }
			inline void setMode(VuMeter::InputMode _val)	{ m_mode = _val; }
            inline void setDecayRate(f32 _val)				{ m_decayRate = _val; }
			inline void setRmsBlend(f32 _val)				{ m_rmsBlend = _val; }
			inline void setPeakHoldDuration(f32 _val)		{ m_peakHoldDuration = _val; }

			// input
			void		doProcess_float32(f32 *__restrict _samples, i32 _frames, i32 _channels);
			void		doProcess_pcm16(i16 *__restrict _samples, i32 _frames, i32 _channels);

			// output
			void		compilData();
			inline const VuMeter::MeterBar &getBar(i32 _bar)const{return m_bar[_bar];}

			// operation
			void	reset();


	private:
			struct MesureMsg
			{
				i32		frameCount;
                f32		peak_dB[CHANNEL_MAX];
                f32		rms_dB[CHANNEL_MAX];
                f32		peak[CHANNEL_MAX];
                f32		rms[CHANNEL_MAX];
				bool	clipping[CHANNEL_MAX];
				i32		headPeakCount[CHANNEL_MAX];
				i32		tailPeakCount[CHANNEL_MAX];
			};

			template<typename T, i32 SIZE> class MsgQueue
			{


		public:
				MsgQueue()
				:
				m_start(0),
				m_end(0)
				{
					memset(m_buffer, 0, sizeof(m_buffer));
				}
				inline bool put(const T &msg)
				{
					const i32	len = (m_end + SIZE - m_start) % SIZE;

					// Never completely fill the queue, because then the state is ambiguous
					if(len >= SIZE - 1) return false;

					m_buffer[m_end] = msg;
					m_end = (m_end + 1) % SIZE;

					return true;
				}

				inline bool							get(T &msg)		{ i32	len = (m_end + SIZE - m_start) % SIZE; if(len == 0) return false; msg = m_buffer[m_start]; m_start = (m_start + 1) % SIZE; return true; }
				inline void							reset()			{ m_start = 0; m_end = 0; memset(m_buffer, 0, sizeof(m_buffer)); }


		private:
				i32 m_start;
				i32 m_end;
				T	m_buffer[SIZE];
			};


	private:
        i32                             m_channel;
			f32							m_samplingRate;
			i32							m_barCount;
			i32							m_numPeakSamplesToClip;
			i32							m_dBRange;
			InputMode					m_mode;
			MsgQueue<MesureMsg, 1024>	m_queue;
			f32							m_dT;
			f32							m_decayRate;
            f32                         m_rmsBlend;
			f32							m_peakHoldDuration;
			MeterBar					m_bar[CHANNEL_MAX];
			void						post(const MesureMsg &_msg)		{ m_queue.put(_msg); }
			template<typename T> void doProcess(T *__restrict _samples, i32 _frames, i32 _channels);
			template<typename T> inline f32 convertSample(T _sample) const;
	};
}
#endif // __VUMETER_H__
