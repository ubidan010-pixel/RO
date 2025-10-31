#ifndef __SIMPLEPOOL_H__
#define __SIMPLEPOOL_H__


namespace AudioSDK
{
	template<class TYPE, int SIZE> class SimplePool
	{


	public:
			SimplePool() :
			m_freedCount(0)
			{
			}

			inline void create()
            { 
                AUDIO_ASSERT(m_freedCount == 0, "pool is yet created"); 
                for(int i = 0; i < SIZE; ++i)
                { 
                    m_freed[i] = m_allocated[i] = WWISE_NEW(TYPE); 
                }
                m_freedCount = SIZE; 
            }
			
            inline void destroy() 
            { 
                for(int i = 0; i < SIZE; ++i)
                { 
                    WWISE_DELETE(m_allocated[i]); 
                }
                m_freedCount = 0; 
            }
			
            inline TYPE *newElement()
            { 
                if(m_freedCount)
                { 
                    TYPE	*newElement = new (m_freed[--m_freedCount]) TYPE(); 
                    m_freed[m_freedCount]=NULL;
                    return newElement; 
                }
                return NULL; 
            }
			
            inline void deleteElement(TYPE *_oldElement)	
            { 
                m_freed[m_freedCount++] = _oldElement; 
                _oldElement->~TYPE();
            }

            inline bool isValidPointer(const TYPE *_test) const
            { 
                for(int i = 0; i < SIZE; ++i)
                { 
                    if(m_allocated[i] == _test) return true; 
                }
                return false;
            }

	private:
			TYPE	*m_allocated[SIZE];
			TYPE	*m_freed[SIZE];
			u32		m_freedCount;
	};
}
#endif // __SIMPLEPOOL_H__
