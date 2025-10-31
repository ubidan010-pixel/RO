#ifndef ITF_LIST_H_
#define ITF_LIST_H_

#pragma once

#include "core/types.h"
#include <list>
#include "core/memory/STDAllocatorOnITFMemory.h"

namespace ITF
{

    template <typename T>
    using AllocList = STDAllocatorOnITFMemory<T, MemoryId::mId_List>;

    template <class T>
    class List : public STD_LIST<T,AllocList<T> >
     //class Vector : public std::vector<T>
    {
    public:

       typedef typename STD_LIST_QUALIFIED<T,AllocList<T> > base_container;

       //typedef typename std::vector<T> base_container;

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
    };

} // namespace ITF

template <typename T>
using ITF_LIST = ITF::List<T>;

#endif /* ITF_LIST_H_ */

