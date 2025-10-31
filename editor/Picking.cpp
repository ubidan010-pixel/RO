#include "precompiled_editor.h"

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_


namespace ITF
{
    #define PICKING_TOLERANCE_FRAMEBUFFERED_ACTOR     60.f
    #define PICKING_TOLERANCE_FRAMEBUFFERED_FRISE     70.f

    PickingShape* Picking::findUnderMouseRealPickingShape(PickingInfo2D& _pickingInfo, const PickingShapeVector& _orderedShapesList)
    {
        PickingShape* pNearestShape = NULL;

        // Find the shape under the mouse with the highest priority
        {
            PickingShapeVector::const_reverse_iterator it = _orderedShapesList.rbegin();
            PickingShapeVector::const_reverse_iterator itEnd = _orderedShapesList.rend();
            while(it != itEnd)
            {
                PickingShape* pShape = (*it);
                if (pShape->getPriority() < PickingShape::m_UnselectablePriority)
                {
                    Vec3d intersection;
                    if(pShape->isUnderPoint2D(EDITOR->getMouse2d(), intersection))
                    {
                        pNearestShape = pShape;
                        pShape->fillPickingInfo(&_pickingInfo);

                        break;
                    }
                }
                ++it;
            }
        }

        // Test all picking shapes, they have the priority over the frame buffer
        if(!pNearestShape)
        {
            f32 nearestDist2D = PickingShape::m_disc_picking_tolerance * PickingShape::m_disc_picking_tolerance * 2.f;

            PickingShapeVector::const_reverse_iterator it = _orderedShapesList.rbegin();
            PickingShapeVector::const_reverse_iterator itEnd = _orderedShapesList.rend();
            while(it != itEnd)
            {
                PickingShape* pShape = (*it);

                if (pShape->getPriority() < PickingShape::m_UnselectablePriority)
                {
                    PickingInfo2D info2D;
                    pShape->fillPickingInfo(&info2D);

                    if (info2D.m_sqrDist2D < nearestDist2D)
                    {
                        pNearestShape = pShape;

                        ITF_MEMCOPY(&_pickingInfo, &info2D, sizeof(PickingInfo2D));
                        nearestDist2D = info2D.m_sqrDist2D;
                    }
                }
                ++it;
            }
        }
       
        return pNearestShape;
    }

    void Picking::fillPickingInfo( const Pickable* _pPickable, PickingInfo2D* _pPicking2D )
    {
        ITF_ASSERT(_pPickable && _pPicking2D);
        if(!_pPickable)
            return;

        const BaseObject::ObjectType& e = _pPickable->getObjectType();
        switch(e)
        {
        case BaseObject::ePolyLine:
        case BaseObject::eFrise:
        case BaseObject::eActor:
            {
                const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_pPickable->getRef());
                ITF_ASSERT(shapes);
                if (shapes)
                {
                    for (u32 iShape = 0; iShape < shapes->size(); iShape++)
                    {
                        PickingShape *shape = (*shapes)[iShape];
                        if (!shape->isSubAnchor())
                        {
                            shape->fillPickingInfo(_pPicking2D);
                            break;
                        }
                    }
                }
            }
            break;

        case BaseObject::eTrajectory:
            break;

        case BaseObject::eInvalid:
            ITF_ASSERT(0); // "Trying to pick an invalid object"
            break;

        default:
            ITF_ASSERT(0); // "Please implement picking for this pickable"
            break;
        }
    }


} //namespace ITF

#endif //ITF_CONSOLE_FINAL
