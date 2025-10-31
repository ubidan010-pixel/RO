#include "precompiled_engine.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////       polyline class implementation
///////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PolyLine)

    BEGIN_SERIALIZATION(PolyLineEdge)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable|ESerialize_Editor)
            SERIALIZE_MEMBER("POS",m_pos);
        END_CONDITION_BLOCK()

        // TEMP HANDLING OF OLD HOLE DATA
        u32 tempHole = (u32)m_holeMode;

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Deprecate)
            SERIALIZE_MEMBER("Hole", tempHole);
        END_CONDITION_BLOCK()

        m_holeMode = (HoleMode)tempHole;

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_ENUM_BEGIN("HoleMode",m_holeMode);
                SERIALIZE_ENUM_VAR(Hole_None);
                SERIALIZE_ENUM_VAR(Hole_Collision);
                SERIALIZE_ENUM_VAR(Hole_Visual);
                SERIALIZE_ENUM_VAR(Hole_Both);
            SERIALIZE_ENUM_END();
            SERIALIZE_MEMBER("Scale",m_scale);
            SERIALIZE_MEMBER("SwitchTexture",m_switchTexture);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()


    BEGIN_SERIALIZATION_CHILD(PolyLine)

        SERIALIZE_FUNCTION(startLoadPolyLine,ESerialize_Data_Load);
        SERIALIZE_FUNCTION(startSavePolyLine,ESerialize_Data_Save);

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable|ESerialize_Editor)
            SERIALIZE_CONTAINER_OBJECT("POINTS",m_edgeData.getMutableEdgeData());
        END_CONDITION_BLOCK()

        SERIALIZE_FUNCTION(endLoadPolyLine, ESerialize_Data_Load);
        SERIALIZE_FUNCTION(endSavePolyLine, ESerialize_Data_Save);

    END_SERIALIZATION()


PolyLine::PolyLine()
: Super()
, m_ownerFrise(NULL)
, m_ownerActor(NULL)
, m_physBody(NULL)
, m_physShape(NULL)
, m_speedLoss(0.f)
, m_querriedRecomputeData(btrue)
, m_isCollisionActive(btrue)
, m_bodyRegistered(bfalse)
, m_angle(0.f)
, m_isSerializable(btrue)
{    
    setObjectType(BaseObject::ePolyLine);
    m_bHoldsFrise = bfalse;
    m_binding.setOwner(this);
    m_connection.setCurrent(this);
}

PolyLine::~PolyLine()
{
    if (m_ownerFrise)
        unregisterPhysic();
    clear();

    SF_DEL(m_physShape);
    
    if ( m_physBody )
    {
        PHYSWORLD->deallocBody(m_physBody);
    }
}


void PolyLine::offsetData(const Vec2d& _offset) 
{
	for (u32 j = 0; j < getPosCount(); j++)
	{
		setPosAt(getPosAt(j) + _offset, j);
	}
}


void PolyPointList::transferEdgeProperties(u32 _index, const PolyLineEdge& _edg)
{
    memcpy(&m_edgeData[_index], &_edg, sizeof(PolyLineEdge)); // ugly, yet the best way not to forget a new attribute
}


void PolyLine::onLoaded()
{
    if (getPosCount() >= 1)
    {
        set2DPos(getPosAt(0));

        changeRelativeAABB(AABB(Vec2d(0.f,0.f),Vec2d(0.f,0.f)));
    }
    forceRecomputeData();
}

PolyPointList::~PolyPointList()
{
    m_edgeData.clear();
}


void PolyPointList::addPoint(const Vec2d& _pos, i32 _at)
{
    PolyLineEdge newEdge(_pos);
    if ((_at<0) ||(_at>=(i32)m_edgeData.size()))
    {
        m_edgeData.push_back(newEdge);
    }
    else
    {
        ITF_VECTOR<PolyLineEdge>::iterator pIt = m_edgeData.begin();
        for (i32 i = 0; i < _at; i++, ++pIt);
        m_edgeData.insert(pIt, newEdge);
    }
	if (m_edgeData.size() == 1)
	{
		m_aabb.setMin(_pos);
		m_aabb.setMax(_pos);
	}
	m_aabb.grow(_pos);
}

void PolyPointList::addEdge( const PolyLineEdge& _edge)
{
    m_edgeData.push_back(_edge);

    Vec2d pos = _edge.getPos();

    if (m_edgeData.size() == 1)
    {
        m_aabb.setMin(pos);
        m_aabb.setMax(pos);
    }
    m_aabb.grow(pos);
}

void PolyPointList::setPosAt(const Vec2d& _pos, u32 _i)
{
    m_edgeData[_i].setPos(_pos);

    if ( m_loop && getPosCount() > 2 && _i == getPosCount()-1 )
        m_edgeData[0].setPos(_pos);

    m_aabb.grow(_pos);
}



void PolyPointList::updateDataAt(u32 _index)
{
    PolyLineEdge& thisData = m_edgeData[_index];
    PolyLineEdge& nextData = m_edgeData[_index+1];

    m_aabb.grow(thisData.getPos());
    m_aabb.grow(nextData.getPos());

    // update this data
    Vec2d vector = nextData.getPos() - thisData.getPos();
    thisData.m_vector = vector;
    thisData.m_length = vector.norm();

    if (thisData.m_length > 0.f)
        thisData.m_normalizedVector = vector / thisData.m_length;
    else
        thisData.m_normalizedVector = Vec2d(0.f,0.f);
}

Vec2d	PolyPointList::getCenter()
{
	if (!m_edgeData.size())
		return Vec2d::Zero;

	Vec2d Center = m_edgeData[0].getPos();
	if (m_edgeData.size() == 1)
		return Center;	// avoid div 0

    for (u32 i = 1; i < m_edgeData.size(); i++)
    {
        Center += m_edgeData[i].getPos();
    }
	return Center / ((float)m_edgeData.size() - 1.f);
}

void PolyPointList::executeRecomputeData()
{
    if (m_edgeData.size())
    {
        if ( m_loop && getPosCount() > 2 )
        {
            m_edgeData[getPosCount()-1].setPos( m_edgeData[0].getPos() );
        }


		if (!m_edgeData.size())
		{
			m_aabb.setMin(Vec2d::Zero);
			m_aabb.setMax(Vec2d::Zero);
		}
		else
		{
			m_aabb.setMin(getPosAt(0));
			m_aabb.setMax(getPosAt(0));
		}

        m_length = 0.f;
        for (u32 i = 0; i < m_edgeData.size()-1; i++)
        {
            updateDataAt(i);
            m_length += m_edgeData[i].getLength();
        }

		if (m_aabb.getMin().m_x > m_aabb.getMax().m_x)
		{
			f32 min = m_aabb.getMin().m_x;
			m_aabb.setMin(Vec2d(m_aabb.getMax().m_x, m_aabb.getMin().m_y));
			m_aabb.setMax(Vec2d(min, m_aabb.getMax().m_y));
		}
		if (m_aabb.getMin().m_y > m_aabb.getMax().m_y)
		{
			f32 min = m_aabb.getMin().m_y;
			m_aabb.setMin(Vec2d(m_aabb.getMin().m_x, m_aabb.getMax().m_y));
			m_aabb.setMax(Vec2d(m_aabb.getMax().m_x, min));
		}
    }
}

void PolyLine::executeRecomputeData()
{
	if (!getPosCount())
		return;
    ITF_ASSERT(m_querriedRecomputeData);
    m_querriedRecomputeData = bfalse;

	checkLoop();
    m_edgeData.executeRecomputeData();
	set2DPos(getPosAt(0));

    if(m_ownerActor)
    {
        setDepth(m_ownerActor->getDepth());
    }

    if ( m_physBody )
    {
        m_physBody->setRecomputeAABB();
    }

	m_relativeAABB.setMin(m_edgeData.getAABB().getMin() - get2DPos());
	m_relativeAABB.setMax(m_edgeData.getAABB().getMax() - get2DPos());
    AABB relativeAABB = getAABB();
    setPos(Vec3d(getAABB().getCenter().m_x, getAABB().getCenter().m_y, getDepth()));
    relativeAABB.setMin(relativeAABB.getMin()-get2DPos());
    relativeAABB.setMax(relativeAABB.getMax()-get2DPos());
    changeRelativeAABB(relativeAABB);
}

void PolyLine::requestDestruction()
{
	if (m_ownerActor)
	{
		ITF_ASSERT(!m_ownerFrise);
		ITF_ASSERT_MSG(bfalse, "call the owner actor instead");
	}
	if (m_ownerFrise)
	{
		ITF_ASSERT(!m_ownerActor);
		m_ownerFrise->deleteOwnedPolyline(this);
	}
	ITF_ASSERT_MSG(bfalse, "Polyline owned by no-one");
	delete this;
}

void PolyPointList::setGameMaterial( const StringID& _id)
{
    for (u32 i = 0; i < m_edgeData.size(); i++)
    {
        setGameMaterialAt(_id, i);
    }
}

bbool PolyPointList::erasePosAt(u32 _index)
{
    if (_index >= m_edgeData.size())
        return bfalse;

    m_edgeData.erase(m_edgeData.begin() + _index);

    if (m_edgeData.size() <= 1 || (isLooping() && m_edgeData.size() <= 2 ))
    {
        return btrue;
    }

    return bfalse;
}

bbool	PolyLine::erasePosAt(   u32 _index ) 
{
	bbool ret = m_edgeData.erasePosAt(_index); 
	if (ret)
		requestDestruction();
	else
		recomputeData();
	return ret;
}

void PolyPointList::setGameMaterialAt( const StringID& _id, i32 _at)
{
    m_edgeData[_at].setGameMaterial(_id);
}

void PolyLine::clear()
{
    m_edgeData.clear();
    m_connection.reset();
}

void PolyLine::setDepth( f32 _depth )
{
    if ( getDepth() != _depth )
    {
        m_pos.m_z = _depth;

        if ( m_bodyRegistered )
        {
            unregisterPhysic();
            registerPhysic();
        }
    }
}


void PolyLine::startLoadPolyLine()
{
/*    m_pivot.m_x = F32_INFINITY;
    m_pivot.m_y = F32_INFINITY;*/
}

void PolyLine::endLoadPolyLine()
{/*
    if (m_pivot.m_x == F32_INFINITY && m_pivot.m_y == F32_INFINITY && getPosCount())
        setPivot(getPosAt(0));

    changeEdgeDataToAbsolute();*/
}

void PolyLine::startSavePolyLine()
{
    // Save relative positions
    // So before save, tranform all edge data
    changeEdgeDataToRelative();
}

void PolyLine::endSavePolyLine()
{
    // Back to absolute
//    changeEdgeDataToAbsolute();
}


// The pivot is the first edge pos, so it stays absolute
void PolyLine::changeEdgeDataToRelative()
{
    ITF_ASSERT_MSG(getPosCount() > 0, "Must be at least one edge");

    for(u32 i = 1; i < getPosCount(); ++i)
		setPosAt(getPosAt(i) - getPosAt(0), i);
}

void PolyLine::changeEdgeDataToAbsolute()
{
    for(u32 i = 1; i < getPosCount(); ++i)
		setPosAt(getPosAt(i) + getPosAt(0), i);
}

bbool PolyLine::isPointInside(const Vec2d& _pos) const
{   // algorithm: cast a ray from _pos to any direction : if we hit an even polyline edge count, we are inside.
    // to determine the direction, we identify the nearest side so that we check the minimum edge count

    // determine dist to nearest boundary
    const AABB& aabb = getAABB();
    f32 ptToXMax = aabb.getMax().m_x - _pos.m_x;
    if (ptToXMax < 0.f)
        return bfalse;

    f32 ptToYMax = aabb.getMax().m_y - _pos.m_y;
    if (ptToYMax < 0.f)
        return bfalse;

    f32 ptToXMin = _pos.m_x - aabb.getMin().m_x;
    if (ptToXMin < 0.f)
        return bfalse;

    f32 ptToYMin = _pos.m_y - aabb.getMin().m_y;
    if (ptToYMin < 0.f)
        return bfalse;

    if (ptToXMax < ptToYMax)
    {
        if ((ptToXMax < ptToXMin) && (ptToXMax < ptToYMin))
            return isPointInside_right(_pos);
    }
    else
    {
        if ((ptToYMax < ptToXMin) && (ptToYMax < ptToYMin))
            return isPointInside_up(_pos);
    }

    if (ptToXMin < ptToYMin)
    {
        if ((ptToXMin < ptToXMax) && (ptToXMin < ptToYMax))
            return isPointInside_left(_pos);
    }
    else
    {
        if ((ptToYMin < ptToXMax) && (ptToYMin < ptToYMax))
            return isPointInside_down(_pos);
    }

    ITF_ASSERT(0);
    return bfalse;
}

bbool PolyLine::isPointInside_up(const Vec2d& _pos) const
{
    u32 colCount = 0;
    i32 count = (i32)getVectorsCount();

    for (i32 i = 0; i < count; i++)
    {
        const PolyLineEdge& edge = getEdgeAt(i);
        const Vec2d& p1 = edge.getPos();
        const Vec2d& p2 = getPosAt(i+1);
        f32 p2x = p2.m_x;
        f32 p2y = p2.m_y;

        if (edge.m_normalizedVector.m_x == 0.f)
            continue; // edge is vertical

        if ( _pos.m_x < p1.m_x && _pos.m_x < p2x )
            continue; // point is to the left of the edge

        if ( _pos.m_x > p1.m_x && _pos.m_x > p2x )
            continue; // point is to the right of the edge

        if ( _pos.m_y > p1.m_y && _pos.m_y > p2y )
            continue; // point is above the edge

        f32 t = ( _pos.m_x - p1.m_x ) / edge.m_vector.m_x;
        f32 yIntersection = p1.m_y + ( edge.m_vector.m_y * t );

        if ( yIntersection < _pos.m_y )
            continue; // Point is above the edge

        colCount++;
    }

    if ( count > 1 )
    {
        const Vec2d& p1 = getPosAt(getPosCount()-1);
        const Vec2d& p2 = getPosAt(0);
        Vec2d dir = p2 - p1;
        f32 p2x = p2.m_x;
        f32 p2y = p2.m_y;

        if (dir.m_x == 0.f)
        {
            // edge is vertical
        }
        else if ( _pos.m_x < p1.m_x && _pos.m_x < p2x )
        {
            // point is to the left of the edge
        }
        else if ( _pos.m_x > p1.m_x && _pos.m_x > p2x )
        {
            // point is to the right of the edge
        }
        else if ( _pos.m_y > p1.m_y && _pos.m_y > p2y )
        {
            // point is above the edge
        }
        else
        {
            f32 t = ( _pos.m_x - p1.m_x ) / dir.m_x;
            f32 yIntersection = p1.m_y + ( dir.m_y * t );

            if ( yIntersection < _pos.m_y )
            {
                // Point is above the edge
            }
            else
            {
                colCount++;
            }
        }
    }

    return (colCount&1) == 1;
}

bbool PolyLine::isPointInside_down(const Vec2d& _pos) const
{
    u32 colCount = 0;
    i32 count = (i32)getVectorsCount();

    for (i32 i = 0; i < count; i++)
    {
        const PolyLineEdge& edge = getEdgeAt(i);
        const Vec2d& p1 = edge.getPos();
        const Vec2d& p2 = getPosAt(i+1);
        f32 p2x = p2.m_x;
        f32 p2y = p2.m_y;

        if (edge.m_normalizedVector.m_x == 0.f)
            continue; // edge is vertical

        if ( _pos.m_x < p1.m_x && _pos.m_x < p2x )
            continue; // point is to the left of the edge

        if ( _pos.m_x > p1.m_x && _pos.m_x > p2x )
            continue; // point is to the right of the edge

        if ( _pos.m_y < p1.m_y && _pos.m_y < p2y )
            continue; // point is below the edge

        f32 t = ( _pos.m_x - p1.m_x ) / edge.m_vector.m_x;
        f32 yIntersection = p1.m_y + ( edge.m_vector.m_y * t );

        if ( yIntersection > _pos.m_y )
            continue; // Point is below the edge

        colCount++;
    }

    if ( count > 1 )
    {
        const Vec2d& p1 = getPosAt(getPosCount()-1);
        const Vec2d& p2 = getPosAt(0);
        Vec2d dir = p2 - p1;
        f32 p2x = p2.m_x;
        f32 p2y = p2.m_y;

        if (dir.m_x == 0.f)
        {
            // edge is vertical
        }
        else if ( _pos.m_x < p1.m_x && _pos.m_x < p2x )
        {
            // point is to the left of the edge
        }
        else if ( _pos.m_x > p1.m_x && _pos.m_x > p2x )
        {
            // point is to the right of the edge
        }
        else if ( _pos.m_y < p1.m_y && _pos.m_y < p2y )
        {
            // point is below the edge
        }
        else
        {
            f32 t = ( _pos.m_x - p1.m_x ) / dir.m_x;
            f32 yIntersection = p1.m_y + ( dir.m_y * t );

            if ( yIntersection > _pos.m_y )
            {
                // Point is above the edge
            }
            else
            {
                colCount++;
            }
        }
    }

    return (colCount&1) == 1;
}

bbool PolyLine::isPointInside_right(const Vec2d& _pos) const
{
    u32 colCount = 0;
    i32 count = (i32)getVectorsCount();

    for (i32 i = 0; i < count; i++)
    {
        const PolyLineEdge& edge = getEdgeAt(i);
        const Vec2d& p1 = edge.getPos();
        const Vec2d& p2 = getPosAt(i+1);
        f32 p2x = p2.m_x;
        f32 p2y = p2.m_y;

        if (edge.m_normalizedVector.m_y == 0.f)
            continue; // edge is horizontal

        if ( _pos.m_y < p1.m_y && _pos.m_y < p2y )
            continue; // point is below the edge

        if ( _pos.m_y > p1.m_y && _pos.m_y > p2y )
            continue; // point is above the edge

        if ( _pos.m_x > p1.m_x && _pos.m_x > p2x )
            continue; // point is to the right of the edge

        f32 t = ( _pos.m_y - p1.m_y ) / edge.m_vector.m_y;
        f32 xIntersection = p1.m_x + ( edge.m_vector.m_x * t );

        if ( xIntersection < _pos.m_x )
            continue; // Point is to the right of the edge

        colCount++;
    }

    if ( count > 1 )
    {
        const Vec2d& p1 = getPosAt(getPosCount()-1);
        const Vec2d& p2 = getPosAt(0);
        Vec2d dir = p2 - p1;
        f32 p2x = p2.m_x;
        f32 p2y = p2.m_y;

        if (dir.m_x == 0.f)
        {
            // edge is horizontal
        }
        else if ( _pos.m_y < p1.m_y && _pos.m_y < p2y )
        {
            // point is below the edge
        }
        else if ( _pos.m_y > p1.m_y && _pos.m_y > p2y )
        {
            // point is above the edge
        }
        else if ( _pos.m_x > p1.m_x && _pos.m_x > p2x )
        {
            // point is to the right of the edge
        }
        else
        {
            f32 t = ( _pos.m_y - p1.m_y ) / dir.m_y;
            f32 xIntersection = p1.m_x + ( dir.m_x * t );

            if ( xIntersection < _pos.m_x )
            {
                // Point is to the right of the edge
            }
            else
            {
                colCount++;
            }
        }
    }

    return (colCount&1) == 1;
}


bbool PolyLine::isPointInside_left(const Vec2d& _pos) const
{
    u32 colCount = 0;
    i32 count = (i32)getVectorsCount();

    for (i32 i = 0; i < count; i++)
    {
        const PolyLineEdge& edge = getEdgeAt(i);
        const Vec2d& p1 = edge.getPos();
        const Vec2d& p2 = getPosAt(i+1);
        f32 p2x = p2.m_x;
        f32 p2y = p2.m_y;

        if (edge.m_normalizedVector.m_y == 0.f)
            continue; // edge is horizontal

        if ( _pos.m_y < p1.m_y && _pos.m_y < p2y )
            continue; // point is below the edge

        if ( _pos.m_y > p1.m_y && _pos.m_y > p2y )
            continue; // point is above the edge

        if ( _pos.m_x < p1.m_x && _pos.m_x < p2x )
            continue; // point is to the left of the edge

        f32 t = ( _pos.m_y - p1.m_y ) / edge.m_vector.m_y;
        f32 xIntersection = p1.m_x + ( edge.m_vector.m_x * t );

        if ( xIntersection > _pos.m_x )
            continue; // Point is to the left of the edge

        colCount++;
    }

    if ( count > 1 )
    {
        const Vec2d& p1 = getPosAt(getPosCount()-1);
        const Vec2d& p2 = getPosAt(0);
        Vec2d dir = p2 - p1;
        f32 p2x = p2.m_x;
        f32 p2y = p2.m_y;

        if (dir.m_x == 0.f)
        {
            // edge is horizontal
        }
        else if ( _pos.m_y < p1.m_y && _pos.m_y < p2y )
        {
            // point is below the edge
        }
        else if ( _pos.m_y > p1.m_y && _pos.m_y > p2y )
        {
            // point is above the edge
        }
        else if ( _pos.m_x < p1.m_x && _pos.m_x < p2x )
        {
            // point is to the left of the edge
        }
        else
        {
            f32 t = ( _pos.m_y - p1.m_y ) / dir.m_y;
            f32 xIntersection = p1.m_x + ( dir.m_x * t );

            if ( xIntersection > _pos.m_x )
            {
                // Point is to the left of the edge
            }
            else
            {
                colCount++;
            }
        }
    }

    return (colCount&1) == 1;
}

void  PolyPointList::setHoleAt(   u32 _index, PolyLineEdge::HoleMode _setHole  )
{
    m_edgeData[_index].m_holeMode = _setHole;
}

PolyLineEdge::HoleMode PolyPointList::getHoleAt(   u32 _index  ) const
{
    return m_edgeData[_index].m_holeMode;
}

void PolyPointList::setLoop( bbool _loop)
{
    if ( isLooping() != _loop )
    {
        m_quierredLoop = btrue;
    }
}

// forceLoop is only called by frieze collision polyline, ckeckLoop() won't be called in this case
void PolyPointList::forceLoop( bbool _loop )
{
    setLooping(_loop);
}

void PolyPointList::checkLoop( )
{
    if ( m_quierredLoop == bfalse || getPosCount() < 2 )
        return;

    m_quierredLoop = bfalse;
    setLooping(!isLooping());

    if ( isLooping() )
    {           
        // add first point
        // Need a copy of first pos because addPoint can do a push_back on edge that invalidate the ref
        Vec2d         posBegin   = getEdgeAt(0).getPos();
        const Vec2d & posEnd     = getEdgeAt(getPosCount()-1).getPos();
        if ( !posBegin.IsEqual(posEnd,MTH_EPSILON) )
        {
            addPoint( posBegin );
        }
    }
    else
    {
        if ( getPosCount() < 3 )
            return;
       
        // delete last point
        erasePosAt( getPosCount() -1 );
    }    
}


void PolyPointList::rotatePoint( u32 _index, f32 _deltaAngle, const Vec3d& _rotationCenter, f32 _z )
{
    Vec3d pos3D = getPosAt(_index).to3d(_z);
    Vec3d axeToRotate = pos3D - _rotationCenter;

    Matrix44 m;
    M44_setMatrixRotationZ(&m, _deltaAngle);
    
    Vec3d newAxe;
    M44_matrixVec3dTransform(&newAxe, &m, &axeToRotate);

    setPosAt((_rotationCenter + newAxe).truncateTo2D(), _index);
}


void PolyLine::setOwnerActor(Actor* _pActor)
{
    m_ownerActor = _pActor;
}


void PolyLine::Connection::reset()
{
    if ( m_previous )
    {   
        m_previous->m_connection.m_next = NULL;
        m_previous = NULL;
    }

    if ( m_next )
    {
        disconnectPoints( m_next );

        m_next->m_connection.m_previous = NULL;
        m_next = NULL;
    }
}

void PolyLine::Connection::disconnectPoints( PolyLine* _pol )
{
    if ( _pol->getPosCount() )
    {
        _pol->setPosAt(_pol->m_connection.m_posInit, 0);
        _pol->updateDataAt(0);
    }
}

void PolyLine::Connection::connectPoints( const Vec2d& _pos )
{  
    ITF_ASSERT( m_current->getPosCount() > 1 );

    m_posInit = m_current->getPosAt(0);

    m_current->setPosAt( _pos, 0 );
    m_current->updateDataAt(0);
}

void PolyLine::Connection::setPrevious_DontConnectPoints( PolyLine* _previous )
{
    m_previous = _previous;
    _previous->m_connection.m_next = m_current;
}

void PolyLine::Connection::setPrevious( PolyLine* _previous, const Vec2d& _pos )
{
    m_previous = _previous;
    _previous->m_connection.m_next = m_current;

    // collapse point
    connectPoints( _pos );
}

void PolyPointList::setPointScaleAt( u32 _index, f32 _scale ) 
{ 
 //   f32 scaleOld = getPointScaleAt( _index );

    m_edgeData[_index].setScale( _scale ); 

//     if ( scaleOld != getPointScaleAt( _index ) )
//         recomputeData();
}

void PolyPointList::resetAllPointsScale()
{
    u32 count = getPosCount();

    for ( u32 index = 0; index < count; index++)
        resetPointScaleAt( index);
}


void PolyPointList::switchTextureAt( u32 _index )
{
    m_edgeData[_index].m_switchTexture = !m_edgeData[_index].m_switchTexture;
}

void PolyLine::copyFrom( const PolyLine* _poly )
{
    m_pos = _poly->m_pos;
    m_edgeData = _poly->m_edgeData;
    m_absoluteAABB = _poly->m_absoluteAABB;
    m_relativeAABB = _poly->m_relativeAABB;

    if ( m_physBody )
    {
        m_physBody->setRecomputeAABB();
    }
}


void PolyLine::registerPhysic()
{
	ITF_ASSERT(!m_ownerActor);
    m_isCollisionActive = btrue;
    if ( m_physBody )
    {
        if ( !m_bodyRegistered )
        {
            PHYSWORLD->insertBody(m_physBody, getDepth());
            m_bodyRegistered = btrue;
        }
    }
}

void PolyLine::unregisterPhysic()
{
	ITF_ASSERT(!m_ownerActor);
    m_isCollisionActive = bfalse;
    if ( m_physBody )
    {
        if ( m_bodyRegistered )
        {
            PHYSWORLD->removeBody(m_physBody);
            m_bodyRegistered = bfalse;
        }
    }
}



void PolyLine::onAddedToScene( Scene* _pScene )
{
    recomputeData();

    // Create polyline's phys body
    // Only for polyline and not frieze by virtuality
    if(getObjectType() == BaseObject::ePolyLine)
    {
        PhysBodyInitCommonInfo bodyInit;
        SET_OWNER(bodyInit, getRef());
        bodyInit.m_static = btrue;
        bodyInit.m_pos = get2DPos();
        bodyInit.m_angle = getAngle();
        bodyInit.m_userData = getRef();
        bodyInit.m_weight = 1.f;
        bodyInit.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;
        bodyInit.m_collisionFilter = ECOLLISIONFILTER_CHARACTERS;

        ITF_ASSERT(m_physBody == NULL);
        m_physShape = newAlloc(mId_Phys,PhysShapePolyline(this));

        m_physBody = PHYSWORLD->allocBody(m_physShape,bodyInit);

        if (!m_ownerActor && _pScene->isActive())
        {
            PHYSWORLD->insertBody(m_physBody, getDepth());
            m_bodyRegistered = btrue;
        }
    }
}

void PolyLine::onRemovedFromScene( Scene* _pScene, bbool _forDeletion )
{
    if ( m_physBody && !m_ownerActor )
    {
        if ( m_bodyRegistered )
        {
            PHYSWORLD->removeBody(m_physBody);
            m_bodyRegistered = bfalse;
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
void PolyLine::onEditorCreated( Pickable* /*_original = NULL*/ )
{
    forceRecomputeData();
}

void PolyLine::onEditorMove(bbool _modifyInitialPos)
{
    unregisterPhysic();
    registerPhysic();

    forceRecomputeData();
}
#endif // ITF_SUPPORT_EDITOR

}
// namespace ITF

