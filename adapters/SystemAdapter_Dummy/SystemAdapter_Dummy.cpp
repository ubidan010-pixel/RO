#include "precompiled_SystemAdapter_Dummy.h"

#include "adapters/SystemAdapter_Dummy/SystemAdapter_Dummy.h"

#include <ctime>
#include <cstdlib>

namespace ITF
{

    SystemAdapter_Dummy::SystemAdapter_Dummy()
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
    }

    SystemAdapter_Dummy::~SystemAdapter_Dummy()
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
    }

    bbool SystemAdapter_Dummy::openGraphics(
        int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits,
        bbool _fullscreen, bbool _mouseCursor, const String & _name, bbool _waitVBL)
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
        return btrue;
    }

    String SystemAdapter_Dummy::userName() const
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
        return String("user");
    }

    String SystemAdapter_Dummy::dateTime() const
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();

        std::time_t rawtime;
        std::tm * timeinfo;
        char buffer[1000];

        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);

        std::strftime(buffer, 80, "%m-%d_%H-%M", timeinfo);
        return buffer;
    }

    void SystemAdapter_Dummy::getTime(Time& _time) const
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();

        std::time_t currentTime;
        std::time(&currentTime);
        std::tm* t = std::localtime(&currentTime);
        _time.m_year = 1900 + t->tm_year;       // tm years are since 1900
        _time.m_month = 1 + t->tm_mon;          // tm month: 0-11 we needs 1-12
        _time.m_day = t->tm_mday;
        _time.m_hour = t->tm_hour;
        _time.m_minute = t->tm_min;
        _time.m_second = t->tm_sec;
    }

    f64 SystemAdapter_Dummy::getEllaspedFromStart()
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
        return 0.0;
    }

    void SystemAdapter_Dummy::queryPerformanceCounter(LARGE_INTEGER* counter) const
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
        counter->QuadPart = 0;
    }

    void SystemAdapter_Dummy::queryPerformanceFrequency(LARGE_INTEGER* TicksPerSecond) const
    {
        TicksPerSecond->QuadPart = 1000;
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
    }

    const char* SystemAdapter_Dummy::getPlatformName()
    {
        return "Dummy";
    }

    // for ITF_MESSAGEBOX_RESULT
    // please keep in mind, this is used for "TOOLS", it's not meant has a default choise for user
    // so when giving this value to a message box, please keep in mind, that we want the "worst case" scenario for tools to detect errors
    i32 SystemAdapter_Dummy::messageBox(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult)
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
        return static_cast<i32>(silentResult);
    }

    void* SystemAdapter_Dummy::allocatePhysicalMemory(u32 _size, u32 _alignment)
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
    #ifdef ITF_MICROSOFT
        return _aligned_malloc(_size, _alignment);
    #else
        // This function is not supported in Microsoft C Runtime library
        return std::aligned_alloc(_alignment, _size);
    #endif
    }

    void SystemAdapter_Dummy::freePhysicalMemory(void* _pointer)
    {
        ITF_SYSTEMADAPTER_NOT_IMPLEMENTED();
    #ifdef ITF_MICROSOFT
        // Coherent with _aligned_malloc call
        return _aligned_free(_pointer);
    #else
        std::free(_pointer);
    #endif
    }

} // namespace ITF



