#ifndef _ITF_RAY_AIBLOWOFFACTION_H_
#define _ITF_RAY_AIBLOWOFFACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF
{

class PhysComponent;
class Ray_AIBlowOffAction_Template;

//------------------------------------------------------------------------------
// Fly up with a bit of pitch and lateral movement
class Ray_AIBlowOffAction : public AIAction
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIBlowOffAction, AIAction,737550905);
    DECLARE_VALIDATE_ACTION()

public:
    Ray_AIBlowOffAction();
    virtual ~Ray_AIBlowOffAction();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate();
    virtual void update(f32 _dt);

private:
    ITF_INLINE const class Ray_AIBlowOffAction_Template* getTemplate() const;

    f32 m_timer;
    f32 m_noMovementTimer;
    Vec2d m_initialDir;
    Vec2d m_previousPos;
    PhysComponent* m_physComponent;
};

//------------------------------------------------------------------------------
class Ray_AIBlowOffAction_Template : public AIAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIBlowOffAction_Template,AIAction_Template,1849869871);
    DECLARE_SERIALIZE();
    DECLARE_AIACTION_TEMPLATE(Ray_AIBlowOffAction);

public:
    Ray_AIBlowOffAction_Template();
    ~Ray_AIBlowOffAction_Template(){}

    ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }
    ITF_INLINE f32 getForceMultiplier() const { return m_forceMultiplier; }
    ITF_INLINE f32 getLateralFrequency() const { return m_lateralFrequency; }
    ITF_INLINE const Angle& getLateralAmplitude() const { return m_lateralAmplitude; }
    ITF_INLINE f32 getPitchFrequency() const { return m_pitchFrequency; }
    ITF_INLINE const Angle& getPitchAmplitude() const { return m_pitchAmplitude; }
    ITF_INLINE f32 getKeepDirDuration() const { return m_keepDirDuration; }
    ITF_INLINE f32 getKeepDirTransitionDuration() const { return m_keepDirTransitionDuration; }
    ITF_INLINE f32 getPostKeepDirLifetime() const { return m_postKeepDirLifetime; }
    ITF_INLINE f32 getNoMovementLifeTime() const { return m_noMovementLifeTime; }
    ITF_INLINE f32 getNoMovementEpsilon() const { return m_noMovementEpsilon; }

private:
    f32 m_maxSpeed;
    f32 m_forceMultiplier;
    f32 m_lateralFrequency;
    Angle m_lateralAmplitude;
    f32 m_pitchFrequency;
    Angle m_pitchAmplitude;
    f32 m_keepDirDuration;
    f32 m_keepDirTransitionDuration;
    f32 m_postKeepDirLifetime;
    f32 m_noMovementLifeTime;
    f32 m_noMovementEpsilon;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AIBlowOffAction_Template*
Ray_AIBlowOffAction::getTemplate() const
{
    return static_cast<const Ray_AIBlowOffAction_Template*>(m_template);
}

}

#endif //_ITF_RAY_AIBLOWOFFACTION_H_

