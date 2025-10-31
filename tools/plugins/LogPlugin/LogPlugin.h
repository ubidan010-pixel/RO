#ifndef _ITF_LOGPLUGIN_H_
#define _ITF_LOGPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

class Transaction;
class Transaction_Serialize_Receive;
class NETPeer;
class Blob;


typedef void(*funcAssetFactoryCommand)(Blob*,Transaction_Serialize_Receive*);

class LogPlugin : public Plugin
{
public:
    LogPlugin();
    virtual ~LogPlugin();
    static const char*      getPluginName() {return  "Log_Plugin";}

    void    update();

    // Plugin inherited
    void                    receive(Blob& _blob,const NETPeer* _peer, u32 _senderID);
 
    void onLog(const String& szExeName,const String& _szLogText);
    void onNewTransaction(const Transaction* _pTransaction);
    void onFinishTransaction(const Transaction* _pTransaction);
    void onUpdateTransactions(const ITF_VECTOR<Transaction*> &_vTransactions);

private:



    ITF_MAP<String,funcAssetFactoryCommand> m_mFuncAssetCommand;
    ITF_VECTOR<Transaction*> m_vTransactionPending;

};

} // namespace ITF


#endif // _ITF_LOGPLUGIN_H_
