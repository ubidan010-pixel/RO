#ifndef __LIFO_H__
#define __LIFO_H__

#include <atomic>
#include "adapters/Adapter_WWISE/AudioSDK/error.h"


namespace	AudioSDK
{
	template<class _TYPE, size_t _SIZE> class Lifo
	{


	public:
			Lifo() :
			m_outIndex(0),
			m_inIndex(0)
			{
			}

			Lifo(const _TYPE &_initialState) :
			m_outIndex(0),
			m_inIndex(0)
			{
				write(_initialState);
			}

			void write(const _TYPE &_element)
            { 
                const i32 inIndex = m_inIndex.fetch_add(1) % _SIZE; //INCREMENT
                
                m_array[inIndex] = _element;

               // m_outIndex.set(inIndex);
                m_outIndex.exchange(inIndex); //???
            }

			const _TYPE&read()		
            {
                return m_array[m_inIndex];
               // return m_array[m_outIndex.get()];
            }


	private:
            std::atomic<unsigned>    m_outIndex;
            std::atomic<unsigned>		m_inIndex;
			_TYPE		    m_array[_SIZE];
	};
}
#endif // __LIFO_H__
