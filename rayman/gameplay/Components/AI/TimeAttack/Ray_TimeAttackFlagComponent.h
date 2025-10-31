
#ifndef _ITF_RAY_TIMEATTACKFLAGCOMPONENT_H_
#define _ITF_RAY_TIMEATTACKFLAGCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

class Ray_TimeAttackFlagComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TimeAttackFlagComponent,ActorComponent,1215046538)
    DECLARE_SERIALIZE()

public:

    Ray_TimeAttackFlagComponent();
    virtual ~Ray_TimeAttackFlagComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onActorClearComponents();
    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        Update( f32 _dt );
    virtual void                        onEvent( class Event* _event );

protected:

    const class Ray_TimeAttackFlagComponent_Template* getTemplate() const;

    f32                                 m_timeCounter;
};

class Ray_TimeAttackFlagComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TimeAttackFlagComponent_Template,TemplateActorComponent,2516661283);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_TimeAttackFlagComponent);

public:

    Ray_TimeAttackFlagComponent_Template();
    ~Ray_TimeAttackFlagComponent_Template();

    f32                                 getWaitTime() const { return m_waitTime; }

    
private:

    f32                                 m_waitTime;
};

ITF_INLINE const class Ray_TimeAttackFlagComponent_Template* Ray_TimeAttackFlagComponent::getTemplate() const
{
    return static_cast<const class Ray_TimeAttackFlagComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_TIMEATTACKFLAGCOMPONENT_H_

