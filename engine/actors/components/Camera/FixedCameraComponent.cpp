#include "precompiled_engine.h"

#ifndef _ITF_FIXEDCAMERACOMPONENT_H_
#include "engine/actors/components/Camera/FixedCameraComponent.h"
#endif //_ITF_FIXEDCAMERACOMPONENT_H_


#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(FixedCameraComponent)
    BEGIN_SERIALIZATION_CHILD(FixedCameraComponent)
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(FixedCameraComponent_Template)
    BEGIN_SERIALIZATION_CHILD(FixedCameraComponent_Template)
        SERIALIZE_MEMBER("zOffset", m_zOffset);
    END_SERIALIZATION()

    FixedCameraComponent_Template::FixedCameraComponent_Template()
        : m_zOffset(10)
    {
    }
    FixedCameraComponent_Template::~FixedCameraComponent_Template()
    {
    }

    FixedCameraComponent::FixedCameraComponent()
        : Super()
    {
    }

    FixedCameraComponent::~FixedCameraComponent()
    {
    }

    Vec3d FixedCameraComponent::getCameraPosition() const 
    {
        Vec3d pos;
        if (m_actor)
        {
            pos = m_actor->getPos();
            pos.m_z += getTemplate()->getZOffset();
        }
        return pos;
    }

}// namespace ITF