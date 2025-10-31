#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#define _ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{
    class ShapeDetectorComponent : public DetectorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ShapeDetectorComponent, DetectorComponent,1842589967);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        ShapeDetectorComponent();
        virtual ~ShapeDetectorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onSceneActive();
        virtual void        onCheckpointLoaded();
        virtual void        Update( f32 _dt );

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE PhysShape* getCurrentShape() const { return m_currentShape; }

        Vec2d               getRefPos( const Vec2d& _pos ) const;
        const Vec2d&        getShapePos() const { return m_shapePos; }

        void                setScaleMultiplier(f32 _scaleMultiplier) {m_scaleMultiplier = _scaleMultiplier;scaleFactoryShape();}

    private:

        void                clear();
        void                updateShapeAndAABB(bbool _init = bfalse);

        void                updateAnimPolyline(bbool _init = bfalse);
        void                updateFactoryShape(bbool _init = bfalse);
        void                scaleFactoryShape();

        AnimLightComponent*             m_animComponent;

        // anim shape
        PhysShapePolygon                m_animShape;
        PhysShapePolygon::PointsList    m_animShapePoints;
        PhysShapePolyline               m_animShapePolyline;
        PolyLine                        m_polyline;

        // factory shape
        PhysShape*                      m_factoryShapeScaled;

        f32                             m_scaleMultiplier;
   
    protected:

        ITF_INLINE const class ShapeDetectorComponent_Template*  getTemplate() const;

        PhysShape*                      m_currentShape;
        Vec2d                           m_refPos;
        Vec2d                           m_shapePos;
    };

    //-------------------------------------------------------------------------------------
    class ShapeDetectorComponent_Template : public DetectorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(ShapeDetectorComponent_Template,DetectorComponent_Template,4056266900);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ShapeDetectorComponent);

    public:

        ShapeDetectorComponent_Template();
        ~ShapeDetectorComponent_Template();

        const PhysShape*                getShape() const { return m_factoryShape; }
        const Vec2d&                    getOffset() const { return m_offset; }
        const StringID&                 getAttachPolyline() const { return m_attachPolyline; }

        const StringID&                 getAnimPolylineId() const { return m_animPolylineId; }
        const StringID&                 getAnimRefPosPointId() const { return m_animRefPosPointId; }
        const StringID&                 getAnimShapePosPointId() const { return m_animShapePosPointId; }

        ITF_INLINE bbool                getShapeIsConcave() const { return m_shapeIsConcave; }

    private:

        PhysShape*                      m_factoryShape;
        Vec2d                           m_offset;
        StringID                        m_attachPolyline;   // the shape can be attached to a polyline (and thus appear / disappear with the animation)

        StringID                        m_animPolylineId;
        StringID                        m_animRefPosPointId;
        StringID                        m_animShapePosPointId;

        bbool                           m_shapeIsConcave;
    };


    const ShapeDetectorComponent_Template*  ShapeDetectorComponent::getTemplate() const {return static_cast<const ShapeDetectorComponent_Template*>(m_template);}
};

#endif //_ITF_SHAPEDETECTORCOMPONENT_H_