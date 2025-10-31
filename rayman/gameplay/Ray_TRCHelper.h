#ifndef _ITF_RAY_TRCHELPER_H_
#define _ITF_RAY_TRCHELPER_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

namespace ITF
{

    class Ray_TRCHelper: TRCHelper
    {

    public:

        Ray_TRCHelper();
        virtual~Ray_TRCHelper() {}
        
        virtual void buildPlayerField(const String8& _menuName, u32 _playerIndex, bbool _asEmpty);
        virtual void buildMessageField(const String8& _menuName, const String& _message, bbool _asEmpty);
        virtual void buildTitleField(const String8& _menuName, const String& _title, bbool _asEmpty);
    };
}

#endif //_ITF_RAY_TRCHELPER_H_
