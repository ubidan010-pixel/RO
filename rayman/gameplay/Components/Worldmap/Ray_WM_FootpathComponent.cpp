#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WM_FOOTPATHCOMPONENT_H_
#include "rayman/gameplay/Components/Worldmap/Ray_WM_FootpathComponent.h"
#endif //_ITF_RAY_WM_FOOTPATHCOMPONENT_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

#ifdef ITF_SUPPORT_EDITOR
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_SpotComponent.h"
#endif //_ITF_RAYWMSPOTCOMPONENT_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_FootpathComponent)
BEGIN_SERIALIZATION_CHILD(Ray_WM_FootpathComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_WM_FootpathComponent)
    VALIDATE_COMPONENT_PARAM("", m_trajectoryNode, "TrajectoryNodeComponent mandatory");
    VALIDATE_COMPONENT_PARAM("", m_fxController, "FXControllerComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_WM_FootpathComponent::Ray_WM_FootpathComponent()
    : Super()
    , m_allOpen(bfalse)
    , m_allOpenTimer(0.0f)
    , m_done(bfalse)
    , m_branches()
    , m_fogShownByBuddyNode(bfalse)
    , m_bezierRenderer()
    , m_trajectoryNode(NULL)
    , m_fxController(NULL)
{
}

//------------------------------------------------------------------------------
Ray_WM_FootpathComponent::~Ray_WM_FootpathComponent()
{
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_trajectoryNode = m_actor->GetComponent<TrajectoryNodeComponent>();
    m_fxController = m_actor->GetComponent<FXControllerComponent>();

    // register to events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventFetchWMOpenPaths,2134285703), this);

    // init renderer
    m_bezierRenderer.onLoaded(&getTemplate()->getBezierRenderer(), m_actor->getResourceGroup());
    m_bezierRenderer.setColor(getColorFactor());
    m_bezierRenderer.setFogColor(getColorFog());
    m_bezierRenderer.setDepth(getTemplate()->getZOffset());
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::onBecomeActive()
{
    Super::onBecomeActive();

    // hide "rollover" fx
    if (Actor* fx = getFX(m_actor))
    {
        hide(fx, btrue);
    }

    m_done = bfalse;
    m_allOpen = bfalse;
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);

    // process events
    if (Ray_EventFetchWMOpenPaths* fetchDone = _event->DynamicCast<Ray_EventFetchWMOpenPaths>(ITF_GET_STRINGID_CRC(Ray_EventFetchWMOpenPaths,2134285703)))
    {
        if (m_done)
        {
            fetchDone->setDone();
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::Update(f32 _dt)
{
    Super::Update(_dt);

#ifdef ITF_SUPPORT_CHEAT
    if (RAY_CHEATMANAGER->getAllWorldMap())
    {
        if (!m_done)
        {
            forceOpen();
        }
    }
#endif

    // no branch, no need for update
    u32 branchesSize = m_branches.size();
    if (0 == branchesSize)
    {
        return;
    }

    // update AABB
    AABB aabb(m_actor->getPos());
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        for (u32 i = 0; i < branch.m_bezier.getNumPoints(); i++)
        {
            const BezierCurve::Point & point = branch.m_bezier.getPointAt(i);
            aabb.grow(point.m_pos);
            aabb.grow(point.m_tanA);
            aabb.grow(point.m_tanB);
        }
    }
    m_actor->growRelativeAABBFromAbsolute(aabb);

    // done, no need for update
    if (m_done)
    {
        return;
    }

    // opening progression
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        if (branch.m_state == BranchState_Opening)
        {
            branch.m_length += getTemplate()->getSpeed() * _dt;
            if (branch.m_length > branch.m_bezier.getTotalLength())
            {
                // done, fully open
                branch.m_state = BranchState_Open;
                branch.m_length = branch.m_bezier.getTotalLength();
                branch.m_bezierCut.clear();

                // show end visual
                if (Actor* endVisual = branch.m_endVisual.getActor())
                {
                    // NB[LaurentCou]: can't be completed at this stage, always
                    // show visual, not beaten
                    show(endVisual, bfalse);
                }
                // hide end fog (cross fade)
                if (Actor* endFog = branch.m_endFog.getActor())
                {
                    hide(endFog, bfalse);
                }
                // hide end roots
                if (Actor* endRoots = branch.m_endRoots.getActor())
                {
                    hideRoots(endRoots, bfalse);
                }
                // show end actor
                if (Actor* endActor = branch.m_endActor.getActor())
                {
                    show(endActor, bfalse);

                    // chain
                    Ray_WM_SpotComponent* endSpot = endActor->GetComponent<Ray_WM_SpotComponent>();
                    Ray_WM_FootpathComponent* endFootpath = endActor->GetComponent<Ray_WM_FootpathComponent>();
                    if (endSpot && endFootpath)
                    {
                        // connect FX
                        endFootpath->playConnectFx();

                        // through mid nodes
                        if (endSpot->isMid())
                        {
                            endFootpath->open();
                        }
                        else
                        {
                            // open after non-completed node to completed nodes
                            if (endSpot->isAccessible() && endSpot->getState()!=SPOT_STATE_COMPLETED)
                            {
                                endFootpath->openFromNonCompletedToCompleted();
                            }

                            // fog teasing
                            endFootpath->teaseFog(bfalse);
                        }
                    }
                }
            }
            else
            {
                // update bezier cut
                u32 endEdgeIndex = 0;
                branch.m_bezier.getCut(branch.m_bezierCut, endEdgeIndex, branch.m_length);
                branch.m_bezierCut.buildEdges(getTemplate()->getSamplePerEdge());
            }
        }
    }

    // when all open, wait for last fade out to end sequence
    if (!m_allOpen)
    {
        bbool allOpen = btrue;
        for (u32 i=0; i<branchesSize; ++i)
        {
            const Branch& branch = m_branches[i];
            if (branch.m_state != BranchState_Open)
            {
                allOpen = bfalse;
                break;
            }
        }
        if (allOpen)
        {
            m_allOpen = btrue;
            m_allOpenTimer = 0.0f;
        }
    }
    else
    {
        m_allOpenTimer = Min(m_allOpenTimer+_dt, getTemplate()->getVisualFadeInDuration());
        if (m_allOpenTimer == getTemplate()->getVisualFadeInDuration())
        {
            m_done = btrue;
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::Draw()
{
    Super::Draw();

    // draw branches
    u32 branchesSize = m_branches.size();
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        if (branch.m_state == BranchState_Opening && branch.m_bezierCut.getNumPoints()>0)
        {
            m_bezierRenderer.draw(branch.m_patches, branch.m_bezierCut);
        }
        else if (branch.m_state == BranchState_Open)
        {
            m_bezierRenderer.draw(branch.m_patches, branch.m_bezier);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::initNoOpen()
{
    setupVisual(m_actor);

    // always hide own fog, will be teased by open/forceOpen when needed
    if (Actor* startFog = getFog(m_actor))
    {
        if (!m_fogShownByBuddyNode)
        {
            hide(startFog, btrue);
        }
    }

    // show roots
    if (Actor* startRoots = getRoots(m_actor))
    {
        showRoots(startRoots, btrue);
    }

    m_done = btrue;
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::forceOpen()
{
    // force open branches
    createBranches(m_branches, btrue, btrue);
    u32 branchesSize = m_branches.size();
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        branch.m_state = BranchState_Open;

        if (Actor* endFog = branch.m_endFog.getActor())
        {
            hide(endFog, btrue);
        }
        if (Actor* endRoots = branch.m_endRoots.getActor())
        {
            hideRoots(endRoots, btrue);
        }
        if (Actor* endActor = branch.m_endActor.getActor())
        {
            setupVisual(endActor);

            // chain
            Ray_WM_SpotComponent* endSpot = endActor->GetComponent<Ray_WM_SpotComponent>();
            Ray_WM_FootpathComponent* endFootpath = endActor->GetComponent<Ray_WM_FootpathComponent>();
            if (endSpot && endFootpath)
            {
                // through mid nodes
                if (endSpot->isMid())
                {
                    endFootpath->forceOpen();
                }
                else
                {
                    // open after non-completed node to completed nodes
                    if (endSpot->isAccessible() && endSpot->getState()!=SPOT_STATE_COMPLETED)
                    {
                        endFootpath->forceOpenFromNonCompletedToCompleted();
                    }

                    // fog teasing
                    endFootpath->teaseFog(btrue);
                }
            }
        }
    }

    // get spot
    Ray_WM_SpotComponent* spot = m_actor->GetComponent<Ray_WM_SpotComponent>();
    ITF_ASSERT(spot);
    if (!spot)
    {
        return;
    }

    // setup self
    setupVisual(m_actor);

    // special cases
    if (Ray_WM_SpotComponent* spot = m_actor->GetComponent<Ray_WM_SpotComponent>())
    {
        // opening MO_A = tease B worlds
        if (s_MO_A == spot->getTag())
        {
            teaseBWorlds(btrue); // force
        }
        else if (s_HomeMap == spot->getTag())
        {
            teaseLandOfTheDead(); // *always* force
        }
    }

    // done
    m_done = btrue;
    m_allOpen = btrue;
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::open()
{
    // re-open after cheat
    if (m_done || m_allOpen)
    {
        m_done = bfalse;
        m_allOpen = bfalse;
    }

    // start opening branches
    createBranches(m_branches, btrue, btrue);
    u32 branchesSize = m_branches.size();

    // no branch, then it's over
    if ( branchesSize == 0 )
    {
        m_done = btrue;
        return;
    }

    // start opening branches
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        if (branch.m_toCompleted)
        {
            // force open what was previously open with openFromNonCompletedToCompleted()
            branch.m_state = BranchState_Open;
        }
        else
        {
            branch.m_state = BranchState_Opening;
        }
    }

    // setup self
    setupVisual(m_actor);

    // special cases
    if (Ray_WM_SpotComponent* spot = m_actor->GetComponent<Ray_WM_SpotComponent>())
    {
        // opening MO_A = tease B worlds
        if (s_MO_A == spot->getTag())
        {
            teaseBWorlds(bfalse);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::forceOpenNonBWorlds()
{
    // force open branches to non-B-worlds
    createBranches(m_branches, btrue, btrue);
    u32 branchesSize = m_branches.size();
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        if (!branch.m_toBWorld)
        {
            branch.m_state = BranchState_Open;

            if (Actor* endFog = branch.m_endFog.getActor())
            {
                hide(endFog, btrue);
            }
            if (Actor* endRoots = branch.m_endRoots.getActor())
            {
                hideRoots(endRoots, btrue);
            }
            if (Actor* endActor = branch.m_endActor.getActor())
            {
                setupVisual(m_actor);
            }
        }
    }

    // setup self
    setupVisual(m_actor);
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::openBWorlds()
{
    // never open twice
    ITF_ASSERT(!m_done && !m_allOpen);
    if (m_done || m_allOpen)
    {
        return;
    }

    // open path to B-worlds
    u32 branchesSize = m_branches.size();
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        if (branch.m_toBWorld)
        {
            branch.m_state = BranchState_Opening;
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::openFromNonCompletedToCompleted()
{
    // start opening branches
    createBranches(m_branches, btrue, bfalse);
    u32 branchesSize = m_branches.size();

    // no branch, then it's over
    if ( branchesSize == 0 )
    {
        return;
    }

    // start opening branches
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        if (branch.m_toCompleted)
        {
            branch.m_state = BranchState_Opening;
        }
        else
        {
            branch.m_state = BranchState_Closed;
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::forceOpenFromNonCompletedToCompleted()
{
    createBranches(m_branches, btrue, bfalse);
    u32 branchesSize = m_branches.size();

    // no branch, then it's over
    if ( branchesSize == 0 )
    {
        return;
    }

    // force open branches
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = m_branches[i];
        if (branch.m_toCompleted)
        {
            branch.m_state = BranchState_Open;
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::teaseFog(bbool _force, bbool _toBWorld /*= bfalse*/)
{
    BranchList teaseBranches;
    createBranches(teaseBranches, bfalse, bfalse);

    u32 branchesSize = teaseBranches.size();
    for (u32 i=0; i<branchesSize; ++i)
    {
        Branch& branch = teaseBranches[i];

        // land of the dead handled separately
        if (branch.m_endTag == s_DE_A1)
        {
            continue;
        }

        // show end fog
        if (branch.m_toBWorld == _toBWorld)
        {
            if (Actor* endFog = branch.m_endFog.getActor())
            {
                show(endFog, _force);

                // prevent re-hiding in initNoOpen of buddy node
                Actor* endActor = branch.m_endActor.getActor();
                Ray_WM_FootpathComponent* endFootpath = endActor ? endActor->GetComponent<Ray_WM_FootpathComponent>() : NULL;
                if (endFootpath)
                {
                    endFootpath->m_fogShownByBuddyNode = btrue;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::playConnectFx()
{
    if (getTemplate()->getConnectFxName().isValid())
    {
        m_fxController->playFX(getTemplate()->getConnectFxName());
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::onEnter()
{
    // show "rollover" fx
    if (Actor* fx = getFX(m_actor))
    {
        show(fx, bfalse);
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::onExit()
{
    // hide "rollover" fx
    if (Actor* fx = getFX(m_actor))
    {
        hide(fx, bfalse);
    }
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_WM_FootpathComponent::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const
{
    Super::drawEdit(_drawInterface, _flags);
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::createBranches(
    BranchList& _branches,
    bbool _spotAccessibility,
    bbool _connect)
{
    // init one branch per child node
    TrajectoryNodeComponent::NodeList candidateNodes;
    m_trajectoryNode->getChildNodes(candidateNodes);
    u32 count = candidateNodes.size();
    for (u32 i=0; i<count; ++i)
    {
        TrajectoryNodeComponent* neighborNode = candidateNodes[i];

        // if neighbor's already a spot, no need to search for a path
        TrajectoryNodeComponent::NodeList path;
        if (!neighborNode->GetActor()->GetComponent<Ray_WM_SpotComponent>())
        {
            // get first path to a spot
            TrajectoryNodeComponent::NodeList visited;
            visited.push_back(m_trajectoryNode);
            visited.push_back(neighborNode);
            if (!Ray_GameScreen_WorldMap::findSpot(neighborNode, path, visited, 0))
            {
                continue;
            }
        }

        // insert first two nodes
        path.insert(neighborNode, 0);
        path.insert(m_trajectoryNode, 0);

        // get spot at the branch end
        u32 pathSize = path.size();
        ITF_ASSERT(pathSize >= 2);
        if (pathSize < 2)
        {
            continue;
        }
        Ray_WM_SpotComponent* spot = path[pathSize-1]->GetActor()->GetComponent<Ray_WM_SpotComponent>();
        if (!spot)
        {
            continue;
        }

        // signal connection on open (forced or not)
        if (_connect)
        {
            spot->onConnection();
        }

        // if spot doesn't match demanded accessibility, no branch
        if (spot->isAccessible() != _spotAccessibility)
        {
            continue;
        }

        // new branch
        _branches.push_back(Branch());
        Branch& branch = _branches.back();

        // sample trajectory
        for (u32 nodeIndex=0; nodeIndex<pathSize; ++nodeIndex)
        {
            TrajectoryNodeComponent* node = path[nodeIndex];

            Vec3d pos = node->GetActor()->getPos();

            Vec3d tangent;
            if (nodeIndex == 0)
            {
                TrajectoryNodeComponent* nextNode = path[nodeIndex+1];
                tangent = nextNode->GetActor()->getPos() - pos;
            }
            else if (nodeIndex == (pathSize-1))
            {
                TrajectoryNodeComponent* prevNode = path[nodeIndex-1];
                tangent = pos - prevNode->GetActor()->getPos();
            }
            else
            {
                TrajectoryNodeComponent* prevNode = path[nodeIndex-1];
                TrajectoryNodeComponent* nextNode = path[nodeIndex+1];
                Vec3d prev = pos - prevNode->GetActor()->getPos();
                prev.normalize();
                Vec3d next = nextNode->GetActor()->getPos() - pos;
                next.normalize();
                tangent = prev + next;
            }
            tangent.normalize();
            Vec3d tanA = pos - tangent * getTemplate()->getTangentStretch();
            Vec3d tanB = pos + tangent * getTemplate()->getTangentStretch();

            BezierCurve::Point point(pos, tanA, tanB);
            branch.m_bezier.addPoint(point);
        }

        // sample Bezier
        branch.m_bezier.buildEdges(getTemplate()->getSamplePerEdge());
        branch.m_patches.resize(branch.m_bezier.getNumEdges());

        // get & store end visual
        Actor* endVisual = getVisual(spot->GetActor());
        branch.m_endVisual = endVisual ? endVisual->getRef() : ActorRef::InvalidRef;

        // get & store end fog
        Actor* endFog = getFog(spot->GetActor());
        branch.m_endFog = endFog ? endFog->getRef() : ActorRef::InvalidRef;

        // get & store end roots
        Actor* endRoots = getRoots(spot->GetActor());
        branch.m_endRoots = endRoots ? endRoots->getRef() : ActorRef::InvalidRef;

        // store end actor
        branch.m_endActor = spot->GetActor()->getRef();
        branch.m_endTag = spot->getTag();

        // mark branches to B worlds
        if (spot->getTag() == s_JU_B ||
            spot->getTag() == s_MU_B ||
            spot->getTag() == s_FO_B ||
            spot->getTag() == s_OC_B)
        {
            branch.m_toBWorld = btrue;
        }

        // mark branches to completed spots (that aren't mid nodes)
        branch.m_toCompleted = !spot->isMid() && (spot->getState() == SPOT_STATE_COMPLETED);
    }
}

//------------------------------------------------------------------------------
Actor* Ray_WM_FootpathComponent::getVisual(Actor* _spot)
{
    // get child actor linked with a "visual" tag
    LinkComponent* linkComponent = _spot->GetComponent<LinkComponent>();
    if (!linkComponent)
    {
        return NULL;
    }
    AIUtils::LinkIterator iterator(linkComponent);
    return iterator.getNextRelativeActorTagFilter(
        ITF_GET_STRINGID_CRC(visual,3771141096), bfalse);
}

//------------------------------------------------------------------------------
Actor* Ray_WM_FootpathComponent::getBoss(Actor* _spot)
{
    // get child actor linked with a "boss" tag
    LinkComponent* linkComponent = _spot->GetComponent<LinkComponent>();
    if (!linkComponent)
    {
        return NULL;
    }
    AIUtils::LinkIterator iterator(linkComponent);
    return iterator.getNextRelativeActorTagFilter(
        ITF_GET_STRINGID_CRC(boss,2488061616), bfalse);
}

//------------------------------------------------------------------------------
Actor* Ray_WM_FootpathComponent::getBeaten(Actor* _spot)
{
    // get child actor linked with a "beaten" tag
    LinkComponent* linkComponent = _spot->GetComponent<LinkComponent>();
    if (!linkComponent)
    {
        return NULL;
    }
    AIUtils::LinkIterator iterator(linkComponent);
    return iterator.getNextRelativeActorTagFilter(
        ITF_GET_STRINGID_CRC(beaten,2684511368), bfalse);
}

//------------------------------------------------------------------------------
Actor* Ray_WM_FootpathComponent::getFog(Actor* _spot)
{
    // get child actor linked with a "fog" tag
    LinkComponent* linkComponent = _spot->GetComponent<LinkComponent>();
    if (!linkComponent)
    {
        return NULL;
    }
    AIUtils::LinkIterator iterator(linkComponent);
    return iterator.getNextRelativeActorTagFilter(
        ITF_GET_STRINGID_CRC(fog,1895684582), bfalse);
}

//------------------------------------------------------------------------------
Actor* Ray_WM_FootpathComponent::getFX(Actor* _spot)
{
    // get child actor linked with an "fx" tag
    LinkComponent* linkComponent = _spot->GetComponent<LinkComponent>();
    if (!linkComponent)
    {
        return NULL;
    }
    AIUtils::LinkIterator iterator(linkComponent);
    return iterator.getNextRelativeActorTagFilter(
        ITF_GET_STRINGID_CRC(fx,3354007880), bfalse);
}

//------------------------------------------------------------------------------
Actor* Ray_WM_FootpathComponent::getRoots(Actor* _spot)
{
    // get child actor linked with a "roots" tag
    LinkComponent* linkComponent = _spot->GetComponent<LinkComponent>();
    if (!linkComponent)
    {
        return NULL;
    }
    AIUtils::LinkIterator iterator(linkComponent);
    return iterator.getNextRelativeActorTagFilter(
        ITF_GET_STRINGID_CRC(roots,2814133504), bfalse);
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::setupVisual(Actor* _actor)
{
    Ray_WM_SpotComponent* spot = _actor->GetComponent<Ray_WM_SpotComponent>();
    if (!spot)
    {
        return;
    }

    // show/hide actor & associated visual/beaten
    Actor* visual = getVisual(_actor);
    Actor* boss = getBoss(_actor);
    Actor* beaten = getBeaten(_actor);
    if (spot->isAccessible())
    {
        if (beaten)
        {
            if (spot->getState() == SPOT_STATE_COMPLETED)
            {
                show(beaten, btrue);
                if (boss)
                {
                    hide(boss, btrue);
                }
                if (visual)
                {
                    hide(visual, btrue);
                }
            }
            else
            {
                hide(beaten, btrue);
                if (boss)
                {
                    show(boss, btrue);
                }
                if (visual)
                {
                    show(visual, btrue);
                }
            }
        }
        else
        {
            if (boss)
            {
                hide(boss, btrue);
            }
            if (visual)
            {
                show(visual, btrue);
            }
        }
        show(_actor, btrue);
    }
    else
    {
        if (visual)
        {
            hide(visual, btrue);
        }
        if (boss)
        {
            show(boss, btrue);
        }
        if (beaten)
        {
            hide(beaten, btrue);
        }
        hide(_actor, btrue);
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::show(Actor* _actor, bbool _force)
{
    // alpha fade in
    f32 delay = _force
        ? F32_BIG_EPSILON // NB[LaurentCou]: something goes wrong with plain zero
        : getTemplate()->getVisualFadeInDuration();
    EventShow alphaChange(1.0f, delay);
    _actor->onEvent(&alphaChange);
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::hide(Actor* _actor, bbool _force)
{
    // alpha fade out
    f32 delay = _force
        ? F32_BIG_EPSILON // NB[LaurentCou]: something goes wrong with plain zero
        : getTemplate()->getVisualFadeInDuration();
    EventShow alphaChange(0.0f, delay);
    _actor->onEvent(&alphaChange);
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::showRoots(Actor* _roots, bbool _force)
{
    // unroll roots (last level of each world)
    EventGeneric openEvent;
    openEvent.setId(_force
        ? ITF_GET_STRINGID_CRC(ForceOpen,410328037)
        : ITF_GET_STRINGID_CRC(Open,812432275)
        );
    _roots->onEvent(&openEvent);
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::hideRoots(Actor* _roots, bbool _force)
{
    // hide roots (last level of each world)
    EventGeneric closeEvent;
    closeEvent.setId(_force
        ? ITF_GET_STRINGID_CRC(ForceClose,3309581368)
        : ITF_GET_STRINGID_CRC(Close,3883599349)
        );
    _roots->onEvent(&closeEvent);
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::teaseBWorlds(bbool _force)
{
    if (Actor* jungleA = RAY_GAMEMANAGER->getWMSpot(s_JU_A))
    {
        if (Ray_WM_FootpathComponent* footpath = jungleA->GetComponent<Ray_WM_FootpathComponent>())
        {
            footpath->teaseFog(_force, btrue);
        }
    }
    if (Actor* musicA = RAY_GAMEMANAGER->getWMSpot(s_MU_A))
    {
        if (Ray_WM_FootpathComponent* footpath = musicA->GetComponent<Ray_WM_FootpathComponent>())
        {
            footpath->teaseFog(_force, btrue);
        }
    }
    if (Actor* foodA = RAY_GAMEMANAGER->getWMSpot(s_FO_A))
    {
        if (Ray_WM_FootpathComponent* footpath = foodA->GetComponent<Ray_WM_FootpathComponent>())
        {
            footpath->teaseFog(_force, btrue);
        }
    }
    if (Actor* oceanA = RAY_GAMEMANAGER->getWMSpot(s_OC_A))
    {
        if (Ray_WM_FootpathComponent* footpath = oceanA->GetComponent<Ray_WM_FootpathComponent>())
        {
            footpath->teaseFog(_force, btrue);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FootpathComponent::teaseLandOfTheDead()
{
    // tease land of the dead when all tooth have been taken but not returned
    if (RAY_GAMEMANAGER->getTotalTakenToothCount() >= 10 &&
        !RAY_GAMEMANAGER->hasAllTeethBeenReturned())
    {
        if (Actor* landOfTheDead = RAY_GAMEMANAGER->getWMSpot(s_DE_A1))
        {
            if (Ray_WM_FootpathComponent* footpath = landOfTheDead->GetComponent<Ray_WM_FootpathComponent>())
            {
                if (Actor* fog = footpath->getFog(landOfTheDead))
                {
                    show(fog, btrue); // force
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_FootpathComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_WM_FootpathComponent_Template)
    SERIALIZE_MEMBER("speed", m_speed);
    SERIALIZE_MEMBER("zOffset", m_zOffset);
    SERIALIZE_MEMBER("tangentStretch", m_tangentStretch);
    SERIALIZE_MEMBER("visualFadeInDuration", m_visualFadeInDuration);
    SERIALIZE_MEMBER("fxFadeInDuration", m_fxFadeInDuration);
    SERIALIZE_MEMBER("cameraFollowDistance", m_cameraFollowDistance);
    SERIALIZE_MEMBER("samplePerEdge", m_samplePerEdge);
    SERIALIZE_MEMBER("connectFxName", m_connectFxName);
    SERIALIZE_OBJECT("bezierRenderer", m_bezierRenderer);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_WM_FootpathComponent_Template::Ray_WM_FootpathComponent_Template()
    : Super()
    , m_speed(1.0f)
    , m_zOffset(0.0f)
    , m_tangentStretch(1.0f)
    , m_visualFadeInDuration(1.0f)
    , m_fxFadeInDuration(0.5f)
    , m_cameraFollowDistance(1.0f)
    , m_samplePerEdge(10)
    , m_connectFxName()
    , m_bezierRenderer()
{
}

//------------------------------------------------------------------------------
Ray_WM_FootpathComponent_Template::~Ray_WM_FootpathComponent_Template()
{
}

} // namespace ITF
