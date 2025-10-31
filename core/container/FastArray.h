#ifndef _ITF_FASTARRAY_H_
#define _ITF_FASTARRAY_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_


#ifdef _MSC_VER
//#pragma once
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

#include <algorithm>	// for sort

namespace ITF
{

#ifndef TOOindex
#define TOOindex        u32
#define TOOinvalidindex 0xFFFFFFFF
#endif 

// T : type contained by the FastArray
// I : type of the index to the FastArray. For example, if your FastArray has never more than 65536 elements, you can specify u16 -> it will use 8 bytes instead of 12.
// If ALIGN == 0, the buffer is aligned with the default memory alignment. Else, the buffer is aligned with ALIGN.
// If ASSIGNOP == true, the operator= is used when resizing the array

template<class T, typename I = u32, u32 ALIGN = 0, bool ASSIGNOP = true>
class FastArray 
{
    // FastArray_compare must return < 0 if *_p_Elem1 < *_p_Elem2, 0 if *_p_Elem1 == *_p_Elem2, > 0 if *_p_Elem1 > *_p_Elem2
    typedef int (*FastArray_compare) (const void *_p_Elem1, const void *_p_Elem2);

    // ===================================================================================================================
    // PUBLIC INTERFACE
    // ===================================================================================================================
public:

    FastArray() :m_data(NULL),m_elemCount(0),m_capacity(0)
	{
#ifndef ITF_FINAL
		m_allocID = MemoryId::mId_SafeArray;
#endif // ITF_FINAL
	}
    FastArray(const FastArray & src) { *this = src; }
    ~FastArray() {destroy();}

    void destroy(void)				{reserve(0);} // free memory
    void clear(void)				{resize(0);}  // does not free memory

    // Size
    void resize(u32 _u_Size);
    void reserve(u32 _u_Size);
    void fit(void)					{reserve(m_elemCount);} // realloc so that capacity fits size.
	u32	 size() const				{return m_elemCount;}
	u32	 capacity() const			{return m_capacity;}
	bool empty(void) const			{return !m_elemCount;}

	// Fill
    void fillZero()					{ITF_MemSet(m_data, 0, sizeof(T) * m_elemCount);}
	void fill( const T& _data );

	// Remove/add/access
    void	push_back(T const &_Value);
	const T &back() const			{ITF_ASSERT(m_elemCount > 0);return m_data[m_elemCount - 1];}
	T &     back()					{ITF_ASSERT(m_elemCount > 0);return m_data[m_elemCount - 1];}
	void    pop_back()				{ITF_ASSERT(m_elemCount);--m_elemCount;}
    T&		push_empty();
    void	insert(T const &_data,u32 _atIndex);
	void	replace(T const &_data,u32 _atIndex);

    //  Find the first occurrence of value and remove it (replace it with last value). Return false if value could not be found.
    bool	findRemove(T const &_value);

    // Find an element if it is not already in the table.
    int		find(T const &_value) const;

    void	removeAtOrdered(int _pos);
	void	removeAtOrdered(int _pos, int _count);
    void	removeAt(int _pos);
    void	shiftAt(int _pos, int _shift);
    void	swap(int _pos1, int _pos2);
	void    swap( FastArray &other);
	void	assign( u32 _num, T const &_value);
	void	setData(T *_data, u32 _size);

	void	serialize(ArchiveMemory& _archive);


    // sort, using ::std::sort. For example of use, look at TOOBoundaryComparator (used by TOOkdtree)
    template <class Comparator> void sort(Comparator comp) {::std::sort<T*, Comparator&>(m_data, m_data+m_elemCount, comp);}
	void sort() { ::std::sort(m_data, m_data + m_elemCount); }

    // Sort, using qsort. This is not optimal because it uses memcpy instead of operator =, try using sort instead. 
    void QSort(FastArray_compare _p_CompareFunction) {qsort(m_data, m_elemCount, sizeof(T), _p_CompareFunction);}

    T *      getDataBuffer()        {return m_data;}
    const T *getDataBuffer() const	{return m_data;}

	// Iterators
	typedef T* iterator;
	typedef const T* const_iterator;

	iterator		begin()			{return m_data; }
	const_iterator	begin() const	{return m_data; }

	iterator end()					{return m_data + m_elemCount; }
	const_iterator end() const		{return m_data + m_elemCount; }

	// Operators
	T &      operator[](TOOindex _x_Index)				{ITF_ASSERT(_x_Index < m_elemCount);return m_data[_x_Index];}
	const T &operator[](TOOindex _x_Index) const		{ITF_ASSERT(_x_Index < m_elemCount);return m_data[_x_Index];}
	const FastArray &operator =(const FastArray &_src);

    // ===================================================================================================================
    // PRIVATE PART
    // ===================================================================================================================
protected:
    // Attributes
    T	*m_data;	
    I	m_elemCount;	
	I	m_capacity;

#ifndef ITF_FINAL
	MemoryId::ITF_ALLOCATOR_IDS m_allocID;
#endif // ITF_FINAL

    // Values used by automatic resize.
    static const u32 TOO_ResizeMul = 3;
    static const u32 TOO_ResizeDiv = 2;
    static const u32 TOO_ResizeMin = 4;
    static const u32 TOO_ResizeConst = 4;

protected:
    // Private methods
    void IncreaseMaxSizeIfFull();
    void PrivateIncreaseSize(u32 _u_Size);
    void PrivateDecreaseSize(u32 _u_Size);

    // Generic behavior (call constructor and destructor)
    template <class P> static void Constructor(P *_Pointer) {new(_Pointer) P;}
    template <class P> static void Destructor(P *_Pointer)  {_Pointer->~P();}
    template <class P> static void ArrayConstructor(P *_pArray, ux _uStart, ux _uEnd) 
    {
        // We call placement new (constructor) on new objects
        ux i;
        for (i=_uStart; i<_uEnd; i++)
        {
            T *pT = _pArray+i;
            new(pT) T; // Placement new (does not allocate memory, just calls the constructor)
        }   
    }

    template <class P> inline static void ArrayDestructor(P *_pArray, ux _uStart, ux _uEnd) 
    {
        // We call destructor on old objects
        ux i;
        for (i=_uStart; i<_uEnd; i++)
        {
            (_pArray+i)->~T();
        }
    }

    // Specific behaviors (don't call constructor or destructor)
    template <class P> static void Constructor(P **_Pointer) {}
    template <class P> static void Destructor(P **_Pointer) {}
    template <class P> static void ArrayConstructor(P **_pArray, ux _uStart, ux _uEnd) {}
    template <class P> static void ArrayDestructor(P **_pArray, ux _uStart, ux _uEnd) {}

    static void Constructor(f32 *_Pointer) {}
    static void Destructor(f32 *_Pointer) {}
    static void ArrayConstructor(f32 *_pArray, ux _uStart, ux _uEnd) {}
    static void ArrayDestructor(f32 *_pArray, ux _uStart, ux _uEnd) {}

    static void Constructor(f64 *_Pointer) {}
    static void Destructor(f64 *_Pointer) {}
    static void ArrayConstructor(f64 *_pArray, ux _uStart, ux _uEnd) {}
    static void ArrayDestructor(f64 *_pArray, ux _uStart, ux _uEnd) {}

    static void Constructor(u32 *_Pointer) {}
    static void Destructor(u32 *_Pointer) {}
    static void ArrayConstructor(u32 *_pArray, ux _uStart, ux _uEnd) {}
    static void ArrayDestructor(u32 *_pArray, ux _uStart, ux _uEnd) {}

    static void Constructor(i32 *_Pointer) {}
    static void Destructor(i32 *_Pointer) {}
    static void ArrayConstructor(i32 *_pArray, ux _uStart, ux _uEnd) {}
    static void ArrayDestructor(i32 *_pArray, ux _uStart, ux _uEnd) {}

    static void Constructor(u16 *_Pointer) {}
    static void Destructor(u16 *_Pointer) {}
    static void ArrayConstructor(u16 *_pArray, ux _uStart, ux _uEnd) {}
    static void ArrayDestructor(u16 *_pArray, ux _uStart, ux _uEnd) {}

    static void Constructor(i16 *_Pointer) {}
    static void Destructor(i16 *_Pointer) {}
    static void ArrayConstructor(i16 *_pArray, ux _uStart, ux _uEnd) {}
    static void ArrayDestructor(i16 *_pArray, ux _uStart, ux _uEnd) {}

    static void Constructor(u8 *_Pointer) {}
    static void Destructor(u8 *_Pointer) {}
    static void ArrayConstructor(u8 *_pArray, ux _uStart, ux _uEnd) {}
    static void ArrayDestructor(u8 *_pArray, ux _uStart, ux _uEnd) {}

    static void Constructor(i8 *_Pointer) {}
    static void Destructor(i8 *_Pointer) {}
    static void ArrayConstructor(i8 *_pArray, ux _uStart, ux _uEnd) {}
    static void ArrayDestructor(i8 *_pArray, ux _uStart, ux _uEnd) {}
};


template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::resize(u32 _size)
{
    if(_size > m_elemCount) 
    {
        if(_size > m_capacity) 
        {
            reserve(_size);
        }
        PrivateIncreaseSize(_size);
    }
    else if (_size < m_elemCount)
    {
        PrivateDecreaseSize(_size);
    }
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::reserve(u32 _size)
{
    if(_size != m_capacity)
    {
        // Reduce the size of the array if needed, before actual realloc.
        if(_size < m_elemCount) 
            PrivateDecreaseSize(_size);

		void *newArray;

        // The branch is chosen at compile time (because ALIGN is a template argument)
		if (_size)
		{
			if(ALIGN)
			{
#ifndef ITF_FINAL
				newArray = ITF::Memory::alignedMallocCategory(_size * sizeof(T), ALIGN, m_allocID);
#else // ITF_FINAL
				newArray = ITF::Memory::alignedMalloc(_size * sizeof(T), ALIGN);
#endif // ITF_FINAL
			}
			else
			{
#ifndef ITF_FINAL
				newArray = ITF::Memory::mallocCategory(_size * sizeof(T), m_allocID);
#else // ITF_FINAL
				newArray = ITF::Memory::malloc(_size * sizeof(T));
#endif // ITF_FINAL
			}
		}
		else
			newArray = NULL;

		ITF_ASSERT((_size == 0) || (newArray != NULL));
		if (m_data)
		{
			if (m_elemCount)
				ITF_Memcpy(newArray,m_data,m_elemCount* sizeof(T));

			if(ALIGN)
				ITF::Memory::alignedFree(m_data);
			else
				ITF::Memory::free(m_data);
		}

        m_data = (T *)newArray;
        m_capacity = static_cast<I>(_size);
    }
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::fill( const T& _data )
{
	for(u32 i = 0; i < m_elemCount; ++i)
		m_data[i] = _data;
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::push_back(T const &_Value)
{
    IncreaseMaxSizeIfFull();

    T *pT = m_data+m_elemCount;
    Constructor(pT);
    *pT = _Value;                 // Calls operator= 
    m_elemCount++;
}


template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline T & FastArray<T,I,ALIGN,ASSIGNOP>::push_empty()
{
    IncreaseMaxSizeIfFull();
    resize(m_elemCount+1);
    return m_data[m_elemCount - 1];
}


template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void	FastArray<T,I,ALIGN,ASSIGNOP>::insert(T const &_data,u32 _atIndex)
{
    u32 uShiftSize = m_elemCount - _atIndex;
    ITF_ASSERT(m_elemCount >= _atIndex);

    // Increment size
    IncreaseMaxSizeIfFull();
    m_elemCount = m_elemCount + 1;

    // Shift end of array
    if (uShiftSize>0)
        memmove(m_data + _atIndex + 1, m_data + _atIndex, uShiftSize * sizeof(T));

    // Replace data at wished position
    T	*pT = m_data + _atIndex;

    Constructor(pT);
    *pT = _data; // Calls operator= 
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::replace(T const &_data,u32 _atIndex)
{
    T	*pT = m_data + _atIndex;
    Constructor(pT);
    *pT = _data;                       // Calls operator= 
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline const FastArray<T,I,ALIGN,ASSIGNOP> &FastArray<T,I,ALIGN,ASSIGNOP>::operator =(const FastArray &_src)
{
    resize(_src->m_elemCount);

    if(ASSIGNOP)
    {
        const ux uSize = _src->m_elemCount;
        const T *pSrc = _src->m_data;
        for(ux i = 0 ; i < uSize ; ++i)
        {
            m_data[i] = pSrc[i]; // Calls operator= 
        }
    }
    else
    {
        ITF_Memcpy(m_data, _src->m_data, sizeof(T) * _src->m_elemCount);
    }

#ifndef ITF_FINAL
	m_allocID = _src.m_allocID;
#endif // ITF_FINAL

	return *this;
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline bool FastArray<T,I,ALIGN,ASSIGNOP>::findRemove(T const &_value)
{
    if(m_elemCount)
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        T	*pCurrent = m_data;
        T	*pLast = m_data + (m_elemCount - 1);
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        while(pCurrent <= pLast)
        {
            if((*pCurrent) == _value)
            {
                if(pCurrent < pLast) *pCurrent = *pLast; // Calls operator= 
                resize(m_elemCount-1);
                return btrue;
            }

            pCurrent++;
        }
    }

    return bfalse;
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline int FastArray<T,I,ALIGN,ASSIGNOP>::find(T const &_value) const
{
    T	*pCurrent = m_data;
    T	*pLast = m_data + m_elemCount;

    while(pCurrent < pLast)
    {
        if((*pCurrent) == _value)
            return (int)(pCurrent - m_data);
        pCurrent++;
    }

    return -1;
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::removeAtOrdered(int _pos)
{
	ITF_ASSERT(_pos < m_elemCount);
    
    Destructor(m_data + _pos);

    memmove(m_data + _pos, m_data + _pos + 1, (m_elemCount - (_pos + 1)) * sizeof(T));

	m_elemCount = m_elemCount-1;    
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::removeAtOrdered(int _pos, int _count)
{
	ux endPos = _pos + _count;

	ITF_ASSERT(endPos <= m_elemCount);

	for(ux i = _pos; i < endPos; ++i)
		Destructor(m_data + i);

	memmove(m_data + _pos, m_data + endPos, (m_elemCount - endPos) * sizeof(T));
	m_elemCount -= _count;
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::removeAt(int _pos)
{
    ITF_ASSERT(_pos < m_elemCount);

    Destructor(m_data + _pos);

    if(_pos != m_elemCount - 1)
    {
        T	*pDest = m_data + _pos;
        T	*pSrc = m_data + (m_elemCount - 1);

        *pDest = *pSrc;                       // Calls operator= 
    }

    m_elemCount = m_elemCount-1;
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::shiftAt(int _pos, int _shift)
{
    /*~~~~~~~~~~~~~~~*/
    u32 uSize = size();
    /*~~~~~~~~~~~~~~~*/

    ITF_ASSERT(_pos <= uSize);
    resize(uSize + _shift);

    // Copy the existing elements
    memmove(m_data + _shift + _pos, m_data + _pos, (uSize - _pos) * sizeof(T));

    // Call the default constructor on the shifted elements.
    ArrayConstructor(m_data,_pos,_pos+_shift);
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::swap(int _pos1, int _pos2)
{
    /*~~~~~~~~~*/
    T	*p1, *p2;
    T	tmp;
    /*~~~~~~~~~*/

    p1 = m_data + _pos1;
    p2 = m_data + _pos2;

    tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::swap( FastArray &other)
{
	ITF::Swap(m_data, other.m_data);
	ITF::Swap(m_elemCount, other.m_elemCount);
	ITF::Swap(m_capacity, other.m_capacity);

#ifndef ITF_FINAL
	ITF::Swap(m_allocID, other.m_allocID);
#endif // ITF_FINAL
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::assign(u32 _num, T const &_value)
{
    clear();
    for(u32 i = 0 ; i < _num ; ++i)
    {
        push_back(_value);
    }
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::setData(T *_data, u32 _size)
{
	ITF_ASSERT((m_data == NULL) && (m_capacity == 0));
	m_data = _data;
	m_capacity = _size;
	m_elemCount = _size;
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::serialize(ArchiveMemory& _archive)
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
	for (iterator iter = this->begin();iter != this->end();++iter)
	{
		T&  val = (*iter);
		serializeValue(val,_archive);
	}
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::IncreaseMaxSizeIfFull()
{
    // Reallocate the array if it is full
    if (m_elemCount == m_capacity)
    {
        ITF_ASSERT_STATIC(TOO_ResizeMul > TOO_ResizeDiv, "TOO_ResizeMul should be bigger than TOO_ResizeDiv");

        // Resize is done so that by default we have : 4, 8, 16, 30, 49, ... 
        // For big reallocates, there is a 50% increase (TOO_ResizeMul / TOO_ResizeDiv).
        // Thus, the buffer grows fast at first to avoid small , and then growth slows down.
        I uNewMaxSize = TOO_ResizeConst + ((m_capacity * TOO_ResizeMul) / TOO_ResizeDiv);

        // Resize should not be bigger than * 2 (except for first time)
        if (uNewMaxSize > m_capacity * 2)
            uNewMaxSize = m_capacity * 2;

        if (uNewMaxSize < TOO_ResizeMin)
            uNewMaxSize = TOO_ResizeMin;

        reserve(uNewMaxSize);
    }
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::PrivateIncreaseSize(u32 _u_Size)
{
    ITF_ASSERT(_u_Size > m_elemCount);

    ArrayConstructor(m_data,m_elemCount,_u_Size);

    m_elemCount = static_cast<I>(_u_Size);
}

template<class T, typename I, u32 ALIGN, bool ASSIGNOP>
inline void FastArray<T,I,ALIGN,ASSIGNOP>::PrivateDecreaseSize(u32 _u_Size)
{
    ITF_ASSERT(_u_Size < m_elemCount);

    ArrayDestructor(m_data,_u_Size,m_elemCount);

    m_elemCount = static_cast<I>(_u_Size);
}

} // namespace ITF

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // _ITF_FASTARRAY_H_ 
