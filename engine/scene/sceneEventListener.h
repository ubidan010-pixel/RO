#ifndef _ITF_SCENEEVENT_LISTENER_H_
#define _ITF_SCENEEVENT_LISTENER_H_



namespace ITF
{
    class Scene;
    /*
    WARNING: All interface classes must have only pure virtual methods and no variable
    */
    class Interface_SceneEventsListener
    {
    public:
        virtual void onSceneAsyncLoadFinished(const Scene* /*_pScene*/) = 0;
        virtual void onScenePreDestroy(Scene* /*_pScene*/) = 0;
        virtual void onDeleteScene(const Scene* /*_pScene*/) = 0;
        virtual void onObjectAdded(const Scene* /*_pScene*/, BaseObject* /*_pObj*/) = 0;
        virtual void onObjectRemoved(const Scene* /*_pScene*/, BaseObject* /*_pObj*/, bbool /*_delete*/) = 0;
        virtual void onPostSceneActivation(const Scene* /*_pScene*/, bbool /*_bActive*/) = 0;
    };

}

#endif //_ITF_SCENEEVENT_LISTENER_H_