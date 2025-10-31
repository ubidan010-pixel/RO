#include "precompiled_gameplay.h"

#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#include "gameplay/Components/Misc/CameraSubjectComponent.h"
#endif //_ITF_CAMERASUBJECTCOMPONENT_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(CameraSubjectComponent)
    BEGIN_SERIALIZATION_CHILD(CameraSubjectComponent)
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(CameraSubjectComponent_Template)
    BEGIN_SERIALIZATION(CameraSubjectComponent_Template)
        SERIALIZE_MEMBER("boundingBoxMin", m_boundingBoxMin);
        SERIALIZE_MEMBER("boundingBoxMax", m_boundingBoxMax);
        SERIALIZE_MEMBER("offset", m_offset);
    END_SERIALIZATION()

    CameraSubjectComponent_Template::CameraSubjectComponent_Template()
    : m_boundingBoxMin(-Vec2d::One)
    , m_boundingBoxMax(Vec2d::One)
    , m_offset(Vec2d::Zero)
    {
    }
    CameraSubjectComponent_Template::~CameraSubjectComponent_Template()
    {
    }

    CameraSubjectComponent::CameraSubjectComponent()
    {
    }

    CameraSubjectComponent::~CameraSubjectComponent()
    {
    }




    AABB CameraSubjectComponent::getAABB() const
    {
        return AABB(
            m_actor->get2DPos() + (getOffset() + getBoundingBoxMin()) *m_actor->getScale(),
            m_actor->get2DPos() + (getOffset() + getBoundingBoxMax()) *m_actor->getScale() );
    }

    bbool CameraSubjectComponent::isSubjectOnScreen( bbool _invalidValue /*= bfalse*/ ) const
    {
        AABB camAABB;
        if ( CAMERACONTROLLERMANAGER->getAABB(m_actor->getDepth(), camAABB) )
        {
            return camAABB.checkOverlap(getAABB());
        }

        // Somehow, the camera is not ready or the player is at a wrong z;
        // whether we consider than on- or off-screen is up to the caller.
        return _invalidValue;
    }

} // namespace ITF