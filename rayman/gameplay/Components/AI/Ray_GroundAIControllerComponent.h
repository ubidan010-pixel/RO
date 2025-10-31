#ifndef _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_
#define _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

class Ray_GroundAIControllerComponent : public GroundAIControllerComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_GroundAIControllerComponent,GroundAIControllerComponent,1159656342)
    DECLARE_SERIALIZE()

public:

    Ray_GroundAIControllerComponent();
    virtual ~Ray_GroundAIControllerComponent();

    virtual bbool       needsUpdate() const { return btrue; }
    virtual bbool       needsDraw() const { return bfalse; }
    virtual bbool       needsDraw2D() const { return bfalse; }

protected:

    const class Ray_GroundAIControllerComponent_Template* getTemplate() const;

};

class Ray_GroundAIControllerComponent_Template : public GroundAIControllerComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_GroundAIControllerComponent_Template,GroundAIControllerComponent_Template,748333989);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_GroundAIControllerComponent);

public:

    Ray_GroundAIControllerComponent_Template();
    ~Ray_GroundAIControllerComponent_Template();

private:

};

ITF_INLINE const class Ray_GroundAIControllerComponent_Template* Ray_GroundAIControllerComponent::getTemplate() const
{
    return static_cast<const class Ray_GroundAIControllerComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_