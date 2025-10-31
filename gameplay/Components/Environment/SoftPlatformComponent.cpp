#include "precompiled_gameplay.h"

#ifndef _ITF_SOFTPLATFORMCOMPONENT_H_
#include "gameplay/components/Environment/SoftPlatformComponent.h"
#endif //_ITF_SOFTPLATFORMCOMPONENT_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_OBJECTGROUP_H_
#include "engine/scene/ObjectGroup.h"
#endif //_ITF_OBJECTGROUP_H_

#ifndef _ITF_PHYSCONSTRAINT_H_
#include "engine/physics/PhysConstraint.h"
#endif //_ITF_PHYSCONSTRAINT_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifdef ITF_IPAD
#include <string>
#endif

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(SoftPlatformComponent)

BEGIN_SERIALIZATION_CHILD(SoftPlatformComponent)
END_SERIALIZATION()

SoftPlatformComponent::BodyData::BodyData()
: m_boneIndex(U32_INVALID)
, m_boneEnd(bfalse)
{
}

SoftPlatformComponent::SoftPlatformComponent()
: m_animComponent(NULL)
, m_softPlatform(NULL)
, m_ready(bfalse)
, m_teleported(bfalse)
{
}

SoftPlatformComponent::~SoftPlatformComponent()
{
    clear();
}

void SoftPlatformComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if ( getTemplate()->getGameMaterial().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
    {
        World::addGameMaterialFromFile(getTemplate()->getGameMaterial(), m_actor->getResourceGroup());
    }

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();
    
    if ( m_animComponent )
    {
        m_animComponent->setDisableVisiblePrediction(btrue);
    }

    createSoftPlatform();

    m_softPlatform->setOwner(m_actor);

    m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTeleport,1075308886),this);
}

void SoftPlatformComponent::createSoftPlatform()
{
    m_softPlatform = newAlloc(mId_Gameplay,SoftPlatform);
}

void SoftPlatformComponent::onResourceReady()
{
    Super::onResourceReady();

    if ( !m_ready )
    {
        readModel();
    }
}

void SoftPlatformComponent::readModel()
{
    m_ready = btrue;
    m_bodies.resize(getTemplate()->getBodies().size());

    SoftPlatform::SoftPlatformInitData softInitData;

    softInitData.m_depth = m_actor->getDepth();
    softInitData.m_bodies.resize(m_bodies.size());
    softInitData.m_constraints.resize(getTemplate()->getConstraints().size());
    softInitData.m_weightMultiplier = getTemplate()->getWeightMultiplier();
    softInitData.m_landSpeedMultiplier = getTemplate()->getLandSpeedMultiplier();
    softInitData.m_hitForceMultiplier = getTemplate()->getHitForceMultiplier();
    softInitData.m_impulseMultiplier = getTemplate()->getImpulseMultiplier();
    softInitData.m_movingPolyForce = getTemplate()->getMovingPolylineForce();
    softInitData.m_gameMaterial = getTemplate()->getGameMaterial().getStringID();
    softInitData.m_phantom = getTemplate()->getUsePhantom();

    Vec2d bonePos = Vec2d::Zero;

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();

    u32 count = 0;
    BodiesArray::iterator it = m_bodies.begin();
    SoftPlatformComponent_Template::BodiesArray::const_iterator itTemplate = getTemplate()->getBodies().begin();

    for ( ; it != m_bodies.end() && itTemplate != getTemplate()->getBodies().end(); ++it, ++itTemplate, count++ )
    {
        BodyData& bodyData = *it;
        SoftPlatform::BodyInitData& bodyInitData = softInitData.m_bodies[count];
        const SoftPlatformComponent_Template::BodyData& bodyDataTemplate = *itTemplate;

        bodyData.m_boneId = bodyDataTemplate.getBoneName();

        u32 nameLength = u32(bodyDataTemplate.getBoneName().length());

        if ( nameLength && bodyDataTemplate.getBoneName()[nameLength-1] == '*')
        {
            bodyData.m_boneEnd = btrue;
            StringID boneName(StringID::StrToCRC(bodyDataTemplate.getBoneName().c_str(), u32(bodyDataTemplate.getBoneName().length()-1)));
            bodyData.m_boneIndex = m_animComponent->getBoneIndex(boneName);
        }
        else
        {
            bodyData.m_boneEnd = bfalse;
            bodyData.m_boneIndex = m_animComponent->getBoneIndex(bodyDataTemplate.getBoneName());
        }

        if ( bodyData.m_boneIndex < boneDynList.size() )
        {
            AnimBoneDyn* boneDyn = &boneDynList[bodyData.m_boneIndex];

            if ( bodyData.m_boneEnd )
            {
                bonePos = boneDyn->m_PosEnd;
            }
            else
            {
                bonePos = boneDyn->m_Pos;
            }
        }

        bodyInitData.m_pos = bonePos;
        bodyInitData.m_static = bodyDataTemplate.getIsStatic();
        bodyInitData.m_gravityMultiplier = bodyDataTemplate.getGravityMultiplier();
        bodyInitData.m_windMultiplier = bodyDataTemplate.getWindMultiplier();
    }

    u32 numConstraints = getTemplate()->getConstraints().size();
    PhysConstraintInitCommonInfo constraintInfo;

    for ( u32 i = 0; i < numConstraints; i++ )
    {
        SoftPlatform::ConstraintInitData& constraintInitData = softInitData.m_constraints[i];
        const SoftPlatformComponent_Template::ConstraintData& constraintDataTemplate = getTemplate()->getConstraints()[i];

        constraintInitData.m_bodyAIndex = findBodyIndex(constraintDataTemplate.getBodyAName());
        constraintInitData.m_bodyBIndex = findBodyIndex(constraintDataTemplate.getBodyBName());
        constraintInitData.m_minAngle = constraintDataTemplate.getMinAngle();
        constraintInitData.m_maxAngle = constraintDataTemplate.getMaxAngle();
        constraintInitData.m_minLength = constraintDataTemplate.getMinLength();
        constraintInitData.m_maxLength = constraintDataTemplate.getMaxLength();
        constraintInitData.m_stiff = constraintDataTemplate.getStiff();
        constraintInitData.m_damp = constraintDataTemplate.getDamp();
        constraintInitData.m_limitAngle = constraintDataTemplate.getIsLimitAngle();
        constraintInitData.m_relaxLength = constraintDataTemplate.getIsRelaxLength();
    }

    m_softPlatform->init(softInitData);

    // Reorder the bones so that we process the children before its parents
    ITF_VECTOR <bbool> processed;
    const VectorAnim<AnimBone>& boneList = m_animComponent->getCurSubAnim(0)->getSkeleton()->m_BonesCurFrame;

    processed.resize(boneList.size());
    std::fill(processed.begin(),processed.end(),0);

    int pos = 0;
    for ( u32 boneIndex = 0; boneIndex < boneList.size(); boneIndex++, pos++ )
    {
        orderBonesRecursive(boneList,boneIndex,processed);
    }
}

// This recursive function adds all the bones into m_orderedBones in an ordered way so that the parent
// go first, then the children
void SoftPlatformComponent::orderBonesRecursive( const VectorAnim<AnimBone>& _boneList,
                                                 u32 _boneIndex, ITF_VECTOR<bbool>& _processed )
{
    const AnimBone* bone = &_boneList[_boneIndex];

    if ( _processed[bone->getIndex()] )
    {
        return;
    }

    if ( bone->m_BoneParent && !_processed[bone->m_BoneParent->getIndex()] )
    {
        orderBonesRecursive(_boneList,bone->m_BoneParent->getIndex(),_processed);
    }

    SoftBoneInfo softBone;

    softBone.m_bodyIndex = U32_INVALID;

    for ( u32 i = 0; i < m_bodies.size(); i++ )
    {
        if ( m_bodies[i].m_boneIndex == _boneIndex && !m_bodies[i].m_boneEnd )
        {
            softBone.m_bodyIndex = i;
            break;
        }
    }

    softBone.m_boneIndex = _boneIndex;
    softBone.m_pos = Vec2d::Zero;
    softBone.m_angle = 0.f;

    if ( bone->m_BoneParent )
    {
        // Look for the index of the parent in the m_orderedBones array
        u32 parentIndex = bone->m_BoneParent->getIndex();

        for ( u32 i = 0; i < m_orderedBones.size(); i++ )
        {
            SoftBoneInfo& checkParent = m_orderedBones[i];
            
            if ( checkParent.m_boneIndex == parentIndex )
            {
                softBone.m_parentIndex = i;
                break;
            }
        }
    }
    else
    {
        softBone.m_parentIndex = U32_INVALID;
    }

    m_orderedBones.push_back(softBone);

    _processed[_boneIndex] = btrue;
}

u32 SoftPlatformComponent::findBodyIndex( const ITF_STDSTRING& _name ) const
{
    u32 nameLength = u32(_name.length());
    bbool boneEnd = nameLength && _name[nameLength-1] == '*';
    StringID boneId = _name;

    for ( u32 i = 0; i < m_bodies.size(); i++ )
    {
        if ( m_bodies[i].m_boneId == boneId &&
             m_bodies[i].m_boneEnd == boneEnd )
        {
            return i;
        }
    }

    return U32_INVALID;
}

void SoftPlatformComponent::onBecomeActive()
{
    m_softPlatform->registerPhysics(btrue);

    Super::onBecomeActive();
}

void SoftPlatformComponent::onBecomeInactive()
{
    m_softPlatform->registerPhysics(bfalse);

    Super::onBecomeInactive();
}

void SoftPlatformComponent::Update( f32 _deltaTime )
{
    if ( !m_ready )
        return;

    if ( m_teleported )
    {
        syncPhysWithAnim();
        m_teleported = bfalse;
    }

    updateConstraints();

    m_softPlatform->update(_deltaTime);

    updateTransforms();
}

void SoftPlatformComponent::updateConstraints()
{
    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( !animMeshScene )
    {
        return;
    }

    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
    SoftPlatform::ConstraintArray& constraints = m_softPlatform->getConstraintsList();
    u32 numConstraints = constraints.size();

    for ( u32 i = 0; i < numConstraints; i++ )
    {
        SoftPlatform::ConstraintData& constraintData = constraints[i];

        if ( constraintData.m_constraint )
        {
            BodyData& bodyA = m_bodies[constraintData.m_bodyAIndex];
            BodyData& bodyB = m_bodies[constraintData.m_bodyBIndex];

            if ( bodyA.m_boneIndex != U32_INVALID && bodyB.m_boneIndex != U32_INVALID )
            {
                Vec2d pA;
                Vec2d pB;

                AnimBoneDyn* boneDyn = &boneDynList[bodyA.m_boneIndex];

                if ( bodyA.m_boneEnd )
                {
                    pA = boneDyn->m_PosEnd;
                }
                else
                {
                    pA = boneDyn->m_Pos;
                }

                boneDyn = &boneDynList[bodyB.m_boneIndex];

                if ( bodyB.m_boneEnd )
                {
                    pB = boneDyn->m_PosEnd;
                }
                else
                {
                    pB = boneDyn->m_Pos;
                }

                f32 bodyDist = ( pA - pB ).norm();

                constraintData.m_constraint->setLength(bodyDist);
            }
        }
    }
}

void SoftPlatformComponent::getParentTransform( u32 _parentSoftBoneIndex, Vec2d& _pos, f32& _angle ) const
{
    if ( _parentSoftBoneIndex == U32_INVALID )
    {
        _pos = Vec2d::Zero;
        _angle = 0.f;
    }
    else
    {
        const SoftBoneInfo& parentBoneInfo = m_orderedBones[_parentSoftBoneIndex];
        _pos = parentBoneInfo.m_pos;
        _angle = parentBoneInfo.m_angle;
    }
}

void SoftPlatformComponent::transformWorldToModel( const Vec2d& _world, const struct transform2d& _xf, Vec2d& _model ) const
{
    _model = mulTransform2dT(_xf,_world);
    _model = _model/m_actor->getScale().m_x;
    _model.m_y *= -1.f;
}

void SoftPlatformComponent::getBodySkelPos( const BodyData& _bodyData,
                                            u32 _bodyIndex,
                                            bbool _isStatic,
                                            AnimBoneDyn *_boneDyn,
                                            u32 _parentIndex,
                                            const struct transform2d& _xf,
                                            Vec2d& _model,
                                            Vec2d& _local ) const
{
    if ( _parentIndex == U32_INVALID )
    {
        _model = Vec2d::Zero;
        _local = Vec2d::Zero;
    }
    else
    {
        const SoftBoneInfo& parentBoneInfo = m_orderedBones[_parentIndex];

        if ( _isStatic )
        {
            if ( _bodyData.m_boneEnd )
            {
                _model = parentBoneInfo.m_pos + AddAngleBTW(_boneDyn->m_PosLocal, parentBoneInfo.m_angle) + AddAngleBTW( Vec2d(_boneDyn->GetScaledLength(), 0.0f), _boneDyn->m_AngleLocal);
                _local = _boneDyn->m_PosLocal + AddAngleBTW( Vec2d(_boneDyn->GetScaledLength(), 0.0f), _boneDyn->m_AngleLocal);
            }
            else
            {
                _model = parentBoneInfo.m_pos + AddAngleBTW(_boneDyn->m_PosLocal, parentBoneInfo.m_angle);
                _local = _boneDyn->m_PosLocal;
            }
        }
        else
        {
            SoftPlatform::BodyData& bodyData = m_softPlatform->getBodyList()[_bodyIndex];
            transformWorldToModel(bodyData.m_body->getPos(),_xf,_model);
            _local = AddAngleBTW(_model - parentBoneInfo.m_pos,-parentBoneInfo.m_angle);
        }
    }
}

void SoftPlatformComponent::getBodySkelRot( const Vec2d _posLocalA,
                                            const Vec2d _posLocalB,
                                            const Vec2d _posModelA,
                                            const Vec2d _posModelB,
                                            f32& _angleLocal,
                                            f32& _angleModel ) const
{
    if ( _posLocalA != _posLocalB )
    {
        Vec2d dir = _posLocalB - _posLocalA;
        dir.normalize();
        _angleLocal = -getVec2Angle(dir);

        dir = _posModelB - _posModelA;
        dir.normalize();
        _angleModel = -getVec2Angle(dir);
    }
    else
    {
        _angleLocal = 0.f;
        _angleModel = 0.f;
    }
}

void SoftPlatformComponent::updateBodyBone( u32 _softBoneIndex, AnimMeshScene* _animScene, const transform2d& _xf )
{
    SoftBoneInfo& orderedBoneInfo = m_orderedBones[_softBoneIndex];
    SafeArray<AnimBoneDyn> & boneDynList = _animScene->m_AnimInfo.getCurrentBoneList();
    AnimBoneDyn *boneDyn = &boneDynList[orderedBoneInfo.m_boneIndex];
    Vec2d parentPos;
    f32 parentAngle;
    Vec2d localPos,bodyBLocalPos;
    Vec2d bodyModelPos,bodyBModelPos;
    f32 bodyModelRot;

    getParentTransform(orderedBoneInfo.m_parentIndex,parentPos,parentAngle);

    if ( orderedBoneInfo.m_bodyIndex != U32_INVALID )
    {
        SoftPlatform::BodyData& softBody = m_softPlatform->getBodyList()[orderedBoneInfo.m_bodyIndex];
        BodyData& body = m_bodies[orderedBoneInfo.m_bodyIndex];
        const SoftPlatformComponent_Template::BodyData& bodyTemplate = getTemplate()->getBodies()[orderedBoneInfo.m_bodyIndex];

        getBodySkelPos(body,orderedBoneInfo.m_bodyIndex,bodyTemplate.getIsStatic(),boneDyn,orderedBoneInfo.m_parentIndex,_xf,bodyModelPos,localPos);

        if ( !softBody.m_body->isStatic() )
        {
            if ( body.m_boneEnd )
            {
                f32 size = ( boneDyn->m_PosLocal - localPos ).norm() / boneDyn->m_Scale.m_x;
                boneDyn->m_Length = size;
            }
            else
            {
                boneDyn->m_PosLocal = localPos;
            }
        }

        if ( softBody.m_constraintIndex != U32_INVALID )
        {
            // If this body is connected to another one we will put the end position where the other body is
            SoftPlatform::ConstraintData& constraint = m_softPlatform->getConstraintsList()[softBody.m_constraintIndex];
            SoftPlatform::BodyData& softBodyB = m_softPlatform->getBodyList()[constraint.m_bodyBIndex];

            transformWorldToModel(softBodyB.m_body->getPos(),_xf,bodyBModelPos);
            bodyBLocalPos = AddAngleBTW(bodyBModelPos - parentPos,-parentAngle);

            getBodySkelRot(localPos,bodyBLocalPos,bodyModelPos,bodyBModelPos,boneDyn->m_AngleLocal,bodyModelRot);
        }
        else
        {
            bodyModelRot = parentAngle + boneDyn->m_AngleLocal;
        }
    }
    else
    {
        if ( orderedBoneInfo.m_parentIndex == U32_INVALID )
        {
            bodyModelRot = boneDyn->m_AngleLocal;
            bodyModelPos = Vec2d::Zero;
        }
        else
        {
            bodyModelRot = parentAngle+boneDyn->m_AngleLocal;
            bodyModelPos = parentPos + AddAngleBTW( boneDyn->m_PosLocal,parentAngle);
        }
    }

    orderedBoneInfo.m_angle = bodyModelRot;
    orderedBoneInfo.m_pos = bodyModelPos + AddAngleBTW( Vec2d(boneDyn->GetScaledLength(),0.f),bodyModelRot);
}

// Update the position of each bone
void SoftPlatformComponent::updateTransforms()
{
    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
    if ( !animMeshScene )
    {
        return;
    }

    // Make sure the static bodies are placed in the right position before updating the skeleton
    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
    u32 numBodies = m_bodies.size();

    for ( u32 i = 0; i < numBodies; i++ )
    {
        const SoftPlatformComponent_Template::BodyData& bodyDataTemplate = getTemplate()->getBodies()[i];

        if ( bodyDataTemplate.getIsStatic() )
        {
            BodyData& body = m_bodies[i];
            AnimBoneDyn* boneDyn = &boneDynList[body.m_boneIndex];
            SoftPlatform::BodyData& softBodyData = m_softPlatform->getBodyList()[i];

            if ( body.m_boneEnd )
            {
                softBodyData.m_body->setPos(boneDyn->m_PosEnd);
            }
            else
            {
                softBodyData.m_body->setPos(boneDyn->m_Pos);
            }
        }
    }

    transform2d xf;

    xf.m_pos = m_actor->get2DPos();
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

    for ( u32 i = 0; i < m_orderedBones.size(); i++ )
    {
        updateBodyBone(i,animMeshScene,xf);
    }
}

void SoftPlatformComponent::updatePhysBodyRotations()
{
    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( !animMeshScene )
    {
        return;
    }

    AnimSkeleton* skeleton = m_animComponent->getNumPlayingSubAnims() ?
                                    m_animComponent->getCurSubAnim(0)->getSkeleton() :
                                    NULL;

    if ( !skeleton )
    {
        return;
    }
    
    SafeArray<AnimBoneDyn>& boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
    u32 numBodies = m_bodies.size();
    u32 numConstraints = m_softPlatform->getConstraintsList().size();

    for ( u32 bodyIndex = 0; bodyIndex < numBodies; bodyIndex++ )
    {
        BodyData& bodyData = m_bodies[bodyIndex];
        SoftPlatform::BodyData& softBodyData = m_softPlatform->getBodyList()[bodyIndex];
        const SoftPlatformComponent_Template::BodyData& bodyDataTemplate = getTemplate()->getBodies()[bodyIndex];
        PhysBody* physBody = softBodyData.m_body;

        if ( !physBody )
        {
            continue;
        }

        if ( bodyDataTemplate.getUseStaticAngle() && bodyData.m_boneIndex < skeleton->m_BonesCurFrame.size() )
        {
            const AnimBone* bone = &skeleton->m_BonesCurFrame[bodyData.m_boneIndex];
            const AnimBone* parentBone = bone->m_BoneParent;

            if ( parentBone )
            {
                u32 parentIndex = parentBone->getIndex();

                if ( parentIndex < boneDynList.size() )
                {
                    physBody->setAngle(bodyDataTemplate.getSetAngle().ToRadians()+boneDynList[parentIndex].m_Angle+m_actor->getAngle());
                }
            }
            else
            {
                physBody->setAngle(bodyDataTemplate.getSetAngle().ToRadians()+m_actor->getAngle());
            }
            
            continue;
        }
        
        f32 angle = 0.f;
        f32 numAv = 0.f;

        for ( u32 constraintIndex = 0; constraintIndex < numConstraints; constraintIndex++ )
        {
            SoftPlatform::ConstraintData& constraintData = m_softPlatform->getConstraintsList()[constraintIndex];

            if ( constraintData.m_bodyAIndex < m_bodies.size() &&
                 constraintData.m_bodyBIndex == bodyIndex )
            {
                PhysBody* bodyA = m_softPlatform->getBodyList()[constraintData.m_bodyAIndex].m_body;

                if ( !bodyA )
                {
                    continue;
                }

                Vec2d dir = physBody->getPos() - bodyA->getPos();

                dir.normalize();

                angle += getVec2Angle(dir);
                numAv += 1.f;
            }
        }

        if ( numAv )
        {
            angle /= numAv;
            physBody->setAngle(angle);
        }
    }
}

void SoftPlatformComponent::onEvent( Event * _event )
{
    Super::onEvent(_event);

    if ( EventAnimUpdated* animUpdated = _event->DynamicCast<EventAnimUpdated>(ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305)) )
    {
        processAnimUpdated(animUpdated);
    }
    else if ( EventTeleport* teleport = _event->DynamicCast<EventTeleport>(ITF_GET_STRINGID_CRC(EventTeleport,1075308886)) )
    {
        m_teleported = btrue;
    }

    if ( m_softPlatform && m_ready )
    {
        m_softPlatform->onEvent(_event);
    }
}

void SoftPlatformComponent::processAnimUpdated( EventAnimUpdated* _animUpdated )
{
    if ( !m_ready )
    {
        return;
    }

    updatePhysBodyRotations();
}

void SoftPlatformComponent::syncPhysWithAnim()
{
    if ( !m_ready )
    {
        return;
    }

    updateTransforms();

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( !animMeshScene )
    {
        return;
    }

    f32 fZ = m_actor->getDepth();
    SoftPlatform::BodiesArray& bodyList = m_softPlatform->getBodyList();
    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
    u32 count = 0;
    for ( BodiesArray::iterator it = m_bodies.begin(); it != m_bodies.end(); ++it, count++ )
    {
        BodyData& bodyInfo = *it;
        SoftPlatform::BodyData& softBodyInfo = bodyList[count];

        if ( !softBodyInfo.m_body )
        {
            continue;
        }

        AnimBoneDyn* boneDyn = &boneDynList[bodyInfo.m_boneIndex];

        if ( bodyInfo.m_boneEnd )
        {
            softBodyInfo.m_body->setPos3D(boneDyn->m_PosEnd.to3d(fZ));
        }
        else
        {
            softBodyInfo.m_body->setPos3D(boneDyn->m_Pos.to3d(fZ));
        }

        softBodyInfo.m_body->synchronizePos();
    }

    m_softPlatform->setDepth(fZ,bfalse);
}

void SoftPlatformComponent::clear()
{
    m_bodies.clear();
    m_orderedBones.clear();
    SF_DEL(m_softPlatform);
}

void SoftPlatformComponent::validate( bbool &_isComponentValidated )
{
    Super::validate(_isComponentValidated);

    VALIDATE_COMPONENT_PARAM("", m_animComponent, "SoftPlatformComponent requires an Anim Component");

    VALIDATE_COMPONENT_PARAM("bodies",m_bodies.size()>1,
        "%s: a soft platform component needs more than one body to be valid",
        m_actor->getUserFriendly().cStr());

    BodiesArray::iterator it = m_bodies.begin();
    SoftPlatformComponent_Template::BodiesArray::const_iterator itTemplate = getTemplate()->getBodies().begin();

    for ( ; it != m_bodies.end() && itTemplate != getTemplate()->getBodies().end(); ++it, ++itTemplate )
    {
        BodyData& bodyData = *it;
        const SoftPlatformComponent_Template::BodyData& bodyDataTemplate = *itTemplate;

        VALIDATE_COMPONENT_PARAM("bone",bodyData.m_boneIndex != U32_INVALID,
            "BodyData: %s - %s - Invalid bone name %s",
            m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), bodyDataTemplate.getBoneName().c_str());
    }

    SoftPlatform::ConstraintArray& constraintsList = m_softPlatform->getConstraintsList();

    if ( constraintsList.size() > 0 )
    {
        u32 count = 0;
        SoftPlatformComponent_Template::ConstraintArray::const_iterator itTemplateConstraint = getTemplate()->getConstraints().begin();

        for ( ; itTemplateConstraint != getTemplate()->getConstraints().end(); ++itTemplateConstraint, count++ )
        {
            SoftPlatform::ConstraintData& data = constraintsList[count];
            const SoftPlatformComponent_Template::ConstraintData& constraintDataTemplate = *itTemplateConstraint;

            VALIDATE_COMPONENT_PARAM("bodyA",data.m_bodyAIndex != U32_INVALID,
                "ConstraintData: %s - %s - Invalid body name %s",
                m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), constraintDataTemplate.getBodyAName().c_str());

            VALIDATE_COMPONENT_PARAM("bodyB",data.m_bodyBIndex != U32_INVALID,
                "ConstraintData: %s - %s - Invalid body name %s",
                m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), constraintDataTemplate.getBodyBName().c_str());
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
void SoftPlatformComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    if ( !m_ready )
    {
        return;
    }

    Super::drawEdit(_drawInterface, _flags );
    
    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    m_softPlatform->drawEdit(_drawInterface,_flags);

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( animMeshScene )
    {
        const f32 actorDepth = GetActor()->getDepth();

        SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
        for ( u32 i = 0; i < m_bodies.size(); i++ )
        {
            const BodyData& bodyData = m_bodies[i];
            const AnimBoneDyn* boneDyn = &boneDynList[bodyData.m_boneIndex];
            const Vec3d pos3d = boneDyn->m_Pos.to3d(actorDepth);
            const Vec3d posEnd3d = boneDyn->m_PosEnd.to3d(actorDepth);

            GFX_ADAPTER->drawDBG3DLine(pos3d, posEnd3d, 0.f, 0.f, 1.f);
            GFX_ADAPTER->drawDBGCircle(pos3d, 0.04f, 1.f, 0.f, 0.f);
            GFX_ADAPTER->drawDBGCircle(posEnd3d, 0.02f, 1.f, 0.f, 0.f);

            if(_drawInterface->showTextTips() && m_actor->isSelected())
            {
                const SoftPlatformComponent_Template::BodyData& bodyDataTemplate = getTemplate()->getBodies()[i];
                Vec3d res2d;

                if ( bodyData.m_boneEnd )
                {
                    GFX_ADAPTER->compute3DTo2D(posEnd3d, res2d);
                }
                else
                {
                    GFX_ADAPTER->compute3DTo2D(pos3d, res2d);
                }
                
                GFX_ADAPTER->drawDBGText(bodyDataTemplate.getBoneName().c_str(), res2d.m_x, res2d.m_y);

                SoftPlatform::BodyData& softBodyData = m_softPlatform->getBodyList()[i];
                GFX_ADAPTER->compute3DTo2D(softBodyData.m_body->getPos().to3d(m_actor->getPos().m_z), res2d);

                String physBodyDebugString;
                physBodyDebugString.setTextFormat("PhysBody: %s",bodyDataTemplate.getBoneName().c_str());
                GFX_ADAPTER->drawDBGText(physBodyDebugString, res2d.m_x, res2d.m_y);
            }
        }
    }
}

void SoftPlatformComponent::onEditorMove(bbool _modifyInitialPos)
{
    syncPhysWithAnim();
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(SoftPlatformComponent_Template)
BEGIN_SERIALIZATION(SoftPlatformComponent_Template)

    SERIALIZE_CONTAINER_OBJECT("softPlatformParticles", m_bodies);
    SERIALIZE_CONTAINER_OBJECT("softPlatformConstraints",m_constraints);
    SERIALIZE_MEMBER("weightMultiplier", m_weightMultiplier);
    SERIALIZE_MEMBER("landSpeedMultiplier", m_landSpeedMultiplier);
    SERIALIZE_MEMBER("hitForceMultiplier", m_hitForceMultiplier);
    SERIALIZE_MEMBER("impulseMultiplier", m_impulseMultiplier);
    SERIALIZE_MEMBER("movingPolylineForce", m_movingPolylineForce);
    SERIALIZE_MEMBER("gameMaterial", m_gameMaterial);
    SERIALIZE_MEMBER("usePhantom", m_usePhantom);

END_SERIALIZATION()

SoftPlatformComponent_Template::SoftPlatformComponent_Template()
: m_weightMultiplier(1.f)
, m_landSpeedMultiplier(1.f)
, m_hitForceMultiplier(1.f)
, m_impulseMultiplier(1.f)
, m_movingPolylineForce(0.f)
, m_usePhantom(bfalse)
{
}

BEGIN_SERIALIZATION_SUBCLASS(SoftPlatformComponent_Template, BodyData)

    SERIALIZE_MEMBER("bone", m_boneName);
    SERIALIZE_MEMBER("static", m_static);
    SERIALIZE_MEMBER("gravityMultiplier",m_gravityMultiplier);
    SERIALIZE_MEMBER("windMultiplier",m_windMultiplier);
    SERIALIZE_MEMBER("useStaticAngle",m_useStaticAngle);
    SERIALIZE_MEMBER("setAngle", m_setAngle);

END_SERIALIZATION()

SoftPlatformComponent_Template::BodyData::BodyData()
: m_gravityMultiplier(1.f)
, m_windMultiplier(1.f)
, m_setAngle(Angle::Zero)
, m_static(bfalse)
, m_useStaticAngle(bfalse)
{
}

BEGIN_SERIALIZATION_SUBCLASS(SoftPlatformComponent_Template,ConstraintData)

    SERIALIZE_MEMBER("bodyA",m_bodyAName);
    SERIALIZE_MEMBER("bodyB",m_bodyBName);
    SERIALIZE_MEMBER("minAngle",m_minAngle);
    SERIALIZE_MEMBER("maxAngle",m_maxAngle);
    SERIALIZE_MEMBER("minLength",m_minLength);
    SERIALIZE_MEMBER("maxLength",m_maxLength);
    SERIALIZE_MEMBER("stiff",m_stiff);
    SERIALIZE_MEMBER("damp",m_damp);
    SERIALIZE_MEMBER("limitAngle",m_limitAngle);
    SERIALIZE_MEMBER("relaxLength",m_relaxLength);

END_SERIALIZATION()

SoftPlatformComponent_Template::ConstraintData::ConstraintData()
: m_minAngle(bfalse,0.f)
, m_maxAngle(bfalse,0.f)
, m_minLength(0.f)
, m_maxLength(0.f)
, m_stiff(1.f)
, m_damp(0.f)
, m_limitAngle(bfalse)
, m_relaxLength(bfalse)
{
}

}

