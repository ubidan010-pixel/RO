#include "precompiled_gameplay.h"

#ifndef _ITF_TELEPORTERCOMPONENT_H_
#include "gameplay/components/misc/TeleporterComponent.h"
#endif //_ITF_TELEPORTERCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_


#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(TeleporterComponent)

    BEGIN_SERIALIZATION_CHILD(TeleporterComponent)
    END_SERIALIZATION()


    TeleporterComponent::TeleporterComponent()
    : Super()
    {
        m_teleportActor.m_spline = NULL;
        clear();
    }


    TeleporterComponent::~TeleporterComponent()
    {
       clear();
    }

    void TeleporterComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    }

    void TeleporterComponent::clear()
    {
        m_teleportActor.m_or = ITF_INVALID_OBJREF;
        SF_DEL(m_teleportActor.m_spline);
    }


    void TeleporterComponent::Update( f32 _deltaTime) 
    {
        Super::Update(_deltaTime);

        if (m_teleportActor.m_or == ITF_INVALID_OBJREF)
        {
            clear();
            return;
        }

        m_teleportActor.m_fTime += _deltaTime * 0.8f;
        Actor* act = (Actor*)GETOBJECT(m_teleportActor.m_or);
        if (!act)
        {
            clear();
            return;
        }

        if (m_teleportActor.m_fTime >+ 1.f)
        {
            act->unpauseComponent(PlayerControllerComponent::GetClassCRCStatic());
            act->unpauseComponent(PhysComponent::GetClassCRCStatic());

            EventTeleport teleEvent(m_actor->getPos(),act->getAngle(),btrue);
            act->onEvent(&teleEvent);
            clear();


            StickToPolylinePhysComponent* comp = act->GetComponent<StickToPolylinePhysComponent>();
            if (comp)
            {
                comp->setDisabled(bfalse);
                comp->setSpeed(Vec2d::Zero);
            }
            GetActor()->getScene()->setAlwaysActiveActor(GetActor(),bfalse);

            return;
        }
        Vec3d pos;
        m_teleportActor.m_spline->GetInterpolatedAtTime(m_teleportActor.m_fTime, pos);
   //     GFX_ADAPTER->drawDBGCircle(pos.m_x, pos.m_y, 0.05f, 1, 1, 1, 0.1, pos.m_z);
        act->setPos(pos);
        act->onForceMove();
    }

    void TeleporterComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            EventTrigger* e = static_cast<EventTrigger*>(_event);
            ITF_ASSERT(e);
            Actor* p = static_cast<Actor*>(GETOBJECT(e->getActivator()));
            if(p && e->getActivated())
            {
                if (m_teleportActor.m_or != ITF_INVALID_OBJREF)
                    return;
                clear();
                GetActor()->getScene()->setAlwaysActiveActor(GetActor(),btrue);
                m_teleportActor.m_fTime = 0.f;
                m_teleportActor.m_or = p->getRef();
                m_teleportActor.m_spline = new Spline();
                m_teleportActor.m_spline->AddSplinePoint(p->getPos(), Spline::interp_spline, 0.f);
                Vec3d midPoint = (m_actor->getPos() + p->getPos()) * 0.5f;
                midPoint.m_y += 4.f;
                //f32 dist = (m_actor->getPos() - p->getPos()).norm();
                m_teleportActor.m_spline->AddSplinePoint(midPoint , Spline::interp_spline, 0.5f);
                m_teleportActor.m_spline->AddSplinePoint(m_actor->getPos() , Spline::interp_spline, 1.f);
                m_teleportActor.mzCamDiff = CAMERA->getZ() - p->getDepth();
                StickToPolylinePhysComponent* comp = p->GetComponent<StickToPolylinePhysComponent>();
                if (comp)
                {
                    comp->setDisabled(btrue);
                    comp->setSpeed(Vec2d::Zero);
                }

                // TODO: To do properly
                //PlayerControllerComponent* comp2 = p->GetComponent<PlayerControllerComponent>();
                //if (comp2)
                //{
                //    comp2->forceFreeFallState();
                //    comp2->Update(TARGETDT);
                //    comp2->forceFreeFallState();
                //}

                p->pauseComponent(ITF_GET_STRINGID_CRC(PhysComponent,1286338096));
                p->pauseComponent(ITF_GET_STRINGID_CRC(PlayerControllerComponent,1225958368));

/*                EventTeleport teleEvent;
                teleEvent.m_layerID = m_actor->getLayerID();
                Vec3d m_pos = m_actor->getPos();
                teleEvent.m_pos = m_pos;

                p->onEvent(&teleEvent);
                const f32 zDiff = CAMERA->getZ() - p->getDepth();
                CAMERA->teleport(Vec3d(m_pos.m_x, m_pos.m_y, m_pos.m_z + zDiff));
                */
            }
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(TeleporterComponent_Template)
    BEGIN_SERIALIZATION(TeleporterComponent_Template)

        SERIALIZE_MEMBER("INDEX",m_index);

    END_SERIALIZATION()
}