#ifndef _ITF_ID_SERVER_H_
#define _ITF_ID_SERVER_H_
#endif  //_ITF_ID_SERVER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_CORETYPES_H_   
#include "core/types.h"
#endif //_ITF_CORETYPES_H_   

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#include "core/system/Synchronize.h"

namespace ITF
{

class BaseObject;

class BaseObjectList
{
    public:
                                BaseObjectList();
        void                    addObject(ObjectRef _ref, bbool _allowDoublons);
        void                    removeObject(ObjectRef _ref);
        void                    clear();
        void                    mergeArray(const ObjectRefList& _obj);
        void                    mergeArray(const BaseObjectList& _list);
        bbool                   hasObject(ObjectRef _ref) const;


        static const int        m_maxThread = 8;
        SafeArray<ObjectRef>    m_refs[m_maxThread];
        SafeArray<u32>          m_threadIds;
        mutable SafeArray<BaseObject*>  m_solvedPointers;
};

/**
*
* ObjectRefs act as smart pointers. Every BaseObject has an objectref. It’s 
* recommended to use ObjectRef rather than pointer to reference an object, 
* because you are sure you will detect if the object has been deleted 
* (avoids dangling pointers).
* How it works?
* Basically, we have a big table of pointers to BaseObjects. An objectRef is 
* the index of an object inside this table. Well actually we have several
* tables (one per thread) and the ObjectRef fetures an additional security. An
* ObjectRef is a 32 bit value decomposed as follows:
* Bits  Value       Remarks
* 3	    Thread ID	Indicates which thread created the object. We have one 
*                   object table per thread to avoid locks.
* 21	Index	    Index of the object inside its table
* 8	    Counter	    Every table entry has a counter. This counter is 
*                   incremented everytime a table entry is re-used (assigned a
*                   new object). This avoids collisions between the ObjectRef 
*                   of the object that previously belonged to that table’s
*                   index and the new object
*
* You can get a pointer to a BaseObject from its ObjectRef using the getObject()
* method (or deprecated macro GETOBJECT).
*
* Traps:
*   - Don’t serialize an ObjectRef on disk. They are valid only during a session
*   - A valid ObjectRef doesn’t mean the object is not NULL
*   - Test an ObjectRef against ITF_INVALID_OBJREF only if you want to know if 
*     the variable has been assigned a value or not
*   - Remember that there is no way to predict if getObject() is going to 
*     return NULL or not.
*/

class IdServer:public TemplateSingleton<IdServer>
{
private:
    #ifdef ASSERT_ENABLED
        static u32 m_registeredCount;
    #endif  //ASSERT_ENABLED

    struct ThreadDataStruct
    {
        u32         m_counter;                  // actually, only m_counterBits bits are used, but for 32 padding it's better this way
        BaseObject* m_object;
    };

    struct ThreadStruct
    {
        SafeArray<u32,1,MemoryId::mId_IDServer>                 m_freeIndexes;    // lists all the free indexes in m_data
        SafeArray<ThreadDataStruct, 1,MemoryId::mId_IDServer>   m_data;
        SafeArray<u32, 16,MemoryId::mId_IDServer>               m_indexesToFree;
        ITF_THREAD_CRITICAL_SECTION                             m_csIndexesToFree;
        void    setIndexToFree(u32 _index);             
        void    processIndexesToFree();
    };

    static const u32        m_maxThread        =    16;                       // bits reserved for counter
    static const u32        m_counterBits      =    8;                        // bits reserved for counter
    static const u32        m_ThreadIndexBits  =    4;
    static const u32        m_indexBits        =    32 - m_counterBits - m_ThreadIndexBits;        // bits reserved for index
    static const u32        m_indexMask        =    (1 << m_indexBits) - 1;    // mask to isolate index from ObjectRef
    static const u32        m_ThreadMask       =    (1 << m_ThreadIndexBits) - 1;

    SafeArray<bbool,m_maxThread>   m_vReservedThreadId;           

    ThreadStruct            m_ThreadData[m_maxThread];
    u32                     m_sessionCounter;
    ITF_THREAD_CRITICAL_SECTION             m_csThreadId;//CS to protect thread id




public: 
    static ITF_INLINE u8     getCounterFromRef  ( u32 _ref ) {return _ref >> (m_indexBits + m_ThreadIndexBits);}
    static ITF_INLINE u32    getIndexFromRef    ( u32 _ref ) {return (_ref>>m_ThreadIndexBits) & m_indexMask;}
    static ITF_INLINE u8     getThreadIndexFromRef  ( u32 _ref ) {return _ref & m_ThreadMask;}


    //////////////////////////////////////////////////////////////////////////
    ///
    ///
                        IdServer            ();

    virtual ~IdServer();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// registers an object into the table
    /// @param    _object    the object to be registered
    /// @return the object's reference
    ///
    ObjectRef            registerObject        ( BaseObject* _object );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// unregisters an object from the table
    /// @param    _ref the object's reference
    ///
    void                unregisterObject    ( ObjectRef _ref );


    //////////////////////////////////////////////////////////////////////////
    ///
    /// computes the pointers corresponding to a list of ObjectRef
    /// @param    _ref the list of ObjectRef
    /// @param    _obj the destination list of pointers
    ///
    void                getObjectList       (const SafeArray<ObjectRef> &_ref, SafeArray<BaseObject*> &_obj);
    void                getObjectList       (const BaseObjectList& _list, SafeArray<BaseObject*> *_obj = NULL);
    void                getObjectListV2     (const BaseObjectList& _list, SafeArray<BaseObject*> *_obj = NULL);
    void                getObjectListV2     (const SafeArray<ObjectRef> &_ref, SafeArray<BaseObject*> &_obj);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// computes the pointers corresponding to a list of ObjectRef, but without
    /// storing NULL pointers in destination list
    /// @param    _ref the list of ObjectRef
    /// @param    _obj the destination list of pointers
    ///
    template <typename containerType>
    void                getObjectListNoNull (const SafeArray<ObjectRef>& _ref, containerType &_obj);
    void                getObjectListNoNull (const SafeArray<ObjectRef> &_ref, SafeArray<BaseObject*> &_obj);
    void                getObjectListNoNull (const BaseObjectList& _list, SafeArray<BaseObject*> *_pObj  = NULL);
    void                getObjectListNoNullV2 (const BaseObjectList& _list, SafeArray<BaseObject*> *_pObj  = NULL);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// returns an object given its reference
    /// @param    _ref the object's reference
    /// @return the object's pointer (or NULL)
    ///
    ITF_INLINE BaseObject*    getObject         ( const ObjectRef &_ref )
    {   // BEWARE, any change in getObject() should be repercuted in getObjectList() and getObjectListNoNull()
        const u32 threadID = getThreadIndexFromRef(_ref.getValue());

        const u32 index = getIndexFromRef(_ref.getValue()); 
        ThreadDataStruct& strData = m_ThreadData[threadID].m_data[index];

        if (getCounterFromRef(_ref.getValue()) != strData.m_counter)
            return NULL;

        return strData.m_object;
    }

    void updateThreadsData();

    u32  getThreadServerId();

    void  unregisterThreadID(u32 _threadId);



    //NOT THREAD DEBUG ONLY
    u32  getThreadRefFreeSize(u32 _threadId)        const {return  m_ThreadData[_threadId].m_freeIndexes.size();}
    u32  getThreadRefFreeCapacity(u32 _threadId)    const {return  m_ThreadData[_threadId].m_freeIndexes.capacity();}
};

#define ID_SERVER           IdServer::getptr()
#define GETOBJECT(_expr)    ID_SERVER->getObject(_expr)

template < typename containerType >
void IdServer::getObjectListNoNull (const SafeArray<ObjectRef>& _ref, containerType &_obj)
{   // BEWARE, any change in getObject() should be repercuted here
    u32 size = _ref.size();
    for (u32 i = 0; i < size; i++)    
    {
        u32 ref = _ref[i].getValue();
        const u32 threadID = getThreadIndexFromRef(ref);
        const u32 index = getIndexFromRef(ref); 
        ThreadDataStruct& strData = m_ThreadData[threadID].m_data[index];
        if (getCounterFromRef(ref) == strData.m_counter && strData.m_object)
            _obj.push_back(strData.m_object);
    }
}


} // namespace ITF

