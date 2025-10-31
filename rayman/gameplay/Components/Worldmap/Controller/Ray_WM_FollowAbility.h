#ifndef _ITF_RAY_WM_FOLLOWABILITY_H_
#define _ITF_RAY_WM_FOLLOWABILITY_H_

#ifndef _ITF_RAY_WM_BASEABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_BaseAbility.h"
#endif //_ITF_RAY_WM_BASEABILITY_H_

namespace ITF {

class Ray_WM_FollowAbility_Template;
class Ray_WM_LeadAbility;

//------------------------------------------------------------------------------
/// Follows the lead player on the world map with a few frames of delay
class Ray_WM_FollowAbility : public Ray_WM_BaseAbility, public IInputListener
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_FollowAbility, Ray_WM_BaseAbility,2791722937)
    DECLARE_SERIALIZE();
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_WM_FollowAbility();
    virtual ~Ray_WM_FollowAbility();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(
        Pickable::HotReloadType _hotReload,
        const Ray_WM_BaseAbility_Template* _template,
        Ray_WM_ControllerComponent* _controller,
        Actor* _actor);
    virtual void onBecomeActive(u32 _playerIndex);
    virtual void onPlayerIndexChange(u32 _newIndex);
    virtual void onBecomeInactive();
    virtual void update(f32 _dt);
    
    virtual void Receive(u32 _deviceID /* player */, f32 _axis, const StringID& _action);

private:
    const Ray_WM_FollowAbility_Template* getTemplate() const;
    
    void applyInput();

    Ray_WM_LeadAbility* getLeadAbility(const ActorRef& _ref);
    bbool findLead();
    void updateMove(f32 _dt);

    ActorRef m_leadRef;
    bbool m_listenerRegistered;
    bbool m_listenerLeave;
};

//------------------------------------------------------------------------------
class Ray_WM_FollowAbility_Template : public Ray_WM_BaseAbility_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_FollowAbility_Template, Ray_WM_BaseAbility_Template,2270475256);
    DECLARE_SERIALIZE();

public:
    Ray_WM_FollowAbility_Template();
    ~Ray_WM_FollowAbility_Template();
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_WM_FollowAbility_Template*
Ray_WM_FollowAbility::getTemplate() const
{
    return static_cast<const Ray_WM_FollowAbility_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_WM_FOLLOWABILITY_H_
