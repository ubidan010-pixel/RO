#include "precompiled_gameplay.h"

#ifndef _ITF_SHAPECOMPONENT_H_
#include "gameplay/Components/Misc/ShapeComponent.h"
#endif //_ITF_SHAPECOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ShapeComponent)

    BEGIN_SERIALIZATION_CHILD(ShapeComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(ShapeComponent)
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getPolylineID().isValid() || m_animComponent, "You need an AnimLightComponent if you use an animation polyline");
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getPolylineID().isValid() || !getTemplate()->getShape(), "The polyline will be ignored if you set a shape");
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getPolylineID().isValid() || isPolylineValid(), "Invalid polyline %s", getTemplate()->getPolylineID().getDebugString());
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getUseAABBShape() || m_animComponent, "You need an AnimLightcomponent if you use AABB shape");
    END_VALIDATE_COMPONENT()

    ShapeComponent::ShapeComponent()
        : Super()
        , m_animComponent(NULL)
        , m_currentShape(NULL)
        , m_factoryShapeScaled(NULL)
    {
    }

    ShapeComponent::~ShapeComponent()
    {
        SF_DEL(m_factoryShapeScaled);
        for (ITF_MAP<StringID,ShapeData>::iterator it = m_shapes.begin(); it != m_shapes.end(); ++it)
        {
            SF_DEL(it->second.m_shape);
        }
    }

    void ShapeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();  // not mandatory, except if using polylines

        if (getTemplate()->getShape())
        {
            scaleFactoryShape();
        }


        m_shapes.clear();

        const FixedArray<ShapeData_Template,4> & shapes = getTemplate()->getShapes();
        for (u32 i = 0; i < shapes.size(); ++i)
        {
            if (shapes[i].m_name.isValid() && shapes[i].m_shape)
            {
                ShapeData data;
                data.m_index = i;
                data.m_shape = NULL;
                scaleShape(shapes[i].m_shape,&data.m_shape);
                m_shapes[shapes[i].m_name] = data;
            }
        }
    }
    void ShapeComponent::scaleShape(const PhysShape * _source, PhysShape ** _dest)
    {
        if ( !(*_dest) ||
             (*_dest)->GetObjectClassCRC() != _source->GetObjectClassCRC() )
        {
            SF_DEL(*_dest);
            *_dest = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(_source->GetObjectClassCRC());
        }

        // make sure the existing scaled shape is not marked as flipped before copying the points
        if ( PhysShapePolygon* polygon = (*_dest)->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            polygon->setFlipped(bfalse);
        }

        PhysShape::CopyShapeScaled(_source, *_dest, m_actor->getScale());
    }

    void ShapeComponent::scaleFactoryShape()
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

        PhysShape::CopyShapeScaled(getTemplate()->getShape(), m_factoryShapeScaled, m_actor->getScale());
    }

    bbool ShapeComponent::usePolylineShape() const
    {
        return !getTemplate()->getShape() && m_animComponent && getTemplate()->getPolylineID().isValid();
    }

    void ShapeComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        updateShape();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        if ( m_currentShape && getTemplate()->getDrawDebug() )
        {
            drawDebug();
        }
#endif // ITF_SUPPORT_DEBUGFEATURE
    }

    void ShapeComponent::updateShape()
    {
        m_currentShape = NULL;
        m_shapePos = m_actor->get2DPos();

        if ( usePolylineShape() )
        {
            updatePolylineShape();
        }
        else if (m_currentShapeName.isValid())
        {
            updateCurrentShape();
        }
        else if (getTemplate()->getUseAABBShape())
        {
            const AABB aabb = m_animComponent->getPatchAABB();
            PhysShapePolygon::PointsList pointsList;
            pointsList.push_back(aabb.getMin());
            pointsList.push_back(aabb.getMinXMaxY());
            pointsList.push_back(aabb.getMax());
            pointsList.push_back(aabb.getMaxXMinY());
            m_AABBShape.setPoints(pointsList);
            m_currentShape = &m_AABBShape;
        }
        else
        {
            updateFactoryShape();
        }
    }

    void ShapeComponent::updateFactoryShape()
    {
        if ( !m_factoryShapeScaled )
        {
            return;
        }

        bbool flipped = m_actor->isFlipped();


        // update shape pos

        if (m_animComponent && getTemplate()->getAttachPolyline().isValid())
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

        if ( getTemplate()->getOffset() != Vec2d::Zero )
        {
            Vec2d offset = getTemplate()->getOffset() * m_actor->getScale();
            Vec2d xDir = Vec2d::Right.Rotate(m_actor->getAngle());
            Vec2d yDir = xDir.getPerpendicular();

            if ( flipped )
            {
                xDir *= -1.f;
            }

            m_shapePos += xDir * offset.m_x;
            m_shapePos += yDir * offset.m_y;
        }


        // flip shape if necessary
        if ( PhysShapePolygon* polygon = m_factoryShapeScaled->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            polygon->setFlipped(flipped);
        }

        m_currentShape = m_factoryShapeScaled;
    }

    bbool ShapeComponent::isPolylineValid() const
    {
        if (m_animComponent && m_animComponent->isLoaded())
        {
            AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
            AnimSkeleton* skeleton = animMeshScene->m_AnimInfo.getCurrentSkeleton();

            AnimPolyline* pPolyline;
            return skeleton->getPolyline(getTemplate()->getPolylineID(), pPolyline);
        }

        return bfalse;
    }

    void ShapeComponent::updatePolylineShape()
    {
        const Vec2d* points;
        u32 pointCount;

        if ( getPolylinePoints(points, pointCount) )
        {
            updatePolylineShapePoints(points, pointCount);
            m_currentShape = &m_polylineShape;
        }
        else
        {
            m_polylineShape.clear();
        }
    }

    bbool ShapeComponent::getPolylinePoints( const Vec2d*& _points, u32& _pointCount )
    {
        ITF_ASSERT(m_animComponent->isLoaded());
        if (!m_animComponent->isLoaded())
        {
            return bfalse;
        }

        _points = m_animComponent->getCurrentPolylinePointBuffer(getTemplate()->getPolylineID(), &_pointCount);
        ITF_WARNING(m_actor, _points == NULL || _pointCount >= 2, "A polyline needs to have at least 2 _points to be used as a phantom.");
        return _points != NULL && _pointCount >= 2;
    }

    void ShapeComponent::updatePolylineShapePoints( const Vec2d* _points, u32 _pointCount )
    {
        m_shapePoints.clear();

        if ( _pointCount )
        {
            transform2d xf;

            xf.m_pos = m_actor->get2DPos();
            xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
            xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

            if ( !m_actor->isFlipped() )
            {
                for (u32 i = 0; i < _pointCount; i++)
                {
                    m_shapePoints.push_back(mulTransform2dT(xf,_points[i]));
                }
            }
            else
            {
                for (i32 i = _pointCount - 1; i >= 0; i--)
                {
                    m_shapePoints.push_back(mulTransform2dT(xf,_points[i]));
                }
            }
        }

        m_polylineShape.setPoints(m_shapePoints);
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void ShapeComponent::drawDebug() const
    {
        DebugDraw::shape(m_shapePos, m_actor->getDepth(), m_actor->getAngle(), m_currentShape,Color::white(),0.0f,"current");
        for(ITF_MAP<StringID,ShapeData>::const_iterator it = m_shapes.begin(); it != m_shapes.end(); ++it)
        {
            if (it->second.m_shape != m_currentShape)
            {
                DebugDraw::shape(m_shapePos,m_actor->getDepth(),m_actor->getAngle(),it->second.m_shape,Color::white(),0.0f,it->first.getDebugString());
            }
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    void ShapeComponent::setShape( StringID _shapeName )
    {
        if (_shapeName.isValid() && _shapeName != m_currentShapeName)
        {
            ITF_MAP<StringID,ShapeData>::iterator it = m_shapes.find(_shapeName);
            if (it != m_shapes.end())
            {
                m_currentShapeName = _shapeName;
            }
        }
    }

    void ShapeComponent::setDefaultShape()
    {
        m_currentShapeName.invalidate();
    }

    void ShapeComponent::updateCurrentShape()
    {
        if (!m_currentShapeName.isValid())
            return;


        ITF_MAP<StringID,ShapeData>::iterator it = m_shapes.find(m_currentShapeName);
        if (it == m_shapes.end())
            return;

        ShapeData & shape = it->second;

        bbool flipped = m_animComponent && m_actor->isFlipped();


        // update shape pos
    
        const ShapeData_Template & shapeT = getTemplate()->getShapes()[shape.m_index];

        StringID attachPolyline = shapeT.m_attachPolyline;
        if (attachPolyline.isValid())
        {
            u32 pointCount = 0;
            const Vec2d* points = m_animComponent?m_animComponent->getCurrentPolylinePointBuffer(attachPolyline, &pointCount):NULL;
            if (points && pointCount)
            {
                m_shapePos = points[0];
            }
            else
            {
                return;
            }
        }

        if ( shapeT.m_offset!= Vec2d::Zero )
        {
            Vec2d offset = shapeT.m_offset * m_actor->getScale();
            Vec2d xDir = Vec2d::Right.Rotate(m_actor->getAngle());
            Vec2d yDir = xDir.getPerpendicular();

            if ( flipped )
            {
                xDir *= -1.f;
            }

            m_shapePos += xDir * offset.m_x;
            m_shapePos += yDir * offset.m_y;
        }


        // flip shape if necessary
        if ( PhysShapePolygon* polygon = shape.m_shape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            polygon->setFlipped(flipped);
        }

        m_currentShape = shape.m_shape;
    }

#ifdef ITF_SUPPORT_EDITOR
    void ShapeComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if ( m_actor->isActive() && !isPaused() && m_currentShape )
        {
            drawDebug();
        }
    }

    void ShapeComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        if (getTemplate() && getTemplate()->getShape()) // null template occured on april 5th 2011
        {
            scaleFactoryShape();
        }

        for (ITF_MAP<StringID,ShapeData>::iterator it = m_shapes.begin(); it != m_shapes.end(); ++it)
        {
            ShapeData & shape = it->second;
            scaleShape(getTemplate()->getShapes()[shape.m_index].m_shape,&shape.m_shape);
        }


        updateShape();
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(ShapeComponent_Template)
    BEGIN_SERIALIZATION(ShapeComponent_Template)

        SERIALIZE_MEMBER("polyline", m_polyline);
        SERIALIZE_OBJECT_WITH_FACTORY("shape", m_factoryShape, PHYSWORLD->getShapesFactory());
        SERIALIZE_MEMBER("offset", m_offset);
        SERIALIZE_MEMBER("attachPolyline", m_attachPolyline);
        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifdef ITF_SUPPORT_DEBUGFEATURE
            SERIALIZE_MEMBER("drawDebug", m_drawDebug);
#endif // ITF_SUPPORT_DEBUGFEATURE
        END_CONDITION_BLOCK()
        SERIALIZE_CONTAINER_OBJECT("shapes",m_shapes);
        SERIALIZE_MEMBER("useAABBShape", m_useAABBShape);

    END_SERIALIZATION()

    BEGIN_SERIALIZATION(ShapeData_Template)
    SERIALIZE_MEMBER("name", m_name);
    SERIALIZE_OBJECT_WITH_FACTORY("shape", m_shape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("offset", m_offset);
    SERIALIZE_MEMBER("attachPolyline", m_attachPolyline);
    END_SERIALIZATION()


    ShapeComponent_Template::ShapeComponent_Template()
        : m_factoryShape(NULL)
        , m_offset(Vec2d::Zero)
#ifdef ITF_SUPPORT_DEBUGFEATURE
        , m_drawDebug(bfalse)
#endif // ITF_SUPPORT_DEBUGFEATURE
        , m_useAABBShape(bfalse)
    {
    }

    ShapeComponent_Template::~ShapeComponent_Template()
    {
        SF_DEL(m_factoryShape);
        for (u32 i = 0; i < m_shapes.size(); ++i)
        {
            SF_DEL(m_shapes[i].m_shape);
        }
    }
}
