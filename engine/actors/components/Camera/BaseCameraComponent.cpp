#include "precompiled_engine.h"

#ifndef _ITF_BASECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/BaseCameraComponent.h"
#endif //_ITF_BASECAMERACOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(BaseCameraComponent)

    BEGIN_SERIALIZATION_ABSTRACT_CHILD(BaseCameraComponent)
    END_SERIALIZATION()


    IMPLEMENT_OBJECT_RTTI(BaseCameraComponent_Template)
    BEGIN_SERIALIZATION_ABSTRACT(BaseCameraComponent_Template)
        SERIALIZE_MEMBER("startAsMainCam", m_startAsMainCamera);
        SERIALIZE_MEMBER("rampUpDestinationCoeff", m_rampUpDestinationCoeff);
        SERIALIZE_MEMBER("rampUpCoeff", m_rampUpCoeff);
    END_SERIALIZATION()

    BaseCameraComponent_Template::BaseCameraComponent_Template()
        : m_startAsMainCamera(bfalse)
        , m_rampUpDestinationCoeff(0.05f)
        , m_rampUpCoeff(0.03f)
    {
    }
    BaseCameraComponent_Template::~BaseCameraComponent_Template()
    {
    }

    BaseCameraComponent::BaseCameraComponent()
        : Super()
    {
    }

    BaseCameraComponent::~BaseCameraComponent()
    {
        CAMERACONTROLLERMANAGER->unregisterCameraController( this );
        CAMERACONTROLLERMANAGER->removeMainCameraController(this, 1, 1);
    }

    void BaseCameraComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        CAMERACONTROLLERMANAGER->registerCameraController( this );

        if (getTemplate()->getStartAsMainCamera())
        {
            CAMERACONTROLLERMANAGER->setMainCameraController(this, 1, 1);
        }
    }

    void BaseCameraComponent::setAsMainCamera( bbool _value )
    {
        if (_value)
        {
            CAMERACONTROLLERMANAGER->setMainCameraController(this, getTemplate()->getRampUpDestinationCoeff(), getTemplate()->getRampUpCoeff());
        }
        else
        {
            CAMERACONTROLLERMANAGER->removeMainCameraController(this, getTemplate()->getRampUpDestinationCoeff(), getTemplate()->getRampUpCoeff());
        }
    }

} // namespace ITF
