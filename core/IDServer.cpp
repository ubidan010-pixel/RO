#include "precompiled_core.h"

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

namespace ITF
{
 // debug purpose, used to log objects referencement count
    #ifdef ASSERT_ENABLED
        u32 IdServer::m_registeredCount = 0;
    #endif  //ASSERT_ENABLED


IdServer::IdServer()
{
    Synchronize::createCriticalSection(&m_csThreadId);

    ITF_ASSERT_CRASH(m_maxThread >= eThreadId_count, "bad thread count");
    ITF_ASSERT_CRASH(1<<m_ThreadIndexBits >= m_maxThread, "bad thread count");

    for (u32 threadIdx = 0; threadIdx < eThreadId_count; threadIdx++)
    {
        ITF_ASSERT(ThreadSettings::m_settings[threadIdx].m_engineIndex == threadIdx);
        const u32 elemCount = ThreadSettings::m_settings[threadIdx].m_maxObjectRef;
        ITF_ASSERT(elemCount <= m_indexMask);
        ThreadStruct& str = m_ThreadData[threadIdx];

        Synchronize::createCriticalSection(&str.m_csIndexesToFree);        
        str.m_freeIndexes.reserve(elemCount);
        str.m_data.resize(elemCount);
        for (u32 elem = 0; elem < elemCount; elem++)
        {
            bbool isInvalidObjRef = (0 == threadIdx && 0 == elem);
            if (!isInvalidObjRef)
                str.m_freeIndexes.push_back(elem);  // don't flag Invalid Objref as useable.

            str.m_data[elem].m_object = NULL;
            str.m_data[elem].m_counter = 0;
        }
    }

    m_vReservedThreadId.resize(m_maxThread);
    m_vReservedThreadId.fillZero();

    m_sessionCounter = 0;

    ITF_ASSERT_CRASH(ObjectRef::InvalidRef.getValue() == ITF_INVALID_OBJREF, "Invalid objectref must have ITF_INVALID_OBJREF as internal value");
    ITF_ASSERT_CRASH(NULL == getObject(ITF_INVALID_OBJREF), "ITF_INVALID_OBJREF must fetch NULL object");
    ITF_ASSERT_CRASH(0 == getThreadIndexFromRef(ITF_INVALID_OBJREF), "ITF_INVALID_OBJREF must point to thread 0");
    ITF_ASSERT_CRASH(0 == getIndexFromRef(ITF_INVALID_OBJREF), "ITF_INVALID_OBJREF must point to index 0");
 }


IdServer::~IdServer()
{
    for (u32 threadIdx = 0; threadIdx < eThreadId_count; threadIdx++)
    {
        ThreadStruct& str = m_ThreadData[threadIdx];
        Synchronize::destroyCriticalSection(&str.m_csIndexesToFree);        
    }

    Synchronize::destroyCriticalSection(&m_csThreadId);
}

ObjectRef IdServer::registerObject(BaseObject* _object)
{
    const u32 threadEngineIndex = ThreadInfo::m_threadEngineIndex.getValue();

    ITF_ASSERT_CRASH(threadEngineIndex < eThreadId_count, "bad thread count");
    
#ifndef ITF_FINAL
    const ThreadSettings& settings = ThreadSettings::m_settings[threadEngineIndex];
    ITF_ASSERT_CRASH(settings.m_maxObjectRef > 0, "registering objet in a thread with NULL m_maxObjectRef");
#endif // FINAL
    
    ThreadStruct& str = m_ThreadData[threadEngineIndex];

    if (str.m_freeIndexes.size() == 0)
    {
        str.processIndexesToFree();
        if (str.m_freeIndexes.size() == 0)
        {
            csAutoLock cs(str.m_csIndexesToFree);
            LOG("out of objref on thread %d", threadEngineIndex);
            for (u32 threadIdx = 0; threadIdx < eThreadId_count; threadIdx++)
            {
                LOG("thread %d - total free = %d / %d - current free : %d", threadIdx, m_ThreadData[threadIdx].m_freeIndexes.size() + m_ThreadData[threadIdx].m_indexesToFree.size(), ThreadSettings::m_settings[threadIdx].m_maxObjectRef, m_ThreadData[threadIdx].m_freeIndexes.size());
            }
/*
            String8 errorMsg;
            errorMsg.setTextFormat("********* VERY IMPORTANT *********\n\n >>>>>>> Call an engineer <<<<<<<\n\nInternal buffer overflow\nThread Max objRef must be increaed\nThread:%d\nEngine stability is no more guaranteed from now\ngrowing IDServer tabled is NOT thread safe and should never occur", threadEngineIndex);
            ITF_ERROR(errorMsg.cStr());
            */
            ITF_ASSERT(bfalse);

            u32 addedCount = str.m_data.size() / 2;
            u32 originalSize = str.m_data.size();
            u32 newSize = originalSize + addedCount;
            
            if (newSize >= (1<<m_indexBits))
                newSize = 1<<m_indexBits;
            ITF_ASSERT_CRASH(newSize > originalSize, "MAX objects reached:%d (thread:%d)", newSize, threadEngineIndex);

            str.m_data.resize(newSize);
            u32 freeOriginalSize = str.m_freeIndexes.size();
            str.m_freeIndexes.resize(freeOriginalSize + addedCount);
            for (u32 i = 0; i < addedCount; i++)
            {
                str.m_freeIndexes[i + freeOriginalSize] = i + originalSize;
            }
        }
    }

       
    const u32 index = str.m_freeIndexes[str.m_freeIndexes.size()-1];
    str.m_freeIndexes.eraseNoOrder(str.m_freeIndexes.size()-1);

    str.m_data[index].m_object = _object;
    u8 counter = static_cast<u8>(str.m_data[index].m_counter);
    counter++;
    str.m_data[index].m_counter = counter;

    ObjectRef ret((str.m_data[index].m_counter << (m_indexBits + m_ThreadIndexBits)) | (index << m_ThreadIndexBits) | threadEngineIndex);
#ifdef GPP_DEBUG
    ret.m_pointer = _object;
#endif //GPP_DEBUG

    ITF_ASSERT(str.m_data.size() == str.m_data.capacity());


    //check myself,yes it's slow ,but we must have until we find the issue,keep it on any platform
#ifndef ITF_FINAL
    ITF_ASSERT_CRASH(_object == getObject(ret),"the getobject(pObj->getref()) doesn't match");
#endif //ITF_FINAL
    /*
    #ifdef ASSERT_ENABLED
    {
        m_registeredCount++;
        u32 threadIndex = threadEngineIndex;
        LOG("Register BaseObject %d", m_registeredCount);
    }
    #endif  //ASSERT_ENABLED
    */

    return  ret;
}

void IdServer::unregisterObject(ObjectRef _ref)
{
    ITF_ASSERT_CRASH(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "Cannot perform this operation from this Thread");
    const u32 threadID = getThreadIndexFromRef(_ref.getValue());
    ITF_ASSERT_CRASH(threadID < eThreadId_count, "Bad ObjectRef");
    ThreadStruct& str = m_ThreadData[threadID];

    const u32 index = getIndexFromRef(_ref.getValue());

   

    if (threadID == eThreadId_mainThread) 
    {
        str.m_data[index].m_object = NULL; 
        str.m_freeIndexes.push_back(index);
    }
    else
        str.setIndexToFree(index);

/*
    #ifdef ASSERT_ENABLED
    {
        ITF_ASSERT(m_registeredCount > 0);
        m_registeredCount--;
        LOG("Unregister BaseObject %d", m_registeredCount);
    }
    #endif  //ASSERT_ENABLED*/
}



u32  IdServer::getThreadServerId()
{
    csAutoLock cs(m_csThreadId);
    for (u32 index = 0;index<m_vReservedThreadId.size();++index)
    {
        if (m_vReservedThreadId[index] == bfalse)
        {
            m_vReservedThreadId[index] = btrue;
            return index;
        }
    }

    ITF_ASSERT(0);
    return U32_INVALID;
}

void  IdServer::unregisterThreadID(u32 _threadId)
{
    csAutoLock cs(m_csThreadId);
    m_vReservedThreadId[_threadId] = bfalse;
}


void IdServer::updateThreadsData()
{
    const u32 threadEngineIndex = ThreadInfo::m_threadEngineIndex.getValue();
    ThreadStruct& str = m_ThreadData[threadEngineIndex];
    str.processIndexesToFree();
}

void IdServer::ThreadStruct::setIndexToFree(u32 _index)
{
    csAutoLock cs(m_csIndexesToFree);
    m_data[_index].m_object = NULL; 
    m_indexesToFree.push_back(_index);
}

void IdServer::ThreadStruct::processIndexesToFree()
{
    csAutoLock cs(m_csIndexesToFree);
    for (u32 i = 0; i < m_indexesToFree.size(); i++)
    {
        m_freeIndexes.push_back(m_indexesToFree[i]);
    }
    m_indexesToFree.clear();
}

void IdServer::getObjectList(const SafeArray<ObjectRef> &_ref, SafeArray<BaseObject*> &_obj)
{   // BEWARE, any change in getObject() should be repercuted here
    u32 size = _ref.size();
    BaseObject** pObj = &_obj.incrSize(size);
    for (u32 i = 0; i < size; i++)    
    {
        const ObjectRef ref = _ref[i];
        BaseObject* ptr = NULL;
        const u32 threadID = getThreadIndexFromRef(ref.getValue());
        const u32 index = getIndexFromRef(ref.getValue()); 
        ThreadDataStruct& strData = m_ThreadData[threadID].m_data[index];
        if (getCounterFromRef(ref.getValue()) == strData.m_counter)
            ptr = strData.m_object;
        *pObj = ptr;
        pObj++;
    }
}

void IdServer::getObjectListV2(const SafeArray<ObjectRef> &_ref, SafeArray<BaseObject*> &_obj)
{   // BEWARE, any change in getObject() should be repercuted here
    i32 size = (i32)_ref.size();
    BaseObject** pObj = &_obj.incrSize(size);
    i32 i=0;
    for (; i < size-4; i+=4)
    {
        const u32 ref1 = _ref[i].getValue();
        const u32 ref2 = _ref[i+1].getValue();
        const u32 ref3 = _ref[i+2].getValue();
        const u32 ref4 = _ref[i+3].getValue();
        BaseObject* ptr1 = NULL;
        BaseObject* ptr2 = NULL;
        BaseObject* ptr3 = NULL;
        BaseObject* ptr4 = NULL;
        const u32 threadID1 = getThreadIndexFromRef(ref1);
        const u32 threadID2 = getThreadIndexFromRef(ref2);
        const u32 threadID3 = getThreadIndexFromRef(ref3);
        const u32 threadID4 = getThreadIndexFromRef(ref4);
        const u32 index1 = getIndexFromRef(ref1); 
        const u32 index2 = getIndexFromRef(ref2); 
        const u32 index3 = getIndexFromRef(ref3); 
        const u32 index4 = getIndexFromRef(ref4); 
        ThreadDataStruct& strData1 = m_ThreadData[threadID1].m_data[index1];
        ThreadDataStruct& strData2 = m_ThreadData[threadID2].m_data[index2];
        ThreadDataStruct& strData3 = m_ThreadData[threadID3].m_data[index3];
        ThreadDataStruct& strData4 = m_ThreadData[threadID4].m_data[index4];
        if (getCounterFromRef(ref1) == strData1.m_counter)
            ptr1 = strData1.m_object;
        if (getCounterFromRef(ref2) == strData2.m_counter)
            ptr2 = strData2.m_object;
        if (getCounterFromRef(ref3) == strData3.m_counter)
            ptr3 = strData3.m_object;
        if (getCounterFromRef(ref4) == strData4.m_counter)
            ptr4 = strData4.m_object;
        pObj[0] = ptr1;
        pObj[1] = ptr2;
        pObj[2] = ptr3;
        pObj[3] = ptr4;
        pObj+=4;
    }
    for (; i < size; i++)    
    {
        const u32 ref = _ref[i].getValue();
        BaseObject* ptr = NULL;
        const u32 threadID = getThreadIndexFromRef(ref);
        const u32 index = getIndexFromRef(ref); 
        ThreadDataStruct& strData = m_ThreadData[threadID].m_data[index];
        if (getCounterFromRef(ref) == strData.m_counter)
            ptr = strData.m_object;
        *pObj = ptr;
        pObj++;
    }
}

void IdServer::getObjectList(const BaseObjectList& _list, SafeArray<BaseObject*> *_pObj)
{   // BEWARE, any change in getObject() should be repercuted here
    if (!_pObj)
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        _pObj = &_list.m_solvedPointers;
        _pObj->clear();
    }
    SafeArray<BaseObject*> &_obj = *_pObj;

    for (u32 iThrd = 0; iThrd <  _list.m_threadIds.size(); iThrd++)
    {
        const u32 threadID = _list.m_threadIds[iThrd];
        const SafeArray<ObjectRef>& table = _list.m_refs[iThrd];
        const u32 refSize = table.size();
        BaseObject** pObj = &_obj.incrSize(refSize);
        ThreadStruct& strSData = m_ThreadData[threadID];
        for (u32 iRef = 0; iRef < refSize; iRef++)
        {
            const u32 ref = table[iRef].getValue();
            BaseObject* ptr = NULL;
            const u32 index = getIndexFromRef(ref); 
            ThreadDataStruct& strData = strSData.m_data[index];
            if (getCounterFromRef(ref) == strData.m_counter)
                ptr = strData.m_object;
            *pObj = ptr;
            pObj++;
        }
    }
}

void IdServer::getObjectListV2(const BaseObjectList& _list, SafeArray<BaseObject*> *_pObj)
{   // BEWARE, any change in getObject() should be repercuted here
    if (!_pObj)
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        _pObj = &_list.m_solvedPointers;
        _pObj->clear();
    }
    SafeArray<BaseObject*> &_obj = *_pObj;

    for (u32 iThrd = 0; iThrd < _list.m_threadIds.size(); iThrd++)
    {
        const u32 threadID = _list.m_threadIds[iThrd];
        const SafeArray<ObjectRef>& table = _list.m_refs[iThrd];
        const i32 refSize = table.size();
        BaseObject** pObj = &_obj.incrSize(refSize);
        ThreadStruct& strSData = m_ThreadData[threadID];
        i32 iRef = 0; 
        for (; iRef < refSize-4; iRef+=4)
        {
            const u32 ref1 = table[iRef].getValue();
            const u32 ref2 = table[iRef+1].getValue();
            const u32 ref3 = table[iRef+2].getValue();
            const u32 ref4 = table[iRef+3].getValue();
            BaseObject* ptr1 = NULL;
            BaseObject* ptr2 = NULL;
            BaseObject* ptr3 = NULL;
            BaseObject* ptr4 = NULL;
            const u32 index1 = getIndexFromRef(ref1); 
            const u32 index2 = getIndexFromRef(ref2); 
            const u32 index3 = getIndexFromRef(ref3); 
            const u32 index4 = getIndexFromRef(ref4); 
            ThreadDataStruct& strData1 = strSData.m_data[index1];
            ThreadDataStruct& strData2 = strSData.m_data[index2];
            ThreadDataStruct& strData3 = strSData.m_data[index3];
            ThreadDataStruct& strData4 = strSData.m_data[index4];
            if (getCounterFromRef(ref1) == strData1.m_counter)
                ptr1 = strData1.m_object;
            if (getCounterFromRef(ref2) == strData2.m_counter)
                ptr2 = strData2.m_object;
            if (getCounterFromRef(ref3) == strData3.m_counter)
                ptr3 = strData3.m_object;
            if (getCounterFromRef(ref4) == strData4.m_counter)
                ptr4 = strData4.m_object;
            *pObj++ = ptr1;
            *pObj++ = ptr2;
            *pObj++ = ptr3;
            *pObj++ = ptr4;
        }
        for (; iRef < refSize; iRef++)
        {
            const u32 ref1 = table[iRef].getValue();
            BaseObject* ptr1 = NULL;
            const u32 index1 = getIndexFromRef(ref1); 
            ThreadDataStruct& strData1 = strSData.m_data[index1];
            if (getCounterFromRef(ref1) == strData1.m_counter)
                ptr1 = strData1.m_object;
            *pObj++ = ptr1;
        }
    }
}

void IdServer::getObjectListNoNull(const SafeArray<ObjectRef> &_ref, SafeArray<BaseObject*> &_obj )
{   // BEWARE, any change in getObject() should be repercuted here
    u32 size = _ref.size();
    if (_obj.capacity() < _obj.size() + _ref.size())
        _obj.reserve(_obj.size() + _ref.size());

    for (u32 i = 0; i < size; i++)    
    {
        const u32 ref = _ref[i].getValue();
        const u32 threadID = getThreadIndexFromRef(ref);
        const u32 index = getIndexFromRef(ref); 
        ThreadDataStruct& strData = m_ThreadData[threadID].m_data[index];
        if (getCounterFromRef(ref) == strData.m_counter && strData.m_object)
            _obj.push_back(strData.m_object);
    }
}

void IdServer::getObjectListNoNull(const BaseObjectList& _list, SafeArray<BaseObject*> *_pObj)
{   // BEWARE, any change in getObject() should be repercuted here
    if (!_pObj)
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        _pObj = &_list.m_solvedPointers;
        _pObj->clear();
    }
    SafeArray<BaseObject*> &_obj = *_pObj;

    for (u32 iThrd = 0; iThrd <  _list.m_threadIds.size(); iThrd++)
    {
        const u32 threadID = _list.m_threadIds[iThrd];
        const SafeArray<ObjectRef>& table = _list.m_refs[iThrd];
        const u32 refSize = table.size();
        if (_obj.capacity() < _obj.size() + table.size())
            _obj.reserve(_obj.size() + table.size());
        ThreadStruct& strSData = m_ThreadData[threadID];
        for (u32 iRef = 0; iRef < refSize; iRef++)
        {
            const u32 ref = table[iRef].getValue();
            const u32 index = getIndexFromRef(ref); 
            ThreadDataStruct& strData = strSData.m_data[index];
            if (getCounterFromRef(ref) == strData.m_counter && strData.m_object)
                _obj.push_back(strData.m_object);
        }
    }
}

void IdServer::getObjectListNoNullV2(const BaseObjectList& _list, SafeArray<BaseObject*> *_pObj)
{   // BEWARE, any change in getObject() should be repercuted here
    if (!_pObj)
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        _pObj = &_list.m_solvedPointers;
        _pObj->clear();
    }
    SafeArray<BaseObject*> &_obj = *_pObj;

    for (u32 iThrd = 0; iThrd <  _list.m_threadIds.size(); iThrd++)
    {
        const u32 threadID = _list.m_threadIds[iThrd];
        const SafeArray<ObjectRef>& table = _list.m_refs[iThrd];
        const i32 refSize = table.size();
        if (_obj.capacity() < _obj.size() + table.size())
            _obj.reserve(_obj.size() + table.size());
        ThreadStruct& strSData = m_ThreadData[threadID];
        i32 iRef = 0;
        for (; iRef < refSize-4; iRef+=4)
        {
            const u32 ref1 = table[iRef].getValue();
            const u32 ref2 = table[iRef+1].getValue();
            const u32 ref3 = table[iRef+2].getValue();
            const u32 ref4 = table[iRef+3].getValue();
            const u32 index1 = getIndexFromRef(ref1); 
            const u32 index2 = getIndexFromRef(ref2); 
            const u32 index3 = getIndexFromRef(ref3); 
            const u32 index4 = getIndexFromRef(ref4); 
            ThreadDataStruct& strData1 = strSData.m_data[index1];
            ThreadDataStruct& strData2 = strSData.m_data[index2];
            ThreadDataStruct& strData3 = strSData.m_data[index3];
            ThreadDataStruct& strData4 = strSData.m_data[index4];
            if (getCounterFromRef(ref1) == strData1.m_counter && strData1.m_object)
                _obj.push_back(strData1.m_object);
            if (getCounterFromRef(ref2) == strData2.m_counter && strData2.m_object)
                _obj.push_back(strData2.m_object);
            if (getCounterFromRef(ref3) == strData3.m_counter && strData3.m_object)
                _obj.push_back(strData3.m_object);
            if (getCounterFromRef(ref4) == strData4.m_counter && strData4.m_object)
                _obj.push_back(strData4.m_object);
        }
        for (; iRef < refSize; iRef++)
        {
            const u32 ref = table[iRef].getValue();
            const u32 index = getIndexFromRef(ref); 
            ThreadDataStruct& strData = strSData.m_data[index];
            if (getCounterFromRef(ref) == strData.m_counter && strData.m_object)
                _obj.push_back(strData.m_object);
        }
    }
}

BaseObjectList::BaseObjectList()
{
}

void    BaseObjectList::addObject(ObjectRef _ref, bbool _allowDoublons)
{
    const u32 threadID = IdServer::getThreadIndexFromRef(_ref.getValue());
    i32 index = m_threadIds.find(threadID);
    if (index < 0)
    {
        index = m_threadIds.size();
        m_threadIds.push_back(threadID);
        ITF_ASSERT_CRASH(m_threadIds.size() < m_maxThread, "increase BaseObjectList::m_maxThread");
    }

    if (!_allowDoublons)
    {
        if (m_refs[index].find(_ref) >= 0)
            return;
    }
    m_refs[index].push_back(_ref);
}

bbool BaseObjectList::hasObject(ObjectRef _ref) const
{
    const u32 threadID = IdServer::getThreadIndexFromRef(_ref.getValue());
    i32 index = m_threadIds.find(threadID);
    if (index < 0)
        return bfalse;
    if (m_refs[index].find(_ref) >= 0)
            return btrue;
    return bfalse;
}

void    BaseObjectList::clear()
{
    m_threadIds.clear();
    for (int i=0; i<m_maxThread; i++)
        m_refs[i].clear();
}

void    BaseObjectList::mergeArray(const ObjectRefList& _obj)
{
    u32 size = _obj.size();
    for (u32 i = 0; i < size; i++)
        addObject(_obj[i], btrue);
}

void    BaseObjectList::mergeArray(const BaseObjectList& _list)
{
    for (u32 iThrd = 0; iThrd <  _list.m_threadIds.size(); iThrd++)
    {
        const u32 threadID = _list.m_threadIds[iThrd];
        const SafeArray<ObjectRef>& table = _list.m_refs[iThrd];
        if (table.size())
        {
            bbool found = bfalse;
            for (u32 iThrd2 = 0; iThrd2 <  m_threadIds.size(); iThrd2++)
            {
                const u32 threadID2 = m_threadIds[iThrd2];
                if (threadID2 == threadID)
                {
                    m_refs[iThrd2].mergeArray(table);
                    found = btrue;
                    break;
                }
            }
            if (!found)
            {
                u32 index = m_threadIds.size();
                m_threadIds.push_back(threadID);
                m_refs[index].mergeArray(table);
            }
        }
    }
}

void    BaseObjectList::removeObject(ObjectRef _ref)
{
    const u32 threadID = IdServer::getThreadIndexFromRef(_ref.getValue());
    i32 index = m_threadIds.find(threadID);
    if (index < 0)
        return;
    i32 index2 = m_refs[index].find(_ref);
    if (index2 < 0)
        return;
    m_refs[index].eraseNoOrder(index2);
}

} // namespace ITF
