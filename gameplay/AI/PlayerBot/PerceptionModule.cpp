#include "precompiled_gameplay.h"
#include "core/Macros.h"

#ifdef ITF_SUPPORT_BOT_AUTO

#include "PerceptionModule.h"

#include <algorithm>
#include <cmath>
#include "Components/common/StickToPolylinePhysComponent.h"
#include "managers/GameManager.h"
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#include "engine/physics/PhysBody.h"
#include "engine/physics/PhysWorld.h"
#include "engine/physics/PhysTypes.h"
#include "engine/physics/PhysShapes.h"
#include "engine/scene/world.h"
#include "engine/actors/actor.h"
#include "gameplay/AI/Utils/AIUtils.h"
#include "rayman/gameplay/Ray_GameMaterial.h"
#include "gameplay/GameplayEvents.h"

namespace ITF
{
    class Ray_GameMaterial_Template;

    struct RaycastHit
    {
        RaycastHit()
            : hit(bfalse)
            , point(Vec2d::Zero)
            , t(0.0f)
            , collidableRef(ITF_INVALID_OBJREF)
            , edgeIndex(U32_INVALID)
        {
        }

        bbool hit;
        Vec2d point;
        f32 t;
        SRayCastContact contact;
        ObjectRef collidableRef;
        u32 edgeIndex;
    };

    static RaycastHit raycastEnvironmentClosest(const Vec2d& start, const Vec2d& end, f32 zLayer)
    {
        RaycastHit result;

        PhysRayCastContactsContainer contacts;
        if (!PHYSWORLD->rayCastEnvironment(start, end, ECOLLISIONFILTER_ENVIRONMENT, zLayer, contacts))
        {
            return result;
        }

        f32 bestT = 2.0f;
        SRayCastContact bestContact;
        ObjectRef bestRef = ITF_INVALID_OBJREF;
        u32 bestEdge = U32_INVALID;
        bbool found = bfalse;

        auto consider = [&](f32 tCandidate, u32 edgeCandidate, const SRayCastContact& contact)
        {
            if (tCandidate < 0.0f || tCandidate > 1.0f)
            {
                return;
            }

            if (!found || tCandidate < bestT)
            {
                bestT = tCandidate;
                bestContact = contact;
                bestRef = contact.m_collidableUserData.isValid() ? contact.m_collidableUserData : contact.m_collidable;
                bestEdge = edgeCandidate;
                found = btrue;
            }
        };

        for (u32 i = 0; i < contacts.size(); ++i)
        {
            const SRayCastContact& contact = contacts[i];
            consider(contact.m_t0, contact.m_edgeIndex0, contact);
            consider(contact.m_t1, contact.m_edgeIndex1, contact);
        }

        if (!found)
        {
            return result;
        }

        result.hit = btrue;
        result.contact = bestContact;
        result.collidableRef = bestRef;
        result.edgeIndex = bestEdge;
        result.t = bestT;
        result.point = start + (end - start) * bestT;
        return result;
    }

    PerceptionModule::PerceptionModule(GameManager* gameManager)
    : m_gameManager(gameManager)
    , m_getStanceCallback(nullptr)
    , m_scanTargetsCallback(nullptr)
    , m_groundedFrames(0)
    , m_airborneFrames(0)
    , m_coyoteTimer(0.0f)
    , m_probeNearX(0.8f)
    , m_probeMidX(1.6f)
    , m_probeFarX(3.2f)
    , m_probeUp(2.0f)
    , m_probeDown(8.0f)
    , m_wallProbeOffsetX(0.6f)
    , m_wallProbeHeight(1.0f)
    , m_wallScanMaxHeight(3.0f)
    , m_coyoteTimeWindow(0.15f)
    {

    }

    PerceptionModule::~PerceptionModule()
    {
    }

    void PerceptionModule::update(f32 deltaTime)
    {
        extractPlayerState();
        extractTerrainSensors(deltaTime);
        scanNearbyTargets();
        debugDraw();
    }

    static bbool gatherHazardEdge(const SRayCastContact& contact, PolyLine*& outPoly, const PolyLineEdge*& outEdge)
    {
        outPoly = nullptr;
        outEdge = nullptr;

        if (contact.m_edgeIndex0 != U32_INVALID)
        {
            AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex0, outPoly, outEdge);
        }

        if ((!outPoly || !outEdge) && contact.m_edgeIndex1 != U32_INVALID)
        {
            AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex1, outPoly, outEdge);
        }

        return (outPoly != nullptr && outEdge != nullptr);
    }

    static bbool detectHazardAlongRay(const Vec2d& start, const Vec2d& end, f32 zLayer, f32& outDistance, u32& outType)
    {
        PhysRayCastContactsContainer contacts;
        if (!PHYSWORLD->rayCastEnvironment(start, end, ECOLLISIONFILTER_ENVIRONMENT, zLayer, contacts))
        {
            return bfalse;
        }

        Vec2d ray = end - start;
        const f32 rayLength = ray.norm();
        if (rayLength <= 0.0001f)
        {
            return bfalse;
        }

        f32 bestT = 2.0f;
        u32 bestType = 0;
        bbool hazardFound = bfalse;

        for (u32 i = 0; i < contacts.size(); ++i)
        {
            const SRayCastContact& contact = contacts[i];
            PolyLine* poly = nullptr;
            const PolyLineEdge* edge = nullptr;
            if (!gatherHazardEdge(contact, poly, edge))
            {
                continue;
            }

            const GameMaterial_Template* gameMaterial = World::getGameMaterial(edge->getGameMaterial());
            if (!gameMaterial)
            {
                continue;
            }

            const Ray_GameMaterial_Template* rayMaterial = gameMaterial->DynamicCast<Ray_GameMaterial_Template>(ITF_GET_STRINGID_CRC(Ray_GameMaterial_Template,2777394804));
            if (!rayMaterial)
            {
                continue;
            }

            u32 dangerousLevel = rayMaterial->getDangerousLevel();
            if (dangerousLevel == 0)
            {
                continue;
            }

            f32 t = (contact.m_t0 >= 0.0f) ? contact.m_t0 : contact.m_t1;
            if (t < 0.0f)
            {
                continue;
            }

            if (t < bestT)
            {
                bestT = t;
                bestType = dangerousLevel;
                hazardFound = btrue;
            }
        }

        if (hazardFound)
        {
            outDistance = bestT * rayLength;
            outType = bestType;
        }

        return hazardFound;
    }

    void PerceptionModule::extractTerrainSensors(f32 deltaTime)
    {
        // Defaults
        m_currentState.sensors = TerrainSensors();

        if (!m_gameManager)
            return;

        Player* player = m_gameManager->getPlayer(0);
        if (!player || !player->getActive())
            return;

        Actor* playerActor = player->getActor();
        if (!playerActor)
            return;

        const Vec3d playerPos = playerActor->getPos();
        const f32 zLayer = playerPos.m_z;

        // Update grounded/airborne frames and coyote timer
        if (m_currentState.isGrounded)
        {
            m_groundedFrames++;
            m_airborneFrames = 0;
            m_coyoteTimer = m_coyoteTimeWindow; // refresh while grounded
        }
        else
        {
            m_airborneFrames++;
            if (m_coyoteTimer > 0.0f)
            {
                m_coyoteTimer -= deltaTime;
                m_coyoteTimer = std::max(m_coyoteTimer, 0.0f);
            }
        }
        m_currentState.sensors.groundedFrames = m_groundedFrames;
        m_currentState.sensors.airborneFrames = m_airborneFrames;
        m_currentState.sensors.coyoteTimeRemaining = m_coyoteTimer;

        StickToPolylinePhysComponent* physCompLocal = playerActor->GetComponent<StickToPolylinePhysComponent>();

        Vec2d gravityDir(0.0f, -1.0f);
        if (PHYSWORLD)
        {
            gravityDir = PHYSWORLD->getGravity(playerPos.truncateTo2D(), zLayer);
            if (!gravityDir.IsEqual(Vec2d::Zero, 0.0001f))
            {
                gravityDir.normalize();
            }
        }

        const f32 playerRadius = physCompLocal ? physCompLocal->getRadius() : 1.0f;
        Vec2d actorScale = playerActor->getLocalInitialScale();
        const f32 actorScaleY = (actorScale.m_y != 0.0f) ? actorScale.m_y : 1.0f;

        const f32 miniStepMax = 0.132f * actorScaleY;
        const f32 stepTolerance = miniStepMax + 0.05f;
        const f32 facingSign = playerActor->isFlipped() ? -1.0f : 1.0f;

        auto classifyOrientation = [&](const RaycastHit& hit) -> StickToPolylinePhysComponent::EdgeOrientationType
        {
            if (!hit.hit)
            {
                return StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF;
            }

            PolyLine* poly = nullptr;
            const PolyLineEdge* edge = nullptr;

            if (hit.collidableRef.isValid())
            {
                AIUtils::getPolyLine(hit.collidableRef, hit.edgeIndex, poly, edge);
            }

            if (poly && edge)
            {
                return static_cast<StickToPolylinePhysComponent::EdgeOrientationType>(
                    AIUtils::getEdgeOrientationType(edge->m_normalizedVector, gravityDir));
            }

            return StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL;
        };

        auto sampleVertical = [&](f32 offset) -> RaycastHit
        {
            Vec2d start(playerPos.m_x + offset, playerPos.m_y + m_probeUp);
            Vec2d end(playerPos.m_x + offset, playerPos.m_y - m_probeDown);
            return raycastEnvironmentClosest(start, end, zLayer);
        };

        const f32 nearOffset = m_probeNearX * facingSign;
        const f32 midOffset = m_probeMidX * facingSign;
        const f32 farOffset = m_probeFarX * facingSign;

        RaycastHit nearHit = sampleVertical(nearOffset);
        RaycastHit midHit = sampleVertical(midOffset);
        RaycastHit farHit = sampleVertical(farOffset);

        const f32 playerFootY = playerPos.m_y - playerRadius;
        auto processGroundSample = [&](const RaycastHit& hit, bbool& hasGround, f32& distOut, f32& hitYOut)
        {
            hasGround = bfalse;
            distOut = 0.0f;
            hitYOut = hit.hit ? hit.point.m_y : playerFootY;
            StickToPolylinePhysComponent::EdgeOrientationType orient = classifyOrientation(hit);
            if (hit.hit && orient == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND)
            {
                hasGround = btrue;
                distOut = (playerPos.m_y + m_probeUp) - hit.point.m_y;
            }
            return orient;
        };

        f32 nearGroundY = playerFootY;
        f32 midGroundY = playerFootY;
        f32 farGroundY = playerFootY;

        StickToPolylinePhysComponent::EdgeOrientationType nearOrientation = processGroundSample(
            nearHit,
            m_currentState.sensors.hasGroundAheadNear,
            m_currentState.sensors.groundAheadDistNear,
            nearGroundY);

        processGroundSample(
            midHit,
            m_currentState.sensors.hasGroundAheadMid,
            m_currentState.sensors.groundAheadDistMid,
            midGroundY);

        processGroundSample(
            farHit,
            m_currentState.sensors.hasGroundAheadFar,
            m_currentState.sensors.groundAheadDistFar,
            farGroundY);

        f32 stepHeightNear = nearHit.hit ? (nearGroundY - playerFootY) : 0.0f;
        bbool wallAtNear = (nearHit.hit && nearOrientation != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND);
        ITF_UNUSED(midGroundY);
        ITF_UNUSED(farGroundY);

        if (m_currentState.sensors.hasGroundAheadNear)
        {
            const f32 dropDepth = playerFootY - nearGroundY;
            m_currentState.sensors.gapDepth = dropDepth > 0.0f ? dropDepth : 0.0f;
        }
        else
        {
            m_currentState.sensors.gapDepth = wallAtNear ? 0.0f : m_probeDown;
        }

        m_currentState.sensors.slopeAhead = 0.0f;
        const f32 slopeDx = 0.2f * facingSign;
        if (m_currentState.sensors.hasGroundAheadNear)
        {
            RaycastHit slopeHit = sampleVertical(nearOffset + slopeDx);
            bbool slopeGround = bfalse;
            f32 slopeDist = 0.0f;
            f32 slopeGroundY = nearGroundY;
            StickToPolylinePhysComponent::EdgeOrientationType slopeOrientation = processGroundSample(
                slopeHit, slopeGround, slopeDist, slopeGroundY);

            if (slopeGround && slopeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND)
            {
                const f32 dy = slopeGroundY - nearGroundY;
                const f32 absDx = fabsf(slopeDx);
                if (absDx > 0.0001f)
                {
                    m_currentState.sensors.slopeAhead = dy / slopeDx;
                }
            }
        }

        // Front wall detection (step height, wall orientation, and forward ray checks)
        bbool blockedForward = bfalse;
        f32 maxWallHeight = 0.0f;

        if (nearHit.hit)
        {
            const f32 positiveStep = stepHeightNear > 0.0f ? stepHeightNear : 0.0f;
            if (positiveStep > stepTolerance)
            {
                blockedForward = btrue;
                maxWallHeight = std::max(maxWallHeight, positiveStep);
            }
        }

        if (wallAtNear)
        {
            const f32 positiveStep = std::max(stepHeightNear, 0.0f);
            blockedForward = btrue;
            maxWallHeight = std::max(maxWallHeight, positiveStep);
        }

        auto applyForwardRay = [&](const Vec2d& start, const Vec2d& end)
        {
            RaycastHit rayHit = raycastEnvironmentClosest(start, end, zLayer);
            if (!rayHit.hit)
            {
                return;
            }

            StickToPolylinePhysComponent::EdgeOrientationType orient = classifyOrientation(rayHit);
            f32 contactHeight = rayHit.point.m_y - playerFootY;
            if (contactHeight < 0.0f)
            {
                contactHeight = 0.0f;
            }

            if (orient != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND || contactHeight > stepTolerance)
            {
                blockedForward = btrue;
                maxWallHeight = std::max(maxWallHeight, contactHeight);
            }
        };

        const f32 frontBaseX = playerPos.m_x + facingSign * (playerRadius * 0.8f);
        const f32 frontFootY = playerFootY + miniStepMax;
        applyForwardRay(Vec2d(frontBaseX, frontFootY), Vec2d(frontBaseX + facingSign * m_wallProbeOffsetX, frontFootY));

        const f32 chestHeightRelative = m_wallProbeHeight;
        const f32 chestHeightAbs = playerPos.m_y + chestHeightRelative;
        applyForwardRay(Vec2d(frontBaseX, chestHeightAbs), Vec2d(frontBaseX + facingSign * m_wallProbeOffsetX, chestHeightAbs));

        if (blockedForward)
        {
            m_currentState.sensors.frontWall = btrue;
            m_currentState.sensors.frontWallHeight = std::max(0.0f, maxWallHeight);
        }
        else
        {
            m_currentState.sensors.frontWall = bfalse;
            m_currentState.sensors.frontWallHeight = 0.0f;
        }

        // Moving platform detection
        bbool bestPlatformMoving = bfalse;
        Vec2d bestPlatformVel(0.0f, 0.0f);
        Vec2d bestPlatformPos(0.0f, 0.0f);
        bbool bestPlatformPosValid = bfalse;
        ObjectRef bestPlatformRef = ITF_INVALID_OBJREF;
        const f32 minPlatformVelSq = 0.01f;

        if (m_currentState.isGrounded)
        {
            if (physCompLocal)
            {
                const PhysContactsContainer& contacts = physCompLocal->getContacts();
                for (u32 i = 0; i < contacts.size(); ++i)
                {
                    const SCollidableContact& contact = contacts[i];
                    PolyLine* poly = nullptr;
                    const PolyLineEdge* edge = nullptr;
                    if (contact.m_collidableUserData.isValid())
                    {
                        AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);
                    }

                    Vec2d candidateVel(0.0f, 0.0f);
                    bbool candidateMoving = bfalse;
                    Vec2d candidatePos(0.0f, 0.0f);
                    bbool candidatePosValid = bfalse;

                    ObjectRef candidateRef = contact.m_collidableUserData.isValid()
                        ? contact.m_collidableUserData
                        : contact.m_collidable;

                    if (poly)
                    {
                        candidatePos = poly->get2DPos();
                        candidatePosValid = btrue;

                        if (poly->m_physShape && deltaTime > 0.0f)
                        {
                            PhysShapeMovingPolyline* movingShape = poly->m_physShape->DynamicCast<PhysShapeMovingPolyline>(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108));
                            if (movingShape && movingShape->hasMoved())
                            {
                                const PolyLine* prevPoly = movingShape->getPreviousPolyline();
                                if (prevPoly && edge && contact.m_edgeIndex < poly->getPosCount())
                                {
                                    Vec2d currentEdgePos = edge->getPos();
                                    Vec2d prevEdgePos = prevPoly->getPosAt(contact.m_edgeIndex);
                                    Vec2d deltaPos = currentEdgePos - prevEdgePos;
                                    candidateVel = deltaPos / deltaTime;
                                    if (candidateVel.sqrnorm() > minPlatformVelSq)
                                    {
                                        candidateMoving = btrue;
                                    }
                                }
                            }
                        }

                        if (!candidateMoving)
                        {
                            PhysBody* body = poly->m_physBody;
                            if (body)
                            {
                                candidateVel = body->getSpeed();
                                if (!body->isStatic() && candidateVel.sqrnorm() > minPlatformVelSq)
                                {
                                    candidateMoving = btrue;
                                }
                            }
                        }
                    }

                    if (!candidateMoving)
                    {
                        BaseObject* collidableObj = candidateRef.isValid() ? GETOBJECT(candidateRef) : nullptr;
                        if (collidableObj)
                        {
                            Actor* platformActor = collidableObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                            if (platformActor)
                            {
                                if (!candidatePosValid)
                                {
                                    candidatePos = platformActor->get2DPos();
                                    candidatePosValid = btrue;
                                }

                                EventQueryPhysicsData physicsQuery;
                                platformActor->onEvent(&physicsQuery);
                                Vec2d querySpeed = physicsQuery.getWorldSpeed();
                                if (querySpeed.sqrnorm() > minPlatformVelSq)
                                {
                                    candidateVel = querySpeed;
                                    candidateMoving = btrue;
                                }
                            }
                        }
                    }

                    if (!candidateMoving && candidatePosValid && candidateRef == m_lastPlatformRef && deltaTime > 0.0f)
                    {
                        Vec2d deltaPos = candidatePos - m_lastPlatformPos;
                        Vec2d approxVel = deltaPos / deltaTime;
                        if (approxVel.sqrnorm() > candidateVel.sqrnorm())
                        {
                            candidateVel = approxVel;
                        }
                        if (approxVel.sqrnorm() > minPlatformVelSq)
                        {
                            candidateMoving = btrue;
                        }
                    }

                    const f32 velocityScore = candidateVel.sqrnorm();
                    if (velocityScore > bestPlatformVel.sqrnorm() || (candidateMoving && !bestPlatformMoving))
                    {
                        bestPlatformVel = candidateVel;
                        bestPlatformRef = candidateRef;
                        if (candidatePosValid)
                        {
                            bestPlatformPos = candidatePos;
                            bestPlatformPosValid = btrue;
                        }
                        bestPlatformMoving = candidateMoving || (velocityScore > minPlatformVelSq);
                    }
                }
            }
        }
        else
        {
            m_lastPlatformRef = ITF_INVALID_OBJREF;
        }

        if (bestPlatformRef.isValid())
        {
            m_lastPlatformRef = bestPlatformRef;
            if (bestPlatformPosValid)
            {
                m_lastPlatformPos = bestPlatformPos;
            }
        }

        if (bestPlatformMoving)
        {
            m_currentState.sensors.movingPlatformBelow = btrue;
            m_currentState.sensors.movingPlatformVel = bestPlatformVel;
        }

        // Hazard sensing
        const Vec2d hazardStart(playerPos.m_x, playerPos.m_y + chestHeightRelative * 0.5f);
        const Vec2d hazardDirs[] = { Vec2d(1.0f, 0.0f), Vec2d(1.0f, -0.35f), Vec2d(1.0f, 0.35f) };
        const f32 hazardRayLength = 5.0f;

        f32 bestHazardDist = F32_INFINITY;
        u32 bestHazardType = 0;
        bbool hazardFound = bfalse;

        for (u32 i = 0; i < ITF_ARRAY_SIZE(hazardDirs); ++i)
        {
            Vec2d dir = hazardDirs[i];
            dir.normalize();
            Vec2d end = hazardStart + dir * hazardRayLength;

            f32 hazardDist = 0.0f;
            u32 hazardType = 0;
            if (detectHazardAlongRay(hazardStart, end, zLayer, hazardDist, hazardType))
            {
                if (hazardDist < bestHazardDist)
                {
                    bestHazardDist = hazardDist;
                    bestHazardType = hazardType;
                    hazardFound = btrue;
                }
            }
        }

        if (hazardFound)
        {
            m_currentState.sensors.hazardAhead = btrue;
            m_currentState.sensors.hazardDist = bestHazardDist;
            m_currentState.sensors.hazardType = bestHazardType;
        }
    }

    void PerceptionModule::extractPlayerState()
    {
        if (!m_gameManager)
        {
            return;
        }
        Player* player = m_gameManager->getPlayer(0);
        if (!player || !player->getActive())
        {
            m_currentState.playerPosition = Vec3d(0.0f, 0.0f, 0.0f);
            m_currentState.playerVelocity = Vec2d(0.0f, 0.0f);
            m_currentState.isGrounded = bfalse;
            m_currentState.stance = STANCE_STAND;
            m_currentState.gameMode = 0;
        }
        else
        {
            Actor* playerActor = player->getActor();
            if (!playerActor)
            {
                return;
            }

            m_currentState.playerPosition = playerActor->getPos();
            m_currentState.gameMode = player->getCurrentMode();

            if (m_getStanceCallback)
            {
                m_currentState.stance = static_cast<EStance>(m_getStanceCallback(m_gameManager, 0));
            }
            else
            {
                m_currentState.stance = STANCE_STAND;
            }

            StickToPolylinePhysComponent* physComp = playerActor->GetComponent<StickToPolylinePhysComponent>();
            if (physComp)
            {
                const Vec2d& worldSpeed = physComp->getWorldSpeed();
                m_currentState.playerVelocity = worldSpeed;
                const PhysContactsContainer& contacts = physComp->getContacts();
                m_currentState.isGrounded = (contacts.size() > 0);
            }
            else
            {
                m_currentState.playerVelocity.m_x = 0.0f;
                m_currentState.playerVelocity.m_y = 0.0f;
                m_currentState.isGrounded = bfalse;
            }
        }
    }

    void PerceptionModule::scanNearbyTargets()
    {
        m_currentState.nextTarget = TargetInfo();

        if (m_scanTargetsCallback)
        {
            m_scanTargetsCallback(m_gameManager, &m_currentState);
        }
    }

    void PerceptionModule::debugDraw() const
    {
        if (!GFX_ADAPTER)
            return;

        char buffer[128];
        const f32 startX = 20.0f;
        const f32 lineHeight = 20.0f;
        f32 y = 50.0f;

        GFX_ADAPTER->drawDBGText("=== AI BOT STATE ===", startX, y, 0.0f, 1.0f, 1.0f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "Position: (%.2f, %.2f, %.2f)",
            m_currentState.playerPosition.m_x,
            m_currentState.playerPosition.m_y,
            m_currentState.playerPosition.m_z);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 1.0f, 1.0f, 1.0f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "Velocity: (%.2f, %.2f)",
            m_currentState.playerVelocity.m_x,
            m_currentState.playerVelocity.m_y);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 1.0f, 1.0f, 0.0f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "Grounded: %s",
            m_currentState.isGrounded ? "YES" : "NO");
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.0f, 1.0f, 0.0f, bfalse);
        y += lineHeight;

        const char* stanceName = "UNKNOWN";
        switch (m_currentState.stance)
        {
            case STANCE_STAND: stanceName = "STAND"; break;
            case STANCE_HANG: stanceName = "HANG"; break;
            case STANCE_CLIMB: stanceName = "CLIMB"; break;
            case STANCE_HELICO: stanceName = "HELICOPTER"; break;
            case STANCE_WALLSLIDE: stanceName = "WALLSLIDE"; break;
            case STANCE_SUPPORT: stanceName = "SUPPORT"; break;
            case STANCE_SWIM: stanceName = "SWIM"; break;
            case STANCE_SWING: stanceName = "SWING"; break;
        }
        sprintf_s(buffer, sizeof(buffer), "Stance: %s", stanceName);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 1.0f, 0.5f, 0.0f, bfalse);
        y += lineHeight;

        const char* modeName = "UNKNOWN";
        switch (m_currentState.gameMode)
        {
            case 0: modeName = "PLATFORMER"; break;
            case 1: modeName = "SHOOTER"; break;
            case 2: modeName = "WORLDMAP"; break;
        }
        sprintf_s(buffer, sizeof(buffer), "Mode: %s", modeName);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.5f, 0.5f, 1.0f, bfalse);
        y += lineHeight;

        const char* targetType = m_currentState.nextTarget.isCheckpoint ? "CHECKPOINT" : "CHANGEPAGE";
        sprintf_s(buffer, sizeof(buffer), "NextTarget: %s (%.1fm)",
            targetType,
            m_currentState.nextTarget.distance);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.0f, 1.0f, 1.0f, bfalse);
        y += lineHeight;

        // TerrainSensors overlay
        const TerrainSensors& s = m_currentState.sensors;
        sprintf_s(buffer, sizeof(buffer), "GroundAhead: N(%s,%.2f) M(%s,%.2f) F(%s,%.2f)",
            s.hasGroundAheadNear ? "Y":"N", s.groundAheadDistNear,
            s.hasGroundAheadMid ? "Y":"N", s.groundAheadDistMid,
            s.hasGroundAheadFar ? "Y":"N", s.groundAheadDistFar);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.7f, 1.0f, 0.7f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "GapDepth: %.2f  FrontWall: %s (h:%.2f)  Slope: %.3f",
            s.gapDepth, s.frontWall ? "YES":"NO", s.frontWallHeight, s.slopeAhead);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 1.0f, 0.7f, 0.3f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "GroundedFrames: %u  AirFrames: %u  Coyote: %.3f",
            s.groundedFrames, s.airborneFrames, s.coyoteTimeRemaining);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.6f, 0.8f, 1.0f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "Platform: %s vel(%.2f, %.2f)",
            s.movingPlatformBelow ? "YES" : "NO",
            s.movingPlatformVel.m_x,
            s.movingPlatformVel.m_y);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.8f, 0.8f, 1.0f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "Hazard: %s dist %.2f type %u",
            s.hazardAhead ? "YES" : "NO",
            s.hazardDist,
            s.hazardType);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 1.0f, 0.4f, 0.4f, bfalse);
    }

}
#endif
