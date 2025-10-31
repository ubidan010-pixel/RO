#ifndef _ITF_SUBSCENECOMPONENT_H_
#define _ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_


#ifndef _ITF_OBJECTGROUP_H_
#include "engine/scene/ObjectGroup.h"
#endif //_ITF_OBJECTGROUP_H_

namespace ITF
{
    class Scene;
    class SubSceneActor;

    class SubSceneComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SubSceneComponent,ActorComponent,393783807);

    public:
        DECLARE_SERIALIZE()

        SubSceneComponent();
        virtual ~SubSceneComponent();
        void            clear();

        virtual bbool   needsUpdate() const   { return btrue; }
        virtual bbool   needsDraw()   const   { return bfalse; }
        virtual bbool   needsDraw2D() const   { return bfalse; }

        // If there is a subscene wait for it, so full logic is loaded
        virtual bbool   isPhysicalReady() const;

        virtual void    Update( f32 _deltaTime );

        void            setExternalSubScene(const String& _relativePath, SubSceneActor* _pSSA_Holder);
        const Path&     getSubScenePath()const { return m_SceneRelativePath; }
        Scene*          getSubScene() const { return m_pScene; }

        void            setLoadedSubScene(Scene* _pScene);

        virtual void    onSceneActive();
        virtual void    onSceneInactive();
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

#ifdef ITF_SUPPORT_EDITOR
        virtual void    onEditorCreated( Actor* _original/* = NULL*/);
        virtual void    onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        void            storeCurrentToInitial();
        void            rebuildAABB();
        void            growInternalRelativeAABBFromAbsolute(const AABB &_aabb, f32 _z);


    private:
      //  ObjectGroup         m_updateGroups[ObjectGroup::GroupIndex_Max];


        Scene*          m_pScene;
        Path            m_SceneRelativePath;
        AABB            m_relativeAABB;
    };

    class SubSceneComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SubSceneComponent_Template,TemplateActorComponent,3856437678);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SubSceneComponent);

    public:

        SubSceneComponent_Template() {}
        ~SubSceneComponent_Template() {}

    private:

    };

}

#endif // _ITF_GRAPHICCOMPONENT_H_

