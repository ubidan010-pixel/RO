#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MURPHYASSISTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MurphyAssistAIComponent.h"
#endif //_ITF_RAY_MURPHYASSISTAICOMPONENT_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageComponent.h"
#endif //_ITF_RAY_CHANGEPAGECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_CAGEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_CageAIComponent.h"
#endif //_ITF_RAY_CAGEAICOMPONENT_H_

#ifndef _ITF_RAY_SKULLCOINCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SkullCoinComponent.h"
#endif //_ITF_RAY_SKULLCOINCOMPONENT_H_

#ifndef _ITF_RayVita_AIRelicBehavior_H_
#include "rayman/gameplay/GamePlayVita/ai/Behaviors/Relic/RayVita_AIRelicBehavior.h"
#endif //_ITF_RayVita_AIRelicBehavior_H_

#ifndef _ITF_RAY_TALKINGBUBBLECOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingBubbleComponent.h"
#endif //_ITF_RAY_TALKINGBUBBLECOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MurphyAssistBubbleAIComponent.h"
#endif //_ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_

namespace ITF
{
    template<typename T>
    static inline void erase_swap_back(ITF_VECTOR<T>& v, u32 idx) {
        const u32 n = v.size();
        if (idx >= n) return;
        if (n > 1 && idx != n - 1) v[idx] = v[n - 1];
        v.pop_back();
    }

    static inline f32 sq_len(const Vec3d& v) {
        return v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z;
    }
    static inline f32 sq_dist(const Vec3d& a, const Vec3d& b) {
        const Vec3d d = Vec3d(a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z);
        return sq_len(d);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_MurphyAssistAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_MurphyAssistAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_MurphyAssistAIComponent)
    END_VALIDATE_COMPONENT()

    //--------------------------------------------------------------------------------------------------------
    Ray_MurphyAssistAIComponent::Ray_MurphyAssistAIComponent()
    : Super()
    , m_assistState(Assist_None)
    , m_currentTargetIdx(-1)
    , m_probeAccum(0.f)
    , m_followAccum(0.f)
    , m_prevTargetPos(Vec3d::Zero)
    , m_smoothSpeed(Vec3d::Zero)
    , m_canFollowPlayer(bfalse)
    , m_talkAccum(0.f)
    , m_stillAccum(0.f)
    , m_lastPlayerX(0.f)
    {
    }

    Ray_MurphyAssistAIComponent::~Ray_MurphyAssistAIComponent()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventActorActiveChanged, 323621555), this);
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_sentences.clear();
        m_sentences.push_back(getCustomTemplate()->getMurphyAssistLocId());

        Super::onActorLoaded(_hotReload);

        if (!getCustomTemplate()->getIndicatorPath().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_indicatorSpawner, getCustomTemplate()->getIndicatorPath());

            Actor* indicatorActor = m_indicatorSpawner.getSpawnee(m_actor->getScene(), m_actor->getPos(), m_actor->getAngle());

            if (indicatorActor)
            {
                m_indicatorRef = indicatorActor->getRef();
                indicatorActor->disable();
            }
        }
        Actor* bubbleActor = m_bubbleRef.getActor();
        if (bubbleActor)
        {
            Ray_TalkingBubbleComponent* bubble = bubbleActor->GetComponent<Ray_TalkingBubbleComponent>();

            if (bubble)
            {
                StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
                i32 takenRelic = RAY_GAMEMANAGER->getTakenRelicCount(tag);
                i32 maxRelic = RAY_GAMEMANAGER->getMaxRelics(tag);

                i32 maxHiddenCage = Max((i32)RAY_GAMEMANAGER->getLevelTotalCageCount(tag) - 1, 0);
                i32 brokenHiddenCage = Max((i32)RAY_GAMEMANAGER->getLevelBrokenCageCount(tag) - 1, 0);

                PathString8_t relicPath, electoonPath;

                getCustomTemplate()->getRelicPath().getString(relicPath);
                getCustomTemplate()->getElectoonPath().getString(electoonPath);

                ITF_VECTOR <TextInfo> textInfos;

                textInfos.resize(1);

                TextInfo& textInfo = textInfos[0];
                //textInfo.m_color = getCustomTemplate()->getTextColor().getAsU32();
                textInfo.m_size = getCustomTemplate()->getInitialFontHeight();
                textInfo.m_text.setTextFormat("Pop the bubble & I'll help find the remaining treasures: \n[actor:%s]%i/%i [actor:%s]%i/%i", electoonPath.cStr(), brokenHiddenCage, maxHiddenCage, relicPath.cStr(), takenRelic, maxRelic);

                bubble->setSentencesStr(textInfos);
                //bubble->setAppear();
            }
        }

        m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventActorActiveChanged, 323621555), this);
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::setState(State _state)
    {
        Super::setState(_state);

        if (m_canFollowPlayer)
            return;

        if (_state == State_Talk)
            spawnToggleBubble();
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::ensureBubbleDisabled()
    {
        Actor* bubbleActor = m_bubbleRef.getActor();
        if (bubbleActor) {
            bubbleActor->disable();
        }
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::stepFollowPlayer(Actor* _player, f32 dt)
    {
        m_followAccum += dt;
        if (m_followAccum < 0.015f) return;
        m_followAccum = 0.f;

        if (!_player)
            return;

        Vec2d offset = getCustomTemplate()->getPlayerFollowOffset();
        if (AIUtils::getFlipped(_player))
        {
            offset.m_x = -offset.m_x;
        }
        offset *= _player->getScale();

        Vec3d targetPos = _player->getPos() + offset.to3d();

        Vec3d newPos = m_actor->getPos();
        newPos.m_z = targetPos.m_z;

        AIUtils::advancedMichelSmooth(
            newPos,
            m_smoothSpeed,
            m_prevTargetPos,
            targetPos,
            getCustomTemplate()->getSpeedBlend(),
            getCustomTemplate()->getSpeedMin(),
            getCustomTemplate()->getSpeedMax(),
            getCustomTemplate()->getBlendAtSpeedMin(),
            getCustomTemplate()->getBlendAtSpeedMax(),
            dt
        );

        m_actor->setPos(newPos);
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::stepGoToTarget(f32 dt)
    {
        if (m_currentTargetIdx < 0 || (u32)m_currentTargetIdx >= m_targets.size()) {
            enterFollowState(); return;
        }

        Actor* tgt = m_targets[(u32)m_currentTargetIdx].actorRef.getActor();
        if (!tgt /*|| !tgt->isActive()*/) {
            enterFollowState(); return;
        }

        Vec3d targetPos = tgt->getPos();
        Vec3d newPos = m_actor->getPos();
        newPos.m_z = targetPos.m_z;

        AIUtils::advancedMichelSmooth(
            newPos,
            m_smoothSpeed,
            m_prevTargetPos,
            targetPos,
            getCustomTemplate()->getSpeedBlend(),
            getCustomTemplate()->getSpeedMin(),
            getCustomTemplate()->getSpeedMax(),
            getCustomTemplate()->getBlendAtSpeedMin(),
            getCustomTemplate()->getBlendAtSpeedMax(),
            dt
        );

        m_actor->setPos(newPos);

        // Arrival check
        f32 arriveStopDist = getCustomTemplate()->getArriveStopDist();
        const f32 stop2 = arriveStopDist * arriveStopDist;
        if (sq_dist(m_actor->getPos(), targetPos) <= stop2) {

        }
    }
    //--------------------------------------------------------------------------------------------------------
    i32 Ray_MurphyAssistAIComponent::findIndexByRef(const ObjectRef& r) const {
        for (u32 i = 0; i < m_targets.size(); ++i)
            if (m_targets[i].actorRef == r) return (i32)i;
        return -1;
    }
    //--------------------------------------------------------------------------------------------------------
    static inline bool cageLogicOK(i32 cageIndex) {
        if (cageIndex < 0) return false;
        const StringID& level = RAY_GAMEMANAGER->getCurrentLevelName();
        return RAY_GAMEMANAGER->isLevelCompletedOnce(level) &&
            !RAY_GAMEMANAGER->getIsCageBroken(cageIndex);
    }
    static inline bool relicLogicOK(i32 relicId) {
        const StringID& level = RAY_GAMEMANAGER->getCurrentLevelName();
        return !RAY_GAMEMANAGER->getIsRelicTaken(level, relicId);
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::considerActorAdd(Actor* a) {
        if (!a || !a->isActive()) return;

        // Cage
        if (auto* changePage = a->GetComponent<Ray_ChangePageComponent>()) {
            SafeArray<ObjectRef>* parents = LINKMANAGER->getParents(changePage->GetActor()->getRef());

            u32 count = parents->size();
            for (u32 i = 0; i < count; i++)
            {
                BaseObject* target = (*parents)[i].getObject();
                if (target)
                {
                    Actor* actor = static_cast<Actor*>(target);
                    if (actor && actor->getObjectType() == BaseObject::eActor)
                    {
                        Ray_CageAIComponent* cageComp = actor->GetComponent<Ray_CageAIComponent>();
                        if (cageComp)
                        {
                            const i32 idx = cageComp->getCageIndex();
                            if (cageLogicOK(idx)) {
                                ObjectRef r = a->getRef();
                                const i32 at = findIndexByRef(r);
                                if (at < 0) {
                                    m_targets.push_back({ MurphyAssistTarget::Cage, r, idx });
                                }
                                else {
                                    m_targets[(u32)at].active = btrue;
                                }
                                m_appearRequested = btrue;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Skull coin
        if (a->GetComponent<Ray_SkullCoinComponent>()) {
            ObjectRef r = a->getRef();
            const i32 at = findIndexByRef(r);
            if (at < 0) {
                m_targets.push_back({ MurphyAssistTarget::SkullCoin, r, -1 });
            }
            else {
                m_targets[(u32)at].active = btrue;
            }
            m_appearRequested = btrue;
        }

        // Relic
        if (auto* fixedAI = a->GetComponent<Ray_FixedAIComponent>()) {
            if (auto* beh = fixedAI->getCurrentBehavior()) {
                if (auto* relic = beh->DynamicCast<RayVita_AIRelicBehavior>(
                    ITF_GET_STRINGID_CRC(RayVita_AIRelicBehavior, 267847435))) {
                    const i32 relicId = relic->getRelicIndex();
                    if (relicLogicOK(relicId)) {
                        ObjectRef r = a->getRef();
                        const i32 at = findIndexByRef(r);
                        if (at < 0) {
                            m_targets.push_back({ MurphyAssistTarget::Relic, r, relicId });
                        }
                        else {
                            m_targets[(u32)at].active = btrue;
                        }
                    }
                }
            }
        }
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::considerActorRemove(Actor* a) {
        if (!a) return;
        const ObjectRef r = a->getRef();

        const i32 at = findIndexByRef(r);
        if (at >= 0) {
            m_targets[(u32)at].active = bfalse;
        }

        if (m_currentTargetIdx >= (i32)m_targets.size()) m_currentTargetIdx = -1;
    }
    //--------------------------------------------------------------------------------------------------------
    i32 Ray_MurphyAssistAIComponent::nearestTargetWithin(const ITF_VECTOR<MurphyAssistTarget>& list, const Vec3d& probePos, f32 maxDeltaX)
    {
        if (list.empty()) return -1;

        i32  bestIdx = -1;
        f32  bestD2 = FLT_MAX;

        const u32 n = list.size();
        for (u32 i = 0; i < n; ++i) {
            Actor* a = list[i].actorRef.getActor();
            if (!a /* || !a->isActive()*/) continue;

            const Vec3d pos = a->getPos();
            const f32 dx = fabsf(pos.m_x - probePos.m_x);
            if (dx > maxDeltaX) continue;

            const f32 d2 = sq_dist(pos, probePos);
            if (d2 < bestD2) { bestD2 = d2; bestIdx = (i32)i; }
        }
        return bestIdx;
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::requestAppear()
    {
        m_appearRequested = btrue;
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::forceDisappear()
    {
        setState(State_Disappear);
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::followPlayer()
    {
        m_canFollowPlayer = btrue;
        const u32 pIdx = GAMEMANAGER->getMainIndexPlayer();
        Actor* pAct = GAMEMANAGER->getActivePlayer(pIdx);
        m_actor->setPos(pAct->getPos() + getCustomTemplate()->getPlayerFollowOffset().to3d());
        setState(State_Talk);
        enterFollowState();
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::markTargetCollected(const ActorRef& _actorRef)
    {
        if (!_actorRef.isValid() || m_targets.empty())
            return;

        for (u32 i = 0; i < m_targets.size(); ++i)
        {
            if (m_targets[i].actorRef == _actorRef)
            {
                const i32 removedIdx = static_cast<i32>(i);
                erase_swap_back(m_targets, i);

                if (m_currentTargetIdx == removedIdx)
                {
                    m_currentTargetIdx = -1;
                    enterFollowState();
                }
                else
                {
                }

                return;
            }
        }
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::enterFollowState()
    {
        ensureBubbleDisabled();

        m_assistState = Assist_FollowPlayer;

        m_smoothSpeed.set(0, 0, 0);
        m_prevTargetPos = m_actor->getPos();
        m_followAccum = 0.f;
        m_talkAccum = 0.f;
        m_stillAccum = 0.f;

        if (m_animatedComponent)
            m_animatedComponent->setAnim(getTemplate()->getTalkAnim());

        m_appearRequested = bfalse;
        m_disappearRequested = bfalse;
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::enterGoToTargetState(i32 idx)
    {
        if (idx < 0) { enterFollowState(); return; }
        m_assistState = Assist_GoToTarget;
        m_currentTargetIdx = idx;
        m_smoothSpeed.set(0, 0, 0);

        if (m_animatedComponent)
            m_animatedComponent->setAnim(getTemplate()->getTalkAnim());
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::Update(f32 _dt)
    {
        Super::Update(_dt);

        if (m_state < State_Talk)
            return;

        if (!m_canFollowPlayer)
        {
            return;
        }

        Actor* player = GAMEMANAGER->getActivePlayer(GAMEMANAGER->getMainIndexPlayer());
        if (!player)
            return;

        const bbool playerStillLong = updatePlayerStillness(player, _dt);

        switch (m_assistState)
        {
        case Assist_None:
            //if (Ray_TalkingBubbleComponent* bubble = getBubbleComponent()) {
            //    if (bubble->isDoneTalking()) {
            //        enterFollowState();
            //    }
            //}
            //else
            {
                enterFollowState();
            }
            break;
        case Assist_FollowPlayer:
            stepFollowPlayer(player, _dt);

            if (playerStillLong)
            {
                enterTalkState();
                break;
            }

            m_probeAccum += _dt;
            if (m_probeAccum >= 0.10f) {
                m_probeAccum = 0.f;
                const i32 idx = nearestTargetWithin(m_targets, player->getPos(), getCustomTemplate()->getGoBackMaxDeltaX());
                if (idx >= 0) enterGoToTargetState(idx);
            }
            break;

        case Assist_GoToTarget:
            stepGoToTarget(_dt);

            if (playerStillLong)
            {
                enterTalkState();
                break;
            }

            m_probeAccum += _dt;

            if (m_probeAccum >= 0.10f)
            {
                m_probeAccum = 0.f;

                const f32 maxDx = getCustomTemplate()->getGoBackMaxDeltaX();

                //const i32 newIdx = nearestTargetWithin(m_targets, player->getPos(), maxDx);
                //if (newIdx >= 0 && newIdx != m_currentTargetIdx)
                //    enterGoToTargetState(newIdx);

                const f32 dxMurphy = fabsf(m_actor->getPos().m_x - player->getPos().m_x);
                if (dxMurphy > maxDx)
                {
                    enterFollowState();
                    break;
                }
            }
            break;

        case Assist_Talking:
        {
            m_talkAccum += _dt;

            const bbool timeUp = (m_talkAccum >= getCustomTemplate()->getJuiceShowTime());

            if (!playerStillLong || timeUp)
            {
                const i32 idx = nearestTargetWithin(m_targets, player->getPos(), getCustomTemplate()->getGoBackMaxDeltaX());
                if (idx >= 0) {
                    enterGoToTargetState(idx);
                }
                else {
                    enterFollowState();
                }
            }
        }
        break;
        default: break;
        }

        updateOffscreenFX();
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::updateOffscreenFX()
    {
        Actor* spawn = m_actor;
        if (!spawn)
        {
            return;
        }

        AABB screenAABB;
        if (!getScreenAABBCropped(Margin(-1, -1, -1, -1), screenAABB))
        {
            return;
        }

#ifdef ITF_SUPPORT_EDITOR
        //DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0.f, screenAABB, Color::green(), 0.f, 5);

        //const u32 pIdx = GAMEMANAGER->getMainIndexPlayer();
        //if (Actor* pAct = GAMEMANAGER->getActivePlayer(pIdx))
        //{
        //    const f32 maxDx = getCustomTemplate()->getGoBackMaxDeltaX();
        //    const f32 cx = pAct->getPos().m_x;

        //    const Vec2d scrMin = screenAABB.getMin();
        //    const Vec2d scrMax = screenAABB.getMax();

        //    const f32 bandMinX = Clamp(cx - maxDx, scrMin.m_x, scrMax.m_x);
        //    const f32 bandMaxX = Clamp(cx + maxDx, scrMin.m_x, scrMax.m_x);

        //    ITF::AABB band;
        //    band.setMin(Vec2d(bandMinX, scrMin.m_y - 1));
        //    band.setMax(Vec2d(bandMaxX, scrMax.m_y + 1));

        //    DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0.f, band, Color::yellow(), 0.f, 3.f, "Δx band");
        //}
#endif

        const Vec2d pos = spawn->getPos().truncateTo2D();

        if (!screenAABB.contains(pos))
        {
            const Vec2d targetPos(
                Clamp(pos.m_x, screenAABB.getMin().m_x, screenAABB.getMax().m_x),
                Clamp(pos.m_y, screenAABB.getMin().m_y, screenAABB.getMax().m_y));

            //DebugDraw::circle(targetPos, m_actor->getDepth(), 0.3f, Color::red(), 0.f, "Murphy");
            enableIndicatorAt(targetPos, Vec2d());
        }
        else if (m_indicatorRef.getActor())
        {
            m_indicatorRef.getActor()->disable();
        }
    }
    //--------------------------------------------------------------------------------------------------------
    bbool Ray_MurphyAssistAIComponent::getScreenAABBCropped(const Margin& _margin, AABB& _screenAABB)
    {
        if (CAMERACONTROLLERMANAGER->getAABB(m_actor->getDepth(), _screenAABB))
        {
            //_margin.addTo(_screenAABB);

            Vec2d min = _screenAABB.getMin();
            Vec2d max = _screenAABB.getMax();

            if ((min.m_x - _margin.m_left) < (max.m_x + _margin.m_right))
            {
                min.m_x -= _margin.m_left;
                max.m_x += _margin.m_right;
            }
            else
            {
                min.m_x = max.m_x = (min.m_x + max.m_x) * 0.5f;
            }

            if ((min.m_y - _margin.m_bottom) < (max.m_y + _margin.m_top))
            {
                min.m_y -= _margin.m_bottom;
                max.m_y += _margin.m_top;
            }
            else
            {
                min.m_y = max.m_y = (min.m_y + max.m_y) * 0.5f;
            }

            _screenAABB.setMin(min);
            _screenAABB.setMax(max);
            ITF_ASSERT(_screenAABB.isValid());

            return btrue;
        }

        return bfalse;
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::enableIndicatorAt(const Vec2d _position, const Vec2d _rotation)
    {
        Actor* indicator = m_indicatorRef.getActor();
        if (indicator)
        {
            indicator->enable();
            indicator->setPos(Vec3d(_position.m_x, _position.m_y, getCustomTemplate()->getIndicatorZOffset()));

            //how to set rotation??
        }
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::spawnToggleBubble()
    {
        if (!getCustomTemplate()->getToggleBubblePath().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_toggleBubbleSpawner, getCustomTemplate()->getToggleBubblePath());

            const Vec3d offset = getCustomTemplate()->getToggleSpawnOffset();
            const Vec3d spawnPos = m_actor->getPos() + offset;
            Actor* toggleBubbleActor = m_toggleBubbleSpawner.getSpawnee(m_actor->getScene(), spawnPos, m_actor->getAngle());

            if (toggleBubbleActor)
            {
                m_toggleBubbleRef = toggleBubbleActor->getRef();

                Ray_MurphyAssistBubbleAIComponent* bComp = toggleBubbleActor->GetComponent<Ray_MurphyAssistBubbleAIComponent>();

                if (bComp)
                {
                    bComp->setOwner(this);
                }
            }
        }
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::enterTalkState()
    {
        m_assistState = Assist_Talking;
        m_talkAccum = 0.f;

        showDialog();
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::showDialog()
    {
        Actor* bubbleActor = m_bubbleRef.getActor();
        if (bubbleActor) {
            bubbleActor->enable();
            bubbleActor->setPos(Vec3d(m_actor->getPos().m_x, m_actor->getPos().m_y, bubbleActor->getPos().m_z));
        }
    }
    //--------------------------------------------------------------------------------------------------------
    bbool Ray_MurphyAssistAIComponent::updatePlayerStillness(const Actor* _player, f32 _dt)
    {
        if (_player)
        {
            const f32 px = _player->getPos().m_x;

            const f32 dx = fabsf(px - m_lastPlayerX);
            m_lastPlayerX = px;

            if (dx < getCustomTemplate()->getMovementEpsilon())
                m_stillAccum += _dt;
            else
                m_stillAccum = 0.f;

            return (m_stillAccum >= getCustomTemplate()->getStillTime());
        }

        m_stillAccum = 0.f;
        return bfalse;
    }
    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistAIComponent::onEvent(Event* _event)
    {
        if (auto* evt = _event->DynamicCast<EventActorActiveChanged>(ITF_GET_STRINGID_CRC(EventActorActiveChanged, 323621555))) {
            if (evt->isActive())
                considerActorAdd(evt->getActor());
            else
                considerActorRemove(evt->getActor());
            return;
        }

        if (auto* onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger, 1343042510))) {
            if (onTrigger->getActivated()) {
                m_appearRequested = btrue;
            }
            return;
        }

        Super::onEvent(_event);
    }
    //--------------------------------------------------------------------------------------------------------
    Ray_MurphyAssistAIComponent_Template::Ray_MurphyAssistAIComponent_Template()
    : Super()
    , m_speedBlend(1.0f)
    , m_speedMin(0.0f)
    , m_speedMax(1.0f)
    , m_blendAtSpeedMin(0.0f)
    , m_blendAtSpeedMax(1.0f)
    , m_playerFollowOffset(0, 0)
    , m_targetEngageRadius (8.0f)
    , m_arriveStopDist (1.0f)
    , m_goBackMaxDeltaX(1.0f)
    , m_indicatorZOffset(0.0)
    , m_toggleBubbleSpawnOffset(0.0, 0.0, 0.0)
    , m_juiceShowTime(0)
    , m_stillTime(0)
    , m_movementEpsilon(0)
    , m_fontInitialHeight(1.f)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_MurphyAssistAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_MurphyAssistAIComponent_Template)
        SERIALIZE_MEMBER("speedBlend", m_speedBlend);
        SERIALIZE_MEMBER("speedMin", m_speedMin);
        SERIALIZE_MEMBER("speedMax", m_speedMax);
        SERIALIZE_MEMBER("blendAtSpeedMin", m_blendAtSpeedMin);
        SERIALIZE_MEMBER("blendAtSpeedMax", m_blendAtSpeedMax);
        SERIALIZE_MEMBER("playerFollowOffset", m_playerFollowOffset);
        SERIALIZE_MEMBER("targetEngageRadius", m_targetEngageRadius);
        SERIALIZE_MEMBER("arriveStopDist", m_arriveStopDist);
        SERIALIZE_MEMBER("goBackMaxDeltaX", m_goBackMaxDeltaX);
        SERIALIZE_MEMBER("murphyAssistDialogTextId", m_murphyAssistDialogTextId);
        SERIALIZE_MEMBER("indicatorPath", m_indicatorAct);
        SERIALIZE_MEMBER("toggleBubblePath", m_toggleBubbleAct);
        SERIALIZE_MEMBER("toggleBubbleSpawnOffset", m_toggleBubbleSpawnOffset);
        SERIALIZE_CONTAINER("juiceLineIds", m_juiceLineIds);
        SERIALIZE_MEMBER("juiceShowTime", m_juiceShowTime);
        SERIALIZE_MEMBER("stillTime", m_stillTime);
        SERIALIZE_MEMBER("movementEpsilon", m_movementEpsilon);
        SERIALIZE_MEMBER("relicPath", m_relicAct);
        SERIALIZE_MEMBER("electoonPath", m_electoonAct);
        SERIALIZE_MEMBER("fontInitialHeight", m_fontInitialHeight);
    END_SERIALIZATION()
}
