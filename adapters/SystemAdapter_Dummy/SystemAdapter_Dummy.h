#pragma once 

#ifdef ITF_DUMMY_SYSTEM_ADAPTER

#include "core/AdaptersInterfaces/SystemAdapter.h"

#ifdef ITF_DUMMY_ADAPTERS_ASSERT_ON_NOT_IMPLEMENTED
    #define ITF_SYSTEMADAPTER_NOT_IMPLEMENTED() ITF_ASSERT_MSG(0, "SystemAdapter not implemented")
#else
    #define ITF_SYSTEMADAPTER_NOT_IMPLEMENTED() do {} while(0)
#endif

namespace ITF
{

    class SystemAdapter_Dummy : public SystemAdapter
    {
    public:
        SystemAdapter_Dummy();
        virtual ~SystemAdapter_Dummy() override;
        virtual bbool openGraphics(int /*_width*/, int /*_height*/, int /*_alphaBits*/, int /*_depthBits*/, int /*_stencilBits*/, bbool /*_fullscreen */ = bfalse, bbool /*_mouseCursor */ = bfalse, const String & /*_name */= "", bbool _waitVBL = btrue) override;

        virtual String userName()const override;
        virtual String dateTime() const override;
        virtual void getTime(Time& /*_time*/) const override;
        virtual f64 getEllaspedFromStart() override;

        virtual void queryPerformanceCounter(LARGE_INTEGER* counter)  const override;
        virtual void queryPerformanceFrequency(LARGE_INTEGER* TicksPerSecond) const override;

        virtual const char* getPlatformName() override;

        // for ITF_MESSAGEBOX_RESULT
        // please keep in mind, this is used for "TOOLS", it's not meant has a default choise for user
        // so when giving this value to a message box, please keep in mind, that we want the "worst case" scenario for tools to detect errors
        virtual i32 messageBox(const String& /*_title*/, const String& /*_msg*/, u32 /*_type*/, ITF_MESSAGEBOX_RESULT /*silentResult*/) override;

        virtual void* allocatePhysicalMemory(u32 _size, u32 _alignment) override;
        virtual void freePhysicalMemory(void* _pointer) override;
    };

    #define SYSTEM_ADAPTER_DUMMY (static_cast<SystemAdapter_Dummy*>(SYSTEM_ADAPTER))

} // namespace ITF

#endif // ITF_DUMMY_SYSTEM_ADAPTER
