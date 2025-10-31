#include "precompiled_gameplay.h"

#ifndef _ITF_ROTATINGPOLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/RotatingPolylineComponent.h"
#endif //_ITF_ROTATINGPOLYLINECOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(RotatingPolylineComponent)

RotatingPolylineComponent::RotatingPoly::RotatingPoly()
: m_force(0.f)
, m_currentTorque(0.f)
, m_currentAngle(0.f)
, m_rotatingBoneIndex(U32_INVALID)
, m_rotatingModelPos(Vec2d::Zero)
, m_rotatingModelAngle(0.f)
{
}

BEGIN_SERIALIZATION_CHILD(RotatingPolylineComponent)
END_SERIALIZATION()


RotatingPolylineComponent::RotatingPolylineComponent()
{
}

RotatingPolylineComponent::~RotatingPolylineComponent()
{
    clear();
}

void RotatingPolylineComponent::clear()
{
    m_rotatingPolys.clear();
}

void RotatingPolylineComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_rotatingPolys.resize(getTemplate()->getRotatingPolys().size());
}

void RotatingPolylineComponent::readModel()
{
    Super::readModel();

    if ( !m_ready )
    {
        return;
    }

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
    AnimSkeleton* skeleton = m_animComponent->getCurSubAnim(0)->getSkeleton();

    RotatingPolyList::iterator it = m_rotatingPolys.begin();
    RotatingPolylineComponent_Template::RotatingPolyList::const_iterator itTemplate = getTemplate()->getRotatingPolys().begin();

    for ( ; it != m_rotatingPolys.end() && itTemplate != getTemplate()->getRotatingPolys().end(); ++it, ++itTemplate )
    {
        RotatingPoly& rotPoly = *it;
        const RotatingPolylineComponent_Template::RotatingPoly& rotPolyTemplate = *itTemplate;

        if ( rotPolyTemplate.getRotatingBoneName().isValid() )
        {
            rotPoly.m_rotatingBoneIndex = m_animComponent->getBoneIndex(rotPolyTemplate.getRotatingBoneName());

            if ( rotPoly.m_rotatingBoneIndex != U32_INVALID )
            {
                AnimBone* animBone = skeleton->getBoneAt(rotPoly.m_rotatingBoneIndex);

                rotPoly.m_orderedBones.push_back(rotPoly.m_rotatingBoneIndex);

                if ( animBone->m_BoneParent )
                {
                    animBone = animBone->m_BoneParent;

                    do {
                        rotPoly.m_orderedBones.push_back(animBone->getIndex());
                        animBone = animBone->m_BoneParent;
                    } while ( animBone );
                }
            }

            calculateRotatingBones(animMeshScene,rotPoly);
        }
    }
}

void RotatingPolylineComponent::calculateRotatingBones( AnimMeshScene* _animMeshScene, RotatingPoly& _rotPoly )
{
    _rotPoly.m_rotatingModelPos = Vec2d::Zero;
    _rotPoly.m_rotatingModelAngle = 0.f;

    if ( _rotPoly.m_orderedBones.size() > 0 )
    {
        // Go from parent to child transforming into local space of the rotating bone
        // We discard the position of the root
        SafeArray<AnimBoneDyn> & boneDynList = _animMeshScene->m_AnimInfo.getCurrentBoneList();
        u32 boneIndex = _rotPoly.m_orderedBones[_rotPoly.m_orderedBones.size()-1];
        AnimBoneDyn* boneDyn   = &boneDynList[boneIndex];

        _rotPoly.m_rotatingModelAngle += boneDyn->m_AngleLocal;
        _rotPoly.m_rotatingModelPos += AddAngleBTW( Vec2d(boneDyn->GetScaledLength(),0.f),_rotPoly.m_rotatingModelAngle);

        for ( i32 i = _rotPoly.m_orderedBones.size()-2; i >= 0; i-- )
        {
            boneIndex = _rotPoly.m_orderedBones[i];

            AnimBoneDyn* boneDyn = &boneDynList[boneIndex];

            _rotPoly.m_rotatingModelPos += AddAngleBTW( boneDyn->m_PosLocal,_rotPoly.m_rotatingModelAngle);
            _rotPoly.m_rotatingModelAngle += boneDyn->m_AngleLocal;

            if ( i != 0 )
            {
                _rotPoly.m_rotatingModelPos += AddAngleBTW( Vec2d(boneDyn->GetScaledLength(),0.f),_rotPoly.m_rotatingModelAngle);
            }
        }
    }
}

void RotatingPolylineComponent::transformWorldToLocal( const Vec2d& _pos, const Vec2d& _forceDir, RotatingPoly& _rotPoly,
                                                       Vec2d& _localPos, Vec2d& _localForceDir ) const
{
    
    

    _localPos = ( _pos - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());
    _localPos = _localPos / m_actor->getScale().m_x;
    _localPos.m_y *= -1.f;
    _localPos = AddAngleBTW( _localPos - _rotPoly.m_rotatingModelPos, -_rotPoly.m_rotatingModelAngle );

    _localForceDir = _forceDir.Rotate(-m_actor->getAngle());
    _localForceDir.m_y *= -1.f;
    _localForceDir = AddAngleBTW(_localForceDir,-_rotPoly.m_rotatingModelAngle);
}

void RotatingPolylineComponent::applyForceFromPoint( const Vec2d& _pos, const Vec2d& _force, f32 _forceMultiplier,
                                                     RotatingPoly& _rotatingPoly, ProceduralPolyline* _procPoly )
{
    Vec2d gravityDir = PHYSWORLD->getGravity(_pos,m_actor->getDepth());
    Vec2d localPos, localGravityDir;

    gravityDir.normalize();
    transformWorldToLocal(_pos,gravityDir,_rotatingPoly,localPos,localGravityDir);

    if ( localPos != Vec2d::Zero )
    {
        f32 radius = localPos.norm()*m_actor->getScale().m_x;
        f32 force = _force.norm() * _forceMultiplier;

        localPos.normalize();

        f32 dotProd = localPos.dot(localGravityDir);
        f32 angleSign = localPos.cross(localGravityDir) > 0.f ? -1.f : 1.f;
        f32 angle = f32_ACos(dotProd)*angleSign;

        force = radius * force * f32_Sin(angle);

        _rotatingPoly.m_force += force;
    }
}

void RotatingPolylineComponent::Update( f32 _deltaTime )
{
    if ( !m_ready )
        return;

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
    Vec2d gravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
    RotatingPolyList::iterator it = m_rotatingPolys.begin();
    RotatingPolylineComponent_Template::RotatingPolyList::const_iterator itTemplate = getTemplate()->getRotatingPolys().begin();

    for ( ; it != m_rotatingPolys.end() && itTemplate != getTemplate()->getRotatingPolys().end(); ++it, ++itTemplate )
    {
        RotatingPoly& rotPoly = *it;
        const RotatingPolylineComponent_Template::RotatingPoly& rotPolyTemplate = *itTemplate;

        calculateRotatingBones(animMeshScene,rotPoly);
        updateRotatingPoly(_deltaTime,rotPolyTemplate.getPolylines(),rotPolyTemplate.getForceMultiplier(),rotPolyTemplate.getBrake(),gravity,rotPoly);
        updateTransform(rotPoly,animMeshScene);
    }
}

void RotatingPolylineComponent::updateRotatingPoly( f32 _dt, const SafeArray <StringID>& _polylines, f32 _forceMultiplier, f32 _brake,
                                                    const Vec2d& gravity, RotatingPoly& _rotatingPoly )
{
    for ( UsersList::const_iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        const PolyUsers& user = *it;

        for ( u32 i = 0; i < _polylines.size(); i++ )
        {
            if ( _polylines[i] != user.m_polylineRef )
            {
                continue;
            }

            ProceduralPolyline* procPoly = getProcPolylineFromAnimRef(user.m_polylineRef);

            if ( !procPoly )
            {
                continue;
            }

            Actor* userActor = AIUtils::getActor(user.m_actor);

            if ( userActor )
            {
                const PolylineParameters* params = getPolylineParametersFromPolyPoint(procPoly,user.m_edgeIndex);
                const PolyLineEdge& edge = procPoly->m_poly->getEdgeAt(user.m_edgeIndex);
                Vec2d vecToPos = userActor->get2DPos() - edge.getPos();
                f32 dotP = edge.m_normalizedVector.dot(vecToPos);
                Vec2d pos = edge.getPos() + (edge.m_normalizedVector*dotP);
                Vec2d force = gravity * params->m_weightMultiplier * user.m_weight;
                applyForceFromPoint(pos,force,_forceMultiplier,_rotatingPoly,procPoly);
            }
        }
    }

    for ( HangingList::const_iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
    {
        const HangingActor& hang = *it;

        for ( u32 i = 0; i < _polylines.size(); i++ )
        {
            if ( _polylines[i] != hang.m_polylineRef )
            {
                continue;
            }

            ProceduralPolyline* procPoly = getProcPolylineFromAnimRef(hang.m_polylineRef);

            if ( !procPoly )
            {
                continue;
            }

            const PolyLineEdge& edge = procPoly->m_poly->getEdgeAt(hang.m_edgeIndex);
            Vec2d hangPos = edge.getPos() + ( edge.m_vector * hang.m_t );
            const PolylineParameters* params = getPolylineParametersFromPolyPoint(procPoly,hang.m_edgeIndex);
            Vec2d force = gravity * params->m_weightMultiplier * hang.m_weight;

            applyForceFromPoint(hangPos,force,_forceMultiplier,_rotatingPoly,procPoly);
        }
    }

    _rotatingPoly.m_currentTorque += _rotatingPoly.m_force*_dt;
    _rotatingPoly.m_force = 0.f;

    if ( _rotatingPoly.m_currentTorque > 0.f )
    {
        _rotatingPoly.m_currentTorque = Max(_rotatingPoly.m_currentTorque-(_brake*_dt),0.f);
    }
    else if ( _rotatingPoly.m_currentTorque < 0.f )
    {
        _rotatingPoly.m_currentTorque = Min(_rotatingPoly.m_currentTorque+(_brake*_dt),0.f);
    }

    _rotatingPoly.m_currentAngle = _rotatingPoly.m_currentAngle + (_rotatingPoly.m_currentTorque*_dt);

    NormalizeAnglePI(_rotatingPoly.m_currentAngle);
}

void RotatingPolylineComponent::updateTransform( RotatingPoly& _rotatingPoly, AnimMeshScene* _animMeshScene )
{
    if ( _rotatingPoly.m_rotatingBoneIndex != U32_INVALID )
    {
        AnimBoneDyn* boneDyn = &_animMeshScene->m_AnimInfo.getCurrentBoneList()[_rotatingPoly.m_rotatingBoneIndex];

        boneDyn->m_AngleLocal += _rotatingPoly.m_currentAngle;
    }
}

void RotatingPolylineComponent::processNewUser( EventStickOnPolyline* _event )
{
    Super::processNewUser(_event);

    if ( !_event->isSticked() )
    {
        return;
    }

    ProceduralPolyline* procPoly = getProcPolylineFromObjRef(_event->getPolyline());

    if ( !procPoly )
    {
        return;
    }

    u32 polyIndex;
    const RotatingPolylineComponent_Template::RotatingPoly* rotatePolyTemplate = getTemplate()->findRotatingPolyFromAnimRef(procPoly->m_animRef,polyIndex);

    if ( !rotatePolyTemplate )
            {
        return;
            }

                const PolyLineEdge& edge = procPoly->m_poly->getEdgeAt(_event->getEdge());
                Vec2d vecToPos = _event->getPos() - edge.getPos();
                f32 dotP = edge.m_normalizedVector.dot(vecToPos);
                Vec2d pos = edge.getPos() + (edge.m_normalizedVector*dotP);

                const PolylineParameters* params = getPolylineParametersFromPolyPoint(procPoly,_event->getEdge());
    applyForceFromPoint(pos,_event->getSpeed()*_event->getWeight()*params->m_landSpeedMultiplier*params->m_weightMultiplier,rotatePolyTemplate->getForceMultiplier(),m_rotatingPolys[polyIndex],procPoly);
}

void RotatingPolylineComponent::processNewHanging( EventHanging* _event )
{
    Super::processNewHanging(_event);

    if ( !_event->isHanging() )
    {
        return;
    }

    ProceduralPolyline* procPoly = getProcPolylineFromObjRef(_event->getPolyline());

    if ( !procPoly )
    {
        return;
    }

    u32 polyIndex;
    const RotatingPolylineComponent_Template::RotatingPoly* rotatePolyTemplate = getTemplate()->findRotatingPolyFromAnimRef(procPoly->m_animRef,polyIndex);

    if ( !rotatePolyTemplate )
    {
        return;
    }

    const PolylineParameters* params = getPolylineParametersFromPolyPoint(procPoly,_event->getEdge());
    const PolyLineEdge& edge = procPoly->m_poly->getEdgeAt(_event->getEdge());
    Vec2d hangPos = edge.getPos() + ( edge.m_vector * _event->getT() );

    applyForceFromPoint(hangPos,_event->getWeight()*_event->getSpeed()*params->m_landSpeedMultiplier*params->m_weightMultiplier,rotatePolyTemplate->getForceMultiplier(),m_rotatingPolys[polyIndex],procPoly);
}

void RotatingPolylineComponent::processPhysBodyForceEvent( EventForceTransfer* _physBodyForce )
{
    u32 polyIndex;
    const RotatingPolylineComponent_Template::RotatingPoly* rotatePolyTemplate = getTemplate()->findRotatingPolyFromAnimRef(_physBodyForce->getPolyID(),polyIndex);

    if ( !rotatePolyTemplate )
    {
        return;
            }

            ProceduralPolyline* poly = getProcPolylineFromAnimRef(_physBodyForce->getPolyID());

            if ( poly )
            {
                u32 numPoints = poly->m_pointsInfo.size();

                for ( u32 i = 0; i < numPoints; i++ )
                {
                    const PolylinePoint& point = poly->m_pointsInfo[i];

                    if ( point.m_pointName == _physBodyForce->getPolyPointID() )
                    {
                applyForceFromPoint(poly->m_poly->getPosAt(i),_physBodyForce->getForce(),rotatePolyTemplate->getForceMultiplier(),m_rotatingPolys[polyIndex],poly);
            break;
        }
        }
    }
}

void RotatingPolylineComponent::validate( bbool &_isComponentValidated )
{
    Super::validate(_isComponentValidated);

    RotatingPolyList::iterator it = m_rotatingPolys.begin();
    RotatingPolylineComponent_Template::RotatingPolyList::const_iterator itTemplate = getTemplate()->getRotatingPolys().begin();

    for ( ; it != m_rotatingPolys.end() && itTemplate != getTemplate()->getRotatingPolys().end(); ++it, ++itTemplate )
    {
        RotatingPoly& rotPoly = *it;
        const RotatingPolylineComponent_Template::RotatingPoly& rotPolyTemplate = *itTemplate;

        VALIDATE_COMPONENT_PARAM("bone",rotPoly.m_rotatingBoneIndex != U32_INVALID,
            "RotatingPoly: %s - %s - Invalid bone name %s",
            m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), rotPolyTemplate.getRotatingBoneName().getDebugString());
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(RotatingPolylineComponent_Template)
BEGIN_SERIALIZATION_CHILD(RotatingPolylineComponent_Template)

    SERIALIZE_CONTAINER_OBJECT("rotatingPolys",m_rotatingPolys);

END_SERIALIZATION()

const RotatingPolylineComponent_Template::RotatingPoly* RotatingPolylineComponent_Template::findRotatingPolyFromAnimRef( const StringID& _ref, u32& _axisIndex ) const
{
    _axisIndex = U32_INVALID;
    RotatingPolylineComponent_Template::RotatingPolyList::const_iterator itTemplate = getRotatingPolys().begin();
    u32 axisCount = 0;

    for ( ; itTemplate != getRotatingPolys().end(); ++itTemplate, axisCount++ )
    {
        const RotatingPoly& axisPoly = *itTemplate;

        u32 numPolys = axisPoly.getPolylines().size();

        for ( u32 i = 0; i < numPolys; i++ )
        {
            if ( axisPoly.getPolylines()[i] == _ref )
            {
                _axisIndex = axisCount;
                return &axisPoly;
            }
        }
    }

    return NULL;
}

BEGIN_SERIALIZATION_SUBCLASS(RotatingPolylineComponent_Template,RotatingPoly)

    SERIALIZE_MEMBER("brake",m_brake);
    SERIALIZE_MEMBER("forceMultiplier",m_forceMultiplier);
    SERIALIZE_MEMBER("bone",m_rotatingBoneName);
    SERIALIZE_CONTAINER("polylines",m_polylines);

END_SERIALIZATION()

RotatingPolylineComponent_Template::RotatingPoly::RotatingPoly()
: m_brake(1.f)
, m_forceMultiplier(0.025f)
{
}

}
