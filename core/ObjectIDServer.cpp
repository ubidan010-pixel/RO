#include "precompiled_core.h"

#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDserver.h"
#endif //_ITF_OBJECTID_SERVER_H_

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


namespace ITF
{

    ObjectIdServer::ObjectIdServer()
        : m_sessionCounter(0)
        , m_userId(0)
    {
        Synchronize::createCriticalSection(&m_csThreadId);
    }


    ObjectIdServer::~ObjectIdServer()
    {
        Synchronize::destroyCriticalSection(&m_csThreadId);
    }

    ObjectId ObjectIdServer::generateObjectId()
    {
        csAutoLock cs(m_csThreadId);

        ObjectId id;

        if (!SYSTEM_ADAPTER)
            return id;//return an invalid id;
        
        id.m_details.m_reserved = 0;

        //get user
        id.m_details.m_user = m_userId;

        //date
        Time date;
        SYSTEM_ADAPTER->getTime(date);

        int year = date.m_year - 2010;
        const int maxYear = (1<<ObjectId::YEAR_BITS)-1;

        ITF_ASSERT_MSG(year >= 0 && year <= maxYear, "Current local date is wrong ?");

        year = Clamp(year,0,maxYear);

        id.m_details.m_year   = year   ;
        id.m_details.m_month  = date.m_month  ;
        id.m_details.m_day    = date.m_day    ;
        id.m_details.m_hour   = date.m_hour   ;
        id.m_details.m_minute = date.m_minute ;
        id.m_details.m_second = date.m_second ;

        //session
        id.m_details.m_session = m_sessionCounter;

        //TODO PROTECT BY ATOMIC OPERATION
        u32 counter = m_sessionCounter+1;
        counter = counter % (1<<ObjectId::SESSION_BITS);
        m_sessionCounter = counter;
        return id;
    }

    void ObjectIdServer::setUserId(u32 _id)
    {
        m_userId = _id % (1<<ObjectId::USER_BITS);
    }

} // namespace ITF
