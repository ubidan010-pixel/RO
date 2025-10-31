// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "enginecommon.h"
#endif // _ENGINECOMMON_H_
//

#include <tchar.h>
#ifndef _ITF_UDPADAPTER_H_
    #include "engine/AdaptersInterfaces/UDPAdapter.h"
#endif // _ITF_UDPADAPTER_H_
#include "adapters/UDPAdapter_ENet/UDPAdapter_ENet.h"
#ifndef _ITF_SINGLETONS_H_
    #include "engine/core/Singletons.h"
#endif // _ITF_SINGLETONS_H_

bool stopServer = false;

DWORD WINAPI Thread_Server (LPVOID lpParameter)
{
    ITF::NETEvent evt;
    for(;;)
    {
        if (ITF::UDP_ADAPTER->getEvent(evt))
        {
            switch (evt.m_type)
            {
            case ITF::NETEvent::connect :
                printf("received connection from %d %d\n", evt.m_host, evt.m_port);
                break;
            case ITF::NETEvent::disconnect :
                printf("received deconnection from %d %d\n", evt.m_host, evt.m_port);
                break;
            case ITF::NETEvent::receive :
                {
                char* str = new char[evt.m_dataLength + 1];
                memcpy(str, evt.m_data, evt.m_dataLength);
                str[evt.m_dataLength] = 0;
                printf("%s\n", str);
                SF_DEL_ARRAY(str);
                break;
                }
            default:
                break;
            }
        }

        if (evt.m_data)
            delete[] evt.m_data;
        evt.m_data = 0;

        if (stopServer)
            break;
    }

    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    printf ("\n\n**** LOG SERVER ****\n");

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
    printf ("connecting to host %s, port %d\n", str.getChar(), GAME_SERVER_PORT);
    while (!ITF::UDP_ADAPTER->hostConnection(str.getChar(), GAME_SERVER_PORT))
    {
        printf("Connection failed, retry will occur in 5 seconds\n");
        Sleep(5000);
    }
    ITF::UDP_ADAPTER->declareLogClient();
    printf("Connection succeeded\n");



    HANDLE w_hServer;
    unsigned long w_ThreadID;
    w_hServer = CreateThread (NULL, NULL, Thread_Server, NULL, 0, &w_ThreadID);
    printf ("press enter key to terminate\n");
    getchar ();

    stopServer = true;
    printf ("\nWait 5 sec to close thread...\n");    
    Sleep (5000); // wait for server thread to exit

    
    return 0;
}

