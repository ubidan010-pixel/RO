#include "precompiled_gameplay.h"

#ifndef _ITF_BEZIERCURVECOMPONENT_H_
#include "gameplay/Components/Misc/BezierCurveComponent.h"
#endif //_ITF_BEZIERCURVECOMPONENT_H_

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

#define TRAJECTORY_SAMPLE_COUNT     100//100
#define TRAJECTORY_SAMPLE_T         (1.0f / (f32)TRAJECTORY_SAMPLE_COUNT)

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(BezierCurveComponent)

    BEGIN_SERIALIZATION_CHILD(BezierCurveComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_OBJECT("bezier",m_bezierCurve );
        END_CONDITION_BLOCK()
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(BezierCurveComponent)
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    BezierCurveComponent::BezierCurveComponent()
    : m_distCurveTotal(0.0f)
    {
        // none
    }

    //*****************************************************************************

    BezierCurveComponent::~BezierCurveComponent()
    {
        //none
    }

    //*****************************************************************************

    void BezierCurveComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Pre-Computing of sampling

        init(); 
    }

    //*****************************************************************************

    void BezierCurveComponent:: onBecomeActive()
    {
        // none
    }

    //*****************************************************************************

    void BezierCurveComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        AABB aabb(m_actor->getPos());

        for (u32 i = 0; i < m_bezierCurve.getNumPoints(); i++)
        {
            const BezierCurve::Point & point = m_bezierCurve.getPointAt(i);
            aabb.grow(actorTransform(point.m_pos)); 
            aabb.grow(actorTransform(point.m_tanA)); 
            aabb.grow(actorTransform(point.m_tanB)); 
        } 

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    //*****************************************************************************

    void BezierCurveComponent::init()
    {
        m_bezierCurve.buildEdges(TRAJECTORY_SAMPLE_COUNT);
        m_distCurveTotal = m_bezierCurve.getTotalLength();
    }

    //*****************************************************************************

    //Vec3d BezierCurveComponent::getPosAtDist( f32 _queryDist)
    //{ 
    //    Vec3d resPos, resTang;
    //    u32 resIndex = U32_INVALID;
    //    f32 t = 0.0f;

    //    getPosAndIndexSegAtDist(_queryDist, resPos, resTang, resIndex, t);

    //    return resPos;

    //}

    ////*****************************************************************************

    //u32 BezierCurveComponent::getIndexSegmentAtDist( f32 _queryDist )
    //{ 
    //    Vec3d resPos, resTang;
    //    u32 resIndex = U32_INVALID;
    //    f32 t = 0.0f;
    //
    //    getPosAndIndexSegAtDist(_queryDist, resPos, resTang, resIndex, t);

    //    return resIndex;
    //}

    ////*****************************************************************************

    //void BezierCurveComponent::getPosAndIndexSegAtDist( f32 _queryDist, Vec3d& _posOut, Vec3d& _tangOut, u32& _indexSegOut, f32& _tOut)
    //{   
    //    if (m_bezierCurve.getNumPoints() == 0)
    //        return;

    //    if (_queryDist >= m_distCurveTotal)
    //    {
    //        _queryDist = m_distCurveTotal - TRAJECTORY_SAMPLE_T;
    //    }

    //    u32 tempIndex = m_bezierCurve.getEdgeIndexAtDistance(_queryDist);
    //    f32 tempT = m_bezierCurve.getTAtDistance(_queryDist,tempIndex);
    //    Vec3d tempPosOut = actorTransform(m_bezierCurve.getPosAtT( tempT,  tempIndex));
    //    Vec3d tempTanOut = actorTransform(m_bezierCurve.getTanAtT( tempT,  tempIndex), bfalse);

    //    _indexSegOut = tempIndex;
    //    _tOut = tempT;
    //    _posOut = tempPosOut;
    //    _tangOut = tempTanOut;

    //} 

 

    ////*****************************************************************************
    
    Vec3d BezierCurveComponent::actorTransform(const Vec3d& _pos, bbool _translate /*= btrue*/)
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

    ////*****************************************************************************

    Vec3d BezierCurveComponent::actorUntransform(const Vec3d& _pos, bbool _translate /*= btrue*/)
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

    ////*****************************************************************************

    bbool BezierCurveComponent::getLockFirstTangent() const
    {
        return getTemplate()->getLockFirstTangent();
    }
    
    u32 BezierCurveComponent::getDefaultNodeCount() const
    {
        return getTemplate()->getDefaultNodeCount();
    }

    ////*****************************************************************************

    //Vec3d BezierCurveComponent::getPos( f32 _t, u32 _nodeIndex )
    //{
    //    Vec3d tempPos = actorTransform(m_bezierCurve.getPosAtT( _t,  _nodeIndex));
    //    return tempPos;
    //}

    ////*****************************************************************************

    //Vec3d BezierCurveComponent::getTangent( f32 _t, u32 _nodeIndex )
    //{
    //    Vec3d tempPos = actorTransform(m_bezierCurve.getTanAtT( _t,  _nodeIndex), bfalse);
    //    return tempPos;
    //}

#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void BezierCurveComponent::onEditorCreated( Actor* _original )
    {
        Super::onEditorCreated(_original);
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(BezierCurveComponent_Template)
    BEGIN_SERIALIZATION(BezierCurveComponent_Template)
        SERIALIZE_MEMBER("lockFirstTangent", m_lockFirstTangent);
        SERIALIZE_MEMBER("defaultNodeCount", m_defaultNodeCount);
    END_SERIALIZATION()

    //*****************************************************************************

    BezierCurveComponent_Template::BezierCurveComponent_Template()
        : Super()
        , m_lockFirstTangent(bfalse)
        , m_defaultNodeCount(4)
    {
        // none
    }

    //*****************************************************************************

    BezierCurveComponent_Template::~BezierCurveComponent_Template()
    {
        // none
    }

    //*****************************************************************************


};
