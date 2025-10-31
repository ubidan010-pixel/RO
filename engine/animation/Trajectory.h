#ifndef _ITF_TRAJECTORY_H_
#define _ITF_TRAJECTORY_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

namespace ITF
{    
    class Scene;
    class Trajectory;
    class Trajectory_Template;
    class PlayTrajectory_evt;
    class PlayTrajectory_evtTemplate;

    class Trajectory : public Pickable
    {
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(Trajectory, Pickable,1495390743)

    public :
        Trajectory();
        ~Trajectory(){}

        void                        init(const Trajectory_Template *_template);
        const Trajectory_Template * getTemplate() { return m_template; }
        
        void                        setBindedEvent (PlayTrajectory_evt* _evt) {m_bindedEvent = _evt;}
        PlayTrajectory_evt*         getBindedEvent() {return m_bindedEvent;}

        bbool                       isSelectionIsLocked() { return m_currentSelectionIsLocked; }
        void                        setSelectionIsLocked(bbool _selectionIsLocked) { m_currentSelectionIsLocked = _selectionIsLocked; }

        void                        fillSplineInfo();
        Spline_Info *               getSplineInfo() { return &m_splineInfo; }

    private:
        const Trajectory_Template * m_template;
        PlayTrajectory_evt*         m_bindedEvent;
        bbool                       m_currentSelectionIsLocked;
        Spline_Info                 m_splineInfo;

    };


    class Trajectory_Template
    {
        DECLARE_SERIALIZE()
    public:
         Trajectory_Template();
        ~Trajectory_Template();

        enum TrajectoryDataType
        {
            Trajectory_LocalPosition = 0,
            Trajectory_Rotation,
            Trajectory_Scale,
            Trajectory_GlobalPosition
        };


        void                        addData          (TrajectoryDataType _type, const Vec3d& _data, const Vec3d& _center, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent = false);
        void                        addGlobalPosData (const Vec3d& _data, const Vec3d& _center, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent = false);
        void                        addLocalPosData  (const Vec3d& _data, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent = false);
        void                        addRotData       (const Vec3d& _data, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent = false);
        void                        addScaleData     (const Vec3d& _data, Spline::Interpolation _interp, f32 _time, bbool addOnlyInsideEvent = false);
       
                   u32              getCount      (TrajectoryDataType _type) const;
        ITF_INLINE u32              getPosCount   () const {return m_splinePos.GetNumPoints();}
        ITF_INLINE u32              getRotCount   () const {return m_splineRot.GetNumPoints();}
        ITF_INLINE u32              getScaleCount () const {return m_splineScale.GetNumPoints();}

        
        Vec3d                       getDataAt       (TrajectoryDataType _type, u32 _index, const Vec3d& _center) const;
        Vec3d                       getGlobalPosAt  (u32 _index, const Vec3d& _center) const;
        Vec3d                       getLocalPosAt   (u32 _index) const;
        ITF_INLINE const   Vec3d&   getRotAt        (u32 _index) const {return m_splineRot.GetNthPoint(_index);} 
        ITF_INLINE const   Vec3d&   getScaleAt      (u32 _index) const {return m_splineScale.GetNthPoint(_index);} 
        
                   f32              getDataTimeAt  (TrajectoryDataType _type, u32 _index) const;
        ITF_INLINE f32              getPosTimeAt   (u32 _index) const {return m_splinePos.getTimeAt(_index);} 
        ITF_INLINE f32              getRotTimeAt   (u32 _index) const {return m_splineRot.getTimeAt(_index);} 
        ITF_INLINE f32              getScaleTimeAt (u32 _index) const {return m_splineScale.getTimeAt(_index);} 
        
                   Spline::Interpolation getDataInterpAt    (TrajectoryDataType _type, u32 _index) const;
        ITF_INLINE Spline::Interpolation getPosInterpAt     (u32 _index) const {return m_splinePos.getInterpAt(_index);} 
        ITF_INLINE Spline::Interpolation getRotInterpAt     (u32 _index) const {return m_splineRot.getInterpAt(_index);} 
        ITF_INLINE Spline::Interpolation getScaleInterpAt   (u32 _index) const {return m_splineScale.getInterpAt(_index);} 

        void                        setDataAt            (TrajectoryDataType _type, const Vec3d& _pos, const Vec3d& _center, Spline::Interpolation _interp, f32 _time, u32 _index, bbool addOnlyInsideEvent = bfalse ); 
        void                        setGlobalPosDataAt   (const Vec3d& _pos, const Vec3d& _center, Spline::Interpolation _interp, f32 _time, u32 _index, bbool addOnlyInsideEvent = bfalse ); 
        void                        setLocalPosDataAt    (const Vec3d& _pos, Spline::Interpolation _interp, f32 _time, u32 _index, bbool addOnlyInsideEvent = bfalse ); 
        void                        setRotDataAt         (const Vec3d& _pos, Spline::Interpolation _interp, f32 _time, u32 _index, bbool addOnlyInsideEvent = bfalse ); 
        void                        setScaleDataAt       (const Vec3d& _pos, Spline::Interpolation _interp, f32 _time, u32 _index, bbool addOnlyInsideEvent = bfalse ); 

        ITF_INLINE void             setDataTimeAt   (TrajectoryDataType _type, f32 _time, u32 _index);
        ITF_INLINE void             setPosTimeAt    (f32 _time, u32 _index) { m_splinePos.setTimeAt(_time, _index); m_splinePos.sortByTime(); }         
        ITF_INLINE void             setRotTimeAt    (f32 _time, u32 _index) { m_splinePos.setTimeAt(_time, _index); m_splinePos.sortByTime(); }         
        ITF_INLINE void             setScaleTimeAt  (f32 _time, u32 _index) { m_splinePos.setTimeAt(_time, _index); m_splinePos.sortByTime(); }         

        void                        removeData      (TrajectoryDataType _type, const u32 _index);
        void                        removePosData   (const u32 _index)  { m_splinePos.RemoveSplinePoint(_index); }
        void                        removeRotData   (const u32 _index)  { m_splineRot.RemoveSplinePoint(_index); }
        void                        removeScaleData (const u32 _index)  { m_splineScale.RemoveSplinePoint(_index); }

        bbool                       extractData      (TrajectoryDataType _type, const u32 _indexBegin, const u32 _indexEnd, const f32 _timeDelta);
        bbool                       extractPosData   (const u32 _indexBegin, const u32 _indexEnd, const f32 _timeDelta)  { return m_splinePos.ExtractSplinePoint(_indexBegin, _indexEnd, _timeDelta); }
        bbool                       extractRotData   (const u32 _indexBegin, const u32 _indexEnd, const f32 _timeDelta)  { return m_splineRot.ExtractSplinePoint(_indexBegin, _indexEnd, _timeDelta); }
        bbool                       extractScaleData (const u32 _indexBegin, const u32 _indexEnd, const f32 _timeDelta)  { return m_splineScale.ExtractSplinePoint(_indexBegin, _indexEnd, _timeDelta); }

        void                        clearData          (TrajectoryDataType _type);
        void                        clearPosData       () {m_splinePos.clear(); }
        void                        clearRotData       () {m_splineRot.clear();}
        void                        clearScaleData     () {m_splineScale.clear();}

        bbool                       computeDataAtTime      (TrajectoryDataType _type, float _time, Vec3d& _dest, const Vec3d& _center) const;
        bbool                       computeGlobalPosAtTime (float _time, Vec3d& _dest, const Vec3d& _center) const;
        bbool                       computeLocalPosAtTime  (float _time, Vec3d& _dest) const;
        bbool                       computeRotAtTime       (float _time, Vec3d& _dest) const;
        bbool                       computeScaleAtTime     (float _time, Vec3d& _dest) const;

        void                        sortDataByTime(TrajectoryDataType _type);
        void                        sortPosByTime();
        void                        sortRotByTime();
        void                        sortScaleByTime();

        f32                         getMaxTime          () const;
        void                        clear               () {m_splinePos.clear(); m_splineRot.clear(); m_splineScale.clear();}
        Pickable*                   clone               (Scene* _pDstScene) const;
        
        void                        serializeToBlob     (Blob* _blob);
        void                        fillFromBlob        (Blob* _blob);
        void                        copyDataFrom(const Trajectory_Template* _from);

        bbool                       isEmpty();
            
        void                        update(f32 /*_deltaTime*/) {}
        void                        draw() {}

        void                        setBindedEventTemplate  (PlayTrajectory_evtTemplate* _evt) {m_bindedEventTemplate = _evt;}
        PlayTrajectory_evtTemplate* getBindedEventTemplate  (                                ) const {return m_bindedEventTemplate;}

        const Spline *              getSplinePos() const { return &m_splinePos; }

    private:
        Spline                      m_splinePos;
        Spline                      m_splineRot;
        Spline                      m_splineScale;

        PlayTrajectory_evtTemplate* m_bindedEventTemplate;
    };
} // namespace ITF



#endif //_ITF_TRAJECTORY_H_

