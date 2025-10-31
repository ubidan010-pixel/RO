#include "precompiled_gameplay.h"

#ifndef _ITF_HINGEPLATFORMCOMPONENT_H_
#include "gameplay/Components/Environment/HingePlatformComponent.h"
#endif //_ITF_HINGEPLATFORMCOMPONENT_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(HingePlatformComponent)
BEGIN_SERIALIZATION_CHILD(HingePlatformComponent)
END_SERIALIZATION()

HingePlatformComponent::HingeBoneData::HingeBoneData()
: m_index(U32_INVALID)
, m_leafIndex(U32_INVALID)
, m_parentBoneIndex(U32_INVALID)
, m_parentHingeIndex(U32_INVALID)
, m_targetAngle(0.f)
, m_currentAngle(0.f)
, m_currentSpeed(0.f)
, m_currentScale(Vec2d::Zero)
, m_localGravity(Vec2d::Zero)
, m_previousTarget(Vec2d::Zero)
, m_polyline(NULL)
, m_polyShape(NULL)
, m_phantom(NULL)
{
}

HingePlatformComponent::PlatformData::PlatformData()
: m_scaleBoneIndex(U32_INVALID)
, m_scaleHingeIndex(U32_INVALID)
, m_linkBoneIndex(U32_INVALID)
, m_linkHingeIndex(U32_INVALID)
, m_currentScaleSpeed(0.f)
, m_targetScale(0.f)
, m_polyline(NULL)
, m_polyShape(NULL)
, m_phantom(NULL)
{
}

const f32 HingePlatformComponent::m_strength2Angle = 0.05f;

HingePlatformComponent::HingePlatformComponent()
: m_currentGravityDir(Vec2d(0.f,-1.f))
, m_thisFrameDt(0.f)
, m_hingeRegistered(bfalse)
{

}

HingePlatformComponent::~HingePlatformComponent()
{
    clear();
}

void HingePlatformComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_hingeBones.resize(getTemplate()->getHingeBoneList().size());
    m_platforms.resize(getTemplate()->getPlatformList().size());
}

void HingePlatformComponent::readModel()
{
    Super::readModel();

    if ( !m_ready || m_hingeBones.size() == 0 )
    {
        return;
    }

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    const u32 uSize = m_hingeBones.size();

    m_currentWeights.resize(uSize);
    m_currentForces.resize(uSize);

    m_currentWeights.fillZero();
    m_currentForces.fillZero();

    // Read the data from the skeleton
    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();

    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        HingeBoneData& hingeData = m_hingeBones[i];
        const HingePlatformComponent_Template::HingeBoneData& hingeDataTemplate = getTemplate()->getHingeBoneList()[i];
        AnimBone* bone;

        hingeData.m_index = m_animComponent->getBoneIndex(hingeDataTemplate.getName());

        if ( hingeData.m_index == U32_INVALID )
        {
            ITF_ERROR("HingeBoneData: %s - %s\n\tInvalid bone name %s",
                m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), hingeDataTemplate.getName().getDebugString());
            bone = NULL;
        }
        else
        {
            bone = m_animComponent->getCurSubAnim(0)->getSkeleton()->getBoneAt(hingeData.m_index);
        }

        if ( bone && boneDynList.size() > hingeData.m_index )
        {
            AnimBoneDyn* boneDyn   = &boneDynList[hingeData.m_index];
            hingeData.m_currentAngle    = 0.f;
            hingeData.m_targetAngle     = 0.f;
            hingeData.m_currentSpeed    = 0.f;
            hingeData.m_currentScale    = boneDyn->m_Scale;
            hingeData.m_previousTarget  = boneDyn->m_PosEnd;

            if ( bone->m_BoneParent )
            {
                hingeData.m_parentBoneIndex = m_animComponent->getBoneIndex(bone->m_BoneParent->m_Name);
            }
            else
            {
                hingeData.m_parentBoneIndex = U32_INVALID;
            }

            if( !hingeDataTemplate.getDisableCollision() )
            {
                hingeData.m_polyline = new PolyLine;
                hingeData.m_polyShape = new PhysShapePolyline;
                hingeData.m_polyline->m_isCollisionActive = bfalse;
                hingeData.m_polyline->setOwnerActor(m_actor);
                hingeData.m_polyline->addPoint(boneDyn->m_Pos);
                hingeData.m_polyline->addPoint(boneDyn->m_PosEnd);
                hingeData.m_polyShape->setPolyline(hingeData.m_polyline);

                PhysPhantomInitCommonInfo initInfo;

                SET_OWNER(initInfo, m_actor->getRef());
                initInfo.m_shape = hingeData.m_polyShape;
                initInfo.m_pos = m_actor->get2DPos();
                initInfo.m_angle = m_actor->getAngle();
                initInfo.m_userData = hingeData.m_polyline->getRef();
                initInfo.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;

                hingeData.m_phantom = PHYSWORLD->allocPhantom(initInfo);
                hingeData.m_phantom->updateAABB();

                if ( m_hingeRegistered )
                {
                    PHYSWORLD->insertPhantom(hingeData.m_phantom,m_actor->getDepth());
                }
            }
        }
    }

    // Now that each bone has its data, register each children on its parent
    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        HingeBoneData& hingeData = m_hingeBones[i];

        if ( hingeData.m_parentBoneIndex != U32_INVALID )
        {
            HingeBoneData* parentData = findHingeDataFromBoneIndex(hingeData.m_parentBoneIndex,hingeData.m_parentHingeIndex);

            if ( parentData )
            {
                parentData->m_children.push_back(i);
            }
            else
            {
                // If the father was not found it means it wasn't registered, so it will act as a root
                hingeData.m_parentBoneIndex = U32_INVALID;
                hingeData.m_parentHingeIndex = U32_INVALID;
            }
        }
        else
        {
            hingeData.m_parentHingeIndex = U32_INVALID;
        }
    }

    // We store the roots and the leafs
    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        HingeBoneData& hingeData = m_hingeBones[i];

        if ( !hingeData.m_children.size() )
        {
            m_leafs.push_back(LeafData());

            hingeData.m_leafIndex = m_leafs.size()-1;

            LeafData& leafData = m_leafs[hingeData.m_leafIndex];
            leafData.m_hingeIndex = i;
        }
    }

    for ( u32 i = 0; i < m_platforms.size(); i++ )
    {
        PlatformData& platformData = m_platforms[i];
        const HingePlatformComponent_Template::PlatformData& platformDataTemplate = getTemplate()->getPlatformList()[i];

        platformData.m_linkBoneIndex = m_animComponent->getBoneIndex(platformDataTemplate.getLinkBone());
        platformData.m_scaleBoneIndex = m_animComponent->getBoneIndex(platformDataTemplate.getScaleBone());
        platformData.m_linkHingeIndex = U32_INVALID;
        platformData.m_scaleHingeIndex = U32_INVALID;
        platformData.m_currentScaleSpeed = 0.f;
        platformData.m_targetScale = 1.f;

        if( !platformDataTemplate.getDisableCollision() )
        {
            ITF_ASSERT( platformData.m_polyline == NULL );
            ITF_ASSERT( platformData.m_polyShape == NULL );
            ITF_ASSERT( platformData.m_phantom == NULL );
            platformData.m_polyline = newAlloc(mId_Gameplay, PolyLine());
            platformData.m_polyShape = newAlloc(mId_Gameplay, PhysShapePolyline());
            platformData.m_polyline->m_isCollisionActive = bfalse;
            platformData.m_polyline->setOwnerActor(m_actor);
            
            //get platform polyline
            u32 numPoints = 0;
            AnimPolyline::PolylineType polylineType = AnimPolyline::shape_error;
            Vec2d * polyline = m_animComponent->getCurrentPolylinePointBuffer(platformDataTemplate.getPolylineAnimRef(), &numPoints, &polylineType);
            if( polyline != NULL )
            {
                for( u32 point = 0; point < numPoints; ++point)
                {
                    platformData.m_polyline->addPoint( polyline[point] );
                }
            }
            platformData.m_polyShape->setPolyline(platformData.m_polyline);

            PhysPhantomInitCommonInfo initInfo;
            SET_OWNER(initInfo, m_actor->getRef());
            initInfo.m_shape = platformData.m_polyShape;
            initInfo.m_pos = m_actor->get2DPos();
            initInfo.m_angle = m_actor->getAngle();
            initInfo.m_userData = platformData.m_polyline->getRef();
            initInfo.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;

            platformData.m_phantom = PHYSWORLD->allocPhantom(initInfo);
            platformData.m_phantom->updateAABB();
            if ( m_hingeRegistered )
            {
                PHYSWORLD->insertPhantom(platformData.m_phantom,m_actor->getDepth());
            }
        }

        findHingeDataFromBoneIndex(platformData.m_linkBoneIndex,platformData.m_linkHingeIndex);
        findHingeDataFromBoneIndex(platformData.m_scaleBoneIndex,platformData.m_scaleHingeIndex);
    }

    // Reorder the bones so that we process the children before its parents
    ITF_VECTOR <bbool> processed;

    processed.resize(m_hingeBones.size());
    std::fill(processed.begin(),processed.end(),0);
        
    int pos = 0;
    for ( u32 hingeIndex = 0; hingeIndex < m_hingeBones.size(); hingeIndex++, pos++ )
    {
        orderBonesRecursive(hingeIndex,processed);
    }
}

void HingePlatformComponent::onBecomeActive()
{
    if ( !m_hingeRegistered )
    {
        for ( u32 i = 0; i < m_hingeBones.size(); i++ )
        {
            HingeBoneData& hingeData = m_hingeBones[i];

            if ( hingeData.m_phantom )
            {
                PHYSWORLD->insertPhantom(hingeData.m_phantom,m_actor->getDepth());
            }
        }

        for ( u32 i = 0; i < m_platforms.size(); i++ )
        {
            PlatformData& platformData = m_platforms[i];

            if ( platformData.m_phantom )
            {
                PHYSWORLD->insertPhantom(platformData.m_phantom,m_actor->getDepth());
            }
        }

        m_hingeRegistered = btrue;
    }

    Super::onBecomeActive();
}

void HingePlatformComponent::onBecomeInactive()
{
    if ( m_hingeRegistered )
    {
        for ( u32 i = 0; i < m_hingeBones.size(); i++ )
        {
            HingeBoneData& hingeData = m_hingeBones[i];

            if ( hingeData.m_phantom )
            {
                PHYSWORLD->removePhantom(hingeData.m_phantom);
            }
        }

        for ( u32 i = 0; i < m_platforms.size(); i++ )
        {
            PlatformData& platformData = m_platforms[i];

            if ( platformData.m_phantom )
            {
                PHYSWORLD->removePhantom(platformData.m_phantom);
            }
        }

        m_hingeRegistered = bfalse;
    }

    Super::onBecomeInactive();
}

void HingePlatformComponent::processHitStim( HitStim* _hit )
{
    // We received a hit. First let's find the polyline that receive it.
    // This can be a hinge bone or a platform
    const PhysContactsContainer& contacts = _hit->getContacts();
    
    //try to get bones list from anim
    AnimMeshScene* animMesh = m_animComponent->getAnimMeshScene();
    const SafeArray<AnimBoneDyn> * boneDynList = NULL;
    if( animMesh != NULL )
    {
        boneDynList = &animMesh->m_AnimInfo.getCurrentBoneList();
    }

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        for ( u32 i = 0; i < m_hingeBones.size(); i++ )
        {
            HingeBoneData& hingeData = m_hingeBones[i];

            if ( hingeData.m_phantom && hingeData.m_phantom->getRef() == contact.m_collidable )
            {
                const HingePlatformComponent_Template::HingeBoneData& hingeDataTemplate = getTemplate()->getHingeBoneList()[i];
                const PolyLineEdge& edge = hingeData.m_polyline->getEdgeAt(contact.m_edgeIndex);
                f32 edgeAngle = getVec2Angle(edge.m_normalizedVector);
                Vec2d hitDirLocal = _hit->getDirection().Rotate(-edgeAngle);
                f32 hitStrength = hitDirLocal.m_y * (_hit->getLevel()+1.f) * hingeDataTemplate.getHitForce();
                f32 resistanceScale = hingeDataTemplate.getResistance() / 20.f;

                resistanceScale = Clamp(resistanceScale,0.f,1.f);

                m_currentForces[i] += hitStrength * (1.f - resistanceScale);

                break;
            }
        }

        //if bone list is valid
        if( boneDynList != NULL )
        {
            //check contact with each platform
            for ( u32 i = 0; i < m_platforms.size(); i++ )
            {
                //check if platform is able to receive contact and if platform is hit
                PlatformData& platformData = m_platforms[i];
                if ( platformData.m_phantom && platformData.m_phantom->getRef() == contact.m_collidable )
                {
                    //get hinge bone data
                    HingeBoneData& hinge = m_hingeBones[platformData.m_linkHingeIndex];
                    if ( hinge.m_leafIndex != U32_INVALID && hinge.m_index != U32_INVALID )
                    {
                        //get platform template data
                        const HingePlatformComponent_Template::PlatformData& platformDataTemplate = getTemplate()->getPlatformList()[i];

                        const Vec2d & hitDirection = _hit->getDirection();
                        const bbool verticalHit = (f32_Abs(hitDirection.m_x) < MTH_EPSILON) && (hitDirection.m_y < -MTH_EPSILON);
                        const bbool validAttack = !platformDataTemplate.getOnlyCrushAttack() || verticalHit;
                        if( validAttack )
                        {
                            //compute hit force according to contact position
                            const Vec2d hingePos = (*boneDynList)[hinge.m_index].m_Pos;
                            const Vec2d dirToActor = ( contact.m_contactPoint - hingePos ).Rotate(-m_actor->getAngle());
                            const Vec2d dirLocal = transformVectorToLocalLeaf(m_leafs[hinge.m_leafIndex],dirToActor).normalize();
                            const f32 angle = f32_ACos(Vec2d::Right.dot(dirLocal));
                            const f32 crossPRod = Vec2d::Right.cross(dirLocal);
                            const f32 moveSign = crossPRod > 0.f ? 1.f : -1.f;
                            const f32 maxPitch = platformDataTemplate.getMaxPitch().ToRadians();
                            const f32 positionForce = Min(angle, maxPitch) / maxPitch;
                            
                            f32 platformAngleLimitationFactor = 1.0f;
                            //if current angle and move sign are in the same direction
                            if( moveSign * hinge.m_currentAngle > 0.0f )
                            {
                                //we need to apply a factor to reduce hit influence near max pitch value
                                platformAngleLimitationFactor = (maxPitch - f32_Min(f32_Abs(hinge.m_currentAngle), maxPitch)) / maxPitch;
                            }

                            //modify speed with force
                            hinge.m_currentSpeed += moveSign * positionForce * platformAngleLimitationFactor * platformDataTemplate.getHitForceMultiplier();
                        }
                    }                
                    break;
                }
            }
        }
    }
}

void HingePlatformComponent::processQueryBlockHits( EventQueryBlocksHits* _query )
{
    const PhysContactsContainer* contacts = _query->getContacts();

    if ( !contacts )
    {
        return;
    }

    u32 numHinges = m_hingeBones.size();

    if ( numHinges )
    {
        u32 numContacts = contacts->size();

        for ( u32 contactIndex = 0; contactIndex < numContacts; contactIndex++ )
        {
            const SCollidableContact& contact = (*contacts)[contactIndex];

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                for ( u32 i = 0; i < numHinges; i++ )
                {
                    HingeBoneData& hingeData = m_hingeBones[i];

                    if ( hingeData.m_polyline &&
                         hingeData.m_polyline->getRef() == contact.m_collidableUserData )
                    {
                        _query->setCanBlockHits(bfalse);
                        return;
                    }
                }
            }
        }
    }

    Super::processQueryBlockHits(_query);
}

// This recursive function adds all the bones into m_orderedBones in an ordered way so that the children
// go first, before than their parents
void HingePlatformComponent::orderBonesRecursive( u32 _hingeIndex, ITF_VECTOR<bbool>& _processed )
{
    HingeBoneData* hinge = &m_hingeBones[_hingeIndex];

    if ( !_processed[_hingeIndex] )
    {
        for ( u32 childIndex = 0; childIndex < hinge->m_children.size(); childIndex++ )
        {
            u32 childHingeIndex = hinge->m_children[childIndex];

            if ( !_processed[childHingeIndex] )
            {
                orderBonesRecursive(childHingeIndex,_processed);
            }
        }

        m_orderedBones.push_back(_hingeIndex);
        _processed[_hingeIndex] = btrue;
    }
}

HingePlatformComponent::HingeBoneData* HingePlatformComponent::findHingeDataFromBoneIndex( u32 _boneIndex, u32& _index )
{
    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        if ( m_hingeBones[i].m_index == _boneIndex )
        {
            _index = i;
            return &m_hingeBones[i];
        }
    }

    return NULL;
}

void HingePlatformComponent::processPhysBodyForceEvent( class EventForceTransfer* _physBodyForce )
{
    u32 platformIndex;
    const HingePlatformComponent_Template::PlatformData* platform = getTemplate()->findPlatformDataFromPolylineRef(_physBodyForce->getPolyID(),platformIndex);

    if ( platform )
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
                    applyForceOnPlatform(poly->m_poly->getPosAt(i),
                                         _physBodyForce->getForce(),platform->getMaxPitch().ToRadians(),poly,m_platforms[platformIndex]);
                    break;
                }
            }
        }
    }
}

void HingePlatformComponent::Update( f32 _deltaTime )
{
    if ( !m_ready )
        return;

    m_currentGravityDir = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
    m_currentGravityDir.normalize();
    m_thisFrameDt = _deltaTime;

    storeLeafLocalAngle();
    calculateCurrentWeights();
    calculateTargetAngle();
    updateMovingPolylines();
    addForces();
    applyForces(_deltaTime);
    updatePlatforms(_deltaTime);
    updateTransforms(_deltaTime);

    m_currentForces.fillZero();
}

void HingePlatformComponent::storeLeafLocalAngle()
{
    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( !animMeshScene || !m_animComponent->getNumPlayingSubAnims() )
    {
        return;
    }

    AnimSkeleton* skeleton = m_animComponent->getCurSubAnim(0)->getSkeleton();

    for ( LeafList::iterator it = m_leafs.begin(); it != m_leafs.end(); ++it )
    {
        LeafData& leaf = *it;
        HingeBoneData& hingeData = m_hingeBones[it->m_hingeIndex];

        if ( hingeData.m_index != U32_INVALID )
        {
            AnimBone* animBone = skeleton->getBoneAt(hingeData.m_index);

            leaf.m_parentsAngle.clear();

            do 
            {
                leaf.m_parentsAngle.push_back(animMeshScene->m_AnimInfo.getCurrentBoneList()[animBone->getIndex()].m_AngleLocal);
                animBone = animBone->m_BoneParent;
            } while ( animBone );
        }
    }
}

// Update the polylines used by the phantoms and set the previous target positions
void HingePlatformComponent::updateHinges()
{
    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( !animMeshScene )
    {
        return;
    }

    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        HingeBoneData& hingeData = m_hingeBones[i];
        const HingePlatformComponent_Template::HingeBoneData& hingeDataTemplate = getTemplate()->getHingeBoneList()[i];

        if ( hingeData.m_index == U32_INVALID )
        {
            continue;
        }

        const AnimBoneDyn* boneDyn = &boneDynList[hingeData.m_index];

        if ( hingeData.m_phantom )
        {
            Vec2d posA = boneDyn->m_Pos;
            Vec2d posB = boneDyn->m_PosEnd;

            hingeData.m_polyline->setPosAt(posA,0);
            hingeData.m_polyline->setPosAt(posB,1);
            hingeData.m_polyline->forceRecomputeData();
            hingeData.m_phantom->setRecomputeAABB();
            hingeData.m_phantom->updateAABB();
        }

        if ( !hingeData.m_previousTarget.IsEqual(boneDyn->m_PosEnd,0.001f) )
        {
            Vec2d prevDir = hingeData.m_previousTarget - boneDyn->m_Pos;
            Vec2d currentDir =  boneDyn->m_PosEnd - boneDyn->m_Pos;
            f32 dif = ( hingeData.m_previousTarget - boneDyn->m_PosEnd ).norm();

            prevDir.normalize();
            currentDir.normalize();

            f32 crossProd = prevDir.cross(currentDir);

            if ( crossProd < -MTH_EPSILON )
            {
                hingeData.m_currentSpeed += dif * m_thisFrameDt * hingeDataTemplate.getDelayBranchMultiplier();
            }
            else if ( crossProd > MTH_EPSILON )
            {
                hingeData.m_currentSpeed -= dif * m_thisFrameDt * hingeDataTemplate.getDelayBranchMultiplier();
            }
        }

        hingeData.m_previousTarget = boneDyn->m_PosEnd;
    }

    for( u32 i = 0; i < m_platforms.size(); ++i )
    {
        PlatformData & platformData = m_platforms[i];
        const HingePlatformComponent_Template::PlatformData& platformDataTemplate = getTemplate()->getPlatformList()[i];

        if( platformData.m_phantom )
        {
            //get platform polyline
            u32 numPoints = 0;
            AnimPolyline::PolylineType polylineType = AnimPolyline::shape_error;
            Vec2d * polyline = m_animComponent->getCurrentPolylinePointBuffer(platformDataTemplate.getPolylineAnimRef(), &numPoints, &polylineType);
            if( polyline != NULL )
            {
                for( u32 point = 0; point < numPoints; ++point)
                {
                    platformData.m_polyline->setPosAt( polyline[point], point );
                }
            }
            
            platformData.m_polyline->forceRecomputeData();
            platformData.m_phantom->setRecomputeAABB();
            platformData.m_phantom->updateAABB();
        }
    }
}

// We calculate how much weight each bone is holding
// Weight is applied from the default weight of each bone and
// the weight of a character standing on it
void HingePlatformComponent::calculateCurrentWeights()
{
    // First we clear out the weight of every bone
    m_currentWeights.fillZero();

    // Now we apply the weight of the people using the polylines
    for ( UsersList::const_iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        const PolyUsers& user = *it;
        u32 platformIndex;
        const HingePlatformComponent_Template::PlatformData* platform = getTemplate()->findPlatformDataFromPolylineRef(user.m_polylineRef,platformIndex);
        ProceduralPolyline* procPoly = getProcPolylineFromAnimRef(user.m_polylineRef);

        f32 weightMultiplier;

        if ( procPoly )
        {
            const PolylineParameters* params = getPolylineParametersFromPolyPoint(procPoly,user.m_edgeIndex);
            weightMultiplier = params->m_weightMultiplier;
        }
        else
        {
            weightMultiplier = 1.f;
        }

        if ( platform )
        {
            if ( m_platforms[platformIndex].m_linkHingeIndex != U32_INVALID )
            {
                m_currentWeights[m_platforms[platformIndex].m_linkHingeIndex] += user.m_weight*weightMultiplier;
            }
        }
        else if ( procPoly )
        {
            // we try to find which hinge contains this polyline edge and add the weight to it
            u32 hingeIndex;

            HingeBoneData* hinge = findHingeDataFromBoneIndex(procPoly->m_pointsInfo[user.m_edgeIndex].m_boneIndex,hingeIndex);

            if ( hinge )
            {
                PolyLine* poly = procPoly->m_poly;
                const PolyLineEdge* edge = ( user.m_edgeIndex < poly->getVectorsCount() ) ? &poly->getEdgeAt(user.m_edgeIndex) : NULL;

                if ( edge )
                {
                    Actor* actor = AIUtils::getActor(user.m_actor);

                    if ( actor )
                    {
                        // Transform the actor position into local space of the edge to see
                        // how the weight is distributed
                        f32 angle = getVec2Angle(edge->m_normalizedVector);
                        Vec2d localPos = actor->get2DPos() - edge->getPos();
                        localPos = localPos.Rotate(-angle);
                        f32 t = localPos.m_x/edge->m_length;
                        t = Clamp(t,0.f,1.f);

                        m_currentWeights[hingeIndex] += user.m_weight*weightMultiplier*(1.f-t);

                        if ( t )
                        {
                            // Share the weight with the next edge
                            if ( user.m_edgeIndex < poly->getPosCount() )
                            {
                                u32 nextHingeIndex;
                                HingeBoneData* nextHinge = findHingeDataFromBoneIndex(procPoly->m_pointsInfo[user.m_edgeIndex+1].m_boneIndex,nextHingeIndex);

                                if ( nextHinge )
                                {
                                    m_currentWeights[nextHingeIndex] += user.m_weight*weightMultiplier*t;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Now apply the weight of the people hanging from the polylines
    for ( HangingList::const_iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
    {
        u32 platformIndex;
        const HangingActor& hanging = *it;
        const HingePlatformComponent_Template::PlatformData* platform = getTemplate()->findPlatformDataFromPolylineRef(hanging.m_polylineRef,platformIndex);

        if ( !platform ||
             m_platforms[platformIndex].m_linkHingeIndex == U32_INVALID ||
             m_platforms[platformIndex].m_linkHingeIndex >= m_currentWeights.size() )
        {
            continue;
        }

        ProceduralPolyline* procPoly = getProcPolylineFromAnimRef(hanging.m_polylineRef);

        f32 weightMultiplier;

        if ( procPoly )
        {
            const PolylineParameters* params = getPolylineParametersFromPolyPoint(procPoly,hanging.m_edgeIndex);
            weightMultiplier = params->m_weightMultiplier;
        }
        else
        {
            weightMultiplier = 1.f;
        }

        m_currentWeights[m_platforms[platformIndex].m_linkHingeIndex] += hanging.m_weight*weightMultiplier;
    }

    // Then we propagate the weights of every bone to its parents
    for ( u32 i = 0; i < m_orderedBones.size(); i++ )
    {
        u32 hingeIndex = m_orderedBones[i];
        HingeBoneData& hinge = m_hingeBones[hingeIndex];
        const HingePlatformComponent_Template::HingeBoneData& hingeTemplate = getTemplate()->getHingeBoneList()[hingeIndex];

        m_currentWeights[hingeIndex] += hingeTemplate.getWeight();

        for ( u32 childIndex = 0; childIndex < hinge.m_children.size(); childIndex++ )
        {
            u32 childHingeIndex = hinge.m_children[childIndex];

            m_currentWeights[hingeIndex] += m_currentWeights[childHingeIndex];
        }
    }
}

// Calculate how much each bone rotates
void HingePlatformComponent::calculateTargetAngle()
{
    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
    Vec2d gravityLocal = m_currentGravityDir.Rotate(-m_actor->getAngle());

    for ( u32 i = 0; i < m_orderedBones.size(); i++ )
    {
        u32 hingeIndex = m_orderedBones[i];
        HingeBoneData& hinge = m_hingeBones[hingeIndex];
        const HingePlatformComponent_Template::HingeBoneData& hingeTemplate = getTemplate()->getHingeBoneList()[hingeIndex];

        if ( hinge.m_children.size() == 0 )
        {
            // Leafs always point towards initial angle (0)
            // Unless it's a platform and someone is standing on it, this case in handled on
            // updatePlatforms
            hinge.m_targetAngle = 0.f;

            for ( u32 leafIndex = 0; leafIndex < m_leafs.size(); leafIndex++ )
            {
                LeafData& leafData = m_leafs[leafIndex];

                if ( leafData.m_hingeIndex == hingeIndex )
                {
                    hinge.m_localGravity = transformVectorToLocalLeaf(leafData,gravityLocal);
                    break;
                }
            }
        }
        else
        {
            // Calculate the local gravity for this bone. We only need to transform it from one of the
            // children, so we take it from the first
            u32 childHingeIndex = hinge.m_children[0];
            HingeBoneData& childHinge = m_hingeBones[childHingeIndex];

            if ( childHinge.m_index != U32_INVALID )
            {
                f32 angleLocal = boneDynList[childHinge.m_index].m_AngleLocal;
                hinge.m_localGravity = childHinge.m_localGravity.Rotate(angleLocal);

                f32 targetAngle = 0.f;

                // For every children we check on which side of the gravity it is. If it's on the left we rotate
                // left, if it's right we rotate right
                for ( u32 i = 0; i < hinge.m_children.size(); i++ )
                {
                    u32 childIndex = hinge.m_children[i];
                    HingeBoneData& child = m_hingeBones[childIndex];

                    if ( child.m_index != U32_INVALID )
                    {
                        f32 angleLocal = boneDynList[child.m_index].m_AngleLocal;
                        Vec2d childDir = Vec2d::Right.Rotate(angleLocal+child.m_targetAngle);
                        f32 crossProd = hinge.m_localGravity.cross(childDir);

                        if ( crossProd > 0.f )
                        {
                            targetAngle -= m_currentWeights[childIndex]*m_strength2Angle;
                        }
                        else
                        {
                            targetAngle += m_currentWeights[childIndex]*m_strength2Angle;
                        }
                    }
                }

                if ( targetAngle )
                {
                    // Now apply the resistance of each branch to bend
                    f32 resistance = hingeTemplate.getResistance() * m_strength2Angle;
                    if ( targetAngle > 0.f )
                    {
                        targetAngle = Max(targetAngle-resistance,0.f);
                    }
                    else if ( targetAngle < 0.f )
                    {
                        targetAngle = Min(targetAngle+resistance,0.f);
                    }

                    // And finally we clamp the angle within some limits
                    targetAngle = ClampAngle(Angle(bfalse,targetAngle),hingeTemplate.getMinAngle(),hingeTemplate.getMaxAngle()).ToRadians();
                }

                hinge.m_targetAngle = targetAngle;
            }
        }
    }
}

f32 HingePlatformComponent::getScaleForWeight( f32 _min, f32 _max, f32 _scaleMin, f32 _scaleMax, f32 _weight ) const
{
    _weight = Clamp(_weight,_min,_max);

    f32 t;

    if ( _max != _min )
    {
        t = ( _weight - _min ) / ( _max - _min );
    }
    else
    {
        t = 0.f;
    }

    return Interpolate(_scaleMin,_scaleMax,t);
}

// This sets the current scale of the platform hinges
void HingePlatformComponent::updatePlatforms( f32 _delta )
{
    // First reset the target scale of each platform
    for ( u32 platformIndex = 0; platformIndex < m_platforms.size(); platformIndex++ )
    {
        m_platforms[platformIndex].m_targetScale = 1.f;
    }

    AnimMeshScene* animMesh = m_animComponent->getAnimMeshScene();

    if ( !animMesh )
    {
        return;
    }

    SafeArray<AnimBoneDyn> & boneDynList = animMesh->m_AnimInfo.getCurrentBoneList();

    // Add a rotation for each person standing on the platform and calculate the target scale
    for ( UsersList::const_iterator itUsers = m_users.begin(); itUsers != m_users.end(); ++itUsers )
    {
        const PolyUsers& user = *itUsers;
        BaseObject* obj = GETOBJECT(user.m_actor);

        if ( obj )
        {
            Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

            if ( actor )
            {
                for ( u32 platformIndex = 0; platformIndex < m_platforms.size(); platformIndex++ )
                {
                    PlatformData& platform = m_platforms[platformIndex];
                    const HingePlatformComponent_Template::PlatformData& platformTemplate = getTemplate()->getPlatformList()[platformIndex];

                    if ( platformTemplate.getPolylineAnimRef() == user.m_polylineRef )
                    {
                        f32 scale = getScaleForWeight(platformTemplate.getScaleMinWeight(),platformTemplate.getScaleMaxWeight(),
                                                      platformTemplate.getScaleMinWeightScale(),platformTemplate.getScaleMaxWeightScale(),
                                                      user.m_weight);
                        platform.m_targetScale = Max(platform.m_targetScale-scale,platformTemplate.getMinScale());

                        if( user.m_weight >= platformTemplate.getHingeMinWeight() )
                        {
                            if ( platform.m_linkHingeIndex != U32_INVALID )
                            {
                                ProceduralPolyline* procPoly = getProcPolylineFromAnimRef(user.m_polylineRef);

                                if ( procPoly )
                                {
                                    HingeBoneData& hinge = m_hingeBones[platform.m_linkHingeIndex];

                                    if ( hinge.m_leafIndex != U32_INVALID && hinge.m_index != U32_INVALID )
                                    {
                                        Vec2d hingePos = boneDynList[hinge.m_index].m_Pos;
                                        Vec2d dirToActor = ( actor->get2DPos() - hingePos ).Rotate(-m_actor->getAngle());
                                        Vec2d dirLocal = transformVectorToLocalLeaf(m_leafs[hinge.m_leafIndex],dirToActor);
                                        dirLocal.normalize();

                                        f32 angle = f32_ACos(Vec2d::Right.dot(dirLocal));
                                        f32 crossPRod = Vec2d::Right.cross(dirLocal);
                                        f32 moveSign = crossPRod > 0.f ? 1.f : -1.f;
                                        f32 t = Min(angle,platformTemplate.getMaxPitch().ToRadians())/platformTemplate.getMaxPitch().ToRadians();

                                        hinge.m_targetAngle += platformTemplate.getMaxPitch().ToRadians() * t * moveSign;
                                    }
                                }
                            }
                        }

                        break;
                    }
                }
            }
        }
    }

    // Add a rotation for each person hanging on the platform
    for ( HangingList::const_iterator itHanging = m_hanging.begin(); itHanging != m_hanging.end(); ++itHanging )
    {
        const HangingActor& hanging = *itHanging;

        for ( u32 platformIndex = 0; platformIndex < m_platforms.size(); platformIndex++ )
        {
            PlatformData& platform = m_platforms[platformIndex];

            if ( platform.m_linkHingeIndex != U32_INVALID )
            {
                const HingePlatformComponent_Template::PlatformData& platformTemplate = getTemplate()->getPlatformList()[platformIndex];

                if ( platformTemplate.getPolylineAnimRef() == hanging.m_polylineRef )
                {
                    HingeBoneData& hinge = m_hingeBones[platform.m_linkHingeIndex];
                    f32 scale = getScaleForWeight(platformTemplate.getScaleMinWeight(),platformTemplate.getScaleMaxWeight(),
                                                  platformTemplate.getScaleMinWeightScale(),platformTemplate.getScaleMaxWeightScale(),
                                                  hanging.m_weight);

                    platform.m_targetScale = Max(platform.m_targetScale-scale,platformTemplate.getMinScale());
                    hinge.m_targetAngle -= platformTemplate.getMaxPitch().ToRadians() * ( ( ( hanging.m_t * 2.f ) - 1.f ) );

                    break;
                }
            }
        }
    }

    // Update the scale of each platform with the target scale we calculated
    for ( u32 platformIndex = 0; platformIndex < m_platforms.size(); platformIndex++ )
    {
        PlatformData& platform = m_platforms[platformIndex];
        const HingePlatformComponent_Template::PlatformData& platformTemplate = getTemplate()->getPlatformList()[platformIndex];

        if ( platform.m_scaleHingeIndex == U32_INVALID )
        {
            continue;
        }

        HingeBoneData& hingeData = m_hingeBones[platform.m_scaleHingeIndex];

        f32 target = platform.m_targetScale - hingeData.m_currentScale.m_x;

        if ( !platform.m_currentScaleSpeed && !target )
        {
            continue;
        }

        // Update the current scale and speed of scaling
        f32 spring = (target*platformTemplate.getScalingStiff()) - (platform.m_currentScaleSpeed*platformTemplate.getScalingDamp());

        platform.m_currentScaleSpeed += spring * _delta;
        hingeData.m_currentScale.m_x += platform.m_currentScaleSpeed * _delta;

        // Limit the scale between [0,+]
        if ( hingeData.m_currentScale.m_x < 0.1f )
        {
            hingeData.m_currentScale.m_x = 0.1f;
            platform.m_currentScaleSpeed = 0.f;
        }
    }
}

// This sets the results of the process into the skeleton and the polylines
void HingePlatformComponent::updateTransforms( f32 _delta )
{
    if ( ! m_animComponent->isLoaded())
    {
        return;
    }

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
    
    // Apply the modifications to the skeleton
    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        HingeBoneData& hingeBone = m_hingeBones[i];
        const HingePlatformComponent_Template::HingeBoneData& hingeBoneTemplate = getTemplate()->getHingeBoneList()[i];

        //compute target angle
        f32 hingeTargetAngle = hingeBone.m_targetAngle;
        if( hingeBoneTemplate.getAlwaysApplyAngleLimitation() )
        {
            hingeTargetAngle = f32_Clamp(hingeTargetAngle, hingeBoneTemplate.getMinAngle().ToRadians(), hingeBoneTemplate.getMaxAngle().ToRadians());
        }            
        f32 target = hingeTargetAngle - hingeBone.m_currentAngle;

        //compute stiff
        f32 stiff = hingeBoneTemplate.getBranchStiff();        
        //if bone must use dynamic stiff
        if( hingeBoneTemplate.useDynamicBranchStiff() )
        {
            if( !hingeBoneTemplate.getDynamicBranchStiffOnlyWayBack() || (hingeBone.m_currentAngle * target < 0.0f) )
            {
                const f32 minAngle = hingeBoneTemplate.getDynamicBranchStiffMinAngle().ToRadians();
                const f32 maxAngle = hingeBoneTemplate.getDynamicBranchStiffMaxAngle().ToRadians();
                const f32 currentAbsAngle = f32_Abs( hingeBone.m_currentAngle );
                const f32 currentAbsClampedAngle = f32_Clamp( currentAbsAngle, minAngle, maxAngle );
                const f32 ratio = (maxAngle - currentAbsClampedAngle) / (maxAngle - minAngle);
                const f32 multiplier = f32_Lerp( hingeBoneTemplate.getDynamicBranchStiffMultiplier(), 1.0f, ratio );
                stiff = stiff * multiplier;
            }
        }

        //compute spring value
        f32 spring = ( target * stiff ) - ( hingeBone.m_currentSpeed * hingeBoneTemplate.getBranchDamp() );

        hingeBone.m_currentSpeed += spring*_delta;
        hingeBone.m_currentAngle += hingeBone.m_currentSpeed * _delta;


        if ( hingeBone.m_index != U32_INVALID )
        {
            AnimBoneDyn& boneDyn = boneDynList[hingeBone.m_index];

            boneDyn.m_AngleLocal  += hingeBone.m_currentAngle;
            
            if (!hingeBoneTemplate.getDisableScale())
            {
                boneDyn.m_Scale   = hingeBone.m_currentScale;
            }
        }
    }
}

// This transforms the gravity from world space into the local space of a leaf
Vec2d HingePlatformComponent::transformVectorToLocalLeaf( const LeafData& _leafData, const Vec2d& _vector ) const
{
    Vec2d ret = _vector;

    i32 count = static_cast<i32>(_leafData.m_parentsAngle.size())-1;
    
    while ( count >= 0 )
    {
        f32 angle = _leafData.m_parentsAngle[count];
        ret = ret.Rotate(-angle);
        count--;
    }

    return ret;
}

void HingePlatformComponent::processNewUser( EventStickOnPolyline* _event )
{
    Super::processNewUser(_event);

    if ( _event->isSticked() )
    {
        ProceduralPolyline* procPoly = getProcPolylineFromObjRef(_event->getPolyline());

        if ( procPoly )
        {
            for ( u32 i = 0; i < m_platforms.size(); i++ )
            {
                PlatformData& platform = m_platforms[i];
                const HingePlatformComponent_Template::PlatformData& platformTemplate = getTemplate()->getPlatformList()[i];

                if ( procPoly && platformTemplate.getPolylineAnimRef() == procPoly->m_animRef )
                {
                    const PolylineParameters* params = getPolylineParametersFromPolyPoint(procPoly,_event->getEdge());
                    Vec2d force = _event->getSpeed()*params->m_landSpeedMultiplier*_event->getWeight()*params->m_weightMultiplier;

                    applyForceOnPlatform(_event->getPos(),force,platformTemplate.getMaxPitch().ToRadians(),procPoly,platform);
                    break;
                }
            }
        }
    }
}

void HingePlatformComponent::processNewHanging( EventHanging* _event )
{
    Super::processNewHanging(_event);

    if ( _event->isHanging() )
    {
        ProceduralPolyline* procPoly = getProcPolylineFromObjRef(_event->getPolyline());

        if ( procPoly )
        {
            for ( u32 i = 0; i < m_platforms.size(); i++ )
            {
                PlatformData& platform = m_platforms[i];
                const HingePlatformComponent_Template::PlatformData& platformTemplate = getTemplate()->getPlatformList()[i];

                if ( platformTemplate.getPolylineAnimRef() == procPoly->m_animRef )
                {
                    const PolyLineEdge& edge = procPoly->m_poly->getEdgeAt(_event->getEdge());
                    const PolylineParameters* params = getPolylineParametersFromPolyPoint(procPoly,_event->getEdge());
                    Vec2d pos = edge.getPos() + ( edge.m_vector * _event->getT() );
                    Vec2d force = _event->getSpeed()*params->m_landSpeedMultiplier*_event->getWeight()*params->m_weightMultiplier;

                    applyForceOnPlatform(pos,force,platformTemplate.getMaxPitch().ToRadians(),procPoly,platform);
                    break;
                }
            }
        }
    }
}

void HingePlatformComponent::processAnimUpdated( EventAnimUpdated* _animUpdated )
{
    Super::processAnimUpdated(_animUpdated);

    if ( !m_ready )
    {
        return;
    }

    updateHinges();
}

void HingePlatformComponent::applyForceOnPlatform( const Vec2d& _pos, const Vec2d& _force, f32 _maxPitch, ProceduralPolyline* _procPoly, PlatformData& _platform )
{
    if ( _platform.m_linkHingeIndex == U32_INVALID )
    {
        return;
    }

    AnimMeshScene* animMesh = m_animComponent->getAnimMeshScene();

    if ( !animMesh )
    {
        return;
    }

    SafeArray<AnimBoneDyn> & boneDynList = animMesh->m_AnimInfo.getCurrentBoneList();

    HingeBoneData* hinge = &m_hingeBones[_platform.m_linkHingeIndex];

    if ( !hinge || hinge->m_leafIndex == U32_INVALID )
    {
        return;
    }

    if ( hinge->m_index > boneDynList.size() )
    {
        return;
    }

    Vec2d forceLocal = _force.Rotate(-m_actor->getAngle());
    forceLocal = transformVectorToLocalLeaf(m_leafs[hinge->m_leafIndex],forceLocal);

    _platform.m_currentScaleSpeed += Vec2d::Right.dot(forceLocal);

    f32 rotForce = Vec2d::Up.dot(forceLocal) * m_strength2Angle;
    Vec2d forceDir = forceLocal;

    forceDir.normalize();

    Vec2d hingePos = boneDynList[hinge->m_index].m_Pos;
    Vec2d dirToActor = ( _pos - hingePos ).Rotate(-m_actor->getAngle());
    Vec2d dirLocal = transformVectorToLocalLeaf(m_leafs[hinge->m_leafIndex],dirToActor);
    f32 crossPRod = Vec2d::Right.cross(forceDir);
    f32 moveSign = crossPRod > 0.f ? 1.f : -1.f;

    f32 angle = f32_ACos(Vec2d::Right.dot(dirLocal));
    f32 t = Min(f32_Abs(angle),_maxPitch)/_maxPitch;

    m_currentForces[_platform.m_linkHingeIndex] += rotForce * t * moveSign;
}

void HingePlatformComponent::updateMovingPolylines()
{
    for ( u32 i = 0; i < getTemplate()->getMovingPolylines().size(); i++ )
    {
        const HingePlatformComponent_Template::MovingPolylineData& movingData = getTemplate()->getMovingPolylines()[i];
        ProceduralPolyline* procPoly = getProcPolylineFromAnimRef(movingData.getPolyRef());

        if ( procPoly && procPoly->m_phantom )
        {
            processMovingPolyline(movingData.getForceMultiplier(),movingData.getResistance(),procPoly);
        }
    }
}

void HingePlatformComponent::processMovingPolyline( f32 _forceMultiplier, f32 _resistance, ProceduralPolyline* _procPoly )
{
    if ( !_procPoly->m_phantom )
    {
        return;
    }

    PhysContactsContainer contacts;

    PHYSWORLD->collidePhantoms(_procPoly->m_phantom->getPos(),
                               _procPoly->m_phantom->getPos(),
                               _procPoly->m_phantom->getAngle(),
                               _procPoly->m_phantom->getShape(),
                               m_actor->getDepth(),
                               ECOLLISIONFILTER_CHARACTERS,
                               contacts);

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& contact = contacts[i];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

        if ( !obj )
        {
            continue;
        }

        Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

        if ( !actor )
        {
            continue;
        }

        EventQueryPhysicsData physicsData;

        actor->onEvent(&physicsData);

        if ( physicsData.getWorldSpeed() != Vec2d::Zero )
        {
            u32 hingeIndex;
            HingeBoneData* hingeData = findHingeDataFromBoneIndex(_procPoly->m_pointsInfo[contact.m_edgeIndex].m_boneIndex,hingeIndex);

            if ( hingeData )
            {
                const PolyLineEdge& edge = hingeData->m_polyline->getEdgeAt(0);
                f32 edgeAngle = getVec2Angle(edge.m_normalizedVector);
                Vec2d moveDirLocal = physicsData.getWorldSpeed().Rotate(-edgeAngle);
                f32 moveStrength = moveDirLocal.m_y * _forceMultiplier;
                f32 resistanceScale = _resistance / 20.f;

                resistanceScale = Clamp(resistanceScale,0.f,1.f);

                m_currentForces[hingeIndex] += moveStrength * (1.f - resistanceScale);
            }
        }
    }
}

void HingePlatformComponent::addForces()
{
    AnimMeshScene* animMesh = m_animComponent->getAnimMeshScene();

    if ( animMesh )
    {
        SafeArray<AnimBoneDyn> & boneDynList = animMesh->m_AnimInfo.getCurrentBoneList();

        for ( u32 i = 0; i < m_hingeBones.size(); i++ )
        {
            HingeBoneData& hinge = m_hingeBones[i];
            const HingePlatformComponent_Template::HingeBoneData& hingeTemplate = getTemplate()->getHingeBoneList()[i];

            if ( hinge.m_index == U32_INVALID )
            {
                continue;
            }

            AnimBoneDyn& boneDyn = boneDynList[hinge.m_index];

            Vec2d endP = boneDyn.m_PosEnd;

            PhysActorForceArray windForces;
            Vec2d wind = Vec2d::Zero;

            PHYSWORLD->getWindForces(endP, Vec2d::Zero, m_actor->getDepth(), windForces);

            for ( u32 windIndex = 0; windIndex < windForces.size(); windIndex++ )
            {
                wind += windForces[windIndex].m_force;
            }

            if ( wind != Vec2d::Zero )
            {
                Vec2d startP = boneDyn.m_Pos;
                Vec2d dir = endP - startP;

                dir.normalize();
                f32 edgeAngle = getVec2Angle(dir);
                Vec2d windDirLocal = wind.Rotate(-edgeAngle);
                f32 windStrength = windDirLocal.m_y * hingeTemplate.getWindMultiplier();
                f32 resistanceScale = hingeTemplate.getResistance() / 20.f;

                resistanceScale = Clamp(resistanceScale,0.f,1.f);

                m_currentForces[i] += windStrength * (1.f - resistanceScale);
            }
        }
    }
}

// Propagate the forces applied on each hinge to the parents and apply them
void HingePlatformComponent::applyForces( f32 _dt )
{
    for ( u32 i = 0; i < m_orderedBones.size(); i++ )
    {
        u32 hingeIndex = m_orderedBones[i];
        f32 force = m_currentForces[hingeIndex];

        if ( !force )
        {
            continue;
        }

        HingeBoneData& hinge = m_hingeBones[hingeIndex];

        hinge.m_currentSpeed += force*_dt;

        u32 parent = hinge.m_parentHingeIndex;

        while ( parent != U32_INVALID )
        {
            m_hingeBones[parent].m_currentSpeed += force*_dt;

            parent = m_hingeBones[parent].m_parentHingeIndex;
        }
    }
}

// Reset the system
void HingePlatformComponent::clear()
{
    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        HingeBoneData& hingeData = m_hingeBones[i];

        if ( hingeData.m_phantom )
        {
            if ( m_hingeRegistered )
            {
                PHYSWORLD->removePhantom(hingeData.m_phantom);
            }

            PHYSWORLD->deallocPhantom(hingeData.m_phantom);
        }

        if ( hingeData.m_polyShape )
        {
            delete hingeData.m_polyShape;
        }

        if ( hingeData.m_polyline )
        {
            delete hingeData.m_polyline;
        }
    }

    for ( u32 i = 0; i < m_platforms.size(); i++ )
    {
        PlatformData& platformData = m_platforms[i];
        if( platformData.m_phantom != NULL )
        {
            if ( m_hingeRegistered )
            {
                PHYSWORLD->removePhantom(platformData.m_phantom);
            }
        }

        PHYSWORLD->deallocPhantom(platformData.m_phantom);
        SF_DEL( platformData.m_polyShape );
        SF_DEL( platformData.m_polyline );
    }

    m_currentWeights.clear();
    m_platforms.clear();
    m_leafs.clear();
    m_hingeBones.clear();
    m_orderedBones.clear();
}

void HingePlatformComponent::validate( bbool &_isComponentValidated )
{
    Super::validate(_isComponentValidated);

    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        HingeBoneData& hingeData = m_hingeBones[i];
        const HingePlatformComponent_Template::HingeBoneData& hingeDataTemplate = getTemplate()->getHingeBoneList()[i];

        VALIDATE_COMPONENT_PARAM("HingeBoneData::boneName",hingeData.m_index != U32_INVALID,
            "HingeBoneData: %s - %s - Invalid bone name %s",
            m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), hingeDataTemplate.getName().getDebugString());
    }

    for ( u32 i = 0; i < m_platforms.size(); i++ )
    {
        PlatformData& platformData = m_platforms[i];
        const HingePlatformComponent_Template::PlatformData& platformDataTemplate = getTemplate()->getPlatformList()[i];

        if ( platformDataTemplate.getLinkBone().isValid() )
        {
            VALIDATE_COMPONENT_PARAM("PlatformData::link",platformData.m_linkBoneIndex != U32_INVALID,
                "PlatformData: %s - %s - Invalid bone name %s",
                m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), platformDataTemplate.getLinkBone().getDebugString());
        }

        if ( platformDataTemplate.getScaleBone().isValid() )
        {
            VALIDATE_COMPONENT_PARAM("PlatformData::scale",platformData.m_scaleBoneIndex != U32_INVALID,
                "PlatformData: %s - %s - Invalid bone name %s",
                m_actor->getUserFriendly().cStr(), StringID(GetObjectClassCRC()).getDebugString(), platformDataTemplate.getScaleBone().getDebugString());
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
void HingePlatformComponent::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags ) const
{
    Super::drawEdit(drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        const HingeBoneData& hingeData = m_hingeBones[i];

        if ( hingeData.m_phantom )
        {
            drawInterface->drawPolyLine(hingeData.m_polyline,btrue);
        }
    }
}

void HingePlatformComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( !animMeshScene )
    {
        return;
    }

    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();
    for ( u32 i = 0; i < m_hingeBones.size(); i++ )
    {
        HingeBoneData& hingeData = m_hingeBones[i];

        if ( hingeData.m_phantom && hingeData.m_index != U32_INVALID )
        {
            AnimBoneDyn& boneDyn = boneDynList[hingeData.m_index];
            Vec2d posA = boneDyn.m_Pos;
            Vec2d posB = boneDyn.m_PosEnd;

            hingeData.m_polyline->setPosAt(posA,0);
            hingeData.m_polyline->setPosAt(posB,1);
            hingeData.m_polyline->forceRecomputeData();
            hingeData.m_phantom->setRecomputeAABB();
            hingeData.m_phantom->updateAABB();
        }
    }
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(HingePlatformComponent_Template)
BEGIN_SERIALIZATION_CHILD(HingePlatformComponent_Template)

    SERIALIZE_CONTAINER_OBJECT("hingeBones",m_hingeBones);
    SERIALIZE_CONTAINER_OBJECT("platforms",m_platforms);
    SERIALIZE_CONTAINER_OBJECT("movingPolylines",m_movingPolylines);

END_SERIALIZATION()

const HingePlatformComponent_Template::PlatformData* HingePlatformComponent_Template::findPlatformDataFromPolylineRef( const StringID& _ref, u32& _platformIndex ) const
{
    _platformIndex = U32_INVALID;

    for ( u32 i = 0; i < getPlatformList().size(); i++ )
    {
        if ( getPlatformList()[i].getPolylineAnimRef() == _ref )
        {
            _platformIndex = i;
            return &m_platforms[i];
        }
    }

    return NULL;
}

BEGIN_SERIALIZATION_SUBCLASS(HingePlatformComponent_Template,HingeBoneData)

    SERIALIZE_MEMBER("name",m_name);
    SERIALIZE_MEMBER("polyline",m_polylineAnimRef);
    SERIALIZE_MEMBER("minAngle",m_minAngle);
    SERIALIZE_MEMBER("maxAngle",m_maxAngle);
    SERIALIZE_MEMBER("weight",m_weight);
    SERIALIZE_MEMBER("resistance",m_resistance);
    SERIALIZE_MEMBER("hitForce",m_hitForce);
    SERIALIZE_MEMBER("windMultiplier",m_windMultiplier);
    SERIALIZE_MEMBER("weightMultiplier",m_weightMultiplier);
    SERIALIZE_MEMBER("branchStiff",m_branchStiff);
    SERIALIZE_MEMBER("branchDamping",m_branchDamping);
    SERIALIZE_MEMBER("branchDelayMultiplier",m_delayBranchMultiplier);
    SERIALIZE_MEMBER("disableScale",m_disableScale);
    SERIALIZE_MEMBER("disableCollision",m_disableCollision);
    SERIALIZE_MEMBER("useDynamicBranchStiff", m_useDynamicBranchStiff);
    SERIALIZE_MEMBER("dynamicBranchStiffMinAngle", m_dynamicBranchStiffMinAngle);
    SERIALIZE_MEMBER("dynamicBranchStiffMaxAngle", m_dynamicBranchStiffMaxAngle);
    SERIALIZE_MEMBER("dynamicBranchStiffMultiplier", m_dynamicBranchStiffMultiplier);
    SERIALIZE_MEMBER("dynamicBranchStiffOnlyWayBack", m_dynamicBranchStiffOnlyWayBack);
    SERIALIZE_MEMBER("alwaysApplyAngleLimitation", m_alwaysApplyAngleLimitation);

END_SERIALIZATION()

HingePlatformComponent_Template::HingeBoneData::HingeBoneData()
: m_minAngle(bfalse,0.f)
, m_maxAngle(bfalse,0.f)
, m_weight(0.f)
, m_resistance(0.f)
, m_hitForce(5.f)
, m_windMultiplier(1.f)
, m_weightMultiplier(1.f)
, m_branchStiff(5.f)
, m_branchDamping(0.2f)
, m_delayBranchMultiplier(20.f)
, m_disableScale(bfalse)
, m_disableCollision(bfalse)
, m_alwaysApplyAngleLimitation(bfalse)
, m_useDynamicBranchStiff(bfalse)
, m_dynamicBranchStiffMinAngle(bfalse, 0.f)
, m_dynamicBranchStiffMaxAngle(bfalse, 0.f)
, m_dynamicBranchStiffMultiplier(1.0f)
, m_dynamicBranchStiffOnlyWayBack(bfalse)
{
}

BEGIN_SERIALIZATION_SUBCLASS(HingePlatformComponent_Template,PlatformData)

    SERIALIZE_MEMBER("poly",m_polylineAnimRef);
    SERIALIZE_MEMBER("scale",m_scaleBone);
    SERIALIZE_MEMBER("link",m_linkBone);
    SERIALIZE_MEMBER("maxPitch",m_maxPitch);
    SERIALIZE_MEMBER("minWeight",m_scaleMinWeight);
    SERIALIZE_MEMBER("minWeightScale",m_scaleMinWeightScale);
    SERIALIZE_MEMBER("maxWeight",m_scaleMaxWeight);
    SERIALIZE_MEMBER("maxWeightScale",m_scaleMaxWeightScale);
    SERIALIZE_MEMBER("MinScale",m_minScale);
    SERIALIZE_MEMBER("scaleStiff",m_scalingStiff);
    SERIALIZE_MEMBER("scaleDamping",m_scalingDamping);
    SERIALIZE_MEMBER("minWeightHinge", m_hingeMinWeight);
    SERIALIZE_MEMBER("disableCollision",m_disableCollision);
    SERIALIZE_MEMBER("hitForceMultiplier",m_hitForceMultiplier);
    SERIALIZE_MEMBER("onlyCrushAttack", m_onlyCrushAttack);

END_SERIALIZATION()

HingePlatformComponent_Template::PlatformData::PlatformData()
: m_maxPitch(bfalse,0)
, m_scaleMinWeightScale(0.5f)
, m_scaleMinWeight(1.f)
, m_scaleMaxWeightScale(0.5f)
, m_scaleMaxWeight(10.f)
, m_minScale(0.1f)
, m_scalingStiff(5.f)
, m_scalingDamping(0.2f)
, m_hingeMinWeight(0.0f)
, m_disableCollision(btrue)
, m_hitForceMultiplier(1.0f)
, m_onlyCrushAttack(bfalse)
{
}

BEGIN_SERIALIZATION_SUBCLASS(HingePlatformComponent_Template,MovingPolylineData)

    SERIALIZE_MEMBER("polyline",m_polyRef);
    SERIALIZE_MEMBER("resistance",m_resistance);
    SERIALIZE_MEMBER("forceMultiplier",m_forceMultiplier);

END_SERIALIZATION()

HingePlatformComponent_Template::MovingPolylineData::MovingPolylineData()
: m_resistance(0.f)
, m_forceMultiplier(100.f)
{
}

}
