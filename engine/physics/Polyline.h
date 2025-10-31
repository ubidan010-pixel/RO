///////////////////////////////////////////////////////////////////////////////
///////////////////////       polyline class definition
///////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_POLYLINE_H_
#define _ITF_POLYLINE_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_OBJBINDING_H_
#include "engine/actors/ObjBinding.h"
#endif //_ITF_OBJBINDING_H_

namespace ITF
{

class Actor;
class Frise;

class PolyLineEdge
{
public:
    DECLARE_SERIALIZE()

    enum HoleMode
    {
        Hole_None = 0,
        Hole_Collision = 1,
        Hole_Visual = 2,
        Hole_Both = 3,
         ENUM_FORCE_SIZE_32(HoleMode)
    };

    
    PolyLineEdge    (                       )
    {
        m_pos = Vec2d::Zero;
        m_vector = Vec2d::Zero;
        m_normalizedVector = Vec2d::Zero;
        m_length = 0.f;
        m_holeMode = Hole_None;
        m_scale = 1.f;
        m_switchTexture = bfalse;
    };

    PolyLineEdge    (   const Vec2d& _pos   )
    {
        m_vector = Vec2d::Zero;
        m_normalizedVector = Vec2d::Zero;
        m_pos = _pos;
        m_length = 0.f;
        m_holeMode = Hole_None;
        m_scale = 1.f;
        m_switchTexture = bfalse;
    }

    ITF_INLINE  const Vec2d&    getPos  (   ) const {return m_pos;}
    ITF_INLINE  void            setPos  (   const Vec2d& _pos   ) {m_pos = _pos;}
    ITF_INLINE  void            setGameMaterial (   const StringID& _id   ) {m_gameMaterial = _id;}
    ITF_INLINE  const StringID& getGameMaterial (   ) const  {return m_gameMaterial;}
    ITF_INLINE  f32             getScale  (   ) const { return m_scale; }
    ITF_INLINE  void            setScale  ( f32 _scale ) { m_scale = Clamp( _scale, 0.5f, 1.5f ); }
    ITF_INLINE  f32             getLength (   ) const { return m_length; }

                Vec2d           m_vector;               // edge vector such as: second vertex = m_pos + m_vector
                Vec2d           m_normalizedVector;     // m_vector normalized
                float           m_length;               // length of m_vector
                HoleMode        m_holeMode;
                bbool           m_switchTexture;

private:
                Vec2d           m_pos;
                StringID        m_gameMaterial;
                f32             m_scale;
};
VECTAGSET(PolyLineEdge, ITF::MemoryId::mId_Polyline)

typedef ITF_VECTOR<PolyLineEdge> PolyLineEdgeList;  // TODO: maybe make this a SafeArray?

class PolyPointList
{
public:
    ~PolyPointList();
    ITF_INLINE const PolyLineEdgeList& getEdgeData() const {return m_edgeData;} // MUST remain const, so that the caller masters when he should call recomputedata()
    ITF_INLINE PolyLineEdgeList& getMutableEdgeData() {return m_edgeData;}      // RESERVED FOR SERIALIZATION (and reserve/resize)
    ITF_INLINE u32      getPosCount             (               ) const     {   return (u32)m_edgeData.size();    }
    ITF_INLINE u32      getVectorsCount         (               ) const     {   return (getPosCount() == 0 ? 0 : getPosCount()-1);    }
    ITF_INLINE const PolyLineEdge&  getEdgeAt   (   u32 _index  ) const     {   return m_edgeData[_index];  }
    void                transferEdgeProperties  (   u32 _index, const PolyLineEdge& _edg  );
    ITF_INLINE const Vec2d&  getPosAt           (   u32 _index  ) const     {   return m_edgeData[_index].getPos();  }
    f32                 getPointScaleAt         (   u32 _index  ) const {   return m_edgeData[_index].getScale();  }
    ITF_INLINE bbool    isLooping               () const     {   return m_loop;  }
    ITF_INLINE const AABB& getAABB() const {   return m_aabb;  }
    PolyLineEdge::HoleMode getHoleAt            (   u32 _index  ) const;
    Vec2d               getCenter               ();
    void                setLoop                 (   bbool _loop);
    f32                 getLength               () const { return m_length; }

private:
    friend class PolyLine;
    friend class Frise;

    PolyPointList()
    {
        m_quierredLoop = bfalse;
        m_length = 0.f;
        setLooping(bfalse);
    }
    void                setPointScaleAt         (   u32 _index, f32 _scale );
    void                resetPointScaleAt       (   u32 _index ) { setPointScaleAt( _index, 1.f ); }
    void                resetAllPointsScale     (   );
    void                addPoint                (   const Vec2d& _pos, i32 _at = -1   );
    bbool               erasePosAt              (   u32 _index );
    void                setPosAt                (   const Vec2d& _pos, u32 _i  );
    ITF_INLINE void     setLooping              ( bbool _looping ) {  m_loop = _looping;  }
    ITF_INLINE bbool    isLooping               () {  return m_loop;  }
    void                updateDataAt            (   u32 _index );
    void                executeRecomputeData    ();
    void                setGameMaterialAt       ( const StringID& _id, i32 _at);
    void                setGameMaterial         ( const StringID& _id);
    void                clear                   () {m_edgeData.clear();}
    void                setHoleAt               (   u32 _index, PolyLineEdge::HoleMode _setHole     );
    void                switchTextureAt         (   u32 _index);
    void                rotatePoint(u32 _index, f32 _deltaAngle, const Vec3d& _rotationCenter, f32 _z);
    void                checkLoop( );
    void                forceLoop( bbool _loop );
    void                addEdge                 ( const PolyLineEdge& _edge);
private:
    PolyLineEdgeList    m_edgeData;
    bbool               m_loop;
    AABB                m_aabb;
    bbool               m_quierredLoop;
    f32                 m_length;
};
VECTAGSET(PolyPointList, ITF::MemoryId::mId_Polyline)

class PolyLine : public BaseObject
{
    friend class Frise;

    DECLARE_OBJECT_CHILD_RTTI(PolyLine, BaseObject,1932163747);
    DECLARE_SERIALIZE()

    class Connection
    {
    public:
        Connection()
            : m_previous(NULL)
            , m_next(NULL)
            , m_current(NULL)
            , m_posInit(Vec2d::Zero)
        {
        }
        
   private:
        PolyLine*           m_previous;
        PolyLine*           m_next;
        PolyLine*           m_current;
        Vec2d               m_posInit;

    public:
        void                connectPoints( const Vec2d& _pos );
        void                disconnectPoints( PolyLine* _pol );
        void                reset();
        void                setPrevious_DontConnectPoints( PolyLine* _previous );
        void                setPrevious( PolyLine* _previous, const Vec2d& _pos );
        void                setCurrent(PolyLine* _pol){ m_current = _pol;}
        PolyLine*           getPrevious() const { return m_previous; }
        PolyLine*           getNext() const { return m_next; }        
    };

private:
    PolyPointList               m_edgeData;
    ObjBinding                  m_binding;    
  
    void                        changeEdgeDataToRelative();
    void                        changeEdgeDataToAbsolute();
    void                        startLoadPolyLine();
    void                        endLoadPolyLine();
    void                        startSavePolyLine();
    void                        endSavePolyLine();
    void                        postload();
    bbool                       m_querriedRecomputeData;
    void                        requestDestruction();
    void                        checkLoop ( ) {m_edgeData.checkLoop();}    

    bbool                       isPointInside_up(const Vec2d& _pos) const;
    bbool                       isPointInside_down(const Vec2d& _pos) const;
    bbool                       isPointInside_left(const Vec2d& _pos) const;
    bbool                       isPointInside_right(const Vec2d& _pos) const;
    Actor*                      m_ownerActor;
    Vec3d                       m_pos;
    f32                         m_angle;
    bbool                       m_isSerializable;

protected:
    virtual void                executeRecomputeData (  );
    bbool                       m_bHoldsFrise;

public:
    void                        setOwnerActor(Actor* _pActor);
    ITF_INLINE Actor*           getOwnerActor() const {return m_ownerActor;}
    void                        onLoaded();
    bbool                       isHoldsFrieze() const { return m_bHoldsFrise; }
    ITF_INLINE const PolyPointList& getEdgeData() const {return m_edgeData;}
    void                        offsetData(const Vec2d& _offset);
    f32                         getLength ( ) const { return m_edgeData.getLength(); }

#ifdef ITF_SUPPORT_EDITOR
    void                        onEditorMove(bbool _modifyInitialPos = btrue);
    void                        onEditorCreated( Pickable* _original = NULL);
#endif // ITF_SUPPORT_EDITOR

    ///////////////////////////////////////////////////////////////////////////
    /** Bounding Box section */
    ///////////////////////////////////////////////////////////////////////////
public:

    const AABB&        getRelativeAABB             (   ) const {return m_relativeAABB;}
    void            changeRelativeAABB             (   const AABB& _relativeAabb ){ ITF_ASSERT(_relativeAabb.isValid()); m_relativeAABB = _relativeAabb;}
    /**
    * Compute absolute AABB and return it.<br>
    * <b>Warning : this is not just an accessor, it actually computes
    * the world coordinates AABB from te local coordinates AABB whenever this method is called.</b><br>
    * @return the world coord AABB
    */
    ITF_INLINE  const AABB&     getAABB ( ) const
    {
        updateAbsoluteAABBFromRelative();
        return m_absoluteAABB;
    }

    /**
    * Refreshes the absolute (world coord) AABB from the relative one
    * This method is const despite the fact that it updates a member. It's because m_relativeAABB is mutalbe.
    * This is evil, and should be changed.    
    */
    ITF_INLINE void             updateAbsoluteAABBFromRelative() const
    {
        const AABB &aabb = getRelativeAABB();
        m_absoluteAABB.setMin( aabb.getMin() + get2DPos() );
        m_absoluteAABB.setMax( aabb.getMax() + get2DPos() );
    }

    /**
    * Set Relative AABB From Absolute AABB. NB : position must be valid
    * @param _absolute the new AABB. The relative aabb will be computed using current position
    */
    void                        changeRelativeAABBFromAbsolute(const AABB &_absolute) {changeRelativeAABB(AABB(_absolute.getMin()-get2DPos(), _absolute.getMax()-get2DPos()));}

    AABB                    m_relativeAABB;
    mutable AABB            m_absoluteAABB; //m_absoluteAABB is only a temporary variable used by getAABB()

    ///////////////////////////////////////////////////////////////////////////
    /** End of Bounding Box section */
    ///////////////////////////////////////////////////////////////////////////

    ITF_INLINE  const Vec3d& getPos() const {return m_pos; }
    void               setPos                      ( const Vec3d& _pos )       { m_pos.m_x = _pos.m_x; m_pos.m_y = _pos.m_y; setDepth(_pos.m_z); }
    
    void                    setSerializable(bbool _b)       { m_isSerializable = _b; }
    bbool                   isSerializable()const           { return m_isSerializable; }
    
    ITF_INLINE  void        set2DPos                    ( const Vec2d& _pos )       { m_pos.m_x = _pos.m_x; m_pos.m_y = _pos.m_y;}
    ITF_INLINE  const Vec2d&    get2DPos                    (   ) const {return m_pos.truncateTo2D(); }
    virtual  void               setAngle(f32 _angle) { m_angle = _angle;    }
    ITF_INLINE  f32             getAngle() const  { return m_angle;}

    Frise*              m_ownerFrise;
    class PhysBody*     m_physBody;
    class PhysShape*    m_physShape;
    f32                 m_speedLoss;
    bbool               m_isCollisionActive;
    Connection          m_connection;
    bbool               m_bodyRegistered;

    PolyLine            ();
    virtual             ~PolyLine           ();
    virtual void        clear();
    ITF_INLINE void     reserve                 ( const u32 _size )         { m_edgeData.getMutableEdgeData().reserve(_size); }
    ITF_INLINE void     resize                  ( const u32 _size )         { m_edgeData.getMutableEdgeData().resize(_size); recomputeData(); }
    ITF_INLINE void     addPoint                (   const Vec2d& _pos, i32 _at = -1   ) {m_edgeData.addPoint(_pos, _at); recomputeData();}
    ITF_INLINE u32      getPosCount             (               ) const     {   return m_edgeData.getPosCount();    }
    ITF_INLINE u32      getVectorsCount         (               ) const     {   return m_edgeData.getVectorsCount();  }
    ITF_INLINE const PolyLineEdge&  getEdgeAt   (   u32 _index  ) const     {   return m_edgeData.getEdgeAt(_index);  }
    ITF_INLINE void     transferEdgeProperties  (   u32 _index, const PolyLineEdge& _edg  ) {return m_edgeData.transferEdgeProperties(_index, _edg);}
    ITF_INLINE const Vec2d&  getPosAt           (   u32 _index  ) const     {   return m_edgeData.getPosAt(_index);  }
    ITF_INLINE f32      getDepth                () const     {   return m_pos.m_z;  }
    ITF_INLINE Vec3d    get3DPosAt              (   u32 _index  ) const     {   Vec3d v = getPosAt(_index).to3d(); v.m_z = getDepth(); return v;  }
    ITF_INLINE void     setHoleAt               (   u32 _index, PolyLineEdge::HoleMode _setHole     ) {m_edgeData.setHoleAt(_index, _setHole);}
    ITF_INLINE PolyLineEdge::HoleMode  getHoleAt           (   u32 _index  ) const {return m_edgeData.getHoleAt(_index);}
    ITF_INLINE void switchTextureAt         (   u32 _index) {m_edgeData.switchTextureAt(_index);}


            void        setDepth                ( f32 _depth );
            void        update                  (   f32 _ellasped );
            void        onAddedToScene(Scene* _pScene);    
            void        onRemovedFromScene(Scene* _pScene,bbool _forDeletion);

    void                registerPhysic();
    void                unregisterPhysic();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Erase a point of the polyline, if the last point is removed then the polyline is removed from the scene
    /// @param _index the point index
    /// @return btrue if the polyline is unregistered from the scene
    bbool               erasePosAt              (   u32 _index );
    ITF_INLINE void     setPosAt                (   const Vec2d& _pos, u32 _i  ) {m_edgeData.setPosAt(_pos, _i); recomputeData();}
    void                recomputeData           (                       );
    virtual void        forceRecomputeData      (                       );
    ITF_INLINE void     updateDataAt            (   u32 _index ) {m_edgeData.updateDataAt(_index);} // recomputeData() not needed
    Vec2d               getAnchorPosAtZ         ( float _z);

    ObjBinding*         getBinding() { return &m_binding; }
    const ObjBinding*   getBinding() const { return &m_binding; }
    ITF_INLINE void     setGameMaterialAt       ( const StringID& _id, i32 _at) {m_edgeData.setGameMaterialAt(_id, _at);}
    ITF_INLINE void     setGameMaterial         ( const StringID& _id)  {m_edgeData.setGameMaterial(_id);}
    bbool               isPointInside(const Vec2d& _pos) const;
    void                setLoop                 (   bbool _loop) {m_edgeData.setLoop(_loop);}
    void                forceLoop               (   bbool _loop ) {m_edgeData.forceLoop(_loop);}
    bbool               isLooping               ( ) const { return m_edgeData.isLooping();}
    PolyLine*           getPrevious             ( ) const { return m_connection.getPrevious(); }
    PolyLine*           getNext                 ( ) const { return m_connection.getNext(); }
    ITF_INLINE f32      getPointScaleAt         (   u32 _index  ) const {return m_edgeData.getPointScaleAt(_index);  }
    ITF_INLINE void     setPointScaleAt         (   u32 _index, f32 _scale ) {m_edgeData.setPointScaleAt(_index, _scale);}
    ITF_INLINE void     resetPointScaleAt       (   u32 _index ) { m_edgeData.resetPointScaleAt(_index); }
    ITF_INLINE void     resetAllPointsScale     (   ) {m_edgeData.resetAllPointsScale();}

    void                copyFrom                ( const class PolyLine* _poly );
};

ITF_INLINE void PolyLine::recomputeData()
{
    m_querriedRecomputeData = btrue;
}

ITF_INLINE void PolyLine::forceRecomputeData()
{
    m_querriedRecomputeData = btrue;
    executeRecomputeData();
}

ITF_INLINE void PolyLine::update( f32 _ellasped )
{
    if (m_querriedRecomputeData)
        executeRecomputeData();

    m_binding.updateChildren(_ellasped);
}

} //namespace ITF


#endif //_ITF_POLYLINE_H_
