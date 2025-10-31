#include "precompiled_engine.h"

#ifndef _ITF_OBJREFSTRESS_H_
#include "engine/unittest/ObjRefStress.h"
#endif //_ITF_OBJREFSTRESS_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_


namespace ITF
{

    volatile bbool allowed = bfalse;
    volatile bbool exited = bfalse;

    #define MAX_PICKABLE 1000
    FixedArray<BaseObject*,MAX_PICKABLE> arrayCheck; 

    u32 index =0;
    u32 indexDeletion = 0;
    u32 WINAPI runAsyncCreateObjectId(void* user)
    {
        while (!allowed) //ugly but expected for what I want to reproduce
        {}

        for (u32 i =0 ;i<64;i++) 
        {
            BaseObject* pObj = new BaseObject();
            arrayCheck[index] = pObj;
            index++;
        }


        u32 counter = 0;

        ITF_DISABLE_WARNING_PUSH
        ITF_DISABLE_WARNING_ID(4127)
        while (true)
        ITF_DISABLE_WARNING_POP

        {
            BaseObject* pObj = new BaseObject();
            arrayCheck[index%MAX_PICKABLE] = pObj;
            index++;
            counter++;
            if (counter > 1024)
            {
                ThreadSettings::threadStartFrame(); // called at the end to flush data
                counter = 0;
            }
        }
      
        ThreadSettings::threadStartFrame(); // called at the end to flush data
        exited = btrue;
        return 0;
    }



    void ObjRefStress::runTest()
    {
        //create a thread X using the scene thread index
        //while this thread is running create also some BaseObject
        allowed = bfalse;
        ITF_MemoryBarrier();

        arrayCheck.resize(MAX_PICKABLE);

        THREADMANAGER->createThread(runAsyncCreateObjectId,(void*) NULL, ThreadSettings::m_settings[eThreadId_SceneManager]);
        allowed = btrue;

        while (!exited)
        {
            if (index>0)
            {
                u32 newIndex = (indexDeletion)%MAX_PICKABLE;
                BaseObject* pObj = arrayCheck[newIndex];
                arrayCheck[newIndex] = NULL;
                SF_DEL(pObj);
                indexDeletion++;
            }
        }

    }



} // namespace ITF
