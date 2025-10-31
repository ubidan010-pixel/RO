#ifndef _ITF_ALIGNED_H_
#define _ITF_ALIGNED_H_

#ifdef ENABLE_ALLOCATOR

#ifndef _ITF_ALLOCATOR_MACROS_H_
#include "core/memory/allocator/allocator_macros.h"
#endif //_ITF_ALLOCATOR_MACROS_H_


#if (defined ITF_WII) || (defined ITF_CTR)
#define ITF_alignof(a) 4		
#else 
#define ITF_alignof(a) 	__alignof(a)
#endif 

namespace ITF
{

template <typename T>
class Aligned
{
#ifndef _SPU
public:
	typedef T AlignedType;

    // Placement new operators (they do not allocate memory, just call the constructor)
    void *operator new(size_t /*_ui_Size*/, void *_p_Data) {return _p_Data;}
    void *operator new[](size_t /*_ui_Size*/, void *_p_Data) {return _p_Data;}

    // Normal new
    void *operator new(size_t _ui_Size ITF_MEM_NewArgsTypes) {return allocate(_ui_Size ITF_MEM_NewArgsPriv);}
    void *operator new[](size_t _ui_Size ITF_MEM_NewArgsTypes) {return allocate(_ui_Size ITF_MEM_NewArgsPriv);}

    // Aligned new
    void *operator new(size_t _ui_Size, u32 _uAlign ITF_MEM_NewArgsTypes) {return allocateAlign(_ui_Size , _uAlign ITF_MEM_NewArgsPriv);}
    void *operator new[](size_t _ui_Size, u32 _uAlign ITF_MEM_NewArgsTypes) {return allocateAlign(_ui_Size , _uAlign ITF_MEM_NewArgsPriv);}

    // Delete operators (same in debug or release)
    void operator delete(void *p) {ITF_MEM_Free(p ITF_MEM_DeleteArgs);}
    void operator delete[](void *p) {ITF_MEM_Free(p ITF_MEM_DeleteArgs);}

#if (defined ITF_WINDOWS) || (defined ITF_X360)
    // These "delete" are used only if an exception occurs. 
    void operator delete(void * /*_p_DataToDelete*/, void * /*_p_Data*/)    {}
    void operator delete[](void * /*_p_DataToDelete*/, void * /*_p_Data*/)    {}
    void operator delete(void * /*_p_DataToDelete*/ , u32 _uAlign ITF_MEM_NewArgsTypes)    {}
    void operator delete[](void * /*_p_DataToDelete*/ , u32 _uAlign ITF_MEM_NewArgsTypes)    {}
#ifdef ITF_DEBUG_ALLOC_
    void operator delete(void * /*_p_DataToDelete*/ ITF_MEM_NewArgsTypes)    {}
    void operator delete[](void * /*_p_DataToDelete*/ ITF_MEM_NewArgsTypes)    {}
#endif //ITF_DEBUG_ALLOC_
#endif 



private:
    // take account type alignment in allocation
    static void* allocate(size_t _ui_Size ITF_MEM_NewArgsTypesPriv)
    {
	    ITF_ASSERT_CRASH( (ITF_alignof(T) > 0), "Error on Alignment" );
        return ALLOCATOR()->p_AllocAlign(_ui_Size,ITF_alignof(T) ITF_MEM_NewArgs); 
    }

	static void* allocateAlign(size_t _ui_Size, u32 _uAlign ITF_MEM_NewArgsTypesPriv)
    {
        return ALLOCATOR()->p_AllocAlign(_ui_Size,_uAlign ITF_MEM_NewArgs); 
    }

#endif // _SPU
};

#define ITF_MEM_CheckAlignment() ITF_ASSERT( (reinterpret_cast<unsigned int>(this) % ITF_alignof(AlignedType)) == 0 )


} // namespace ITF
#endif //ENABLE_ALLOCATOR
#endif //_ITF_ALIGNED_H_
