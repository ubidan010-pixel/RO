#ifndef _ITF_TESTISCPLUGIN_H_
#define _ITF_TESTISCPLUGIN_H_

#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#include <set>

namespace ITF
{

class Transaction;
class Transaction_Serialize_Receive;
class NETPeer;
class Blob;


typedef void(*funcAssetFactoryCommand)(Blob*,Transaction_Serialize_Receive*);

class Plugin_TestISC : public Plugin,  public Interface_SceneEventsListener
{	
private:
    enum AppState
    {
        NoState,
        BootstrapLoading,
    };

public:
    Plugin_TestISC();
    virtual ~Plugin_TestISC();
    static const char*      getPluginName() {return  "Plugin_TestISC";}

    void    update();

    // Plugin inherited
    void                    receive(Blob& _blob,const NETPeer* _peer, u32 _senderID);
 
    void onLog(const String& szExeName,const String& _szLogText);
    void onNewTransaction(const Transaction* _pTransaction);
    void onFinishTransaction(const Transaction* _pTransaction);
    void onUpdateTransactions(const ITF_VECTOR<Transaction*> &_vTransactions);
	void OnFileMissing(const String& _actorFileName, Path& _luaPath );
	void OnRefMissing(const char* filename,const String& reference);
		
	virtual void onSceneAsyncLoadFinished(const Scene* _pScene);	
	virtual void OnDataError(const String& _str,Pickable* pickable);
	
	virtual void OnMapLoaded(Scene* rootScene);

	virtual void onScenePreDestroy(Scene* _pScene) {}
	virtual void onDeleteScene(const Scene* _pScene) {}
	virtual void onObjectAdded(const Scene* _pScene, BaseObject* _pObj) {}
	virtual void onPostSceneActivation(const Scene* _pScene, bbool _bActive) {}
	virtual void  OnFatalError(const char* msg);
	virtual void  OnWarning(const char* msg);
    virtual void  OnFileOpened(const char* filename, const char* threadId, u32 frameId);
    virtual void  OnWorldUpdated();

	virtual void onObjectRemoved(const ITF::Scene *,ITF::BaseObject *, bbool){}
	const NETPeer* _peer;
	u32 _senderID;
	String worldpath;
	Path curPath;

private:
    void RefObjectResources( ITF::Blob& blobToSend, const ObjectRef& ref );
    void ListObjectResources( ITF::Blob& blobToSend, const ObjectRef& ref, i32 cellId = -1 );
    void ListResources( SafeArray< ActorRef >& actors, SafeArray< ObjectRef >& frizes );
    void RecurseResources( ITF::Blob& blobToSend, const ResourceGroup::ResourceList& list, i32 cellId );

    ITF_MAP<String,funcAssetFactoryCommand> m_mFuncAssetCommand;
    ITF_VECTOR<Transaction*> m_vTransactionPending;

    bool                                    _requestExplore;
    std::set< StringID::StringIdValueType > _resourceSet;
};

} // namespace ITF


#endif // _ITF_TESTISCPLUGIN_H_
