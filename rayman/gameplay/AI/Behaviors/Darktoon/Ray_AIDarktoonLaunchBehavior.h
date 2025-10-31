#ifndef _ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_
#define _ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{

class AIBallisticsAction;
class AIBallisticsAction_Template;
class PhantomComponent;

//------------------------------------------------------------------------------
class Ray_AIDarktoonLaunchBehavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonLaunchBehavior, AIBehavior,1382927332);
    DECLARE_VALIDATE_BEHAVIOR()

public:
    Ray_AIDarktoonLaunchBehavior();
    ~Ray_AIDarktoonLaunchBehavior();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void update(f32 _dt);
    virtual void onActionFinished();
    
    ITF_INLINE bbool isLanding() const { return m_currentAction == m_landing; }

    ITF_INLINE void setOrigin(const Vec3d& _val) { m_origin = _val; }
    ITF_INLINE void setTarget(const Vec3d& _val) { m_target = _val; }

private:
    ITF_INLINE const class Ray_AIDarktoonLaunchBehavior_Template * getTemplate() const;

    bbool m_started;
    Vec3d m_origin;
    Vec3d m_target;
    AIBallisticsAction* m_ballistics;
    AIAction* m_landing;
    bbool m_phantomWasDisabled;
    PhantomComponent* m_phantomComponent;
};

//------------------------------------------------------------------------------
class Ray_AIDarktoonLaunchBehavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonLaunchBehavior_Template,TemplateAIBehavior,547595480);
    DECLARE_SERIALIZE();
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIDarktoonLaunchBehavior);

public:
    Ray_AIDarktoonLaunchBehavior_Template();
    ~Ray_AIDarktoonLaunchBehavior_Template();

    ITF_INLINE const AIBallisticsAction_Template * getBallistics() const { return m_ballistics; }
    ITF_INLINE const AIAction_Template * getLanding() const { return m_landing; }

private:
    AIBallisticsAction_Template* m_ballistics;
    AIAction_Template* m_landing;
};

ITF_INLINE const class Ray_AIDarktoonLaunchBehavior_Template * Ray_AIDarktoonLaunchBehavior::getTemplate() const
{
    return static_cast<const class Ray_AIDarktoonLaunchBehavior_Template *>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_
