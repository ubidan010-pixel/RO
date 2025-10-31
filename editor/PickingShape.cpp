#include "precompiled_editor.h"

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_PICKING_H_
#include "editor/picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_PLANE_H_
#include "core/math/plane.h"
#endif //_ITF_PLANE_H_


namespace ITF
{

const f32    PickingShape::m_disc_Radius            = 20.f;
const f32    PickingShape::m_disc_sqRadius          = PickingShape::m_disc_Radius * PickingShape::m_disc_Radius;
const f32    PickingShape::m_disc_picking_tolerance = 50.f;

void PickingShape::setOwner(const ObjectRef _owner)
{
    ITF_ASSERT(NULL != GETOBJECT(_owner));
    m_owner = _owner;
}


PickingShape::PickingShape(Interface_PickingShapePlugin* _creatorPlugin)
: m_data(BLOB_EXTRACT_AND_WRITE_ENABLED)
, m_shapeType(ShapeType_Invalid)
, m_owner(ITF_INVALID_OBJREF)
, m_lastUpdateFrame(0)
, m_isSubAnchor(bfalse)
, m_priority(0)
, m_creatorPickingShapePlugin(_creatorPlugin)
, m_snapWithMainAnchor(btrue)
{
}

PickingShape::~PickingShape()
{
    EDITOR->onPickingShapeDeleted(this);
    ITF_MAP<Plugin*, PickingShapePluginData*>::iterator iter;
    for (iter=m_PluginData.begin(); iter!=m_PluginData.end(); iter++)
    {
        SF_DEL(iter->second);
    }
}
const ObjectRef& PickingShape::getOwner() const
{
    ITF_ASSERT(m_owner.isValid());
    return m_owner;
}

const u32 PickingShape::getlastUpdateFrame() const
{
    return m_lastUpdateFrame;
}

void PickingShape::setlastUpdateFrame(u32 _frame)
{
    m_lastUpdateFrame = _frame;
}

//
void PickingShape::allocatePluginData()
{
    ITF_ASSERT_MSG(m_PluginData.size()==0, "You must allocate once...");
    ITF_ASSERT_MSG(PLUGINGATEWAY, "Init PLUGINGATEWAY before trying to allocate plugin datas");

    ITF_VECTOR<Plugin*> pluginList;
    PLUGINGATEWAY->getPluginList(pluginList);
    for (ITF_VECTOR<Plugin*>::iterator iter = pluginList.begin(); iter!=pluginList.end(); iter++)
    {
        std::pair<Plugin*, PickingShapePluginData*> item;
        item.first = *iter;
        item.second = (*iter)->createPickingShapeData(this);
        if (item.second)
            m_PluginData.insert(item);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////
/// Update status every frame. done before draw. . Default code calls each plugin's PickingShapePluginData  update
void PickingShape::update()
{
    for (ITF_MAP<Plugin*, PickingShapePluginData*>::const_iterator iter= m_PluginData.begin();
        iter!=m_PluginData.end();
        iter++)
    {
        iter->second->update();
    }
}

//////////////////////////////////////////////////////////////////////////
PickingShapePluginData *PickingShape::getPluginData(Plugin *_plugin)
{
    ITF_MAP<Plugin*, PickingShapePluginData*>::const_iterator iter = m_PluginData.find(_plugin);
    if (iter==m_PluginData.end())
        return NULL;
    return iter->second;
}

//////////////////////////////////////////////////////////////////////////
void PickingShape::draw()
{
    for (ITF_MAP<Plugin*, PickingShapePluginData*>::const_iterator iter= m_PluginData.begin();
        iter!=m_PluginData.end();
        iter++)
    {
        iter->second->draw();
    }

}

void PickingShape::handleMousePressed(const Vec2d &_mousePos)
{
    for (ITF_MAP<Plugin*, PickingShapePluginData*>::const_iterator iter= m_PluginData.begin();
        iter!=m_PluginData.end();
        iter++)
    {
        ITF_ASSERT(iter->second);
        iter->second->handleMousePressed(_mousePos);
    }
}
void PickingShape::handleMouseMove(const Vec2d &_mousePos)
{
    for (ITF_MAP<Plugin*, PickingShapePluginData*>::const_iterator iter= m_PluginData.begin();
        iter!=m_PluginData.end();
        iter++)
    {
        ITF_ASSERT(iter->second);
        iter->second->handleMouseMove(_mousePos);
    }
}
void PickingShape::handleMouseReleased(const Vec2d &_mousePos)
{
    for (ITF_MAP<Plugin*, PickingShapePluginData*>::const_iterator iter= m_PluginData.begin();
        iter!=m_PluginData.end();
        iter++)
    {
        ITF_ASSERT(iter->second);
        iter->second->handleMouseReleased(_mousePos);
    }
}

void PickingShape::handleMouseWheel( i32 _wheelDelta )
{
    for (ITF_MAP<Plugin*, PickingShapePluginData*>::const_iterator iter= m_PluginData.begin();
        iter!=m_PluginData.end();
        iter++)
    {
        ITF_ASSERT(iter->second);
        iter->second->handleMouseWheel(_wheelDelta);
    }
}

void PickingShape::rotateAroundAxisZ(f32 _deltaAngle, const Vec3d* _pRotationCenter/* = NULL*/)
{
    for (ITF_MAP<Plugin*, PickingShapePluginData*>::const_iterator iter= m_PluginData.begin();
        iter!=m_PluginData.end();
        iter++)
    {
        ITF_ASSERT(iter->second);
        iter->second->rotateAroundAxisZ(_deltaAngle, _pRotationCenter);
    }
}


//////////////////////////////////////////////////////////////////////////
bbool PickingShape::initPicking(PickingInfo2D* _pPicking2D)
{
    ITF_ASSERT(_pPicking2D);
    if (!_pPicking2D)
        return bfalse;

    _pPicking2D->m_sqrDist2D    =   F32_INFINITY;
    _pPicking2D->m_targetRef    =   ITF_INVALID_OBJREF;


    const BaseObject* owner = getOwnerPtr();
    if (!owner)
        return bfalse;

    _pPicking2D->m_anchor.clear();
    _pPicking2D->m_extraInformation = 0;
    _pPicking2D->m_targetRef        = getOwner();
    _pPicking2D->m_type             = owner->getObjectType();
    _pPicking2D->m_zDist            = 0.f;
    _pPicking2D->m_angle            = 0.0f;

    return btrue;
}

BaseObject* PickingShape::getOwnerPtr() const
{
    BaseObject* pOwner = GETOBJECT(m_owner);
    ITF_ASSERT(pOwner);
    return pOwner;
}

//

PickingShape_Disc::PickingShape_Disc(Interface_PickingShapePlugin* _creatorSpecific)
    : PickingShape(_creatorSpecific)
    , m_posType(Common)
    , m_angle(0.0f)
{
    m_shapeType = ShapeType_Disc;
}

void PickingShape_Disc::fillPickingInfo(PickingInfo2D* _pPicking2D)
{
    if (!initPicking(_pPicking2D))
        return;

    _pPicking2D->m_targetRadius     = m_disc_sqRadius;

    Vec3d pos2D;
    GFX_ADAPTER->compute3DTo2D(getPos(), pos2D);
    _pPicking2D->m_anchor               = pos2D.truncateTo2D() - EDITOR->getMouse2d();
    const f32 dist = _pPicking2D->m_anchor.sqrnorm();
    if (dist <= m_disc_picking_tolerance * m_disc_picking_tolerance * 0.25f)
        _pPicking2D->m_sqrDist2D        = dist;
    _pPicking2D->m_extraInformation     = m_data;
    _pPicking2D->m_targetRadius         = m_disc_picking_tolerance;
    _pPicking2D->m_zDist                = pos2D.m_z;

    const Vec2d nInvAnchor              = (EDITOR->getMouse2d() - pos2D.truncateTo2D()).normalize();
    _pPicking2D->m_angle                = Vec2d::XAxis.getOrientedAngle(nInvAnchor);
}

bool PickingShape_Disc::hasPriorityOver( const PickingShape* _pOther ) const
{
    if(getPriority() < _pOther->getPriority())
    {
        return true;
    }
    else if (getPriority() > _pOther->getPriority())
    {
        return false;
    }
    else
    {
        if(_pOther->getShapeType() == PickingShape::ShapeType_Disc)
        {
            return getPos().m_z > static_cast<const PickingShape_Disc*>(_pOther)->getPos().m_z;
        }
        else
        {
            return getOwner() < _pOther->getOwner();
        }
    }
}
    
bbool PickingShape_Disc::isUnderPoint2D(const Vec2d &_point, Vec3d &_intersection)
{
    bbool result = bfalse;

    const Vec3d pointWithZ(_point.m_x, _point.m_y, 0.1f);
    Vec3d linePoint[2];
    f32 dummy;

    //Test intersection between line corresponding to given _point on screen, and the object plane
    GFX_ADAPTER->compute2DTo3D(pointWithZ, linePoint[0]);
    linePoint[1] = CAMERA->getCorrectedPos();

    const f32 z = getPos().m_z;
    const Plane objectPlane(Vec3d(0,0,1), z);

    if(objectPlane.testLineIntersection(linePoint[0], linePoint[1]-linePoint[0], _intersection, dummy))
    {
        _intersection.m_z = z;

        // Test distance in screenspace
        Vec3d pos2D, intersection2D;
        GFX_ADAPTER->compute3DTo2D(getPos(), pos2D);
        GFX_ADAPTER->compute3DTo2D(_intersection, intersection2D);

        // potential issue here : the visible shape's screen size is unknown
        result = (pos2D.truncateTo2D() - intersection2D.truncateTo2D()).sqrnorm() < PickingShape::m_disc_sqRadius;
    }

    return result;
}

void PickingShape_Disc::dragByMouse( const Vec2d & _mousePos )
{
    Pickable* pPickable = static_cast<Pickable*>(getOwnerPtr());
    ITF_ASSERT(pPickable && pPickable->isSelected());

    PLUGINGATEWAY->onSetShapePosition(this, _mousePos);
}

PickingShape_Edge::PickingShape_Edge(Interface_PickingShapePlugin* _creatorSpecific)
: PickingShape(_creatorSpecific)
, m_z(0.0f)
{
    m_shapeType = ShapeType_Edge;
}


void PickingShape_Edge::fillPickingInfo(PickingInfo2D* _pPicking2D)
{
    if (!initPicking(_pPicking2D))
        return;

    _pPicking2D->m_targetRadius     =   m_disc_sqRadius;

    Vec3d point3DCoord, point2DCoord1, point2DCoord2;
    point3DCoord.m_x = m_pt1.m_x;
    point3DCoord.m_y = m_pt1.m_y;
    point3DCoord.m_z = m_z;
    GFX_ADAPTER->compute3DTo2D(point3DCoord, point2DCoord1);
    point3DCoord.m_x = m_pt2.m_x;
    point3DCoord.m_y = m_pt2.m_y;
    GFX_ADAPTER->compute3DTo2D(point3DCoord, point2DCoord2);
    Vec2d edge2d(point2DCoord2.m_x - point2DCoord1.m_x, point2DCoord2.m_y - point2DCoord1.m_y );
    f32 edgeLen = edge2d.norm();
    Vec2d nedge2d = edge2d;
    nedge2d = nedge2d / edgeLen;
    Vec2d nInvAnchor(EDITOR->getMouse2d() - point2DCoord1.truncateTo2D());
    f32 dot  = nInvAnchor.dot(nedge2d);

    if (dot > m_disc_Radius && dot < edgeLen - m_disc_Radius)
    {
        Vec2d proj(point2DCoord1.m_x + nedge2d.m_x * dot, point2DCoord1.m_y + nedge2d.m_y * dot);
        Vec2d anchor(proj - EDITOR->getMouse2d());
        nedge2d = nedge2d.getPerpendicular();
        dot = nInvAnchor.dot(nedge2d);
        if (fabs(dot) <= m_disc_picking_tolerance) // dist to the edge 
    {
            _pPicking2D->m_sqrDist2D            = anchor.sqrnorm();
        _pPicking2D->m_anchor               = point2DCoord1.truncateTo2D() - EDITOR->getMouse2d();
        _pPicking2D->m_zDist                = point2DCoord1.m_z;
        _pPicking2D->m_extraInformation     = m_data;
            _pPicking2D->m_targetRadius         = m_disc_picking_tolerance;
        nInvAnchor.normalize();
        _pPicking2D->m_angle                = Vec2d::XAxis.getOrientedAngle(nInvAnchor);
    }
}
}

void PickingShape_Edge::setPt1(const Vec2d& _pt)
{
    m_pt1 = _pt;
}

void PickingShape_Edge::setPt2(const Vec2d& _pt)
{
    m_pt2 = _pt;
}

void PickingShape_Edge::setPts(const Vec2d& _pt1, const Vec2d& _pt2)
{
    m_pt1 = _pt1;
    m_pt2 = _pt2;
}

void PickingShape_Edge::setPts(const PolyLineEdge& edge, f32 _z)
{
    m_pt1 = edge.getPos();
    m_pt2 = edge.getPos() + edge.m_vector;
    setDepth(_z);
}

void PickingShape_Edge::setDepth(f32 _z)
{
    m_z = _z;
}

const Vec2d& PickingShape_Edge::getPt1() const
{
    return m_pt1;
}

const Vec2d& PickingShape_Edge::getPt2() const
{
    return m_pt2;
}

const f32 PickingShape_Edge::getDepth()
{
    return m_z;
}

void PickingShape_Edge::dragByMouse( const Vec2d & _mousePos )
{
    Pickable* pPickable = static_cast<Pickable*>(getOwnerPtr());
    ITF_ASSERT(pPickable && pPickable->isSelected());

    PLUGINGATEWAY->onSetShapePosition(this, _mousePos);
}

bbool PickingShape_Edge::isUnderPoint2D( const Vec2d &_point, Vec3d &_intersection )
{
    return bfalse; // Temp fix edge picking (just reverted)
    /*
    Vec3d point3DCoord, point2DCoord1, point2DCoord2;
    point3DCoord.m_x = m_pt1.m_x;
    point3DCoord.m_y = m_pt1.m_y;
    point3DCoord.m_z = m_z;
    GFX_ADAPTER->compute3DTo2D(point3DCoord, point2DCoord1);
    point3DCoord.m_x = m_pt2.m_x;
    point3DCoord.m_y = m_pt2.m_y;
    GFX_ADAPTER->compute3DTo2D(point3DCoord, point2DCoord2);

    const f32 fDist = fabs(PointSignedDistToSegment(_point, point2DCoord1.truncateTo2D(), point2DCoord2.truncateTo2D()));

    return fDist < PickingShape::m_disc_sqRadius;
    */
}

} //namespace ITF

#endif //ITF_CONSOLE_FINAL
