#pragma once

#include "Atomic.h"


namespace ITF
{

    namespace sound
    {






        // THREAD SAFE LOCKLESS simple list of commands: ONE thread adds commands, ONE OTHER thread reads commands
        // THIS IS NOT THREAD SAFE IF MULTIPLE THREADS READ OR IF MULTIPLE THREADS WRITE 
        // Element can be int, pointer, or a struct 




        template <typename T>
        class CircularCommandList
        {
        public:


            CircularCommandList(i32 _arraySize)
                : m_elementArray(NULL)
                , m_arraySize(_arraySize)
                , m_readIndex(0)
                , m_writeIndex(0)
            {
                m_elementArray = (T*)ITF::Memory::mallocCategory(_arraySize * sizeof(T), ITF::MemoryId::mId_AudioEngine);///Memory::malloc(_arraySize * sizeof(T), Memory::engine);

                RAKI_ASSERT(m_elementArray);
            }


            ~CircularCommandList()
            {
                Memory::free(m_elementArray);
            };


            void addElement(T o_Element)
            {
                RAKI_ASSERT((m_writeIndex + 1) % m_arraySize != m_readIndex);

                m_elementArray[m_writeIndex] = o_Element;

                ITF::wwise::Atomic::setWithMemBarrier(m_writeIndex, (m_writeIndex + 1) % m_arraySize);
            }


            T* getElementIfAvailable()
            {
                u32 i_WriteIndex = ITF::wwise::Atomic::getWithMemBarrier(m_writeIndex);

                if (i_WriteIndex != m_readIndex)
                {
                    i32 i_ReadIndex = m_readIndex;

                    m_readIndex = (m_readIndex + 1) % m_arraySize;

                    return &m_elementArray[i_ReadIndex];
                }

                return NULL;
            }


        private:

            u32 m_arraySize;

            u32 m_readIndex;

            volatile u32 m_writeIndex;

            T* m_elementArray;
        };





    } // namespace sound

} // namespace ITF
