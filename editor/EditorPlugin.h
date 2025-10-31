#ifndef _ITF_EDITOR_PLUGIN_H_
#define _ITF_EDITOR_PLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_
#include "editor/PickingShape/PickingShapeData_RotationScale.h"
#endif //_ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_

namespace ITF
{
    class Scene;
    class PickingShape;
    class PickingShapePluginData;

    class EditorPlugin : public Plugin
    {
    public:
        static const char* getPluginName() { return  "Editor_Plugin"; }

        EditorPlugin();
        virtual ~EditorPlugin();

        virtual PickingShapePluginData* createPickingShapeData(PickingShape *);
        
        virtual void onObjectCreated(BaseObject* _pObject);
        virtual void onObjectDeleted(BaseObject* _pObject);
        virtual void onObjectChanged(BaseObject* _pObject);
        virtual void onPickableRenamed(Pickable* _pObj, const String8& _previousUserFriendly);
        virtual void onPickableSceneChanged(Pickable* _pPickable, Scene* _src, Scene* _dst);

    private:
        void fullInstancesReload(const Scene* _modifiedScene)const;
        void findInstances(Pickable* _object, PickableList& results)const;
        void findInstanceScenes(const Scene* _scene, SafeArray<Scene*>& results)const;
        void updateNewSubSceneInstanceFromExistingInstance(SubSceneActor* pSSA)const;

        //////////////////////////////////////////////////////////////////////////
        ///Internal class for picking shape of actor
        class PickingShapeData : public PickingShapeData_RotationScale
        {
        public:
            //////////////////////////////////////////////////////////////////////////
            ///Construction
            PickingShapeData(PickingShape*, Plugin *);

            //////////////////////////////////////////////////////////////////////////
            ///Update component
            virtual void        update();

            //////////////////////////////////////////////////////////////////////////
            ///Draw component
            virtual void        draw();

            //////////////////////////////////////////////////////////////////////////
            /// Scale the actor
            virtual void        scale(const Vec2d& _newScale);

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// handle Mouse Pressed
            /// @param _mousePos position of mouse
            virtual void        handleMousePressed(const Vec2d & _mousePos);

        private:
            virtual void    preChange();
            virtual void    postChange();
            virtual void    finalChange();
        };
    };

} // namespace ITF


#endif // _ITF_EDITOR_PLUGIN_H_
