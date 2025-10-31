#ifndef _ITF_RAY_WM_BASEABILITY_H_
#define _ITF_RAY_WM_BASEABILITY_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

namespace ITF {

class Ray_WM_BaseAbility_Template;
class Ray_WM_ControllerComponent;

//------------------------------------------------------------------------------
/// To move a world map player in different ways
class Ray_WM_BaseAbility : public IRTTIObject
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_WM_BaseAbility, IRTTIObject,746535245)
    DECLARE_VALIDATE_OBJECT()

public:
    Ray_WM_BaseAbility();
    virtual ~Ray_WM_BaseAbility();

    virtual void onActorLoaded(
        Pickable::HotReloadType _hotReload,
        const Ray_WM_BaseAbility_Template* _template,
        Ray_WM_ControllerComponent* _controller,
        Actor* _actor);
    virtual void onBecomeActive(u32 _playerIndex);
    virtual void onPlayerIndexChange(u32 _newIndex);
    virtual void onBecomeInactive();
    virtual void update(f32 /*_dt*/)  {}
    
    ITF_INLINE bbool isActive() const { return m_active; }

#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(class ActorDrawEditInterface* _drawInterface, u32 _flags) const {}
#endif

protected:
    const Ray_WM_BaseAbility_Template* getTemplate() const;

    const Ray_WM_BaseAbility_Template* m_template;
    Ray_WM_ControllerComponent* m_controller;
    Actor* m_actor;
    bbool m_active;
    u32 m_playerIndex;
};

//------------------------------------------------------------------------------
class Ray_WM_BaseAbility_Template : public IRTTIObject
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_BaseAbility_Template, IRTTIObject,3339370241);
    DECLARE_SERIALIZE_VIRTUAL();

public:
    Ray_WM_BaseAbility_Template();
    virtual ~Ray_WM_BaseAbility_Template();
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_WM_BaseAbility_Template*
Ray_WM_BaseAbility::getTemplate() const
{
    return static_cast<const Ray_WM_BaseAbility_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_WM_BASEABILITY_H_
