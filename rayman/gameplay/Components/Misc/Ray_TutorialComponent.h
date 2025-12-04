#ifndef _ITF_RAY_TUTORIALCOMPONENT_H_
#define _ITF_RAY_TUTORIALCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

namespace ITF {

class PlayerDetectorComponent;
class Ray_TutorialComponent_Template;
class UIComponent;

//------------------------------------------------------------------------------
class Ray_TutorialComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TutorialComponent, ActorComponent,2778069029)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_TutorialComponent();
    virtual ~Ray_TutorialComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorClearComponents();
    virtual void onBecomeInactive();
    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void Update(f32 _dt);
    virtual void onEvent(Event * _event);

private:

    ITF_INLINE const Ray_TutorialComponent_Template* getTemplate() const;

    void show();
    void hide();

    bbool isSprintTutorialDisabled() const;
    void disableSprintTutorialIfNeeded();

    bbool m_active;
    bbool m_wasShown;
    bbool m_wasVisible;

    PlayerDetectorComponent* m_playerDetector;
};

//------------------------------------------------------------------------------
class Ray_TutorialComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TutorialComponent_Template, TemplateActorComponent,3930255658);
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_TutorialComponent);
    DECLARE_SERIALIZE();

public:
    Ray_TutorialComponent_Template();
    virtual ~Ray_TutorialComponent_Template();

    ITF_INLINE bbool getStartsActive() const { return m_startsActive; }
    ITF_INLINE bbool isSprintTutorial() const { return m_isSprintTutorial; }
    ITF_INLINE LocalisationId getLineId() const { return m_lineId; }

private:
    bbool m_startsActive;
    bbool m_isSprintTutorial;
    LocalisationId m_lineId;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_TutorialComponent_Template*
Ray_TutorialComponent::getTemplate() const
{
    return static_cast<const Ray_TutorialComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_TUTORIALCOMPONENT_H_
