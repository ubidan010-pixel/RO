#include "precompiled_engine.h"

#ifndef _ITF_SCADAPTER_H_
#include "engine/AdaptersInterfaces/SCAdapter.h"
#endif //_ITF_SCADAPTER_H_

#ifndef _ITF_PROCESSSPAWNER_H_
#include "core/system/ProcessSpawner.h"
#endif //_ITF_PROCESSSPAWNER_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_


#ifndef _ALGORITHM_
    #include <algorithm>
#endif // _ALGORITHM_

#if !defined(ITF_IPAD) && !defined(ITF_CTR)	&& !defined(ITF_WII)//TODO_iPad do we need this?

namespace ITF
{

//////////////////////////////////////////////////////////////////////////
    void    SCRequestID::operator ++ ()
    {
        m_value++;
    }

    bool    SCRequestID::operator == ( const SCRequestID& o    ) const
    {
        return o.getValue() == m_value;
    }

    bool   SCRequestID::operator == ( const SCRequestID* o            ) const
    {
        return o->getValue() == m_value;
    }

    bool    SCRequestID::operator <  ( const SCRequestID& o    ) const
    {
        return m_value < o.getValue();
    }

//////////////////////////////////////////////////////////////////////////

    SCRequest::~SCRequest()
    {
      SF_DEL(m_processSpawner);
    }

    void SCRequest::execute()
    {
        m_processSpawner   = new ProcessSpawner();
        m_result    = m_processSpawner->create(m_appName,m_commandLine,m_logFile);
        m_bDone     = btrue;
    }

    u32 WINAPI proxyAdpaterRun(void* user)
    {
        SCAdapter* adapter = (SCAdapter*)(user);
        adapter->process_request();
        return 0;
    }

    void SCAdapter::init(const String& applicationName,const String& url, u32 port, const String& username, const String& userpass, const String& depotFolder,const String& logFile)
    {
        m_ApplicationName   = applicationName;
        m_Url               = url;
        m_Port              = port;
        m_Username          = username;
        m_Userpass          = userpass;
        m_DepotFolder       = depotFolder;
        m_logFile           = logFile;

        m_RunningThread = THREADMANAGER->createThread(proxyAdpaterRun, this, ThreadSettings::m_settings[eThreadId_SCAdapter]);
    }


    SCRequestID SCAdapter::prepare(char* first,... )
    {
        String szParam;
        va_list marker;
        va_start(marker,first);
        char* i = first;

        if (first)
            szParam = first;

        while (i!=NULL)
        {
            szParam += " ";
            i = va_arg( marker, char*);
            if (i)
               szParam += i;
            
        }

#if ITF_DEBUG_LEVEL > 0
        LOG("Source control request : \n\t%s\n\n", StringToUTF8(szParam).get());
#endif // ITF_DEBUG_LEVEL > 0
       
        //Make a new request
        SCRequest* request = new SCRequest(m_ApplicationName,szParam,m_logFile);
        return add_request(request);
    }

    SCRequestID SCAdapter::add_request(SCRequest* request)
    {
        //[TODO] make it thread safe
        ITF_ASSERT(request);
        m_vRequests.push_back(request);
        ++m_currentRequestID;
        request->setId(m_currentRequestID);
        return m_currentRequestID;
    }

//////////////////////////////////////////////////////////////////////////
    SCAdapter::~SCAdapter()
    {
        m_bExit = btrue;
        //[TODO]
        //wait the thread exit event before leaving
        sleep(500);


        SF_DEL(m_RunningThread);

        for (ITF_VECTOR<SCRequest*>::iterator iter = m_vRequests.begin();iter!=m_vRequests.end();++iter)
        {
            SF_DEL(*iter);
        }

        for (ITF_MAP<SCRequestID,SCRequest*>::iterator iter = m_mResquestDone.begin();iter!=m_mResquestDone.end();++iter)
        {
            SF_DEL((*iter).second);
        }
    }


    bbool SCAdapter::pollRequest(const SCRequestID& id,SCRequestInfo& rqInfo)
    {
        ITF_MAP<SCRequestID,SCRequest*>::iterator iter =m_mResquestDone.find(id);
        
        if (iter!=m_mResquestDone.end())
        {   
            SCRequest* request = (*iter).second;
            const ProcessSpawner* processSpawner = request->getProcessSpawner();
            rqInfo.setOutputDetails(processSpawner->getLog());
            rqInfo.setResult(request->getResult());
            m_mResquestDone.erase(iter);
            SF_DEL(request);


            return btrue;
        }

        return bfalse;
    }
    
    void SCAdapter::process_request()
    {
        m_bExit  = bfalse;
        
        while (!m_bExit)
        { //[TODO] make it thread safe
            ThreadSettings::threadStartFrame();

            //make an copy even if empty...
            //LOCK
            ITF_VECTOR<SCRequest*> vRequestPending  = m_vRequests;
            //UNLOCK
            ITF_VECTOR<SCRequest*> vRequestToRemove;

            vRequestToRemove.reserve(vRequestPending.size());
            for (ITF_VECTOR<SCRequest*>::iterator iter = vRequestPending.begin();iter!=vRequestPending.end();++iter)
            {
                (*iter)->execute();

                m_mResquestDone[(*iter)->getId()] = *iter;

                vRequestToRemove.push_back(*iter);
            }

             //LOCK
            ITF_VECTOR<SCRequest*>::iterator iter2 = vRequestToRemove.begin();
            while(iter2!=vRequestToRemove.end())
            {
                ITF_VECTOR<SCRequest*>::iterator iterToRemove = find(m_vRequests.begin(),m_vRequests.end(),*iter2);
                ITF_ASSERT(iterToRemove != m_vRequests.end());
                m_vRequests.erase(iterToRemove);

                iter2 = vRequestToRemove.erase(iter2);
            }
            //UNLOCK
            
            
            Sleep(50);
        }
    }


}// namespace ITF

#endif //ITF_IPAD
