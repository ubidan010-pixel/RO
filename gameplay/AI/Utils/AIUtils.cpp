#include "precompiled_gameplay.h"

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

    namespace AIUtils
    {
        void getActorsByInteraction( Faction _myFaction, Interaction _interaction, const DepthRange& _depthRange, ActorList& _actors )
        {
            Actor* allActorsBuff[512];
            ActorList allActors(sizeof(allActorsBuff)/sizeof(Actor*), MemoryId::mId_Temporary, allActorsBuff);

            AI_MANAGER->getActorsFromLayer(_depthRange, allActors);

            const u32 allActorCount = allActors.size();
            for (u32 i = 0; i < allActorCount; i++)
            {
                Actor* actor = allActors[i];

                if ( FACTION_MANAGER->hasInteraction(_myFaction, getFaction(actor), _interaction) )
                {
                    _actors.push_back(actor);
                }
            }
        }

        void getEnemies( Faction _myFaction, const DepthRange& _depthRange, ActorList& _enemies )
        {
            getActorsByInteraction(_myFaction, INTERACTION_TARGET, _depthRange, _enemies);
        }

        void getLivePlayers( const DepthRange& _depthRange, ActorList& _players )
        {
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Player* player = GAMEMANAGER->getPlayer(i);
                if ( player && player->getActive() && !player->isDead() )
                {
                    if ( Actor* playerActor = player->getActor() )
                    {
                        if ( _depthRange.contains(playerActor->getDepth()) )
                        {
                            _players.push_back(playerActor);
                        }
                    }
                }
            }
        }

        void getLivePlayers( ActorList& _players )
        {
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Player* player = GAMEMANAGER->getPlayer(i);
                if ( player && player->getActive() && !player->isDead() )
                {
                    if ( Actor* playerActor = player->getActor() )
                    {
                        _players.push_back(playerActor);
                    }
                }
            }
        }

        void getLivePlayers(const AABB & _range, ActorList& _players)
        {
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Player* player = GAMEMANAGER->getPlayer(i);
                if ( player && player->getActive() && !player->isDead() )
                {
                    if ( Actor* playerActor = player->getActor() )
                    {
                        if( _range.contains( playerActor->get2DPos() ) )
                        {
                            _players.push_back(playerActor);
                        }
                    }
                }
            }
        }

        void getPlayers( const DepthRange& _depthRange, ActorList& _players )
        {
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Actor* player = GAMEMANAGER->getActivePlayer(i);
                if ( player && _depthRange.contains(player->getDepth()) )
                {
                    _players.push_back(player);
                }
            }
        }

        void getPlayers( ActorList& _players )
        {
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Actor* player = GAMEMANAGER->getActivePlayer(i);
                if ( player )
                {
                    _players.push_back(player);
                }
            }
        }

        void getLivePlayersForGameMode( ActorList& _players, GameMode _mode )
        {
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Player* player = GAMEMANAGER->getPlayer(i);
                if ( player && player->getActive() && !player->isDead() )
                {
                    if ( Actor* playerActor = getActor( player->getActorForMode( _mode ) ) )
                    {
                        _players.push_back(playerActor);
                    }
                }
            }
        }


        class CompareActorbyDist
        {

        public:

            CompareActorbyDist(Vec2d _posRef)
            : m_posRef(_posRef)
            {}

            typedef Actor* actorPtr;
            i32 operator() (const actorPtr& _A, const actorPtr& _B) const
            {
                f32 sqrDistA = (_A->get2DPos() - m_posRef).sqrnorm();
                f32 sqrDistB = (_B->get2DPos() - m_posRef).sqrnorm();

                if (sqrDistA < sqrDistB)
                {
                    return -1;
                }
                else if (sqrDistA > sqrDistB)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }

        public:

            Vec2d m_posRef;

        };


        void getSortedPlayersInRange(Actor* _referenceActor, f32 _range, ActorList& _players)
        {
            // Get players
            getPlayers(_referenceActor->getDepth(), _players);

            // Sort player by dist
            _players.quickSort(CompareActorbyDist(_referenceActor->get2DPos()));   
        }

         void getSortedActorsInRange(Actor* _referenceActor, f32 _range, ActorList& _actors)
        {
            // Get actors
            getActorsInRange(_referenceActor, _range, _actors);

            // Sort actors
            _actors.quickSort(CompareActorbyDist(_referenceActor->get2DPos()));
        }

        void getActorsInRange(Actor* _referenceActor, f32 _range, ActorList& _actors)
        {
            Actor* allActorsBuff[512];
            ActorList allActors(sizeof(allActorsBuff)/sizeof(Actor*), MemoryId::mId_Temporary, allActorsBuff);

            AI_MANAGER->getActorsFromLayer(_referenceActor->getDepth(), allActors);

            const Vec2d& refActorPos = _referenceActor->get2DPos();
            f32 sqrRange = _range * _range;

            const u32 allActorCount = allActors.size();
            for (u32 i = 0; i < allActorCount; i++)
            {
                Actor* otherActor = allActors[i];

                if (otherActor == _referenceActor)
                {
                    continue;
                }
            
                f32 sqrDist = (otherActor->get2DPos() - refActorPos).sqrnorm();

                if (sqrDist <= sqrRange)
                {
                    _actors.push_back(otherActor);
                }
            }
        }


        // return the closest live player
        Actor* getClosestLivePlayer( Actor* _referenceActor )
        {
            Actor* playersBuff[8];
            ActorList players(sizeof(playersBuff)/sizeof(Actor*), MemoryId::mId_Temporary, playersBuff);

            getLivePlayers(_referenceActor->getDepth(), players);
            return getClosestActor(players, _referenceActor, bfalse, F32_INFINITY);
        }

        //return the closest live player inside the wanted AABB
        Actor * getClosestLivePlayer(Actor * _referenceActor, const AABB & _range)
        {
            Actor* playersBuff[8];
            ActorList players(sizeof(playersBuff)/sizeof(Actor*), MemoryId::mId_Temporary, playersBuff);

            getLivePlayers(_referenceActor->getDepth(), players);
            //filter with AABB
            return getClosestActor(players, _referenceActor, _referenceActor->isFlipped(), _range);
        }

        //return the closest live player inside the wanted AABB
        Actor * getClosestLivePlayer(const Vec2d & _position, const DepthRange & _depth, const bbool _flipped, const AABB & _range)
        {
            Actor* playersBuff[8];
            ActorList players(sizeof(playersBuff)/sizeof(Actor*), MemoryId::mId_Temporary, playersBuff);

            getLivePlayers(_depth, players);
            //filter with AABB
            return getClosestActor(players, _position, _flipped, _range);
        }

        //return the closest live player inside the wanted shape
        Actor * getClosestLivePlayer(const Vec2d & _position, const DepthRange & _depth, const PhysShapePolygon * const _range)
        {
            Actor* playersBuff[8];
            ActorList players(sizeof(playersBuff)/sizeof(Actor*), MemoryId::mId_Temporary, playersBuff);

            getLivePlayers(_depth, players);
            //filter with AABB
            return getClosestActor(players, _position, _range);
        }

        // return the lead player (live player that's the furthest along the map direction)
        Actor* getLeadLivePlayer( Actor* _referenceActor )
        {
            Actor* playersBuff[8];
            ActorList players(sizeof(playersBuff)/sizeof(Actor*), MemoryId::mId_Temporary, playersBuff);

            getLivePlayers(_referenceActor->getDepth(), players);

            if (players.size())
            {
                Vec2d mapDir;
                if ( CAMERACONTROLLERMANAGER->getMapDirection(mapDir, _referenceActor->getPos()) )
                {
                    sortOnDirection(mapDir, players);
                    return players[players.size() - 1];
                }
                else
                {
                    return getClosestActor(players, _referenceActor, bfalse, F32_INFINITY);
                }
            }

            return NULL;
        }

        Faction getFaction(const Actor* _actor)
        {
            AIData * data = AI_MANAGER->getAIData(_actor->getRef(),_actor->getDepth());
            if (data)
            {
                if (data->m_faction == FACTION_UNKNOWN)
                    return 0;
                else
                    return data->m_faction;
            }
            // unknown faction, consider as neutral
            return 0;
        }

        //get faction from a stim
        Faction getStimFaction(const HitStim * const _stim)
        {
            Faction stimFaction = U32_INVALID;
            Actor* sender = getActor(_stim->getSender());
            if (sender)
            {
                stimFaction = _stim->getFaction();
                if (stimFaction == U32_INVALID)
                {
                    stimFaction = AIUtils::getFaction(sender);
                }
            }
            return stimFaction;
        }



        // do I want to target this guy? PARAM ORDER IS IMPORTANT!
        bbool isTarget( Faction _me, Faction _him )
        {
            return FACTION_MANAGER->hasInteraction(_me, _him, INTERACTION_TARGET);
        }

        // can this guy hit me? does it cause me damage? PARAM ORDER IS IMPORTANT!
        bbool isEnemy( Faction _me, Faction _him, bbool* _receiveDamage /*= NULL*/ )
        {
            InteractionMask interactions = FACTION_MANAGER->getInteractions(_me, _him);

            if (_receiveDamage)
            {
                *_receiveDamage = (interactions & INTERACTION_RECEIVEDAMAGE) != 0;
            }

            return (interactions & INTERACTION_RECEIVEHIT) != 0;
        }

        bbool isEnemyHit( const HitStim* _hit, Faction _receiverFaction, bbool* _receiveDamage /*= NULL*/ )
        {
            Actor* sender = getActor(_hit->getSender());
            if (sender)
            {
                Faction senderFaction = _hit->getFaction();
                if (senderFaction == U32_INVALID)
                {
                    senderFaction = AIUtils::getFaction(sender);
                }

                InteractionMask interactions = FACTION_MANAGER->getInteractions(_receiverFaction, senderFaction);

                if (_receiveDamage)
                {
                    *_receiveDamage = (interactions & INTERACTION_RECEIVEDAMAGE) != 0;
                }

                return (interactions & INTERACTION_RECEIVEHIT) != 0;
            }

            ITF_ASSERT_MSG(0, "HitStim has no sender!");
            return bfalse;
        }



        bbool isDamageHit( const HitStim* _hit, Faction _receiverFaction )
        {
            Actor* sender = getActor(_hit->getSender());
            if (sender)
            {
                Faction senderFaction = _hit->getFaction();
                if (senderFaction == U32_INVALID)
                {
                    senderFaction = AIUtils::getFaction(sender);
                }
                return FACTION_MANAGER->hasInteraction(_receiverFaction, senderFaction, INTERACTION_RECEIVEDAMAGE);
            }

            ITF_ASSERT_MSG(0, "HitStim has no sender!");
            return bfalse;
        }



        // Returns the closest (registered) actor contained in a circle
        // Returns also how many actors we detect
        // _mustHaveOppositeFlip: only return actors that look at us (if detectBehind = false)
        Actor* getClosestActor( Interaction _interaction,
            Actor* _actor, bbool _flipped,
            f32 _range, bbool _detectBehind /*= btrue*/,
            u32* _actorCount /*= NULL*/,
            bbool _mustHaveOppositeFlip /*= bfalse*/ )
        {
            Actor* actorsBuff[512];
            ActorList actors(sizeof(actorsBuff)/sizeof(Actor*), MemoryId::mId_Temporary, actorsBuff);

            getActorsByInteraction(getFaction(_actor), _interaction, _actor->getDepth(), actors);
            return getClosestActor(actors, _actor, _flipped, _range, _detectBehind, _actorCount, _mustHaveOppositeFlip);
        }

        // Given an actor list, returns the closest one contained in a circle.
        // Returns also how many actors we detect.
        // _mustHaveOppositeFlip: only return actors that look at us (if detectBehind = false)
        Actor* getClosestActor( const ActorList& _actors,
            Actor* _actor, bbool _flipped,
            f32 _range, bbool _detectBehind /*= btrue*/,
            u32* _actorCount /*= NULL*/,
            bbool _mustHaveOppositeFlip /*= bfalse*/ )
        {
            Actor* closestActor = NULL;
            u32 actorCount = 0;

            const Vec2d& myPos = _actor->get2DPos();
            const Vec2d myDir = AIUtils::getLookDir(_actor, _flipped);

            f32 sqrRange = _range == F32_INFINITY ? F32_INFINITY : _range * _range;
            f32 minSqrDistance = F32_INFINITY;

            for (u32 i = 0; i < _actors.size(); i++)
            {
                Actor* otherActor = _actors[i];

                if (otherActor == _actor)
                {
                    continue;
                }

                if (_mustHaveOppositeFlip && getFlipped(otherActor) == _flipped)
                {
                    continue;
                }

                Vec2d vecToActor = otherActor->get2DPos() - myPos;

                if (_detectBehind || vecToActor.dot(myDir) > 0)
                {
                    f32 sqrDist = vecToActor.sqrnorm();

                    if ( sqrDist < sqrRange )
                    {
                        actorCount++;

                        if ( sqrDist < minSqrDistance )
                        {
                            minSqrDistance = sqrDist;
                            closestActor = otherActor;
                        }
                    }
                }
                // else: he's behind us and we don't detect him
            }


            if (_actorCount)
                *_actorCount = actorCount;

            return closestActor;
        }

        // Returns the closest detected (registered) actor in a circle
        // Returns also how many actors we detect
        // _mustHaveOppositeFlip: if true + detectBehind = false, only return actors that look at us
        bbool hasActorInRange( Interaction _interaction,
            Actor* _actor, bbool _flipped,
            f32 _range, bbool _detectBehind /*= btrue*/,
            bbool _mustHaveOppositeFlip /*= bfalse*/ )
        {
            Actor* actorsBuff[512];
            ActorList actors(sizeof(actorsBuff)/sizeof(Actor*), MemoryId::mId_Temporary, actorsBuff);

            getActorsByInteraction(getFaction(_actor), _interaction, _actor->getDepth(), actors);
            return hasActorInRange(actors, _actor, _flipped, _range, _detectBehind, _mustHaveOppositeFlip);
        }

        // Returns the closest detected (registered) actor in a circle
        // Returns also how many actors we detect
        // _mustHaveOppositeFlip: if true + detectBehind = false, only return actors that look at us
        bbool hasActorInRange( const ActorList& _actors,
            Actor* _actor, bbool _flipped,
            f32 _range, bbool _detectBehind /*= btrue*/,
            bbool _mustHaveOppositeFlip /*= bfalse*/ )
        {
            const Vec2d& myPos = _actor->get2DPos();
            Vec2d myDir = AIUtils::getLookDir(_actor, _flipped);

            f32 sqrRange = _range * _range;


            const u32 actorCount = _actors.size();
            for (u32 i = 0; i < actorCount; i++)
            {
                Actor* otherActor = _actors[i];

                if (otherActor == _actor)
                {
                    continue;
                }

                if (_mustHaveOppositeFlip && getFlipped(otherActor) == _flipped)
                {
                    continue;
                }

                Vec2d vecToActor = otherActor->get2DPos() - myPos;

                if (_detectBehind || vecToActor.dot(myDir) > 0)
                {
                    if ( vecToActor.sqrnorm() < sqrRange )
                    {
                        return btrue;
                    }
                }
                // else: he's behind us and we don't detect him
            }

            return bfalse;
        }

        // Returns the closest (registered) actor contained in an AABB
        // Returns also how many actors we detect
        Actor* getClosestActor( Interaction _interaction,
            Actor* _actor, bbool _flipped,
            const AABB& _range,
            u32* _actorCount /*= NULL*/ )
        {
            Actor* actorsBuff[512];
            ActorList actors(sizeof(actorsBuff)/sizeof(Actor*), MemoryId::mId_Temporary, actorsBuff);

            getActorsByInteraction(getFaction(_actor), _interaction, _actor->getDepth(), actors);
            return getClosestActor(actors, _actor, _flipped, _range, _actorCount);
        }

        // Given an actor list, returns the closest one contained in an AABB
        // Returns also how many actors we detect
        Actor* getClosestActor( const ActorList& _actors,
            Actor* _actor, bbool _flipped,
            const AABB& _range,
            u32* _actorCount /*= NULL*/ )
        {
            const Vec2d& myPos = _actor->get2DPos();
            AABB aabb = getAbsoluteAABB(_range, myPos, _flipped);

            Actor* closestActor = NULL;
            u32 actorCount = 0;

            f32 minSqrDistance = F32_INFINITY;

            for (u32 i = 0; i < _actors.size(); i++)
            {
                Actor* otherActor = _actors[i];

                if (otherActor == _actor)
                {
                    continue;
                }

                if ( aabb.contains(otherActor->get2DPos()) )
                {
                    actorCount++;

                    f32 sqrDist = (otherActor->get2DPos() - myPos).sqrnorm();
                    if ( sqrDist < minSqrDistance )
                    {
                        minSqrDistance = sqrDist;
                        closestActor = otherActor;
                    }
                }
            }


            if (_actorCount)
            {
                *_actorCount = actorCount;
            }

            return closestActor;
        }

        // Given an actor list, returns the closest one contained in an AABB
        // Returns also how many actors we detect
        Actor* getClosestActor( const ActorList& _actors,
            const Vec2d & _position,
            const bbool _flipped,
            const AABB& _range,
            u32* _actorCount /*= NULL*/ )
        {
            AABB aabb = getAbsoluteAABB(_range, _position, _flipped);

            Actor* closestActor = NULL;
            u32 actorCount = 0;

            f32 minSqrDistance = F32_INFINITY;

            for (u32 i = 0; i < _actors.size(); i++)
            {
                Actor* otherActor = _actors[i];

                if ( aabb.contains(otherActor->get2DPos()) )
                {
                    actorCount++;

                    f32 sqrDist = (otherActor->get2DPos() - _position).sqrnorm();
                    if ( sqrDist < minSqrDistance )
                    {
                        minSqrDistance = sqrDist;
                        closestActor = otherActor;
                    }
                }
            }


            if (_actorCount)
            {
                *_actorCount = actorCount;
            }

            return closestActor;
        }

        

        // Given an actor list, returns the closest one contained in an AABB
        // Returns also how many actors we detect
        Actor* getClosestActor( const ActorList& _actors,
            const Vec2d & _position,
            const PhysShapePolygon * const _range,
            u32* _actorCount /*= NULL*/ )
        {
            ITF_ASSERT(_range);

            Actor* closestActor = NULL;
            u32 actorCount = 0;

            f32 minSqrDistance = F32_INFINITY;

            for (u32 i = 0; i < _actors.size(); i++)
            {
                Actor* otherActor = _actors[i];

                if ( _range->isPointInside(otherActor->get2DPos()) )
                {
                    actorCount++;

                    f32 sqrDist = (otherActor->get2DPos() - _position).sqrnorm();
                    if ( sqrDist < minSqrDistance )
                    {
                        minSqrDistance = sqrDist;
                        closestActor = otherActor;
                    }
                }
            }


            if (_actorCount)
            {
                *_actorCount = actorCount;
            }

            return closestActor;
        }

        bbool hasActorInRange( Interaction _interaction,
            Actor* _actor, bbool _flipped,
            const AABB& _range )
        {
            const Vec2d& myPos = _actor->get2DPos();
            Faction myFaction = getFaction(_actor);

            AABB aabb = getAbsoluteAABB(_range, myPos, _flipped);

            Actor* allActorsBuff[512];
            ActorList allActors(sizeof(allActorsBuff)/sizeof(Actor*), MemoryId::mId_Temporary, allActorsBuff);

            AI_MANAGER->getActorsFromLayer(_actor->getDepth(), allActors);

            const u32 allActorCount = allActors.size();
            for (u32 i = 0; i < allActorCount; i++)
            {
                Actor* otherActor = allActors[i];

                if (otherActor == _actor)
                {
                    continue;
                }

                if ( FACTION_MANAGER->hasInteraction(myFaction, getFaction(otherActor), _interaction) )
                {
                    continue;
                }

                if ( aabb.contains(otherActor->get2DPos()) )
                {
                    return btrue;
                }
            }

            return bfalse;
        }


        // Returns the closest detected enemy in a circle
        // Returns also how many enemies we detect
        Actor* getClosestEnemy( Actor* _actor, bbool _flipped, f32 _range, bbool _detectBehind /*= btrue*/, u32* _enemyCount /*= NULL*/, bbool _mustHaveOppositeFlip /*= bfalse*/ )
        {
            return getClosestActor( INTERACTION_TARGET, _actor, _flipped, _range, _detectBehind, _enemyCount, _mustHaveOppositeFlip );
        }

        // Returns the closest detected enemy in an AABB
        // Returns also how many enemies we detect
        Actor* getClosestEnemy( Actor* _actor, bbool _flipped, const AABB& _range, u32* _enemyCount /*= NULL */ )
        {
            return getClosestActor( INTERACTION_TARGET, _actor, _flipped, _range, _enemyCount );
        }

        bbool hasEnemyInRange( Actor* _actor, bbool _flipped, f32 _range, bbool _detectBehind /*= btrue*/, bbool _mustHaveOppositeFlip /*= bfalse */ )
        {
            return hasActorInRange( INTERACTION_TARGET, _actor, _flipped, _range, _detectBehind, _mustHaveOppositeFlip );
        }

        bbool hasEnemyInRange( Actor* _actor, bbool _flipped, const AABB& _range )
        {
            return hasActorInRange( INTERACTION_TARGET, _actor, _flipped, _range );
        }

        Vec2d getCameraDirection(Actor* _actor)
        {
            // get camera modifier direction *at the given actor's position* if set, or (1, 0)
            CameraControllerManager* camControllerMgr = CAMERACONTROLLERMANAGER;
            if (!camControllerMgr)
            {
                return Vec2d::XAxis;
            }
            const CameraModifierComponent* camModifier = camControllerMgr->getCameraModifierComponent(
                _actor->getAABB(), _actor->getDepth());
            if (!camModifier)
            {
                return Vec2d::XAxis;
            }
            const CamModifierUpdate& updateData = camModifier->getUpdateData();
            Vec2d direction = updateData.getDirection();
            if (direction == Vec2d::Zero)
            {
                return Vec2d::XAxis;
            }
            direction.normalize();
            return direction;
        }

        Actor* getClosestPursuer(Actor* _actor, f32& _minDot)
        {
            // get distance to the closest player
            _minDot = F32_INFINITY;
            Actor* closestPlayer = NULL;

            Actor* playersBuff[8];
            ActorList players(sizeof(playersBuff)/sizeof(Actor*), MemoryId::mId_Temporary, playersBuff);

            AIUtils::getPlayers(players);
            for (u32 i=0; i<players.size(); ++i)
            {
                // ignore dead players
                EventQueryIsDead query;
                players[i]->onEvent(&query);
                if (query.getIsDead())
                {
                    continue;
                }

                // distance in camera space = dot product with camera directon
                Vec2d playerToMe = _actor->get2DPos() - players[i]->get2DPos();
                f32 dot = getCameraDirection(_actor).dot(playerToMe);
                if (dot < _minDot)
                {
                    _minDot = dot;
                    closestPlayer = players[i];
                }
            }
            return closestPlayer;
        }

        Vec2d mirrorVector( const Vec2d& _edge, const Vec2d& _vec, f32 _limit )
        {
            Vec2d edgeNormal = _edge.getPerpendicular();
            Vec2d exitVec = _vec;

            exitVec.normalize();

            // Calculate the entry angle on the polyline and mirror it to get the edge
            f32 angle = -f32_ACos(exitVec.dot(edgeNormal));
            if ( exitVec.cross(edgeNormal) > 0.f )
            {
                angle *= -1.f;
            }

            // Clamp it between our bounce range
            angle = ClampAngle(Angle(bfalse,angle),Angle(bfalse,-_limit),Angle(bfalse,_limit)).ToRadians();

            // Set the bounce speed
            Vec2d ret = edgeNormal.Rotate(angle) * _vec.norm();

            return ret;
        }

        AABB getAbsoluteAABB( const AABB& _relativeAABB, const Vec2d& _absolutePos, bbool _flipped )
        {
            AABB aabb = _relativeAABB;

            if (_flipped)
            {
                aabb.FlipHorizontaly(bfalse);
            }

            aabb.setMin(aabb.getMin() + _absolutePos);
            aabb.setMax(aabb.getMax() + _absolutePos);

            return aabb;
        }

        Vec2d getLookDir( const f32 _angle, const bbool _flipped )
        {
            Vec2d dir = Vec2d::Right.Rotate(_angle);

            if ( _flipped )
            {
                dir *= -1.f;
            }

            return dir;
        }

        Vec2d getLookDir( const Actor* _actor )
        {
            return getLookDir(_actor->getAngle(), _actor->isFlipped());
        }

        Vec2d getOffsetPosition( Actor* _actor, bbool _flipped, const Vec2d& _localOffset )
        {
            Vec2d worldOffset = _localOffset.Rotate(_actor->getAngle());

            if ( _flipped )
            {
                worldOffset *= -1.f;
            }

            return _actor->get2DPos() + worldOffset;
        }

        bbool getFlipped( Actor* _actor )
        {
            return _actor->isFlipped();
        }

        void getAngleAndFlipped( const Vec2d& _lookDir, f32& _angle, bbool& _flipped )
        {
            if ( Vec2d::Right.dot( _lookDir ) < 0.0f )
            {
                _angle = (-_lookDir).getAngle();
                _flipped = btrue;
            }
            else
            {
                _angle = _lookDir.getAngle();
                _flipped = bfalse;
            }
        }

        Actor* getActor( ObjectRef _ref )
        {
            BaseObject* obj = GETOBJECT(_ref);
            if ( obj )
            {
                return obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            }

            return NULL;
        }

        PolyLine* getPolyLine( ObjectRef _polyRef )
        {
            BaseObject* obj = GETOBJECT(_polyRef);
            if ( obj )
            {
                return obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));
            }

            return NULL;
        }

        f32 getEdgeFriction( const PolyLineEdge* _edge )
        {
            const GameMaterial_Template* mat = World::getGameMaterial(_edge->getGameMaterial());

            if ( mat )
            {
                return mat->getFriction();
            }
            else
            {
                return 1.f;
            }
        }

        const PolyLineEdge* getPolyLineEdge( ObjectRef _polyRef, u32 _edgeIndex )
        {
            if ( _edgeIndex == U32_INVALID)
            {
                return NULL;
            }

            PolyLine* poly = getPolyLine(_polyRef);
            if ( poly && _edgeIndex < poly->getPosCount() )
            {
                return &poly->getEdgeAt(_edgeIndex);
            }

            return NULL;
        }

        void getPolyLine( ObjectRef _polyRef, u32 _edgeIndex, PolyLine*& _polyLine, const PolyLineEdge*& _edge )
        {
            _polyLine = NULL;
            _edge = NULL;

            BaseObject* obj = GETOBJECT(_polyRef);

            if ( obj )
            {
                _polyLine = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

                if ( _polyLine )
                {
                    if ( _edgeIndex < _polyLine->getVectorsCount() )
                    {
                        _edge = &_polyLine->getEdgeAt(_edgeIndex);
                    }
                }
            }
        }



        // _start = Start position of the check
        // _gravity = Gravity normalized
        // _poly = Polyline to analyze
        // _edgeIndex = Edge index where we start the search
        // _left = True if we go in the opposite way of the polyline
        // _minHeight = Minimum height of an edge to consider it as an end
        // _maxDist = Optional value to exit the search when a certain distance is reached
        f32 getRemainingDistanceOnEdge( const Vec2d &_start,  const Vec2d& _gravityDir, PolyLine* _poly,
            u32 _edgeIndex, bbool _left, f32 _minHeight,
            f32 _maxDist )
        {
            // Gravity must be normalized
            ITF_ASSERT(_gravityDir.isNormalized());
            ITF_ASSERT(_poly);

            const PolyLineEdge* edge = &_poly->getEdgeAt(_edgeIndex);
            Vec2d gravityXDir = _gravityDir.getPerpendicular();

            f32 dist = 0.f;
            Vec2d edgeToPos = _start - edge->getPos();

            f32 startPos = edge->m_normalizedVector.dot(edgeToPos);
            startPos = Clamp(startPos,0.f,edge->m_length);

            if ( !_left )
            {
                dist += edge->m_length - startPos;
            }
            else
            {
                dist += startPos;
            }

            PolyLine* _startPoly = _poly;
            u32 _startEdge = _edgeIndex; 

            getAdjacentEdge(_poly,_edgeIndex,!_left,_poly,_edgeIndex);

            while ( _poly && _edgeIndex < _poly->getVectorsCount() &&
                  ( !_maxDist || _maxDist > dist ) )
            {
                const PolyLineEdge& currentEdge = _poly->getEdgeAt(_edgeIndex);

                f32 edgeAngle = f32_ACos(gravityXDir.dot(currentEdge.m_normalizedVector));
                bool up = gravityXDir.cross(currentEdge.m_normalizedVector) > 0.f;

                if ( _left )
                {
                    up = !up;
                }

                if ( !up && edgeAngle > MTH_PIBY4 && currentEdge.m_length >= _minHeight )
                {
                    break;
                }

                dist += currentEdge.m_length;
                getAdjacentEdge(_poly,_edgeIndex,!_left,_poly,_edgeIndex);

                if ( _poly == _startPoly && _edgeIndex == _startEdge )
                {
                    break;
                }
            }
            return dist;
        }

        // Get the total length of a a polyline from _T [0..1], going towards right (edge 0 to End), or left (End to edge 0)
        f32 getRemainingDistanceOnEdge( f32 _t, PolyLine* _poly, u32 _edgeIndex, bbool _left )
        {
            PolyLine* startPoly = _poly;
            u32 startEdge = _edgeIndex;
            const PolyLineEdge* edge = &_poly->getEdgeAt(_edgeIndex);
            f32 dist;

            if ( !_left )
            {
                dist = edge->m_length * ( 1.f - _t );
            }
            else
            {
                dist = edge->m_length * _t;
            }

            getAdjacentEdge(_poly,_edgeIndex,!_left,_poly,_edgeIndex);

            while ( _poly && _edgeIndex < _poly->getVectorsCount() )
            {
                const PolyLineEdge& currentEdge = _poly->getEdgeAt(_edgeIndex);

                dist += currentEdge.m_length;

                getAdjacentEdge(_poly,_edgeIndex,!_left,_poly,_edgeIndex);

                if ( _poly == startPoly && _edgeIndex == startEdge )
                {
                    break;
                }
            }

            return dist;
        }


        f32 getDistanceBetweenEdgePoints( const PolyLine* _poly, u32 _edgeIndex1, f32 _edgePos1, u32 _edgeIndex2, f32 _edgePos2)
        {
            if (_edgeIndex1 > _edgeIndex2)
                return -getDistanceBetweenEdgePoints( _poly, _edgeIndex2, _edgePos2, _edgeIndex1, _edgePos1);
            if (_edgeIndex1 == _edgeIndex2 && _edgePos1 > _edgePos2)
                return -getDistanceBetweenEdgePoints( _poly, _edgeIndex2, _edgePos2, _edgeIndex1, _edgePos1);

            // egdeIndex1 <= egdeIndex2
            f32 len = 0;
            while (_edgeIndex1 < _edgeIndex2)
            {
                len += (1.f - _edgePos1)*_poly->getEdgeAt(_edgeIndex1).m_length;
                _edgeIndex1 ++;
                _edgePos1   = 0;
            }
            return len + (_edgePos2 - _edgePos1)*_poly->getEdgeAt(_edgeIndex1).m_length;
        }

        bbool moveOnEdge( PolyLine* _poly, f32 & delta, PolyLine*& _newPoly, u32 & _edgeIndex, f32 & _edgePos, PolyLineEdge::HoleMode _stopOnHole)
        {
            _newPoly = _poly;

            const PolyLineEdge *pEdge = &_poly->getEdgeAt(_edgeIndex);
            bbool isStopped = bfalse;
            if (!pEdge)
                return isStopped;

            f32 posX = pEdge->m_length * _edgePos;
            posX += delta;
            while (posX < 0) //
            {
                u32 prevEdge;
                PolyLine* prevPoly;

                getAdjacentEdge(_poly,_edgeIndex,bfalse,prevPoly,prevEdge);

                if (prevEdge != U32_INVALID && !(prevPoly->getHoleAt(prevEdge) & _stopOnHole ))
                {
                    AIUtils::getAdjacentEdge(_poly,_edgeIndex,bfalse,_newPoly,_edgeIndex);
                    pEdge = &_newPoly->getEdgeAt(_edgeIndex);
                    posX += pEdge->m_length;
                } else
                {
                    break;
                }
            }
            while (posX > pEdge->m_length)
            {
                u32 nextEdge;
                PolyLine* nextPoly;

                getAdjacentEdge(_poly,_edgeIndex,btrue,nextPoly,nextEdge);

                if (nextEdge != U32_INVALID && !(nextPoly->getHoleAt(nextEdge) & _stopOnHole ))
                {
                    posX -= pEdge->m_length;
                    getAdjacentEdge(_poly,_edgeIndex,btrue,_newPoly,_edgeIndex);
                    pEdge = &_newPoly->getEdgeAt(_edgeIndex);
                } else
                {
                    break;
                }
            }
            if (posX < 0)
            {
                delta      -= posX;
                posX        = 0;
                isStopped   = btrue;
            }
            if (posX > pEdge->m_length)
            {
                delta      -= pEdge->m_length - posX;
                posX        = pEdge->m_length;
                isStopped   = btrue;
            }

            _edgePos = posX / pEdge->m_length;

            return isStopped;
        }


        // _start = Start position of the check
        // _poly = Polyline to analyze
        // _edgeIndex = Edge index where we start the search
        // _left = True if we go in the opposite way of the polyline
        // _gravityDir = Gravity normalized
        // _slopeEdgeIndex = Optional parameter to return the edge with the biggest slope
        f32 getSlopeAngle( const Vec2d& _start, PolyLine* _poly, u32 _edgeIndex,
            bbool _left, f32 _distCheck, const Vec2d& _gravityDir, u32* _slopeEdgeIndex )
        {
            ITF_ASSERT(_gravityDir.isNormalized());

            PolyLine* currentPolyline = _poly;
            const PolyLineEdge* edge = &currentPolyline->getEdgeAt(_edgeIndex);
            Vec2d gravityDirX = _gravityDir.getPerpendicular();
            f32 dist = 0.f;
            Vec2d edgeToPos = _start - edge->getPos();
            f32 startPos = edge->m_normalizedVector.dot(edgeToPos);

            startPos = Clamp(startPos,0.f,edge->m_length);

            if ( _left )
            {
                dist += startPos;
            }
            else
            {
                dist += edge->m_length - startPos;
            }

            f32 slope = 0.f;
            u32 currentEdge = _edgeIndex;
            const PolyLineEdge* nextEdge;
            Vec2d nextDir;
            PolyLine* nextPolyline;
            u32 nextEdgeIndex;

            if ( _slopeEdgeIndex )
            {
                *_slopeEdgeIndex = currentEdge;
            }

            while ( dist < _distCheck )
            {
                getAdjacentEdge(currentPolyline,currentEdge,!_left,nextPolyline,nextEdgeIndex);

                if ( nextEdgeIndex == U32_INVALID )
                {
                    slope = Max(slope,MTH_PIBY2);

                    if ( _slopeEdgeIndex )
                    {
                        *_slopeEdgeIndex = U32_INVALID;
                    }

                    break;
                }
                else
                {
                    currentEdge = nextEdgeIndex;
                    currentPolyline = nextPolyline;
                }

                nextEdge = &currentPolyline->getEdgeAt(currentEdge);
                nextDir = nextEdge->m_normalizedVector;

                f32 angle = f32_ACos(gravityDirX.dot(nextDir));
                if ( angle > f32_Abs(slope) )
                {
                    f32 sign = gravityDirX.cross(nextDir) > 0.f ? 1.f : -1.f;
                    slope = Max(angle,f32_Abs(slope));
                    slope *= sign;

                    if ( _left )
                    {
                        slope *= -1.f;
                    }

                    if ( _slopeEdgeIndex )
                    {
                        *_slopeEdgeIndex = currentEdge;
                    }
                }

                edge = nextEdge;
                dist += edge->m_length;
            }

            return slope;
        }

        // This function searches along the current edge until it finds another edge whose angle is between _minAngle and _maxAngle
        bbool searchEdgeAngle( const Vec2d& _start, PolyLine* _poly, u32 _edgeIndex, bbool _left, f32 _distCheck, const Vec2d& _gravityDir,
                               f32 _minAngle, f32 _maxAngle,
                               PolyLine** _foundPoly, u32* _foundEdge )
        {
            bbool found = bfalse;
            ITF_ASSERT(_gravityDir.isNormalized());

            PolyLine* currentPolyline = _poly;
            const PolyLineEdge* edge = &currentPolyline->getEdgeAt(_edgeIndex);
            Vec2d gravityDirX = _gravityDir.getPerpendicular();
            f32 dist = 0.f;
            Vec2d edgeToPos = _start - edge->getPos();
            f32 startPos = edge->m_normalizedVector.dot(edgeToPos);

            startPos = Clamp(startPos,0.f,edge->m_length);

            if ( _left )
            {
                dist += startPos;
            }
            else
            {
                dist += edge->m_length - startPos;
            }

            u32 currentEdge = _edgeIndex;
            const PolyLineEdge* nextEdge;
            Vec2d nextDir;
            PolyLine* nextPolyline;
            u32 nextEdgeIndex;

            if ( _foundEdge )
            {
                *_foundEdge = currentEdge;
            }

            if ( _foundPoly )
            {
                *_foundPoly = currentPolyline;
            }

            while ( dist < _distCheck )
            {
                getAdjacentEdge(currentPolyline,currentEdge,!_left,nextPolyline,nextEdgeIndex);

                if ( nextEdgeIndex == U32_INVALID )
                {
                    if ( _foundEdge )
                    {
                        *_foundEdge = U32_INVALID;
                    }

                    if ( _foundPoly )
                    {
                        *_foundPoly = NULL;
                    }

                    break;
                }
                else
                {
                    currentEdge = nextEdgeIndex;
                    currentPolyline = nextPolyline;
                }

                nextEdge = &currentPolyline->getEdgeAt(currentEdge);
                nextDir = nextEdge->m_normalizedVector;

                f32 angle = f32_ACos(fabsf(gravityDirX.dot(nextDir)));

                if ( angle >= _minAngle && angle <= _maxAngle )
                {
                    if ( _foundEdge )
                    {
                        *_foundEdge = currentEdge;
                    }

                    if ( _foundPoly )
                    {
                        *_foundPoly = currentPolyline;
                    }

                    found = btrue;

                    break;
                }

                edge = nextEdge;
                dist += edge->m_length;
            }

            return found;
        }

        bbool hasWallInFront( const Vec2d& _start, bbool _leftParse,
            PolyLine* _poly, u32 _edgeIndex,
            f32 _maxX, f32 _maxY,
            f32& _wallHeight,
            bbool m_drawDebug /*= bfalse*/ )
        {
            Vec2d maxReached;  // (0,0)

            u32 edgeCount = _poly->getVectorsCount();
            i32 parseDirection = _leftParse ? -1 : 1;

            for (u32 i = _edgeIndex; i < edgeCount; i += parseDirection )
            {
                const PolyLineEdge* edge = &_poly->getEdgeAt(i);

#ifdef ITF_SUPPORT_DEBUGFEATURE
                if (m_drawDebug)
                {
                    String msg;msg.setTextFormat("%d", i);
                    DebugDraw::edge(edge, _poly->getDepth(), Color::red(), 0.2f, msg);
                }
#endif // ITF_SUPPORT_DEBUGFEATURE

                Vec2d edgeEnd = edge->getPos();
                if ( !_leftParse )
                {
                    edgeEnd += edge->m_vector;
                }


                Vec2d delta = edgeEnd - _start;
                maxReached.m_x = Max( maxReached.m_x, delta.m_x * parseDirection );
                maxReached.m_y = Max( maxReached.m_y, delta.m_y );

                if ( maxReached.m_x > _maxX || 
                    maxReached.m_y > _maxY )
                {
                    break;
                }
            }

            _wallHeight = maxReached.m_y;
            return _wallHeight >= _maxY;
        }

        // _actor = Actor to check if there is a wall
        // _walkLeft = True if we go in the opposite way of the polyline
        // _poly = Polyline to analyze
        // _startEdge = Edge index where we start the search
        // _searchDist = For how long we want to search along the polyline
        // _radius = Radius of the character
        // _gravityDir = The gravity normalized
        // _wallHeight = Returns the height of the wall found
        bbool hasWallInFront( Actor* _actor, bbool _walkLeft, PolyLine* _poly,
            u32 _startEdge, f32 _searchDist, f32 _radius, 
            const Vec2d& _gravityDir,
            f32& _wallHeight, const PolyLineEdge** _wallEdge /*= NULL*/, Vec2d * _wallTop /*= NULL*/ )
        {
            // Gravity must be normalized
            ITF_ASSERT(_gravityDir.isNormalized());

            u32 slopeEdgeIndex = U32_INVALID;
            f32 angle = getSlopeAngle(_actor->get2DPos(),
                _poly,
                _startEdge,
                _walkLeft,_searchDist,
                _gravityDir,
                &slopeEdgeIndex);

            if ( angle > (MTH_DEGTORAD*65.f) && slopeEdgeIndex != U32_INVALID )
            {
                const PolyLineEdge& wall = _poly->getEdgeAt(slopeEdgeIndex);
                if (_wallEdge)
                {
                    *_wallEdge = &wall;
                }

                Vec2d gravityNormal = _gravityDir.getPerpendicular();
                Vec2d feet = _actor->get2DPos() + ( _gravityDir * _radius );
                Vec2d intersectionA, intersectionB;

                if ( Line_Line(feet,gravityNormal,wall.getPos(),_gravityDir,intersectionA) )
                {
                    Vec2d wallEnd = wall.getPos()+wall.m_vector;

                    if ( Line_Line(feet,gravityNormal,wallEnd,_gravityDir,intersectionB) )
                    {
                        f32 height1 = ( intersectionA - wall.getPos() ).sqrnorm();
                        f32 height2 = ( intersectionB - wallEnd ).sqrnorm();

                        if (height1 > height2)
                        {
                            _wallHeight = height1;
                            if (_wallTop)
                                *_wallTop = wall.getPos();
                        }
                        else
                        {
                            _wallHeight = height2;
                            if (_wallTop)
                                *_wallTop = wallEnd;
                        }

                        return btrue;
                    }
                }
            }

            return bfalse;
        }

        bbool isWall( bbool _left, const Vec2d & _startPos, PolyLine * _poly, u32 _startEdge, f32 _searchDistance, const Vec2d & _gravityDir, f32 & _wallHeight, Vec2d & _wallTop)
        {
            ITF_ASSERT(_gravityDir.isNormalized());

            PolyLine* currentPolyline = _poly;
            const PolyLineEdge* edge = &currentPolyline->getEdgeAt(_startEdge);
            Vec2d gravityDirX = _gravityDir.getPerpendicular();

            f32 angle = f32_ACos(gravityDirX.dot(edge->m_normalizedVector));
            if (angle > MTH_DEGTORAD*65.f)
            {
                //it is a wall now find top of wall
                f32 dist = 0.f;
                Vec2d edgeToPos = _startPos - edge->getPos();
                f32 startPos = edge->m_normalizedVector.dot(edgeToPos);

                startPos = Clamp(startPos,0.f,edge->m_length);

                if ( _left )
                {
                    dist = startPos;
                }
                else
                {
                    dist = edge->m_length - startPos;
                }

                //DebugDraw::edge( _startPos, edge->getPos() + edge->m_vector, edge->m_normalizedVector.getPerpendicular(), 0.0f,Color::white(), 1.0f);

                u32 currentEdge = _startEdge;
                const PolyLineEdge* nextEdge;
                Vec2d nextDir;
                PolyLine* nextPolyline;
                u32 nextEdgeIndex;

                while (dist < _searchDistance)
                {
                    getAdjacentEdge(currentPolyline,currentEdge,!_left,nextPolyline,nextEdgeIndex);

                    if ( nextEdgeIndex == U32_INVALID )
                    {
                        //stop
                        break;
                    }

                    currentEdge = nextEdgeIndex;
                    currentPolyline = nextPolyline;

                    nextEdge = &currentPolyline->getEdgeAt(currentEdge);
                    nextDir = nextEdge->m_normalizedVector;

                    f32 angle = f32_ACos(gravityDirX.dot(nextDir));

                    if ( angle < MTH_DEGTORAD*65.f)
                    {
                        //stop
                        break;
                    }
                    //DebugDraw::edge( edge, 0.0f,Color::white(), 1.0f);

                    edge = nextEdge;
                    dist += edge->m_length;
                }
                //DebugDraw::edge( edge, 0.0f,Color::white(), 1.0f);

                _wallHeight = dist;
                _wallTop = edge->getPos();
                if (!_left)
                    _wallTop += edge->m_vector;

                //DebugDraw::circle(_wallTop,0.0f,0.1f,Color::yellow(),1.0f);
                return btrue;
            }
            else
            {
                return bfalse;
            }
        }

        u32 getNextEdgeInfo( const Vec2d& _start, PolyLine* _poly, u32 _edgeIndex,
            bbool _left, const Vec2d& _gravityDir, AIUtils::EdgeType & _edgeType, 
            f32 & _distanceToEdge, f32 & _edgeLength, bbool & _up, PolyLine ** _nextPoly /* = NULL*/)
        {
            ITF_ASSERT(_gravityDir.isNormalized());

            const PolyLineEdge* edge = &_poly->getEdgeAt(_edgeIndex);
            Vec2d gravityDirX = _gravityDir.getPerpendicular();
            f32 dist = 0.f;
            Vec2d edgeToPos = _start - edge->getPos();
            f32 startPos = edge->m_normalizedVector.dot(edgeToPos);

            startPos = Clamp(startPos,0.f,edge->m_length);

            if ( _left )
            {
                dist += startPos;
            }
            else
            {
                dist += edge->m_length - startPos;
            }

            PolyLine* nextPolyline;
            u32 nextEdgeIndex;


            AIUtils::getAdjacentEdge(_poly,_edgeIndex,!_left,nextPolyline,nextEdgeIndex);

            if ( nextEdgeIndex == U32_INVALID )
            {
                _edgeType = AIUtils::EdgeType_Invalid;
                _distanceToEdge = dist;
                _edgeLength = 0.0f;
                _up = bfalse;
                return U32_INVALID;
            }

            const PolyLineEdge & nextEdge = nextPolyline->getEdgeAt(nextEdgeIndex);
            _edgeType = AIUtils::getEdgeType(nextEdge);
            _distanceToEdge = dist;
            _edgeLength = nextEdge.m_length;
            _up = edge->m_normalizedVector.getPerpendicular().dot(nextEdge.m_normalizedVector) > 0.0f;
            _up = _left?!_up:_up;
            if( _nextPoly != NULL )
            {
                *_nextPoly = nextPolyline;
            }
            return nextEdgeIndex;
        }

        // _actor = Actor to check if there is a wall
        // _walkLeft = True if we go in the opposite way of the polyline
        // _poly = Polyline to analyze
        // _startEdge = Edge index where we start the search
        // _searchDist = For how long we want to search along the polyline
        // _radius = Radius of the character
        // _gravityDir = The gravity normalized
        // _holeDepth = Returns the depth of the hole found
        bbool hasHoleInFront( Actor* _actor, bbool _walkLeft, PolyLine* _poly,
            u32 _startEdge, f32 _searchDist, f32 _radius, 
            const Vec2d& _gravityDir, f32& _holeDepth )
        {
            // Gravity must be normalized
            ITF_ASSERT(_gravityDir.isNormalized());

            u32 slopeEdgeIndex = U32_INVALID;
            f32 angle = getSlopeAngle(_actor->get2DPos(),
                _poly,
                _startEdge,
                _walkLeft,_searchDist,
                _gravityDir,
                &slopeEdgeIndex);

            if (slopeEdgeIndex == U32_INVALID)
            {
                _holeDepth = 10000.f;
                return btrue;
            }
            else
            {
                if ( angle < (MTH_DEGTORAD*-65.f))
                {
                    const PolyLineEdge& wall = _poly->getEdgeAt(slopeEdgeIndex);

                    Vec2d gravityNormal = _gravityDir.getPerpendicular();
                    Vec2d feet = _actor->get2DPos() + ( _gravityDir * _radius );
                    Vec2d intersectionA, intersectionB;

                    if ( Line_Line(feet,gravityNormal,wall.getPos(),_gravityDir,intersectionA) )
                    {
                        Vec2d wallEnd = wall.getPos()+wall.m_vector;

                        if ( Line_Line(feet,gravityNormal,wallEnd,_gravityDir,intersectionB) )
                        {
                            f32 height1 = ( intersectionA - wall.getPos() ).sqrnorm();
                            f32 height2 = ( intersectionB - wallEnd ).sqrnorm();

                            _holeDepth = Max(height1,height2);

                            return btrue;
                        }
                    }
                }
            }

            return bfalse;
        }

        Vec2d getFeetPos( Actor* _actor )
        {
            return _actor->get2DPos() + ( ( Vec2d::Right.Rotate(_actor->getAngle()-MTH_PIBY2) ) * _actor->getRadius() );
        }

        Actor* getActorFromContact( ObjectRef _ref )
        {
            if ( BaseObject* obj = GETOBJECT(_ref) )
            {
                if ( Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                {
                    return actor;
                }
                else if ( PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)) )
                {
                    return poly->getOwnerActor();
                }
            }

            return NULL;
        }

        void sortContactsOnShape( Actor* _sender, const Vec2d& _dir,
            PhysShapePolygon& _polygon,
            const PhysContactsContainer& _contacts,
            SortedContactInfoArray& _result, f32& _shapeStart, f32& _shapeEnd )
        {
            const Vec2d& _pos = _sender->get2DPos();
            f32 _angle = _sender->getAngle();
            f32 polygonStart = 0.f;
            f32 polygonEnd = 0.f;
            const PhysShapePolygon::PointsList& polygonPoints = _polygon.getVertex();
            u32 numPoints = polygonPoints.size();

            // First estimate the length of the shape along the hit direction
            for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
            {
                f32 d = _dir.dot(polygonPoints[pointIndex]);

                if ( ( polygonStart == polygonEnd ) && ( polygonEnd == 0.f ) )
                {
                    polygonStart = polygonEnd = d;
                }
                else
                {
                    if ( d < polygonStart )
                    {
                        polygonStart = d;
                    }
                    else if ( d > polygonEnd )
                    {
                        polygonEnd = d;
                    }
                }
            }

            _shapeStart = polygonStart;
            _shapeEnd = polygonEnd;

            u32 numContacts = _contacts.size();
            u32 resultSize;
            Actor* actor;
            PolyLine* poly;
            const PolyLineEdge* edge;
            ObjectRef refPoly, refActor, refCollideable;
            u32 found;
            Vec2d localPos;

            // Then enter each contact, sorting them by distance from the start
            for ( u32 contactIndex = 0; contactIndex < numContacts; contactIndex++ )
            {
                const SCollidableContact& c =_contacts[contactIndex];

                if ( c.m_collidableUserData == _sender->getRef() )
                {
                    continue;
                }

                BaseObject* collideable = GETOBJECT(c.m_collidable);
                BaseObject* obj = GETOBJECT(c.m_collidableUserData);

                if ( !obj || !collideable )
                {
                    continue;
                }

                poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));
                edge = NULL;

                if ( poly )
                {
                    if ( c.m_edgeIndex < poly->getVectorsCount() )
                    {
                        edge = &poly->getEdgeAt(c.m_edgeIndex);
                    }
                    else
                    {
                        continue;
                    }

                    actor = poly->getOwnerActor();
                    refPoly = poly->getRef();
                }
                else
                {
                    actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                    refPoly = ITF_INVALID_OBJREF;
                }

                refCollideable = c.m_collidable;

                if ( actor )
                {
                    refActor = actor->getRef();
                }
                else
                {
                    refActor = ITF_INVALID_OBJREF;
                }

                if ( !actor && !poly )
                {
                    continue;
                }

                localPos = ( c.m_contactPoint - _pos ).Rotate(-_angle);

                f32 dist = _dir.dot(localPos) - _shapeStart;

                found = U32_INVALID;
                resultSize = _result.size();

                // If we already had a contact for this actor or polyline we update the information
                // if this contact is closer
                for ( u32 i = 0; i < resultSize; i++ )
                {
                    SortedContactInfo& hitInfo = _result[i];

                    if ( hitInfo.m_actor == refActor &&
                         hitInfo.m_collideable == refCollideable )
                    {
                        found = i;

                        bbool contactAdded(bfalse);

                        for (u32 j = 0; j < hitInfo.m_contacts.size(); j++)
                        {
                            if ( dist < hitInfo.m_contacts[j].m_dist )
                            {
                                hitInfo.m_contacts.insert(ContactWithDist(c, dist), j);
                                contactAdded = btrue;
                                break;
                            }
                        }

                        if (!contactAdded)
                        {
                            hitInfo.m_contacts.push_back(ContactWithDist(c, dist));
                        }

                        break;
                    }
                }

                if ( found == U32_INVALID )
                {
                    for ( u32 i = 0; i < resultSize; i++ )
                    {
                        SortedContactInfo& temp = _result[i];

                        f32 dif = fabsf(temp.m_contacts[0].m_dist - dist);

                        if ( dif < MTH_EPSILON )
                        {
                            // If it's the same distance, keep the phantom first
                            BaseObject* baseObj = GETOBJECT(temp.m_collideable);
                            bbool isPhantom = (baseObj != NULL) && (baseObj->DynamicCast<PhysPhantom>(ITF_GET_STRINGID_CRC(PhysPhantom,126559815)) != NULL);

                            if ( isPhantom )
                            {
                                continue;
                            }
                            else
                            {
                                found = i;
                                break;
                            }
                        }
                        else if ( temp.m_contacts[0].m_dist > dist )
                        {
                            found = i;
                            break;
                        }
                    }

                    if ( found == U32_INVALID )
                    {
                        found = _result.size();
                        _result.resize(_result.size()+1);
                    }
                    else
                    {
                        _result.insert(SortedContactInfo(),found);
                    }

                    SortedContactInfo& hitInfo = _result[found];

                    hitInfo.m_actor = refActor;
                    hitInfo.m_polyline = refPoly;
                    hitInfo.m_collideable = refCollideable;
                    hitInfo.m_contacts.push_back(ContactWithDist(c, dist));
                }
            }
        }

        void getAdjacentEdge( PolyLine* _poly, u32 _currentEdge, bbool _right, PolyLine*& _adjPoly, u32& _adjEdge )
        {
            u32 numEdges = _poly->getVectorsCount();

            if ( _right )
            {
                if ( _currentEdge < numEdges-1 )
                {
                    _adjPoly = _poly;
                    _adjEdge = _currentEdge + 1;
                }
                else if ( _poly->isLooping() )
                {
                    _adjPoly = _poly;
                    _adjEdge = 0;
                }
                else if ( _poly->getNext() )
                {
                    _adjPoly = _poly->getNext();
                    _adjEdge = 0;
                }
                else
                {
                    _adjPoly = NULL;
                    _adjEdge = U32_INVALID;
                }
            }
            else
            {
                if ( _currentEdge > 0 )
                {
                    _adjPoly = _poly;
                    _adjEdge = _currentEdge - 1;
                }
                else if ( _poly->isLooping() )
                {
                    _adjPoly = _poly;
                    _adjEdge = numEdges-1;
                }
                else if ( _poly->getPrevious() && _poly->getPrevious()->getVectorsCount() )
                {
                    _adjPoly = _poly->getPrevious();
                    _adjEdge = _adjPoly->getVectorsCount()-1;
                }
                else
                {
                    _adjPoly = NULL;
                    _adjEdge = U32_INVALID;
                }
            }
        }

        void HitShape::setShape(AnimPolylineEvent * _event, AnimLightComponent * _animComponent, Actor * _actor)
        {
            u32 numPoints = _event->getNumPoints();

            if ( numPoints > 1 )
            {
                const Vec2d* points = _event->getPoints();
                Vec2d p;
                bbool flipped = _actor->isFlipped();

                m_shapePoints.clear();

                if ( flipped )
                {
                    for ( i32 i = static_cast<i32>(numPoints)-1; i >= 0; i-- )
                    {
                        p = ( points[i] - _actor->get2DPos() ).Rotate(-_actor->getAngle());

                        m_shapePoints.push_back(p);
                    }
                }
                else
                {
                    for ( u32 i = 0; i < numPoints; i++ )
                    {
                        p = ( points[i] - _actor->get2DPos() ).Rotate(-_actor->getAngle());

                        m_shapePoints.push_back(p);
                    }
                }

                m_shape.setPoints(m_shapePoints);
            }
        }

        void HitShape::setShape(StringID _polyId, AnimLightComponent * _animComponent, Actor * _actor)
        {
            u32 numPoints;
            const Vec2d* points = _animComponent->getCurrentPolylinePointBuffer(_polyId, &numPoints);

            if ( numPoints > 1 )
            {
                Vec2d p;
                bbool flipped = _actor->isFlipped();

                m_shapePoints.clear();

                if ( flipped )
                {
                    for ( i32 i = static_cast<i32>(numPoints)-1; i >= 0; i-- )
                    {
                        p = ( points[i] - _actor->get2DPos() ).Rotate(-_actor->getAngle());

                        m_shapePoints.push_back(p);
                    }
                }
                else
                {
                    for ( u32 i = 0; i < numPoints; i++ )
                    {
                        p = ( points[i] - _actor->get2DPos() ).Rotate(-_actor->getAngle());

                        m_shapePoints.push_back(p);
                    }
                }

                m_shape.setPoints(m_shapePoints);
            }
        }

        PolyLine* getPolylineRegion( const DepthRange& _depthRange, const StringID& _id, const Vec2d& _pos )
        {
            const RegionsManager::RegionDataList* polySwimList = REGIONS_MANAGER->getRegions(_depthRange);

            if ( polySwimList )
            {
                for ( u32 i = 0; i < polySwimList->size(); i++ )
                {
                    const RegionsManager::RegionData& regionData = (*polySwimList)[i];

                    if ( regionData.m_id != _id )
                    {
                        continue;
                    }

                    PolyLine* poly = AIUtils::getPolyLine(regionData.m_ref);

                    if ( !poly )
                    {
                        continue;
                    }

                    if ( !poly->isPointInside(_pos) )
                    {
                        continue;
                    }

                    return poly;
                }
            }

            return NULL;
        }

        void getPolylineRegions( const DepthRange& _depthRange, const Vec2d& _pos, PolylineRegionList& _regionList )
        {
            const RegionsManager::RegionDataList* polySwimList = REGIONS_MANAGER->getRegions(_depthRange);
            PolylineRegionData data;

            if ( polySwimList )
            {
                for ( u32 i = 0; i < polySwimList->size(); i++ )
                {
                    const RegionsManager::RegionData& regionData = (*polySwimList)[i];

                    PolyLine* poly = AIUtils::getPolyLine(regionData.m_ref);

                    if ( !poly )
                    {
                        continue;
                    }

                    if ( !poly->isPointInside(_pos) )
                    {
                        continue;
                    }

                    data.m_polyline = poly;
                    data.m_regionId = regionData.m_id;

                    _regionList.push_back(data);
                }
            }
        }

        void getTravelExitPointWithActorFeet( const Vec3d& _pos, Vec3d& _res_Pos, Actor* _actor )
        {
            _res_Pos = _pos;
            if ( _actor == NULL )
                return;

            if ( AIUtils::getPolylineRegion(_actor->getDepth(),s_RegionName_Swim, _pos.truncateTo2D() ) == NULL )
            {
                _res_Pos += ( _actor->get2DPos() - AIUtils::getFeetPos(_actor) ).to3d(_actor->getPos().m_z );	//	add player feet dist to avoid beeing inside coll
            }    
        }

        //---------------------------------------------------------------------------------------------------------------------
        // Compute the vertical eject force for given player info
        // Compute a parabolic move with player physic
        //
        Vec2d computeChangePageVerticalEjectForce( Actor* _actor, const Vec2d& _finalPoint )
        {
            Vec2d gravity;
            if( StickToPolylinePhysComponent * actorPhysComponent = _actor->GetComponent<StickToPolylinePhysComponent>() )
            {
                gravity = actorPhysComponent->getCurrentGravity();
            }

            f32 x, y;
            f32 a = 1.3f;	// Init angle
            Vec2d targetPos = _finalPoint;
            Vec2d toTargetPos = targetPos - _actor->get2DPos();
            f32 deltaZ = toTargetPos.m_y;
            x = f32_Abs( toTargetPos.m_x );
            y = deltaZ;
            deltaZ = ( tanf( a ) * x ) - y;

            // delta Z < 0
            //--------------
            if ( deltaZ < 0.0f )
            {
                f32 temp =  y / x;
                if ( f32_Abs( temp ) > 1.0f )
                {
                    a = MTH_PIBY2 - atanf( x / y );
                }
                else
                {
                    a = atanf( temp );
                }
                a += 0.1f; 
                deltaZ = ( tanf( a ) * x ) - y;
            }

            // delta Z > 0
            //--------------
            f32 initSpeed = x / f32_Cos( a );
            if ( deltaZ > 0.0001f )
            {
                initSpeed *= f32_Sqrt( f32_Abs( gravity.m_y ) * 0.5f / deltaZ );
            }

            f32 rotAngle = MTH_PI + a;
            if ( toTargetPos.m_x > 0.0 )
            {
                rotAngle = a;
            }

            Vec2d startSpeed = Vec2d::Right.Rotate( -rotAngle );
            startSpeed.normalize();
            startSpeed *= f32_Abs( f32_Cos(a) ) * initSpeed;
            startSpeed.m_y = f32_Sin( a ) * initSpeed;

            return startSpeed / LOGICDT;
        }

        bbool isTargetStillValid( Actor* _actor, Actor* _target )
        {
            // check that the victim is still valid:
            // not deleted/inactive, not dead, hasn't changed faction ([un-]darktoonified)...
            // TODO: use a query event?

            if ( !_target )
            {
                return bfalse;
            }

            if ( !_target->isActive() ||
                !_target->isEnabled() )
            {
                return bfalse;
            }

            if ( AIComponent* targetAI = _target->GetComponent<AIComponent>() )
            {
                if (targetAI->getHealth() <= 0)
                {
                    return bfalse;
                }
            }

            DepthRange actorDepth(_actor->getDepth());
            DepthRange targetDepth(_target->getDepth());
            if (actorDepth != targetDepth)
                return bfalse;

            return AIUtils::isTarget(getFaction(_target), getFaction(_actor));
        }

        void growAABB( const LinkComponent* _linkComponent )
        {
            Actor* _actor = _linkComponent->GetActor();
            AABB aabb(_actor->getPos());

            const LinkComponent::ChildrenList& children = _linkComponent->getChildren();
            const u32 childrenCount = children.size();
            for (u32 i = 0; i < childrenCount; i++)
            {
                if ( Pickable* child = SceneObjectPathUtils::getObjectFromRelativePath(_actor, children[i].getPath()) )
                {
                    aabb.grow(child->getAABB());
                }
            }

            _actor->growRelativeAABBFromAbsolute(aabb);
        }

        void growAABB( const ObjBinding* _objBinding )
        {
            Actor* actor = static_cast<Actor*>(_objBinding->getOwner());
            AABB aabb(actor->getPos());

            const ObjBinding::ChildrenList& childList = _objBinding->getChildren();
            for (ObjBinding::ChildrenList::const_iterator it = childList.begin(); it !=childList.end(); ++it)
            {
                ObjectRef childRef = it->m_child;
                Actor* child =  static_cast<Actor*>(childRef.getObject());

                if (child)
                {
                    aabb.grow(child->getAABB());
                }
            }

            actor->growRelativeAABBFromAbsolute(aabb);
        }

        bbool isAnimFinished( AnimLightComponent* _animComponent, AnimatedComponent* _animatedComponent )
        {
            if ( _animatedComponent )
            {
                return _animatedComponent->isMainNodeFinished();
            }
            else if ( _animComponent )
            {
                return _animComponent->isSubAnimFinished();
            }
            else
            {
                // There should be one or the other.
                ITF_ASSERT_MSG(0, "This actor has no AnimLightComponent/AnimatedComponent! Did you forget to add one?");
                return bfalse;
            }
        }

        bbool isAnimLooped( AnimLightComponent* _animComponent, AnimatedComponent* _animatedComponent )
        {
            if ( _animatedComponent )
            {
                return _animatedComponent->isAnimNodeLooped();
            }
            else if ( _animComponent )
            {
                return _animComponent->isAnimLooped();
            }
            else
            {
                // There should be one or the other.
                ITF_ASSERT_MSG(0, "This actor has no AnimLightComponent/AnimatedComponent! Did you forget to add one?");
                return bfalse;
            }
        }

        bbool matchEvent( const Event* _registeredEvent, const Event* _receivedEvent )
        {
            if ( _registeredEvent &&
                 _receivedEvent->IsClassCRC(_registeredEvent->GetObjectClassCRC()))
            {
                if ( const EventTrigger* registeredTrigger = _registeredEvent->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
                {
                    EventTrigger* receivedTrigger = (EventTrigger*)_receivedEvent;
                    return receivedTrigger->getActivated() == registeredTrigger->getActivated();
                }
                else if ( const EventGeneric* registeredGeneric = _registeredEvent->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
                {
                    EventGeneric* receivedGeneric = (EventGeneric*)_receivedEvent;
                    return receivedGeneric->getId() == registeredGeneric->getId();
                }
                else if ( const EventStickOnPolyline* registeredStick = _registeredEvent->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
                {
                    EventStickOnPolyline* receivedStick = (EventStickOnPolyline*)_receivedEvent;
                    return receivedStick->isSticked() == registeredStick->isSticked();
                }
                else
                {
                    return btrue;
                }
            }

            return bfalse;
        }

        ITF::AIUtils::EdgeType getEdgeType( const PolyLineEdge & _edge )
        {
            return getEdgeType(_edge.m_normalizedVector);
        }
        ITF::AIUtils::EdgeType getEdgeType( const Vec2d & _dir )
        {
            f32 angle = _dir.getAngle();
            angle = canonizeAngle(angle);
            angle = f32_Abs(angle);

            const f32 wallAngle = 1.107148718f;
            const f32 ceilingAngle = MTH_PI - wallAngle;

            if (angle < wallAngle)
                return EdgeType_Floor;
            else if (angle < ceilingAngle)
                return EdgeType_Wall;
            else
                return EdgeType_Ceiling;

        }

        Color getEdgeColor( const PolyLineEdge & _edge )
        {
            return getEdgeColor(getEdgeType(_edge));
        }

        Color getEdgeColor( EdgeType _type )
        {
            Color color = Color::green();
            if (_type == EdgeType_Wall)
                color = Color::blue();
            else if (_type == EdgeType_Ceiling)
                color = Color::pink();
            return color;
        }

        f32 getSceneZFromPickable( const Pickable* _pickable )
        {
            const Scene* scene = _pickable->getScene();

            if ( scene )
            {
                const SubSceneActor* subSceneActor = scene->getSubSceneActorHolder();

                if ( subSceneActor )
                {
                    return subSceneActor->getPos().m_z;
                }
            }

            return 0.f;
        }

        //*****************************************************************************

        Actor* LinkIterator::getNextRelativeActor()
        {
            const Actor* parent = m_linkComponent->GetActor();

            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            const u32 childrenCount = children.size();

            for (; m_i < childrenCount; m_i++)
            {
                if ( Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(parent, children[m_i].getPath()) )
                {
#ifndef ITF_CONSOLE_FINAL
                    if ( m_ignoreErroneousChildren && pickable->hasDataError() )
                    {
                        continue;
                    }
#endif
                    if ( pickable->getObjectType() == BaseObject::eActor )
                    {
                        m_i++;
                        return static_cast<Actor*>(pickable);
                    }
                }
            }

            return NULL;
        }

        Actor* LinkIterator::getNextRelativeActorTagFilter( const StringID& _tagFilter, bbool _exclude )
        {
            const Actor* parent = m_linkComponent->GetActor();

            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            const u32 childrenCount = children.size();

            for (; m_i < childrenCount; m_i++)
            {
                const ChildEntry& child = children[m_i];

                if (_exclude)
                {
                    if ( child.hasTag(_tagFilter) )
                    {
                        continue;
                    }
                }
                else
                {
                    if ( !child.hasTag(_tagFilter) )
                    {
                        continue;
                    }
                }

                if ( Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(parent, child.getPath()) )
                {
#ifndef ITF_CONSOLE_FINAL
                    if ( m_ignoreErroneousChildren && pickable->hasDataError() )
                    {
                        continue;
                    }
#endif
                    if ( pickable->getObjectType() == BaseObject::eActor )
                    {
                        m_i++;
                        return static_cast<Actor*>(pickable);
                    }
                }
            }

            return NULL;
        }

        Frise* LinkIterator::getNextRelativeFrise()
        {
            const Actor* parent = m_linkComponent->GetActor();

            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            const u32 childrenCount = children.size();

            for (; m_i < childrenCount; m_i++)
            {
                if ( Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(parent, children[m_i].getPath()) )
                {
#ifndef ITF_CONSOLE_FINAL
                    if ( m_ignoreErroneousChildren && pickable->hasDataError() )
                    {
                        continue;
                    }
#endif
                    if ( pickable->getObjectType() == BaseObject::eFrise )
                    {
                        m_i++;
                        return static_cast<Frise*>(pickable);
                    }
                }
            }

            return NULL;
        }

        //*****************************************************************************

        Actor* ActorBindIterator::getNextChild()
        {          
            const ObjectRefList& children = m_actor->getChildrenBindHandler().getChildren();

            const u32 childrenCount = children.size();

            for (; m_i < childrenCount; m_i++)
            {
                Actor* child = static_cast<Actor*>(children[m_i].getObject());

                if (!child)
                {
                    continue;
                }

#ifndef ITF_CONSOLE_FINAL
                if ( m_ignoreErroneousChildren && child->hasDataError() )
                {
                    continue;
                }
#endif

                m_i++;
                return child;
                
            }

            return NULL;
        }


        void sendEventToBoundChildren( const Actor* _actor, Event* _event )
        {
            ActorBindIterator iterator(_actor);
            while ( Actor* child = iterator.getNextChild() )
            {
                child->onEvent(_event);
            }
        }

        void sendEventToLivePlayers(Event* _event)
        {
            Actor* playersBuff[8];
            ActorList players(sizeof(playersBuff)/sizeof(Actor*), MemoryId::mId_Temporary, playersBuff);

            AIUtils::getLivePlayers(players);
            for(u32 index = 0; index < players.size(); ++index)
            {
                Actor* player = players[index];
                if(player)
                {
                    player->onEvent(_event);
                }
            }
        }

        void addListenersToBoundFrises( Actor* _actor )
        {
            AIUtils::ActorBindIterator iterator(_actor);
            while ( Actor* child = iterator.getNextChild() )
            {
                if ( SubSceneActor* group = child->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) )
                {
                    if ( Scene* scene = group->getSubScene() )
                    {
                        const PickableList& frises = scene->getFrises();
                        for (u32 i = 0; i < frises.size(); i++)
                        {
                            Frise* frise = static_cast<Frise*>(frises[i]);
                            frise->addEventListener(_actor->getRef());
                        }
                    }
                }
            }
        }

        void removeListenersToBoundFrises( Actor* _actor )
        {
            AIUtils::ActorBindIterator iterator(_actor);
            while ( Actor* child = iterator.getNextChild() )
            {
                if ( SubSceneActor* group = child->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) )
                {
                    if ( Scene* scene = group->getSubScene() )
                    {
                        const PickableList& frises = scene->getFrises();
                        for (u32 i = 0; i < frises.size(); i++)
                        {
                            Frise* frise = static_cast<Frise*>(frises[i]);
                            frise->removeEventListener(_actor->getRef());
                        }
                    }
                }
            }
        }



        //*****************************************************************************


        f32 walkCalculateEfficiency( f32 _currentSpeed, f32 _maxSpeed, f32 _frictionCoef, f32 _gravity, const Vec2d& _gravityDir, f32 _angle,
                                     f32 _minGroundAngleEfficiencyAngle, f32 _maxGroundAngleEfficiencyAngle,
                                     f32 _minGroundAngleEfficiencyMult, f32 _maxGroundAngleEfficiencyMult,
                                     bbool _useGroundAngle )
        {
            f32 speed = Min(_currentSpeed,_maxSpeed);
            f32 t = speed / _maxSpeed;

            // At less speed, best efficiency
            f32 currentEfficiency = Interpolate(1.f,0.f,t);

            // finally multiply it by the friction of the ground
            f32 frictionFactor = _frictionCoef / _gravity;

            currentEfficiency *= Clamp(frictionFactor,0.f,1.f);

            // The friction will be multiplied by a factor based on the angle of the ground
            if ( _useGroundAngle )
            {
                Vec2d actorDir = Vec2d::Right.Rotate(_angle);
                f32 groundAngle = f32_ACos(_gravityDir.getPerpendicular().dot(actorDir));

                groundAngle = Clamp(groundAngle,_minGroundAngleEfficiencyAngle,_maxGroundAngleEfficiencyAngle);

                t = ( groundAngle - _minGroundAngleEfficiencyAngle ) /
                    ( _maxGroundAngleEfficiencyAngle - _minGroundAngleEfficiencyAngle );

                f32 efficiencyMultiplier = Interpolate(_minGroundAngleEfficiencyMult,_maxGroundAngleEfficiencyMult,t);

                currentEfficiency *= efficiencyMultiplier;
            }
            
            return currentEfficiency;
        }

        // We use this imaginary edge when the ground ends so that we can pivot around something on the extremity
        void getImaginaryAdjacentEdge( bbool _left, const Vec2d& _gravityDir, Vec2d& _dir, Vec2d& _normal )
        {
            // The imaginary adjacent edge will always be a vertical line
            if ( _left )
            {
                _dir = -_gravityDir;
            }
            else
            {
                _dir = _gravityDir;
            }

            _normal = _dir.getPerpendicular();
        }

        void getEdgeDataInTime( f32 _t, PolyLine* _poly, const PolyLineEdge& _edge, u32 _edgeIndex, Vec2d& _pos, Vec2d& _vec, Vec2d& _dir, f32& _length )
        {
            bbool useMoving;

            if ( _t == 1.f )
            {
                useMoving = bfalse;
            }
            else
            {
                if ( _poly->m_physShape && _poly->m_physShape->DynamicCast<PhysShapeMovingPolyline>(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108)) )
                {
                    const PhysShapeMovingPolyline* movingPoly = static_cast<const PhysShapeMovingPolyline*>(_poly->m_physShape);
                    useMoving = movingPoly->hasMoved();
                }
                else
                {
                    useMoving = bfalse;
                }
            }

            if ( useMoving )
            {
                const PhysShapeMovingPolyline* movingPoly = static_cast<const PhysShapeMovingPolyline*>(_poly->m_physShape);
                const PolyLineEdge& prevEdge = movingPoly->getPreviousPolyline()->getEdgeAt(_edgeIndex);
                Vec2d moveVec0 = _edge.getPos() - prevEdge.getPos();
                Vec2d edgePos1Prev = prevEdge.getPos() + prevEdge.m_vector;
                Vec2d edgePos1Next = _edge.getPos() + _edge.m_vector;
                Vec2d moveVec1 = edgePos1Next - edgePos1Prev;

                _pos = prevEdge.getPos() + (moveVec0*_t);

                Vec2d edgePos1 = edgePos1Prev + (moveVec1*_t);

                _vec = edgePos1 - _pos;
                _length = _vec.norm();
                _dir = _vec;
                _dir.normalize();
            }
            else
            {
                _pos = _edge.getPos();
                _dir = _edge.m_normalizedVector;
                _vec = _edge.m_vector;
                _length = _edge.m_length;
            }
        }

        void getAdjacentEdgeData( PolyLine* _stickedLine, const PolyLineEdge& _edge, u32 _edgeIndex, const Vec2d& _gravityDir, 
                                  bbool _back, f32 _t, f32 _relDist, AdjacentEdgeData& _adjacentEdgeData )
        {
            // Calculate the data of the edge where we are going and the angles with it
            PolyLine* adjPoly;
            u32 adjEdge;

            AIUtils::getAdjacentEdge(_stickedLine,_edgeIndex,!_back,adjPoly,adjEdge);

            Vec2d edgePos;
            Vec2d edgeVec;
            Vec2d edgeDir;
            f32 edgeLength;

            getEdgeDataInTime(_t,_stickedLine,_edge,_edgeIndex,edgePos,edgeVec,edgeDir,edgeLength);

            if ( _back )
            {
                if ( adjEdge == U32_INVALID )
                {
                    _adjacentEdgeData.m_polyline = NULL;
                    getImaginaryAdjacentEdge(btrue,_gravityDir,_adjacentEdgeData.m_adjacentEdgeDir,_adjacentEdgeData.m_adjacentEdgeNormal);
                    _adjacentEdgeData.m_adjacentEdgePos = edgePos - _adjacentEdgeData.m_adjacentEdgeDir;
                    _adjacentEdgeData.m_adjacentEdgeIndex = U32_INVALID;
                    _adjacentEdgeData.m_adjacentEdgeLength = 1.f;
                    _adjacentEdgeData.m_adjacentEdgeAngle = MTH_PIBY2;
                }
                else
                {
                    _adjacentEdgeData.m_polyline = adjPoly;
                    _adjacentEdgeData.m_adjacentEdgeIndex = adjEdge;

                    const PolyLineEdge& newEdge = adjPoly->getEdgeAt(_adjacentEdgeData.m_adjacentEdgeIndex);

                    Vec2d adjEdgePos;
                    Vec2d adjEdgeVec;
                    Vec2d adjEdgeDir;
                    f32 adjEdgeLength;

                    getEdgeDataInTime(_t,adjPoly,newEdge,adjEdge,adjEdgePos,adjEdgeVec,adjEdgeDir,adjEdgeLength);

                    _adjacentEdgeData.m_adjacentEdgePos = adjEdgePos;
                    _adjacentEdgeData.m_adjacentEdgeDir = adjEdgeDir;
                    _adjacentEdgeData.m_adjacentEdgeNormal = _adjacentEdgeData.m_adjacentEdgeDir.getPerpendicular();
                    _adjacentEdgeData.m_adjacentEdgeLength = adjEdgeLength;
                    _adjacentEdgeData.m_adjacentEdgeAngle = getVec2Angle(adjEdgeDir);
                }

                _adjacentEdgeData.m_dotProdEdges = Clamp((-_adjacentEdgeData.m_adjacentEdgeDir).dot(edgeDir),-1.f,1.f);
                _adjacentEdgeData.m_crossProdEdges = (-_adjacentEdgeData.m_adjacentEdgeDir).cross(edgeDir);
                _adjacentEdgeData.m_edgesCommonPoint = edgePos;
                _adjacentEdgeData.m_offset = f32_Abs(_relDist);
                _adjacentEdgeData.m_normalA = _adjacentEdgeData.m_adjacentEdgeNormal;
                _adjacentEdgeData.m_normalB = edgeDir.getPerpendicular();
            }
            else
            {
                if ( adjEdge == U32_INVALID )
                {
                    _adjacentEdgeData.m_polyline = NULL;
                    getImaginaryAdjacentEdge(bfalse,_gravityDir,_adjacentEdgeData.m_adjacentEdgeDir,_adjacentEdgeData.m_adjacentEdgeNormal);
                    _adjacentEdgeData.m_adjacentEdgePos = edgePos + edgeVec;
                    _adjacentEdgeData.m_adjacentEdgeIndex = U32_INVALID;
                    _adjacentEdgeData.m_adjacentEdgeLength = 1.f;
                    _adjacentEdgeData.m_adjacentEdgeAngle = -MTH_PIBY2;
                }
                else
                {
                    _adjacentEdgeData.m_polyline = adjPoly;
                    _adjacentEdgeData.m_adjacentEdgeIndex = adjEdge;

                    const PolyLineEdge& newEdge = adjPoly->getEdgeAt(_adjacentEdgeData.m_adjacentEdgeIndex);

                    Vec2d adjEdgePos;
                    Vec2d adjEdgeVec;
                    Vec2d adjEdgeDir;
                    f32 adjEdgeLength;

                    getEdgeDataInTime(_t,adjPoly,newEdge,adjEdge,adjEdgePos,adjEdgeVec,adjEdgeDir,adjEdgeLength);

                    _adjacentEdgeData.m_adjacentEdgePos = adjEdgePos;
                    _adjacentEdgeData.m_adjacentEdgeDir = adjEdgeDir;
                    _adjacentEdgeData.m_adjacentEdgeNormal = _adjacentEdgeData.m_adjacentEdgeDir.getPerpendicular();
                    _adjacentEdgeData.m_adjacentEdgeLength = adjEdgeLength;
                    _adjacentEdgeData.m_adjacentEdgeAngle = getVec2Angle(adjEdgeDir);
                }

                _adjacentEdgeData.m_dotProdEdges = Clamp((-edgeDir).dot(_adjacentEdgeData.m_adjacentEdgeDir),-1.f,1.f);
                _adjacentEdgeData.m_crossProdEdges = (-edgeDir).cross(_adjacentEdgeData.m_adjacentEdgeDir);
                _adjacentEdgeData.m_edgesCommonPoint = edgePos + edgeVec;
                _adjacentEdgeData.m_offset = _relDist - edgeLength;
                _adjacentEdgeData.m_normalA = edgeDir.getPerpendicular();
                _adjacentEdgeData.m_normalB = _adjacentEdgeData.m_adjacentEdgeNormal;
            }
        }

        bbool isContactBlockingObstacle( const Vec2d& _edgePos0, const Vec2d& _edgeDir, f32 _edgeLength, const class GameMaterial_Template* _gameMaterial,
                                         PolyLine* _poly, const PolyLineEdge* _edge, f32 _t,
                                         const SCollidableContact& _contact, const Vec2d& _moveDir, f32 _radius,
                                         ObjectRef _stickedPoly, u32 _stickedEdge, f32 _relDist, const Vec2d& _gravityDir, bbool _treatCornerCase )
        {
            if ( _contact.m_edgeIndex == _stickedEdge && _contact.m_collidableUserData == _stickedPoly )
            {
                return bfalse;
            }

            Vec2d edgeNorm = _edgeDir.getPerpendicular();

            f32 normalProjection = _contact.m_normal.dot(edgeNorm);

            // If the normal of the collision goes in the same direction as the normal of the edge then
            // we skip it
            if ( normalProjection >= -MTH_EPSILON )
            {
                return bfalse;
            }

            f32 distToContact = ( _contact.m_testPosition - _contact.m_contactPoint ).sqrnorm();
            f32 distThreshold = (_radius*_radius)-0.001f;

            if ( _moveDir != Vec2d::Zero )
            {
                // If we are moving along the edge just on the surface, or out of itwe don't consider it an obstacle
                if ( distToContact >= distThreshold )
                {
                    if ( _contact.m_t <= 0.005f )
                    {
                        // We need a double test because on corner cases we may adjust the speed not in the direction of the contact but
                        // in the direction of the edge
                        f32 moveDotP = _contact.m_normal.dot(_moveDir);

                        if ( moveDotP < 0.02f )
                        {
                            // If we go in the opposite direction of the contact normal
                            return bfalse;
                        }
                    }
                    else if ( _contact.m_t >= 0.99f )
                    {
                        return bfalse;
                    }
                }
            }
            else
            {
                if ( distToContact >= distThreshold )
                {
                    return bfalse;
                }
            }

            if ( _gameMaterial && !_gameMaterial->isSolid() )
            {
                // For non-solid polylines, we want to collide with them only when we really go opposite to its normal
                if ( _moveDir != Vec2d::Zero )
                {
                    f32 dotP = edgeNorm.dot(_moveDir);

                    if ( dotP >= -0.2f )
                    {
                        return bfalse;
                    }
                }
                else
                {
                    return bfalse;
                }
            }

            {
                // Special rule for corners. We only collide with it if the center of our character is on top
                // of the half of the normal between the two edges
                Vec2d vecToPos = _contact.m_testPosition - _edgePos0;
                f32 distance = _edgeDir.dot(vecToPos);

                if ( distance <= 0.f || distance >= _edgeLength )
                {
                    AdjacentEdgeData adjacentEdgeData;
                    f32 halfLength = _edgeLength*0.5f;

                    getAdjacentEdgeData(_poly,*_edge,_contact.m_edgeIndex,_gravityDir,distance<halfLength,_t,_relDist,adjacentEdgeData);

                    f32 angleBetweenNormals = f32_ACos(adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB));

                    // Find out what's our angle between ourselves and the normal from the edge from the left
                    if ( angleBetweenNormals )
                    {
                        Vec2d myNorm = _contact.m_testPosition - adjacentEdgeData.m_edgesCommonPoint;
                        myNorm.normalize();

                        f32 angle = f32_ACos(adjacentEdgeData.m_normalA.dot(myNorm));

                        if ( ( distance < halfLength && angle < (angleBetweenNormals*0.5f) ) ||
                            ( distance > halfLength && angle > (angleBetweenNormals*0.5f) ) )
                        {
                            return bfalse;
                        }
                    }
                }
            }

            // If we are going from one edge to another we cannot consider the next edge as an obstacle
            // This issue was found on RAYMAN_Colas_MULTI_glisse, the character was slowing down when sliding
            // from one edge to another
            PolyLine* currentPoly;
            const PolyLineEdge* currentEdge;

            getPolyLine(_stickedPoly,_stickedEdge,currentPoly,currentEdge);

            if ( currentPoly )
            {
                AdjacentEdgeData adjacentEdgeData;

                if ( currentEdge )
                {
                    bbool checkBack = _relDist < (currentEdge->m_length*0.5f);

                    getAdjacentEdgeData(currentPoly,*currentEdge,_stickedEdge,_gravityDir,checkBack,_t,_relDist,adjacentEdgeData);

                    if ( adjacentEdgeData.m_polyline == _poly &&
                        adjacentEdgeData.m_adjacentEdgeIndex == _contact.m_edgeIndex &&
                        adjacentEdgeData.m_crossProdEdges >= 0.f )
                    {
                        return bfalse;
                    }
                }
            }

            if ( _treatCornerCase )
            {
                // If we were not sticked we check if it's a corner case
                Vec2d vecToPos = _contact.m_testPosition - _edgePos0;
                f32 dist = _edgeDir.dot(vecToPos);

                if ( dist < 0.f || dist > _edgeLength )
                {
                    AdjacentEdgeData adjacentEdgeData;

                    getAdjacentEdgeData(_poly,*_edge,_contact.m_edgeIndex,_gravityDir,dist < 0.f,_t,_relDist,adjacentEdgeData);

                    if ( adjacentEdgeData.m_crossProdEdges > 0.f && adjacentEdgeData.m_adjacentEdgeIndex != U32_INVALID )
                    {
                        f32 angleBetweenNormals = f32_ACos(adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB));

                        // Find out what's our angle between ourselves and the normal from the edge from the left
                        if ( angleBetweenNormals )
                        {
                            Vec2d myNorm = _contact.m_testPosition - adjacentEdgeData.m_edgesCommonPoint;
                            myNorm.normalize();

                            f32 angle = f32_ACos(adjacentEdgeData.m_normalA.dot(myNorm));

                            // With the angle we can check if we are within the boundaries of the edge
                            if ( ( dist < 0.f && angle >= (angleBetweenNormals*0.5f) ) ||
                                 ( dist > 0.f && angle <= (angleBetweenNormals*0.5f) ) )
                            {
                            }
                            else
                            {
                                // It looks like it's a corner case but this is not the edge we want to process
                                // Most probably it later in the contacts array so we wait for it to arrive
                                // By setting speedSet to true we avoid any process hoping that we will get the
                                // right edge later in the loop
                                return bfalse;
                            }
                        }
                    }
                }
            }

            return btrue;
        }

        Vec2d findFreeCollisionPosition( const Vec2d& _pos, f32 _radius, f32 _depth, bbool _iterate )
        {
            Vec2d retPos = _pos;
            PhysContactsContainer contacts;
            PhysShapeCircle circle(_radius);
            bbool blocked = bfalse;
            static const u32 numIterations = 3;
            u32 iteration = 0;
            f32 distThreshold = (_radius*_radius)-0.001f;

            do
            {
                contacts.clear();
                blocked = bfalse;

                PHYSWORLD->checkEncroachment(retPos,retPos,0.f,&circle,ECOLLISIONFILTER_ENVIRONMENT,_depth,contacts);

                u32 numContacts = contacts.size();

                for ( u32 i = 0; i < numContacts; i++ )
                {
                    const SCollidableContact& contact = contacts[i];

                    if ( contact.m_edgeIndex == U32_INVALID )
                    {
                        continue;
                    }

                    f32 distToContact = ( contact.m_testPosition - contact.m_contactPoint ).sqrnorm();

                    if ( distToContact >= distThreshold )
                    {
                        continue;
                    }

                    retPos = contact.m_contactPoint - ( contact.m_normal * _radius );
                    blocked = btrue;
                    break;
                }

                iteration++;

            } while ( blocked && _iterate && iteration < numIterations );

            return retPos;
        }

        void getSwimingPolylineData( const PolyLine* _polyLine, const Vec3d& _pos, const Vec2d& _gravityDir, f32 _surfaceRange,
                                     u32& _surfaceEdge, f32& _surfaceEdgeT, bbool& _onSurface,
                                     u32& _closestEdge, f32& _closestEdgeT, bbool& _isInside )
        {
            f32 minDist = 0.f;
            f32 minDistSurface = 0.f;
            bbool minSurfaceSet = bfalse;
            bbool minEdgeSet = bfalse;
            const Vec2d& _pos2d = _pos.truncateTo2D();
            Vec2d end_pos2d_down = _pos2d + (_gravityDir*10000.f);
            Vec2d end_pos2d_up = _pos2d + (_gravityDir*(-10000.f));
            Vec2d intersection;

            _onSurface = bfalse;
            _isInside = _polyLine->isPointInside(_pos2d);

            // Find the closest edge
            for ( u32 i = 0; i < _polyLine->getVectorsCount(); i++ )
            {
                const PolyLineEdge& edge = _polyLine->getEdgeAt(i);
                Vec2d edgeToPos = _pos2d - edge.getPos();
                f32 t = edge.m_normalizedVector.dot(edgeToPos)/edge.m_length;

                t = Clamp(t,0.f,1.f);

                Vec2d pos = edge.getPos() + (edge.m_vector*t);
                f32 dist = ( pos - _pos2d ).sqrnorm();

                if ( !minEdgeSet || dist < minDist )
                {
                    _closestEdge = i;
                    _closestEdgeT = t;
                    minDist = dist;
                    minEdgeSet = btrue;
                }

                if ( edge.m_normalizedVector.getPerpendicular().dot(_gravityDir) > 0.f )
                {
                    continue;
                }

                if ( Segment_Segment(_pos2d,end_pos2d_up,edge.getPos(),edge.getPos()+edge.m_vector,intersection) )
                {
                    f32 dist = ( intersection - _pos2d ).sqrnorm();

                    if ( !minSurfaceSet || dist < minDistSurface )
                    {
                        _surfaceEdge = i;
                        _surfaceEdgeT = ( intersection - edge.getPos() ).norm() / edge.m_length;
                        _onSurface = dist < _surfaceRange;
                        minDistSurface = dist;
                        minSurfaceSet = btrue;
                    }
                }
                else if ( !_isInside &&
                    Segment_Segment(_pos2d,end_pos2d_down,edge.getPos(),edge.getPos()+edge.m_vector,intersection) )
                {
                    f32 dist = ( intersection - _pos2d ).sqrnorm();

                    if ( !minSurfaceSet || dist < minDistSurface )
                    {
                        _surfaceEdge = i;
                        _surfaceEdgeT = ( intersection - edge.getPos() ).norm() / edge.m_length;
                        _onSurface = dist < _surfaceRange;
                        minDistSurface = dist;
                        minSurfaceSet = btrue;
                    }
                }
            }

            if ( _onSurface && _surfaceEdge < _polyLine->getVectorsCount() )
            {
                const PolyLineEdge& surfaceEdge = _polyLine->getEdgeAt(_surfaceEdge);
                Vec2d surfacePoint = surfaceEdge.getPos() + (surfaceEdge.m_vector * _surfaceEdgeT);
                Vec2d checkPos = surfacePoint + Vec2d::Up;

                const RegionsManager::RegionDataList* regionList = REGIONS_MANAGER->getRegions(_pos.m_z);

                if ( regionList )
                {
                    for ( u32 i = 0; i < regionList->size(); i++ )
                    {
                        const RegionsManager::RegionData& regionData = (*regionList)[i];

                        if ( regionData.m_id != s_RegionName_Swim )
                        {
                            continue;
                        }

                        PolyLine* poly = AIUtils::getPolyLine(regionData.m_ref);

                        if ( !poly || poly == _polyLine )
                        {
                            continue;
                        }

                        if ( !poly->isPointInside(checkPos) )
                        {
                            continue;
                        }

                        _onSurface = bfalse;
                        break;
                    }
                }
            }
        }

        Color getColor( const Actor* _actor )
        {
            if ( AnimLightComponent* anim = _actor->GetComponent<AnimLightComponent>() )
            {
                return anim->getColorFactor() * anim->getColor();
            }

            return Color::white();
        }

        void disableCollision( Actor* _actor, bbool _disable /*= btrue*/ )
        {
            EventDisableCollision eventDisableCollision(_disable);
            _actor->onEvent(&eventDisableCollision);
        }

        void setAlpha(Pickable * _pickable, f32 _value)
        {
            if ( _pickable )
            {
                Actor* actor = _pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                if ( actor )
                {
                    SubSceneActor* pSubScene = static_cast<SubSceneActor*> (actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)));
                    if (pSubScene)
                    {
                        pSubScene->setAlpha(_value);
                    }
                    else
                    {
                        GraphicComponent* pGraphic = actor->GetComponent<GraphicComponent>();
                        if (pGraphic)
                            pGraphic->setAlpha(_value);
                    }
                }
                else if (_pickable->getObjectType() == BaseObject::eFrise)
                {
                    Frise* pFrise = (Frise*)_pickable;
                    pFrise->setglobalAlphaFade(_value);
                }
            }
        }

        f32 calculateAlpha( const f32 dist, const f32 _alphaRadialProgressionSpeed, const f32 _time, const f32 _alphaFadeTime, const bbool _in )
        {
            const f32 timeToTarget = dist / _alphaRadialProgressionSpeed;

            const f32 ratio = Clamp((_time - timeToTarget)/_alphaFadeTime,0.0f,1.0f);
            const f32 alphaMax = _in?1.0f:0.0f;
            const f32 alphaMin = 1.0f - alphaMax;
            const f32 value = (ratio * (alphaMax - alphaMin)) + alphaMin;
            return value;
        }

        bbool setRadialAlpha( Pickable* pickable, Vec2d _pos,const f32 _alphaRadialProgressionSpeed, const f32 _time, const f32 _alphaFadeTime, const bbool _in  )
        {
            bbool done = btrue;
            if ( pickable )
            {
                Actor* actor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                if ( actor )
                {
                    SubSceneActor* pSubScene = static_cast<SubSceneActor*> (actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)));
                    if (pSubScene)
                    {
                        if ( Scene* pScene = pSubScene->getSubScene())
                        {
                            const PickableList& children = pScene->getPickableList();
                            const u32 childrenCount = children.size();

                            for (u32 i = 0; i < childrenCount; i++)
                            {
                                done = setRadialAlpha(children[i], _pos, _alphaRadialProgressionSpeed, _time, _alphaFadeTime, _in) && done;
                            }
                        }
                    }
                    else
                    {
                        GraphicComponent* pGraphic = actor->GetComponent<GraphicComponent>();
                        if (pGraphic)
                        {
                            const f32 dist = (actor->get2DPos() - _pos).norm();
                            const f32 value = calculateAlpha(dist, _alphaRadialProgressionSpeed, _time, _alphaFadeTime, _in);
                            const f32 targetValue = _in?1.0f:0.0f;
                            if (value != targetValue)
                                done = bfalse;
                            pGraphic->setAlpha(value);
                        }
                    }
                }
                else if (pickable->getObjectType() == BaseObject::eFrise)
                {
                    Frise* pFrise = (Frise*)pickable;
                    const f32 dist = (pickable->getAnchorPos().truncateTo2D()- _pos).norm();
                    const f32 value = calculateAlpha(dist, _alphaRadialProgressionSpeed, _time, _alphaFadeTime, _in);
                    const f32 targetValue = _in?1.0f:0.0f;
                    if (value != targetValue)
                        done = bfalse;
                    pFrise->setglobalAlphaFade(value);
                }
            }
            return done;
        }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        void debugDrawPolylineEdgeTypes( const PolyLine& _poly, const Vec2d& _offset /*= Vec2d::Zero*/, f32 _duration /*= 0*/ )
        {
            const f32 z = _poly.getDepth();

            const u32 edgeCount = _poly.getPosCount();
            for (u32 i = 0; i < edgeCount; i++)
            {
                const PolyLineEdge& edge = _poly.getEdgeAt(i);

                Color color = Color::green();

                EdgeType type = getEdgeType(edge);

                if (type == EdgeType_Wall)
                    color = Color::blue();
                else if (type == EdgeType_Ceiling)
                    color = Color::pink();

                DebugDraw::edge(
                    _offset + edge.getPos(),
                    _offset + edge.getPos() + edge.m_vector,
                    edge.m_normalizedVector.getPerpendicular(),
                    z, color, _duration);
                //draw angles 
                //DebugDraw::text(_offset + edge.getPos(),z,Color::red(),"%.2f",edge.m_normalizedVector.getAngle() * 180.0f / MTH_PI);
            }
        }
#endif // ITF_SUPPORT_DEBUGFEATURE

        class CompareOnDirection
        {
        public:

            CompareOnDirection(const Vec2d& _direction)
                : m_direction(_direction)
            {}

            typedef Actor* actorPtr;
            i32 operator() (const actorPtr& _A, const actorPtr& _B) const
            {
                f32 dotA = m_direction.dot(_A->get2DPos());
                f32 dotB = m_direction.dot(_B->get2DPos());

                if (dotA < dotB)
                {
                    return -1;
                }
                else if (dotA > dotB)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }

        private:

            Vec2d m_direction;
        };

        void sortOnDirection( const Vec2d& _direction, ActorList& _players )
        {
            _players.quickSort(CompareOnDirection(_direction));
        }

        ECharacterSize calculateCharacterSize( f32 _initialScale, f32 _currentScale )
        {
            if ( _initialScale > _currentScale )
            {
                return CHARACTERSIZE_SMALL;
            }
            else if ( _initialScale < _currentScale )
            {
                return CHARACTERSIZE_BIG;
            }
            else
            {
                return CHARACTERSIZE_NORMAL;
            }
        }

        f32 limit(const f32 _value, const f32 _mini, const f32 _max)
        {
            if(_value < _mini)
            {
                return _mini;
            }
            if(_value > _max)
            {
                return _max;
            }
            return _value;
        }

        void getNearestPolylineInfo(const Vec3d & _source, Vec3d& _defaultPos, f32& _sceneZ )
        {
            World* pWorld = CURRENTWORLD;
            if(!pWorld)
                return;

            u32 ii;
            f32 bestDist = F32_INFINITY;
            bbool found = bfalse;

            for(u32 i = 0; i < pWorld->getSceneCount(); ++i)
            {
                Scene* pScene = pWorld->getSceneAt(i);
                ITF_ASSERT(pScene);
                if(!pScene)
                    continue;

                for (ii= 0; ii < pScene->getFrises().size(); ii++)
                {
                    const Frise* pFrieze = (Frise*)(pScene->getFrises()[ii]);
                    const SafeArray<PolyLine*>* pCollisionData = pFrieze->getCollisionData_WorldSpace();

                    if ( !pCollisionData || !pCollisionData->size())
                        continue;

                    for (u32 j= 0 ; j<pFrieze->getPosCount(); j++)
                    {
                        f32 dist = f32_Abs(pFrieze->getPosAt(j).m_x - _source.m_x);
                        if ((!found) || (dist<bestDist))
                        {
                            bestDist = dist;
                            found = btrue;
                            _defaultPos = (pFrieze->getPosAt(j) + pFrieze->getEdgeAt(j).m_vector * 0.5f + pFrieze->getEdgeAt(j).m_normalizedVector.getPerpendicular() /** p->getRadius()*/ * 1.1f).to3d();
                            _defaultPos.m_z = pFrieze->getDepth();
                            _sceneZ = AIUtils::getSceneZFromPickable(pFrieze);
                        }
                    }
                }
            }
        }

        bbool isWallRunEnabledOnPolyline( const PolyLineEdge* _curEdge, const PolyLineEdge* _prevEdge )
        {
            f32 prevAngle = getVec2Angle(_prevEdge->m_normalizedVector);
            f32 newAngle = getVec2Angle(_curEdge->m_normalizedVector);
            f32 dif = getShortestAngleDelta(prevAngle,newAngle);

            if ( fabs(dif) > StickToPolylinePhysComponent::s_wallRunAngleThreshold )
                return bfalse;

            return btrue;                    
        }

        bbool isWallRunEnabledOnPolyline( const Vec2d& _currentDir, const Vec2d& _prevDir )
        {
            f32 prevAngle = getVec2Angle(_prevDir);
            f32 newAngle = getVec2Angle(_currentDir);
            f32 dif = getShortestAngleDelta(prevAngle,newAngle);

            if ( fabs(dif) > StickToPolylinePhysComponent::s_wallRunAngleThreshold )
                return bfalse;

            return btrue;                    
        }

        int getEdgeOrientationType( const Vec2d& _dir, const Vec2d& _gravity )
        {
            f32 angle = f32_ACos(_gravity.getPerpendicular().dot(_dir));

            if ( angle <= StickToPolylinePhysComponent::s_physWallAngle )
            {
                return StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND;
            }
            else if ( angle <= StickToPolylinePhysComponent::s_physRoofAngle )
            {
                return StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL;
            }
            else
            {
                return StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF;
            }
        }

        void setAlwaysActive( Actor* _bodyPart, const bbool _active )
        {
            if ( _bodyPart == NULL )
                return;

            Scene* scene = _bodyPart->getScene();
            if (scene)
            {
                bbool currentlyActive = (scene->getAlwaysActiveActors().find(_bodyPart) != -1);
                if ( currentlyActive != _active )
                {
                    scene->setAlwaysActiveActor(_bodyPart, _active);
                }
            }
        }

    } // namespace AIUtils
} // namespace ITF
