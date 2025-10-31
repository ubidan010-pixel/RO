#ifndef ITF_ASSERTMANAGER_H_
#define ITF_ASSERTMANAGER_H_

#pragma once

namespace ITF
{
    typedef unsigned int FlagType;
    class AssertManager
    {
    public:

#if defined(EXPORT_ASSERTS)
        static void init();
        static void cleanup();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Try to register the assert to the assert database, will slow the game if too many differents asserts are reached
        /// @param _file the file
        /// @param _line the line
        /// @param _assertion the assert condition
        /// @param _message the assert message
        /// @return true if we skip next export for this assert
        static bool exportAssertToWebStatPage(const char* _file, int _line, const char* _assertion, const char* _message, ... );

        static char assertServerInsertPage[512];

        static void close() {AssertManager::assertServerInsertPage[0] = 0;}
#endif // EXPORT_ASSERTS

        enum Choice
        {
            Break,
            IgnoreOnce,
            IgnoreAlways
        };
        
        static Choice CallAssert(const char * condition, const char * file, int line, const char * message, ... );
        static void displayAssert(const char * condition, const char * file, int line, const char * message, ... );
        static void OutputDebugMsg( const char * condition, const char * file, int line, int skipCount, const char * message, ... );
        static void OutputDebugMsg(const char * _str);

        // Flag manipulation, to enable/disable specific asserts
        static inline FlagType isFlagEnabled(FlagType _flag)    { return (m_bAssertEnabled & _flag); }
        static void setFlag(FlagType _flag)                     { m_bAssertEnabled |= _flag; }
        static void unsetFlag(FlagType _flag)                   { m_bAssertEnabled &= ~_flag; }
        static const FlagType m_FlagAll             = 0xffFFffFF;
        static const FlagType m_FlagWarningAndError = 0x00000001;

    private:
        static FlagType m_bAssertEnabled;
    };


} //namespace ITF


#endif // ITF_ASSERTMANAGER_H_
