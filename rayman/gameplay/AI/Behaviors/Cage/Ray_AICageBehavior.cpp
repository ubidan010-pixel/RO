#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AICAGEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Cage/Ray_AICageBehavior.h"
#endif //_ITF_RAY_AICAGEBEHAVIOR_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef __RAY_AIELECTOONBEHAVIOR_H__
#include "rayman/gameplay/AI/Behaviors/Electoon/Ray_AIElectoonBehavior.h"
#endif //__RAY_AIELECTOONBEHAVIOR_H__

#ifndef _ITF_RAY_AIMEDALBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Medal/Ray_AIMedalBehavior.h"
#endif //_ITF_RAY_AIMEDALBEHAVIOR_H_

#ifndef _ITF_RAY_ENTERDOORCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_EnterDoorComponent.h"
#endif //_ITF_RAY_ENTERDOORCOMPONENT_H_

#ifndef _ITF_RAY_ELECTOONTARGETCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ElectoonTargetComponent.h"
#endif //_ITF_RAY_ELECTOONTARGETCOMPONENT_H_

#ifndef _ITF_AFTERFXCOMPONENT_H_
#include "engine/actors/components/AfterFxComponent.h"
#endif //_ITF_AFTERFXCOMPONENT_H_

#ifndef _ITF_RAY_FORCEFIELDCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ForceFieldComponent.h"
#endif //_ITF_RAY_FORCEFIELDCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageComponent.h"
#endif //_ITF_RAY_CHANGEPAGECOMPONENT_H_

#ifndef _ITF_RAY_CAGEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_CageAIComponent.h"
#endif //_ITF_RAY_CAGEAICOMPONENT_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "Ray_OnlineTrackingManager.h"
#endif


#define CAGE_STAND_ANIM         "Stand"
#define CAGE_BREAKS_ANIM        "Cage_Smash"
#define CAGE_BROKEN_ANIM        "Cage_broken"
#define CAGE_LEFT_LOCK_FORMAT   "LLockState%d"
#define CAGE_RIGHT_LOCK_FORMAT  "RLockState%d"
#define CAGE_STEP_COUNT         6
#define CAGE_INTERMEDIATE_STEP  2
#define CAGE_BIGPUNCH_LEVEL_THRESHOLD 1

#define CAGE_SMASHOPENS_MARKER  "MRK_Smash_opens"
#define CAGE_LEFT_LOCK_POLYLINE "L_Col_Lock_L"
#define CAGE_RIGHT_LOCK_POLYLINE "L_Col_Lock_R"
#define CAGE_COL_POLYLINE "L_Col_Cage"


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AICageBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AICageBehavior_Template)
        SERIALIZE_MEMBER("electoonCount", m_electoonCount);
        SERIALIZE_MEMBER("electoonAppearRadius", m_electoonAppearRadius);
        SERIALIZE_MEMBER("cageSpawnerYOffset", m_cageSpawnerYOffset);
        SERIALIZE_MEMBER("electoonPath", m_electoonPath);
        SERIALIZE_MEMBER("stayElectoonPath", m_stayElectoonPath);
        SERIALIZE_MEMBER("durationBeforeFadeOut", m_durationBeforeFadeOut);
        SERIALIZE_MEMBER("stillHeartPath", m_stillHeartPath);
        SERIALIZE_MEMBER("broken_generatedRewardRadius", m_broken_generatedRewardRadius);
        SERIALIZE_CONTAINER("broken_generatedRewards", m_broken_generatedRewards);
        SERIALIZE_MEMBER("heartOffset", m_heartOffset);
        SERIALIZE_MEMBER("lastCageIndex", m_lastCageIndex);
        SERIALIZE_MEMBER("alreadyOpenAlpha", m_alreadyOpenAlpha);
        SERIALIZE_MEMBER("stayElectoonXOffset", m_stayElectoonXOffset);

        SERIALIZE_MEMBER("cinePath", m_cinePath);
        SERIALIZE_MEMBER("endCinePath", m_endCinePath);
    END_SERIALIZATION()

        bbool Ray_AICageBehavior::s_globalLocksOptOut = bfalse;

        Ray_AICageBehavior_Template::Ray_AICageBehavior_Template()
        : m_electoonCount(10)
        , m_electoonAppearRadius(0.4f)
        , m_cageSpawnerYOffset(0.3f)
        , m_durationBeforeFadeOut(5.f)
        , m_broken_generatedRewardRadius(0.5f)
        , m_heartOffset(0,0,0)
        , m_lastCageIndex(4)
        , m_alreadyOpenAlpha(0.75f)
        , m_stayElectoonXOffset(0.5f)
    {

    }

    IMPLEMENT_OBJECT_RTTI(Ray_AICageBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AICageBehavior)
    VALIDATE_BEHAVIOR_PARAM("", m_actor->GetComponents<PolylineComponent>().size()==1, "there must 1 poly line component for cage collision, left lock phantom, right lock phantom");
    VALIDATE_BEHAVIOR_PARAM("", m_cageAIComponent, "there must be a cage ai component");
    VALIDATE_BEHAVIOR_PARAM("cinePath", !getTemplate()->getCinePath().isEmpty(), "path to medal cinematics is not set");
    VALIDATE_BEHAVIOR_PARAM("endCinePath", !getTemplate()->getEndCinePath().isEmpty(), "path to final medal cinematics is not set");
    END_VALIDATE_BEHAVIOR()

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AICageBehavior::Ray_AICageBehavior()
        : Super()
        , m_state(State_NA)
        , m_lLockStep(0)
        , m_rLockStep(0)
        , m_lastLLockAnim(-1)
        , m_lastRLockAnim(-1)
        , m_isLLockAnimPlaying(bfalse)
        , m_isRLockAnimPlaying(bfalse)
        , m_timeInCurrentState(0.0f)
        , m_smashOpenTimer(-1)
        , m_fadeTimer(FLT_MAX)
        , m_sendActivationMessageToObjectsASAP(bfalse)
        , m_lastFrameLockAltered(0)
        , m_cageAIComponent(NULL)
        , m_spawnedRewardsThisTime(bfalse)
        , m_spawnedEntranceElectoons(bfalse)
        , m_wasSmashOpen(bfalse)
        , m_locksOptOut(bfalse)
    {        
    }

    Ray_AICageBehavior::~Ray_AICageBehavior()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), m_aiComponent);
        setLockOptOut(bfalse);
    }

    void Ray_AICageBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        SPAWNER->declareNeedsSpawnee(m_actor, &m_electoonGenerator, getTemplate()->getElectoonPath());
        SPAWNER->declareNeedsSpawnee(m_actor, &m_stayElectoonGenerator, getTemplate()->getStayElectoonPath());
        SPAWNER->declareNeedsSpawnee(m_actor, &m_stillHeartGenerator, getTemplate()->getStillHeartPath());
        m_breakerActor.invalidate();

        m_spawnedRewardsThisTime = bfalse;
        m_wasSmashOpen = bfalse;
        m_cageAIComponent = m_actor->GetComponent<Ray_CageAIComponent>();
        if (m_cageAIComponent)
        {
            i32 cageIndex = m_cageAIComponent->getCageIndex();
            ITF_WARNING_CATEGORY(LD, m_actor, cageIndex>=0 && cageIndex<MEDAL_ELECTOON_COUNT, "Invalid cage index (%d) - must be in 0...%d", cageIndex,MEDAL_ELECTOON_COUNT-1);
        }

        const Path &cinePath = isLastCage()? getTemplate()->getEndCinePath():getTemplate()->getCinePath();
        SPAWNER->declareNeedsSpawnee(m_actor, &m_cineGenerator, cinePath);

        m_actor->setScale(Vec2d(4.f,4.f));

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventMedalCinematicEnded,1196816250),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanZoomOnTarget,4237975921),m_aiComponent);

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),m_aiComponent);

        resetState();
    }

    void Ray_AICageBehavior::onResourceReady()
    {
        Super::onResourceReady();                        
    }

    void Ray_AICageBehavior::sendActivationMessageToMostObjects()
    {
        LinkComponent *link = m_actor->GetComponent<LinkComponent>();
        if (link)
        {
            const LinkComponent::ChildrenList &children = link->getChildren();
            u32 count = children.size();
            for (u32 i=0; i<count; i++)
            {
                EventTrigger trigger;
                trigger.setActivated(btrue);
                trigger.setActivator(m_actor->getRef());
                trigger.setRefPos(m_actor->get2DPos());
                Actor *target = (Actor*)SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                if (target && target->getObjectType()==BaseObject::eActor)
                {
                    if (!target->GetComponent<Ray_EnterDoorComponent>() && !target->GetComponent<AfterFxComponent>()) //doors are handled by the cage itself
                    {
                        target->onEvent(&trigger);
                    }
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Actor *Ray_AICageBehavior::spawnBrokenCageRewardFromCode(u32 _code, const Vec3d &_pos)
    {
        Actor *act = NULL;
        switch(_code)
        {
        case 1: act = m_stillHeartGenerator.getSpawnee(m_actor->getScene(), _pos+getTemplate()->getHeartOffset()); break;
        default: act = RAY_GAMEMANAGER->getLumGenerator().getSpawnee(m_actor->getScene(), _pos); break;
        }
        if(act)
        {
            ActorBind *parentBind = newAlloc(mId_Gameplay, ActorBind);
            SceneObjectPathUtils::getRelativePathFromObject(act, m_actor, parentBind->m_parent);
            parentBind->m_type = ActorBind::Root;
            Vec3d posOffset = act->getPos()-m_actor->getPos();
            posOffset.m_z  += 0.015f;
            parentBind->setInitialPositionOffset(posOffset);
            parentBind->setPosOffset(posOffset);
            act->setParentBind(parentBind);

        }

        return act;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::spawnBrokenCageRewards()
    {
        Vec3d center;

        if (m_spawnedRewardsThisTime)
            return;

        const SafeArray<u32> &rewardCodes=getTemplate()->getBroken_generatedRewards();
        if (rewardCodes.size()>0)
        {
            center.setFromVec2d(Vec2d(0,getTemplate()->getCageSpawnerYOffset())*m_actor->getScale());
            center.m_z = 0;
            center += m_actor->getPos();
            spawnBrokenCageRewardFromCode(rewardCodes[0], center);

            f32 radius = getTemplate()->getBroken_generatedRewardRadius() * m_actor->getScale().m_x;
            f32 angle = 0;
            f32 angleStep = 0;
            if (rewardCodes.size())
            {
                angleStep = MTH_2PI/(rewardCodes.size()-1.f);
            }
            for (u32 i=1; i<rewardCodes.size(); i++, angle += angleStep)
            {
                f32 c,s;
                f32_CosSin(angle, &c, &s);
                Vec3d pos = center + Vec3d(radius*c,radius*s,0.f);
                spawnBrokenCageRewardFromCode(rewardCodes[i], pos);
            }
        }

        m_spawnedRewardsThisTime = btrue;

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::resetState()
    {
        m_lLockStep = 0;
        m_rLockStep = 0;
        m_wasSmashOpen = bfalse;
        m_lastFrameLockAltered = CURRENTFRAME;
        m_fadeTimer = FLT_MAX;
        m_breakerActor.invalidate();

        setLockOptOut(bfalse);

        if (RAY_GAMEMANAGER->getIsCageBroken(m_actor))
        {
            m_sendActivationMessageToObjectsASAP = btrue;

            //setState(State_Broken);
            sendSpawnElectoonsIfNotAlreadyDone();
            setState(State_Stands);
            if (m_cageAIComponent && m_cageAIComponent->wasBrokenInSession())
                sendCloseCageDoor();
            m_animatedComponent->setAlpha(getTemplate()->getAlreadyOpenAlpha());
            //spawnBrokenCageRewards();
        }
        else
        {
            setState(State_Stands);        
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AICageBehavior::areAllEnemiesDead()
    {
        bbool isEnemyAlive = bfalse;
        LinkComponent *linkComp = m_actor->GetComponent<LinkComponent>();
        if (linkComp)
        {
            const LinkComponent::ChildrenList &children = linkComp->getChildren();
            for (u32 i=0; i<children.size(); i++)
            {
                Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                if (p)
                {
                    Actor *act = p->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                    if (act)
                    {
                        Ray_AIComponent *groundAI = act->GetComponent<Ray_AIComponent>();
                        if (groundAI)
                        {
                            if (act->isEnabled())
                            {
                                isEnemyAlive  = btrue;
                                break;
                            }
                        }
                    }

                }
            }
        }
        return !isEnemyAlive;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::getElectoonTargets(SafeArray<ObjectRef> &_electoonPlacements, u32 _electoonCount) const
    {
        LinkComponent *linkComp = m_actor->GetComponent<LinkComponent>();
        _electoonPlacements.clear();
        _electoonPlacements.reserve(_electoonCount);
        FixedArray<ObjectRef, 32> targets;
        if (linkComp)
        {
            const LinkComponent::ChildrenList &children = linkComp->getChildren();
            for (u32 i=0; i<children.size(); i++)
            {
                if ( children[i].getTagValueList().size() == 0 )
                {
                    Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                    if (p)
                    {
                        Actor *act = p->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                        if (act)
                        {
                            Ray_ElectoonTargetComponent *destComponent = act->GetComponent<Ray_ElectoonTargetComponent>();
                            if (destComponent)
                            {
                                targets.push_back(act->getRef());
                            }
                        }

                    }
                }
            }
        }

        u32 targetCount = targets.size();
        if (targetCount)
        {
            u32 targetIndex=0;           
            for (u32 i=0; i<_electoonCount; i++)
            {
                _electoonPlacements.push_back(targets[targetIndex]);
                targetIndex = targetIndex+1;
                if (targetIndex>=targetCount)
                {
                    targetIndex -= targetCount;
                }

            }
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::setLockOptOut(bbool _forbid)
    {
        //if cage goes to FORBID : global lock
        //if cage goes FROM FORBIT : global unlock
        if (_forbid != m_locksOptOut)
        {
            m_locksOptOut = _forbid;
            s_globalLocksOptOut = _forbid;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::update( f32 _deltaTime )
    {
        Super::update(_deltaTime);

        m_timeInCurrentState += _deltaTime;
        m_fadeTimer -= _deltaTime;



        if (m_fadeTimer<0) /////////TEMPORARY!!!!
        {
            m_fadeTimer = FLT_MAX;
            Color startColor = Color::black();
            startColor.m_a = 0.f;

            RAY_GAMEMANAGER->fxFade(startColor,Color::black(),2.0f,bfalse);
        }

        //Update the states. 
        switch (m_state)
        {
            case State_Preappears:
            {
                m_fadeTimer = FLT_MAX;
                setState(State_Stands);
            }break;
            case State_Stands:
            {
                m_fadeTimer = FLT_MAX;
                sendActivationToAfterFX(bfalse);
            } break;
            case State_Breaks:
            {
                setLockOptOut(btrue);
                m_smashOpenTimer--;
                if (m_smashOpenTimer==0)
                {
                    //We need a few frames before actually showing the medal
                    {
                        Ray_EventShowElectoonMedal showMedal;

                        showMedal.m_forBrokenCage       = btrue;
                        showMedal.m_autoEndLevel        = isLastCage();
                        showMedal.m_forLumAndTimeAttacks = bfalse;
                        showMedal.m_dontSetLumAttack1   = btrue;
                        showMedal.m_dontSetLumAttack2   = btrue;
                        showMedal.m_dontSetTimeAttack   = btrue;

                        Actor *cineActor = m_cineGenerator.getSpawnee(m_actor->getScene(), m_actor->getPos());
                        if (cineActor)
                        {
                            showMedal.m_customCineRef = cineActor->getRef();
                        }

                        showMedal.setSender(m_actor->getRef());

                        EVENTMANAGER->broadcastEvent(&showMedal);
                    }
                }

                if (m_animatedComponent->isSubAnimFinished() && m_smashOpenTimer<0 && !CINEMATICMANAGER->getLockPlayers())
                {
                    setState(State_Broken);
                }                
            } break;

            case State_Broken:
            {
                if (m_sendActivationMessageToObjectsASAP)
                {
                    m_sendActivationMessageToObjectsASAP = bfalse;
                    sendActivationMessageToMostObjects();
                }
            } break;

        }
        //
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Check if given puch is for a lock
    bbool Ray_AICageBehavior::checkPunchStimOnPhantomPolyline(const StringID &_lockID, PunchStim *_hit)
    {
        const PolylineComponent *comp = (PolylineComponent*)m_actor->GetComponent<PolylineComponent>();
        if (comp)
        {

            for (u32 contactIndex=0; contactIndex<_hit->getContacts().size(); contactIndex++)
            {
                const SCollidableContact &contact = _hit->getContacts()[contactIndex];
                ObjectRef polylineRef = contact.m_collidableUserData;

                const PolylineComponent::ProceduralPolyline *line = comp->getProcPolylineFromObjRef(polylineRef);
                if (line && line->m_animRef == _lockID)
                {
                    return btrue;
                }
            }
        }
        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AICageBehavior::incrementLockStep(u32 _lockIndex, u32 &_step)
    {
        m_lastFrameLockAltered = CURRENTFRAME;
        if (_step<(CAGE_STEP_COUNT-1))
        {

            u32 nextValue;
            
            if (_step<CAGE_INTERMEDIATE_STEP)
                nextValue = CAGE_INTERMEDIATE_STEP;
            else
                nextValue = CAGE_STEP_COUNT-1;
            
            updateLock(_lockIndex, nextValue);
            setLockRumbleAnim(_lockIndex, btrue);
            setCageRumbleAnim(btrue);
            if (nextValue>=(u32)(CAGE_STEP_COUNT-1))
            {
                return btrue;
            }
            return bfalse;
        }
        else
        {
            return btrue;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::onEvent( Event* _event )
    {

        if (EventQueryCanZoomOnTarget *queryCanZoom=_event->DynamicCast<EventQueryCanZoomOnTarget>(ITF_GET_STRINGID_CRC(EventQueryCanZoomOnTarget,4237975921)))
        {
            queryCanZoom->setCanZoomOnTarget(m_state==State_Stands);
        }

        if (m_state==State_Stands)
        { 
            if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
            {

                if (!isLinkedToForceField())
                {

                    if (m_fxController)
                    {
                        const StringID feedbackAction = hit->getActionFromHit();
                        u32 fxHandle = m_fxController->playFeedback(hit->getSender(), feedbackAction, m_actor->getRef());
                        m_fxController->setFXPos(fxHandle,hit->getFxPos());
                    }

                    if (GAMEMANAGER->isPlayerActor(hit->getSender()))
                    {
                        bbool isBigPunch = (hit->getLevel()>=CAGE_BIGPUNCH_LEVEL_THRESHOLD);
                        EReceivedHitType hitType = hit->getReceivedHitType();
                        bbool isAimedHit = hitType==RECEIVEDHITTYPE_FRONTPUNCH || hitType==RECEIVEDHITTYPE_UPPUNCH;

                        bbool punchedLock = bfalse;
                        bbool forceBreak = bfalse;

                        Actor *theSender = IRTTIObject::SafeDynamicCast<Actor>(hit->getSender().getObject(),ITF_GET_STRINGID_CRC(Actor,2546623115));
                        bbool isTornadoAttack = bfalse;

                        if (theSender && (hitType==RECEIVEDHITTYPE_FRONTPUNCH) )
                        {
                            Ray_PlayerControllerComponent *playerComp = theSender->GetComponent<Ray_PlayerControllerComponent>();
                            if (playerComp)
                            {
                                isTornadoAttack = (playerComp->getCurrentPunchHitType() == PUNCHTYPE_TORNADO);
                            }
                        }

                        if (hitType==RECEIVEDHITTYPE_CHARGE_PUNCH || isTornadoAttack)
                        {
                            forceBreak = btrue;
                            isAimedHit = bfalse;

                        }

                        //If player hit the cage, process
                        if (isAimedHit)
                        {
                            if (checkPunchStimOnPhantomPolyline(StringID(CAGE_LEFT_LOCK_POLYLINE), hit))
                            {
                                punchedLock = btrue;
                                if (m_isLLockAnimPlaying)
                                {                            
                                }
                                else
                                {
                                    if (isBigPunch)
                                        m_lLockStep = std::max(m_lLockStep, (u32)(CAGE_STEP_COUNT-3));

                                    incrementLockStep(0, m_lLockStep);
                                }
                            }
                            if (checkPunchStimOnPhantomPolyline(StringID(CAGE_RIGHT_LOCK_POLYLINE), hit))
                            {
                                punchedLock = btrue;
                                if (m_isRLockAnimPlaying)
                                {
                                }
                                else
                                {
                                    if (isBigPunch)
                                        m_rLockStep = std::max(m_rLockStep, (u32)(CAGE_STEP_COUNT-3));

                                    incrementLockStep(1, m_rLockStep);
                                }
                            }
                        }

                        //crushing the cage by the top
                        bbool hitOnTop = (hit->getReceivedHitType()==RECEIVEDHITTYPE_EARTHQUAKE)
                            && checkPunchStimOnPhantomPolyline(StringID(CAGE_COL_POLYLINE),hit);
                        if (!punchedLock 
                            && (hitOnTop||forceBreak) )
                        {
                            u32 minFrameDelay = (u32)(0.5f/LOGICDT);
                            if (CURRENTFRAME>=(m_lastFrameLockAltered+minFrameDelay))
                            {
                                m_lLockStep = m_rLockStep = CAGE_STEP_COUNT;
                                incrementLockStep(0, m_lLockStep);
                                incrementLockStep(1, m_rLockStep);
                            }
                        }

                        //
                        if (std::min(m_lLockStep, m_rLockStep)>=(u32)(CAGE_STEP_COUNT-2))
                        {
                            m_breakerActor = hit->getSender();
                            setState(State_Breaks);
                        }

                    }
                }
            }
            else if (EventQueryBlocksHits* onQueryBlockHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
            {
                if (m_state!=State_Broken)
                {
                    onQueryBlockHits->setCanBlockHits(btrue);
                }
            }
            else if (Ray_EventQueryOpen * queryOpen = _event->DynamicCast<Ray_EventQueryOpen>(ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153)))
            {
                queryOpen->setOpen(m_state == State_Broken);
            }
        }

        if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            const StringID &eventName = animEvent->getName();
            if ( eventName == CAGE_SMASHOPENS_MARKER)
            {
                //Synchronization event when cage opens
                processSmashOpenEvent();
            }
            else if (eventName == ITF_GET_STRINGID_CRC(MRK_Rumble_GameplayStop,668094208))
            {
                setCageRumbleAnim(bfalse);
            }
            else if (eventName == ITF_GET_STRINGID_CRC(MRK_L_Hit_Strong_GameplayStop,1534188029))
            {
                setLockRumbleAnim(0,bfalse);
            }
            else if (eventName == ITF_GET_STRINGID_CRC(MRK_R_Hit_Strong_GameplayStop,1658941372))
            {
                setLockRumbleAnim(1,bfalse);
            }
            else
            {
                const StringID lAnimMarkers[]= //From _to_state1 only
                {
                    ITF_GET_STRINGID_CRC(MRK_L_To_State1_GamePlay_Finish,180351789),
                    ITF_GET_STRINGID_CRC(MRK_L_To_State2_GamePlay_Finish,3556854375),
                    ITF_GET_STRINGID_CRC(MRK_L_To_State3_GamePlay_Finish,1326952848),
                    ITF_GET_STRINGID_CRC(MRK_L_To_State4_GamePlay_Finish,459884758),
                    ITF_GET_STRINGID_CRC(MRK_L_To_StateFinish_GamePlay_Finish,10979455)
                };

                const StringID rAnimMarkers[]= //From _to_state1 only. State 0 is not present here
                {
                    ITF_GET_STRINGID_CRC(MRK_R_To_State1_GamePlay_Finish,3026229968),
                    ITF_GET_STRINGID_CRC(MRK_R_To_State2_GamePlay_Finish,665273904),
                    ITF_GET_STRINGID_CRC(MRK_R_To_State3_GamePlay_Finish,656299885),
                    ITF_GET_STRINGID_CRC(MRK_R_To_State4_GamePlay_Finish,2896260490),
                    ITF_GET_STRINGID_CRC(MRK_R_To_StateFinish_GamePlay_Finish,4025327464)
                };

                //following LLock/RLock state anims
                ITF_ASSERT(ITF_ARRAY_SIZE(lAnimMarkers) == ITF_ARRAY_SIZE(rAnimMarkers));
                for (u32 index=0; index<ITF_ARRAY_SIZE(lAnimMarkers); index++)
                {
                    if (lAnimMarkers[index] == eventName)
                    {
                        m_isLLockAnimPlaying = bfalse;
                    }
                    else if (rAnimMarkers[index] == eventName)
                    {
                        m_isRLockAnimPlaying = bfalse;
                    }
                }
            }
        }
        else if (EventRevertToLastCheckpoint *checkpointEvent = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)))
        {
            //RAY_GAMEMANAGER->setIsCageBroken(m_actor,  state);
            resetState();
        }
        else if ( Ray_EventQueryDoorData* queryData = _event->DynamicCast<Ray_EventQueryDoorData>(ITF_GET_STRINGID_CRC(Ray_EventQueryDoorData,2144329540)) )
        {
            queryData->setData(m_actor->get2DPos(),m_exitPoint,btrue);
        }
        else if ( Ray_EventMedalCinematicEnded *cineEnded = _event->DynamicCast<Ray_EventMedalCinematicEnded>(ITF_GET_STRINGID_CRC(Ray_EventMedalCinematicEnded,1196816250)))
        {
            m_sendActivationMessageToObjectsASAP = btrue;
            sendActivationToChangePage(btrue);
        }

        Super::onEvent(_event);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AICageBehavior::isLastCage() const
    {
        return m_cageAIComponent && (m_cageAIComponent->getCageIndex() == getTemplate()->getLastCageIndex());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::setCageRumbleAnim(bbool _on)
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(CageRumble,2497550221), (_on?1.0f:0.f));
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::setLockRumbleAnim(u32 _lockIndex, bbool _on)
    {
        _on = bfalse; //currently, animators have problems with it
        if (_lockIndex==0)
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(LStrongRumble,4080756941), (_on?1.0f:0.0f));
        }
        else
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(RStrongRumble,2267751619), (_on?1.0f:0.0f));
        }        
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::processSmashOpenEvent()
    {
        if (m_wasSmashOpen)
            return;

        //Send info to the medal
        m_smashOpenTimer = 5;
        m_wasSmashOpen = btrue;
        //f64 t = SYSTEM_ADAPTER->getTime();
        //DmStartProfiling("devkit:\\sebastien.mathieu\\technowithtools\\trace_electoons.cap", 40*1024*1024);
        u32 electoonCount = getTemplate()->getElectoonCount();

        SafeArray<ObjectRef> electoonPlacements;
        getElectoonTargets(electoonPlacements, electoonCount);

        for (u32 i=0; i<electoonCount; i++)
        {
            Vec3d center;
            center.setFromVec2d(Vec2d(0,getTemplate()->getCageSpawnerYOffset())*m_actor->getScale());
            center.m_z = 0;
            f32 rndRad = Seeder::getSharedSeeder().GetFloat(0, getTemplate()->getElectoonAppearRadius());
            f32 rndAngle = Seeder::getSharedSeeder().GetFloat(0, MTH_2PI);

            Vec3d sc;
            f32_CosSin(rndAngle, &sc.m_x, &sc.m_y);
            sc.m_z = 0.f;
            Vec3d spawnpos = m_actor->getPos()+center+rndRad*sc;
    
            Actor *act = m_electoonGenerator.getSpawnee(m_actor->getScene(), spawnpos);
            if (act)
            {

                if (electoonPlacements.size()==electoonCount)
                {
                    EventSetTarget setTarget(electoonPlacements[i]);
                    setTarget.setSender(m_actor->getRef());
                    act->onEvent(&setTarget);
                }

                act->setFlipped(Seeder::getSharedSeeder().GetFloat(0.f,1.f)<0.5f);
            }
        }
        //f64 dt = SYSTEM_ADAPTER->getTime()-t;
        //DmStopProfiling();
        //LOG("--DT=%lf", dt);

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Update lock state
    void Ray_AICageBehavior::updateLock(u32 _lockIndex, u32 _step)
    {
        switch(_lockIndex)
        {
        case 0:            
            m_lLockStep = _step;
            break;
        case 1:
            m_rLockStep = _step;
            break;
        default : 
            ITF_ASSERT(0); 
            break;
        }

        setStandInputs();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::setStandInputs()
    {
        ITF_ASSERT(m_state==State_Stands);

        char pBuffer1[64], pBuffer2[64];
        for (u32 i=0; i<CAGE_STEP_COUNT; i++)
        {
            sprintf(pBuffer1, CAGE_LEFT_LOCK_FORMAT, i);
            sprintf(pBuffer2, CAGE_RIGHT_LOCK_FORMAT, i);

            m_animatedComponent->setInput(pBuffer1, (i==m_lLockStep)?1.0f:0.0f);
            m_animatedComponent->setInput(pBuffer2, (i==m_rLockStep)?1.0f:0.0f);
        }

        //we track left/right lock animation playing
        if (m_lLockStep==0)
        {
            m_isLLockAnimPlaying = bfalse;
        }
        else
        {
            if ((i32)m_lLockStep!=m_lastLLockAnim)
            {
                m_isLLockAnimPlaying = btrue;
            }
        }

        if (m_rLockStep==0)
        {
            m_isRLockAnimPlaying = bfalse;
        }
        else
        {
            if ((i32)m_rLockStep!=m_lastRLockAnim)
            {
                m_isRLockAnimPlaying = btrue;
            }
        }

        //
        m_lastLLockAnim = (i32)m_lLockStep;
        m_lastRLockAnim = (i32)m_rLockStep;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AICageBehavior::isLinkedToForceField()
    {
        LinkComponent *link = m_actor->GetComponent<LinkComponent>();
        if (link)
        {
            const LinkComponent::ChildrenList &children = link->getChildren();
            u32 count = children.size();
            for (u32 i=0; i<count; i++)
            {
                EventTrigger trigger;

                Actor *target = (Actor*)SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                if (target && target->getObjectType()==BaseObject::eActor)
                {
                    Ray_ForceFieldComponent *forceField = target->GetComponent<Ray_ForceFieldComponent>();
                    if (forceField)
                    {
                        return forceField->isLocked();
                    }
                }
            }
        }
        return bfalse;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::sendCloseCageDoor()
    {
        LinkComponent *link = m_actor->GetComponent<LinkComponent>();
        if (link)
        {
            const LinkComponent::ChildrenList &children = link->getChildren();
            u32 count = children.size();
            for (u32 i=0; i<count; i++)
            {                
                Actor *target = (Actor*)SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                if (target && target->getObjectType()==BaseObject::eActor)
                {
                    Ray_ChangePageComponent *changePage = target->GetComponent<Ray_ChangePageComponent>();
                    if (changePage && changePage->isCageMapDoor())
                    {
                        changePage->setShut(btrue);
                    }
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::sendActivationToChangePage(bbool _activation)
    {
        LinkComponent *link = m_actor->GetComponent<LinkComponent>();
        if (link)
        {
            const LinkComponent::ChildrenList &children = link->getChildren();
            u32 count = children.size();
            for (u32 i=0; i<count; i++)
            {
                EventTrigger trigger;
                trigger.setActivated(_activation);
                trigger.setActivator(m_actor->getRef());
                trigger.setSender(m_actor->getRef());

                Actor *target = (Actor*)SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                if (target && target->getObjectType()==BaseObject::eActor)
                {
                    Ray_ChangePageComponent *changePageComponent = target->GetComponent<Ray_ChangePageComponent>();
                    if (changePageComponent && changePageComponent->isWarpZoneExit())
                    {
                        target->onEvent(&trigger);

                        if (_activation)
                        {

                            if (m_breakerActor.isValid())
                            {
                                Ray_EventChangePage changePage;
                                changePage.setActivated(_activation);
                                changePage.setActivator(m_breakerActor);
                                changePage.setSender(m_actor->getRef());
                                target->onEvent(&changePage);
                            }

                        }
                    }
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::sendActivationToAfterFX(bbool _activation)
    {
        LinkComponent *link = m_actor->GetComponent<LinkComponent>();
        if (link)
        {
            const LinkComponent::ChildrenList &children = link->getChildren();
            u32 count = children.size();
            for (u32 i=0; i<count; i++)
            {
                if ( children[i].getTagValueList().size() == 0 )
                {
                    EventTrigger trigger;
                    trigger.setActivated(_activation);
                    trigger.setActivator(m_actor->getRef());
                    trigger.setSender(m_actor->getRef());

                    EventActivate activate;
                    activate.setSender(m_actor->getRef());
                    activate.setActivated(_activation);

                    Actor *target = (Actor*)SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
                    if (target && target->getObjectType()==BaseObject::eActor)
                    {
                        if (target->GetComponent<AfterFxComponent>()) //doors are handled by the cage itself
                        {
                            target->onEvent(&activate);
                            target->onEvent(&trigger);
                            if (!_activation)
                            {
                                EventResetAfterFxAlpha reset;
                                target->onEvent(&reset);
                            }
                        }
                    }
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// When the cage is open we want to spawn electoons at the entrance of the level
    void Ray_AICageBehavior::sendSpawnElectoonsIfNotAlreadyDone()
    {
        if (m_spawnedEntranceElectoons)
            return;
        m_spawnedEntranceElectoons = btrue;

        LinkComponent *linkComp = m_actor->GetComponent<LinkComponent>();

        if (linkComp)
        {
            const LinkComponent::ChildrenList &children = linkComp->getChildren();

            for (u32 i=0; i<children.size(); i++)
            {
                const ChildEntry& child = children[i];

                if ( child.hasTag(ITF_GET_STRINGID_CRC(spawnElectoons,1976413112)) )
                {
                    Pickable* p;
                    
                    if ( child.getPath().getIsAbsolute() )
                    {
                        p = SceneObjectPathUtils::getObjectFromAbsolutePath(child.getPath());
                    }
                    else
                    {
                        p = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, child.getPath());
                    }

                    if (p)
                    {
                        u32 electoonCount = getTemplate()->getElectoonCount();

                        for (u32 i=0; i<electoonCount; i++)
                        {
                            Vec2d center;
                            center = Vec2d(0,getTemplate()->getCageSpawnerYOffset())*m_actor->getScale();
                            Actor *destActor = IRTTIObject::SafeDynamicCast<Actor>(p,ITF_GET_STRINGID_CRC(Actor,2546623115));
                            f32 destActorAngle = 0;
                            if (destActor)
                            {
                                center.m_x = getTemplate()->getStayElectoonXOffset();
                                if (!destActor->isFlipped())
                                    center.m_x = -center.m_x;
                                destActorAngle = destActor->getAngle();
                            }

                            f32 rndRad = Seeder::getSharedSeeder().GetFloat(0, getTemplate()->getElectoonAppearRadius());
                            f32 rndAngle = Seeder::getSharedSeeder().GetFloat(0, MTH_2PI);

                            Vec2d sc;
                            f32_CosSin(rndAngle, &sc.m_x, &sc.m_y);
                            Vec2d spawnOffset = center+rndRad*sc;
                            spawnOffset = spawnOffset.Rotate(destActorAngle);

                            //this patch is to fix a bug in scene order initialization. If the scene which contains the target pos is not initialized, its pos will be zero.
                            //In this case, we use world initial pos.
                            Vec3d targetPos = p->getPos();
                            if (targetPos.IsEqual(Vec3d::Zero))
                            {
                                targetPos = p->getWorldInitialPos();
                            }

                            //
                            Vec3d spawnpos = targetPos+spawnOffset.to3d();

                            Actor *act = m_stayElectoonGenerator.getSpawnee(p->getScene(), spawnpos);
                            if (act)
                            {
                                act->setFlipped(Seeder::getSharedSeeder().GetFloat(0.f,1.f)<0.5f);
                            }
                        }

                        break;
                    }
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::sendEventsWhenEnteringBrokenCageState()
    {
        sendActivationToAfterFX(btrue);
        sendSpawnElectoonsIfNotAlreadyDone();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Set state, and initialize the state we enter
    void Ray_AICageBehavior::setState(State_t _state)
    {
        if (_state!=m_state)
        {
            m_timeInCurrentState = 0;
            m_state = _state;
        }

        m_isLLockAnimPlaying = bfalse;
        m_isRLockAnimPlaying = bfalse;
        m_smashOpenTimer = -1;
        


        switch(_state)
        {
        case State_Preappears:
            m_sendActivationMessageToObjectsASAP = bfalse;
            break;
        case State_Stands:
        {
            m_animatedComponent->setAnim(CAGE_STAND_ANIM);
            m_lLockStep = 0;
            m_rLockStep = 0;
            m_lastLLockAnim = -1;
            m_lastRLockAnim = -1;
            m_sendActivationMessageToObjectsASAP = bfalse;
            sendActivationToAfterFX(bfalse);
            setStandInputs();
        } break;
        case State_Breaks: 
        {
            ITF_ASSERT_CRASH(m_cageAIComponent, "no cage AI");
            m_cageAIComponent->setBrokenInSession(btrue);
            m_animatedComponent->setAnim(CAGE_BREAKS_ANIM);

            CAMERACONTROLLERMANAGER->setIgnoreShakeCamFromCageBehaviour(btrue);
            CAMERACONTROLLERMANAGER->resetShake();
            CAMERACONTROLLERMANAGER->unregisterAllSubjects();

#ifdef ITF_SUPPORT_ONLINETRACKING

			u32 cage_index = m_cageAIComponent->getCageIndex();

			if (cage_index  == 4)
			{
				RAY_GAMEMANAGER->getOnlineTrackingManager()->m_finalBrokenCageIndex = cage_index;
			}
			else if (cage_index==0)
			{
				RAY_GAMEMANAGER->getOnlineTrackingManager()->m_hiddenBrokenCageIndex1 = cage_index;
			}
			else if (cage_index==1)
			{
				RAY_GAMEMANAGER->getOnlineTrackingManager()->m_hiddenBrokenCageIndex2 = cage_index;
			}
			
#endif

        } break;

        case State_Broken:
        {
            ITF_ASSERT_CRASH(m_cageAIComponent, "no cage AI");
            if (m_cageAIComponent->wasBrokenInSession())
                sendCloseCageDoor();
            m_animatedComponent->setAnim(CAGE_BROKEN_ANIM);
            sendEventsWhenEnteringBrokenCageState();

            CAMERACONTROLLERMANAGER->setIgnoreShakeCamFromCageBehaviour(bfalse);
            setLockOptOut(bfalse);

            //m_actor->disable();
        } break;

        }
    }

#ifdef ITF_SUPPORT_EDITOR
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AICageBehavior::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags  ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_cageAIComponent)
        {
            Vec3d projPos;
            GFX_ADAPTER->compute3DTo2D(m_actor->getPos(), projPos);
            String text;
            i32 cageIndex = m_cageAIComponent->getCageIndex();
            if (cageIndex>=0 && cageIndex<MEDAL_ELECTOON_COUNT)
                text.setTextFormat("Cage #%d", cageIndex);
            else
                text = "INVALID CAGE INDEX";

            GFX_ADAPTER->drawDBGText(text, Color::red(), projPos.m_x, projPos.m_y);
        }

        SafeArray<ObjectRef> electoonPlacements;        
        u32 count = getTemplate()->getElectoonCount();
        getElectoonTargets(electoonPlacements, count);
        count = electoonPlacements.size();
        for (u32 i=0; i<count; i++)
        {
            Actor *act = (Actor*)electoonPlacements[i].getObject();
            Ray_ElectoonTargetComponent *target = act->GetComponent<Ray_ElectoonTargetComponent>();
            if (target)
            {
                f32 normDX = act->getPos().m_x-m_actor->getPos().m_x;
                f32 dx;
                Vec3d target1,target2;

                dx = target->getDeltaXFromCageToTarget(normDX,target->getMinRange());
                target1 = act->getPos();
                target1.m_x = m_actor->getPos().m_x+dx;
                dx = target->getDeltaXFromCageToTarget(normDX,target->getMaxRange());
                target2 = act->getPos();
                target2.m_x = m_actor->getPos().m_x+dx;

                DebugDraw::line3D(target1, target2, Color::pink(), 10.0f);

                f32 minX = std::min(target1.m_x, target2.m_x);
                DebugDraw::text(Vec3d(minX, target1.m_y, target1.m_z), Vec2d(0, -17.0f), Color::pink(), Color::zero(), "electoons");
            }
        }
    }
#endif // ITF_SUPPORT_EDITOR

}
