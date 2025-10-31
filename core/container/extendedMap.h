#ifndef ITF_MAP_H_
#define ITF_MAP_H_

#pragma once

#include "core/types.h"
#include <map>

#include "core/memory/STDAllocatorOnITFMemory.h"

namespace ITF
{

    template <typename T>
    using MapAllocator = STDAllocatorOnITFMemory<T, MemoryId::mId_Map>;

	template <class TKey,class TValue>
    class Map : public STD_MAP<TKey, TValue, STD_LESS<TKey>, MapAllocator<STD_PAIR<const TKey,TValue>> >
    {
    public:
        typedef typename STD_MAP_QUALIFIED<TKey, TValue,STD_LESS<TKey>, MapAllocator<STD_PAIR<const TKey,TValue> > > base_container;

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

        using base_container::base_container;

        void serialize(ArchiveMemory& _archive)
        {   
            if (_archive.isReading())
            {
                u32 _size = 0;
                _archive.serialize(_size);

                this->clear();
                
                TKey    key;
                TValue  keyvalue;

                for (u32 index = 0;index<_size;++index)
                {                    
                    serializeValue(key,_archive);
                    serializeValue(keyvalue,_archive);

                    (*this)[key] = keyvalue;
                }
                
            }
            else
            {
                u32 _size = this->size();
                _archive.serialize(_size);

                for (iterator iter = this->begin();iter!=this->end();++iter)
                {
                    TKey     key         = (*iter).first;
                    TValue   keyvalue    = (*iter).second;
                  
                    serializeValue(key,_archive);
                    serializeValue(keyvalue,_archive);
                }
            }
        }

        // Force to return a 32 bit as there is too much case where the size is assuming it.
        // Assuming no size go above 4GiB is acceptable.
        [[nodiscard]] uSize size() const noexcept
        {
            return static_cast<uSize>(base_container::size());
        }
    };

} // namespace ITF

template <typename TKey, typename TValue>
using ITF_MAP = ITF::Map<TKey, TValue>;

#endif    /* ITF_MAP_H_ */

