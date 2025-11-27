#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_STRINGWAVEGENERATORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StringWaveGeneratorComponent.h"
#endif //_ITF_RAY_STRINGWAVEGENERATORCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef __ITF_RAY_FLUIDSHAPERS_H__
#include "rayman/gameplay/Components/Misc/Ray_FluidShapers.h"
#endif

#ifndef _ITF_RAY_WAVEBALLCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WaveBallComponent.h"
#endif //_ITF_RAY_WAVEBALLCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_StringWaveGeneratorComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_StringWaveGeneratorComponent)
            BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
                SERIALIZE_MEMBER("startsActivated", m_startsActivated);
            END_CONDITION_BLOCK()
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_StringWaveGeneratorComponent)
        VALIDATE_COMPONENT_PARAM("syncOffset", 0.f <= getTemplate()->getSyncOffset() && getTemplate()->getSyncOffset() <= 1.f, "must be in the [0,1] range");
        VALIDATE_COMPONENT_PARAM("growthTransitionDuration", getTemplate()->getGrowthTransitionDuration()>0, "Invalid Off=>On duration");
        VALIDATE_COMPONENT_PARAM("", !(getTemplate()->getIsSpawner() && getTemplate()->getIsFullLength()), "cannot have a spawner with full length");
        VALIDATE_COMPONENT_PARAM("growthTransition_TimeStartsRed", getTemplate()->getGrowthTransitionDuration()>getTemplate()->getGrowthTransition_TimeStartsRed(), "growthTransition_TimeStartsRed must be smaller than total transition");
        VALIDATE_COMPONENT_PARAM("smallScaleWhenBlackInTransition", getTemplate()->getSmallScaleWhenBlackInTransition()<1, "must be < 1");
    END_VALIDATE_COMPONENT()


    ///////////////////////////////////////////////////////////////////////////////////////////

    Ray_StringWaveGeneratorComponent::Ray_StringWaveGeneratorComponent()
        : Super()
        , m_startsActivated(btrue)
        , m_activated(btrue)
        , m_phantomRadius(1)
        , m_normalizedEdgeDir(1,0)
        , m_state(State_NA)
        , m_timeInCurrentState(0.f)
        , m_scaleBecauseOfTransition(1.f)
        , m_needsFrieze(bfalse)
        , m_fxControllerComponent(NULL)
        , m_offToOnFX(U32_INVALID)
    {
    }

    Ray_StringWaveGeneratorComponent::~Ray_StringWaveGeneratorComponent()
    {
        while (m_fluidShapers.size())
        {
            popOldestWave(bfalse);
        }
        removeObsoleteRegisteredWaves(btrue);
        invalidateRelatedFrieze();
    }

    void Ray_StringWaveGeneratorComponent::stopOffToOnFXIfNeeded()
    {
        if (m_offToOnFX!=U32_INVALID)
        {
            m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

            if (m_fxControllerComponent)
                m_fxControllerComponent->stopFXFromHandle(m_offToOnFX);
            m_offToOnFX = U32_INVALID;
        }
    }

    void Ray_StringWaveGeneratorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSpawnWave,588272106), this);
        m_phantomRadius = 1;
        m_scaleBecauseOfTransition = 1;
        m_needsFrieze = (m_actor->GetComponent<Ray_WaveBallComponent>()==NULL);
        setState(State_On);


        ITF_ASSERT(m_fluidShapers.size()==0);
        
        if (!getTemplate()->getIsSpawner())
        {
            //if not a spawner, auto create new item
            createNewDefaultWave();
        }

        m_activated = m_startsActivated;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_StringWaveGeneratorComponent::decideIfMustSendExciterMessageToWaveBall(Frise *_linkedFrieze, FluidShaperDesc &_shaperDesc, f32 _previousOffset, bbool &_isPreparing)
    {
        FluidShaper *shaper = _shaperDesc.m_shaper;
        bbool mustSendExciterMessage = bfalse;

        _isPreparing = bfalse;

        if (_linkedFrieze)
        {
            f32 projectedActorPos = _shaperDesc.m_shaper->project2DPos(m_actor->get2DPos());
            f32 cueWidth = getTemplate()->getColoredSpawningCueWidth();
            f32 cueWidthLeft=0, cueWidthRight=0;
            if (_shaperDesc.m_hasTemporalAnnouncement)
            {
                if (_shaperDesc.m_speed>0)
                {
                    cueWidthLeft = 0;
                    cueWidthRight = cueWidth;
                }
                else if (_shaperDesc.m_speed<0)
                {
                    cueWidthLeft = cueWidth;
                    cueWidthRight = 0;
                }
            }
            f32 waveBorder1WithCue = shaper->getLinearOffset() - shaper->getRadius();
            f32 waveBorder2WithCue = shaper->getLinearOffset() + shaper->getRadius();
            f32 waveBorder1 = waveBorder1WithCue + cueWidthLeft;
            f32 waveBorder2 = waveBorder2WithCue - cueWidthRight;
            mustSendExciterMessage = (projectedActorPos>=waveBorder1) && (projectedActorPos<=waveBorder2);
            _isPreparing = _shaperDesc.m_timeLeftBeforeMoving>0;
        }
        else
        {
            mustSendExciterMessage = (_previousOffset<=0) && (_shaperDesc.m_offset>=0) ;

        }

        mustSendExciterMessage = mustSendExciterMessage && (m_state!=State_Off);
        return mustSendExciterMessage;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::sendWaveGeneratorNotification(FluidShaperDesc &_shaperDesc, bbool _exciterState, bbool _isPreparing_ExicterMessage)
    {
        Ray_EventWaveGeneratorNotification notification;
        notification.setSender(m_actor->getRef());
        bbool exciterState_Filtered = _shaperDesc.m_shaper->getIsColored() && _exciterState && !_isPreparing_ExicterMessage;
        notification.setup((u32)_shaperDesc.m_waveType,
            m_scaleBecauseOfTransition, 
            exciterState_Filtered,
            _isPreparing_ExicterMessage
            );
        m_actor->onEvent(&notification);       
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::popOldestWave(bbool _killSounds)
    {
        if (m_fluidShapers.size()==0)
            return; //nothing to do

        u32 lastIndex = (u32)(m_fluidShapers.size()-1);
        FluidShaperDesc &shaperDesc = m_fluidShapers[lastIndex];
        shaperDesc.m_shaper->decrementRefCount();
        if (_killSounds)
        {
            stopWaveFX(&shaperDesc);
            stopPreparingWaveFX(&shaperDesc);
        }
        m_fluidShapers.eraseKeepOrder(lastIndex);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::stopWaveFX(FluidShaperDesc *_shaperDesc)
    {
        if (_shaperDesc->m_waveFXHandle!=U32_INVALID)
        {
            m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
            if (m_fxControllerComponent)
            {
                m_fxControllerComponent->stopFXFromHandle(_shaperDesc->m_waveFXHandle);
            }
            _shaperDesc->m_waveFXHandle = U32_INVALID;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::stopPreparingWaveFX(FluidShaperDesc *_shaperDesc)
    {
        if (_shaperDesc->m_preparingWaveFXHandle!=U32_INVALID)
        {
            m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
            if (m_fxControllerComponent)
            {
                m_fxControllerComponent->stopFXFromHandle(_shaperDesc->m_preparingWaveFXHandle);
            }
            _shaperDesc->m_preparingWaveFXHandle = U32_INVALID;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::createNewWave(WaveType_t _waveType, f32 _speed, f32 _timeBeforeMoving, bbool _mustOffsetByRadius)
    {
        if (m_fluidShapers.size()>=getTemplate()->getSpawneeLimit())
        {
            popOldestWave();
        }
        switch(_waveType)
        {
        case Wave_Spikes:
            {
                Ray_FluidShaper_Spikes *spike = newAlloc(mId_Gameplay, Ray_FluidShaper_Spikes);
                FluidShaperDesc shaperDesc(Wave_Spikes,spike, _speed, _timeBeforeMoving);
                m_fluidShapers.insert(shaperDesc, 0);                    
            } break;

        case Wave_Bouncer:
            {
                Ray_FluidShaper_Bump *bump = newAlloc(mId_Gameplay, Ray_FluidShaper_Bump);
                m_fluidShapers.insert(FluidShaperDesc(Wave_Bouncer,bump, _speed, _timeBeforeMoving), 0);           
            } break;

        default : 
            {
                ITF_WARNING(m_actor, bfalse, "Unknown wave type");
                return;
            }
        }
        if (_mustOffsetByRadius)
        {
            FluidShaperDesc &shaperDesc = m_fluidShapers[0];
            FluidShaper *shaper = shaperDesc.m_shaper;
            shaper->setRadius(m_phantomRadius*m_actor->getScale().m_x);
            float offsetToApply = shaper->getRadius();
            offsetToApply -= getTemplate()->getColoredSpawningCueWidth();
            shaperDesc.m_hasTemporalAnnouncement = btrue;
            if (_speed>0)
            {
                shaperDesc.m_offset -= offsetToApply;
            } else if (_speed<0)
            {
                shaperDesc.m_offset += offsetToApply;
            }
        }
        m_fluidShapers[0].m_shaper->incrementRefCount();
        setShaperDynamicParameters(0, 0);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::createNewDefaultWave()
    {
        WaveType_t waveType = getTemplate()->getIsSpike()?Wave_Spikes:Wave_Bouncer;
        createNewWave(waveType,0,0,bfalse);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventTrigger* eventTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            processTrigger(eventTrigger);
        }
        else if ( Ray_EventSpawnWave *eventSpawn = _event->DynamicCast<Ray_EventSpawnWave>(ITF_GET_STRINGID_CRC(Ray_EventSpawnWave,588272106)))
        {
            processSpawnWave(eventSpawn);
        }
    }

    bbool Ray_StringWaveGeneratorComponent::getFriezeProjectionOnEdge(Frise *_frieze, f32 &_minX, f32 &_maxX, Vec2d &_center)
    {
        _minX = FLT_MAX;
        _maxX = -FLT_MAX;
        _center.set(0,0);

        i32 pointCount=0;
        f32 z = _frieze->getDepth();

        AABB bounding;
        bounding.invalidate();

        const PolyPointList& pointList = _frieze->getPointsData();
        u32 segmentCount = pointList.getVectorsCount();

        for (u32 i=0; i<segmentCount; i++)
        {
            const Vec2d &p0_untransformed= pointList.getPosAt(i);
            const Vec2d &p1_untransformed= pointList.getPosAt(i+1);

            Vec3d p0_3d, p1_3d;
            p0_3d = _frieze->transformPos(Vec3d(p0_untransformed.m_x, p0_untransformed.m_y, z));
            p1_3d = _frieze->transformPos(Vec3d(p1_untransformed.m_x, p1_untransformed.m_y, z));

            const PolyLineEdge &edge = pointList.getEdgeAt(i);
            if (edge.m_holeMode & PolyLineEdge::Hole_Visual)
            {
                continue;
            }

            const Vec2d &p0 = (const Vec2d&)p0_3d;
            const Vec2d &p1 = (const Vec2d&)p1_3d;

            if (pointCount==0)
            {
                bounding.setMin(p0);
                bounding.setMax(p0);
                bounding.grow(p1);
            }
            else
            {
                bounding.grow(p0);
                bounding.grow(p1);
            }
            pointCount += 2;

            f32 x0 = p0.dot(m_normalizedEdgeDir);
            f32 x1 = p1.dot(m_normalizedEdgeDir);

            _minX = std::min(_minX, x0);
            _minX = std::min(_minX, x1);

            _maxX = std::max(_maxX, x0);
            _maxX = std::max(_maxX, x1);
        }

        if (pointCount)
        {
            _center = bounding.getCenter();
            return btrue;
        }
        return bfalse;
    }

    i32 Ray_StringWaveGeneratorComponent::projectOnPointList(Frise *_frieze, const Vec2d &pos, const PolyPointList& _pointList, f32 &_percentInSegment, Vec2d &_edgeDirection, f32 &_bestDist, Vec3d &_posWithProjection0)
    {
        i32 segmentIndex = -1;
        u32 segmentCount = _pointList.getVectorsCount();

        f32 z = _frieze->getDepth();

        for (u32 i=0; i<segmentCount; i++)
        {
            const Vec2d &p0_untransformed= _pointList.getPosAt(i);
            const Vec2d &p1_untransformed= _pointList.getPosAt(i+1);

            Vec3d p0_3d, p1_3d;
            p0_3d = _frieze->transformPos(Vec3d(p0_untransformed.m_x, p0_untransformed.m_y, z));
            p1_3d = _frieze->transformPos(Vec3d(p1_untransformed.m_x, p1_untransformed.m_y, z));

            const PolyLineEdge &edge = _pointList.getEdgeAt(i);
            if (edge.m_holeMode & PolyLineEdge::Hole_Visual)
            {
                continue;
            }
            //
            f32 dist = PointSignedDistToSegment(pos, (Vec2d&)p0_3d, (Vec2d&)p1_3d);
            dist = f32_Abs(dist); //it's needed
            if (dist<_bestDist)
            {
                _bestDist = dist;
                segmentIndex = (i32)i;
            }

        }

        if (segmentIndex>=0)
        {

            const Vec2d &p0_untransformed = _pointList.getPosAt((u32)segmentIndex);
            const Vec2d &p1_untransformed = _pointList.getPosAt((u32)segmentIndex+1);

            Vec3d p0_3d, p1_3d;
            p0_3d = _frieze->transformPos(Vec3d(p0_untransformed.m_x, p0_untransformed.m_y, z));
            p1_3d = _frieze->transformPos(Vec3d(p1_untransformed.m_x, p1_untransformed.m_y, z));

            _edgeDirection = (const Vec2d&)p1_3d-(const Vec2d&)p0_3d;
            f32 edgeLen = _edgeDirection.norm();
            _edgeDirection.normalize();

            Vec3d edgeDirection3d(_edgeDirection.m_x, _edgeDirection.m_y, 0);

            _posWithProjection0 = -(((Vec2d&)p0_3d).dot(_edgeDirection))*edgeDirection3d+p0_3d;

            ITF_ASSERT(fabsf(((Vec2d&)_posWithProjection0).dot(_edgeDirection))<0.1f);

            f32 distOnEdge = (pos-(const Vec2d&)p0_3d).dot(_edgeDirection);
            if (edgeLen != 0)
            {
                _percentInSegment = distOnEdge/edgeLen;
            }
            else
            {
                _percentInSegment = 0;
            }


        }

        return segmentIndex;
    }

    void Ray_StringWaveGeneratorComponent::invalidateRelatedFrieze()
    {

        Frise *frieze = IRTTIObject::DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));
        if (frieze)
        {
            EventOnLinkedToWaveGenerator unlinkEvent(m_actor->getRef(), bfalse);
            unlinkEvent.setSender(m_actor->getRef());
            frieze->onEvent(&unlinkEvent);

        }
        m_relatedFriezeRef.invalidate();
    }


    void Ray_StringWaveGeneratorComponent::computeRelatedFrieze()
    {
        Frise *foundFrieze = NULL;

        World *world = m_actor->getWorld();
        if (!world)
        {
            invalidateRelatedFrieze();
            return;
        }

        Vec2d phantomAABBRadius = Vec2d(m_phantomRadius, m_phantomRadius);
        Vec2d worldInitialPos;
        Vec2d::Vec3dTo2d(m_actor->getWorldInitialPos(), worldInitialPos);

        AABB generatorAABB(worldInitialPos-phantomAABBRadius,worldInitialPos+phantomAABBRadius);

        f32 bestDistance = FLT_MAX;

        for (u32 sceneIndex=0; sceneIndex<world->getSceneCount(); sceneIndex++)
        {
            Scene *scene = world->getSceneAt(sceneIndex);
            if (scene)
            {
                //every actor which is active
                const PickableList &l = scene->getFrises();

                for (u32 i=0; i<l.size(); i++)
                {
                    Frise *frise = (Frise*)l[i];
                    ITF_ASSERT_CRASH(frise!=NULL, "incorrect scene");

                    FriseConfig *config = frise->getConfig();
                    if (!config)
                    {
                        continue;
                    }
                    if (config->m_methode == Frise::InString)
                    {
                        const AABB &aabb = frise->getAABB();
                        if (aabb.isValid() && aabb.checkOverlap(generatorAABB))
                        {
                            f32 dummy;
                            Vec2d curNormalizedEdgeDir;
                            f32 distance = FLT_MAX;
                            Vec3d pointWithProj0;
                            if (projectOnPointList(frise, worldInitialPos, frise->getPointsData(), dummy, curNormalizedEdgeDir, distance,pointWithProj0)>=0)
                            {
                                if (distance<bestDistance)
                                {
                                    bestDistance = distance;
                                    m_normalizedEdgeDir = curNormalizedEdgeDir;
                                    m_relatedFriezeRef = frise->getRef();
                                    m_pointWithProjection0=pointWithProj0;
                                    foundFrieze = frise;
                                }
                            }

                            ITF_WARNING(frise, !frise->isLooping(), "string frieze should not loop");
                        }
                    }
                }
            }
        }

        if (foundFrieze)
        {
            EventOnLinkedToWaveGenerator linked(m_actor->getRef(), btrue);
            linked.setSender(m_actor->getRef());
            foundFrieze->onEvent(&linked);
        }
        else
        {
            invalidateRelatedFrieze();
        }
    }

    void Ray_StringWaveGeneratorComponent::processTrigger( EventTrigger* _eventTrigger )
    {
        if (_eventTrigger->getToggle())
        {
            m_activated = !m_activated;        
        }
        else
        {
            m_activated = _eventTrigger->getActivated();
        }
    }

    void Ray_StringWaveGeneratorComponent::processSpawnWave( Ray_EventSpawnWave* _eventSpawnWave )
    {
        if (m_activated && getTemplate()->getIsSpawner())
        {
            createNewWave((WaveType_t)_eventSpawnWave->getWaveType(), 
                _eventSpawnWave->getSpeed(),
                _eventSpawnWave->getDelayBeforeMoving(),
                _eventSpawnWave->getMustOffsetByRadius());
        }
    }

    bbool Ray_StringWaveGeneratorComponent::getPositionFromLinear(f32 _linear, Vec3d &_position)
    {
        if (m_relatedFriezeRef.isValid())
        {
            Vec3d dir3d(m_normalizedEdgeDir.m_x, m_normalizedEdgeDir.m_y, 0);
            _position = m_pointWithProjection0+dir3d*_linear;
            return btrue;
        }
        else
        {
            return bfalse;
        }
    }

    void Ray_StringWaveGeneratorComponent::setShaperDynamicParameters(u32 _index, f32 _dt)
    {
        FluidShaperDesc &shaperDesc = m_fluidShapers[_index];
        FluidShaper *shaper = shaperDesc.m_shaper;
        shaper->setProjectionDir(m_normalizedEdgeDir);
        f32 minX, maxX;
        Frise *frieze = IRTTIObject::SafeDynamicCast<Frise>(m_relatedFriezeRef.getObject(),ITF_GET_STRINGID_CRC(Frise,2579114664));
        Vec2d center;
        bbool isFirst = _index==0;
        bbool mustSendExciterMessage = bfalse;
        bbool isPreparing_ExicterMessage = bfalse;
        f32 pulsingScale = 1;

        if (frieze)
        {
            getFriezeProjectionOnEdge(frieze, minX, maxX, center);
        }
        else
        {
            minX = 0;
            maxX = 0;
            center.set(0,0);
        }
        shaperDesc.m_minX = minX;
        shaperDesc.m_maxX = maxX;
        if (getTemplate()->getIsFullLength())
        {
            //ITF_WARNING(m_actor, isOk, "no valid frieze found for full length effect");
            shaper->setPosition(center);
            shaper->setRadius((maxX-minX)*0.5f);
            mustSendExciterMessage = btrue;

        }
        else
        {
            if (getTemplate()->getLimitWaveToBorders())
            {
                if (shaperDesc.m_waveType==Wave_Spikes)
                {
                    Ray_FluidShaper_Spikes *spikes = (Ray_FluidShaper_Spikes*)shaper;

                    f32 radius = spikes->getRadius();
                    shaper->setAnnouncerRange(minX-radius, maxX+radius);
                }
            }
            shaper->setPosition(m_actor->get2DPos());
            f32 previousOffset = shaperDesc.m_offset;
            if (shaperDesc.m_timeLeftBeforeMoving>0)
            {
                shaperDesc.m_timeLeftBeforeMoving -= _dt;
                pulsingScale = getTemplate()->getPulseScaleWhenWaiting()*(1+ f32_Sin(shaperDesc.m_timeLeftBeforeMoving*getTemplate()->getPulseFreqWhenWaiting()));
            }
            else
            {
                shaperDesc.m_offset += shaperDesc.m_speed*_dt;
            }

            f32 linearOffset = shaper->getLinearOffset()+shaperDesc.m_offset;
            shaper->setLinearOffset(linearOffset);
            shaper->setRadius(m_phantomRadius*m_actor->getScale().m_x);

            if (isFirst)
            {
                mustSendExciterMessage = decideIfMustSendExciterMessageToWaveBall(frieze, shaperDesc, previousOffset, isPreparing_ExicterMessage);
            }


        }


        updateWaveFX(_index, bfalse);
        
        f32 actorYScale = 1;
        if (shaperDesc.m_waveType != Wave_Spikes)
        {
            actorYScale = m_actor->getScale().m_y;
        }
        shaper->setYScale(getTemplate()->getYScale()*actorYScale*m_scaleBecauseOfTransition*pulsingScale);
        bbool isInTransition_BlackZone = m_scaleBecauseOfTransition<(getTemplate()->getGrowthTransition_TimeStartsRed()/getTemplate()->getGrowthTransitionDuration());
        shaper->setIsColored((actorYScale>=getTemplate()->getActorYScaleThreshold()) && !isInTransition_BlackZone);
        shaper->setHasGameMaterial(m_scaleBecauseOfTransition>=1);

        if (isFirst)
        {            
            sendWaveGeneratorNotification(shaperDesc, mustSendExciterMessage, isPreparing_ExicterMessage);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
     
        for (u32 i=0; i<m_fluidShapers.size(); i++)
        {
            FluidShaperDesc &shaperDesc = m_fluidShapers[i];

            stopWaveFX(&shaperDesc);
            stopPreparingWaveFX(&shaperDesc);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::updateWaveFX(u32 _index, bbool _forcedDisable)
    {
        FluidShaperDesc &shaperDesc = m_fluidShapers[_index];
        FluidShaper *shaper = shaperDesc.m_shaper;

        if (m_fxControllerComponent)
        {
            f32 totalRadiusWithAnnounce = shaper->getRadius()+shaper->getTransitionLen();
            f32 linearOffset = shaper->getLinearOffset();
            bbool isVisible = ((linearOffset-totalRadiusWithAnnounce)<=shaperDesc.m_maxX)
                &&
                ((linearOffset+totalRadiusWithAnnounce)>=shaperDesc.m_minX);
            isVisible = isVisible && !_forcedDisable;

            if (isVisible && m_state==State_On)
            {

                bbool isPreparing = shaperDesc.m_timeLeftBeforeMoving>0;
                if (isPreparing)
                {
                    stopWaveFX(&shaperDesc);
                    if (shaperDesc.m_preparingWaveFXHandle==U32_INVALID)
                    {
                        shaperDesc.m_preparingWaveFXHandle = m_fxControllerComponent->playFX(getPreparingWaveFX(shaperDesc.m_waveType));
                    }
                }
                else
                {
                    stopPreparingWaveFX(&shaperDesc);
                    if (shaperDesc.m_waveFXHandle==U32_INVALID)
                    {
                        shaperDesc.m_waveFXHandle = m_fxControllerComponent->playFX(getWaveFX(shaperDesc.m_waveType));
                    }
                }
            }
            else
            {
                stopWaveFX(&shaperDesc);
                stopPreparingWaveFX(&shaperDesc);
            }
            //wave FX position
            Vec3d wavePosition;
            if (getPositionFromLinear(shaper->getLinearOffset(), wavePosition))
            {
                if (shaperDesc.m_waveFXHandle != U32_INVALID)
                {
                    m_fxControllerComponent->setFXPosFromHandle(shaperDesc.m_waveFXHandle, wavePosition);
                }
                if (shaperDesc.m_preparingWaveFXHandle != U32_INVALID)
                {
                    m_fxControllerComponent->setFXPosFromHandle(shaperDesc.m_preparingWaveFXHandle, wavePosition);
                }
            }
        }           
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    const StringID &Ray_StringWaveGeneratorComponent::getOffToOnFX(WaveType_t _type)
    {
        switch (_type)
        {
        case Wave_Spikes: return getTemplate()->getOffToOnFX_Spikes();
        case Wave_Bouncer: return getTemplate()->getOffToOnFX_Bouncer();
        //case Wave_Pusher: return getTemplate()->getOffToOnFX_Pusher();
        default : ITF_ASSERT(0);return getTemplate()->getOffToOnFX_Spikes();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    const StringID &Ray_StringWaveGeneratorComponent::getPreparingWaveFX(WaveType_t _type)
    {
        switch (_type)
        {
        case Wave_Spikes: return getTemplate()->getPreparingWaveFX_Spikes();
        case Wave_Bouncer: return getTemplate()->getPreparingWaveFX_Bouncer();
            //case Wave_Pusher: return getTemplate()->getPreparingWaveFX_Pusher();
        default : ITF_ASSERT(0);return getTemplate()->getPreparingWaveFX_Spikes();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    const StringID &Ray_StringWaveGeneratorComponent::getWaveFX(WaveType_t _type)
    {
        switch (_type)
        {
        case Wave_Spikes: return getTemplate()->getWaveFX_Spikes();
        case Wave_Bouncer: return getTemplate()->getWaveFX_Bouncer();
            //case Wave_Pusher: return getTemplate()->getWaveFX_Pusher();
        default : ITF_ASSERT(0);return getTemplate()->getWaveFX_Spikes();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::setState(State_t _newState)
    {
        m_state = _newState;
        m_timeInCurrentState = 0;

        stopOffToOnFXIfNeeded();

        switch(m_state)
        {
        case State_OffToOn:
            if (m_fxControllerComponent)
            {
                //for non-spawners, select on to off effect from type
                if (!getTemplate()->getIsSpawner())
                {
                    if (m_fluidShapers.size())
                    {
                        WaveType_t waveType = m_fluidShapers[0].m_waveType;
                        Vec3d position;
                        if (getPositionFromLinear(m_fluidShapers[0].m_shaper->getLinearOffset(), position))
                        {
                            m_offToOnFX = m_fxControllerComponent->playFX(getOffToOnFX(waveType));
                            if (m_offToOnFX!=U32_INVALID)
                            {
                                m_fxControllerComponent->setFXPosFromHandle(m_offToOnFX, position);
                            }
                        }
                    }
                }
                
            }
            break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_StringWaveGeneratorComponent::getLayerIndexFromType(WaveType_t _type)
    {
        switch(_type)
        {
        case Wave_Bouncer : return (u32) Layer_Bouncer;
        case Wave_Spikes : return (u32) Layer_Spikes;
        case Wave_Pusher : return (u32) Layer_Pusher;
        default: ITF_ASSERT_MSG(bfalse, "unknown type"); return 0;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////

    void Ray_StringWaveGeneratorComponent::applyStringWave(u32 index, Frise *_frieze)
    {
        FluidShaperDesc &shaperDesc = m_fluidShapers[index];

        setShaperDynamicParameters(index, LOGICDT);

        u32 layerIndex = getLayerIndexFromType(shaperDesc.m_waveType);

        if (shaperDesc.m_waveType == Wave_Spikes)
        {
            Ray_FluidShaper_Spikes *spike = (Ray_FluidShaper_Spikes*)shaperDesc.m_shaper;
            spike->addTimeSlice(LOGICDT);
            spike->setTransitionLen(getTemplate()->getSpikeMarginLength());

            //Spiky waves oscillate each frame
            if (_frieze)
            {
                FriseConfig *config = _frieze->getConfig();
                f32 multiplier;
                if (config && config->m_fluidSideCount==1)
                {
                    switch(CURRENTFRAME%3)
                    {
                    case 0: multiplier = -1; break;
                    case 1: multiplier = 0.1f; break;
                    case 2: multiplier = 1.f; break;
                    default: multiplier = 1.f;
                    }
                }
                else
                {
                    multiplier = 1;
                }
                _frieze->setFluidLayerCollisionHeightMultiplier(layerIndex, multiplier);
            }
        }
        if (_frieze)
        {
            registerWaveApplication(_frieze, shaperDesc);
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Register shaper in list
    void Ray_StringWaveGeneratorComponent::registerWaveApplication(Frise *_frieze, FluidShaperDesc &_shaperDesc)
    {
        ITF_ASSERT(_frieze!=NULL);
        FluidShaperApplication *newPending = NULL;            
        ObjectRef friezeRef(_frieze->getRef());

        u32 layerIndex = getLayerIndexFromType(_shaperDesc.m_waveType);
        newPending = findWaveApplication(_shaperDesc.m_shaper, friezeRef, layerIndex);
        if (!newPending)
        {
            newPending = &m_registeredShaperApplications.incrSize(1);
            newPending->m_friezeRef = _frieze->getRef();
            newPending->m_layerIndex = layerIndex;
            newPending->m_shaper = _shaperDesc.m_shaper;
            ITF_ASSERT(m_registeredShaperApplications.size()<=getTemplate()->getSpawneeLimit()*2);

        }
        _frieze->registerFluidShaper(layerIndex,newPending->m_shaper);
        newPending->m_lastFrame = CURRENTFRAME;

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Remove waves which were not updated this frame
    /// \param _removeAll : If set, unregister all registered shapers.
    void Ray_StringWaveGeneratorComponent::removeObsoleteRegisteredWaves(bbool _removeAll)
    {
        for (i32 i=0; i<(i32)m_registeredShaperApplications.size(); i++)
        {
            FluidShaperApplication &app = m_registeredShaperApplications[i];
            if (_removeAll || app.m_lastFrame!=CURRENTFRAME)
            {
                Frise *frise = IRTTIObject::SafeDynamicCast<Frise>(app.m_friezeRef.getObject(),ITF_GET_STRINGID_CRC(Frise,2579114664));
                if (frise)
                {
                    frise->unregisterFluidShaper(app.m_layerIndex, app.m_shaper);
                }
                m_registeredShaperApplications.eraseKeepOrder(i);
                i--;
            }

        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// find shaper application in m_registeredShaperApplications
    Ray_StringWaveGeneratorComponent::FluidShaperApplication *Ray_StringWaveGeneratorComponent::findWaveApplication(FluidShaper *_shaper, const ObjectRef &_friezeRef, i32 _layerIndex)
    {
        for (u32 i=0; i<m_registeredShaperApplications.size(); i++)
        {
            FluidShaperApplication &app = m_registeredShaperApplications[i];
            if (app.m_friezeRef == _friezeRef && 
                app.m_layerIndex == _layerIndex &&
                app.m_shaper == _shaper)
            {
                return &app;
            }
        }
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveGeneratorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        m_timeInCurrentState += _dt;

        // activation

        bbool makePulse = bfalse;

        if ( m_activated )
        {
            if (getTemplate()->getSyncWithMetronome())
            {
                f32 percent = METRONOME_MANAGER->getBarPercentage(getTemplate()->getSyncOffset(), getTemplate()->getSyncRatio());
                if (percent<=getTemplate()->getSyncActivePercent())
                {
                    makePulse = btrue;
                }
            }
            else
            {
                makePulse = btrue;
            }
        }
        //updating state
        m_scaleBecauseOfTransition = 1;
        bbool applyPulse = makePulse || !getTemplate()->getOnOffAffectsAllWaves();
        switch(m_state)
        {
        case State_Off:
            {
                m_scaleBecauseOfTransition = 0;
                if (applyPulse)
                {
                    setState(State_OffToOn);
                }
            } break;
        case State_OffToOn:
            {
                f32 transitionDuration = getTemplate()->getGrowthTransitionDuration();
                f32 blackModeDuration = getTemplate()->getGrowthTransition_TimeStartsRed();
                f32 percentInBlackMode = m_timeInCurrentState/blackModeDuration;
                if (percentInBlackMode<=1)
                {
                    m_scaleBecauseOfTransition = getTemplate()->getSmallScaleWhenBlackInTransition();
                }
                else
                {
                    f32 redTransitionDuration = transitionDuration-blackModeDuration;
                    f32 percentInRed = (m_timeInCurrentState-blackModeDuration)/redTransitionDuration;

                    m_scaleBecauseOfTransition = (1-percentInRed)*getTemplate()->getSmallScaleWhenBlackInTransition()+percentInRed*1.f;
                }
                if (m_scaleBecauseOfTransition>1)
                    m_scaleBecauseOfTransition = 1;

                if (!applyPulse)
                {
                    setState(State_Off);
                }
                else if (m_timeInCurrentState>=transitionDuration)
                {
                    setState(State_On);
                }


            } break;
        case State_On:
            {
                m_scaleBecauseOfTransition = 1;
                if (!applyPulse)
                {
                    setState(State_Off);
                }
            } break;
        }
        if (!m_relatedFriezeRef.isValid() && m_needsFrieze)
        {
            computeRelatedFrieze();
            ITF_WARNING(m_actor, m_relatedFriezeRef.isValid(), "Wave generator does not lie on a String frieze");
        }

        //Making pulse
        if (applyPulse )
        {
            Frise *frieze = IRTTIObject::SafeDynamicCast<Frise>(m_relatedFriezeRef.getObject(),ITF_GET_STRINGID_CRC(Frise,2579114664));
            for (u32 waveIndex=0; waveIndex<m_fluidShapers.size(); waveIndex++)
            {
                applyStringWave(waveIndex,frieze);
            }
        }
        else
        {
            Ray_EventWaveGeneratorNotification notification;
            notification.setSender(m_actor->getRef());
            notification.setup(Wave_Spikes, //dummy
                m_scaleBecauseOfTransition, 
                bfalse,
                bfalse
                );
            m_actor->onEvent(&notification);       

            for (u32 waveIndex=0; waveIndex<m_fluidShapers.size(); waveIndex++)
            {
                updateWaveFX(waveIndex, btrue);
            }
        }
        removeObsoleteRegisteredWaves();
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_StringWaveGeneratorComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::text(m_actor->getPos()+Vec3d(0,0,0), Color::white(), "wave generator");
    }

    void Ray_StringWaveGeneratorComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        computeRelatedFrieze();
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_StringWaveGeneratorComponent_Template)
        BEGIN_SERIALIZATION(Ray_StringWaveGeneratorComponent_Template)
        SERIALIZE_MEMBER("sync", m_syncWithMetronome);
        SERIALIZE_MEMBER("syncOffset", m_syncOffset);
        SERIALIZE_MEMBER("syncRatio", m_syncRatio);
        SERIALIZE_MEMBER("isSpike", m_isSpike);
        SERIALIZE_MEMBER("yScale", m_yScale);
        SERIALIZE_MEMBER("syncActivePercent", m_syncActivePercent);
        SERIALIZE_MEMBER("actorYScaleThreshold", m_actorYScaleThreshold);
        SERIALIZE_MEMBER("isFullLength", m_isFullLength);
        SERIALIZE_MEMBER("growthTransitionDuration", m_growthTransitionDuration);
        SERIALIZE_MEMBER("growthTransition_TimeStartsRed", m_growthTransition_TimeStartsRed);
        SERIALIZE_MEMBER("spikeMarginLength", m_spikeMarginLength);
        SERIALIZE_MEMBER("isSpawner", m_isSpawner);
        SERIALIZE_MEMBER("spawneeLimit", m_spawneeLimit);
        SERIALIZE_MEMBER("onOffAffectsAllWaves", m_onOffAffectsAllWaves);
        SERIALIZE_MEMBER("smallScaleWhenBlackInTransition", m_smallScaleWhenBlackInTransition);
        SERIALIZE_MEMBER("limitWaveToBorders", m_limitWaveToBorders);
        SERIALIZE_MEMBER("coloredSpawningCueWidth", m_coloredSpawningCueWidth);
        SERIALIZE_MEMBER("pulseScaleWhenWaiting", m_pulseScaleWhenWaiting);
        SERIALIZE_MEMBER("pulseFreqWhenWaiting", m_pulseFreqWhenWaiting);
        SERIALIZE_MEMBER("offToOnFX_Spikes", m_offToOnFX_Spikes);
        SERIALIZE_MEMBER("offToOnFX_Bouncer", m_offToOnFX_Bouncer);
        SERIALIZE_MEMBER("waveFX_Spikes", m_waveFX_Spikes);
        SERIALIZE_MEMBER("waveFX_Bouncer", m_waveFX_Bouncer);
        SERIALIZE_MEMBER("preparingWaveFX_Spikes", m_preparingWaveFX_Spikes);
        SERIALIZE_MEMBER("preparingWaveFX_Bouncer", m_preparingWaveFX_Bouncer);
    END_SERIALIZATION()

        Ray_StringWaveGeneratorComponent_Template::Ray_StringWaveGeneratorComponent_Template()
        : m_syncWithMetronome(bfalse)
        , m_syncOffset(0.f)
        , m_isSpike(bfalse)
        , m_yScale(1.f)
        , m_syncRatio(1.f)
        , m_syncActivePercent(0.1f)
        , m_actorYScaleThreshold(0.5f)
        , m_isFullLength(bfalse)
        , m_growthTransitionDuration(1.0f)
        , m_growthTransition_TimeStartsRed(0.8f)
        , m_spikeMarginLength(1.f)
        , m_isSpawner(bfalse)
        , m_spawneeLimit(8)
        , m_onOffAffectsAllWaves(btrue)
        , m_smallScaleWhenBlackInTransition(0.1f)
        , m_limitWaveToBorders(bfalse)
        , m_coloredSpawningCueWidth(0.3f)
        , m_pulseScaleWhenWaiting(2.f)
        , m_pulseFreqWhenWaiting(5.f)
        , m_offToOnFX_Spikes("Wave_DangerousStatic")
        , m_offToOnFX_Bouncer("Wave_DangerousStatic")
        , m_preparingWaveFX_Spikes("Wave_DangerousStatic")
        , m_preparingWaveFX_Bouncer("Wave_DangerousStatic")
        , m_waveFX_Spikes("Wave_Move")
        , m_waveFX_Bouncer("Wave_Move")
    {
    }


}
