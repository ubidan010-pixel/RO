
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTDECIDERFINDTARGETATTACKPOS_H_
#include "rayman/gameplay/ai/BTs/Ray_BTDeciderFindTargetAttackPos.h"
#endif // _ITF_RAY_BTDECIDERFINDTARGETATTACKPOS_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTDeciderFindTargetAttackPos);

Ray_BTDeciderFindTargetAttackPos::Ray_BTDeciderFindTargetAttackPos()
: m_physComponent(NULL)
{
}

void Ray_BTDeciderFindTargetAttackPos::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_physComponent = _tree->getActor()->GetComponent<PhysComponent>();
}

bbool Ray_BTDeciderFindTargetAttackPos::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_physComponent )
    {
        ITF_WARNING_PARAM(_actor,"stickToPolylinePhysComponent",m_physComponent!=NULL,"%s: stickToPolylinePhysComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getFactTarget().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"factTarget",getTemplate()->getFactTarget().isValid(),"%s: missing fact to read the target",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getFactPos().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"factPos",getTemplate()->getFactPos().isValid(),"%s: missing fact to store the position",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTDeciderFindTargetAttackPos::decide( f32 _dt )
{
    ObjectRef target = ITF_INVALID_OBJREF;

    if ( !m_tree->getBlackBoard().getFact(getTemplate()->getFactTarget(),target) )
    {
        return bfalse;
    }

    Actor* targetActor = AIUtils::getActor(target);

    if ( !targetActor )
    {
        return bfalse;
    }

    Vec2d dirToMe = m_tree->getActor()->get2DPos() - targetActor->get2DPos();
    f32 crossP = m_physComponent->getCurrentGravityDir().dot(dirToMe);
    bbool imLeft = crossP > 0.f;
    EventQueryPhysicsData physData;
    EventQueryPhysShape physShape;
    f32 leftSide, rightSide;
    Vec2d targetPoint = targetActor->get2DPos();

    targetActor->onEvent(&physShape);

    if ( physShape.getPhysShape() )
    {
        getShapeInfo(physShape.getPhysShape(),physShape.getPosition(),physShape.getAngle(),leftSide,rightSide);

        if ( imLeft )
        {
            targetPoint.m_x = leftSide;
        }
        else
        {
            targetPoint.m_x = rightSide;
        }
    }

    targetActor->onEvent(&physData);

    if ( physData.getStickyEdgePolyline() != U32_INVALID )
    {
        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(physData.getStickyPolyline(),physData.getStickyEdgePolyline(),poly,edge);

        if ( poly && edge )
        {
            Vec2d dirToPoint = targetPoint - edge->getPos();
            f32 dist = edge->m_normalizedVector.dot(dirToPoint);
            Vec2d posOnEdge = edge->getPos() + ( edge->m_normalizedVector * dist );
            targetPoint = posOnEdge + ( edge->m_normalizedVector.getPerpendicular() * m_physComponent->getRadius() );
        }
    }

    m_tree->getBlackBoard().setFact(getTemplate()->getFactPos(),targetPoint);

    return Super::decide(_dt);
}

void Ray_BTDeciderFindTargetAttackPos::getShapeInfo( const PhysShape* _shape,
                                                     const Vec2d& _pos, f32 _angle,
                                                     f32& _leftSide, f32& _rightSide ) const
{
    AABB bBox;

    _shape->calculateAABB(_pos,_pos,_angle,bBox);

    _leftSide = bBox.getMin().m_x;
    _rightSide = bBox.getMax().m_x;
}

IMPLEMENT_OBJECT_RTTI(Ray_BTDeciderFindTargetAttackPos_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTDeciderFindTargetAttackPos_Template)

    SERIALIZE_MEMBER("factTarget",m_factTarget);
    SERIALIZE_MEMBER("factPos",m_factPos);

END_SERIALIZATION()

Ray_BTDeciderFindTargetAttackPos_Template::Ray_BTDeciderFindTargetAttackPos_Template()
{
}

}