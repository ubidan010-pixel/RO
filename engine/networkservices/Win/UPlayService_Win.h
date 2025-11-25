#if defined(ITF_SUPPORT_UPLAY) && defined(ITF_WINDOWS)
#ifndef _ITF_UPLAYSERVICE_WIN_H_
#define _ITF_UPLAYSERVICE_WIN_H_

#ifndef _ITF_UPLAYSERVICE_
#include "engine/networkservices/UPlayService.h"
#endif //_ITF_UPLAYSERVICE_

#include "UplayPC/include/UPC.h"

namespace ITF
{
    class UPlayService_Win : public UPlayService
    {

    public:

        UPlayService_Win();
        ~UPlayService_Win();

        virtual u32 update();
        virtual int initialize(int _uplayId);
        virtual void terminate();
        virtual i32 showOverlay();
        virtual String8 getToken();
        inline virtual bbool isOverlayActive() { return m_isOverlayActive; }

    private:
        UPC_Context* m_Context;
        bbool m_isInitialized;
        bbool m_isOverlayActive;

        String8 m_language;

        static void showOverlayCallback(UPC_Event* inEvent, void* inData);
    };

} // namespace ITF


#endif //_ITF_NETWORKSERVICES_WIN_H_

#endif //ITF_SUPPORT_UPLAY && ITF_WINDOWS
