#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ELECTOONTARGETCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ElectoonTargetComponent.h"
#endif //_ITF_RAY_ELECTOONTARGETCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ElectoonTargetComponent_Template)

    BEGIN_SERIALIZATION(Ray_ElectoonTargetComponent_Template)
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_ElectoonTargetComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ElectoonTargetComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_ElectoonTargetComponent)
    END_VALIDATE_COMPONENT()

    Ray_ElectoonTargetComponent::Ray_ElectoonTargetComponent()
    {
    }

    Ray_ElectoonTargetComponent::~Ray_ElectoonTargetComponent()
    {
    }

    f32 Ray_ElectoonTargetComponent::getMinRange() const
    {
        f32 scale = std::max(m_actor->getScale().m_x,0.f); 
        return scale*getTemplate()->getTypicalMinRange();
    }

    f32 Ray_ElectoonTargetComponent::getMaxRange() const
    {
        return getMinRange()+(getTemplate()->getTypicalMaxRange()-getTemplate()->getTypicalMinRange());
    }

    f32 Ray_ElectoonTargetComponent::getDeltaXFromCageToTarget(f32 _dx, f32 _landingDistance)
    {
        _landingDistance = std::min(_landingDistance, fabsf(_dx)* 0.99f);

        if (_dx<0)
        {
            _dx += _landingDistance;
        }
        else
        {
            _dx -= _landingDistance;
        }
        if (fabsf(_dx)<0.1f) _dx = 0.1f;

        return _dx;
    }

};