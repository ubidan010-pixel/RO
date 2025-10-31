#include "precompiled_gameplay.h"

#ifndef _ITF_AXISPOLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/AxisPolylineComponent.h"
#endif //_ITF_AXISPOLYLINECOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(AxisPolylineComponent)
BEGIN_SERIALIZATION_CHILD(AxisPolylineComponent)
END_SERIALIZATION()

AxisPolylineComponent::AxisPolylineComponent()
{
}

AxisPolylineComponent::~AxisPolylineComponent()
{
    clear();
}

AxisPolylineComponent::AxisPoly::AxisPoly()
: m_forces(0.f)
, m_totalWeight(0.f)
, m_currentTorque(0.f)
, m_currentAngle(0.f)
, m_rotatingBoneIndex(U32_INVALID)
, m_rotatingModelPos(Vec2d::Zero)
, m_rotatingModelAngle(0.f)
{
}

void AxisPolylineComponent::clear()
{
    m_axisPolylines.clear();
}

void AxisPolylineComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_axisPolylines.resize(getTemplate()->getAxisPolylines().size());
}

void AxisPolylineComponent::Update( f32 _deltaTime )
{
    if ( !m_ready )
        return;

    u32 numAxis = m_axisPolylines.size();
    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    for ( u32 i = 0; i < numAxis; i++ )
    {
        AxisPoly& axisPoly = m_axisPolylines[i];
        const AxisPolylineComponent_Template::AxisPoly& axisPolyTemplate = getTemplate()->getAxisPolylines()[i];
        calculateRotatingBones(animMeshScene,axisPoly);
        updateAxis(_deltaTime,animMeshScene,axisPolyTemplate.getPolylines(),axisPolyTemplate.getWeightToAngle(),
                    axisPolyTemplate.getStiff(),axisPolyTemplate.getDamp(),axisPolyTemplate.getMaxAngle(),axisPoly);
        updateTransform(axisPoly,animMeshScene);
    }
}

void AxisPolylineComponent::readModel()
{
    Super::readModel();

    if ( !m_ready )
    {
        return;
    }

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
    AnimSkeleton* skeleton = m_animComponent->getCurSubAnim(0)->getSkeleton();
    AxisPolylineComponent_Template::AxisPolyList::const_iterator itTemplate = getTemplate()->getAxisPolylines().begin();
    AxisPolyList::iterator it = m_axisPolylines.begin();

    for ( ; it != m_axisPolylines.end() && itTemplate != getTemplate()->getAxisPolylines().end(); ++it, ++itTemplate )
    {
        AxisPoly& axisPoly = *it;
        const AxisPolylineComponent_Template::AxisPoly& templateAxisPoly = *itTemplate;

        if ( templateAxisPoly.getRotatingBoneName().isValid() )
        {
            axisPoly.m_rotatingBoneIndex = m_animComponent->getBoneIndex(templateAxisPoly.getRotatingBoneName());

            if ( axisPoly.m_rotatingBoneIndex != U32_INVALID )
            {
                AnimBone* animBone = skeleton->getBoneAt(axisPoly.m_rotatingBoneIndex);

                axisPoly.m_orderedBones.push_back(axisPoly.m_rotatingBoneIndex);

                if ( animBone->m_BoneParent )
                {
                    animBone = animBone->m_BoneParent;

                    do {
                        axisPoly.m_orderedBones.push_back(animBone->getIndex());
                        animBone = animBone->m_BoneParent;
                    } while ( animBone );
                }
            }

            calculateRotatingBones(animMeshScene,axisPoly);
        }
    }
}

void AxisPolylineComponent::calculateRotatingBones( AnimMeshScene* _animMeshScene, AxisPoly& _axisPoly )
{
    _axisPoly.m_rotatingModelPos = Vec2d::Zero;
    _axisPoly.m_rotatingModelAngle = 0.f;

    SafeArray<AnimBoneDyn> & boneDynList = _animMeshScene->m_AnimInfo.getCurrentBoneList();
    
    if ( _axisPoly.m_orderedBones.size() > 0 )
    {
        // Go from parent to child transforming into local space of the rotating bone
        // We discard the position of the root
        u32 boneIndex = _axisPoly.m_orderedBones[_axisPoly.m_orderedBones.size()-1];
        AnimBoneDyn* boneDyn   = &boneDynList[boneIndex];

        _axisPoly.m_rotatingModelAngle += boneDyn->m_AngleLocal;
        _axisPoly.m_rotatingModelPos += AddAngleBTW( Vec2d(boneDyn->GetScaledLength(),0.f),_axisPoly.m_rotatingModelAngle);

        for ( i32 i = _axisPoly.m_orderedBones.size()-2; i >= 0; i-- )
        {
            boneIndex = _axisPoly.m_orderedBones[i];

            AnimBoneDyn* boneDyn = &boneDynList[boneIndex];

            _axisPoly.m_rotatingModelPos += AddAngleBTW( boneDyn->m_PosLocal,_axisPoly.m_rotatingModelAngle);
            _axisPoly.m_rotatingModelAngle += boneDyn->m_AngleLocal;

            if ( i != 0 )
            {
                _axisPoly.m_rotatingModelPos += AddAngleBTW( Vec2d(boneDyn->GetScaledLength(),0.f),_axisPoly.m_rotatingModelAngle);
            }
        }
    }
}

void AxisPolylineComponent::transformWorldToLocal( const Vec2d& _pos, const Vec2d& _gravityDir, AxisPoly& _axisPoly,
                                                      Vec2d& localPos, Vec2d& localGravityDir ) const
{
    localPos = ( _pos - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());
    localPos = localPos / m_actor->getScale().m_x;
    localPos.m_y *= -1.f;
    localPos = AddAngleBTW( localPos - _axisPoly.m_rotatingModelPos, -_axisPoly.m_rotatingModelAngle );

    localGravityDir = _gravityDir.Rotate(-m_actor->getAngle());
    localGravityDir.m_y *= -1.f;
    localGravityDir = AddAngleBTW(localGravityDir,-_axisPoly.m_rotatingModelAngle);
}

void AxisPolylineComponent::updateAxis( f32 _dt, AnimMeshScene* _animMeshScene, const SafeArray <StringID>& _polylines,
                                        f32 _weight2Angle, f32 _stiff, f32 _damp, const Angle& _maxAngle, AxisPoly& _axisPoly )
{
    _axisPoly.m_totalWeight = 0.f;

    if ( !m_users.empty() )
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

                ProceduralPolyline* poly = getProcPolylineFromAnimRef(user.m_polylineRef);

                if ( poly )
                {
                    BaseObject* obj = GETOBJECT(user.m_actor);

                    if ( obj )
                    {
                        Actor* userActor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                        Vec2d pos2d = userActor->get2DPos();

                        u32 axisIndex;
                        const AxisPolylineComponent_Template::AxisPoly* polyTemplate = getTemplate()->findAxisPolyFromAnimRef(user.m_polylineRef,axisIndex);
                        f32 weight = polyTemplate ? user.m_weight * polyTemplate->getWeightMultiplier() : user.m_weight;

                        applyWeightFromPoint(pos2d,weight,_axisPoly);
                    }

                    break;
                }
            }
        }
    }

    for ( HangingList::const_iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
    {
        const HangingActor& hanging = *it;

        for ( u32 i = 0; i < _polylines.size(); i++ )
        {
            if ( _polylines[i] != hanging.m_polylineRef )
            {
                continue;
            }

            ProceduralPolyline* poly = getProcPolylineFromAnimRef(hanging.m_polylineRef);

            if ( poly )
            {
                BaseObject* obj = GETOBJECT(hanging.m_actor);

                if ( obj )
                {
                    Actor* userActor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                    Vec2d pos2d = userActor->get2DPos();
                    u32 axisIndex;
                    const AxisPolylineComponent_Template::AxisPoly* polyTemplate = getTemplate()->findAxisPolyFromAnimRef(hanging.m_polylineRef,axisIndex);
                    f32 weight = polyTemplate ? hanging.m_weight * polyTemplate->getWeightMultiplier() : hanging.m_weight;

                    applyWeightFromPoint(pos2d,weight,_axisPoly);
                }
            }
        }
    }

    f32 targetAngle = _axisPoly.m_totalWeight * _weight2Angle;
    f32 delta = getShortestAngleDelta(_axisPoly.m_currentAngle,targetAngle);

    _axisPoly.m_forces += (delta * _stiff) - ( _axisPoly.m_currentTorque * _damp);
    _axisPoly.m_currentTorque += _axisPoly.m_forces*_dt;
    _axisPoly.m_currentAngle += _axisPoly.m_currentTorque*_dt;
    _axisPoly.m_currentAngle = ClampAngle(Angle(bfalse,_axisPoly.m_currentAngle),-_maxAngle,_maxAngle).ToRadians();
    _axisPoly.m_forces = 0.f;
}

void AxisPolylineComponent::updateTransform( AxisPoly& _axisPoly, AnimMeshScene* _animMeshScene )
{
    if ( _axisPoly.m_rotatingBoneIndex != U32_INVALID )
    {
        AnimBoneDyn* boneDyn = &_animMeshScene->m_AnimInfo.getCurrentBoneList()[_axisPoly.m_rotatingBoneIndex];

        boneDyn->m_AngleLocal += _axisPoly.m_currentAngle;
    }
}

void AxisPolylineComponent::applyForceFromPoint( const Vec2d& _pos, f32 _force, AxisPoly& _axisInfo )
{
    Vec2d gravityDir = PHYSWORLD->getGravity(_pos,m_actor->getDepth());
    Vec2d localPos, localGravityDir;

    gravityDir.normalize();
    transformWorldToLocal(_pos,gravityDir,_axisInfo,localPos,localGravityDir);

    if ( localPos != Vec2d::Zero )
    {
        f32 radius = localPos.norm()*m_actor->getScale().m_x;

        localPos.normalize();

        f32 dotProd = localPos.dot(localGravityDir);
        f32 angleSign = localPos.cross(localGravityDir) > 0.f ? -1.f : 1.f;
        f32 angle = f32_ACos(dotProd)*angleSign;
        f32 force = radius * _force * f32_Sin(angle);

        _axisInfo.m_forces += force;
    }
}

void AxisPolylineComponent::applyWeightFromPoint( const Vec2d& _pos, f32 _weight, AxisPoly& _axisInfo )
{
    Vec2d gravityDir = PHYSWORLD->getGravity(_pos,m_actor->getDepth());
    Vec2d localPos, localGravityDir;

    gravityDir.normalize();
    transformWorldToLocal(_pos,gravityDir,_axisInfo,localPos,localGravityDir);

    if ( localPos != Vec2d::Zero )
    {
        f32 radius = localPos.norm()*m_actor->getScale().m_x;

        localPos.normalize();

        f32 dotProd = localPos.dot(localGravityDir);
        dotProd = Clamp(dotProd,-1.f,1.f);
        f32 angleSign = localPos.cross(localGravityDir) > 0.f ? -1.f : 1.f;
        f32 weight = _weight * radius * angleSign;

        _axisInfo.m_totalWeight += weight;
    }
}

void AxisPolylineComponent::applyLand( ObjectRef _poly, const Vec2d& _pos,
                                       const Vec2d& _speed, f32 _weight )
{
    bbool found = bfalse;
    AxisPolyList::iterator it = m_axisPolylines.begin();
    AxisPolylineComponent_Template::AxisPolyList::const_iterator itTemplate = getTemplate()->getAxisPolylines().begin();

    for ( ; it != m_axisPolylines.end() && itTemplate != getTemplate()->getAxisPolylines().end(); ++it, ++itTemplate )
    {
        AxisPoly& axisInfo = *it;
        const AxisPolylineComponent_Template::AxisPoly& axisInfoTemplate = *itTemplate;

        for ( u32 i = 0; i < axisInfoTemplate.getPolylines().size(); i++ )
        {
            ProceduralPolyline* poly = getProcPolylineFromAnimRef(axisInfoTemplate.getPolylines()[i]);

            if ( !poly || poly->m_poly->getRef() != _poly )
            {
                continue;
            }

            applyForceFromPoint(_pos,_speed.norm()*_weight*axisInfoTemplate.getWeightMultiplier()*axisInfoTemplate.getForceMultiplier(),axisInfo);
            found = btrue;
            break;
        }

        if ( found )
        {
            break;
        }
    }
}

void AxisPolylineComponent::onNewUser( EventStickOnPolyline* _event, ProceduralPolyline* _procPoly )
{
    Super::onNewUser(_event,_procPoly);

    if ( !_event->isSticked() )
    {
        return;
    }

    applyLand(_event->getPolyline(),_event->getPos(),_event->getSpeed(),_event->getWeight());
}

void AxisPolylineComponent::onNewHangActor( EventHanging* _event, ProceduralPolyline* _procPoly )
{
    Super::onNewHangActor(_event,_procPoly);

    if ( !_event->isHanging() )
    {
        return;
    }
    
    BaseObject* obj = GETOBJECT(_event->getPolyline());

    if ( !obj )
    {
        return;
    }

    const PolyLine* poly = static_cast<PolyLine*>(obj);
    const PolyLineEdge& edge = poly->getEdgeAt(_event->getEdge());
    Vec2d pos = edge.getPos() + ( edge.m_vector * _event->getT() );

    applyLand(_event->getPolyline(),pos,_event->getSpeed(),_event->getWeight());
}

void AxisPolylineComponent::processPhysBodyForceEvent( EventForceTransfer* _physBodyForce )
{
    u32 axisIndex;
    const AxisPolylineComponent_Template::AxisPoly* axisPolyTemplate = getTemplate()->findAxisPolyFromAnimRef(_physBodyForce->getPolyID(),axisIndex);

    if ( axisPolyTemplate )
    {
        ProceduralPolyline* poly = getProcPolylineFromAnimRef(_physBodyForce->getPolyID());

        if ( poly )
        {
            u32 numPoints = poly->m_pointsInfo.size();

            for ( u32 i = 0; i < numPoints; i++ )
            {
                const PolylinePoint& point = poly->m_pointsInfo[i];

                if ( point.m_pointName == _physBodyForce->getPolyPointID() )
                {
                    applyForceFromPoint(poly->m_poly->getPosAt(i),_physBodyForce->getForce().norm()*axisPolyTemplate->getForceMultiplier(),m_axisPolylines[axisIndex]);
                }
            }
        }
    }
}

void AxisPolylineComponent::validate( bbool &_isComponentValidated )
{
    Super::validate(_isComponentValidated);

    AxisPolylineComponent_Template::AxisPolyList::const_iterator itTemplate = getTemplate()->getAxisPolylines().begin();
    AxisPolyList::iterator it = m_axisPolylines.begin();

    for ( ; it != m_axisPolylines.end() && itTemplate != getTemplate()->getAxisPolylines().end(); ++it, ++itTemplate )
    {
        AxisPoly& axisPoly = *it;
        const AxisPolylineComponent_Template::AxisPoly& axisPolyTemplate = *itTemplate;

        VALIDATE_COMPONENT_PARAM("bone",axisPoly.m_rotatingBoneIndex != U32_INVALID,
            "AxisPoly: %s - %s - Invalid bone name %s",
            m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), axisPolyTemplate.getRotatingBoneName().getDebugString());
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(AxisPolylineComponent_Template)
BEGIN_SERIALIZATION_CHILD(AxisPolylineComponent_Template)

    SERIALIZE_CONTAINER_OBJECT("axisPolylines",m_axisPolylines);

END_SERIALIZATION()

const AxisPolylineComponent_Template::AxisPoly* AxisPolylineComponent_Template::findAxisPolyFromAnimRef( const StringID& _ref, u32& _axisIndex ) const
{
    _axisIndex = U32_INVALID;
    AxisPolylineComponent_Template::AxisPolyList::const_iterator itTemplate = getAxisPolylines().begin();
    u32 axisCount = 0;

    for ( ; itTemplate != getAxisPolylines().end(); ++itTemplate, axisCount++ )
    {
        const AxisPoly& axisPoly = *itTemplate;

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

BEGIN_SERIALIZATION_SUBCLASS(AxisPolylineComponent_Template,AxisPoly)

    SERIALIZE_MEMBER("stiff",m_stiff);
    SERIALIZE_MEMBER("damp",m_damp);
    SERIALIZE_MEMBER("weightToAngle",m_weightToAngle);
    SERIALIZE_MEMBER("maxAngle",m_maxAngle);
    SERIALIZE_MEMBER("bone",m_rotatingBoneName);
    SERIALIZE_MEMBER("weightMultiplier",m_weightMultiplier);
    SERIALIZE_MEMBER("forceMultiplier",m_forceMultiplier);
    SERIALIZE_CONTAINER("polylines",m_polylines);

END_SERIALIZATION()

AxisPolylineComponent_Template::AxisPoly::AxisPoly()
: m_stiff(5.f)
, m_damp(2.5f)
, m_weightToAngle(0.05f)
, m_maxAngle(bfalse,0.785f)
, m_weightMultiplier(1.f)
, m_forceMultiplier(0.005f)
{
}

}

