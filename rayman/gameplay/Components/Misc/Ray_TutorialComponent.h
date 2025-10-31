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

    void show(f32 _dt);
    void hide();
    void setup(InputAdapter::PadType _padType);
    void setupWii(f32 _dt);
    void setupSwitch(f32 _dt);

    void updateSetup(f32 _dt);
    void reset();

    UIMenu* getUIMenu();

    bbool isSprintTutorialDisabled() const;
    void disableSprintTutorialIfNeeded();

    bbool m_active;
    bbool m_wasShown;
    bbool m_wasVisible;

    f32 m_padCountdown;
    typedef ITF_VECTOR<InputAdapter::PadType> PadTypeList;
    PadTypeList m_padTypes;
    u32 m_padTypeIndex;
    InputAdapter::PadType m_padType;

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
    ITF_INLINE f32 getPadDisplayDuration() const { return m_padDisplayDuration; }
    ITF_INLINE LocalisationId getNunchukWiiLineId() const { return m_nunchukWiiLineId; }
    ITF_INLINE LocalisationId getSidewayWiiLineId() const { return m_sidewayWiiLineId; }
    ITF_INLINE LocalisationId getClassicWiiLineId() const { return m_classicWiiLineId; }
    ITF_INLINE LocalisationId getPs3LineId() const { return m_ps3LineId; }
    ITF_INLINE LocalisationId getVitaLineId() const { return m_vitaLineId; }
    ITF_INLINE LocalisationId getCtrLineId() const { return m_ctrLineId; }
    ITF_INLINE LocalisationId getX360LineId() const { return m_x360LineId; }
    ITF_INLINE LocalisationId getPS5LineId() const { return m_ps5LineId; }
    ITF_INLINE LocalisationId getNXJoyconLineId() const { return m_nxJoyConLineId; }
    ITF_INLINE LocalisationId getNXJoyconDualLineId() const { return m_nxJoyConDualLineId; }
    ITF_INLINE LocalisationId getNXProLineId() const { return m_nxProLineId; }
    ITF_INLINE LocalisationId getXBoxLineId() const { return m_xboxLineId; }

private:
    bbool m_startsActive;
    bbool m_isSprintTutorial;
    f32 m_padDisplayDuration;
    LocalisationId m_nunchukWiiLineId;
    LocalisationId m_sidewayWiiLineId;
    LocalisationId m_classicWiiLineId;
    LocalisationId m_ps3LineId;
    LocalisationId m_vitaLineId;
    LocalisationId m_ctrLineId;
    LocalisationId m_x360LineId;
    LocalisationId m_ps5LineId;
    LocalisationId m_nxJoyConLineId;
    LocalisationId m_nxJoyConDualLineId;
    LocalisationId m_nxProLineId;
    LocalisationId m_xboxLineId;
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
