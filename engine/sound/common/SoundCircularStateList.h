#pragma once

//#include "engine/sound/common/SoundAtomic.h"
#include "RakiEngine/System/Atomic/Atomic.h"

namespace ITF
{
	namespace sound
	{
		// WARNING
		// this code comes from raki, this is not very safe, but this was used for RL
		// WARNING

		// THREAD SAFE LOCKLESS simple list of states: ONE thread adds states, ONE OTHER thread reads last state written
		// THIS IS NOT THREAD SAFE IF MULTIPLE THREADS READ OR IF MULTIPLE THREADS WRITE
		// Elements can be int, pointer, or a struct


// THREAD SAFE LOCKLESS simple list of states: ONE thread adds states, ONE OTHER thread reads last state written
// THIS IS NOT THREAD SAFE IF MULTIPLE THREADS READ OR IF MULTIPLE THREADS WRITE 
// Elements can be int, pointer, or a struct 





        template <typename T>
        class CircularStateList
        {
        public:


            CircularStateList(i32 _arraySize, const T& _firstElement)
                : m_elementArray(NULL)
                , m_arraySize(_arraySize)
                , m_currentIndex(0)
            {
                m_elementArray = (T*)ITF::Memory::mallocCategory(_arraySize * sizeof(T), ITF::MemoryId::mId_AudioEngine);//Memory::malloc(_arraySize * sizeof(T));

                RAKI_ASSERT(m_elementArray);

                m_elementArray[m_currentIndex] = _firstElement;
            }


            ~CircularStateList()
            {
                ITF::Memory::free(m_elementArray);
            //    Memory::free(m_elementArray);
            }

            u32 size() const
            {
                return m_arraySize;
            }

            void write(const T _element)
            {
                u32 writeIndex = (m_currentIndex + 1) % m_arraySize;

                m_elementArray[writeIndex] = _element;

                raki::Atomic::setWithMemBarrier(m_currentIndex, writeIndex);
            }


            const T& read()
            {
                return m_elementArray[m_currentIndex];
            }


        private:

            u32 m_arraySize;

            volatile u32 m_currentIndex;

            T* m_elementArray;
        };
	} // namespace sound
} // namespace ITF
