#ifndef _ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_
#define _ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{

class AnimatedComponent;
class Ray_DarktoonTrapHoleComponent_Template;

//------------------------------------------------------------------------------
// Hole for darktoon traps: just sets an anim input, depending on the trap state
class Ray_DarktoonTrapHoleComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DarktoonTrapHoleComponent, ActorComponent,3015066619)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_DarktoonTrapHoleComponent();
    ~Ray_DarktoonTrapHoleComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void updateAnimInput();
    void setOpenHole(bbool val) { m_openHole = val; }

private:
    const Ray_DarktoonTrapHoleComponent_Template* getTemplate() const;

    bbool m_openHole;
    AnimatedComponent* m_animatedComponent;
};

//------------------------------------------------------------------------------
class Ray_DarktoonTrapHoleComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DarktoonTrapHoleComponent_Template,TemplateActorComponent,1117493895);
    DECLARE_SERIALIZE()
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DarktoonTrapHoleComponent);

public:
    Ray_DarktoonTrapHoleComponent_Template();
    ~Ray_DarktoonTrapHoleComponent_Template();

    const StringID& getAnim() const { return m_anim; }

private:
    StringID m_anim;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_DarktoonTrapHoleComponent_Template*
Ray_DarktoonTrapHoleComponent::getTemplate() const
{
    return static_cast<const Ray_DarktoonTrapHoleComponent_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_
