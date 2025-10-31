#ifndef _ITF_UPLAYSERVICE_
#define _ITF_UPLAYSERVICE_

#ifdef ITF_SUPPORT_UPLAY

namespace ITF
{
    // https://confluence.ubisoft.com/pages/viewpage.action?pageId=3162815472#id-[TECH]UbisoftConnect-PYLONINFO
#if defined(ITF_UPLAYPC)
    constexpr static int UPLAY_ID = 66807;
#elif defined(ITF_STEAMPC)
    constexpr static int UPLAY_ID = 66807;
#endif

    class UPlayService
    {
    public:

        UPlayService() {}
        virtual ~UPlayService() {};

        virtual u32 update() { return 0; }
        virtual int initialize(int _uplayId) { return 0; }
        virtual void terminate() {}

        virtual bbool isOverlayActive() { return bfalse; };
        virtual String8 getInstalledLanguage() { return "en-US"; };
        virtual String8 getUserName() { return String8::emptyString; }
        virtual String8 getUserEmail() { return String8::emptyString; }
        virtual bbool isClientConnected() { return btrue; }
        virtual bbool isClientOnline() { return btrue; }

        // Show Game Page home
        virtual i32 showOverlay() { return 0; }

        // Show a specific MicroApp section (the DeepLink url) and pass app-specific params
        virtual i32 showOverlay(const String8& _deepLink,
            std::list<std::pair<String8, String8> > _params = {})
            { return 0; }
    };
} // namespace ITF

#endif  //ITF_SUPPORT_UPLAY

#endif //_ITF_UPLAYSERVICE_
