#ifndef _ITF_MEMORYPOOLSIZED_H_
#define _ITF_MEMORYPOOLSIZED_H_

#if !defined(ITF_IPAD) && !defined(ITF_CTR) &&  !defined(ITF_PS3) && !defined(ITF_WII)

namespace ITF
{

class memoryPoolSized
{

public:

    memoryPoolSized(u32 _poolID):m_poolID(_poolID) {};
    void deallocate(void* ptr);

    ITF_INLINE const u32 getPoolID() {return m_poolID;}


protected:

    u32     m_poolID;
};

template<class T> void free(T* _ptr, memoryPoolSized& _pool)
{
    if (_ptr) 
    {
        _ptr->~T();		// explicit destructor call
        _pool.deallocate(_ptr);
    }
}

} // namespace ITF


void* operator new(size_t,ITF::memoryPoolSized& _pool);

#endif //ITF_IPAD

#endif // _ITF_MEMORYPOOLSIZED_H_
 