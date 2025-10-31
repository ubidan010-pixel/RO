#pragma once

#include "core/system/Atomic.h"

namespace ITF
{
	namespace sound
	{
		class Atomic32 final
		{
		public:
			explicit Atomic32(const u32 _initialValue = 0)
				: m_value{ _initialValue }
			{
			}
			~Atomic32() {}

			void set(const u32 _value) { ITF::Atomic::exchange(m_value, _value); }
			u32 get() const { return ITF::Atomic::add(reinterpret_cast<volatile i32&>(const_cast<volatile u32&>(m_value)), 0); }
			u32 incrementAndGetPrevious() { return ITF::Atomic::increment(m_value) - 1; }
			u32 decrementAndGetPrevious() { return ITF::Atomic::decrement(m_value) + 1; }

		public:
			static Atomic32* newArray(size_t _nbElements)
			{
				const size_t alignedSize = (sizeof(Atomic32) & 3) ? ((sizeof(Atomic32) + 4) & 0xfffffffc) : sizeof(Atomic32);
				void* ptr = Memory::alignedMalloc(alignedSize * _nbElements, 64);
				Atomic32* element = (Atomic32*)ptr;
				for (size_t i = 0; i < _nbElements; ++i)
				{
					new (element) Atomic32;
					++element;
				}
				return (Atomic32*)ptr;
			}

			static void deleteArray(Atomic32* _array, size_t _nbElements)
			{
				Atomic32* element = _array;
				for (size_t i = 0; i < _nbElements; ++i)
				{
					element->~Atomic32();
					++element;
				}
				Memory::alignedFree(_array);
			}

		private:
			volatile u32 m_value;
		};

	} // namespace sound
} // namespace ITF