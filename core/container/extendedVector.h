#ifndef    ITF_VECTOR_H_
#define    ITF_VECTOR_H_

#pragma once

#include "core/types.h"
#include <vector>

#include "core/memory/STDAllocatorOnITFMemory.h"

namespace ITF
{
    template <typename T, ITF::MemoryId::ITF_ALLOCATOR_IDS tag>
    using VectorAllocator = STDAllocatorOnITFMemory<T, tag>;

    template <typename T> struct Vector_MemIDS_traits {enum { Value = ITF::MemoryId::mId_Vector }; };
    template <typename T> struct Vector_MemIDS_traits<T*> {enum { Value = ITF::MemoryId::mId_VectorPointer }; };
    #define VECTAGSET(type,tag) template<> struct Vector_MemIDS_traits<type> {enum { Value = tag }; };

    class ObjectRef;
    VECTAGSET(ObjectRef,                ITF::MemoryId::mId_VectorObjectRef)
    class String;
    VECTAGSET(String,                   ITF::MemoryId::mId_VectorString)
    class String8;
    VECTAGSET(String8,                  ITF::MemoryId::mId_VectorString)
    class String_Uncached;
    VECTAGSET(String_Uncached, ITF::MemoryId::mId_VectorString)
    class VertexPCT;
    VECTAGSET(VertexPCT,                ITF::MemoryId::mId_Vertex)
    VECTAGSET(u64,                      ITF::MemoryId::mId_VectorInt)
    VECTAGSET(i64,                      ITF::MemoryId::mId_VectorInt)
    VECTAGSET(u32,                      ITF::MemoryId::mId_VectorInt)
    VECTAGSET(i32,                      ITF::MemoryId::mId_VectorInt)
    VECTAGSET(u16,                      ITF::MemoryId::mId_VectorInt)
    VECTAGSET(i16,                      ITF::MemoryId::mId_VectorInt)
    VECTAGSET(u8,                       ITF::MemoryId::mId_VectorInt)
    VECTAGSET(i8,                       ITF::MemoryId::mId_VectorInt)

    template <class T>
    class Vector : public STD_VECTOR<T, VectorAllocator<T, (ITF::MemoryId::ITF_ALLOCATOR_IDS)Vector_MemIDS_traits<T>::Value> >
    {
    public:

        typedef typename STD_VECTOR_QUALIFIED<T, VectorAllocator<T, (ITF::MemoryId::ITF_ALLOCATOR_IDS)Vector_MemIDS_traits<T>::Value> > base_container;

    public:
        typedef typename base_container::value_type				value_type;
        typedef typename base_container::pointer				pointer;
        typedef typename base_container::const_pointer			const_pointer;
        typedef typename base_container::reference				reference;
        typedef typename base_container::const_reference		const_reference;
        typedef typename base_container::iterator				iterator;
        typedef typename base_container::const_iterator			const_iterator;
        typedef typename base_container::reverse_iterator		reverse_iterator;
        typedef typename base_container::const_reverse_iterator	const_reverse_iterator;
        typedef typename base_container::size_type				size_type;
        typedef typename base_container::difference_type		difference_type;
        typedef typename base_container::allocator_type			allocator_type;


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief  Removes an element at index given in parameter
        /// @param  _index    The index.
        void removeAt(size_type _index)
        {
            this->erase(this->begin() + _index);
        }

        void serialize(ArchiveMemory& _archive)
        {   
            if (_archive.isReading())
            {
                u32 _size = 0;
                _archive.serialize(_size);

                this->clear();
                this->resize(_size);
            }
            else
            {
                u32 _size = this->size();
                _archive.serialize(_size);

            }
			for (iterator iter = this->begin();iter != this->end();++iter)
			{
				T&  val = (*iter);
				serializeValue(val,_archive);
			}
		}
		
		explicit Vector(): base_container()
		{
		}

        explicit Vector(uSize size) : base_container(size)
        {
        }

        // Force to return a 32 bit as there is too much case where the size is assuming it.
        // Assuming no vector size go above 4GiB is acceptable.
        [[nodiscard]] uSize size() const noexcept
        {
            return static_cast<uSize>(base_container::size());
        }

        // Same trick for capacity
        [[nodiscard]] uSize capacity() const noexcept
        {
            return static_cast<uSize>(base_container::capacity());
        }
	
		void resize(size_type _newSize)
		{
			if (_newSize != this->size())
				base_container::resize(_newSize);
		}

        void resize(size_type _newSize, const T & _val)
        {
            if (_newSize != this->size())
                base_container::resize(_newSize, _val);
        }

    };

	//template< typename T >
	//struct VectorAnim	{
	//	typedef Vector<T,e_mem_tag_anim> Type;
	//};
	template <class T>
    class VectorAnim : public Vector<T>
	{
	};

} // namespace ITF

template <typename T>
using ITF_VECTOR = ITF::Vector<T>;

#endif    /* ITF_VECTOR_H_ */

