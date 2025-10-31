#ifndef _ITF_OBJECTID_SERVER_H_
#define _ITF_OBJECTID_SERVER_H_

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

//////////////////////////////////////////////////////////////////////////
///
/// Delivers unique object references
///
class ObjectIdServer : public TemplateSingleton<ObjectIdServer>
{
private:
    u32                             m_userId;
    u32                             m_sessionCounter;
    ITF_THREAD_CRITICAL_SECTION     m_csThreadId;//CS to protect thread id

public: 

    ObjectIdServer();
    ~ObjectIdServer();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Generate a unique ObjectId, protected by a critical section
    /// @return a unique ObjectId
    ObjectId                        generateObjectId();

    void                            setUserId(u32 _id);
};

#define OBJECTID_SERVER             ObjectIdServer::getptr()

} // namespace ITF

#endif  //_ITF_OBJECTID_SERVER_H_
