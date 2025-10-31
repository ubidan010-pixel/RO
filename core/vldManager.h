#ifndef _ITF_VLDMANAGER_H_
#define _ITF_VLDMANAGER_H_

#if defined (ITF_WINDOWS)
#include "VisualLeakDetector/vldapi.h"		// detects memory leaks
#endif // WIN32

namespace ITF
{
#ifdef _DEBUG 

    #define ITF_VLD_SUPPORTED

    typedef void (*funcVLD)();

    class vldManager
    {
    public:
        vldManager()
        {

        }

        static void disableApplication()
        {
            m_bEnableApplication = bfalse;
            if (m_funcDisable)
                m_funcDisable();
        }

        static void apply()
        {
            if (!m_bEnableApplication && m_funcDisable)
                m_funcDisable();
        }

        static void tryEnable()
        {
            if (m_bEnableApplication && m_funcEnable)
                m_funcEnable();
        }

        static void disable()
        {
            if (m_bEnableApplication && m_funcDisable) 
                m_funcDisable();
        }

        static funcVLD  m_funcEnable;
        static funcVLD  m_funcDisable;

    protected:

        static bbool    m_bEnableApplication;


       
    };
#else
    class vldManager
    {
    public:
        vldManager(){};

        static void apply() {};
        static void enable()    {};
        static void disable()   {};
        static void tryEnable() {};
    };
#endif
    

} // namespace ITF


#endif // _ITF_VLDMANAGER_H_
