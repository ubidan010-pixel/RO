#ifndef __MEMALIGNED_H__
#define __MEMALIGNED_H__

#include "adapters/Adapter_WWISE/AudioSDK/memory.h"

#if (defined ITF_WII) || (defined ITF_CTR)
#define ALIGNOF(a) 4		
#else 
#define ALIGNOF(a) 	__alignof(a)
#endif 

namespace AudioSDK
{

    template <typename T>
    class MemAligned
    {

    public:
        typedef T AlignedType;

        // Placement new operators (they do not allocate memory, just call the constructor)
        void *operator new(size_t /*_ui_Size*/, void *_p_Data) {return _p_Data;}
        void *operator new[](size_t /*_ui_Size*/, void *_p_Data) {return _p_Data;}

        // Normal new
        void *operator new(size_t _ui_Size )    {return allocate(_ui_Size );}
        void *operator new[](size_t _ui_Size )  {return allocate(_ui_Size );}

        // MemAligned new
        void *operator new(size_t _ui_Size, unsigned int _uAlign )   {return allocateAlign(_ui_Size , _uAlign );}
        void *operator new[](size_t _ui_Size, unsigned int _uAlign ) {return allocateAlign(_ui_Size , _uAlign );}

        // Delete operators (same in debug or release)
        void operator delete(void *p)   { ITF::Memory::alignedFree(p ); }
        void operator delete[](void *p) { ITF::Memory::alignedFree(p ); }

#if (defined ITF_WINDOWS) || (defined ITF_X360)
        // These "delete" are used only if an exception occurs. 
        void operator delete(void * /*_p_DataToDelete*/, void * /*_p_Data*/)    {}
        void operator delete[](void * /*_p_DataToDelete*/, void * /*_p_Data*/)    {}
        void operator delete(void * /*_p_DataToDelete*/ , unsigned int _uAlign )    {}
        void operator delete[](void * /*_p_DataToDelete*/ , unsigned int _uAlign )    {}
#endif 



    private:
        // take account type alignment in allocation
        static inline void* allocate(size_t _size )
        {
            ITF_ASSERT_CRASH( (ALIGNOF(T) > 0), "Error on Alignment" );
            return ITF::Memory::alignedMalloc( _size, (size_t)ALIGNOF(T) );
        }

        static inline void* allocateAlign(size_t _ui_Size, unsigned int _uAlign )
        {
            return ITF::Memory::alignedMalloc( _size, (size_t)_uAlign );
        }
    };

} // namespace 


#endif // __MEMALIGNED_H__
