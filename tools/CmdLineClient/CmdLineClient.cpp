// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "enginecommon.h"
#endif // _ENGINECOMMON_H_
//

#include <windows.h>
#include <tchar.h>
#ifndef _ITF_UDPADAPTER_H_
    #include "engine/AdaptersInterfaces/UDPAdapter.h"
#endif // _ITF_UDPADAPTER_H_
#include "adapters/UDPAdapter_ENet/UDPAdapter_ENet.h"
#ifndef _ITF_SINGLETONS_H_
    #include "engine/core/Singletons.h"
#endif // _ITF_SINGLETONS_H_

#ifndef _ITF_GFX_ADAPTER_H_
    #include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#include "adapters/GFXAdapter_OpenGL/GFXAdapter_OpenGL.h"
#ifndef _ITF_COMMANDSHANDLER_H_
    #include "engine/script/CommandsHandler.h"
#endif //_ITF_COMMANDSHANDLER_H_
#ifndef _ITF_LUAHANDLER_H_
    #include "engine/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_


#ifndef _ITF_LOGADAPTER_NET_H_
#include "adapters/LOGAdapter_Net/LOGAdapter_Net.h"
#endif //_ITF_LOGADAPTER_NET_H_

int _tmain(int argc, _TCHAR* argv[])
{
    printf ("\n\n**** CMDLINE ****\n");
    ITF::String url;

    if (argc != 2)
    {
        printf("usage: LogServer <url>\n Defaulting url to 127.0.0.1\n");
        url.setText("127.0.0.1");
    }
    else
        url.setText((ITF::u16*)argv[1]);


    ITF::StringConverter str(url);

    ITF::Singletons& pSing = ITF::Singletons::get();
    pSing.setUDPAdapter (new ITF::UDPAdapter_ENet());

    ITF::UDP_ADAPTER->initialize();
    ITF::UDP_ADAPTER->setClient();
    pSing.setLOGAdapter (new ITF::LOGAdapter_Net());

    pSing.setCommandsHandler(new ITF::CommandsHandler());
    pSing.setLUAHandler (new ITF::LUAHandler());

    printf ("connecting to host %s, port %d\n", str.getChar(), GAME_SERVER_PORT);
    if (!ITF::UDP_ADAPTER->hostConnection(str.getChar(), GAME_SERVER_PORT))
    {
        printf("Connection failed\n");
        printf ("press enter key to terminate\n");
        getchar ();
        exit(0);
    }
    ITF::UDP_ADAPTER->declareCmdLineClient();
    printf("Connection succeeded\n");

    for(;;)
    {
        char cmd[1024];
        cmd[0] = 0;
        printf("\nenter a command:");
        scanf("%s", cmd);
        if (!strcmp(cmd,"exit"))
            break;
        ITF::UDP_ADAPTER->sendPacketToHost(ITF::UDPAdapter::RecipientType_CommandsHandler, cmd, (ITF::u32)strlen(cmd));
    }
    return 0;
}

