
#ifndef _ITF_POINTSCOLLISIONCOMPONENT_H_
#define _ITF_POINTSCOLLISIONCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{
    class PointsCollisionComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PointsCollisionComponent,ActorComponent,313321252);
        DECLARE_VALIDATE_COMPONENT()

    public:

        DECLARE_SERIALIZE()

        PointsCollisionComponent();
        virtual ~PointsCollisionComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onEvent( class Event* _event);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    protected:

        const class PointsCollisionComponent_Template* getTemplate() const;

        struct PolyUsers
        {
            ObjectRef                       m_polylineRef;
            u32                             m_edgeIndex;
            ObjectRef                       m_actor;
        };

        typedef ITF_LIST <PolyUsers> UsersList;
        typedef SafeArray <class PolyLine*> PolylinesList;

        void                                setDisabled( const bbool _disabled );

        void                                processNewUser( class EventStickOnPolyline* _event );
        void                                processNewHanging( class EventHanging* _event );
        void                                processUserEdgeChange( class EventStickOnPolylineChangeEdge* _event );

        void                                onNewUser( ActorRef _actor, class PolyLine* _poly, u32 _edgeIndex );
        void                                onRemoveUser( ActorRef _actor );
        void                                detachAndClearUsers();

        void                                registerPolylineDatas();
        void                                unregisterPolylineDatas();

        bbool                               m_disabled;
        PolylinesList                       m_currentPolylines;
        UsersList                           m_users;
        bbool                               m_objectsRegistered;
    };

    //---------------------------------------------------------------------------------------------------

    class PointsCollisionComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PointsCollisionComponent_Template,TemplateActorComponent,1109051015);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PointsCollisionComponent);

    public:

        PointsCollisionComponent_Template() {}
        ~PointsCollisionComponent_Template() {}

        struct PolylineData
        {
            DECLARE_SERIALIZE();

            PolylineData() : m_loop(bfalse), m_movingPolyline(bfalse) {}

            SafeArray <Vec2d>               m_pointList;
            Path                            m_gameMaterial;
            StringID                        m_regionType;
            bbool                           m_loop;
            bbool                           m_movingPolyline;
        };

        typedef ITF_VECTOR <PolylineData> PolylineDataList;

        u32                                 getNumPolylines() const { return m_polylines.size(); }
        const PolylineData&                 getPolylineData( u32 _index ) const { return m_polylines[_index]; }

    private:

        PolylineDataList                    m_polylines;
    };

    ITF_INLINE const PointsCollisionComponent_Template* PointsCollisionComponent::getTemplate() const
    {
        return static_cast<const PointsCollisionComponent_Template*>(m_template);
    }
}

#endif // _ITF_POINTSCOLLISIONCOMPONENT_H_
