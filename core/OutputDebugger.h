#ifndef _ITF_OUTPUTDEBUGGER_H_
#define _ITF_OUTPUTDEBUGGER_H_

#ifdef ITF_FINAL
    #define DECLARE_OUTPUTDBG
    #define OUTPUTDBG(expr)  ((void)0)
#else

namespace ITF
{
    #define DECLARE_OUTPUTDBG OutputDebugger localOutputDebugger
    #define OUTPUTDBG(expr)  localOutputDebugger << (expr)

    class OutputDebugger
    {
    public:
        OutputDebugger();

        void    showString(const char* _str) const;
        const OutputDebugger& operator<<(const char* str) const;
        const OutputDebugger& operator<<(char* str) const;
        const OutputDebugger& operator<<(const String& str) const;
        const OutputDebugger& operator<<(String& str) const;
        const OutputDebugger& operator<<(i32 _num) const;
        const OutputDebugger& operator<<(u32 _num) const;
        const OutputDebugger& operator<<(u64 _num) const;
        const OutputDebugger& operator<<(f32 _num) const;
    };
}

#endif // ITF_FINAL
#endif // _ITF_OUTPUTDEBUGGER_H_
