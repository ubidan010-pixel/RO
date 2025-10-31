#ifndef _ITF_SAFEARRAY_H_
#define _ITF_SAFEARRAY_H_

#pragma once

#include "core/types.h"
#include "core/Archive.h"
#include "core/crc.h"

namespace ITF
{
    namespace SafeArrayUtils
    {
        i32     find32(u32 _data2Test, u32* _data, u32 Number, i32* _lastFoundIndex);
        i32     find16(u16 _data2Test, u16* _data, u32 Number, i32* _lastFoundIndex);
    }

	template<u32 _bytes, bbool _hasStaticBuffer>
	class SafeArrayStaticBuffer
	{
	public:
		u32 *getBuffer() {return m_staticBuffer;}
		u32 getBufferItemCount() {return 1;}

	private:
		u32 m_staticBuffer[(_bytes+3)/4];
	};

	template<u32 _bytes>
	class SafeArrayStaticBuffer<_bytes, 0>
	{
	public:
		u32 *getBuffer() const {return NULL;}
		u32 getBufferItemCount() const {return 0;}
	};


	template <class T, u32 ResizeMin = 8, MemoryId::ITF_ALLOCATOR_IDS allocID = MemoryId::mId_SafeArray, bbool _hasStaticBuffer = btrue, bbool _copyFit = btrue>
	class SafeArray
	{
	public:
		SafeArray();
		explicit SafeArray (u32 _reserve, MemoryId::ITF_ALLOCATOR_IDS _allocID = MemoryId::mId_SafeArray,T *_buffer=NULL);
		SafeArray   (   const SafeArray& _original   );
		~SafeArray  (   );

		T&			back()					{ITF_ASSERT(m_size);return m_data[m_size-1];}
		const T&	back()const				{ITF_ASSERT(m_size);return m_data[m_size-1];}
		void		pop_back()				{ITF_ASSERT(m_size);--m_size;}
		const T*    getDataBuffer() const	{return m_data;}
	
		void		push_back		(const T& _data);
		void		push_back_sort  (const T& _data);

		void		clear()					{resize(0);}
		u32			size() const			{return m_size;}
		u32			capacity() const		{return m_capacity;}
		void		fillZero()				{ITF_MemSet(m_data, 0, sizeof(T) * m_size);}
		void		fill( const T& _data );

		// Operators
		const T&	operator[] ( size_t _index) const {ITF_ASSERT_CRASH(_index < size_t(m_size), "Out of bound access");return m_data[_index];}
		T&			operator[] (size_t _index)		{ITF_ASSERT_CRASH(_index < size_t(m_size), "Out of bound access");return m_data[_index];}
		bool		operator==(const SafeArray& other) const;
		SafeArray&  operator=(const SafeArray& _src);

		// Iterators
		typedef T* iterator;
		typedef const T* const_iterator;

		iterator		begin()			{return m_data; }
		const_iterator	begin() const	{return m_data; }
		iterator		end()			{return m_data + m_size; }
		const_iterator	end() const		{return m_data + m_size; }


		void		eraseKeepOrder(u32 _index);
		void		eraseNoOrder (u32 _index);
		void		eraseRangeNoOrder (   u32 _startIndex, u32 _endIndex  );
		void		buildFrom ( const SafeArray& _from,  u32 _startIndex, u32 _endIndex  );

		// ************************************************************************************
		i32			findWithPrefetch(   const T& _data)  const {return find(_data);}
		i32			find        (   const T& _data)  const;
		i32			sortedFind        (   const T& _data) const;
		i32			findStartIndex(   const T& _data, u32 _startIndex  ) const;
		void		insert      (   const T& _data, u32 _atIndex );

		void		reserve(size_t _size);	// Can make the capacity grow, it never reduces it.
		void		resize(size_t _size);		// Set the new size and make the array grow if necessary
		void		setCapacity(size_t _size);	// Set the new capacity: it can lower the capacity.

		T&			incrSize(u32 _count = 1);
		void		fit()			{ setCapacity(m_size); }

		// merges parameter array at the end of current array
		void		mergeArray (const SafeArray& _other);

		void		serialize(ArchiveMemory& _archive);
		u32			computeCRC() const					{	return CRC::compute32( (unsigned char*)m_data, sizeof(T) * m_size); }

		// ************************************************************************************
		void		quickSort()					{quickSort(SafeArray::compare);}

		// ************************************************************************************
		template <typename functorT>	
		void		quickSort(const functorT& _quickCompareFunct)
		{
			quickSortProcess(_quickCompareFunct, 0, size() - 1);
		}

	private:

		void		resizeNoConstructor( u32 _size );

		//Default compare function 
		static i32 compare(const T& _A, const T& _B)
		{
			if (_A < _B)
			{
				return -1;
			}
			else if (_B < _A)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}

		template <typename functorT>
		void quickSortProcess(const functorT& _quickCompareFunct, i32 _startIndex, i32 _endIndex)
		{
			if (_startIndex >= _endIndex)
			{
				return;
			}

			ITF_ASSERT_CRASH(_startIndex >= 0, "Bad index in SafeArray !");
			ITF_ASSERT_CRASH(_endIndex < (i32)size(), "Bad index in SafeArray !");

			i32 leftIndex   = _startIndex - 1;
			i32 rightIndex  = _endIndex + 1;

			const T& pivot = m_data[_startIndex];

			for (;;) // to prevent warning : while(1)
			{
				do 
				{
					rightIndex--;
				} while (_quickCompareFunct(m_data[rightIndex], pivot) > 0); // m_data[rightIndex] > pivot

				do 
				{
					leftIndex++;
				} while (_quickCompareFunct(m_data[leftIndex],pivot) < 0); // m_data[leftIndex]  < pivot

				if (leftIndex < rightIndex)
				{
					quickSortSwitch(leftIndex, rightIndex);
				}
				else
				{
					break;
				}
			}

			quickSortProcess(_quickCompareFunct, _startIndex, rightIndex);
			quickSortProcess(_quickCompareFunct, rightIndex+1, _endIndex);
		}
 
		void quickSortSwitch(i32 _indexA, i32 _indexB)
		{
			T dataTemp = m_data[_indexA];

			m_data[_indexA] = m_data[_indexB];
			m_data[_indexB] = dataTemp;
		}

	#ifdef _DEBUG
		bool		isOnStack() const;
	#endif //_DEBUG

	private:

		T*			m_data;
		u32			m_size;
		u32			m_capacity:25;
		u32			m_noFree:1; // m_noFree -> should not free m_data
		u32			m_allocID:6;

		SafeArrayStaticBuffer<sizeof(T),_hasStaticBuffer> m_staticBuffer;


		void IncreaseCapacity();
		void PrivateIncreaseSize(u32 _size);
		void PrivateDecreaseSize(u32 _size);

		// Generic behavior (call constructor and destructor)
		template <class P> static void Constructor(P *_Pointer) {new(_Pointer) P;}
		template <class P> static void Destructor(P *_Pointer)  {/*_Pointer->~P();*/}
		template <class P> static void ArrayConstructor(P *_array, ux _start, ux _end) 
		{
			// We call placement new (constructor) on new objects
			T * array = _array+_start;
			T * arrayEnd = _array+_end;
			for (; array <arrayEnd; array++)
				new(array) T;    
		}

		template <class P> inline static void ArrayDestructor(P *_array, ux _start, ux _end) 
		{
			// We call destructor on old objects
			/*T * array = _array+_start;
			T * arrayEnd = _array+_end;
			for (; array <arrayEnd; array++)
				array->~T();*/
		}

		template <class P> static void copy(P *_dst, P*_src, ux _size) 
		{
			/*
			P *dstLast = _dst+_size;
			for(;_dst<dstLast;)
				*(_dst++) = *(_src++); // Call the operator =
			*/
			ITF_Memcpy(_dst,_src,_size*sizeof(P));
		}

		// Specific behaviors for Plain Old Data
		template <class P> static void Constructor(P **_Pointer) {}
		template <class P> static void Destructor(P **_Pointer) {}
		template <class P> static void ArrayConstructor(P **_pArray, ux _uStart, ux _uEnd) {}
		template <class P> static void ArrayDestructor(P **_pArray, ux _uStart, ux _uEnd) {}
		template <class P> static void copy(P **_dst, P**_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(P*));}

		static void Constructor(f32 *_Pointer) {}
		static void Destructor(f32 *_Pointer) {}
		static void ArrayConstructor(f32 *_pArray, ux _uStart, ux _uEnd) {}
		static void ArrayDestructor(f32 *_pArray, ux _uStart, ux _uEnd) {}
		static void copy(f32 *_dst, f32 *_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(f32));}

		static void Constructor(f64 *_Pointer) {}
		static void Destructor(f64 *_Pointer) {}
		static void ArrayConstructor(f64 *_pArray, ux _uStart, ux _uEnd) {}
		static void ArrayDestructor(f64 *_pArray, ux _uStart, ux _uEnd) {}
		static void copy(f64 *_dst, f64 *_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(f64));}

		static void Constructor(u32 *_Pointer) {}
		static void Destructor(u32 *_Pointer) {}
		static void ArrayConstructor(u32 *_pArray, ux _uStart, ux _uEnd) {}
		static void ArrayDestructor(u32 *_pArray, ux _uStart, ux _uEnd) {}
		static void copy(u32 *_dst, u32 *_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(u32));}

		static void Constructor(i32 *_Pointer) {}
		static void Destructor(i32 *_Pointer) {}
		static void ArrayConstructor(i32 *_pArray, ux _uStart, ux _uEnd) {}
		static void ArrayDestructor(i32 *_pArray, ux _uStart, ux _uEnd) {}
		static void copy(i32 *_dst, i32 *_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(i32));}

		static void Constructor(u16 *_Pointer) {}
		static void Destructor(u16 *_Pointer) {}
		static void ArrayConstructor(u16 *_pArray, ux _uStart, ux _uEnd) {}
		static void ArrayDestructor(u16 *_pArray, ux _uStart, ux _uEnd) {}
		static void copy(u16 *_dst, u16 *_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(u16));}

		static void Constructor(i16 *_Pointer) {}
		static void Destructor(i16 *_Pointer) {}
		static void ArrayConstructor(i16 *_pArray, ux _uStart, ux _uEnd) {}
		static void ArrayDestructor(i16 *_pArray, ux _uStart, ux _uEnd) {}
		static void copy(i16 *_dst, i16 *_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(i16));}

		static void Constructor(u8 *_Pointer) {}
		static void Destructor(u8 *_Pointer) {}
		static void ArrayConstructor(u8 *_pArray, ux _uStart, ux _uEnd) {}
		static void ArrayDestructor(u8 *_pArray, ux _uStart, ux _uEnd) {}
		static void copy(u8 *_dst, u8 *_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(u8));}

		static void Constructor(i8 *_Pointer) {}
		static void Destructor(i8 *_Pointer) {}
		static void ArrayConstructor(i8 *_pArray, ux _uStart, ux _uEnd) {}
		static void ArrayDestructor(i8 *_pArray, ux _uStart, ux _uEnd) {}
		static void copy(i8 *_dst, i8 *_src, ux _size) {ITF_Memcpy(_dst,_src,_size*sizeof(i8));}
	};

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::SafeArray() : m_data(NULL), m_size(0), m_capacity(0),m_noFree(0)
	{
		ITF_COMPILE_TIME_ASSERT(default_size_is_0, ResizeMin>0);
		m_allocID  = allocID;
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::SafeArray(u32 _reserve, MemoryId::ITF_ALLOCATOR_IDS _allocID,T *_buffer)
	{
		ITF_COMPILE_TIME_ASSERT(default_size_is_0, ResizeMin>0);
		m_allocID  = _allocID;

		m_size = 0;
		if (_buffer)
		{
			m_data = _buffer;
			m_noFree = 1;
			m_capacity = _reserve;
		}
		else
		{
			m_data = NULL;
			m_capacity = 0;
			m_noFree = 0;
			reserve(_reserve);
		}
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::SafeArray   (   const SafeArray& _original   )
	{
		ITF_COMPILE_TIME_ASSERT(default_size_is_0, ResizeMin>0);

		m_allocID  = _original.m_allocID;
		m_size = 0;
		m_capacity = 0;
		m_data = NULL;
		resizeNoConstructor(_original.size());
		if (m_data)	
			copy(m_data,_original.m_data,size());
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::~SafeArray  (   )
	{
		setCapacity(0);
	#ifndef ITF_FINAL
		m_size = 0xdeadbeef; // marks deletion
	#endif
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline bool SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::operator==(const SafeArray& other) const
	{
		if (other.m_size != m_size)
			return bfalse;

		if (m_size==0)
			return btrue;

		return (memcmp(m_data, other.m_data, m_size * sizeof(T)) == 0);
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>&  SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::operator=(const SafeArray& _src)
	{
		if (this == &_src)
			return *this;

		ITF_DISABLE_WARNING_PUSH
		ITF_DISABLE_WARNING_ID(4127)
		if (_copyFit)
		{
			setCapacity(size_t(_src.m_size));
			resize(size_t(_src.m_size));
		}
		else
			resizeNoConstructor(_src.m_size);
		ITF_DISABLE_WARNING_POP
		copy(m_data,_src.m_data,m_size);

		m_allocID = _src.m_allocID;

		return *this;
	}


	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void    SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::push_back   (   const T& _data )
	{
		if (m_size == m_capacity)
			IncreaseCapacity();

		//T *pT = m_data+m_size;
		//Constructor(pT);
		//*pT = _data;                 // Calls operator= 
		//m_size++;
		m_data[m_size++] = _data;
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void    SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::push_back_sort   (   const T& _data )
	{
		i32 lowIndex = 0;
		i32 highIndex = m_size;
		i32 midIndex = (lowIndex + highIndex) / 2;
		while(midIndex > lowIndex)
		{
			if (m_data[midIndex] <= _data )
				lowIndex = midIndex;
			else
				highIndex = midIndex;
			midIndex = (lowIndex + highIndex) / 2;
		}
		if (0 == m_size || m_data[midIndex] > _data)
			insert(_data, midIndex);
		else
			insert(_data, midIndex+1);

	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void	SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::fill( const T& _data )
	{
		T *data = m_data;
		T *dataEnd = m_data+m_size;
		for(;data <dataEnd; ++data)
			*data = _data;
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::eraseKeepOrder (   u32 _index  )
	{
		ITF_ASSERT_CRASH(_index < m_size, "Out of bound access... forcing crash to prevent any corruption");

		m_size--;
		if (_index == m_size)
			return;

		memmove(&m_data[_index], &m_data[_index + 1], sizeof(T) * (m_size - _index));
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::eraseNoOrder (   u32 _index  )
	{
		ITF_ASSERT_CRASH(_index < m_size, "Out of bound access... forcing crash to prevent any corruption");

		if (_index == m_size-1)
		{
			m_size--;
			return;
		}

		memmove(&m_data[_index], &m_data[m_size-1], sizeof(T));
		m_size--;
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::eraseRangeNoOrder (   u32 _startIndex, u32 _endIndex  ) // as in stl : [_startIndex, _endIndex), which means that _startIndex is erased while _endIndex isn't
	{
		ITF_ASSERT_CRASH(_endIndex <= m_size, "Out of bound access... forcing crash to prevent any corruption");
		ITF_ASSERT_CRASH(_startIndex < _endIndex, "wrong indexes... forcing crash to prevent any corruption");

		u32 delta = _endIndex - _startIndex;

		if (_endIndex == m_size)
		{
			m_size -= delta;
			return;
		}

		u32 availableCountAfterEndIndex = m_size - _endIndex;
		u32 movedBytes = delta;
		if (availableCountAfterEndIndex < delta)
			movedBytes = availableCountAfterEndIndex;

		memmove(&m_data[_startIndex], &m_data[m_size-delta], sizeof(T) * movedBytes);

		m_size -= movedBytes;
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::buildFrom ( const SafeArray& _from,  u32 _startIndex, u32 _endIndex  ) // [_startIndex, _endIndex), which means that _startIndex is included while _endIndex isn't
	{
		ITF_ASSERT_CRASH(_endIndex <= _from.m_size, "Out of bound access... forcing crash to prevent any corruption");
		ITF_ASSERT_CRASH(_startIndex < _endIndex, "wrong indexes... forcing crash to prevent any corruption");
		ITF_ASSERT_CRASH(this != &_from, "can't self-copy, use erase instead");

		u32 delta = _endIndex - _startIndex;

		resizeNoConstructor(delta);
		copy(m_data,_from.m_data+_startIndex,delta);
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline i32  SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::find (const T& _data)  const 
	{
	#if defined (ITF_WINDOWS) || defined (ITF_X360)
	#pragma warning( push )
	#pragma warning( disable : 4127 )
	#endif
		if (sizeof(T) == sizeof(u32)) 
		{
			i32 Res = SafeArrayUtils::find32(
			*((u32*)&_data), 
			(u32*)(&m_data[0]), 
			m_size,
			NULL
			) ;
			return Res;
		}
		else if (sizeof(T) == sizeof(u16))
		{
			i32 Res = SafeArrayUtils::find16(
			*((u16*)&_data), 
			(u16*)(&m_data[0]), 
			m_size,
			NULL
			) ;
			return Res;
		}
		else
		{
			T *Parse = m_data;
			for (u32 i = 0; i < m_size; i++)
			{
				if (*(Parse++) == _data)
					return (i32)i;
			}
			return -1;
		}
	#if defined (ITF_WINDOWS) || defined (ITF_X360)
	#pragma warning( pop )
	#endif
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline i32 SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::sortedFind  (   const T& _data) const 
	{
		if (0 == m_size)
			return -1;

		i32 lowIndex = 0;
		i32 highIndex = m_size;
		i32 midIndex = (lowIndex + highIndex) / 2;
		while(midIndex > lowIndex)
		{
			if (m_data[midIndex] <= _data )
				lowIndex = midIndex;
			else 
				highIndex = midIndex;
			midIndex = (lowIndex + highIndex) / 2;
		}
		if (_data == m_data[midIndex])
			return midIndex;
		return -1;
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline i32  SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::findStartIndex(   const T& _data, u32 _startIndex  ) const
	{
		ITF_ASSERT(_startIndex <= m_size);

		for (u32 i = _startIndex; i < m_size; i++)
		{
			if (m_data[i] == _data)
				return (i32)i;
		}
		return -1;
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void    SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::insert      (   const T& _data, u32 _atIndex )
	{
		u32 uShiftSize = m_size - _atIndex;
		ITF_ASSERT(m_size >= _atIndex);

		// Increment size
		if (m_size == m_capacity)
			IncreaseCapacity();
		m_size = m_size + 1;

		// Shift end of array
		if (uShiftSize>0)
			memmove(m_data + _atIndex + 1, m_data + _atIndex, uShiftSize * sizeof(T));

		// Replace data at wished position
		T	*pT = m_data + _atIndex;

		//Constructor(pT);
		*pT = _data; // Calls operator= 
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::resize(const size_t _size)
	{
        const u32 size = u32(_size);
		if(size > m_size) 
		{
			if(size > m_capacity)
			{
				reserve(size);
			}
			PrivateIncreaseSize(size);
		}
		else if (size < m_size)
		{
			PrivateDecreaseSize(size);
		}
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::resizeNoConstructor(u32 _size)
	{
		if(_size > m_capacity) 
			reserve(_size);
		m_size = _size;
	}


	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::reserve(size_t _size)
	{
		if (_size > size_t(m_capacity))
			setCapacity(_size);
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::setCapacity(const size_t _size)
	{
        const u32 size = u32(_size);
		if(size != m_capacity)
		{
			// Reduce the size of the array if needed, before actual realloc.
			if(size < m_size)
				PrivateDecreaseSize(size);

			void *newArray;
			u32 noFree = 0;
			if (size <= m_staticBuffer.getBufferItemCount())
			{
				noFree = 1;
				newArray = (size ? m_staticBuffer.getBuffer() : NULL);
			}
			else
			{
				newArray = ITF::Memory::mallocCategory(size * sizeof(T), (MemoryId::ITF_ALLOCATOR_IDS) m_allocID );
			}

			ITF_ASSERT_CRASH((size == 0) || (newArray != NULL),"SafeArray::setCapacity newArray NULL");
			if (m_data)
			{
				if (m_size)
					ITF_Memcpy(newArray, m_data, m_size* sizeof(T));

				if (!m_noFree)
					ITF::Memory::free(m_data);
			}

			m_noFree = noFree;
			m_data = (T *)newArray;
			m_capacity = size;
		}
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline T& SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::incrSize(u32 _count)
	{
		if((size()+_count) > capacity())
			reserve(size()*2 + _count);
		resize(size()+_count);
		return m_data[m_size-_count];
	}

	// merges parameter array at the end of current array
	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void    SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::mergeArray (const SafeArray& _other)
	{
		u32 dstSize = size();
		u32 srcSize = _other.size();
		resizeNoConstructor(dstSize+srcSize);
		copy(m_data+dstSize,_other.m_data,srcSize);
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::serialize(ArchiveMemory& _archive)
	{
		if (_archive.isReading())
		{
			u32 _size = 0;
			_archive.serialize(_size);

			clear();
			resize(_size);
		}
		else
		{
			u32 _size = size();
			_archive.serialize(_size);
		}
		for (iterator iter = begin();iter != end();++iter)
		{
			T&  val = (*iter);
			serializeValue(val,_archive);
		}
	}


	#ifdef _DEBUG
	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline bool SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::isOnStack() const
	{ 
		int a;
		iPtr stackDistance = iPtr(this) - iPtr(&a);
		return abs(stackDistance) < 4096;
	}
	#endif //_DEBUG

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::IncreaseCapacity()
	{
		const u32 ResizeMul = 4;
		const u32 ResizeDiv = 3;

		ITF_ASSERT_STATIC(ResizeMul > ResizeDiv, "TOO_ResizeMul should be bigger than TOO_ResizeDiv");

		// Resize is done so that by default we have : 4, 8, 16, 30, 49, ... 
		// For big reallocates, there is a 50% increase (TOO_ResizeMul / TOO_ResizeDiv).
		// Thus, the buffer grows fast at first to avoid too many allocs, and then growth slows down.
		/*u32 uNewMaxSize = ResizeMin + ((m_capacity * ResizeMul) / ResizeDiv);

		// Resize should not be bigger than * 2 (except for first time)
		if (uNewMaxSize > m_capacity * 2)
			uNewMaxSize = m_capacity * 2;

		if (uNewMaxSize < ResizeMin)
			uNewMaxSize = ResizeMin;*/

		// Detect overflow (more than 2^16 elements with I == u16)
		//ITF_ASSERT(uNewMaxSize < (1<<(sizeof(I)*8)));*/

		u32 uNewMaxSize = (m_capacity * 2 > ResizeMin ? m_capacity * 2 : ResizeMin);
		reserve(uNewMaxSize);
	}

	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::PrivateIncreaseSize(u32 _size)
	{
		ITF_ASSERT(_size > m_size);
		ArrayConstructor(m_data,m_size,_size);
		m_size = _size;
	}


	template <class T, u32 ResizeMin,MemoryId::ITF_ALLOCATOR_IDS allocID, bbool _hasStaticBuffer, bbool _copyFit>
	inline void SafeArray<T,ResizeMin,allocID,_hasStaticBuffer,_copyFit>::PrivateDecreaseSize(u32 _size)
	{
		ITF_ASSERT(_size < m_size);
		ArrayDestructor(m_data,_size,m_size);
		m_size = _size;
	}

} // namespace ITF

#endif // _ITF_SAFEARRAY_H_
