#include "precompiled_gameplay.h"

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"

#include <algorithm>
#endif //_ITF_ALIASMANAGER_H_


namespace ITF {

//------------------------------------------------------------------------------
#define PADRUMBLECONFIG_PATH GETPATH_ALIAS("padrumbleconfig")

//------------------------------------------------------------------------------
BEGIN_SERIALIZATION(PadRumble)
    SERIALIZE_MEMBER("name", m_name);
    SERIALIZE_MEMBER("intensity", m_intensity);
    SERIALIZE_MEMBER("duration", m_duration);
END_SERIALIZATION()

//------------------------------------------------------------------------------
PadRumble::PadRumble()
    : m_name()
    , m_intensity(1.0f)
    , m_duration(0.0f)
{
}

//------------------------------------------------------------------------------
PadRumbleManager::PadRumbleManager()
    : m_intensityMultiplier(1.0f)
{
}

//------------------------------------------------------------------------------
PadRumbleManager::~PadRumbleManager()
{
    if (EVENTMANAGER)
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252), this);
    }
    if (TEMPLATEDATABASE)
    {
        const Path configFile(PADRUMBLECONFIG_PATH);
        TEMPLATEDATABASE->removeTemplateClient(configFile.getStringID(), this);
    }
}

//------------------------------------------------------------------------------
void PadRumbleManager::init()
{
    ITF_ASSERT(EVENTMANAGER);
    if (EVENTMANAGER)
    {
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252),this);
    }

    // load pad rumble config file
    const Path configFile(PADRUMBLECONFIG_PATH);
    TEMPLATEDATABASE->addTemplateClient(configFile.getStringID(), this);
    const PadRumbleManager_Template* config = TEMPLATEDATABASE->getTemplate<PadRumbleManager_Template>(this, configFile);
    if (!config)
    {
        ITF_FATAL_ERROR("Couldn't load pad rumble config: %s", configFile.getString8().cStr());
    }
    m_rumbles = config->getRumbles(); // copy
}

//------------------------------------------------------------------------------
void PadRumbleManager::onEvent(Event* _event)
{
#ifdef ITF_SUPPORT_HOTRELOAD
    if (EventSingletonConfigChanged* configChanged = _event->DynamicCast<EventSingletonConfigChanged>(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252)))
    {
        // reload camera shake config file
        String relativeFile = configChanged->getAbsoluteFilename();
        FilePath::transformToRelative(relativeFile);
        Path relativeFilePath(relativeFile);
        const Path configFile(PADRUMBLECONFIG_PATH);
        if (relativeFilePath == configFile)
        {
            const PadRumbleManager_Template* config = TEMPLATEDATABASE->reloadTemplate<PadRumbleManager_Template>(configFile);
            if (!config)
            {
                ITF_FATAL_ERROR("Couldn't reload pad rumble config: %s", configFile.getString8().cStr());
            }
            m_rumbles = config->getRumbles(); // copy
        }
    }
#endif //ITF_SUPPORT_HOTRELOAD
}

//------------------------------------------------------------------------------
void PadRumbleManager::startRumble(const StringID& _name, u32 _numPad)
{
    // guard
    ITF_ASSERT(INPUT_ADAPTER);
    if (!INPUT_ADAPTER)
    {
        return;
    }

    // search for rumble
    ITF_VECTOR<PadRumble>::const_iterator it; 
    for (it = m_rumbles.begin(); it != m_rumbles.end(); ++it)
    {
        if (it->getName() == _name)
        {
            break;
        }
    }

    // warn and give up if not found
    if (it == m_rumbles.end())
    {
        ITF_WARNING(NULL, bfalse, "Pad rumble %s undefined", _name.getDebugString());
        return;
    }

    // start rumble on given pad
    const f32 baseIntensity = it->getIntensity();
    const f32 scaledIntensity = std::max(0.0f, std::min(1.0f, baseIntensity * m_intensityMultiplier));

    INPUT_ADAPTER->startRumble(
        _numPad,
        it->getDuration(),
        scaledIntensity,
        scaledIntensity
        );
}

//------------------------------------------------------------------------------
void PadRumbleManager::stopRumble(u32 _numPad)
{
    // guard
    ITF_ASSERT(INPUT_ADAPTER);
    if (!INPUT_ADAPTER)
    {
        return;
    }

    // stop rumble on given pad
    INPUT_ADAPTER->stopRumble(_numPad);
}

//------------------------------------------------------------------------------
void PadRumbleManager::setIntensityMultiplier(f32 multiplier)
{
    m_intensityMultiplier = std::max(0.0f, std::min(1.0f, multiplier));
}

//------------------------------------------------------------------------------
PadRumbleManager* PadRumbleManager::s_instance = NULL;

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(PadRumbleManager_Template)
BEGIN_SERIALIZATION(PadRumbleManager_Template)
    SERIALIZE_CONTAINER_OBJECT("rumbles", m_rumbles);
END_SERIALIZATION()

} // namespace ITF
