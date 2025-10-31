#ifndef _ITF_PROCEDURALPOLYLINE_H_
#define _ITF_PROCEDURALPOLYLINE_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{
    class EventStickOnPolyline;
    class EventHanging;

    class ProceduralPolyline
    {
    public:

        ProceduralPolyline();
        virtual ~ProceduralPolyline();

        void                init( Actor* _owner, IEventListener* _eventListener, bbool _usePhantom = bfalse );
        void                activate();
        void                deactivate();
        void                setPoints( const SafeArray<Vec2d>& _points );
        void                update( f32 _dt );
        void                onEvent( Event* _event );
#ifdef ITF_SUPPORT_EDITOR
        void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE PolyLine& getPolyline() { ITF_ASSERT(m_polyline); return *m_polyline; }
        ITF_INLINE const PolyLine& getPolyline() const { ITF_ASSERT(m_polyline); return *m_polyline; }
        const ObjectRefList& getObjectsOnPolyline() const { return m_objectsOnPolyline; }

        void                setGameMaterial( const Path& _value );

        bbool               isInitialized() { return m_polyline != NULL; }
        bbool               isActive() { return m_active; }

    private:



        void                clear();

        void                processStickToPolyline( EventStickOnPolyline* _eventStick );
        void                processHangOnPolyline( EventHanging* _eventHang );

        void                createPolylinePhantom();
        void                deletePolylinePhantom();

        PolyLine*           m_polyline;
        class PhysPhantom*  m_phantom;
        bbool               m_active;
        bbool               m_phantomRegistered;
        Path                m_material;
        ObjectRefList       m_objectsOnPolyline;
    };
};

#endif //_ITF_PROCEDURALPOLYLINE_H_