#include "precompiled_gameplay.h"

#ifndef _ITF_CURVECOMPONENT_H_
#include "gameplay/Components/Misc/CurveComponent.h"
#endif //_ITF_CURVECOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#define TRAJECTORY_SAMPLE_COUNT     100
#define TRAJECTORY_SAMPLE_T         (1.0f / (f32)TRAJECTORY_SAMPLE_COUNT)

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(CurveComponent)

    BEGIN_SERIALIZATION_CHILD(CurveComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_CONTAINER_OBJECT("points", m_nodeArray);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(CurveComponent, TrajectoryNode)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("pos", m_pos);
            SERIALIZE_MEMBER("tangA", m_tangA);
            SERIALIZE_MEMBER("tangB", m_tangB);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(CurveComponent)
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    CurveComponent::CurveComponent()
    : m_distCurveTotal(0.0f)
    {
        // none
    }

    //*****************************************************************************

    CurveComponent::~CurveComponent()
    {
        //none
    }

    //*****************************************************************************

    void CurveComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Pre-Computing of sampling
        init(); 
    }

    //*****************************************************************************

    void CurveComponent:: onBecomeActive()
    {
        // none
    }

    //*****************************************************************************

    void CurveComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
    }

    //*****************************************************************************

    void CurveComponent::init()
    {
        Vec3d prevPos, curPos;
        Vec3d p0, p1, p2, p3;

        if (m_nodeArray.size() == 0)
            return;

        m_distCurveTotal = 0.0f;

        for (u32 i = 0; i < m_nodeArray.size() - 1; i++)
        {

            TrajectoryNode& node        = m_nodeArray[i];
            TrajectoryNode& nextNode    = m_nodeArray[i+1];

            node.trajectorySamples.clear();
            node.m_lengthSeg = 0.0f;

            p0 = actorTransform(node.m_pos);
            p1 = actorTransform(node.m_tangB);
            p2 = actorTransform(nextNode.m_tangA);
            p3 = actorTransform(nextNode.m_pos);

            prevPos   = p0;

            for (i32 iSample = 0; iSample < TRAJECTORY_SAMPLE_COUNT; iSample++)
            {
                f32 t = TRAJECTORY_SAMPLE_T * (iSample + 1);    
                f32 tInv = 1.0f - t;

                // control 4 points
                curPos =    ( p0 * tInv * tInv * tInv ) 
                    + ( p1 * 3.f * t * tInv * tInv ) 
                    + ( p2 * 3.f * t * t * tInv ) 
                    + ( p3 * t * t * t );


                f32 lengthSample = (prevPos - curPos).norm();
                node.m_lengthSeg += lengthSample; 

                node.trajectorySamples.push_back(node.m_lengthSeg);

                prevPos = curPos;

            }

            m_distCurveTotal += node.m_lengthSeg;
        }
    }

    //*****************************************************************************

    Vec3d CurveComponent::getPosAtDist( f32 _queryDist)
    { 
        Vec3d resPos, resTang;
        u32 resIndex = U32_INVALID;
        f32 t = 0.0f;

        getPosAndIndexSegAtDist(_queryDist, resPos, resTang, resIndex, t);

        return resPos;

    }

    //*****************************************************************************

    u32 CurveComponent::getIndexSegmentAtDist( f32 _queryDist )
    { 
        Vec3d resPos, resTang;
        u32 resIndex = U32_INVALID;
        f32 t = 0.0f;
    
        getPosAndIndexSegAtDist(_queryDist, resPos, resTang, resIndex, t);

        return resIndex;
    }

    //*****************************************************************************

    void CurveComponent::getPosAndIndexSegAtDist( f32 _queryDist, Vec3d& _posOut, Vec3d& _tangOut, u32& _indexSegOut, f32& _tOut)
    {   
        if (m_nodeArray.size() == 0)
            return;

        if (_queryDist >= m_distCurveTotal)
        {
            _queryDist = m_distCurveTotal - TRAJECTORY_SAMPLE_T;
        }

        for(u32 i = 0; i < m_nodeArray.size() - 1; i++)
        {
            TrajectoryNode& node = m_nodeArray[i];
            f32 length = node.m_lengthSeg;

            if (_queryDist <= length)
            {
                // node reached

                for (u32 iSample = 0; iSample < node.trajectorySamples.size(); iSample++)
                {
                    f32 sampleDist = node.trajectorySamples[iSample];
                    if (sampleDist >= _queryDist)
                    {
                        // sample+1 reached
                        f32 prevSampleDist  = 0.f;
                        f32 prevSampleT     = 0.f;

                        if (iSample > 0)
                        {
                            prevSampleDist = node.trajectorySamples[iSample-1];
                            prevSampleT = TRAJECTORY_SAMPLE_T * iSample;
                        }

                        // interpolate t
                        f32 lengthSample = sampleDist - prevSampleDist;
                        f32 t = prevSampleT + ((_queryDist - prevSampleDist) / lengthSample) * TRAJECTORY_SAMPLE_T;

                        _posOut         = getPos(t, i);
                        _tangOut        = getTangent(t, i);
                        _indexSegOut    = i;
                        _tOut           = t;

                        return;
                    }
                }
            }
            else
            {
                _queryDist -= length;

            }
        }
    } 

    //*****************************************************************************

    // EDITOR ONLY !!!
    void CurveComponent::getClosestPos( const Vec3d& _refPos, Vec3d& _closetPosOut, Vec3d& _tangOut, f32& _distOnCurveOut, u32& _indexSegOut, f32& _tOut )
    { 
        f32 minSqrDist = F32_INFINITY;
        f32 curSqrDist = 0.0f;
        Vec3d closestPos;
        Vec3d tang;

        if (m_nodeArray.size() == 0)
            return ;

        Vec3d prevPos = m_actor->getPos();
        f32 distOnCurve = 0.0f;
        f32 distBuffer  = 0.0f;
        f32 tTemp = 0.0f;

        for(u32 i = 0; i < m_nodeArray.size() - 1; i++)
        {
 
            for (i32 iSample = 0; iSample < TRAJECTORY_SAMPLE_COUNT; iSample++)
            {
                f32 t = TRAJECTORY_SAMPLE_T * (iSample + 1);    

                Vec3d curPos = getPos(t, i);
                curSqrDist = (curPos - _refPos).sqrnorm();
                
                // get distance on curve
                distBuffer += (curPos - prevPos).norm();
                prevPos = curPos;

                if (curSqrDist < minSqrDist)
                {
                    minSqrDist = curSqrDist;
                    closestPos = curPos;
                    tang = getTangent(t, i);
                    tTemp = t;
                    _indexSegOut = i;

                    distOnCurve += distBuffer;
                    distBuffer = 0.0f;
                }
            }
        }

        // Result 
        _closetPosOut   = closestPos;
        _tangOut        = tang;
        _distOnCurveOut = distOnCurve;
        _tOut           = tTemp;

    }

    //*****************************************************************************
    
    Vec3d CurveComponent::actorTransform(const Vec3d& _pos, bbool _translate /*= btrue*/)
    {
        if (!m_actor)
        {
            return _pos;
        }
        Vec2d pos2d = _pos.truncateTo2D();
        pos2d.m_x = m_actor->isFlipped() ? -pos2d.m_x : pos2d.m_x;
        pos2d = pos2d.Rotate(m_actor->getBoundWorldInitialAngle());
        Vec3d transformed = pos2d.to3d(_pos.m_z);
        if (_translate)
        {
            transformed += m_actor->getBoundWorldInitialPos();
        }
        return transformed;
    }

    //*****************************************************************************

    Vec3d CurveComponent::actorUntransform(const Vec3d& _pos, bbool _translate /*= btrue*/)
    {
        if (!m_actor)
        {
            return _pos;
        }
        Vec3d translated = _pos;
        if (_translate)
        {
            translated -= m_actor->getBoundWorldInitialPos();
        }
        Vec2d pos2d = translated.truncateTo2D();
        pos2d = pos2d.Rotate(-m_actor->getBoundWorldInitialAngle());
        pos2d.m_x = m_actor->isFlipped() ? -pos2d.m_x : pos2d.m_x;
        Vec3d transformed = pos2d.to3d(translated.m_z);
        return transformed;
    }

    //*****************************************************************************

    Vec3d CurveComponent::getPos( f32 _t, u32 _nodeIndex )
    {
        ITF_ASSERT_CRASH(m_nodeArray.size() >= 2, "not enough nodes");
        ITF_ASSERT_CRASH(_nodeIndex <= m_nodeArray.size() - 2, "requested nodeIndex too big");

        const Vec3d p0 = m_nodeArray[_nodeIndex].m_pos;
        const Vec3d p1 = m_nodeArray[_nodeIndex].m_tangB;
        const Vec3d p2 = m_nodeArray[_nodeIndex + 1].m_tangA;
        const Vec3d p3 = m_nodeArray[_nodeIndex + 1].m_pos;

        return actorTransform(getBezierPosition(p0, p1, p2, p3, _t));
    }

    //*****************************************************************************

    Vec3d CurveComponent::getTangent( f32 _t, u32 _nodeIndex )
    {
        ITF_ASSERT_CRASH(m_nodeArray.size() >= 2, "not enough nodes");
        ITF_ASSERT_CRASH(_nodeIndex <= m_nodeArray.size() - 2, "requested nodeIndex too big");

        const Vec3d p0 = m_nodeArray[_nodeIndex].m_pos;
        const Vec3d p1 = m_nodeArray[_nodeIndex].m_tangB;
        const Vec3d p2 = m_nodeArray[_nodeIndex + 1].m_tangA;
        const Vec3d p3 = m_nodeArray[_nodeIndex + 1].m_pos;

        return actorTransform(getBezierTangent(p0, p1, p2, p3, _t), bfalse);
    }
      
#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void CurveComponent::onEditorCreated( Actor* _original )
    {
        Super::onEditorCreated(_original);
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(CurveComponent_Template)
    BEGIN_SERIALIZATION(CurveComponent_Template)
    END_SERIALIZATION()

    //*****************************************************************************

    CurveComponent_Template::CurveComponent_Template()
    {
        // none
    }

    //*****************************************************************************

    CurveComponent_Template::~CurveComponent_Template()
    {
        // none
    }

    //*****************************************************************************


};
