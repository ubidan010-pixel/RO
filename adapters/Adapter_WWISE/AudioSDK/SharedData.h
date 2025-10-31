#ifndef __SHAREDDATA_H__
#define __SHAREDDATA_H__

#include "core/system/Synchronize.h"

namespace	AudioSDK
{
	template<class _TYPE> class SharedData
	{


	public:
			SharedData() : m_isFull(false)						
            { 
                m_mutex.init(); 
            }

            explicit SharedData(const _TYPE &_value)	
            { 
                m_mutex.init(); 
                m_mutex.lock(); 
                m_data = _value; 
                m_isFull =true;
                m_mutex.unlock(); 
            }
			
            ~SharedData()
            { 
                m_mutex.destroy(); 
            }

			inline void	write(const _TYPE &_value)	
            { 
                m_mutex.lock(); 
                m_data = _value; 
                m_isFull=true;
                m_mutex.unlock(); 
            }

			inline bool read(_TYPE &_value) 
			{
				m_mutex.lock();
                const bool ret = m_isFull;
                _value = m_data;
                m_isFull=false;
				m_mutex.unlock();
                return ret;
			}

	private:
			mutable ITF::Mutex  m_mutex;
			 _TYPE			    m_data;
            bool                m_isFull;
	};
}
#endif // __SHAREDDATA_H__
