#ifndef _ITF_SYSTEMMESSAGELISTENER_X360_H_
#define _ITF_SYSTEMMESSAGELISTENER_X360_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

namespace ITF
{
///////////////////////////////////////////////////////////////////////////////////////////
///Event queue for xbox messages
class SystemMessageListener_x360
{
public:
    struct Message
    {
        DWORD m_notification;
        ULONG_PTR m_param;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Constructor
    /// @param _acceptedMessages
    /// @param _acceptedCount
    SystemMessageListener_x360(const DWORD *_acceptedMessages, u32 _acceptedCount)
    {
        m_acceptedMessages.resize(_acceptedCount);
        for (u32 index=0; index<_acceptedCount; index++)
            m_acceptedMessages[index] = _acceptedMessages[index];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// isAcceptedMessage
    /// @param _message the message to test
    /// @return btrue if among accepted
    bbool isAcceptedMessage(DWORD _message)
    {
        u32 acceptedCount = m_acceptedMessages.size();
        for (u32 i=0; i<acceptedCount; i++)
            if (m_acceptedMessages[i]==_message)
                return btrue;
        return bfalse;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// push a message. This method can be overloaded
    /// @param _message
    /// @param _parameter
    virtual void pushMessage(DWORD _message, ULONG_PTR _parameter)
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        if (!isAcceptedMessage(_message))
            return;

        Message m;
        m.m_notification = _message;
        m.m_param = _parameter;
        m_messages.push_back(m);
    }  

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Pop a message from the list
    /// @param _message [out] The message
    /// @param parameter [out] its parameter
    /// @return btrue if a message was retrieved, otherwise _message and _parameter are not output
    bbool popMessage(DWORD &_message, ULONG_PTR &_parameter)
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        if (!m_messages.size())
            return bfalse;

        _message = m_messages[0].m_notification;
        _parameter = m_messages[0].m_param;
        m_messages.eraseKeepOrder(0);
        return btrue;
    }

private:
    SafeArray<DWORD> m_acceptedMessages;
    SafeArray<Message> m_messages;
    ULONGLONG m_categories;
};
}

#endif // _ITF_SYSTEMMESSAGELISTENER_X360_H_