#ifndef _ITF_PICKING_H_
#define _ITF_PICKING_H_

#ifndef _ITF_EDITOR_TYPES_H_
#include "editor/Editor_Types.h"
#endif //_ITF_EDITOR_TYPES_H_

namespace ITF
{

#define ITF_POLYLINE_SPHERE_RADIUS  0.2f

#define ITF_POLYLINE_SPHERE_RADIUS_2D  20.f
#define ITF_ANCHOR_PICKING_RADIUS   100.0f

#define ITF_PICKING_2D_MAX_DIST     50.0f
#define ITF_BIG_EPSILON             0.005f

    class Actor;
    class Frise;
    class PolyLine;
    class Trajectory;
    class Pickable;
    struct PickingInfo;
    class PickingShape;



    struct PickingInfo2D
    {
        PickingInfo2D()
        {
            m_type              = BaseObject::eInvalid;
            m_targetRef         = ITF_INVALID_OBJREF;

            m_anchor            = Vec2d::Zero;
            m_sqrDist2D         = 0.0f;
            m_zDist             = 0.0f;
            m_targetRadius      = 0.0f;
            m_angle             = 0.0f;
            m_extraInformation  = 0;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// make a basic main anchor for given pickable
        /// @param _pickable
        void makeBasicMainAnchor(Pickable *_pickable);

        u32                     m_extraInformation; //Convention : 0 is valid for basic main anchor
        ObjectRef               m_targetRef;
        BaseObject::ObjectType  m_type;
        Vec2d                   m_anchor;
        f32                     m_sqrDist2D;
        f32                     m_zDist;
        f32                     m_targetRadius;
        f32                     m_angle;
    };

    class Picking
    {
    public:
        /// Get best picking info
        /// @param _pickingInfo the picking info of the nearest shape
        /// @param _shapesList picking shapes to test
        /// @return the nearest shape
        static PickingShape* findUnderMouseRealPickingShape(PickingInfo2D& _pickingInfo, const PickingShapeVector& _orderedShapesList);

        /// Fill info of a particular pickable object in  generic way
        /// @param _pPickable the object
        /// @param _pPickingInfo [out] the info
        static void fillPickingInfo(const Pickable* _pPickable, PickingInfo2D* _pPickingInfo);
    };

} // namespace ITF

#endif // _ITF_PICKING_H_