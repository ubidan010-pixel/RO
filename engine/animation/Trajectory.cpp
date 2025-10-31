#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_TRAJECTORY_H_
#include "engine/animation/trajectory.h"
#endif //_ITF_TRAJECTORY_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PLAYTRAJECTORY_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTrajectory_evt.h"
#endif //_ITF_PLAYTRAJECTORY_EVT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H


namespace ITF
{

BEGIN_SERIALIZATION_CHILD(Trajectory)
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(Trajectory)

BEGIN_SERIALIZATION(Trajectory_Template)

    SERIALIZE_OBJECT("translation",m_splinePos);
    SERIALIZE_OBJECT("rotation",m_splineRot);
    SERIALIZE_OBJECT("scale",m_splineScale);

END_SERIALIZATION()


    Trajectory::Trajectory()
    : m_template(NULL)
    , m_bindedEvent(NULL)
    , m_currentSelectionIsLocked(bfalse)
    {
         setObjectType(BaseObject::eTrajectory);
    }

    void Trajectory::init( const Trajectory_Template *_template )
    {
        m_template = _template;
    }


    Trajectory_Template::Trajectory_Template() : m_bindedEventTemplate(NULL)
    {
    }

    Trajectory_Template::~Trajectory_Template()
    {
    }


    bbool Trajectory_Template::isEmpty()
    {
        return m_splinePos.GetNumPoints() == 0 &&
               m_splineRot.GetNumPoints() == 0 &&
               m_splineScale.GetNumPoints() == 0;
    }


    Vec3d   Trajectory_Template::getGlobalPosAt(u32 _index, const Vec3d& _center) const
    {
        Vec3d res = m_splinePos.GetNthPoint(_index) + _center;
        return res;
    } 
    
    Vec3d Trajectory_Template::getLocalPosAt(u32 _index) const 
    {
        Vec3d res = m_splinePos.GetNthPoint(_index);
        return res;
    } 

    void    Trajectory_Template::addGlobalPosData( const Vec3d& _data, const Vec3d& _center, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent/* = bfalse*/ )
    {
        if(addOnlyInsideEvent)
        {
            PlayTrajectory_evtTemplate* evt = getBindedEventTemplate();
            if (evt && (_time < 0.f || _time > evt->getDuration()))
                return;
        }
        m_splinePos.AddSplinePoint(_data - _center, _interp, _time);
    }

    void    Trajectory_Template::addLocalPosData( const Vec3d& _data, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent/* = bfalse*/ )
    {
        if(addOnlyInsideEvent)
        {
            PlayTrajectory_evtTemplate* evt = getBindedEventTemplate();
            if (evt && (_time < 0.f || _time > evt->getDuration())) 
                return;
        }
        m_splinePos.AddSplinePoint(_data, _interp, _time);
    }

    void    Trajectory_Template::addRotData( const Vec3d& _data, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent/* = bfalse*/ )
    {
        if(addOnlyInsideEvent)
        {
            PlayTrajectory_evtTemplate* evt = getBindedEventTemplate();
            if (evt && (_time < 0.f || _time > evt->getDuration()))
                return;
        }
        m_splineRot.AddSplinePoint(_data, _interp, _time);
    }

    void    Trajectory_Template::addScaleData( const Vec3d& _data, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent/* = bfalse*/ )
    {
        if(addOnlyInsideEvent)
        {
            PlayTrajectory_evtTemplate* evt = getBindedEventTemplate();
            if (evt && (_time < 0.f || _time > evt->getDuration()))
            return;
        }
        m_splineScale.AddSplinePoint(_data, _interp, _time);
    }


    void    Trajectory_Template::setGlobalPosDataAt(const Vec3d& _pos, const Vec3d& _center, Spline::Interpolation _interp, float _time, u32 _index, bbool addOnlyInsideEvent/* = bfalse*/ )
    {
        if(addOnlyInsideEvent)
        {
            PlayTrajectory_evtTemplate* evt = getBindedEventTemplate();
            if (evt && (_time < 0.f || _time > evt->getDuration()))
                return;
        }
        m_splinePos.setSplinePointAt(_pos - _center, _interp, _time, _index);
    }

    void    Trajectory_Template::setLocalPosDataAt(const Vec3d& _pos, Spline::Interpolation _interp, float _time, u32 _index, bbool addOnlyInsideEvent/* = bfalse*/ )
    {
        if(addOnlyInsideEvent)
        {
            PlayTrajectory_evtTemplate* evt = getBindedEventTemplate();
            if (evt && (_time < 0.f || _time > evt->getDuration()))
                return;
        }
        m_splinePos.setSplinePointAt(_pos, _interp, _time, _index);
    }

    void    Trajectory_Template::setRotDataAt   (const Vec3d& _pos, Spline::Interpolation _interp, f32 _time, u32 _index, bbool addOnlyInsideEvent/* = bfalse*/ )
    {
        if(addOnlyInsideEvent)
        {
            PlayTrajectory_evtTemplate* evt = getBindedEventTemplate();
            if (evt && (_time < 0.f || _time > evt->getDuration()))
                return;
        }
        m_splineRot.setSplinePointAt(_pos, _interp, _time, _index);
    }

    void    Trajectory_Template::setScaleDataAt (const Vec3d& _pos, Spline::Interpolation _interp, f32 _time, u32 _index, bbool addOnlyInsideEvent/* = bfalse*/ )
    {
        if(addOnlyInsideEvent)
        {
            PlayTrajectory_evtTemplate* evt = getBindedEventTemplate();
            if (evt && (_time < 0.f || _time > evt->getDuration()))
                return;
        }
        m_splineScale.setSplinePointAt(_pos, _interp, _time, _index);
    }



    void Trajectory_Template::copyDataFrom(const Trajectory_Template* _from)
    {
        clear();

        for (u32 tydeIdx = 0; tydeIdx < 3; tydeIdx++)
        {
            TrajectoryDataType type = (TrajectoryDataType)tydeIdx;
            for (i32 i = 0; i < (i32)_from->getPosCount(); i++)
            {
                addData(type, _from->getDataAt(type, i, Vec3d::Zero), Vec3d::Zero, _from->getDataInterpAt(type, i), _from->getDataTimeAt(type, i));
            }
        }
    }

    Pickable* Trajectory_Template::clone(Scene* _pDstScene) const
    {
        return NULL;
    }

    bbool Trajectory_Template::computeGlobalPosAtTime (float _time, Vec3d& _dest, const Vec3d& _center) const
    {
        if (!getPosCount())
            return bfalse;
        bbool res = m_splinePos.GetInterpolatedAtTime(_time, _dest);
        _dest += _center;
        return res;
    }

    bbool Trajectory_Template::computeLocalPosAtTime (float _time, Vec3d& _dest) const
    {
        if (!getPosCount())
            return bfalse;
        return m_splinePos.GetInterpolatedAtTime(_time, _dest);
    }

    bbool Trajectory_Template::computeRotAtTime (float _time, Vec3d& _dest) const
    {
        if (!getRotCount())
            return bfalse;
        return m_splineRot.GetInterpolatedAtTime(_time, _dest);
    }

    bbool Trajectory_Template::computeScaleAtTime (float _time, Vec3d& _dest) const
    {
        if (!getScaleCount())
            return bfalse;
        return m_splineScale.GetInterpolatedAtTime(_time, _dest);
    }

    void Trajectory_Template::serializeToBlob (Blob* _blob)
    {
        m_splinePos.serializeToBlob(_blob);
        m_splineRot.serializeToBlob(_blob);
        m_splineScale.serializeToBlob(_blob);
    }

    void Trajectory_Template::fillFromBlob (Blob* _blob)
    {
        m_splinePos.fillFromBlob(_blob);
        m_splineRot.fillFromBlob(_blob);
        m_splineScale.fillFromBlob(_blob);
    }

    f32 Trajectory_Template::getMaxTime   () const
    {
        f32 lmax, lmax2, lmax3;
        if (m_splinePos.GetNumPoints())
            lmax = m_splinePos.getTimeAt(m_splinePos.GetNumPoints()-1);
        else
            lmax = 0.f;

        if (m_splineRot.GetNumPoints())
            lmax2 = m_splineRot.getTimeAt(m_splineRot.GetNumPoints()-1);
        else
            lmax2 = 0.f;

        if (m_splineScale.GetNumPoints())
            lmax3 = m_splineScale.getTimeAt(m_splineScale.GetNumPoints()-1);
        else
            lmax3 = 0.f;

        if ((lmax >= lmax2) && (lmax >= lmax3))
            return lmax;
        if ((lmax2 >= lmax) && (lmax2 >= lmax3))
            return lmax2;
        if ((lmax3 >= lmax) && (lmax3 >= lmax2))
            return lmax3;
        return lmax;
    }

    void Trajectory_Template::sortPosByTime()
    {
        m_splinePos.sortByTime();
    }

    void Trajectory_Template::sortRotByTime()
    {
        m_splineRot.sortByTime();
    }

    void Trajectory_Template::sortScaleByTime()
    {
        m_splineScale.sortByTime();
    }

    void Trajectory::fillSplineInfo()
    {
        m_splineInfo.m_spline   = *getTemplate()->getSplinePos();
        m_splineInfo.m_pos = getPos();
    }

    
    ////////////////////////////////////////////////////////
    /// TRAJECTORY TYPE FUNCTIONS
    ////////////////////////////////////////////////////////

    void Trajectory_Template::addData( TrajectoryDataType _type, const Vec3d& _data, const Vec3d& _center, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent /*= false*/ )
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            addLocalPosData( _data, _interp, _time, addOnlyInsideEvent);
            break;
        case Trajectory_Rotation:
            addRotData( _data, _interp, _time, addOnlyInsideEvent);
            break;
        case Trajectory_Scale:
            addScaleData( _data, _interp, _time, addOnlyInsideEvent);
            break;
        case Trajectory_GlobalPosition:
            addGlobalPosData( _data, _center, _interp, _time, addOnlyInsideEvent);
            break;
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
    }

    u32 Trajectory_Template::getCount( TrajectoryDataType _type ) const
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            return getPosCount();
        case Trajectory_Rotation:
            return getRotCount();
        case Trajectory_Scale:
            return getScaleCount();
        case Trajectory_GlobalPosition:
            return getPosCount();
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
        return 0;
    }

    ITF::Vec3d Trajectory_Template::getDataAt( TrajectoryDataType _type, u32 _index, const Vec3d& _center ) const
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            return getLocalPosAt(_index);
        case Trajectory_Rotation:
            return getRotAt(_index);
        case Trajectory_Scale:
            return getScaleAt(_index);
        case Trajectory_GlobalPosition:
            return getGlobalPosAt(_index, _center);
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
        return Vec3d::Zero;
    }

    f32 Trajectory_Template::getDataTimeAt( TrajectoryDataType _type, u32 _index ) const
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            return getPosTimeAt(_index);
        case Trajectory_Rotation:
            return getRotTimeAt(_index);
        case Trajectory_Scale:
            return getScaleTimeAt(_index);
        case Trajectory_GlobalPosition:
            return getPosTimeAt(_index);
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
        return 0.f;
    }

    Spline::Interpolation Trajectory_Template::getDataInterpAt( TrajectoryDataType _type, u32 _index ) const
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            return getPosInterpAt(_index);
        case Trajectory_Rotation:
            return getRotInterpAt(_index);
        case Trajectory_Scale:
            return getScaleInterpAt(_index);
        case Trajectory_GlobalPosition:
            return getPosInterpAt(_index);
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
        return Spline::interp_linear;
    }

    void Trajectory_Template::setDataAt( TrajectoryDataType _type, const Vec3d& _pos, const Vec3d& _center, Spline::Interpolation _interp, f32 _time, u32 _index, bbool addOnlyInsideEvent /*= bfalse */ )
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            setLocalPosDataAt(_pos, _interp, _time, _index, addOnlyInsideEvent);
            break;
        case Trajectory_Rotation:
            setRotDataAt(_pos, _interp, _time, _index, addOnlyInsideEvent);
            break;
        case Trajectory_Scale:
            setScaleDataAt(_pos, _interp, _time, _index, addOnlyInsideEvent);
            break;
        case Trajectory_GlobalPosition:
            setGlobalPosDataAt(_pos, _center, _interp, _time, _index, addOnlyInsideEvent);
            break;
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
    }

    ITF_INLINE void Trajectory_Template::setDataTimeAt( TrajectoryDataType _type, f32 _time, u32 _index )
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            setPosTimeAt(_time, _index);
            break;
        case Trajectory_Rotation:
            setRotTimeAt(_time, _index);
            break;
        case Trajectory_Scale:
            setScaleTimeAt(_time, _index);
            break;
        case Trajectory_GlobalPosition:
            setPosTimeAt(_time, _index);
            break;
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
    }

    void Trajectory_Template::removeData( TrajectoryDataType _type, const u32 _index )
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            removePosData(_index);
            break;
        case Trajectory_Rotation:
            removeRotData(_index);
            break;
        case Trajectory_Scale:
            removeScaleData(_index);
            break;
        case Trajectory_GlobalPosition:
            removePosData(_index);
            break;
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
    }

    ITF::bbool Trajectory_Template::extractData( TrajectoryDataType _type, const u32 _indexBegin, const u32 _indexEnd, const f32 _timeDelta )
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            return extractPosData(_indexBegin, _indexEnd, _timeDelta);
        case Trajectory_Rotation:
            return extractRotData(_indexBegin, _indexEnd, _timeDelta);
        case Trajectory_Scale:
            return extractScaleData(_indexBegin, _indexEnd, _timeDelta);
        case Trajectory_GlobalPosition:
            return extractPosData(_indexBegin, _indexEnd, _timeDelta);
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
        return bfalse;
    }

    void Trajectory_Template::clearData( TrajectoryDataType _type )
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            clearPosData();
            break;
        case Trajectory_Rotation:
            clearRotData();
            break;
        case Trajectory_Scale:
            clearScaleData();
            break;
        case Trajectory_GlobalPosition:
            clearPosData();
            break;
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
    }

    ITF::bbool Trajectory_Template::computeDataAtTime( TrajectoryDataType _type, float _time, Vec3d& _dest, const Vec3d& _center ) const
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            return computeLocalPosAtTime( _time, _dest);
        case Trajectory_Rotation:
            return computeRotAtTime( _time, _dest);
        case Trajectory_Scale:
            return computeScaleAtTime( _time, _dest);
        case Trajectory_GlobalPosition:
            return computeGlobalPosAtTime( _time, _dest, _center);
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
        return bfalse;
    }

    void Trajectory_Template::sortDataByTime( TrajectoryDataType _type )
    {
        switch (_type)
        {
        case Trajectory_LocalPosition:
            sortPosByTime();
            break;
        case Trajectory_Rotation:
            sortRotByTime();
            break;
        case Trajectory_Scale:
            sortScaleByTime();
            break;
        case Trajectory_GlobalPosition:
            sortPosByTime();
            break;
        default:
            {
                ITF_ASSERT_MSG(0, "Bad trajectory data type used !");
            }
            break;
        }
    }
} // namespace ITF


