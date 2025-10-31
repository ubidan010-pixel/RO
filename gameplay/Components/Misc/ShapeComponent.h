#ifndef _ITF_SHAPECOMPONENT_H_
#define _ITF_SHAPECOMPONENT_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{
    class ShapeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ShapeComponent, ActorComponent,1137022961);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        ShapeComponent();
        virtual ~ShapeComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        bbool               isValid() const { return m_currentShape != NULL; }
        PhysShape*          getShape() const { return m_currentShape; }
        const Vec2d&        getShapePos() const { ITF_ASSERT(m_currentShape); return m_shapePos; }
        PhysShape*          getFactoryShapeScaled() const { return m_factoryShapeScaled; }

        void                setShape(StringID _shapeName);
        void                setDefaultShape();

    protected:

        PhysShape*          getCurrentShape() const { return m_currentShape; }
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug() const;
#endif // ITF_SUPPORT_DEBUGFEATURE

        struct ShapeData
        {
            u32 m_index;
            PhysShape * m_shape;
        };

    private:
        ITF_INLINE const class ShapeComponent_Template*  getTemplate() const;

        AnimLightComponent* m_animComponent;

        void                updateShape();
        void                updateCurrentShape();

        bbool               usePolylineShape() const;
        bbool               isPolylineValid() const;    // for validation

        void                scaleFactoryShape();
        void                updateFactoryShape();

        void                scaleShape(const PhysShape * _source, PhysShape ** _dest);

        void                updatePolylineShape();
        bbool               getPolylinePoints( const Vec2d*& _points, u32& _pointCount );
        void                updatePolylineShapePoints( const Vec2d* _points, u32 _pointCount );

        PhysShape*          m_currentShape;     // the currently used shape; can be set externally via setShape()
        Vec2d               m_shapePos;

        PhysShape*          m_factoryShapeScaled;

        PhysShapePolygon    m_polylineShape;    // our own shape, created from a polyline (optional)
        PhysShapePolygon::PointsList m_shapePoints;

        StringID            m_currentShapeName;

        ITF_MAP<StringID,ShapeData> m_shapes;

        PhysShapePolygon    m_AABBShape;

    };

    struct ShapeData_Template
    {
        DECLARE_SERIALIZE();
        ShapeData_Template() 
            : m_shape(NULL),m_offset(Vec2d::Zero)
        {
        }
        StringID    m_name;
        PhysShape * m_shape;
        Vec2d       m_offset;
        StringID    m_attachPolyline;
    };

    //-------------------------------------------------------------------------------------
    class ShapeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ShapeComponent_Template,TemplateActorComponent,112285537);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ShapeComponent);

    public:

        ShapeComponent_Template();
        ~ShapeComponent_Template();

        const StringID&                             getPolylineID() const { return m_polyline; }
        const PhysShape*                            getShape() const { return m_factoryShape; }
        const Vec2d&                                getOffset() const { return m_offset; }
#ifdef ITF_SUPPORT_DEBUGFEATURE
        bbool                                       getDrawDebug() const { return m_drawDebug; }
#endif // ITF_SUPPORT_DEBUGFEATURE
        const StringID&                             getAttachPolyline() const { return m_attachPolyline; }
        ITF_INLINE const FixedArray<ShapeData_Template,4> &  getShapes() const { return m_shapes; }

        ITF_INLINE ITF::bbool getUseAABBShape() const { return m_useAABBShape; }
    private:

        StringID            m_polyline;         // the name of a polyline (from the animation) to create a shape from

        // factory shape parameters
        PhysShape*          m_factoryShape;     // our own shape, created from the factory (optional)
        Vec2d               m_offset;           // an offset we can give to our shape (not used for animation polylines)
        StringID            m_attachPolyline;   // the shape can be attached to a polyline (and thus appear / disappear with the animation)

        FixedArray<ShapeData_Template,4> m_shapes;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        bbool               m_drawDebug;
#endif // ITF_SUPPORT_DEBUGFEATURE
        bbool               m_useAABBShape;

    };




    ITF_INLINE const ShapeComponent_Template*  ShapeComponent::getTemplate() const {return static_cast<const ShapeComponent_Template*>(m_template);}
}

#endif // _ITF_SHAPECOMPONENT_H_