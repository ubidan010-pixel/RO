#ifndef    ITF_MEMORYTRACKINGVECTOR_H_
#define    ITF_MEMORYTRACKINGVECTOR_H_

#include "core/types.h"

#include <vector>
#include "core/memory/STDAllocatorOnMalloc.h"

namespace ITF
{
    template <class T>
    class memoryTrackingVector : public std::vector<T, STDAllocatorOnMalloc<T> >
    {
    public:

       typedef typename std::vector<T, STDAllocatorOnMalloc<T> > base_container;


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

    };

} // namespace ITF

#endif    /* ITF_MEMORYTRACKINGVECTOR_H_ */

