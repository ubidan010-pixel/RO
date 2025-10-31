#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SKULLCOINCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SkullCoinComponent.h"
#endif //_ITF_RAY_SKULLCOINCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#define SKULLCOIN_SUBDIV 128

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_SkullCoinComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_SkullCoinComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_MEMBER("isTaken",m_isTaken);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_SkullCoinComponent)
    END_VALIDATE_COMPONENT()

    const StringID SKULLCOIN_IDLE("Idle");
    const StringID SKULLCOIN_FLYINGTOSCORE("PickUp");
    const StringID SKULLCOIN_CARRIED("Carried");
    const StringID SKULLCOIN_STAR_PARTICLE_FX("star");
    const StringID SKULLCOIN_FOLLOWPLAYER_FX("followPlayer");


    //////////////////////////////////////////////////////////////////////////

    Ray_SkullCoinComponent::Ray_SkullCoinComponent()
    : Super()
    , m_fxControllerComponent(NULL)
    , m_state(State_NA)
    , m_timeInCurrentState(0)
    , m_isTaken(bfalse)
    , m_animComponent(NULL)
    , m_linkAlpha(1)
    , m_linkVibrationTimer(0)
    , m_smoothedTargetPosSpeed(0,0,0)
    , m_previousTargetPos(0,0,0)
    , m_speed(0,0)
    , m_pickedPosition(0,0,0)
    , m_tweenComponent(NULL)
    , m_currentMonitoredHealth(U32_INVALID)
    , m_starFX(U32_INVALID)
    , m_followFX(U32_INVALID)
    {
        // none
    }

    //////////////////////////////////////////////////////////////////////////

    Ray_SkullCoinComponent::~Ray_SkullCoinComponent()
    {
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), this);
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    void  Ray_SkullCoinComponent::updatePulsationScale(f32 _interpolator, f32 _t)
    {
        _interpolator *= _interpolator;
        f32 pulsation = getTemplate()->getMinPulsation()*(1-_interpolator)+getTemplate()->getMaxPulsation()*_interpolator;
        f32 amplitudeMin = getTemplate()->getMinScaleAtStart()*(1-_interpolator)+getTemplate()->getMinScaleAtEnd()*_interpolator;
        f32 amplitudeMax = getTemplate()->getMaxScaleAtStart()*(1-_interpolator)+getTemplate()->getMaxScaleAtEnd()*_interpolator;
        
        f32 sine = f32_Sin(pulsation*_t);
        sine *= sine*sine; 
        sine = f32_Abs(sine);
        f32 s = (amplitudeMin+(amplitudeMax-amplitudeMin)*sine);
        m_actor->setScale(Vec2d(s,s));
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_SkullCoinComponent::backToNormalScaleProgressively()
    {
        f32 targetScale = m_actor->getWorldInitialScale().m_x;
        f32 scale = m_actor->getScale().m_x;
        scale = (targetScale+scale)*0.5f;
        m_actor->setScale(Vec2d(scale,scale));
    }
    //////////////////////////////////////////////////////////////////////////

    void Ray_SkullCoinComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225), this);
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_tweenComponent = m_actor->GetComponent<TweenComponent>();
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),this);
#endif
        if (!getTemplate()->getTexture().isEmpty())
        {
            m_atlas.initialize(m_actor->getResourceGroup(),getTemplate()->getTexture(),SKULLCOIN_SUBDIV);
        }

        m_starFX = U32_INVALID;
        m_followFX = U32_INVALID;

        resetState();
    }

    void Ray_SkullCoinComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
        resetState();
    }

    //////////////////////////////////////////////////////////////////////////

    void Ray_SkullCoinComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    //////////////////////////////////////////////////////////////////////////

    void Ray_SkullCoinComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (EventQueryBlocksHits* blockHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
        {
            bbool canBlockHit = btrue;
            blockHits->setCanBlockHits(canBlockHit);
        }
        else if (Ray_EventRewardPickedUp* pickUp = _event->DynamicCast<Ray_EventRewardPickedUp>(ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225)))
        {
            pickup(pickUp->getActivator(), pickUp->getExplodesImmediately());
        }
        else if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
        {
#ifdef ITF_SUPPORT_EDITOR
            if ((m_actor->getWorld() != SPAWNER->getSpawnerWorld()) && !m_actor->isSpawned())
            {
                queryLumCount->addNormalLums(getTemplate()->getLumCount());
            }
#endif
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bbool Ray_SkullCoinComponent::checkIfReceivedHit()
    {
        Ray_Player * player = (Ray_Player*)RAY_GAMEMANAGER->getPlayerFromActor(m_rewardReceiver);
        if (!player || !player->getActive() || player->isDead())
        {
            m_currentMonitoredHealth = 0;
            return btrue;
        }
        u32 currentHealth = player->getHitPoints();
        if (m_currentMonitoredHealth==U32_INVALID)
        {
            m_currentMonitoredHealth = currentHealth;
        }
        bbool justHit =currentHealth<m_currentMonitoredHealth;
        m_currentMonitoredHealth = currentHealth;
        return justHit;
    }
    //////////////////////////////////////////////////////////////////////////
    Vec3d Ray_SkullCoinComponent::computePosFromVisualOffset()
    {
        return m_animComponent->getOffsettedPos();
    }
    //////////////////////////////////////////////////////////////////////////
    Vec2d Ray_SkullCoinComponent::computeVisualOffsetFromPos(const Vec3d &_newPos)
    {
        return (_newPos.truncateTo2D() - m_actor->get2DPos()).Rotate(-m_animComponent->getOffsettedAngle());
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_SkullCoinComponent::backToNormalPos(f32 _dt)
    {
        //Spike movement : simple formula for reaching a position with a constant force for a given duration
        f32 forceDuration = getTemplate()->getBackToNormalInertia();
        f32 frameCount = std::max(forceDuration/_dt,1.f);
        f32 sum_dt2 = frameCount * (frameCount - 1) * 0.5f * (_dt*_dt);
        Vec2d deltaPos = -m_visualOffset;;

        f32 deltaScale = 1+getTemplate()->getBackToNormalBounciness();
        deltaPos *= deltaScale;
        Vec2d acceleration = (deltaPos-(frameCount*_dt)*m_speed)*(1.f/sum_dt2);

        Vec2d force = acceleration;
        m_speed += force*_dt;
        m_visualOffset += m_speed*_dt;
    }
    //////////////////////////////////////////////////////////////////////////

    void Ray_SkullCoinComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        m_linkVibrationTimer += _deltaTime;        
        m_timeInCurrentState += _deltaTime;

        if (m_starFX!=U32_INVALID)
        {
            m_fxControllerComponent->setFXPos(m_starFX, computePosFromVisualOffset());
        }
        if (m_followFX != U32_INVALID)
        {
            m_fxControllerComponent->setFXPos(m_followFX, computePosFromVisualOffset());
        }


        m_animComponent->setInstancePositionOffset(m_visualOffset);

        f32 playRate = 1.f;

        //Update the states. 
        switch (m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Taken:
            {                
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_WaitingToBePickedUp:
            {
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////

        case State_BackToNormalPos:
            {
                backToNormalScaleProgressively();
                backToNormalPos(_deltaTime);

                m_linkAlpha = m_linkAlpha*0.9f;
                if (m_visualOffset.sqrnorm()<0.001f)
                {
                    setState(State_WaitingToBePickedUp);
                }
            } break;
        case State_FollowPlayer:
            {                
                m_linkAlpha = m_linkAlpha*0.1f+0.9f;
                if (m_linkAlpha>1)
                    m_linkAlpha = 1;
                f32 challengeDuration = getTemplate()->getChallengeDuration();
                if (challengeDuration==0) challengeDuration = 0.01f;
                Vec3d newPos = computePosFromVisualOffset();

                f32 pickedDistanceSQ = (m_actor->getPos()-newPos).sqrnorm();
                f32 maxDistance = getTemplate()->getMaxDistanceBeforeExploding();
                bbool tooFar = (pickedDistanceSQ>maxDistance*maxDistance);

                if (m_timeInCurrentState>=challengeDuration || tooFar)
                {
                    setState(State_Exploding);
                }
                else
                {
                    Player * player = RAY_GAMEMANAGER->getPlayerFromActor(m_rewardReceiver);
                    Actor *target = NULL;
                    if (player)
                        target = player->getActor(); //should not be null
                    if (checkIfReceivedHit() || !target)
                    {
                        setState(State_BackToNormalPos);
                    }
                    else
                    {
                        f32 percent = std::min(m_timeInCurrentState/challengeDuration,1.f);
                        updatePulsationScale(percent, m_timeInCurrentState);

                        Vec2d offset = getTemplate()->getPlayerFollowOffset();
                        if (AIUtils::getFlipped(target))
                        {
                            offset.m_x = -offset.m_x;
                        }
                        offset *= target->getScale();
                        Vec3d targetPos = target->getPos() + offset.to3d();                        
                        newPos.m_z = targetPos.m_z;


                        // smooth position depending on target speed
                        AIUtils::advancedMichelSmooth(
                            newPos,
                            m_smoothedTargetPosSpeed,
                            m_previousTargetPos,
                            targetPos,
                            getTemplate()->getSpeedBlend(),
                            getTemplate()->getSpeedMin(),getTemplate()->getSpeedMax(),
                            getTemplate()->getBlendAtSpeedMin(),
                            getTemplate()->getBlendAtSpeedMax(),
                            _deltaTime
                            );

                        m_visualOffset = computeVisualOffsetFromPos(newPos);
                    }
                        
                }
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Exploding:
            {
                playRate = 1.f;
                //backToNormalScaleProgressively();
                m_linkAlpha = m_linkAlpha*0.5f;
                m_actor->enable();
                if (m_animComponent->isSubAnimFinished() && (!m_fxControllerComponent || m_fxControllerComponent->IsAllFXStopped()))
                {
                    setState(State_Taken);
                }
            } break;
        }

        m_animComponent->setPlayRate(playRate);

    }
    //////////////////////////////////////////////////////////////////////////
    ///Set state, and initialize the state we enter
    void Ray_SkullCoinComponent::setState(State_t _state)
    {
        //leaving old state
        m_fxControllerComponent->stopFX(SKULLCOIN_STAR_PARTICLE_FX);
        m_fxControllerComponent->stopFX(SKULLCOIN_FOLLOWPLAYER_FX);

        m_starFX = U32_INVALID;
        m_followFX = U32_INVALID;

        //
        if (_state!=m_state)
        {
            m_timeInCurrentState = 0;
            m_state = _state;
        }

        bbool canBeTweened = btrue;
        bbool mustBeAlwaysActive = bfalse;
        ///Entering new state
        switch(_state)
        {
        case State_WaitingToBePickedUp:
            m_currentMonitoredHealth = U32_INVALID;
            m_actor->setScale(m_actor->getLocalInitialScale());
            m_rewardReceiver.invalidate();
            m_visualOffset.clear();
            m_linkAlpha = 0;
            setCurrentAnim(SKULLCOIN_IDLE);
            break;
        case State_Exploding:
            {
                canBeTweened = bfalse;
                backToNormalScaleProgressively();
                mustBeAlwaysActive = btrue;
                Ray_EventSpawnRewardLum onSpawnReward;
                Vec2d worldOffset = (computePosFromVisualOffset()-m_actor->getPos()).truncateTo2D();                
                onSpawnReward.setup(m_actor, worldOffset.Rotate(-m_actor->getAngle()).to3d());
                onSpawnReward.setNumRewards(getTemplate()->getLumCount());
                onSpawnReward.setForceYellow(btrue);
                onSpawnReward.setRewardReceiver(m_rewardReceiver);

                RAY_GAMEMANAGER->SpawnRewardLum( &onSpawnReward );

                setCurrentAnim(SKULLCOIN_FLYINGTOSCORE);
                ITF_ASSERT_CRASH(!m_isTaken, "already taken. Critical bug.");
                m_isTaken = btrue;                
                m_starFX = m_fxControllerComponent->playFX(SKULLCOIN_STAR_PARTICLE_FX);               

            } break;
        case State_FollowPlayer:
            {
                setCurrentAnim(SKULLCOIN_CARRIED);
                canBeTweened = bfalse;
                m_followFX = m_fxControllerComponent->playFX(SKULLCOIN_FOLLOWPLAYER_FX);
                m_smoothedTargetPosSpeed.set(0,0,0);
                m_previousTargetPos = m_actor->getPos();

                m_linkVibrationTimer = 0;
                m_visualOffset.clear();
                mustBeAlwaysActive = btrue;
                m_currentMonitoredHealth = U32_INVALID;
            } break;
        case State_BackToNormalPos:
            {
                m_speed = m_smoothedTargetPosSpeed.truncateTo2D();
                m_speed.normalize();
                m_speed*=getTemplate()->getBackToNormalInitSpeed();
                mustBeAlwaysActive = btrue;
            } break;
        case State_Taken:
            {
                m_linkAlpha = 0;
                m_currentMonitoredHealth = U32_INVALID;

                m_visualOffset.clear();
                m_actor->disable();
            } break;
        }

        setAlwaysActive(mustBeAlwaysActive);
        m_actor->setAlwaysVisible(mustBeAlwaysActive);
        if (m_tweenComponent)
        {
            if (!canBeTweened && !m_tweenComponent->isTweeningSuspended())
                m_tweenComponent->suspendTweening();
            if (canBeTweened && m_tweenComponent->isTweeningSuspended())
                m_tweenComponent->resumeTweening();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    f32 Ray_SkullCoinComponent::computeLineThickness(f32 _x, f32 _endX, f32 _radius, f32 _minThicknessPercent)
    {
        f32 y1SQ, y2SQ;

        f32 xoffset = _x - _radius;
        y1SQ = _radius*_radius - xoffset*xoffset;

        f32 xfromEndOffset = _endX-_radius-_x;
        y2SQ = _radius*_radius - xfromEndOffset*xfromEndOffset;
        
        f32 minWidth = (_radius*_minThicknessPercent);
        f32 deltaWidth = _radius-minWidth;

        f32 xFromCenter = _x-(_endX*0.5f);
        f32 yMiddle = 0;
        if (_x>=_radius && xfromEndOffset>0)
        {
            f32 den = (_endX-2*_radius)*0.5f;
            if (den>0.1f)
            {
                xFromCenter/= den;
            }
            else
            {
                xFromCenter = 0;
            }
        
            yMiddle = deltaWidth*xFromCenter*xFromCenter+minWidth;
        }
        return f32_ApproxSqrt(std::max(yMiddle*yMiddle, std::max(y1SQ,y2SQ)));
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_SkullCoinComponent::Draw()
    {
        Super::Draw();

        if (m_visualOffset.sqrnorm()>0)
        {
            f32 radius = getTemplate()->getRadius();
            const f32  zOffset = -0.001f;
            Vec3d startPoint(m_actor->getPos());
            Vec3d endPoint(m_animComponent->getOffsettedPos());
            Vec2d direction = (endPoint-startPoint).truncateTo2D();
            direction.normalize();

            startPoint.m_z += zOffset;
            endPoint.m_z += zOffset;
            startPoint -= direction.to3d()*radius;
            //endPoint += direction.to3d()*radius;

            Vec3d upward = direction.getPerpendicular().to3d();

            Vec3d step = (endPoint-startPoint)/SKULLCOIN_SUBDIV;
            Vec3d currentPoint = startPoint;
            f32 length = (endPoint-startPoint).norm();

            f32 percentOfLenInterp = length/getTemplate()->getLenghtForMinThickness();
            percentOfLenInterp = f32_Clamp(percentOfLenInterp,0,1);
            percentOfLenInterp = 1-percentOfLenInterp;
            percentOfLenInterp = percentOfLenInterp*percentOfLenInterp;
            percentOfLenInterp = 1-percentOfLenInterp;
            f32 thicknessMultiplicationFactor = f32_Lerp(1, getTemplate()->getMinLinkThicknessFactor(), percentOfLenInterp);


            f32 x=0;
            f32 xStep = length/SKULLCOIN_SUBDIV;
            static Vec3d posUp[SKULLCOIN_SUBDIV+1];
            static Vec3d posDown[SKULLCOIN_SUBDIV+1];
            u32 colors[SKULLCOIN_SUBDIV+1];
            float startAlphaLen = std::max(length*getTemplate()->getStartAlphaLen(),0.01f);
            float endAlphaLen = std::max(length*getTemplate()->getEndAlphaLen(),0.01f);
            float linkAlphaMultiplier  = getTemplate()->getLinkAlphaMultiplier()*m_linkAlpha;

            //f32 thicknesses[SKULLCOIN_SUBDIV+1];
            //f32 vpm = 0.5f/radius;
            for (u32 i=0; i<=SKULLCOIN_SUBDIV; i++, x+=xStep, currentPoint +=step)
            {                
                f32 thickness = computeLineThickness(x, length, radius, thicknessMultiplicationFactor);
                //thicknesses[i]= thickness*vpm;
                posUp[i] = (currentPoint + thickness*upward);
                posDown[i] = currentPoint - thickness*upward;
                f32 alpha;
                if (x<startAlphaLen)
                {
                    alpha = x/startAlphaLen;
                }
                else if (x>(length-endAlphaLen))
                {
                    alpha = (length-x)/endAlphaLen;
                }
                else
                {
                    alpha = 1;
                }
                colors[i] = Color(alpha*linkAlphaMultiplier,1,1,1).getAsU32();
            }

            VertexPCT* vtx = m_atlas.startFillVertexBuffer();
            f32 u = 0;
            f32 uStep = getTemplate()->getBezierUPerMeter()*length/SKULLCOIN_SUBDIV;
            for (u32 i=0; i<SKULLCOIN_SUBDIV; i++)
            {
                const Vec3d &up=posUp[i];
                const Vec3d &upp1 = posUp[i+1];
                f32 z = up.m_z;
                f32 zp1 = upp1.m_z;
                f32 minV = 0.f;//thicknesses[i];
                f32 maxV = 1.f;//0.5f+minV;
                u32 color = colors[i];
                //minV = 0.5f-minV;
                vtx[0].setData((const Vec2d&)up, Vec2d(u,minV), color,z);
                vtx[1].setData((const Vec2d&)posDown[i], Vec2d(u,maxV), color,z);
                u+=uStep;
                color = colors[i+1];
                vtx[2].setData((const Vec2d&)posDown[i+1], Vec2d(u,maxV), color,zp1);
                vtx[3].setData((const Vec2d&)upp1, Vec2d(u,minV), color,zp1);
                vtx+=4;
            }
            m_atlas.endFillVertexBuffer();
            m_atlas.setFogColor(getColorFog());
            //m_atlas.setColor(Color(m_linkAlpha,1,1,1));
            GFX_MATERIAL &material = m_atlas.getMaterial();
            material.m_blendMode = (GFX_BLENDMODE)getTemplate()->getBlendMode();
            material.m_uvAnim.m_speedTrans = getTemplate()->getUVAnimTrans();
            material.m_uvAnim.m_uvFlagAnim = UVANIM_F_TRANS;
            m_atlas.drawVertexBuffer(m_actor->getDepth()+zOffset, SKULLCOIN_SUBDIV);
        }
        
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_SkullCoinComponent::onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion)
    {
        Super::onActorRemovedFromScene(_pScene, _forDeletion);
        setAlwaysActive(bfalse);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_SkullCoinComponent::setAlwaysActive(bbool _alwaysActive)
    {
        if (!m_actor)
            return;

        Scene *scene = m_actor->getScene();
        if (!scene)
            return;
        if (_alwaysActive)
        {
            if (scene->getAlwaysActiveActors().find(m_actor)<0)
            {
                scene->setAlwaysActiveActor(m_actor);
            }
        }
        else
        {
            if (scene->getAlwaysActiveActors().find(m_actor)>=0)
            {
                scene->setAlwaysActiveActor(m_actor, bfalse);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_SkullCoinComponent::setCurrentAnim(const StringID &_name)
    {
        if (_name==m_currentAnim) 
            return; 
        m_currentAnim = _name; 
        m_animComponent->setAnim(m_currentAnim);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_SkullCoinComponent::resetState()
    {
        m_currentAnim.invalidate();
        m_visualOffset.clear();
        m_currentMonitoredHealth = U32_INVALID;

        if (m_isTaken)
            setState(State_Taken);
        else
            setState(State_WaitingToBePickedUp);        
    }

    //////////////////////////////////////////////////////////////////////////

    void Ray_SkullCoinComponent::pickup(ActorRef _playerRef, bbool _explodesImmediately)
    {
        if (m_state!=State_WaitingToBePickedUp)
            return;
        m_pickedPosition = m_actor->getPos();
        ITF_ASSERT(!m_isTaken);
        m_rewardReceiver = _playerRef;
        if (!m_isTaken)
        {
            if (_explodesImmediately)
            {
                setState(State_Exploding);
            }
            else
            {
                setState(State_FollowPlayer);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Template
    //////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(Ray_SkullCoinComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_SkullCoinComponent_Template)
        SERIALIZE_MEMBER("lumCount", m_lumCount);
        SERIALIZE_MEMBER("challengeDuration", m_challengeDuration);
        SERIALIZE_MEMBER("bezierUPerMeter", m_bezierUPerMeter);
        SERIALIZE_MEMBER("bezierStartWidth", m_bezierStartWidth);
        SERIALIZE_MEMBER("bezierEndWidth", m_bezierEndWidth);
        SERIALIZE_MEMBER("tesselationLength", m_tesselationLength);
        SERIALIZE_MEMBER("blendMode", m_blendMode);
        SERIALIZE_MEMBER("texture", m_texture);
        SERIALIZE_MEMBER("backToNormalSpeed", m_backToNormalSpeed);
        SERIALIZE_MEMBER("minPulsation", m_minPulsation);
        SERIALIZE_MEMBER("maxPulsation", m_maxPulsation);
        SERIALIZE_MEMBER("minScaleAtStart", m_minScaleAtStart);
        SERIALIZE_MEMBER("maxScaleAtStart", m_maxScaleAtStart);
        SERIALIZE_MEMBER("minScaleAtEnd", m_minScaleAtEnd);
        SERIALIZE_MEMBER("maxScaleAtEnd", m_maxScaleAtEnd);
        SERIALIZE_MEMBER("linkPulsation", m_linkPulsation);
        SERIALIZE_MEMBER("linkAmplitude", m_linkAmplitude);
        SERIALIZE_MEMBER("playerFollowOffset", m_playerFollowOffset);

        SERIALIZE_MEMBER("speedBlend", m_speedBlend);
        SERIALIZE_MEMBER("speedMin", m_speedMin);
        SERIALIZE_MEMBER("speedMax", m_speedMax);
        SERIALIZE_MEMBER("blendAtSpeedMin", m_blendAtSpeedMin);
        SERIALIZE_MEMBER("blendAtSpeedMax", m_blendAtSpeedMax);

        SERIALIZE_MEMBER("backToNormalInertia", m_backToNormalInertia);
        SERIALIZE_MEMBER("backToNormalBounciness", m_backToNormalBounciness);
        SERIALIZE_MEMBER("backToNormalInitSpeed", m_backToNormalInitSpeed);

        SERIALIZE_MEMBER("maxDistanceBeforeExploding", m_maxDistanceBeforeExploding);
        SERIALIZE_MEMBER("radius", m_radius);
        SERIALIZE_MEMBER("minLinkThicknessFactor", m_minLinkThicknessFactor);
        SERIALIZE_MEMBER("lenghtForMinThickness", m_lenghtForMinThickness);
        SERIALIZE_MEMBER("uvAnimTrans", m_uvAnimTrans);
        SERIALIZE_MEMBER("startAlphaLen", m_startAlphaLen);
        SERIALIZE_MEMBER("endAlphaLen", m_endAlphaLen);
        SERIALIZE_MEMBER("linkAlphaMultiplier", m_linkAlphaMultiplier);

    END_SERIALIZATION()

    //////////////////////////////////////////////////////////////////////////

    Ray_SkullCoinComponent_Template::Ray_SkullCoinComponent_Template()
        : Super()
        , m_lumCount(25)
        , m_challengeDuration(20.f)
        , m_bezierUPerMeter(1.f)
        , m_bezierStartWidth(0.2f)
        , m_bezierEndWidth(0.2f)
        , m_tesselationLength(0.3f)
        , m_blendMode(7)
        , m_backToNormalSpeed(2.f)
        , m_minPulsation(1.f)
        , m_maxPulsation(2.f)
        , m_minScaleAtStart(2)
        , m_maxScaleAtStart(2)
        , m_minScaleAtEnd(0.5f)
        , m_maxScaleAtEnd(3.f)
        , m_linkPulsation(0.5f)
        , m_linkAmplitude(0.1f)
        , m_playerFollowOffset(0,0)
        , m_backToNormalInertia(0.1f)
        , m_backToNormalBounciness(0.5f)
        , m_speedBlend(1.0f)
        , m_speedMin(0.0f)
        , m_speedMax(1.0f)
        , m_blendAtSpeedMin(0.0f)
        , m_blendAtSpeedMax(1.0f)
        , m_maxDistanceBeforeExploding(20)
        , m_backToNormalInitSpeed(10)
        , m_radius(1.f)
        , m_minLinkThicknessFactor(0.5f)
        , m_lenghtForMinThickness(5.f)
        , m_uvAnimTrans(0,0)
        , m_startAlphaLen(0.f)
        , m_endAlphaLen(0.f)
        , m_linkAlphaMultiplier(1.f)
    {
        // none
    }

    //////////////////////////////////////////////////////////////////////////

    Ray_SkullCoinComponent_Template::~Ray_SkullCoinComponent_Template()
    {
    }

    //////////////////////////////////////////////////////////////////////////


}
