#ifndef _ITF_LINKMANAGER_H_
#define _ITF_LINKMANAGER_H_

namespace ITF
{
    #define LINKMANAGER LinkManager::get()

    typedef std::pair<ObjectRef, ObjectRefList> ObjectMultimapPair;
    typedef ITF_MAP<ObjectRef, ObjectRefList> ObjectMultimap;

    class LinkManager final : public Interface_SceneEventsListener
    {
        static LinkManager*     s_instance;
    public:

        static void             create()    { s_instance = newAlloc(mId_Gameplay, LinkManager()); }
        static void             destroy();
        static LinkManager*     get()       { return s_instance; }  //ITF_ASSERT_MSG(s_instance, "LinkManager not ready, be sure to call LinkManager::create() !");


        LinkManager();
        ~LinkManager();

        void            init();

        void            addLink( const ObjectRef& _parent, const ObjectRef& _child );
        void            removeLink( const ObjectRef& _parent, const ObjectRef& _child );

        void            removeActor( const ObjectRef& _actor );
        void            removeParentLinks( const ObjectRef& _actor );
        void            removeChildLinks( const ObjectRef& _actor );

        void            onPickableSceneChange(Pickable* _pObj, Scene* _src, Scene *_dst, PickableList& _changedObjects);

        /***    Interface_SceneEventsListener       ***/
        // used to clear links on scene change

        virtual void onSceneAsyncLoadFinished(const Scene* /*_pScene*/) {}
        virtual void onScenePreDestroy(Scene* _scene);
        virtual void onDeleteScene(const Scene* /*_pScene*/) {}
        virtual void onObjectAdded(const Scene* /*_pScene*/, BaseObject* /*_pObj*/) {}
        virtual void onObjectRemoved(const Scene* /*_pScene*/, BaseObject* /*_pObj*/, bbool /*_delete*/) {}
        virtual void onPostSceneActivation(const Scene* /*_pScene*/, bbool /*_bActive*/) {}

        /***    end Interface_SceneEventsListener       ***/


        ObjectRefList*   getParents( const ObjectRef& _child );
        ObjectRefList*   getChildren( const ObjectRef& _parent );

        ITF_INLINE ObjectMultimap getParentsToChildren() const { return m_parentsToChildren; }
        ITF_INLINE ObjectMultimap getChildrenToParents() const { return m_childrenToParents; }


        void            getFlatTree( const ObjectRef& _parent, ObjectMultimap& _tree );

        class Node;
        typedef ITF_VECTOR<Node> NodeList;
        class Node
        {
        public:
            ObjectRef   m_object;
            NodeList    m_childNodes;
        };

        typedef FixedArray<ObjectRef, 128> ProcessedNodeList;
        void            getTree( const ObjectRef& _root, Node& _tree );
        void            getTree( const ObjectRef& _root, Node& _tree, ProcessedNodeList& _processedNodes );

#ifdef ITF_SUPPORT_DEBUGFEATURE
        // debug draw
        enum DrawDebug_t : u64
        {
            DrawDebug_Off = 0,
            DrawDebug_Selection_BothWays,
            DrawDebug_Selection_ForwardOnly,
            DrawDebug_All,
            DrawDebug_Count,
        };
        DrawDebug_t m_drawDebug;
        void cycleDrawDebug() { m_drawDebug = DrawDebug_t((m_drawDebug + 1) % DrawDebug_Count); }
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:

        void            removeParentToChildrenLink( const ObjectRef& _parent, const ObjectRef& _child );
        void            removeChildToParentsLink( const ObjectRef& _parent, const ObjectRef& _child );

        ObjectMultimap   m_parentsToChildren;
        ObjectMultimap   m_childrenToParents;
    };
}

#endif //_ITF_LINKMANAGER_H_

