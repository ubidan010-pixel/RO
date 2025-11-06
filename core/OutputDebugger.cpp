#include "precompiled_core.h"

#ifndef ITF_FINAL

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#ifdef ITF_CTR
    #ifndef _ITF_ERRORHANDLER_H_
    #include "core/error/ErrorHandler.h"
    #endif //_ITF_ERRORHANDLER_H_
#endif

namespace ITF
{

    OutputDebugger::OutputDebugger()
    {
    }

    void    OutputDebugger::showString(const char* _str) const
    {
        #if defined(ITF_MICROSOFT)
            OutputDebugStringA(_str);
        #elif defined(ITF_X360)
            OutputDebugStringA(_str);
        #elif defined(ITF_CTR)
            NN_LOG("%s", _str);
        #else
            #undef printf
            printf("%s", _str);
        #endif
    }


    const OutputDebugger& OutputDebugger::operator<<(const char* str) const
    {
        showString(str);
        return *this;
    }

    const OutputDebugger& OutputDebugger::operator<<(char* str) const
    {
        showString(str);
        return *this;
    }

    const OutputDebugger& OutputDebugger::operator<<(const String& str) const
    {
        StringConverter mstr(str);
        showString(mstr.getChar());
        return *this;
    }

    const OutputDebugger& OutputDebugger::operator<<(String& str) const
    {
        StringConverter mstr(str);
        showString(mstr.getChar());
        return *this;
    }


    const OutputDebugger& OutputDebugger::operator<<(u32 _num) const
    {
        String8 str;
        str.setTextFormat("%d", _num);
        showString(str.cStr());
        return *this;
    }

    const OutputDebugger& OutputDebugger::operator<<(u64 _num) const
    {
        String8 str;
        str.setTextFormat("%zd", _num);
        showString(str.cStr());
        return *this;
    }

    const OutputDebugger& OutputDebugger::operator<<(i32 _num) const
    {
        String8 str;
        str.setTextFormat("%d", _num);
        showString(str.cStr());
        return *this;
    }

    const OutputDebugger& OutputDebugger::operator<<(f32 _num) const
    {
        String8 str;
        str.setTextFormat("%f", _num);
        showString(str.cStr());
        return *this;
    }
}

#endif // ITF_FINAL
