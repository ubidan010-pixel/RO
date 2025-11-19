#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_


namespace ITF
{
#ifdef DEVELOPER_BEN
    //#define DEBUG_REVIVE
#endif // DEVELOPER_BEN

    BEGIN_SERIALIZATION_SUBCLASS(Ray_PlayerControllerComponent,StateRevive_Template)
        SERIALIZE_MEMBER("duration", m_duration);
        SERIALIZE_MEMBER("bezierHitMultiplier", m_bezierHitMultiplier);
        SERIALIZE_MEMBER("bezierMidRadius", m_bezierMidRadius);
        SERIALIZE_MEMBER("bezierMidInfluence", m_BezierMidInfluence);
    END_SERIALIZATION()

    Ray_PlayerControllerComponent::StateRevive_Template::StateRevive_Template()
        : m_duration(1.f)
        , m_bezierHitMultiplier(2.f)
        , m_bezierMidRadius(3.f)
        , m_BezierMidInfluence(2.f)
    {
    }

    Ray_PlayerControllerComponent::StateRevive_Template::~StateRevive_Template()
    {
    }

    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateRevive)

    Ray_PlayerControllerComponent::StateRevive::StateRevive()
        : m_timer(0.f)
        , m_shouldRevive(bfalse)
    {
    }

    Ray_PlayerControllerComponent::StateRevive::~StateRevive()
    {
    }

    const Ray_PlayerControllerComponent::StateRevive_Template* Ray_PlayerControllerComponent::StateRevive::getTemplate() const
    {
        return &m_parent->getTemplate()->getStateRevive();
    }


    void Ray_PlayerControllerComponent::StateRevive::setData( ActorRef _otherPlayer, const Vec2d& _dir, const Vec2d& _targetPos )
    {
        if (m_target.isValid())
        {
            return;
        }

        m_target = _otherPlayer;
        m_targetPos = _targetPos;
        m_startPos = m_actor->get2DPos();
        m_startDir = _dir;
        m_timer = 0.f;
    }

    void Ray_PlayerControllerComponent::StateRevive::onEnter()
    {
        Super::onEnter();

        Actor* reviver = m_target.getActor();

        ITF_ASSERT(reviver);

        if ( reviver )
        {
            f32 initialScale = reviver->getWorldInitialScale().m_x;
            f32 currentScale = reviver->getScale().m_x;
            f32 scale = currentScale / initialScale;
            f32 myScale = m_actor->getWorldInitialScale().m_x*scale;

            m_actor->setScale(Vec2d(myScale,myScale));
        }

        m_characterPhys->setDisabled(btrue);

        m_parent->setCurrentPhantomShape(m_parent->getTemplate()->getStateDeadSoul().getPhantomShape());

        m_parent->disableSoftCollision();        

        //// Set faction : player -> ghost
        //m_parent->setFaction(m_parent->getTemplate()->getDeadSoulFaction());

        //m_parent->startDeadSoulFx();

        //EventTrail showTrail(btrue);
        //m_actor->onEvent(&showTrail);

#ifdef ITF_SUPPORT_ONLINETRACKING
		if (CONFIG->m_enableonlineTracking==btrue)
        {
		    u32 currentEngineVersion = Versioning::getEngineVersion();

		    f32 duration  = m_parent->m_stateDeadSoulTimer;
    		
		    u32 num_player = GAMEMANAGER->getNumActivePlayers();
            PathString_t levelName;

		    if (CURRENTWORLD!=NULL)
            {
			    if (CURRENTWORLD->getRootScene()!=NULL)
                {
				    CURRENTWORLD->getRootScene()->getPath().getString(levelName);
				}
    		}

			 Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

			ITF_STDSTRING checkPointStr = "";
			if ( checkpoint )
			{
				ObjectPath path;
				SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint,path);
				path.toString(checkPointStr);
			}

		    String msg;

		    msg.setTextFormat("VERSION=%d&NUMPLAYERS=%d&LEVELNAME=%s&CHECKPOINT=%s&PLAYERID=%d&DURATION=%.2f",
					    currentEngineVersion,
					    num_player,
                        StringToUTF8(levelName).get(),
					    checkPointStr.c_str(),
					    m_parent->getPlayerIndex()+1,
					    duration);

		    StringConverter c(msg);

            auto trk = ONLINETRACKING_ADAPTER;
		    if (trk)
		    {
			    //trk->sendTag("BUBBLE_EXIT",c.getChar() );

                trk->setAttributeInt("numPlayer", num_player);
                trk->setAttributeString("numPlayer", StringToUTF8(levelName).get());
                trk->setAttributeString("checkPoint", checkPointStr.c_str());
                trk->setAttributeInt("playerId", m_parent->getPlayerIndex() + 1);
                trk->setAttributeFloat("time", duration);

                trk->sendSignal("BubbleExit");
		    }
		}
#endif // ITF_SUPPORT_ONLINETRACKING
    }

    void Ray_PlayerControllerComponent::StateRevive::onExit()
    {
        Super::onExit();

        m_target.invalidate();
        m_targetPos = Vec2d::Zero;
        m_startPos = Vec2d::Zero;
        m_startDir = Vec2d::Zero;
        m_timer = 0.f;
        m_shouldRevive = bfalse;

        m_characterPhys->setDisabled(bfalse);
        m_parent->setPhantomShape();
        m_parent->restoreSoftCollision();
        m_actor->setAngle(0.f);
        m_parent->updatePhantom();

        // don't teleport the camera if no actor registered to avoid camera cut
        if ( m_actor->isEnabled() && !m_parent->m_pageActive )
        {
            CAMERACONTROLLERMANAGER->registerSubject(m_actor->getRef(), 0.0f, btrue, bfalse );
        }

        if ( m_parent->m_playerIndex != U32_INVALID && ( m_parent->m_nextState != &m_parent->m_stateDeadSoul || m_parent->m_nextState != &m_parent->m_stateDeath ) )
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_parent->m_playerIndex));
            player->setHitPoints(RAY_GAMEMANAGER->getStartHitPointsAfterDeath());
        }

        m_parent->m_inmunityCounter = m_parent->getTemplate()->getInmunityTimeRevive();
        m_parent->m_inmunityCounterReceiveHit = m_parent->getTemplate()->getInmunityTimeReceiveHitRevive();
    }

    bbool Ray_PlayerControllerComponent::StateRevive::checkStateChange( f32 _dt )
    {
        if (m_shouldRevive)
        {
            if ( !m_parent->trySwim(bfalse) )
            {
                m_parent->changeState(&m_parent->m_stateFalling);
            }
            else if ( m_parent->m_currentState != &m_parent->m_stateSwimIdle )
            {
                m_parent->changeState(&m_parent->m_stateSwimIdle);
            }

            return btrue;
        }

        return bfalse;
    }

    void Ray_PlayerControllerComponent::StateRevive::update(f32 _dt)
    {
        Actor* targetPlayer = m_target.getActor();

        if ( targetPlayer )
        {
            m_targetPos = targetPlayer->get2DPos();
        }

        m_timer = Min(m_timer+_dt,1.f);

        f32 t = m_timer / getTemplate()->getDuration();

        const Vec2d dir = (m_targetPos - m_startPos).normalize();
        const Vec2d perp = dir.getPerpendicular();
        const Vec2d midTarget = m_targetPos + perp * getTemplate()->getBezierMidRadius();

        Bezier4<Vec2d> b1, b2;
        b1.m_p0 = m_startPos;
        b1.m_p1 = m_startPos + m_startDir * getTemplate()->getBezierHitMultiplier();
        b1.m_p2 = midTarget - dir * getTemplate()->getBezierMidInfluence();
        b1.m_p3 = midTarget;

        b2.m_p0 = midTarget;
        b2.m_p1 = midTarget + dir * getTemplate()->getBezierMidInfluence();
        b2.m_p2 = m_targetPos - perp * getTemplate()->getBezierMidRadius();
        b2.m_p3 = m_targetPos;

        Vec2d pos;
        Vec2d tangent;

        if (t < 0.5f)
        {
            pos = b1.getPos(t * 2);
            tangent = b1.getTangent(t * 2);
        }
        else
        {
            pos = b2.getPos((t - 0.5f) * 2);
            tangent = b2.getTangent((t - 0.5f) * 2);
        }

        m_actor->set2DPos(pos);
        m_actor->setAngle(tangent.normalize().getAngle());

        if (t >= 1.f)
        {
            // target reached
            m_shouldRevive = btrue;
            // match target's speed
            if ( targetPlayer )
            {
                PhysComponent * physComp = targetPlayer->GetComponent<PhysComponent>();
                if ( physComp != NULL )
                {
                    m_characterPhys->setSpeed( physComp->getSpeed() );
                }
            }
        }

#ifdef DEBUG_REVIVE
        DebugDraw::bezier(b1, m_actor->getDepth(), Color::green(), 3.f, 0.1f);
        DebugDraw::bezier(b2, m_actor->getDepth(), Color::red(), 3.f, 0.1f);
        DebugDraw::circle(pos, m_actor->getDepth(), 0.3f, Color::yellow());
#endif // DEBUG_REVIVE
    }


    void Ray_PlayerControllerComponent::StateRevive::updateAnimInputs()
    {
        Super::updateAnimInputs();

        m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_REVIVECURSOR],Min(m_timer * 2.f / getTemplate()->getDuration(), 1.f));
    }

}
