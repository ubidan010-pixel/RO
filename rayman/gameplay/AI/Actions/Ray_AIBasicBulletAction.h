#ifndef _ITF_RAY_AIBASICBULLETACTION_H_
#define _ITF_RAY_AIBASICBULLETACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_RAY_BASICBULLET_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_BasicBullet.h"
#endif //_ITF_RAY_BASICBULLET_H_

namespace ITF
{

class Ray_AIBasicBulletAction_Template;

//------------------------------------------------------------------------------
/// Wrapper action around the Ray_BasicBullet helper class
class Ray_AIBasicBulletAction : public AIAction
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIBasicBulletAction, AIAction,3677683072);
    DECLARE_VALIDATE_ACTION()

public:

    Ray_AIBasicBulletAction();
    virtual ~Ray_AIBasicBulletAction();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate();
    virtual void onDeactivate();

    virtual void update(f32 _dt);
    virtual void onEvent(Event* _event);

    ITF_INLINE void setMoveDir(const Vec2d & _dir) { m_bulletAbility.setMoveDir(_dir); }
    ITF_INLINE void setMoveSpeed(const f32 & _speed) { m_bulletAbility.setMoveSpeed(_speed); }

private:
    ITF_INLINE const Ray_AIBasicBulletAction_Template* getTemplate() const;

    Ray_BasicBullet m_bulletAbility;
};

//------------------------------------------------------------------------------
class Ray_AIBasicBulletAction_Template : public AIAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIBasicBulletAction_Template,AIAction_Template,2719645349);
    DECLARE_SERIALIZE();
    DECLARE_AIACTION_TEMPLATE(Ray_AIBasicBulletAction);

public:
    Ray_AIBasicBulletAction_Template();
    ~Ray_AIBasicBulletAction_Template() {}

    ITF_INLINE const Ray_BasicBullet_Template*	getBasicBulletTemplate() const { return &m_basicBullet; }
    ITF_INLINE bbool getHasOwner() const { return m_hasOwner; }

private :
    Ray_BasicBullet_Template m_basicBullet;
    bbool m_hasOwner;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AIBasicBulletAction_Template*
Ray_AIBasicBulletAction::getTemplate() const
{
    return static_cast<const Ray_AIBasicBulletAction_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_AIBASICBULLETACTION_H_

