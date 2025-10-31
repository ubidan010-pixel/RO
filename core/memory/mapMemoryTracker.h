#ifndef _ITF_MAPMEMORYTRACKER_H_
#define _ITF_MAPMEMORYTRACKER_H_

#include "core/types.h"
#include <map>

#include "core/memory/STDAllocatorOnMalloc.h"

namespace ITF
{

    template <class Key, class Tp, class Compare = STD_LESS<Key> >
    class mapMemoryTracker : public STD_MAP<Key, Tp, Compare, STDAllocatorOnMalloc< STD_PAIR<const Key, Tp > > >
    {
        typedef typename STD_MAP_QUALIFIED<Key, Tp, Compare, STDAllocatorOnMalloc< STD_PAIR_QUALIFIED<const Key, Tp > > > base_container;

    public:
        typedef typename base_container::key_type				key_type;
        typedef typename base_container::mapped_type			mapped_type;
        typedef typename base_container::value_type				value_type;
        typedef typename base_container::key_compare			key_compare;
        typedef typename base_container::value_compare			value_compare;
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

        mapMemoryTracker()   {}

        explicit mapMemoryTracker(const Compare& _comp) : base_container(_comp)   {}

        template <class T>
        mapMemoryTracker(T first, T second)
            : base_container(first, second)                             {}

        template <class T>
        mapMemoryTracker(T first, T second, const Compare& _comp)
            : base_container(first, second, _comp)                     {}

        mapMemoryTracker(const mapMemoryTracker& _x)
            : base_container(_x)                                       {}

        mapMemoryTracker& operator=(const mapMemoryTracker& _x)
        {
            base_container::operator=(_x);
            return *this;
        }
    };
}
#endif //!_ITF_MAPMEMORYTRACKER_H_
