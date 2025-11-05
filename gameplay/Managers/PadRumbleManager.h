#ifndef _ITF_PADRUMBLEMANAGER_H_
#define _ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif //_ITF_EVENTLISTENER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

namespace ITF {

#define PADRUMBLEMANAGER PadRumbleManager::get()

//------------------------------------------------------------------------------
/// Pad rumble definition
class PadRumble
{
public:
    DECLARE_SERIALIZE()

    PadRumble();

    ITF_INLINE const StringID& getName() const { return m_name; }
    ITF_INLINE f32 getIntensity() const { return m_intensity; }
    ITF_INLINE f32 getDuration() const { return m_duration; }

private:
    // serialized
    StringID m_name;
    f32 m_intensity;
    f32 m_duration;
};

//------------------------------------------------------------------------------
/// To start & stop pad rumbles
class PadRumbleManager final : public IEventListener
{
    static PadRumbleManager* s_instance;

public:
    static void create() { s_instance = newAlloc(mId_Singleton, PadRumbleManager()); }
    static void destroy() { SF_DEL(s_instance); }
    static PadRumbleManager* get() { return s_instance; } 

    PadRumbleManager();
    ~PadRumbleManager();

    void init();
    void onEvent(Event* _event);
    
    /// start rumble by name, one pad
    void startRumble(const StringID& _name, u32 _numPad);

    /// stop rumble, one pad
    void stopRumble(u32 _numPad);

    void setIntensityMultiplier(f32 multiplier);
    ITF_INLINE f32 getIntensityMultiplier() const { return m_intensityMultiplier; }

private:
    ITF_VECTOR<PadRumble> m_rumbles;
    f32 m_intensityMultiplier;
};

//------------------------------------------------------------------------------
/// To serialize pad rumble definitions
class PadRumbleManager_Template : public TemplateObj
{
    DECLARE_OBJECT_CHILD_RTTI(PadRumbleManager_Template, TemplateObj,3418702772);
    DECLARE_SERIALIZE()

public:
    PadRumbleManager_Template() : Super() {}
    PadRumbleManager_Template(const Path& _path) : Super(_path) {}

    ITF_INLINE const ITF_VECTOR<PadRumble>& getRumbles() const { return m_rumbles; }

private:
    ITF_VECTOR<PadRumble> m_rumbles;
};

} // namespace ITF

#endif //_ITF_PADRUMBLEMANAGER_H_
