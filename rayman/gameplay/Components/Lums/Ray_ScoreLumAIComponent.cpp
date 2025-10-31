#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SCORELUMAICOMPONENT_H_
#include "rayman/gameplay/Components/Lums/Ray_ScoreLumAIComponent.h"
#endif //_ITF_RAY_SCORELUMAICOMPONENT_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_ScoreLumAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ScoreLumAIComponent_Template)
    SERIALIZE_MEMBER("lumAtlasPath", m_lumAtlasPath);
    SERIALIZE_MEMBER("framePerSecond", m_framePerSecond);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_ScoreLumAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ScoreLumAIComponent)
    END_SERIALIZATION()

    void Ray_ScoreLumAIComponent::onActorLoaded(Pickable::HotReloadType _hotReloadType)
    {
        Super::onActorLoaded(_hotReloadType);
        const u32 maxParticle = 128;
        m_currentLumParticles.reserve(maxParticle);

        m_lumAtlas.initialize(m_actor->getResourceGroup(),getTemplate()->getLumAtlasPath(),maxParticle);

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPreCheckpointSave,3321202665),this);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_ScoreLumAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
        m_currentLumParticles.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    Ray_ScoreLumAIComponent::~Ray_ScoreLumAIComponent()
    {
        m_lumAtlas.clear();
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPreCheckpointSave,3321202665),this);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_ScoreLumAIComponent::sendLumReachesScoreLum(Ray_PickedLumTrajectoryFollower &_lum)
    {
        u32 scoreValue = _lum.getScoreValue();
        u32 playerIndex = _lum.getPlayerIndex();
        //Signal the score lum behavior that a lum reached it
        Ray_EventLumReachesScore lumEvent;
        lumEvent.m_isAccrobatic = bfalse;
        lumEvent.m_valueToAdd = scoreValue;
        lumEvent.m_playerIndex = playerIndex;
        lumEvent.setSender(m_actor->getRef());
        m_actor->onEvent(&lumEvent);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_ScoreLumAIComponent::flushLumParticles()
    {
        for (i32 lumIndex=0; lumIndex<(i32)m_currentLumParticles.size(); lumIndex++)
        {
            Ray_PickedLumTrajectoryFollower &lum = m_currentLumParticles[lumIndex];
            reachScoreLumIfNotAlreadyDone(lum);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_ScoreLumAIComponent::playJoinSound(bbool /*_joins*/)
    {
        if (m_soundComponent)
            m_soundComponent->playSound(ITF_GET_STRINGID_CRC(playerJoined,486793185));
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_ScoreLumAIComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);
        if (EventRevertToLastCheckpoint *checkpointEvent = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)))
        {
            m_currentLumParticles.clear();
        }
        else if (EventPreCheckpointSave *preCheckpointSave = _event->DynamicCast<EventPreCheckpointSave>(ITF_GET_STRINGID_CRC(EventPreCheckpointSave,3321202665)))
        {
            flushLumParticles();
            for (u32 i=0; i<s_maxPlayers; i++)
            {
                Ray_PlayerHudScoreComponent::flushPendingScoreIncrements(i);
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_ScoreLumAIComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);
        Vec3d projPos, projSpeed; 
        Vec3d pos;
        bbool isWaitingBeforeStart=bfalse;
        f32 alpha=1;
        Vec3d tmpProj;
        AABB projAABB;

        m_lumAtlas.clearVertices();
        Texture *atlasTexture = m_lumAtlas.getAtlasTexture();
        u32 frameCount = 1;
        if (atlasTexture)
        {
            frameCount = (u32)atlasTexture->getUVAtlas()->getNumberUV();
        }

        for (i32 lumIndex=0; lumIndex<(i32)m_currentLumParticles.size(); lumIndex++)
        {
            Ray_PickedLumTrajectoryFollower &lum = m_currentLumParticles[lumIndex];

            u32 frameIndex = (u32)(lum.getTimeSinceTaken()*getTemplate()->getFramePerSecond());
            frameIndex %=frameCount;
            bbool justReachedAlphaThreshold = bfalse;

            f32 lumScale=1;
            f32 lambda = lum.computePosOnTrajectory(projPos, projSpeed, isWaitingBeforeStart, 0, alpha, justReachedAlphaThreshold, lumScale);
            const AABB &relAABB = lum.getRelativeParticleAABB();
            if (projPos.m_z >0&& projPos.m_z<1)
            {
                GFX_ADAPTER->compute2DTo3D(projPos, pos);
                AABB worldAABB(relAABB);
                worldAABB.Translate((const Vec2d&)pos);
                
                projAABB.invalidate();
                
                GFX_ADAPTER->compute3DTo2D(worldAABB.getMin().to3d(pos.m_z), tmpProj);
                projAABB.grow(tmpProj);
                GFX_ADAPTER->compute3DTo2D(worldAABB.getMax().to3d(pos.m_z), tmpProj);
                projAABB.grow(tmpProj);
                projAABB.ScaleFromCenter(Vec2d(lumScale,lumScale));


                m_lumAtlas.addQuad(frameIndex, projAABB.getMin(),projAABB.getMinXMaxY(), projAABB.getMax(), projAABB.getMaxXMinY(), 0.001f,Color(alpha,1,1,1).getAsU32());
                if (justReachedAlphaThreshold)
                {
                    if (m_soundComponent)
                        m_soundComponent->playSound(ITF_GET_STRINGID_CRC(lumReachedScore,766007316));

                    lum.setClientProcessedReachedAlphaThreshold();
                    reachScoreLumIfNotAlreadyDone(lum);
                }

            }
            else
            {
                lambda = 1.f;
            }

            //Delete when finished. DO NOT ADD CODE AFTER DELETION
            if (lambda>=1)
            {
                reachScoreLumIfNotAlreadyDone(lum);
                m_currentLumParticles.erase(m_currentLumParticles.begin()+lumIndex);
                lumIndex--;
            }
            else
            {
                lum.incrementTime(_deltaTime);
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_ScoreLumAIComponent::reachScoreLumIfNotAlreadyDone(Ray_PickedLumTrajectoryFollower &_lum)
    {
        if (!_lum.getHasAlreadySentReachScoreLum())
        {
            _lum.setHasAlreadySentReachScoreLum(btrue);
            sendLumReachesScoreLum(_lum);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_ScoreLumAIComponent::Draw2D()
    {
        Super::Draw2D();

        m_lumAtlas.draw2d(UI_TEXTMANAGER->getAnimDepthRank(), bfalse);

    }
    //////////////////////////////////////////////////////////////////////////
    Ray_PickedLumTrajectoryFollower &Ray_ScoreLumAIComponent::pushLumTrajectoryFollower(const Ray_PickedLumTrajectoryFollower& _lum)
    {
        m_currentLumParticles.push_back(_lum);
        return m_currentLumParticles.back();
    }


} // namespace ITF

