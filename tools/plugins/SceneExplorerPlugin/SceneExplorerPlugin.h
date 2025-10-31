#ifndef _ITF_SCENEEXPLORERPLUGIN_H_
#define _ITF_SCENEEXPLORERPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_SCENE_TYPES_H_  
#include "engine\scene\scene_types.h"
#endif //_ITF_SCENE_TYPES_H_


namespace ITF
{
class Scene;
class Frise;
class Actor;
class PolyLine;

class SceneExplorerPlugin : public Plugin
{
public:
    SceneExplorerPlugin();
    virtual ~SceneExplorerPlugin();

    static const char*      getPluginName() {return  "SceneExplorer_Plugin";}
    static const float      getUpdateRate() {return 0.5f;} // in seconds

    // Plugin inherited
    virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );

    virtual void            onPostSceneActivation(const Scene* _scene, bbool bActivate); // inherited from Plugin

	virtual void			onActorPicked(ObjectRef _actor);
	virtual void			onFrisePicked   (ObjectRef _frise);

    virtual void            UnselectAll(bbool _force);
    virtual void            update();


private:
	//Scene tree
	class SceneTree
	{
	public:
		//Constructors/Destructors
		SceneTree() {m_isRoot = false;}
		~SceneTree();
		SceneTree(const SceneTree &_other);

		//Comparison
		bool isEqual_IncludingObjects(SceneTree *_other);

		//Save a description of this class to a Blob
		void saveToBlob(ITF::Blob &_blob, bool _isOffline);

		//Data
		PickableList               m_actors;
        PickableList               m_frises;
		ITF_LIST<SceneTree*>	m_childScenes;
		String					m_sceneUniqueName, m_scenePath;
		bool					m_isRoot; //root scene is a fake scene for hosting several scenes

	};

	void					setCameraOnMultiSelection(SafeArray<ObjectRef> &selection);
	SceneTree*				getSceneTree(Scene *_theScene);
	void					sendSceneTree();
    void                    sendClearSelection();
	void					onActorOrFrisePicked(ObjectRef _item);
	void					sendUpdateIfInconsistent();
	static void				listScenesAndSubscene(const ITF_LIST<SceneTree*> &_loadedScenes, ITF_SET<Scene*> &_result);
	static bool				isScenePathAlreadyLoaded(const ITF_LIST<SceneTree*> &_loadedScenes, const String &_scenePath);
    static void             add2DActors (PickableList &_actors, Scene *_baseScene);
    static void             fillWithNonSpawnableActors(PickableList &_result, const PickableList &_sourceActors);

    //processing of messages sent by external tool
    void                    received_SelectScene(Blob& _blob);
    void                    received_DeleteItem(Blob& _blob);
    void                    received_RenameItem(Blob& _blob);
    void                    received_DuplicateAndMoveItem(Blob& _blob);
    void                    received_SelectObjects(Blob& _blob);

    ObjectRef				m_actorRef;
	bool					m_mustSendSceneTreeASAP;
    ObjectRefList           m_objectsToSelectASAP;

	SceneTree				*m_lastSceneTree; //Initially NULL, points to the last scene Tree. Il not null, the pointer is valid.
};

} // namespace ITF


#endif // _ITF_SCENEEXPLORERPLUGIN_H_
