#ifndef _ITF_RAY_POWERUPDISPLAY_H_
#define _ITF_RAY_POWERUPDISPLAY_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

namespace ITF {

class PlayerControllerComponent;
class Ray_PowerUpDisplay_Template;

//------------------------------------------------------------------------------
/// Base class for power-ups that need a custom visual separated from the main
/// player anim (heart shield, super punch...)
class Ray_PowerUpDisplay : public IRTTIObject
{
public:
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_PowerUpDisplay,IRTTIObject,3867674880)
    DECLARE_VALIDATE_OBJECT()

    Ray_PowerUpDisplay();
    virtual ~Ray_PowerUpDisplay() {}
    
    virtual void init(
        Actor * _actor,
        u32 _playerIndex,
        const Ray_PowerUpDisplay_Template* _template
        );
    virtual void onActorLoaded(Pickable::HotReloadType _hotReload) {}
    virtual void onBecomeActive() {}
    virtual void onBecomeInactive() {}
    virtual void update(f32 _dt) {}
    virtual void onEvent(Event* _event) {}
    void setPlayerIndex(u32 _index) {m_playerIndex = _index;} 
protected:
    bbool isEnabled() const;
    void setEnabled(bbool _val) const;
    u32 getAmmo() const;
    void useAmmo() const;

    Actor* m_actor;
    u32 m_playerIndex;
    const Ray_PowerUpDisplay_Template* m_template;
};

//------------------------------------------------------------------------------
class Ray_PowerUpDisplay_Template : public IRTTIObject
{
public:
    DECLARE_OBJECT_CHILD_RTTI(Ray_PowerUpDisplay_Template,IRTTIObject,488070125)
    DECLARE_SERIALIZE_VIRTUAL()

    Ray_PowerUpDisplay_Template() : Super(), m_id() {}
    virtual ~Ray_PowerUpDisplay_Template() {}
    
    ITF_INLINE const StringID& getId() const { return m_id; }

private:
    StringID m_id;
};

} // namespace ITF

#endif // #define _ITF_RAY_POWERUPDISPLAY_H_
