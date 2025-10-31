#include "precompiled_gameplay.h"

#ifndef _ITF_CAMERADETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/CameraDetectorComponent.h"
#endif //_ITF_CAMERADETECTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(CameraDetectorComponent)

    BEGIN_SERIALIZATION_CHILD(CameraDetectorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(CameraDetectorComponent)
    END_VALIDATE_COMPONENT()


    CameraDetectorComponent::CameraDetectorComponent()
    {
    }

    CameraDetectorComponent::~CameraDetectorComponent()
    {
    }

    void CameraDetectorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_actorsInside.clear();
    }

    void CameraDetectorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        // camera teleported, wait one frame before re-check
        //
        if ( !CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
        {
            m_actorsInside.clear();
        }
       
        const AABB screenAABB = CAMERACONTROLLERMANAGER->getAABB();
        ICameraController* cameraController  = CAMERACONTROLLERMANAGER->getMainCameraController();
        
        // check if the camera was not teleported to have valid AABB values
        //
        if (cameraController && !CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
        {
            // by default, use current camera controller
            // To avoid multi triggering (by different camera controllers), if the template param is set, use the current actor as trigger owner
            ActorRef cameraControllerRef = getTemplate()->getTriggEachCameraController() ? cameraController->getActorRef() : m_actor->getRef();

            if (cameraControllerRef.getActor())
            {
                if (!m_currentShape)
                {
                    if (screenAABB.contains(AABB(m_actor->getPos())))
                        m_actorsInside.push_back(cameraControllerRef);
                }
                else
                {
                    AABB detectorAABB;
                    m_currentShape->calculateAABB(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),detectorAABB);
                    if (screenAABB.checkOverlap(detectorAABB))
                    {
                        m_actorsInside.push_back(cameraControllerRef);
#ifndef ITF_FINAL
                        //DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),m_currentShape);
                        //DebugDraw::shape(CAMERA->getPos(),0.0f,&box,Color::red());
#endif // ITF_FINAL
                   }
                }
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(CameraDetectorComponent_Template)
    BEGIN_SERIALIZATION_CHILD(CameraDetectorComponent_Template)
        SERIALIZE_MEMBER("triggEachCameraController",m_triggEachCameraController);
    END_SERIALIZATION()

    CameraDetectorComponent_Template::CameraDetectorComponent_Template()
    : Super()
    , m_triggEachCameraController( btrue )
    {
    }
};
