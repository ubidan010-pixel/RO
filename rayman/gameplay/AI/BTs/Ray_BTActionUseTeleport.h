#ifndef _ITF_RAY_BTACTIONUSETELEPORT_H_
#define _ITF_RAY_BTACTIONUSETELEPORT_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

    class Ray_BTActionUseTeleport : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionUseTeleport,BTAction,582486196);

    public:

        Ray_BTActionUseTeleport();
        ~Ray_BTActionUseTeleport() {}

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );
        virtual void                            onActivate();
        virtual void                            onDeactivate();
        virtual bbool                           update( f32 _dt );

    private:

        enum TeleportType
        {
            TELEPORTTYPE_UNKNOWN,
            TELEPORTTYPE_CHANGEPAGE,
            TELEPORTTYPE_ENTERDOOR,
        };

        struct ChangePageData
        {
            ChangePageData() : m_speed(0.f), m_moveDir(Vec3d::Zero), m_travelfinished(bfalse), m_enterEjectForce(Vec2d::Zero), m_enterMovefinished(bfalse) {}

            ActorRef	m_player;
            Spline		m_spline;
            f32			m_speed;
            Vec3d		m_moveDir;		
            bbool		m_travelfinished;
            Vec2d		m_enterEjectForce;
            bbool		m_enterMovefinished;
        };

        enum EnterDoorState
        {
            ENTERDOORSTATE_NONE,
            ENTERDOORSTATE_WALKIN,
            ENTERDOORSTATE_WALKOUT,
        };

        struct EnterDoorData
        {
            EnterDoorData() : m_state(ENTERDOORSTATE_NONE), m_prevDirToTarget(Vec2d::Zero) {}

            EnterDoorState          m_state;
            Vec2d                   m_prevDirToTarget;
            ObjectRef               m_targetActor;
        };

        const class Ray_BTActionUseTeleport_Template*  getTemplate() const;

        void                                    prepareChangePage( Ray_EventTriggerChangePage* _event );
        void                                    prepareEnterDoor( Ray_EventTriggerEnterDoor* _event );
        bbool                                   updateChangePage( f32 _dt );
        bbool                                   updateEnterDoor( f32 _dt );

        class BaseAIControllerComponent*        m_aiController;
        Ray_EventTriggerChangePage              m_changePage;
        ChangePageData                          m_changePageData;
        Ray_EventTriggerEnterDoor               m_enterDoor;
        EnterDoorData                           m_enterDoorData;
        TeleportType                            m_type;
    };

    class Ray_BTActionUseTeleport_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionUseTeleport_Template,BTAction_Template,519951568);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionUseTeleport);

    public:

        Ray_BTActionUseTeleport_Template();
        ~Ray_BTActionUseTeleport_Template() {}

    private:

    };

    ITF_INLINE const Ray_BTActionUseTeleport_Template*  Ray_BTActionUseTeleport::getTemplate() const {return static_cast<const Ray_BTActionUseTeleport_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONUSETELEPORT_H_
