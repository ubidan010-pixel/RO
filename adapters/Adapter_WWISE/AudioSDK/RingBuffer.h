#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include "adapters/Adapter_WWISE/AudioSDK/tools.h"
#include "core/system/Synchronize.h"

namespace	AudioSDK
{
	template<typename _TYPE, u32 _SIZE> class RingBuffer
	{


	public:
			RingBuffer() :
			m_write(0),
			m_read(0),
			m_freeSize(_SIZE)
			{
				//m_mutex.init();
			}
			~RingBuffer() 
			{
				//m_mutex.destroy();
			}

			inline bool write(const _TYPE &_arg)
            { 
                ITF::ScopeLock<ITF::Mutex> scopeLock(m_mutex); 

                if(m_freeSize > 0)
                { 
                    m_circularBuffer[m_write] = _arg; 
                    m_write = (m_write + 1) % _SIZE; 
                    --m_freeSize; 
                    return true; 
                }
                return false; 
            }


			inline bool	read(_TYPE &_arg)
            { 
                ITF::ScopeLock<ITF::Mutex> scopeLock(m_mutex); 

                if(m_freeSize < _SIZE)
                { 
                    _arg = m_circularBuffer[m_read]; 
                    m_read = (m_read + 1) % _SIZE; 
                    ++m_freeSize;
                    return true; 
                }
                return false; 
            }


	private:
			 ITF::Mutex	m_mutex;
			_TYPE	m_circularBuffer[_SIZE];
			u32		m_write;
			u32		m_read;
			i32		m_freeSize;
	};
}
#endif // __RINGBUFFER_H__
