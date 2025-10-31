#ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__
#define _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__

#ifdef ITF_X360
    #ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_X360_H_
    #include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages_x360.h"
    #endif
#else
    #ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_DUMMY_H_
    #include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages_dummy.h"
    #endif
#endif

#endif