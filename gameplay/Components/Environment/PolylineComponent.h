
#ifndef _ITF_POLYLINECOMPONENT_H_
#define _ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifdef ITF_IPAD
#include <string>
#endif

namespace ITF
{
    struct PolylineParameters
    {
        PolylineParameters();

        DECLARE_SERIALIZE()

        f32                             m_speedLoss;
        f32                             m_weightMultiplier;
        f32                             m_landSpeedMultiplier;
        f32                             m_hitForce;
        f32                             m_impulseMultiplier;
        f32                             m_windMultiplier;
        StringID                        m_regionType;
        Path                            m_gameMaterial;
        bbool                           m_environment;
        bbool                           m_usePhantom;
        bbool                           m_useMovingPolyline;
        bbool                           m_forceNoBlockHit;

        SafeArray <StringID>            m_polylineIds;
        SafeArray <StringID>            m_pointsIds;
    };

    typedef ITF_VECTOR <PolylineParameters> PolylineParamsList;

    typedef FixedArray<ActorRef, 8> PolylineUserList;

    class PolylineComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PolylineComponent,ActorComponent,4280564469);
        void                postLoadPropertyEdit();

        DECLARE_VALIDATE_COMPONENT()


    public:

        DECLARE_SERIALIZE()

        struct PolylinePoint
        {
            StringID            m_pointName;
            StringID            m_boneName;
            u32                 m_boneIndex;
            u32                 m_parametersIndex;
        };

        struct ProceduralPolyline
        {
            ProceduralPolyline();

            StringID                        m_animRef;
            class PolyLine*                 m_poly;
            class PhysPhantom*              m_phantom;
            SafeArray <PolylinePoint>       m_pointsInfo;
            u32                             m_parametersIndex;
            StringID                        m_regionType;
            bbool                           m_active;
            bbool                           m_registered;
        };
        typedef SafeArray <ProceduralPolyline*> ProceduralPolylinesList;

        //
        PolylineComponent();
        virtual ~PolylineComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
        virtual void        onEvent( class Event* _event);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        
        void                setDisabled( bbool _val);
        bbool               getDisabled() const { return m_disabled; }

        bbool               containsPolyline( ObjectRef _polylineRef ) const;

        f32                 getWeight() const;
        f32                 getMaxUserWeight() const;
        bbool               hasActors() const { return m_users.size() > 0 || m_hanging.size() > 0; }
        u32                 getUserCount() const { return u32(m_users.size() + m_hanging.size()); }
        u32                 getUserCount( const StringID& _polylineID ) const;
        void                getUserList( PolylineUserList& _userList ) const;

        const ProceduralPolylinesList&  getProcPolylines() const {return m_currentPolylines;}
        const ProceduralPolyline*       getProcPolylineFromObjRef( ObjectRef _ref ) const;
        const PolyLine*                 getPolylineFromAnimRef( const StringID& _id ) const;

    protected:

        const class PolylineComponent_Template* getTemplate() const;

        struct PolyUsers
        {
            StringID            m_polylineRef;
            u32                 m_edgeIndex;
            ObjectRef           m_actor;
            f32                 m_weight;
        };

        struct HangingActor
        {
            ObjectRef           m_actor;
            StringID            m_polylineRef;
            Vec2d               m_pos;
            u32                 m_edgeIndex;
            f32                 m_t;
            f32                 m_weight;
            Vec2d               m_speed;
        };

        typedef ITF_LIST <PolyUsers> UsersList;
        typedef ITF_LIST <HangingActor> HangingList;

        virtual void                        onResourceReady();
        virtual void                        readModel();

        virtual void                        processNewUser( class EventStickOnPolyline* _event );
        virtual void                        processNewHanging( class EventHanging* _event );
        virtual void                        processHangingUpdate( class EventHangUpdate* _event );
        virtual void                        processUserEdgeChange( class EventStickOnPolylineChangeEdge* _event );
        virtual void                        processHitStim( class HitStim* _hitStim ) {}
        virtual void                        processAnimUpdated( class EventAnimUpdated* _animUpdated );
        virtual void                        processPhysBodyForceEvent( class EventForceTransfer* _forceEvent ) {}
        virtual void                        processQueryBlockHits( class EventQueryBlocksHits* _query );
        virtual void                        processDisableCollision( class EventDisableCollision* _disableCollisionEvent );

        virtual void                        onNewHangActor ( class EventHanging* _hang, ProceduralPolyline* _procPoly );
        virtual void                        onRemoveHangActor( class EventHanging* _hang );
        virtual void                        onNewUser( class EventStickOnPolyline* _stick, ProceduralPolyline* _procPoly );
        virtual void                        onRemoveUser( class EventStickOnPolyline* _stick );

        void                                registerPolylineDatas();
        void                                unregisterPolylineDatas();
        void                                updatePolylines( f32 _dt );

        const PolylineParameters*           getPolylineParametersFromPolyPoint( const ProceduralPolyline* _poly, u32 _edgeIndex ) const;

        ProceduralPolyline*                 createPolylineFromAnim( class AnimPolyline* _animPolyline ) const;
        void                                createPolylinePhantom( ProceduralPolyline* _proceduralPoly ) const;
        void                                deletePolyline( ProceduralPolyline* _proceduralPoly );
        void                                deletePolylinePhantom( ProceduralPolyline* _proceduralPoly );
        ProceduralPolyline*                 getProcPolylineFromObjRef( ObjectRef _ref );
        ProceduralPolyline*                 getProcPolylineFromAnimRef( const StringID& _ref );
        void                                updateAnimPolyline( f32 _dt,
                                                                ProceduralPolyline* _proceduralPoly,
                                                                class AnimPolyline* _animPoly,
                                                                bbool _flipped ) const;

        void                                clear();

        bbool                               m_disabled;

        ProceduralPolylinesList             m_currentPolylines;
        
        class AnimLightComponent*           m_animComponent;
        HangingList                         m_hanging;
        UsersList                           m_users;
        bbool                               m_ready;
        bbool                               m_useGameMaterial;
        bbool                               m_objectsRegistered;
    };

    //---------------------------------------------------------------------------------------------------

    class PolylineComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PolylineComponent_Template,TemplateActorComponent,1921333574);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PolylineComponent);

    public:

        PolylineComponent_Template()
            : m_computeLocalToGlobal(bfalse)
        {}
        ~PolylineComponent_Template() {}

        const PolylineParamsList&           getPolylineParamsList() const { return m_polylineParams; }
        const PolylineParameters*           getPolylineParameters( u32 _index ) const;
        u32                                 getPolylineParametersIndex( const StringID& _poly, const StringID& _point ) const;
        ITF_INLINE bbool                    getComputeLocalToGlobal() const { return m_computeLocalToGlobal; }

    private:

        PolylineParamsList                  m_polylineParams;
        static const PolylineParameters     s_defaultPolylineParams;
        bbool                               m_computeLocalToGlobal;
    };

    ITF_INLINE const PolylineComponent_Template* PolylineComponent::getTemplate() const
    {
        return static_cast<const PolylineComponent_Template*>(m_template);
    }
}

#endif // _ITF_POLYLINECOMPONENT_H_
