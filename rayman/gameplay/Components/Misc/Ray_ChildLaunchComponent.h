#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#define _ITF_RAY_CHILDLAUNCHCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif // _ITF_SPAWNER_H_

namespace ITF
{

class AnimLightComponent;
class EventOnLink;
class LinkComponent;
class Ray_ChildLaunchComponent_Template;

//------------------------------------------------------------------------------
// Disable linked children and launch them on demand, one by one. The launcher
// is set as the sender of the Ray_EventChildLaunch, allowing using it's
// position as a start position (see Ray_BubblePrizePlatformComponent &
// Ray_GeyserComponent_Template for examples)
class Ray_ChildLaunchComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ChildLaunchComponent, ActorComponent,3515647608)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_ChildLaunchComponent();
    virtual ~Ray_ChildLaunchComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onBecomeActive();
    virtual void onCheckpointLoaded();
    virtual void Update(f32 _dt);
    virtual void onEvent(Event* _event);
    
#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(class ActorDrawEditInterface* /*drawInterface*/, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

    bbool canLaunch() const;
    void launchAll();
    void launch();
    
    ITF_INLINE u32 getLaunchCount() { return (m_finalIndex == U32_INVALID) ? m_childrenList.size() : m_finalIndex+1; }
    ITF_INLINE u32 getNextLaunchIndex() { return m_nextLaunchIndex; }

    ITF_INLINE bbool getHintFxRuntimeDisabled() const { return m_hintFxRuntimeDisabled; }
    ITF_INLINE void setHintFxRuntimeDisabled(bbool _val) { m_hintFxRuntimeDisabled = _val; }
protected:
    u32 m_nextLaunchIndex;
private:
    typedef ITF_VECTOR<ObjectPath> ChildrenList;

    ITF_INLINE const Ray_ChildLaunchComponent_Template* getTemplate() const;

    Vec3d getLaunchPos() const;
    void launchOne();

    virtual void processLink(EventOnLink* link);
    virtual void collectChildren();

    bbool needsHintFx() const;
    void updateHintFx(f32 _dt);

    ChildrenList m_childrenList;
    bbool m_childrenCollected;
    u32 m_finalIndex;

    bbool m_hintFxEnabled;
    bbool m_hintFxRuntimeDisabled;
    SpawneeGenerator m_hintFxGenerator;
    ActorRef m_hintFxActorRef;

    LinkComponent* m_linkComponent;
    AnimLightComponent* m_animLightComponent;
};

//------------------------------------------------------------------------------
class Ray_ChildLaunchComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ChildLaunchComponent_Template,TemplateActorComponent,3683817819);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ChildLaunchComponent);

public:
    Ray_ChildLaunchComponent_Template();
    virtual ~Ray_ChildLaunchComponent_Template();
    
    ITF_INLINE bbool getDisableAfterLaunch() const { return m_disableAfterLaunch; }
    ITF_INLINE const StringID& getLaunchPolyline() const { return m_launchPolyline; }
    ITF_INLINE const Path& getHintFxPath() const { return m_hintFxPath; }
    ITF_INLINE bbool getHintFxInstantDestroy() const { return m_hintFxInstantDestroy; }

private:
    bbool m_disableAfterLaunch;
    StringID m_launchPolyline;
    Path m_hintFxPath;
    bbool m_hintFxInstantDestroy;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_ChildLaunchComponent_Template*
Ray_ChildLaunchComponent::getTemplate() const
{
    return static_cast<const Ray_ChildLaunchComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_
