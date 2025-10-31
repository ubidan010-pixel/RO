#include "precompiled_HostdbGateway.h"

#ifdef WIN32
    #include "VisualLeakDetector/vld.h"        // detects memory leaks
#endif // WIN32

#ifndef _ITF_UDPADAPTER_H_
    #include "engine/AdaptersInterfaces/TCPAdapter.h"
#endif // _ITF_UDPADAPTER_H_

#ifndef _ITF_TCPADAPTER_ITF_H_
    #include "adapters/TCPAdapter_ITF/TCPAdapter_ITF.h"
#endif //_ITF_TCPADAPTER_ITF_H_

#ifndef _ITF_NETPACKETHANDLER_H_
    #include "engine/network/NETPacketHandler.h"
#endif // _ITF_NETPACKETHANDLER_H_

#ifndef _ITF_TRANSACTION_FACTORY_H_
    #include "engine\network\Transaction_Factory.h"
#endif // _ITF_TRANSACTION_FACTORY_H_

#ifndef _ITF_TRANSACTION_HANDLER_H_
    #include "engine\network\Transaction_Handler.h"
#endif // _ITF_TRANSACTION_HANDLER_H_

#ifndef _ITF_XMLALL_H_
    #include "engine/xml/XML_All.h"
#endif // _ITF_XMLALL_H_

#ifndef _ITF_SCADAPTER_H_
    #include "engine/AdaptersInterfaces/SCAdapter.h"
#endif //_ITF_SCADAPTER_H_

#ifndef _ITF_SCADAPTERSVN_H_
    #include "adapters/SCAdapter_SVN/SCAdapter_SVN.h"
#endif // _ITF_SCADAPTERSVN_H_

#ifndef _ITF_FILESERVER_H_
    #include "engine/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_STORAGEADAPTER_SQLITE_H_
    #include "adapters/StorageAdapter_Sqlite/StorageAdapter_Sqlite.h"
#endif //_ITF_STORAGEADAPTER_SQLITE_H_

#ifndef _ITF_SYSTEMADAPTER_H_GLFW_
    #include "adapters/SystemAdapter_GLFW/SystemAdapter_GLFW.h"
#endif // _ITF_SYSTEMADAPTER_H_GLFW_

#ifndef _ITF_THREAD_H_
    #include "engine/system/Thread.h"
#endif _ITF_THREAD_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif // _ITF_PLUGINGATEWAY_H_

#ifndef _ITF_LOGPLUGIN_H_
#include "tools/plugins/LogPlugin/LogPlugin.h"
#endif //_ITF_LOGPLUGIN_H_

#ifndef _ITF_DBGATEWAYMANAGER_H_
#include "ClientdbGateway/manager/dbgatewayManager.h"
#endif //_ITF_DBGATEWAYMANAGER_H_

#ifndef _ITF_DBEXPORT_H_
#include "dbImportExport/dbExport.h"
#endif //_ITF_DBEXPORT_H_

#ifndef _ITF_DBIMPORT_H_
#include "dbImportExport/dbImport.h"
#endif //_ITF_DBIMPORT_H_


#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif // _ITF_RESOURCEMANAGER_H_

#define XML_CONFIG "coreConfig/host.xml"

//--------------------------------------------------------------------------------

ITF::u32 GetDatabaseID()
{
    ITF::String sID = ITF::XMLAll::readField("databank_id", XML_CONFIG);
    if(sID == "")
    {
        // Default ID will be used !
        // Did you forget to set up your host ?
        ITF_ASSERT(0);

        // Default ID
        return 1;
    }

    return sID.atoui32();
}

//--------------------------------------------------------------------------------

void CreateSingletons()
{
    ITF::Singletons&  pSing = ITF::Singletons::get();
    pSing.setTCPAdapter(new ITF::TCPAdapter_ITF());
    pSing.setSCAdapter(new ITF::SCAdapter_SVN());
    pSing.setStorageAdapter(new ITF::StorageAdapter_Sqlite());
    ITF::STORAGE_ADAPTER->setDatabankID(ITF::StorageAdapter::DatabankID_Local);
    pSing.setSystemAdapter(new ITF::SystemAdapter_GLFW());
    pSing.setResourceManager(new ITF::ResourceManager());
}

//--------------------------------------------------------------------------------

volatile ITF::bbool stopServer = ITF::bfalse;

DWORD WINAPI Thread_Server (LPVOID lpParameter)
{
    // Constructor register itself as listener
    ITF::Transaction_Factory trFactory;

    for(;;)
    {
        ITF::sleep(3);

        ITF::NETPACKET_HANDLER->update(ITF::TCP_ADAPTER);
        ITF::TRANSACTION_HANDLER->update();

        if (ITF::PLUGINGATEWAY)
        {
            ITF::PLUGINGATEWAY->update();
            ITF::dbGatewayManager::getSingleton()->update();
        }

        if (stopServer)
            break;
    }

    return 0;
}

//--------------------------------------------------------------------------------

void InitializeSourceControl()
{
    ITF_ASSERT(ITF::SC_ADAPTER != NULL);

    ITF::String localdepot_dir  = ITF::XMLAll::readField("localdepot_dir", XML_CONFIG);
    ITF::String SVN_username    = ITF::XMLAll::readField("svn_user_name", XML_CONFIG);
    ITF::String SVN_userpass    = ITF::XMLAll::readField("svn_user_password", XML_CONFIG);
    ITF::String SVN_server_url  = ITF::XMLAll::readField("svn_server_url", XML_CONFIG);
    ITF::u32    SVN_server_port = ITF::XMLAll::readField("svn_server_port", XML_CONFIG).atoui32();
    ITF::String SVN_bin_dir     = ITF::XMLAll::readField("svn_bin_dir", XML_CONFIG);
    ITF::String SVN_log_file    = ITF::XMLAll::readField("svn_log_file", XML_CONFIG);

    ITF::SC_ADAPTER->init(SVN_bin_dir, SVN_server_url, SVN_server_port, SVN_username, SVN_userpass, localdepot_dir, SVN_log_file);
}
//--------------------------------------------------------------------------------

int func_exit()
{
    if(ITF::ERROR_HANDLER)
        LOG("[EXIT] Shutting down all threads...\n");

    stopServer = true;

    ITF::sleep(100);


    if (ITF::PLUGINGATEWAY)
    {
        delete ITF::PLUGINGATEWAY;
    }
    ITF::Singletons::get().setPluginGateWay(NULL);

    ITF::SINGLETONS.destroy();

    return 0;
}

void RegisterPlugins()
{
    // Plugin Gateway initialization
    ITF::PluginGateWay::createSingleton();
    ITF::PLUGINGATEWAY->init("127.0.0.1", 2000, ITF::btrue);
    ITF::PLUGINGATEWAY->addPlugin(ITF::LogPlugin::getPluginName(), new ITF::LogPlugin());
}


void ErrorCallback(const ITF::String& szExeName,const ITF::String& _szLogtext)
{
    if (ITF::PLUGINGATEWAY)
        ITF::PLUGINGATEWAY->onLog(szExeName,_szLogtext);
}

void onNewTransaction(const ITF::Transaction* _pTransaction)
{
    ITF::LogPlugin* logPlugin =( ITF::LogPlugin*) ITF::PLUGINGATEWAY->getPluginByName(ITF::LogPlugin::getPluginName());
    ITF_ASSERT(logPlugin);

    logPlugin->onNewTransaction(_pTransaction);
}

void onUpdateTransactions(const ITF_VECTOR<ITF::Transaction*> &_vTransactions)
{

    ITF::LogPlugin* logPlugin =( ITF::LogPlugin*) ITF::PLUGINGATEWAY->getPluginByName(ITF::LogPlugin::getPluginName());
    ITF_ASSERT(logPlugin);

    logPlugin->onUpdateTransactions(_vTransactions);
}

void onFinishTransaction(const ITF::Transaction* _pTransaction)
{
    ITF::LogPlugin* logPlugin =( ITF::LogPlugin*) ITF::PLUGINGATEWAY->getPluginByName(ITF::LogPlugin::getPluginName());
    ITF_ASSERT(logPlugin);

    logPlugin->onFinishTransaction(_pTransaction);
}


int main(int argc, char* argv[])
{
    using namespace ITF;
    _onexit(func_exit);

    CreateSingletons();

    TCP_ADAPTER->initialize();
    String localDepotDir = XMLAll::readField("localdepot_dir", XML_CONFIG);
    LOG("DEPOT : %S\n", localDepotDir.cStr());

    SINGLETONS.setdbGatewayManager(new dbGatewayManager());
 
    RegisterPlugins();

    ITF::ERROR_HANDLER->setCallback(ErrorCallback);

    FILESERVER->setWorkingDepot(localDepotDir);

    ITF_VERIFY(STORAGE_ADAPTER->init(FILESERVER->getSystemDepot(),btrue));

    InitializeSourceControl();

    
    int default_port = XMLAll::readField("listening_port", XML_CONFIG).atoi32();

    if (!TCP_ADAPTER->setHost("", default_port))
    {
        LOG("Connection failed\n");
        LOG ("press enter key to terminate\n");
        getchar ();
        exit(0);
    }           

    TRANSACTION_HANDLER->setCBOnNewTransaction(onNewTransaction);
    TRANSACTION_HANDLER->setCBOnFinishTransaction(onFinishTransaction);
    TRANSACTION_HANDLER->setCBOnUpdateTransactions(onUpdateTransactions);

    ITF_MemoryBarrier();
    HANDLE w_hServer;
    unsigned long w_ThreadID;
    w_hServer = CreateThread (NULL, NULL, Thread_Server, NULL, 0, &w_ThreadID);
    LOG("Server ready\n");
    LOG("press enter key to terminate\n");
    getchar ();

    func_exit();



    return 0;
}


//[KEEP it for test]
    //test export
/*
    dbExport* pExport = new dbExport();
    pExport->exportModel();
    SF_DEL(pExport);

    return 0;
    
    //test import

    dbImport* pImport = new dbImport();
    pImport->importModel();
    SF_DEL(pImport);

    
    return 0;*/