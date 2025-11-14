#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TUTORIALCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_TutorialComponent.h"
#endif //_ITF_RAY_TUTORIALCOMPONENT_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PERSISTENTGAMEDATA_H_
#include "rayman/gameplay/Ray_PersistentGameData.h"
#endif //_ITF_RAY_PERSISTENTGAMEDATA_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_TutorialComponent)
BEGIN_SERIALIZATION_CHILD(Ray_TutorialComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_TutorialComponent)
    VALIDATE_COMPONENT_PARAM("nunchukWiiLineId", getTemplate()->getNunchukWiiLineId()!=U32_INVALID, "Line ID mandatory");
    VALIDATE_COMPONENT_PARAM("sidewayWiiLineId", getTemplate()->getSidewayWiiLineId()!=U32_INVALID, "Line ID mandatory");
    VALIDATE_COMPONENT_PARAM("classicWiiLineId", getTemplate()->getClassicWiiLineId()!=U32_INVALID, "Line ID mandatory");
    VALIDATE_COMPONENT_PARAM("ps3LineId", getTemplate()->getPs3LineId()!=U32_INVALID, "Line ID mandatory");
    VALIDATE_COMPONENT_PARAM("vitaLineId", getTemplate()->getVitaLineId()!=U32_INVALID, "Line ID mandatory");
    VALIDATE_COMPONENT_PARAM("ctrLineId", getTemplate()->getCtrLineId()!=U32_INVALID, "Line ID mandatory");
    VALIDATE_COMPONENT_PARAM("x360LineId", getTemplate()->getX360LineId()!=U32_INVALID, "Line ID mandatory");
    VALIDATE_COMPONENT_PARAM("", m_playerDetector, "PlayerDetectorComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_TutorialComponent::Ray_TutorialComponent()
    : Super()
    , m_active(bfalse)
    , m_wasShown(bfalse)
    , m_padCountdown(-1.0f)
    , m_padTypes()
    , m_padTypeIndex(U32_INVALID)
    , m_padType(InputAdapter::Pad_Invalid)
    , m_playerDetector(NULL)
    , m_wasVisible(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_TutorialComponent::~Ray_TutorialComponent()
{
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if ( m_wasVisible )
    {
        hide();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    if ( m_wasVisible )
    {
        hide();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_playerDetector = m_actor->GetComponent<PlayerDetectorComponent>();

    // starts active if required
    m_active = getTemplate()->getStartsActive();

    // register events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

    // register sprint tutorial
    if (getTemplate()->isSprintTutorial())
    {
        RAY_GAMEMANAGER->registerSprintTutorial();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::Update(f32 _dt)
{
    Super::Update(_dt);

    // do nothing if inactive
    if (!m_active)
    {
        hide();
        disableSprintTutorialIfNeeded();
        return;
    }

    // disable sprint tutorial when displayed once
    if (getTemplate()->isSprintTutorial() && isSprintTutorialDisabled())
    {
        hide();
        return;
    }

    // disable tutorials when a ("locking") sequence is playing (RO-10460)
    if (CINEMATICMANAGER->getLockPlayers())
    {
        hide();
        return;
    }

    // show/hide tutorial depending on players inside
    if (m_playerDetector->getActorsInside().size() > 0)
    {
        show(_dt);
    }
    else
    {
        hide();
        disableSprintTutorialIfNeeded();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if (EventTrigger* triggerEvt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        m_active = triggerEvt->getActivated();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::show(f32 _dt)
{
    // update setup: cycle pad type on Wii etc.
    updateSetup(_dt);

    // remember we were shown once
    m_wasShown = btrue;
    m_wasVisible = btrue;
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::hide()
{
    if ( m_wasVisible )
    {
        RAY_GAMEMANAGER->hideTutorialText();
    }

    // reset tutorial
    reset();
    m_wasVisible = bfalse;
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::setup(InputAdapter::PadType _padType)
{
    // store pad type
    m_padType = _padType;

    auto selectFirstValid = [](LocalisationId id, LocalisationId idFallback)
        {
            if (id == LocalisationId::Invalid)
                return idFallback;
            return id;
        };

    // get line ID and config icon to use
    LocalisationId lineId;
    switch(_padType)
    {
    case InputAdapter::Pad_X360:
    {
        lineId = getTemplate()->getX360LineId();
    }
    break;
    case InputAdapter::Pad_PS3:
    {
        lineId = getTemplate()->getPs3LineId();
    }
    break;
    case InputAdapter::Pad_WiiSideWay:
    {
        lineId = getTemplate()->getSidewayWiiLineId();
    }
    break;
    case InputAdapter::Pad_WiiNunchuk:
    {
        lineId = getTemplate()->getNunchukWiiLineId();
    }
    break;
    case InputAdapter::Pad_WiiClassic:
    {
        lineId = getTemplate()->getClassicWiiLineId();
    }
    break;
    case InputAdapter::Pad_Vita:
    {
        lineId = getTemplate()->getVitaLineId();
    }
    break;
    case InputAdapter::Pad_CTR:
    {
        lineId = getTemplate()->getCtrLineId();
    }
    break;
    case InputAdapter::Pad_PS5:
    {
        lineId = selectFirstValid(getTemplate()->getPS5LineId(), getTemplate()->getPs3LineId());
    }
    break;
    case InputAdapter::Pad_NX_Joycon:
    {
        lineId = selectFirstValid(getTemplate()->getNXJoyconLineId(), getTemplate()->getSidewayWiiLineId());
    }
    break;
    case InputAdapter::Pad_NX_Joycon_Dual:
    {
        lineId = selectFirstValid(getTemplate()->getNXJoyconDualLineId(), getTemplate()->getX360LineId());
    }
    break;
    case InputAdapter::Pad_NX_Pro:
    {
        lineId = selectFirstValid(getTemplate()->getNXProLineId(), getTemplate()->getPs3LineId());
    }
    break;
    case InputAdapter::Pad_GenericXBox:
    {
        lineId = selectFirstValid(getTemplate()->getXBoxLineId(), getTemplate()->getX360LineId());
    }
    break;
    case InputAdapter::Pad_Keyboard:
    {
        lineId = getTemplate()->getX360LineId(); // TODO: add keyboard line ID
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled pad type %d", _padType);
    }

    // fill localized text
    RAY_GAMEMANAGER->showTutorialText(lineId,_padType);
}

void Ray_TutorialComponent::setupWii(f32 _dt)
{
    // gather active pad types
    PadTypeList newPadTypes;
    for (u32 i = 0; i<s_maxPlayers; ++i)
    {
        if (Player* player = GAMEMANAGER->getPlayer(i))
        {
            if (player->getActive())
            {
                InputAdapter::PadType padType = INPUT_ADAPTER->getDebugInputPadType(i);
                PadTypeList::iterator check = std::find(newPadTypes.begin(), newPadTypes.end(), padType);
                if (check == newPadTypes.end())
                {
                    newPadTypes.push_back(INPUT_ADAPTER->getDebugInputPadType(i));
                }
            }
        }
    }

    ITF_ASSERT(newPadTypes.size() > 0);
    if (newPadTypes.size() == 0)
    {
        return;
    }

    // check if some pad config changed
    u32 padTypesSize = m_padTypes.size();
    bbool changed = newPadTypes.size() != padTypesSize;
    if (!changed)
    {
        for (u32 i = 0; i<padTypesSize; ++i)
        {
            if (newPadTypes[i] != m_padTypes[i])
            {
                changed = btrue;
                break;
            }
        }
    }

    // store current pad types
    m_padTypes = newPadTypes;

    // reinit on change
    if (changed)
    {
        m_padCountdown = getTemplate()->getPadDisplayDuration();
        m_padTypeIndex = 0;
    }
    // cycle pad type
    else
    {
        m_padCountdown = Max(m_padCountdown - _dt, 0.0f);
        if (m_padCountdown == 0.0f)
        {
            m_padCountdown = getTemplate()->getPadDisplayDuration();
            m_padTypeIndex = (m_padTypeIndex + 1) % m_padTypes.size();
        }
    }

    // Wii setup
    setup(m_padTypes[m_padTypeIndex]);
}

void Ray_TutorialComponent::setupSwitch(f32 _dt)
{
    // gather active pad types
    PadTypeList newPadTypes;
    for (u32 i = 0; i < s_maxPlayers; ++i)
    {
        if (Player* player = GAMEMANAGER->getPlayer(i))
        {
            if (player->getActive())
            {
                InputAdapter::PadType padType = INPUT_ADAPTER->getDebugInputPadType(i);
                PadTypeList::iterator check = std::find(newPadTypes.begin(), newPadTypes.end(), padType);
                if (check == newPadTypes.end())
                {
                    newPadTypes.push_back(INPUT_ADAPTER->getDebugInputPadType(i));
                }
            }
        }
    }

    ITF_ASSERT(newPadTypes.size() > 0);
    if (newPadTypes.size() == 0)
    {
        return;
    }

    // check if some pad config changed
    u32 padTypesSize = m_padTypes.size();
    bbool changed = newPadTypes.size() != padTypesSize;
    if (!changed)
    {
        for (u32 i = 0; i < padTypesSize; ++i)
        {
            if (newPadTypes[i] != m_padTypes[i])
            {
                changed = btrue;
                break;
            }
        }
    }

    // store current pad types
    m_padTypes = newPadTypes;

    // reinit on change
    if (changed)
    {
        m_padCountdown = getTemplate()->getPadDisplayDuration();
        m_padTypeIndex = 0;
    }
    // cycle pad type
    else
    {
        m_padCountdown = Max(m_padCountdown - _dt, 0.0f);
        if (m_padCountdown == 0.0f)
        {
            m_padCountdown = getTemplate()->getPadDisplayDuration();
            m_padTypeIndex = (m_padTypeIndex + 1) % m_padTypes.size();
        }
    }

    // setup
    setup(m_padTypes[m_padTypeIndex]);
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::updateSetup(f32 _dt)
{
    String8 platform = SYSTEM_ADAPTER->getPlatformName();

#ifndef ITF_FINAL
    if (SYSTEM_ADAPTER->hasDebugInput())
    {
        platform = SYSTEM_ADAPTER->getDebugInput();
    }
#endif

    static const String8 s_WII = "WII";
    static const String8 s_PS3 = "PS3";
    static const String8 s_VITA = "VITA";
    static const String8 s_CTR = "CTR";
    static const String8 s_PS5 = "PS5";
    static const String8 s_NX = "NX";
    static const String8 s_OUNCE = "OUNCE";
    static const String8 s_XBoxSeries = "XBOXSERIES";

    // $GB: todo, on PC, handle all the supported controlers

    if (platform == s_WII)
    {
        setupWii(_dt);
    }
    else if (platform == s_PS3)
    {
        // PS3 setup
        setup(InputAdapter::Pad_PS3);
    }
    else if (platform == s_VITA)
    {
        // PS VITA setup
        setup(InputAdapter::Pad_Vita);
    }
    else if (platform == s_CTR)
    {
        // CTR(3DS) setup
        setup(InputAdapter::Pad_CTR);
    }
    else if (platform == s_PS5)
    {
        setup(InputAdapter::Pad_PS5);
    }
    else if (platform == s_NX || platform == s_OUNCE)
    {
        setupSwitch(_dt);
    }
    else if (platform == s_XBoxSeries)
    {
        setup(InputAdapter::Pad_GenericXBox);
    }
    else
    {
        // $GB : todo replace by generic XBox
        setup(InputAdapter::Pad_X360);
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::reset()
{
    m_padCountdown = -1.0f;
    m_padTypes.clear();
    m_padTypeIndex = U32_INVALID;
    m_padType = InputAdapter::Pad_Invalid;
}

//------------------------------------------------------------------------------
bbool Ray_TutorialComponent::isSprintTutorialDisabled() const
{
    ITF_ASSERT(getTemplate()->isSprintTutorial());

    ITF_ASSERT(RAY_GAMEMANAGER);
    if (!RAY_GAMEMANAGER)
    {
        return bfalse;
    }

    Ray_PersistentGameData_Universe* pgd = RAY_GAMEMANAGER->getPersistentGameData();
    ITF_ASSERT(pgd);
    if (!pgd)
    {
        return bfalse;
    }

    return pgd->getSprintTutorialDisabled();
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::disableSprintTutorialIfNeeded()
{
    // not shown yet, skip
    if (!m_wasShown)
    {
        return;
    }

    // not a sprint tutorial, skip
    if (!getTemplate()->isSprintTutorial())
    {
        return;
    }

    ITF_ASSERT(RAY_GAMEMANAGER);
    if (!RAY_GAMEMANAGER)
    {
        return;
    }

    Ray_PersistentGameData_Universe* pgd = RAY_GAMEMANAGER->getPersistentGameData();
    ITF_ASSERT(pgd);
    if (!pgd)
    {
        return;
    }

    pgd->setSprintTutorialDisabled(btrue);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_TutorialComponent_Template)
BEGIN_SERIALIZATION(Ray_TutorialComponent_Template)
    SERIALIZE_MEMBER("startsActive", m_startsActive);
    SERIALIZE_MEMBER("isSprintTutorial", m_isSprintTutorial);
    SERIALIZE_MEMBER("padDisplayDuration", m_padDisplayDuration);
    SERIALIZE_MEMBER("nunchukWiiLineId", m_nunchukWiiLineId);
    SERIALIZE_MEMBER("sidewayWiiLineId", m_sidewayWiiLineId);
    SERIALIZE_MEMBER("classicWiiLineId", m_classicWiiLineId);
    SERIALIZE_MEMBER("ps3LineId", m_ps3LineId);
    SERIALIZE_MEMBER("vitaLineId", m_vitaLineId);
    SERIALIZE_MEMBER("ctrLineId", m_ctrLineId);
    SERIALIZE_MEMBER("x360LineId", m_x360LineId);
    SERIALIZE_MEMBER("ps5LineId", m_ps5LineId);
    SERIALIZE_MEMBER("nxJoyconLineId", m_nxJoyConLineId);
    SERIALIZE_MEMBER("nxJoyConDualLineId", m_nxJoyConDualLineId);
    SERIALIZE_MEMBER("nxProLineId", m_nxProLineId);
    SERIALIZE_MEMBER("xboxLineId", m_xboxLineId);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_TutorialComponent_Template::Ray_TutorialComponent_Template()
: Super()
, m_startsActive(bfalse)
, m_isSprintTutorial(bfalse)
, m_padDisplayDuration(1.0f)
{
}

//------------------------------------------------------------------------------
Ray_TutorialComponent_Template::~Ray_TutorialComponent_Template()
{
}

} // namespace ITF
