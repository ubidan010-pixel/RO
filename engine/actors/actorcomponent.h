#ifndef _ITF_ACTORCOMPONENT_H_
#define _ITF_ACTORCOMPONENT_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif //_ITF_EVENTLISTENER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif // _ITF_PICKABLE_H_

#ifndef _ITF_IMGUIPROPERTYSYSTEM_H_
#include "engine/AdaptersInterfaces/ImGui/ImGuiPropertySystem.h"
#endif //_ITF_IMGUIPROPERTYSYSTEM_H_

struct lua_State;

#define DECLARE_VALIDATE_COMPONENT() virtual void validate(bbool &_isComponentValidated);
#define BEGIN_VALIDATE_COMPONENT(_classname_) void _classname_::validate(bbool &_isComponentValidated) {Super::validate(_isComponentValidated);
#define END_VALIDATE_COMPONENT() }

namespace ITF
{

class Event;
class GhostRecorderInfo;
class GhostPlayerInfo;

class ActorComponent : public IRTTIObject, public IEventListener, public IInspectable
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(ActorComponent,IRTTIObject,2669192659)

public:
    DECLARE_SERIALIZE_VIRTUAL()

    ActorComponent();
    virtual ~ActorComponent();

#ifdef ITF_SUPPORT_EDITOR
    enum DrawEditFlags
    {   // powers of 2 only
        DrawEditFlag_Nothing    = 0,
        DrawEditFlag_Collisions = 1,
        DrawEditFlag_All = 0xffffffff
    };
#endif // ITF_SUPPORT_EDITOR

    virtual bbool       needsUpdate() const = 0;
    virtual bbool       needsDraw() const = 0;
    virtual bbool       needsDraw2D() const = 0;
    virtual bbool       keepAlive() { return bfalse; } // Must return true for component life dependent

    virtual void        Update( f32 _deltaTime ) {}
    virtual void        Draw() {}
    virtual void        Draw2D() {}
#ifdef ITF_SUPPORT_EDITOR
    virtual void        drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const {}
    virtual void        onEditorMove(bbool _modifyInitialPos = btrue) {}
    virtual void        onEditorCreated( class Actor* _original ) {}  // the actor has just been created in the editor (drag'n'drop from a lua, ctrl+drag clone, or copy-paste clone)
#endif // ITF_SUPPORT_EDITOR
    virtual void        onChildChanged (ObjectRef _changedObject, PickableEditorChangeType _type) {};
    virtual void        onParentChanged (ObjectRef _changedObject, PickableEditorChangeType _type) {};
    virtual void        onForceMove() {}
    virtual void        onActorAddedToScene(Scene* /*_pScene*/) {};
    virtual void        onActorRemovedFromScene(Scene* /*_pScene*/, bbool /*_forDeletion -  if false, could be for changing its scene*/) {};
    virtual void        onSceneActive() {}
    virtual void        onSceneInactive() {}
    virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/) { /*Remove the assert while we do the transition ITF_ASSERT(m_template);*/ }
    virtual void        onCheckpointLoaded() {}
    virtual void        onBecomeActive() {}
    virtual void        onBecomeInactive() {}
    virtual void        onDepthChanged( f32 _oldZ, f32 _newZ ) {}
    virtual void        onActorClearComponents() {}
    virtual bbool       isActorDestructionValidated() { return btrue; }              // each component can unvalidate the actor destruction request by returning false, but it MUST BE controlled !!!

    virtual void        onUpdatePos() {}

    class World*        getWorld() const;

    class Actor*        GetActor() const { return m_actor; }
    void                SetActor( class Actor* _actor ) { ITF_ASSERT(m_actor==NULL || m_actor == _actor); m_actor = _actor; }

    const class TemplateActorComponent* getTemplate() const { return m_template; }
    void                                setTemplate( const class TemplateActorComponent* _template );

    virtual void        onEvent( Event * _event) {}

            void        pause();
            void        unpause();
    ITF_INLINE bbool    isPaused() const    { return m_pause; }
    bbool               isActive() const;

    /*
    *   callback invoked when the owner actor unloads its resources
    */
    virtual void        onUnloadResources() {};
    virtual void        onResourceLoaded()  {};     // all resources are loaded; components that have resources should process them now
    virtual void        onResourceReady()   {};     // all resources are loaded and processed; components that depend on others can use them now

    virtual void        validate(bbool &_isComponentValidated) {_isComponentValidated = btrue;}
    virtual void        regenerateObjectId() {};

protected:

    class Actor*                        m_actor;
    const class TemplateActorComponent* m_template;
    bbool                               m_pause;

#ifdef ITF_SUPPORT_IMGUI
public:
    const char* inspectableName() const override { return "Actor Component"; }
protected:
    void buildProperties(PropertyBag& bag) override {}
#endif //ITF_SUPPORT_IMGUI
};

}

#endif // _ITF_ACTORCOMPONENT_H_
