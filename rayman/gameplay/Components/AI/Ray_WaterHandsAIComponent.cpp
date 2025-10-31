#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WATERHANDSAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_WaterHandsAIComponent.h"
#endif //_ITF_RAY_WATERHANDSAICOMPONENT_H_

#ifndef _ITF_RAY_HANDSAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_HandsAIComponent.h"
#endif //_ITF_RAY_HANDSAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PATCHCURVECOMPONENT_H_
#include "gameplay/Components/Misc/PatchCurveComponent.h"
#endif //_ITF_PATCHCURVECOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#define DIST_MIN            0.1f
#define RATIO_SMOOTH        0.1f

#define DIST_MIN_ALPHA      0.1f
#define DIST_MAX_ALPHA      1.0f

#define NB_TRAME_FOR_CHARGE 9 // Match with anim between MRK_FullOpen and MRK_Catch

#define DIST_CHECK_OVER_HAND	0.1f

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_WaterHandsAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WaterHandsAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("rangeMaxDetect", m_rangeMaxDetect);
        END_CONDITION_BLOCK()

		BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
			SERIALIZE_MEMBER("isDead",m_isDead);
		END_CONDITION_BLOCK()

    END_SERIALIZATION()



    BEGIN_VALIDATE_COMPONENT(Ray_WaterHandsAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_patchCurveComponent, "Ray_WaterHandsAIComponent requires a PatchCurveComponent");
    END_VALIDATE_COMPONENT()

    // ****************************************************************************

    Ray_WaterHandsAIComponent::Ray_WaterHandsAIComponent()
    : m_patchCurveComponent(NULL)
    , m_linkComponent(NULL)
    , m_animLightComponent(NULL)
    , m_rangeMaxDetect(20.0f)
    , m_friseSwim(NULL)
    , m_mustDie(bfalse)
    , m_isDying(bfalse)
	, m_isDead(bfalse)
	, m_typeAnimCount(TYPE_ANIM_COUNT)
	, m_handleFxRoot(U32_INVALID)
    {
        // none
    }

    // ****************************************************************************

    Ray_WaterHandsAIComponent::~Ray_WaterHandsAIComponent()
    {
        // none
    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get Components
        m_patchCurveComponent   = m_actor->GetComponent<PatchCurveComponent>();   // mandatory, validated
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        m_linkComponent         = m_actor->GetComponent<LinkComponent>();
        m_animLightComponent    = m_actor->GetComponent<AnimLightComponent>();


        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHandsRelease,579391720), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796), this);


        // Spawner
        SPAWNER->declareNeedsSpawnee(m_actor, &m_handsSpawner, getTemplate()->getSpawnHandPath());

		if (!getTemplate()->getRandomHands())
		{
			m_typeAnimCount = 1;
		}

    }

    // ****************************************************************************

	void Ray_WaterHandsAIComponent::onCheckpointLoaded()
	{
		Super::onCheckpointLoaded();

		if (m_isDead)
		{
			m_actor->disable();
		}
	}

	// ****************************************************************************

    void Ray_WaterHandsAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        PolyLine* swimPolyline = AIUtils::getPolylineRegion(m_actor->getDepth(), s_RegionName_Swim, m_actor->get2DPos());
        if (swimPolyline)
        {   
            //ITF_ASSERT_CRASH(swimPolyline->m_ownerFrise, "WaterHands must be in water frise.");  
            m_friseSwim = swimPolyline->m_ownerFrise;
        }
        else
        {
            //ITF_WARNING(m_actor, 0, "This object must be in water !");
        }

		// Start FX bubbles
		if (m_handleFxRoot == U32_INVALID)
		{
			m_handleFxRoot = playFx(m_actor->getPos());
		}
    }

	// ****************************************************************************

	void Ray_WaterHandsAIComponent::onBecomeInactive()
	{
		if (m_handleFxRoot != U32_INVALID)
		{
			stopFx(m_handleFxRoot);
			m_handleFxRoot = U32_INVALID;
		}
	}

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::onResourceReady()
    {
        // PrepareAttack for link attack
        prepareAttack();
    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (!m_friseSwim)
        {
            PolyLine* swimPolyline = AIUtils::getPolylineRegion(m_actor->getDepth(), s_RegionName_Swim, m_actor->get2DPos());
            if (swimPolyline)
            {   
                m_friseSwim = swimPolyline->m_ownerFrise;
            }
        }


        updateDetect(_dt);
        updateAttack(_dt);
        updateRotation(_dt);

        updateDeath();

#ifndef ITF_FINAL
        // Debug
        //updateDebug(_dt);
#endif // ITF_FINAL

        growAABB();
    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::updateDeath()
    {
        if (!m_mustDie || m_isDying)
        {
            return;
        }

        setHealth(0);
       if (m_listAttack.size() == 0)
       {
           m_isDying = btrue;
		   m_isDead = btrue;

           // Play anim death
           if (m_animLightComponent)
           {
               const StringID animDeath = ITF_GET_STRINGID_CRC(Death,2142372323);
               m_animLightComponent->setAnim(animDeath);
           }           
       }
    
    }

    // ****************************************************************************

    bbool Ray_WaterHandsAIComponent::isAttackable(Vec2d _pos)
    {
    	PolyLine* enemySwimPolyline = AIUtils::getPolylineRegion(m_actor->getDepth(), s_RegionName_Swim, _pos);

        if (!enemySwimPolyline)
        {
            return bfalse;
        }

        if (m_friseSwim != enemySwimPolyline->m_ownerFrise)
        {
            return bfalse;
        }

        f32 rangeSqr = (_pos - m_actor->get2DPos()).sqrnorm();
        if (rangeSqr > (m_rangeMaxDetect * m_rangeMaxDetect))
        {
            return bfalse;
        }

        return btrue;
    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::updateDetect( f32 _dt )
    {

        if (m_mustDie)
        {
            return;
        }

        ActorList targetList;
        AIUtils::getPlayers(targetList);

        for (u32 i = 0; i < targetList.size(); i++)
        { 
            Actor* detectEnemy         = targetList[i];
            ActorRef detectEnemyRef    = detectEnemy->getRef();

            if (isAttackable(detectEnemy->get2DPos()) && !queryIsDead(detectEnemy) && !queryIsCaught(detectEnemy))
            {
                // Detect !

                bbool foundAttack = bfalse;

                // Search an current attack for this player
                for (u32 j = 0; j < m_listAttack.size(); j++) 
                {
                    SlotAttack& slot = m_listAttack[j];
                    Actor* handAct = slot.m_handRef.getActor();

                    if (!handAct)
                    {
                        continue;
                    }

                    if (slot.m_playerRef == detectEnemyRef)
                    {           
                        // Refresh attack !
                        foundAttack = btrue;
                        changeState(slot, STATE_ATTACK_FW); 
                        break;
                    }                    
                }

                u32 idLinkAttack = U32_INVALID;
                f32 distMinWithFreeAttack = F32_INFINITY;

                if (!foundAttack)
                { 
                    // Search an free link attack for this player
                    for (u32 j = 0; j < m_listAttack.size(); j++) 
                    {
                        SlotAttack& slot = m_listAttack[j];
                        Actor* handAct = slot.m_handRef.getActor();

                        if (!handAct)
                        {
                            continue;
                        }

                        if (!slot.m_playerRef.isValid())
                        {           
                            f32 sqrDist = (detectEnemy->get2DPos() - slot.m_handRef.getActor()->get2DPos()).sqrnorm();
                            if (sqrDist < distMinWithFreeAttack)
                            {
                                foundAttack = btrue;
                                distMinWithFreeAttack = sqrDist;
                                idLinkAttack = j;
                            }   
                        }                    
                    }
                }

                if (!foundAttack)
                {
                    newSpawnAttack(detectEnemy);
                }
                else if (foundAttack && idLinkAttack != U32_INVALID)
                {
                    SlotAttack& slotLinkAttack = m_listAttack[idLinkAttack];
                    slotLinkAttack.m_playerRef = detectEnemyRef;
                    changeState(slotLinkAttack, STATE_ATTACK_FW);
                }

            } 
        }
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::prepareAttack()
    {

        // clear
        m_listAttack.clear();
        m_actor->getBinding()->unbindAllChildren();

        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            const u32 childrenCount = children.size();

            for (u32 i = 0; i < childrenCount; i++)
            {
                if ( Pickable* child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath()) )
                {
                    Actor* actorChild = child->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if (actorChild)
                    {                       
                        newLinkAttack(actorChild);
                    }
                }
            }
        }        
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::newSpawnAttack(Actor* _player)
    {
        if (_player)
        {
            Vec3d posCreate = m_actor->getPos();
            posCreate.m_z += 0.01f; 

            Actor* actHand = m_handsSpawner.getSpawnee(m_actor->getScene(), posCreate);
            ITF_WARNING(m_actor, actHand, "I can't spawn my WaterHands actor !");

            if (actHand)
            {
                // Bind handActor to force update order + for linkCurveComponent
                m_actor->getBinding()->bindChild(actHand->getRef());

                SlotAttack newSlotAttack;
                newSlotAttack.m_isAttackSpawned = btrue;
                newSlotAttack.m_playerRef  = _player->getRef();
                newSlotAttack.m_handRef    = actHand->getRef(); 
                newSlotAttack.m_targetPos  = _player->get2DPos(); 

                if (getTemplate()->getLineareMode())
                {
                    newSlotAttack.m_RootPos = Vec2d(_player->get2DPos().m_x, m_actor->get2DPos().m_y);
                    newSlotAttack.m_handleFx = playFx(newSlotAttack.m_RootPos.to3d());
                }
                else
                {
                    newSlotAttack.m_RootPos = m_actor->get2DPos();
                }

                newSlotAttack.m_targetIntermedA         = newSlotAttack.m_RootPos;
                newSlotAttack.m_targetAngle             = getAngle(actHand, _player);
                newSlotAttack.m_targetAngleIntermedA    = 0.0f;

                actHand->set2DPos(newSlotAttack.m_RootPos);

                // Random type anim
                newSlotAttack.m_typeAnim = Seeder::getSharedSeeder().GetU32(0, m_typeAnimCount - 1);

                m_listAttack.push_back(newSlotAttack);
                changeState(m_listAttack.back(), STATE_ATTACK_FW);

            }
        }
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::newLinkAttack(Actor* _handChild)
    {
        if (_handChild)
        {
            // Bind handActor to force update order + for linkCurveComponent
            m_actor->getBinding()->bindChild(_handChild->getRef());

            SlotAttack newSlotAttack;
            newSlotAttack.m_isAttackSpawned = bfalse;
            newSlotAttack.m_playerRef.invalidate();
            newSlotAttack.m_handRef    = _handChild->getRef(); 
            newSlotAttack.m_targetPos  = _handChild->get2DPos();
            newSlotAttack.m_RootPos    = m_actor->get2DPos();

            newSlotAttack.m_targetIntermedA         = _handChild->get2DPos();
            newSlotAttack.m_targetAngle             = 0.0f;
            newSlotAttack.m_targetAngleIntermedA    = 0.0f;

            _handChild->set2DPos(_handChild->get2DPos());

            // Random type anim
            newSlotAttack.m_typeAnim = Seeder::getSharedSeeder().GetU32(0, m_typeAnimCount - 1);

            m_listAttack.push_back(newSlotAttack);
            changeState(m_listAttack.back(), STATE_LINKATTACK_DISABLED);
        }
    }

    //*****************************************************************************
   
    u32 Ray_WaterHandsAIComponent::getHP( Actor* _player )
    {
        Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayerFromActor(_player->getRef()));

        if (player)
        {
            return player->getHitPoints();
        }

        return 0;
    }

    //*****************************************************************************



    bbool Ray_WaterHandsAIComponent::queryIsDead( Actor* _player )
    {
        EventQueryIsDead query;
        _player->onEvent(&query);
        return query.getIsDead();
    }

    //*****************************************************************************

    bbool Ray_WaterHandsAIComponent::queryIsCaught( Actor* _player )
    {
        EventQueryIsCaught query;
        _player->onEvent(&query);
        return query.getIsCaught();
    }

    //*****************************************************************************

    u32 Ray_WaterHandsAIComponent::playFx(Vec3d _pos)
    {
        const StringID fxName = ITF_GET_STRINGID_CRC(Waterhands,3509689363);

        if (m_fxControllerComponent)
        {
            // Offset
            _pos.m_y -= 0.3f;   

            u32 fxHandle = m_fxControllerComponent->playFX(fxName);
            m_fxControllerComponent->setFXPos(fxHandle,_pos);
            return fxHandle;
        }

        return U32_INVALID;
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::stopFx(u32 _indexFx)
    {
        ITF_ASSERT(_indexFx != U32_INVALID);

        if (m_fxControllerComponent && _indexFx != U32_INVALID)
        {
            m_fxControllerComponent->stopFX(_indexFx);
        }
    }

    //*****************************************************************************

    bbool Ray_WaterHandsAIComponent::handIsFree( u32 _index )
    {
        if (_index < m_listAttack.size())
        {
            SlotAttack& slot = m_listAttack[_index];

            if (slot.m_hasCatch)
            {
                return bfalse;
            }
        }
        return btrue;
    }

    //*****************************************************************************

    bbool Ray_WaterHandsAIComponent::playerIsCatchable( Actor* _player )
    {
        if (queryIsDead(_player))
        {
            return bfalse;
        }

        if (queryIsCaught(_player))
        {
            return bfalse;
        }

        ActorRef playerRef = _player->getRef();

        if (playerRef.isValid())
        {
            for (u32 i = 0; i < m_listAttack.size(); i++)
            {
                SlotAttack& slot = m_listAttack[i];

                if (slot.m_playerRef == playerRef && slot.m_hasCatch)
                {
                    return bfalse;
                }
            }
        }

        return btrue;
    }

    //*****************************************************************************

    u32 Ray_WaterHandsAIComponent::getSlotIndexByHand(Actor* _hand)
    {
        if (_hand)
        {
            ActorRef handRef = _hand->getRef();

            for (u32 i = 0; i < m_listAttack.size(); i++)
            {
                SlotAttack& slot = m_listAttack[i];

                if (slot.m_handRef == handRef)
                {
                    return i;
                }
            }
        }

        return U32_INVALID;
    }

    //*****************************************************************************

    u32 Ray_WaterHandsAIComponent::getSlotIndexByTarget(Actor* _target)
    {
        if (_target)
        {
            ActorRef targetRef = _target->getRef();

            for (u32 i = 0; i < m_listAttack.size(); i++)
            {
                SlotAttack& slot = m_listAttack[i];

                if (slot.m_playerRef == targetRef)
                {
                    return i;
                }
            }
        }

        return U32_INVALID;
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::launchFinalAttack(Actor* _hand, Actor* _target)
    {
        if (!playerIsCatchable(_target))
        {
            return;
        }

        u32 slotIndex = getSlotIndexByHand(_hand);

        if (slotIndex != U32_INVALID)
        {
            if (!handIsFree(slotIndex))
            {
                return;
            }

            switchTarget(slotIndex, _target);
            lockAttack(slotIndex);

            SlotAttack& slot = m_listAttack[slotIndex];
            changeState(slot, STATE_ATTACK_ANTICIP);
        }
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::switchTarget(u32 _indexSlot, Actor* _newTarget)
    {
        if (_newTarget)
        {
            ActorRef newTargetRef = _newTarget->getRef();
            SlotAttack& slot = m_listAttack[_indexSlot];
            
            if (slot.m_playerRef == newTargetRef)
            {
                // It's already this target
                return;
            }

            u32 indexOldSlot = getSlotIndexByTarget(_newTarget);
            if (indexOldSlot != U32_INVALID)
            {
                // swicth target
                SlotAttack& oldSlot = m_listAttack[indexOldSlot];

                if (slot.m_playerRef.isValid())
                {
                    oldSlot.m_playerRef = slot.m_playerRef;
                }
                else
                {
                    oldSlot.m_playerRef.invalidate();
                     oldSlot.m_mustReturn = btrue;
                }               
            }

            // New target for this attack
            slot.m_playerRef = _newTarget->getRef();
        }
    
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::lockAttack(u32 _indexSlot)
    {
        // TODO :  mettre assert a chaque fois indexSlot -> size tab
        SlotAttack& slot = m_listAttack[_indexSlot];
        slot.m_hasCatch = btrue;
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::chargeOnPlayer(Actor* _hand)
    {
        u32 slotIndex = getSlotIndexByHand(_hand);

        if (slotIndex != U32_INVALID)
        {
            SlotAttack& slot = m_listAttack[slotIndex];
            changeState(slot, STATE_ATTACK_CHARGE);
        }   
    }

    //*****************************************************************************

    bbool Ray_WaterHandsAIComponent::catchPlayer(Actor* _hand)
    {
        u32 slotIndex = getSlotIndexByHand(_hand);

        if (slotIndex != U32_INVALID)
        {
            SlotAttack& slot = m_listAttack[slotIndex];
            changeState(slot, STATE_ATTACK_WAIT_CAUGHT);
        }  

        return bfalse;
    }

    //*****************************************************************************

    void Ray_WaterHandsAIComponent::updateRotation( f32 _dt )
    {
        f32 ratioAngle = 0.05f;

        for (u32 i = 0; i < m_listAttack.size(); i++)
        {
            SlotAttack& slot = m_listAttack[i];

            Actor* handActor   = slot.m_handRef.getActor();
            Actor* playerActor = slot.m_playerRef.getActor();

            if (!handActor)
            {
                continue;
            }

            if (playerActor && isAttackable(playerActor->get2DPos()) && slot.m_state == STATE_ATTACK_FW)
            {
                slot.m_targetAngle = handActor->getAngle() + getAngle(handActor, playerActor);
            }
            else if (slot.m_state == STATE_ATTACK_BW || slot.m_state == STATE_ATTACK_CATCH_BW)
            {
                slot.m_targetAngle = getBezierAngle(slot); 
            }
            else if (slot.m_state == STATE_ATTACK_ANTICIP || slot.m_state == STATE_ATTACK_CHARGE)
            {
                ratioAngle = 1.0f;
                slot.m_targetAngle = handActor->getAngle() + getAngle(handActor, playerActor);
            } 
            else if (!slot.m_isAttackSpawned && (slot.m_state == STATE_LINKATTACK_BW || slot.m_state == STATE_LINKATTACK_DISABLED))
            {
                slot.m_targetAngle = handActor->getBoundWorldInitialAngle();                
            }

            slot.m_targetAngleIntermedA += getShortestAngleDelta(slot.m_targetAngleIntermedA, slot.m_targetAngle) * ratioAngle;
            handActor->setAngle(slot.m_targetAngleIntermedA);

        }
    }

     //*****************************************************************************

    void Ray_WaterHandsAIComponent::updateAttack( f32 _dt )
    {
        for (u32 i = 0; i < m_listAttack.size(); i++)
        {
            SlotAttack& slot    = m_listAttack[i];
            Actor* handActor    = slot.m_handRef.getActor();
            Actor* playerActor  = slot.m_playerRef.getActor();

            if (!handActor)
            {
                continue;
            }

            Vec2d curHandPos = handActor->get2DPos();
            AnimLightComponent* animHand = handActor->GetComponent<AnimLightComponent>();

            switch(slot.m_state)
            {
                case STATE_LINKATTACK_DISABLED:
                    {
                        idleSin(slot, _dt);
                        break;
                    }

                case STATE_ATTACK_FW:


					if (playerActor && (playerActor->get2DPos() - handActor->get2DPos()).norm() < DIST_CHECK_OVER_HAND)
					{
						// Launch attack if player is exactly on hand
						launchFinalAttack(handActor, playerActor);
					}
                    else if (playerActor && !playerIsCatchable(playerActor))
                    {
                        changeState(slot, STATE_ATTACK_BW);
                    }
                    else if (playerActor && isAttackable(playerActor->get2DPos()))
                    {
                        // Player is alway in water
                        if (!moveHand(slot, _dt, RATIO_SMOOTH))
                        {
                            // miss attack
                            changeState(slot, STATE_ATTACK_BW);
                        }
                    } 
                    else
                    {
                        // Player is out of water -> stop attack
                        changeState(slot, STATE_ATTACK_WAIT);
                    }
                    break;
        
                // *****************************

                case STATE_ATTACK_WAIT:

                    if (slot.m_time > 0.0f)
                    {
                        moveHand(slot, _dt, RATIO_SMOOTH);
                        slot.m_time -= _dt;
                    }
                    else
                    {
                        if (slot.m_isAttackSpawned)
                        {
                            changeState(slot, STATE_ATTACK_BW);
                        }
                        else
                        {
                            changeState(slot, STATE_LINKATTACK_BW);
                        }
                    }
                    break;

                // *****************************

                case STATE_ATTACK_BW:

                    if (!moveHand(slot, _dt, RATIO_SMOOTH))
                    {
                        changeState(slot, STATE_ATTACK_DESTROY);
                    }

                    break;

                 // *****************************

                case STATE_LINKATTACK_BW:

                    if (!moveHand(slot, _dt, RATIO_SMOOTH))
                    {
                        changeState(slot, STATE_LINKATTACK_DISABLED);
                    }

                    break;

                    
                // *****************************

                case STATE_ATTACK_ANTICIP:
                    {

                        Vec2d dir = slot.m_playerRef.getActor()->get2DPos() -  handActor->get2DPos();
                        dir.normalize();
                        slot.m_targetPos = handActor->get2DPos() - dir * 0.5f; // TODO : expose dist recul ?


                        moveHand(slot, _dt, RATIO_SMOOTH);

                        if(slot.m_time > 0.0f)
                        {
                            slot.m_time -= _dt;
                        }
                        else
                        {
                            changeState(slot, STATE_ATTACK_CHARGE);
                        }

                        break;
                    }

                    // *****************************

                case STATE_ATTACK_CHARGE:
                    {
                        if (slot.m_countTrameCharge > 0)                        
                        {
                            Vec2d relativeBonesTarget = getPosHandbySnapBone(slot);
                            Vec2d posHand = slot.m_handRef.getActor()->get2DPos();

                            f32 sqrCheckDist = (relativeBonesTarget-slot.m_posStartCharge).sqrnorm();
                            
#ifndef ITF_FINAL
                            //DebugDraw::circle(relativeBonesTarget, 0.0f, 0.2f, Color::red());
                            //DebugDraw::line2D(relativeBonesTarget, slot.m_posStartCharge, 0.0f, Color::red(), 6.0f);
#endif // ITF_FINAL

                            f32 distMaxCharge = getTemplate()->getDistMaxCharge();
                            f32 sqrDistMaxCharge = distMaxCharge * distMaxCharge;
                            if(sqrCheckDist > sqrDistMaxCharge)
                            {
                                slot.m_missAttack = btrue;
                                changeState(slot, STATE_ATTACK_CHARGE_MISS);
                                break;
                            }


                            Vec2d dir = posHand - relativeBonesTarget;
                            dir.normalize(); 

                            f32 distance = (slot.m_distCharge * slot.m_countTrameCharge);
                            Vec2d newPosHand = relativeBonesTarget + (dir * distance);
                            slot.m_countTrameCharge--;

                            slot.m_handRef.getActor()->set2DPos(newPosHand);

                        }  

                        break; 
                    }

                    // *****************************

                case STATE_ATTACK_CHARGE_MISS:
                    {
                        if (slot.m_countTrameCharge > 0)                        
                        {
                            Vec2d posHand = slot.m_handRef.getActor()->get2DPos();

                            Vec2d dir = posHand - slot.m_targetPos;
                            dir.normalize();

                            f32 distance = (slot.m_distCharge * slot.m_countTrameCharge);
                            Vec2d newPosHand = slot.m_targetPos + (dir * distance);
                            slot.m_countTrameCharge--;

                            slot.m_handRef.getActor()->set2DPos(newPosHand);
                        }  

                        break; 
                    }


                // *****************************

                case STATE_ATTACK_WAIT_CAUGHT:

                    if (slot.m_time > 0.0f)
                    {
                        moveHand(slot, _dt, RATIO_SMOOTH);

                        if (!slot.m_missAttack)
                        {
                            if (slot.m_isKillAttack)
                            {
                                snapPlayerCaught(slot);
                            }
                        }

                        slot.m_time -= _dt;
                    }
                    else
                    {
                        changeState(slot, STATE_ATTACK_CATCH_BW);
                    }
                    break;

                // *****************************

                case STATE_ATTACK_CATCH_BW:
                    {
                        if (moveHand(slot, _dt, RATIO_SMOOTH))
                        {
                            if (slot.m_isKillAttack)
                            {
                                // Player is catch
                                snapPlayerCaught(slot);
                            }
                        }
                        else
                        {
                            if (slot.m_isKillAttack)
                            {
                                // Kill player
                                Ray_EventInstantKill eventKill;
                                eventKill.setSender(m_actor->getRef());
                                playerActor->onEvent(&eventKill);
                            }

                            if (slot.m_isAttackSpawned)
                            {
                                changeState(slot, STATE_ATTACK_DESTROY);
                            }
                            else
                            {
                                slot.m_playerRef.invalidate();
                                changeState(slot, STATE_LINKATTACK_BW);
                            }
                        }
                   
                        break;
                    }

                case STATE_ATTACK_DESTROY:
                    {
                        bbool isSpawnedAttack = slot.m_isAttackSpawned;

                        if (isSpawnedAttack)
                        {
                            if (getTemplate()->getLineareMode())
                            {
                                stopFx(slot.m_handleFx);
                            }

                            // return finish
                            m_listAttack.removeAt(i);
                            handActor->requestDestruction();
                        }

                        break;
                    }
            }

        
            // Compute alpha
            if (animHand && slot.m_isAttackSpawned)
            {
                f32 distFromRoot = (slot.m_RootPos - handActor->get2DPos()).norm();

                if (distFromRoot >= DIST_MIN_ALPHA && distFromRoot < DIST_MAX_ALPHA)
                {
                    slot.m_alpha = distFromRoot / (DIST_MAX_ALPHA - DIST_MIN_ALPHA);
                     
                } 
                else if (distFromRoot < DIST_MIN_ALPHA)
                {
                    slot.m_alpha = 0.0f;
                }
                else
                {
                    slot.m_alpha = 1.0f;
                } 

                animHand->setAlpha(slot.m_alpha);  
            }

#ifndef ITF_FINAL
            // Debug
            //DebugDraw::circle(slot.m_targetPos, 0.0f, 0.2f, Color::red());
            //DebugDraw::circle(slot.m_targetIntermedA, 0.0f, 0.2f, Color::blue());
#endif // ITF_FINAL
        }
    }
    
    // ****************************************************************************

    void Ray_WaterHandsAIComponent::changeState( SlotAttack& _slot,  u32 _newState )
    {

        Actor* handAct = _slot.m_handRef.getActor();
        Actor* targetAct = _slot.m_playerRef.getActor();

        //ITF_ASSERT(handAct && targetAct);
        //if (!handAct || !targetAct)
        //{
        //    return;
        //}

        switch (_newState)
        {

        case STATE_LINKATTACK_DISABLED:
            {
                _slot.m_targetPos = handAct->get2DPos();
                _slot.m_time = 0.0f;
                _slot.m_hasCatch = bfalse;
                _slot.m_mustReturn = bfalse;
                _slot.m_missAttack = bfalse;
                _slot.m_playerRef.invalidate();
                playAnim(_slot, _newState);
                _slot.m_state = _newState;
                break;
            }

        case STATE_ATTACK_FW:
        {
            if (_slot.m_state != STATE_ATTACK_ANTICIP && _slot.m_state != STATE_ATTACK_CHARGE && !_slot.m_hasCatch && !_slot.m_mustReturn)
            {
                if (_slot.m_state != _newState)
                { 
                    playAnim(_slot, _newState);
                }
 
                _slot.m_targetPos = targetAct->get2DPos();
                _slot.m_state = _newState;
            }
            break;
        }

        case STATE_ATTACK_WAIT:
            {
                _slot.m_targetPos   = _slot.m_targetIntermedA;
                _slot.m_time        = getTemplate()->getTimeWaitOnPlayerEscape();
                _slot.m_state       = _newState;

                playAnim(_slot, _newState);
                break;
            }

        case STATE_ATTACK_ANTICIP:
            {
                _slot.m_targetPos = handAct->get2DPos();

                _slot.m_posStartCharge = handAct->get2DPos();

                _slot.m_state = _newState;  
                _slot.m_time = getTemplate()->getTimeAnticip();
                playAnim(_slot, _newState);
                break;
            }

        case STATE_ATTACK_CHARGE:
            {
                f32 dist = (handAct->get2DPos() - getPosHandbySnapBone(_slot)).norm();
                _slot.m_distCharge = dist / NB_TRAME_FOR_CHARGE;
                _slot.m_countTrameCharge = NB_TRAME_FOR_CHARGE;

               // _slot.m_posStartCharge = handAct->get2DPos();

                _slot.m_state = _newState;   
                playAnim(_slot, _newState);
                break;
            }

        case STATE_ATTACK_CHARGE_MISS:
            {
                
                f32 distMaxCharge = getTemplate()->getDistMaxCharge();

                _slot.m_distCharge = distMaxCharge / _slot.m_countTrameCharge;

                Vec2d relativeBonesTarget = getPosHandbySnapBone(_slot);
                Vec2d posHand = _slot.m_handRef.getActor()->get2DPos();
                Vec2d dir = posHand - relativeBonesTarget;
                dir.normalize(); 
                //_slot.m_targetPos = posHand - (dir * DIST_CHARGE_MAX);
                _slot.m_targetPos = _slot.m_posStartCharge - (dir * distMaxCharge);

                
                _slot.m_state = _newState;   
                break;
            }

        case STATE_ATTACK_WAIT_CAUGHT: 
            {
                _slot.m_time        = getTemplate()->getTimeWaitAfterCaught();
                _slot.m_state       = _newState;
                _slot.m_targetIntermedA = handAct->get2DPos();

                if (_slot.m_missAttack)
                {
                    _slot.m_isKillAttack = bfalse;
                }
                else
                {
                    _slot.m_targetPos   = targetAct->get2DPos();

                    u32 playerHP = getHP(targetAct);

                    if (playerHP == 1)
                    {
                        _slot.m_isKillAttack = btrue;

                        // Event catch player (+ stop control)
                        Ray_EventHandsCaught eventCaughtPlayer; 
                        eventCaughtPlayer.setSender(m_actor->getRef());
                        targetAct->onEvent(&eventCaughtPlayer); 
                    }
                    else
                    {        
                        PunchStim punchStim;

                        punchStim.setReceivedHitType(RECEIVEDHITTYPE_EJECTXY);
                        punchStim.setIsRadial(bfalse);
                        punchStim.setDirection(Vec2d::Up.Rotate(handAct->getAngle()));
                        punchStim.setFxPos(targetAct->getPos());
                        punchStim.setLevel(0);
                        punchStim.setSender(m_actor->getRef());
                        punchStim.setAngle(handAct->getAngle());
                        punchStim.setPos(handAct->get2DPos());
                        punchStim.setFaction(getFaction());

                        targetAct->onEvent(&punchStim);

                        _slot.m_isKillAttack = bfalse;
                    }
                }



                break;
            }

        case STATE_ATTACK_BW:
            {
                _slot.m_targetPos = _slot.m_RootPos;
                _slot.m_state     = _newState;

                playAnim(_slot, _newState);
                break;
            }

        case STATE_LINKATTACK_BW:
            {
                _slot.m_targetPos = handAct->getWorldInitialPos().truncateTo2D();
                _slot.m_state     = _newState;

                playAnim(_slot, _newState);
                break;
            }


        case STATE_ATTACK_CATCH_BW: 
            {
                _slot.m_targetPos = _slot.m_RootPos;
                _slot.m_state     = _newState;
                break;
            }

        case STATE_ATTACK_DESTROY:
            _slot.m_state = _newState;
            break;
        }
    }

    // ****************************************************************************
    
    void Ray_WaterHandsAIComponent::snapPlayerCaught( SlotAttack& _slot )
    {
        const StringID boneCatchName = ITF_GET_STRINGID_CRC(B_SnapTarget,1217905540);
     
        Actor* playerCaught = _slot.m_playerRef.getActor();

        AnimLightComponent* animHand = _slot.m_handRef.getActor()->GetComponent<AnimLightComponent>();
        if (animHand)
        {
            u32 boneIndex = animHand->getBoneIndex(boneCatchName);
            if (boneIndex != U32_INVALID)
            {
                Vec3d bonePos;
                if (animHand->getBonePos(boneIndex, bonePos))
                {
                    playerCaught->set2DPos(bonePos.truncateTo2D());                    
                    playerCaught->setAngle(_slot.m_handRef.getActor()->getAngle());
                }
            }                         
        }
    } 

    // ****************************************************************************

    Vec2d Ray_WaterHandsAIComponent::getPosHandbySnapBone( SlotAttack& _slot )
    {
        const StringID boneCatchName = ITF_GET_STRINGID_CRC(B_SnapTarget,1217905540);

        Vec2d vecRes;
        Actor* playerCaught = _slot.m_playerRef.getActor();        
        Actor* hand = _slot.m_handRef.getActor();

        if (!playerCaught)
        {
            return hand->get2DPos();
        }

        AnimLightComponent* animHand = _slot.m_handRef.getActor()->GetComponent<AnimLightComponent>();
        if (animHand)
        {
            u32 boneIndex = animHand->getBoneIndex(boneCatchName);
            if (boneIndex != U32_INVALID)
            {
                Vec3d bonePos;
                if (animHand->getBonePos(boneIndex, bonePos))
                {
                    Vec2d offsetPos = bonePos.truncateTo2D() - hand->get2DPos();
                    vecRes = playerCaught->get2DPos() - offsetPos;
                }
            }                         
        }

        return vecRes;
    }

    // ****************************************************************************


    void Ray_WaterHandsAIComponent::playAnim( SlotAttack& _slot, u32 _state)
    {
        const StringID animIdle01      = ITF_GET_STRINGID_CRC(Stand01,147266583);
        const StringID animAnticip01   = ITF_GET_STRINGID_CRC(Anticip01,1259266327);
        const StringID animCatch01     = ITF_GET_STRINGID_CRC(Catch01,2262032280);

        const StringID animIdle02      = ITF_GET_STRINGID_CRC(Stand02,55218973);
        const StringID animAnticip02   = ITF_GET_STRINGID_CRC(Anticip02,830293921);
        const StringID animCatch02     = ITF_GET_STRINGID_CRC(Catch02,2971914741);

        const StringID animIdle03      = ITF_GET_STRINGID_CRC(Stand03,3582228742);
        const StringID animAnticip03   = ITF_GET_STRINGID_CRC(Anticip03,1672889851);
        const StringID animCatch03     = ITF_GET_STRINGID_CRC(Catch03,1985304148);


        AnimLightComponent* animComponentHand = _slot.m_handRef.getActor()->GetComponent<AnimLightComponent>();

        if (animComponentHand)
        {
            if (_slot.m_typeAnim == TYPE_ANIM_MOUTH)
            {
                if (_state == STATE_ATTACK_ANTICIP)
                {
                    animComponentHand->setAnim(animAnticip01);
                }
                else if (_state == STATE_ATTACK_CHARGE)
                {
                    animComponentHand->setAnim(animCatch01);
                }
                else if (_state == STATE_ATTACK_CATCH_BW)
                {
                     animComponentHand->setAnim(animCatch01);
                }
                else
                {
                    animComponentHand->setAnim(animIdle01);
                }
            }
            else if (_slot.m_typeAnim == TYPE_ANIM_HAND)
            {
                if (_state == STATE_ATTACK_ANTICIP)
                {
                    animComponentHand->setAnim(animAnticip02);
                }
                else if (_state == STATE_ATTACK_CHARGE)
                {
                    animComponentHand->setAnim(animCatch02);
                }
                else if (_state == STATE_ATTACK_CATCH_BW)
                {
                    animComponentHand->setAnim(animCatch02);
                }
                else
                {
                    animComponentHand->setAnim(animIdle02);
                }
            }
            else if (_slot.m_typeAnim == TYPE_ANIM_HAND_2)
            {
                if (_state == STATE_ATTACK_ANTICIP)
                {
                    animComponentHand->setAnim(animAnticip03);
                }
                else if (_state == STATE_ATTACK_CHARGE)
                {
                    animComponentHand->setAnim(animCatch03);
                }
                else if (_state == STATE_ATTACK_CATCH_BW)
                {
                    animComponentHand->setAnim(animCatch03);
                }
                else
                {
                    animComponentHand->setAnim(animIdle03);
                }
            }
           
        }
    }

    // ****************************************************************************


    f32 Ray_WaterHandsAIComponent::getAngle(Actor* _source, Actor* _dest)
    {

        Vec2d posTarget = _dest->get2DPos();
        Vec2d vecToTarget = (posTarget - _source->get2DPos());

        // Actor orientation
        Vec2d dirActor = Vec2d::Up.Rotate(_source->getAngle()); // Up : because hand's root is oriented right 
        vecToTarget.normalize();

        // Compute angle
        f32 angle = vecToTarget.getOrientedAngle(dirActor); 

        //if (m_actor->isFlipped()) // _source
        //{
        //    angle = MTH_PI - angle;
        //} 

        return angle;

    }

    // ****************************************************************************

    f32 Ray_WaterHandsAIComponent::getBezierAngle( SlotAttack& _slot )
    {

        const Vec2d p0 = _slot.m_RootPos;
        const Vec2d n0 = m_patchCurveComponent->getParentAttachNormal();
        const Vec2d p3 = _slot.m_handRef.getActor()->get2DPos();

        Vec2d n1 = (p0 - p3);
        f32 length = n1.norm();
        n1 *= 1 / length;

        const Vec2d p1 = p0 + n0 * length * 0.5f;
        const Vec2d p2 = p3 + n1 * length * 0.5f;

        Vec2d tangent = getBezierTangent(p0, p1, p2, p3, 1.0f);
        f32 angle = tangent.normalize().getAngle() - MTH_PIBY2;

        return angle;

    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::idleSin( SlotAttack& _slot, f32 _dt)
    {
        Actor* hand =  _slot.m_handRef.getActor();
        Vec2d pos = _slot.m_targetPos;

        pos += Vec2d(0.0f, 0.1f) * (f32_Sin(_slot.m_time * 2.0f));    

        hand->set2DPos(pos);

        _slot.m_time += _dt;

    }

    // ****************************************************************************


    bbool Ray_WaterHandsAIComponent::moveHand( SlotAttack& _slot, f32 _dt, f32 _ratio)
    {

        Actor* handActor = _slot.m_handRef.getActor();

        Vec2d targetFinal   = _slot.m_targetPos;
        Vec2d curPos        = handActor->get2DPos();

        f32 speedMax = 0.0f;
        if (_slot.m_state == STATE_ATTACK_FW)
        {
            speedMax = getTemplate()->getSpeedAttack();
        }
        else
        {
            speedMax = getTemplate()->getSpeedReturn();
        }
        f32 distMax =  speedMax * _dt;
        f32 distMaxSqr = distMax * distMax;

        Vec2d targetInterm = Interpolate(_slot.m_targetIntermedA, targetFinal, _ratio);

        f32 distIntermSqr = (_slot.m_targetIntermedA - targetInterm).sqrnorm();
        if (distIntermSqr > distMaxSqr)
        {
            targetInterm = _slot.m_targetIntermedA + ((targetFinal - _slot.m_targetIntermedA).normalize() * distMax);
        }

        // Move : Compute New Pos
        Vec2d newPos = Interpolate(curPos, targetInterm, _ratio);

#ifndef ITF_FINAL
        //DebugDraw::circle(targetInterm, 0.0f, 0.1f, Color::red(), 1.0f);
        //DebugDraw::circle(newPos, 0.0f, 0.1f, Color::cyan(), 0.0f);
#endif // ITF_FINAL

        f32 distance = (newPos - targetFinal).norm();
        if(( _slot.m_state == STATE_ATTACK_FW /*&& isAttackable(newPos)*/) || distance > DIST_MIN)
        {
            _slot.m_targetIntermedA = targetInterm;
            handActor->set2DPos(newPos);
            return btrue; 
        }

        return bfalse;
    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        const StringID eventID       = ITF_GET_STRINGID_CRC(WaterHand_CatchPlayer,2981687535);
        const StringID s_MRKCatch    = ITF_GET_STRINGID_CRC(MRK_Catch,14748549);
        const StringID s_MRKFullopen = ITF_GET_STRINGID_CRC(MRK_FullOpen,3291432407);

        if (EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {         
            ActorRef handSender = trigger->getSender();
            ActorRef player     = trigger->getActivator();
            launchFinalAttack(handSender.getActor(), player.getActor());
        } 
        if (AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            ActorRef handSender = animEvent->getSender();

            //if (animEvent->getName() == s_MRKFullopen)
            //{
            //    chargeOnPlayer(handSender.getActor());
            //}
            //else 
            
            if (animEvent->getName() == s_MRKCatch)
            {
                catchPlayer(handSender.getActor());
            }

        }
        else if (Ray_EventHandsRelease* releaseEvent = _event->DynamicCast< Ray_EventHandsRelease>(ITF_GET_STRINGID_CRC(Ray_EventHandsRelease,579391720)))
        {
            releaseAll();
        }
        else if(HitStim* hitEvent = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
        {
            receiveHit(hitEvent);
        }

    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::receiveHit(HitStim* _hitEvent)
    {

        if (m_mustDie)
        {
            return;
        }

        m_mustDie = btrue;

        // Play anim hit
        if (m_animLightComponent)
        {
            const StringID animHit = ITF_GET_STRINGID_CRC(Hit,2856859763);
            m_animLightComponent->setAnim(animHit);

			// Spawn lum
			Ray_EventSpawnRewardLum evtRewardLum;
			evtRewardLum.clear();
			evtRewardLum.setup(m_actor, Vec3d::Zero);
			evtRewardLum.setNumRewards(1);
			evtRewardLum.setRewardReceiver(_hitEvent->getSender());
			evtRewardLum.setEjectionDir(Vec2d::Up);
			GAMEMANAGER->onEvent(&evtRewardLum);   
        }

        if (m_fxController)
        {
            const StringID feedbackAction = _hitEvent->getActionFromHit();
            u32 fxHandle = m_fxController->playFeedback(_hitEvent->getSender(), feedbackAction, m_actor->getRef());
            m_fxController->setFXPos(fxHandle, _hitEvent->getFxPos());
        }

        // Retract all attack
        for (u32 i = 0; i < m_listAttack.size(); i++)
        {
            SlotAttack& slot = m_listAttack[i];
            changeState(slot, STATE_ATTACK_BW);

            // release players

            if (Actor* actPlayer = slot.m_playerRef.getActor())
            {
                Ray_EventHandsRelease eventRelease;
                eventRelease.setSender(m_actor->getRef());
                actPlayer->onEvent(&eventRelease);                
            }
        }

    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::releaseAll()
    {
        for (u32 i = 0; i < m_listAttack.size(); i++)
        {
            SlotAttack& slot    = m_listAttack[i];

            if (slot.m_state != STATE_ATTACK_DISABLED)
            {
                if (slot.m_isAttackSpawned)
                {
                    changeState(slot, STATE_ATTACK_BW);
                }
                else
                {
                    changeState(slot, STATE_LINKATTACK_BW);
                }
            }
        }
    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::Draw()
    {
        Super::Draw();

        const Vec2d parentNormal = m_patchCurveComponent->getParentAttachNormal();

        for (u32 i = 0; i < m_listAttack.size(); i++)
        {
            SlotAttack& slot = m_listAttack[i];
            Actor* handActor   = slot.m_handRef.getActor();

            if (handActor)
            {
                const Vec2d childNomal   = m_patchCurveComponent->getChildAttachNormal(handActor, slot.m_RootPos, handActor->get2DPos());
                const Vec2d childAttach  = m_patchCurveComponent->getChildAttachPosition(handActor);

                m_patchCurveComponent->drawLink(slot.m_RootPos, parentNormal, childAttach, childNomal, slot.m_patchCurve, slot.m_alpha);
            }

        }

    }

    // ****************************************************************************

    void Ray_WaterHandsAIComponent::growAABB()
    {

        AABB aabb(m_actor->getPos());

        for (u32 i = 0; i < m_listAttack.size(); i++)
        {
            SlotAttack& slot = m_listAttack[i];
            Actor* handActor   = slot.m_handRef.getActor();

            if (handActor)
            {
                aabb.grow(handActor->getAABB());
            }
        }

        m_actor->growRelativeAABBFromAbsolute(aabb);

    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    // ****************************************************************************
    void Ray_WaterHandsAIComponent::updateDebug( f32 _dt )
    {
        for (u32 i = 0; i < m_listAttack.size(); i++) 
        {
            SlotAttack& slot = m_listAttack[i];

            if (slot.m_state == STATE_ATTACK_CHARGE)
            {
                DebugDraw::text(slot.m_handRef.getActor()->get2DPos(), 0.0f, Color::red(), "State : %d / ChargeGrame : %d", slot.m_state, slot.m_countTrameCharge);
            }
            else
            {
                DebugDraw::text(slot.m_handRef.getActor()->get2DPos(), 0.0f, Color::red(), "State : %d", slot.m_state);
            }
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

#ifdef ITF_SUPPORT_EDITOR
    // ****************************************************************************
    void Ray_WaterHandsAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
        if (getTemplate()->getUseRadiusMaxAttack())
        {
            DebugDraw::circle(m_actor->getPos(), m_rangeMaxDetect, Color::red(), 0.0f);
        }
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_WaterHandsAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_WaterHandsAIComponent_Template)
        SERIALIZE_OBJECT("enemyDetectionRange",     m_enemyDetectionRange);
        SERIALIZE_MEMBER("spawnHandPath",           m_spawnHandPath);

        SERIALIZE_MEMBER("speedAttack",             m_speedAttack);
        SERIALIZE_MEMBER("speedReturn",             m_speedReturn);

        SERIALIZE_MEMBER("timeWaitOnPlayerEscape",  m_timeWaitOnPlayerEscape);
        SERIALIZE_MEMBER("timeWaitAfterCaught",     m_timeWaitAfterCaught);
        SERIALIZE_MEMBER("timeAnticip",             m_timeAnticip);
       
        SERIALIZE_MEMBER("useRadiusMaxAttack",      m_useRadiusMaxAttack);
        SERIALIZE_MEMBER("linearMode",              m_linearMode);

        SERIALIZE_MEMBER("distMaxCharge",           m_distMaxCharge);
		SERIALIZE_MEMBER("randomHands",             m_randomHands);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_WaterHandsAIComponent_Template::Ray_WaterHandsAIComponent_Template()
    : m_speedAttack(2.0f)
    , m_speedReturn(3.0f)
    , m_timeWaitOnPlayerEscape(2.0f)
    , m_timeWaitAfterCaught(0.5f)
    , m_useRadiusMaxAttack(btrue)
    , m_linearMode(bfalse)
    , m_timeAnticip(0.5f)
    , m_distMaxCharge(2.5f)
	, m_randomHands(btrue)
    {
        // none
    }

    //*****************************************************************************

    Ray_WaterHandsAIComponent_Template::~Ray_WaterHandsAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************




}

