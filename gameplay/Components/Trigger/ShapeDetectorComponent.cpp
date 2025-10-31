#include "precompiled_gameplay.h"

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ShapeDetectorComponent)

    BEGIN_SERIALIZATION_CHILD(ShapeDetectorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(ShapeDetectorComponent)
    END_VALIDATE_COMPONENT()


    ShapeDetectorComponent::ShapeDetectorComponent()
        : m_animComponent(NULL)
        , m_factoryShapeScaled(NULL)
        , m_currentShape(NULL)
        , m_refPos(Vec2d::Zero)
        , m_shapePos(Vec2d::Zero)
        , m_scaleMultiplier(1.f)
    {
    }

    ShapeDetectorComponent::~ShapeDetectorComponent()
    {
        clear();
    }

    void ShapeDetectorComponent::clear()
    {
        SF_DEL(m_factoryShapeScaled);
        m_currentShape = NULL;
        m_animShapePoints.clear();
    }

    void ShapeDetectorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();

        m_animShapePolyline.setPolyline(&m_polyline);

        if (getTemplate()->getShape())
        {
            scaleFactoryShape();
            updateFactoryShape(btrue);
        }
    }

    void ShapeDetectorComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        m_shapePos = m_actor->get2DPos();
    }


    void ShapeDetectorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        updateShapeAndAABB();
    }

    void ShapeDetectorComponent::updateAnimPolyline(bbool _init)
    {
        AnimPolyline* pPolyline = NULL;
        u32 numPoints = 0;
        const Vec2d* points = NULL;
        transform2d xf;

        xf.m_pos = m_actor->get2DPos();
        xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
        xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

        if ( m_animComponent && !_init)
        {
            u32 pointIndex;
            const Vec2d* pointsBuffer = m_animComponent->getCurrentFirstPolylinePoint(getTemplate()->getAnimRefPosPointId(), &pPolyline, &pointIndex);
            if (pointsBuffer)
            {
                m_refPos = mulTransform2dT(xf,pointsBuffer[pointIndex]);
            }

            pointsBuffer = m_animComponent->getCurrentFirstPolylinePoint(getTemplate()->getAnimShapePosPointId(), &pPolyline, &pointIndex);
            if (pointsBuffer)
            {
                m_shapePos = pointsBuffer[pointIndex];
            }

            points = m_animComponent->getCurrentPolylinePointBuffer(getTemplate()->getAnimPolylineId(), &numPoints);
        }

        if ( numPoints )
        {
            if ( !getTemplate()->getShapeIsConcave() )
            {
                m_animShapePoints.resize(numPoints);

                if ( !m_actor->isFlipped() )
                {
                    for ( u32 i = 0; i < numPoints; i++ )
                    {
                        m_animShapePoints[i] = mulTransform2dT(xf,points[i]);
                    }
                }
                else
                {
                    for ( i32 i = static_cast<i32>(numPoints)-1; i >= 0; i-- )
                    {
                        m_animShapePoints[numPoints-i-1] = mulTransform2dT(xf,points[i]);
                    }
                }

                m_animShape.setPoints(m_animShapePoints);
                m_currentShape = &m_animShape;
            }
            else
            {
                m_polyline.resize(numPoints);

                if ( !m_actor->isFlipped() )
                {
                    for ( u32 i = 0; i < numPoints; i++ )
                    {
                        m_polyline.setPosAt(points[i], i);
                    }
                }
                else
                {
                    for ( i32 i = static_cast<i32>(numPoints)-1; i >= 0; i-- )
                    {
                        m_polyline.setPosAt(points[i], numPoints-i-1);
                    }
                }

                m_polyline.update(0);
                m_currentShape = &m_animShapePolyline;
            }
        }
        else
        {
            m_animShapePoints.clear();
            m_polyline.clear();
            updateFactoryShape(_init);
        }
    }

    Vec2d ShapeDetectorComponent::getRefPos( const Vec2d& _pos ) const
    {
        Vec2d localPos = ( _pos - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());
        return localPos - m_refPos;
    }

    void ShapeDetectorComponent::scaleFactoryShape()
    {
        if ( !m_factoryShapeScaled ||
              m_factoryShapeScaled->GetObjectClassCRC() != getTemplate()->getShape()->GetObjectClassCRC() )
        {
            SF_DEL(m_factoryShapeScaled);
            m_factoryShapeScaled = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getShape()->GetObjectClassCRC());
        }

        // make sure the existing scaled shape is not marked as flipped before copying the points
        if ( PhysShapePolygon* polygon = m_factoryShapeScaled->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            polygon->setFlipped(bfalse);
        }

        PhysShape::CopyShapeScaled(getTemplate()->getShape(), m_factoryShapeScaled, m_actor->getScale()*m_scaleMultiplier);
    }

    void ShapeDetectorComponent::updateFactoryShape(bbool _init)
    {
        if ( !m_factoryShapeScaled )
        {
            return;
        }

        bbool flipped = m_actor->isFlipped();

        // update shape pos

        if (m_animComponent && getTemplate()->getAttachPolyline().isValid())
        {
            m_shapePos = m_actor->get2DPos();
            if (!_init)
            {
                u32 pointCount = 0;
                const Vec2d* points = m_animComponent->getCurrentPolylinePointBuffer(getTemplate()->getAttachPolyline(), &pointCount);
                if (points && pointCount)
                {
                    m_shapePos = points[0];
                }
                else
                {
                    return;
                }
            }
        }

        if ( getTemplate()->getOffset() != Vec2d::Zero )
        {
            Vec2d offset = getTemplate()->getOffset() * m_actor->getScale()*m_scaleMultiplier;
            Vec2d xDir = Vec2d::Right.Rotate(m_actor->getAngle());
            Vec2d yDir = xDir.getPerpendicular();

            if ( flipped )
            {
                xDir *= -1.f;
            }

            Vec2d::MulXAdd(&m_shapePos, &xDir, &offset, &m_shapePos);
            Vec2d::MulYAdd(&m_shapePos, &yDir, &offset, &m_shapePos);
        }


        // flip shape if necessary
        if ( PhysShapePolygon* polygon = m_factoryShapeScaled->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            polygon->setFlipped(flipped);
        }

        m_currentShape = m_factoryShapeScaled;
    }

     void ShapeDetectorComponent::updateShapeAndAABB(bbool _init)
     {
         m_currentShape = NULL;
         m_shapePos = m_actor->get2DPos();

         if ( getTemplate()->getAnimPolylineId().isValid() || getTemplate()->getAnimRefPosPointId().isValid() || getTemplate()->getAnimShapePosPointId().isValid() )
         {
             updateAnimPolyline(_init);
         }
         else
         {
             updateFactoryShape(_init);
         }

         if ( !m_currentShape )
         {
             m_actorsInside.clear();
         }
         else
         {
             // grow AABB
             AABB aabb(m_actor->getPos());
             AABB shapeAABB;
             m_currentShape->calculateAABB(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),shapeAABB);
             aabb.grow(shapeAABB);
             m_actor->growRelativeAABBFromAbsolute(aabb);
         }
     }

     void ShapeDetectorComponent::onSceneActive()
     {
         Super::onSceneActive();

         updateShapeAndAABB(btrue);
     }


#ifdef ITF_SUPPORT_EDITOR
     void ShapeDetectorComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
     {
         Super::drawEdit(_drawInterface, _flags);

         if (_flags != ActorComponent::DrawEditFlag_All)
         {
             return;
         }

         if (m_disabled)
         {
             return;
         }

         if ( m_currentShape )
         {
             DebugDraw::shape(m_shapePos, m_actor->getPos().m_z, m_actor->getAngle(), m_currentShape,
                 Color::white(), 0.f, "trigger", 85 * MTH_DEGTORAD);
         }
     }

     void ShapeDetectorComponent::onEditorMove(bbool _modifyInitialPos)
     {
         if (getTemplate()->getShape())
         {
             scaleFactoryShape();
         }

         m_shapePos = m_actor->get2DPos();

         if ( getTemplate()->getAnimPolylineId().isValid() || getTemplate()->getAnimRefPosPointId().isValid() || getTemplate()->getAnimShapePosPointId().isValid() )
         {
             updateAnimPolyline();
         }
         else
         {
             updateFactoryShape();
         }
     }
#endif // ITF_SUPPORT_EDITOR

     //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(ShapeDetectorComponent_Template)
    BEGIN_SERIALIZATION_CHILD(ShapeDetectorComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("shape", m_factoryShape, PHYSWORLD->getShapesFactory());
        SERIALIZE_MEMBER("offset", m_offset);
        SERIALIZE_MEMBER("attachPolyline", m_attachPolyline);
        SERIALIZE_MEMBER("animPolylineID", m_animPolylineId);
        SERIALIZE_MEMBER("animRefPosPointID", m_animRefPosPointId);
        SERIALIZE_MEMBER("animShapePosPointID", m_animShapePosPointId);
        SERIALIZE_MEMBER("shapeIsConcave", m_shapeIsConcave);
    END_SERIALIZATION()

    ShapeDetectorComponent_Template::ShapeDetectorComponent_Template()
        : m_factoryShape(NULL)
        , m_offset(Vec2d::Zero)
        , m_shapeIsConcave(bfalse)
    {
    }

    ShapeDetectorComponent_Template::~ShapeDetectorComponent_Template()
    {
        SF_DEL(m_factoryShape);
    }
};