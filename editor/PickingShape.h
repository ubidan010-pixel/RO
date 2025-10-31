#ifndef _ITF_PICKINGSHAPE_H_
#define _ITF_PICKINGSHAPE_H_


#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

namespace ITF
{
    class   PolyLineEdge;
    struct  PickingInfo2D;
    class   Plugin;
    class   PickingShape;

    //////////////////////////////////////////////////////////////////////////
    /// Base class for storing data for a given plugin type
    class PickingShapePluginData
    {
    public:
        PickingShapePluginData(PickingShape *_pick, Plugin *_plugin) : m_pickingShape(_pick), m_plugin(_plugin) {}
        virtual ~PickingShapePluginData() {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// update
        virtual void        update() {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// draw
        virtual void        draw() {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse Pressed
        /// @param _mousePos position of mouse
        virtual void        handleMousePressed(const Vec2d &/*_mousePos*/) {} //NB : in this function, CAMERA is supposed to be applied

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse move
        /// @param _mousePos position of mouse ( may be stationary )
        virtual void        handleMouseMove(const Vec2d &/*_mousePos*/) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse Released
        /// @param _mousePos
        virtual void        handleMouseReleased(const Vec2d &/*_mousePos*/) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse wheel
        /// @param _wheelDelta
        virtual void        handleMouseWheel(i32 /*_wheelDelta*/) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called when rotating the shape around the Z axis
        /// @param _deltaAngle the current mouse psotion in screen space
        /// @param _pRotationCenter the rotation center, NULL means local rotation
        virtual void        rotateAroundAxisZ(f32 /*_deltaAngle*/, const Vec3d* /*_pRotationCenter*/ = NULL) {}
        
    protected:

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called when the helpers are used to scale the object
        virtual void        scale(const Vec2d& _newScale) {}

        PickingShape*       m_pickingShape;
        Plugin*             m_plugin;
    };

    class Interface_PickingShapePlugin
    {
        public:
            Interface_PickingShapePlugin()            {}
            virtual ~Interface_PickingShapePlugin()   {}

            // TODO add generic stuff here and remove them from Interface_ActorEdListener
    };

    //////////////////////////////////////////////////////////////////////////
    ///Base class for picking shapes (anchors, ...)
    class PickingShape
    {
    public:
        enum ShapeType
        {
            ShapeType_Disc,
            ShapeType_Edge,
            ShapeType_Invalid,
            ENUM_FORCE_SIZE_32(0)
        };
        static const u32    m_UnselectablePriority = 100;

        //////////////////////////////////////////////////////////////////////////

                            PickingShape(Interface_PickingShapePlugin* _creatorSpecific);
        virtual             ~PickingShape();

        const ObjectRef&    getOwner() const;
        virtual BaseObject* getOwnerPtr() const;
        u32                 getData() const                                     { return m_data; }
        void                setData(u32 _data)                                  { m_data = _data; }
        void                setOwner(const ObjectRef _owner);
        void                setSubAnchor(bbool _isSubAnchor)                    { m_isSubAnchor = _isSubAnchor; }
        bbool               isSubAnchor() const                                 { return m_isSubAnchor; }
        const u32           getlastUpdateFrame() const;
        void                setlastUpdateFrame(const u32 _frame);
        virtual void        fillPickingInfo(PickingInfo2D* _pPicking2D) = 0;
        PickingInfo2D*      getPickingInfoPtr()                                 { return &m_lastPickingInfo; }

        u32                 getPriority()const                                  { return m_priority; }
        void                setPriority(u32 _prio)                              { m_priority = _prio; }
        virtual bool        hasPriorityOver(const PickingShape* _pOther)const   { return getPriority() < _pOther->getPriority(); }

        static  bool        compareShapes(PickingShape* _p1, PickingShape* _p2) { return _p2->hasPriorityOver(_p1); }

        bbool               canSnapWithMainShape()const                         { return m_snapWithMainAnchor; }
        void                setSnapWithMainShape( bbool _b)                     { m_snapWithMainAnchor = _b; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Test if the shape is under the given 2D position
        /// @param _point The 2D position
        /// @param _intersection The 3D intersection at the shape depth
        /// @return if test is succefull so _intersection can be used
        virtual bbool       isUnderPoint2D(const Vec2d &_point, Vec3d &_intersection) { return bfalse; }

        //////////////////////////////////////////////////////////////////////////
        /// Call every plugin to allocate custom data if needed
        void allocatePluginData();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Update status every frame. done before draw. . Default code calls each plugin's PickingShapePluginData  update
        virtual void update();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// draw pickingshape-specific representation. Default code calls each plugin's PickingShapePluginData draw
        virtual void draw();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse Pressed
        /// @param _mousePos position of mouse
        virtual void        handleMousePressed(const Vec2d &_mousePos); //NB : in this function, CAMERA is supposed to be applied

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse move
        /// @param _mousePos position of mouse ( may be stationary )
        virtual void        handleMouseMove(const Vec2d &_mousePos);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse Released
        /// @param _mousePos
        virtual void        handleMouseReleased(const Vec2d &_mousePos);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse wheel
        /// @param _wheelDelta
        virtual void        handleMouseWheel(i32 _wheelDelta);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called when the mouse is used to drag the shape
        /// @param _mousePos the current mouse position in screen space
        virtual void        dragByMouse(const Vec2d &/*_mousePos*/) {};

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called when the mouse is used to rotate the shape around the Z axis
        /// @param _deltaAngle the current mouse psotion in screen space
        /// @param _pRotationCenter the rotation center, NULL means local rotation
        virtual void        rotateAroundAxisZ(f32 /*_deltaAngle*/, const Vec3d* /*_pRotationCenter*/ = NULL);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get plugin data for given plugin, or null
        /// @param _plugin the plugin
        PickingShapePluginData *getPluginData(Plugin *_plugin);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get plugin interface data for given plugin, or null
        Interface_PickingShapePlugin*   getCreatorPickingShapePlugin() const    { return m_creatorPickingShapePlugin; }

        virtual Vec3d getCenter() = 0;

        const ShapeType     getShapeType() const {return m_shapeType;}
        static const f32    m_disc_Radius;
        static const f32    m_disc_sqRadius;
        static const f32    m_disc_picking_tolerance;


    protected:
        bbool               initPicking(PickingInfo2D* _pPicking2D);
        PickingInfo2D       m_lastPickingInfo;
        ShapeType           m_shapeType;
        u32                 m_data;

    private:
        bbool               m_isSubAnchor;
        ObjectRef           m_owner;
        u32                 m_lastUpdateFrame;
        u32                 m_priority; // 0 mean highest priority
        bbool               m_snapWithMainAnchor;
        Interface_PickingShapePlugin* m_creatorPickingShapePlugin;
        ITF_MAP<Plugin*, PickingShapePluginData*> m_PluginData;
    };

    class PickingShape_Disc : public PickingShape
    {
    public:

        enum PosType
        {
            Common,             // set initPos = currentPos = mousePos
            InitPos,            // set initPos = mousePos, offset currentPos
            InitPosFromCurrent  // set currentPos = mousePos, offset initPos
        };

                            PickingShape_Disc(Interface_PickingShapePlugin* _creatorSpecific = NULL);
        virtual void        fillPickingInfo(PickingInfo2D* _pPicking2D);
        void                setPos(const Vec3d& _pos)                       { m_pos = _pos; }
        const Vec3d&        getPos() const                                  { return m_pos;}
        void                setAngle(f32 _angle)                            { m_angle = _angle; }
        f32                 getAngle() const                                { return m_angle;}
        ITF_INLINE PosType  getPosType() const                              { return m_posType; }
        ITF_INLINE void     setPosType( PosType _value )                    { m_posType = _value; }

        virtual bool        hasPriorityOver(const PickingShape* _pOther)const;
        virtual bbool       isUnderPoint2D(const Vec2d &_point, Vec3d &_intersection);
        virtual void        dragByMouse(const Vec2d & _mousePos);
        virtual Vec3d       getCenter() {return m_pos;}
        virtual void        update() {PickingShape::update();}
        virtual void        draw() {PickingShape::draw();}
    
    private:
        Vec3d               m_pos;
        f32                 m_angle;
        PosType             m_posType;
    };

    class PickingShape_Edge : public PickingShape
    {
    public:
                            PickingShape_Edge(Interface_PickingShapePlugin* _creatorSpecific = NULL);
        virtual void        fillPickingInfo(PickingInfo2D* _pPicking2D);

        void                setPt1(const Vec2d& _pt);
        void                setPt2(const Vec2d& _pt);
        void                setPts(const Vec2d& _pt1, const Vec2d& _pt2);
        void                setPts(const PolyLineEdge& edge, f32 _z);
        void                setDepth(f32 _z);
        const Vec2d&        getPt1() const;
        const Vec2d&        getPt2() const;
        const f32           getDepth();
        virtual void        dragByMouse(const Vec2d & _mousePos);
        virtual Vec3d       getCenter() {return Vec3d((m_pt1.m_x + m_pt2.m_x) * 0.5f, (m_pt1.m_y + m_pt2.m_y) * 0.5f, m_z);}

        virtual bbool       isUnderPoint2D(const Vec2d &_point, Vec3d &_intersection);

    private:
        Vec2d               m_pt1, m_pt2;
        f32                 m_z;
    };
} // namespace ITF

#endif // _ITF_PICKINGSHAPE_H_