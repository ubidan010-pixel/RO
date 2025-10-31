// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_LOGPLUGIN_H_
#include "LogPlugin.h"
#endif //_ITF_LOGPLUGIN_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_BLOBUTILS_H_
#include "core/blobUtils.h"
#endif //_ITF_BLOBUTILS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

//DEPRECATE SOON

namespace ITF
{
    bbool addLog(Blob* /*_blobToWrite*/,Blob* /*_parameters*/)
    {
        return btrue;
    }

    void Transaction_AddLog(Blob* /*_pBlob*/,Transaction_Serialize_Receive* /*_pTransaction*/)
    {
    }

    void LogPlugin::onUpdateTransactions(const ITF_VECTOR<Transaction*> &/*_vTransactions*/)
    {
    }

    void LogPlugin::onFinishTransaction(const Transaction* /*_pTransaction*/)
    {      
      
    }


    void LogPlugin::onNewTransaction(const Transaction* /*pTransaction*/)
    {
      

    }


    LogPlugin::LogPlugin()
    {
    }

    LogPlugin::~LogPlugin()
    {

        m_mFuncAssetCommand.clear();
    }

    void    LogPlugin::update()
    {
        
    }


    void LogPlugin::onLog(const String& /*_szExeName*/,const String& /*_szLogText*/)
    {
    }


    void    LogPlugin::receive(Blob& /*_blob*/,const NETPeer* /*_peer*/, u32 /*_senderID*/)
    {
    }



} // namespace ITF

