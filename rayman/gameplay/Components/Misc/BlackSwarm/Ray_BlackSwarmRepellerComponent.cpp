#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_
#include "Rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmRepellerComponent.h"
#endif //_ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_

#ifndef _ITF_RAY_BLACKSWARMZONEMANAGER_H_
#include "Rayman/gameplay/Managers/Ray_BlackSwarmZoneManager.h"
#endif //_ITF_RAY_BLACKSWARMZONEMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

namespace ITF
{
	IMPLEMENT_OBJECT_RTTI(Ray_BlackSwarmRepellerAIComponent)

	BEGIN_SERIALIZATION_CHILD(Ray_BlackSwarmRepellerAIComponent)
        SERIALIZE_MEMBER("syncOffset", m_syncOffset_instance);
        SERIALIZE_MEMBER("radiusMax", m_maxRadius);
        SERIALIZE_MEMBER("radiusMin", m_minRadius);
        SERIALIZE_MEMBER("cycleDuration", m_cycleDuration);
	END_SERIALIZATION()

	BEGIN_VALIDATE_COMPONENT(Ray_BlackSwarmRepellerAIComponent)
        VALIDATE_COMPONENT_PARAM("textureBlendMode", getTemplate()->getTextureBlendMode() < GFX_BLEND_NUMBER, "Invalid blend mode for repeller texture");
	END_VALIDATE_COMPONENT()

    const f32 Ray_BlackSwarmRepellerAIComponent::MIN_TIME_BETWEEN_INTERACTIVE_SOUNDS = 0.25f;

	//------------------------------------------------------------------
	Ray_BlackSwarmRepellerAIComponent::Ray_BlackSwarmRepellerAIComponent()
	:	m_repellerState(RepellerState_UNDEFINED),
        m_cycleDuration(-1.0f),
		m_cycleTimer(0.0f),
        m_syncOffset_instance(F32_INFINITY),
        m_maxRadius(-1.0f),
        m_minRadius(-1.0f),
		m_radius(0.0f),
		m_radiusPulse(0.0f),
        m_alphaPulse(1.0f),
		m_radiusPulseState(RadiusPulseState_UNDEFINED),
		m_radiusPulseTimer(0.0f),
		m_registered(bfalse),
		m_repelCenter(Vec2d::Zero),
		m_finalRadius(0.0f),
        m_timeSinceLastInteractionSound(0.0f),
        m_animLightComponent(NULL),
        m_fxControllerComponent(NULL),
        m_soundLoopHandle(U32_INVALID),
        m_fxHandle(U32_INVALID),
		m_cycle(0)
	{
	}

	//------------------------------------------------------------------
	Ray_BlackSwarmRepellerAIComponent::~Ray_BlackSwarmRepellerAIComponent()
	{
		if( m_registered )
		{
			//unregister repeller to swarm zone manager
            Ray_BlackSwarmZoneManager * const manager = Ray_BlackSwarmZoneManager::getSingleton();
            if( manager != NULL )
            {
			    manager->unregisterSwarmRepeller( m_actor->getRef() );
            }
			m_registered = bfalse;
		}
		//free pulse texture resource
		if( m_pulseTextureId.isValidResourceId() )
		{
			m_actor->removeResource(m_pulseTextureId);
            m_pulseTextureId.invalidateResourceId();
        }
		//destroy pulse mesh
		destroyPulseMesh();
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		Super::onActorLoaded(_hotReload);

        //init max radius
        if( m_maxRadius < 0.0f )
        {
            m_maxRadius = getTemplate()->getSafeZoneRadiusMax();
        }
        if( m_minRadius < 0.0f )
        {
            m_minRadius = getTemplate()->getSafeZoneRadiusMin();
        }

        //init cycle duration
        if( m_cycleDuration < 0.0f )
        {
            m_cycleDuration = getTemplate()->getCycleDuration();
        }

		//get anim light component
		m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();

        //get fx controller component
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

		//get pulse texture resource id
		if( !getTemplate()->getPulseTexturePath().isEmpty() )
		{
			m_pulseTextureId = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getPulseTexturePath());
		}

        //compute states data
        computeRepellerStateData();
		//register to stim event if repeller is interactive
		if( getTemplate()->isInteractive() )
		{
			ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),this);
		}
        //register to anim event
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);

		//create pulse mesh
		createPulseMesh();

		//register repeller to swarm zone manager
        Ray_BlackSwarmZoneManager * const manager = Ray_BlackSwarmZoneManager::getSingleton();
        ITF_ASSERT_MSG( manager != NULL, "Swarm zones manager is not yet created." );
        if( manager != NULL )
        {
            manager->registerSwarmRepeller( m_actor->getRef() );
		    m_registered = btrue;
        }

        //load sync offset instance from template only if current sync offset is not customized
        if( m_syncOffset_instance == F32_INFINITY )
        {
            m_syncOffset_instance = getTemplate()->getSyncOffset();
        }
	}

    //------------------------------------------------------------------
    void Ray_BlackSwarmRepellerAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        
		//init repeller state
		ChangeRepellerState( getTemplate()->getDefaultRepellerState() );

        //check if repeller is a classic repeller (not interactive and not periodic)
        if( !getTemplate()->isInteractive() && m_cycleDuration < 0.0f )
        {
            // play constant looping sound
            playLoopSound();
        }
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmRepellerAIComponent::computeRepellerStateData()
    {
        //clear previously computed data
        m_repellerStateData.clear();

        RepellerStateData data;
        
        //compute OFF state data
        if( getTemplate()->getStateRatioOFF() >= 0.0f )
        {
            data.m_startRatio = 0.0f;
            data.m_endRatio = getTemplate()->getStateRatioOFF();
            data.m_infinite = bfalse;
        }
        else
        {
            data.m_startRatio = 0.0f;
            data.m_endRatio = 0.0f;
            data.m_infinite = btrue;
        }
        m_repellerStateData.push_back( data );

        //compute OFF to ON state data
        if( getTemplate()->getStateRatioOFFtoON() >= 0.0f )
        {
            data.m_startRatio = m_repellerStateData[RepellerState_OFF].m_endRatio;
            data.m_endRatio = m_repellerStateData[RepellerState_OFF].m_endRatio + getTemplate()->getStateRatioOFFtoON();
            data.m_infinite = bfalse;
        }
        else
        {
            data.m_startRatio =  m_repellerStateData[RepellerState_OFF].m_endRatio;
            data.m_endRatio =  m_repellerStateData[RepellerState_OFF].m_endRatio;
            data.m_infinite = btrue;
        }
        m_repellerStateData.push_back( data );

        //compute ON state data
        if( getTemplate()->getStateRatioON() >= 0.0f )
        {
            data.m_startRatio = m_repellerStateData[RepellerState_OFF_TO_ON].m_endRatio;
            data.m_endRatio = m_repellerStateData[RepellerState_OFF_TO_ON].m_endRatio + getTemplate()->getStateRatioON();
            data.m_infinite = bfalse;
        }
        else
        {
            data.m_startRatio =  m_repellerStateData[RepellerState_OFF_TO_ON].m_endRatio;
            data.m_endRatio = m_repellerStateData[RepellerState_OFF_TO_ON].m_endRatio;
            data.m_infinite = btrue;
        }
        m_repellerStateData.push_back( data );

        //compute ON to OFF state data
        if( getTemplate()->getStateRatioONtoOFF() >= 0.0f )
        {
            data.m_startRatio = m_repellerStateData[RepellerState_ON].m_endRatio;
            data.m_endRatio = m_repellerStateData[RepellerState_ON].m_endRatio + getTemplate()->getStateRatioONtoOFF();
            data.m_infinite = bfalse;
        }
        else
        {
            data.m_startRatio =  m_repellerStateData[RepellerState_ON].m_endRatio;
            data.m_endRatio = m_repellerStateData[RepellerState_ON].m_endRatio;
            data.m_infinite = btrue;
        }
        m_repellerStateData.push_back( data );

        //verify states ratios consistency
        ITF_WARNING( m_actor, (m_repellerStateData[RepellerState_ON_TO_OFF].m_startRatio + getTemplate()->getStateRatioONtoOFF()) == 1.0f, "States ratio sum is not 1.0" );
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmRepellerAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        
        //stop sound
        stopFx(m_soundLoopHandle);
        if( m_fxControllerComponent != NULL )
        {
            m_fxControllerComponent->stopFX("Swarm_Interactive");
            m_fxControllerComponent->stopFX("Swarm_Periodic");
        }
        //stop fx
        stopFx(m_fxHandle);

    }

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::Update(f32 _dt)
	{
		Super::Update( _dt );

        if( !isFinished() )
        {
		    //update repeller state
		    UpdateRepellerState( _dt );
        }

        //update radius pulse
	    UpdateRadiusPulse( _dt );

	    //compute final repel radius
	    m_finalRadius = (m_radius + m_radiusPulse);

	    //compute final repel center
	    computeRepelCenter();

	    //update AABB
	    updateAABB();

		//compute vertex buffer for pulse mesh
		computePulseVertexBuffer();

        //update time since last interaction
        m_timeSinceLastInteractionSound += _dt;
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::ChangeRepellerState(const RepellerState _newState)
	{
		m_repellerState = _newState;

		switch(m_repellerState)
		{
		case RepellerState_OFF:
			{
				//update cycle count
				++m_cycle;
                //reset cycle timer
                m_cycleTimer = 0.0f;
                //reset radius
				m_radius = m_minRadius;
				//activate pulse only if radius is not null
				if( m_radius > 0.0f )
				{
					ChangeRadiusPulseState( RadiusPulseState_INCREASING );
				}
				else
				{
					ChangeRadiusPulseState( RadiusPulseState_NONE );
				}
                //stop fx
                stopFx(m_fxHandle);
			}
			break;
		case RepellerState_OFF_TO_ON:
			{
                //set gong animation
                if( m_animLightComponent != NULL )
                {
				    m_animLightComponent->setAnim("Gong");
                }
				//desactivate pulse
				ChangeRadiusPulseState( RadiusPulseState_NONE );
                //start fx
                playFx( getTemplate()->getParticleFxName() );
                //play periodic sound
                if( !getTemplate()->isInteractive() )
                {
                    playPeriodicSound();
                }
			}
			break;
		case RepellerState_ON:
			{
                m_radius = m_maxRadius;
				//activate pulse
				ChangeRadiusPulseState( RadiusPulseState_INCREASING );
                //start fx
                playFx( getTemplate()->getParticleFxName() );
			}
			break;
		case RepellerState_ON_TO_OFF:
			{
				//desactivate pulse
				ChangeRadiusPulseState( RadiusPulseState_NONE );
                //stop fx
                stopFx(m_fxHandle);
			}
			break;
		default:
			break;
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::UpdateRepellerState(const f32 _dt)
	{
		//update cycle timer
		m_cycleTimer += _dt;
        //compute current state ratio (0.0f start state, 1.0f end state)
        const f32 currentRatio = getCurrentRepellerStateTimeRatio();

		//update state
		switch( m_repellerState )
		{
		case RepellerState_OFF:
			{
                if( currentRatio >= 1.0f )
                {
					ChangeRepellerState( RepellerState_OFF_TO_ON );
				}
			}
			break;
		case RepellerState_OFF_TO_ON:
			{
                if( currentRatio < 1.0f )
                {
				    m_radius = m_minRadius + ((m_maxRadius - m_minRadius) * currentRatio);
                }
                else
                {
					ChangeRepellerState( RepellerState_ON );
				}
			}
			break;
		case RepellerState_ON:
			{
				if( currentRatio >= 1.0f )
                {
					ChangeRepellerState( RepellerState_ON_TO_OFF );
				}
			}
			break;
		case RepellerState_ON_TO_OFF:
			{
                if( currentRatio < 1.0f )
                {
				    m_radius = m_maxRadius - ((m_maxRadius - m_minRadius) * currentRatio);
                }
                else
                {
					ChangeRepellerState( RepellerState_OFF );
				}
			}
			break;
		default:
			break;
		}
	}

    //------------------------------------------------------------------
    f32 Ray_BlackSwarmRepellerAIComponent::getCurrentRepellerStateTimeRatio() const
    {
        f32 ratio = 0.0f;

        if( m_cycleDuration > 0.0f )
        {
            if( !m_repellerStateData[m_repellerState].m_infinite )
            {
                //compute ratio based on metronome
                const f32 minRatio = m_repellerStateData[m_repellerState].m_startRatio;
                const f32 maxRatio = m_repellerStateData[m_repellerState].m_endRatio;
                //if state lasts more than 0 seconds
                if( (maxRatio - minRatio) > 0.0f )
                {
                    //get metronome current state
                    f32 globalRatio = getCurrentCycleRatio();
                    if( globalRatio >= minRatio )
                    {
                        //clamp ratio in current state values
                        globalRatio = Min( globalRatio, maxRatio );
                        ratio = (globalRatio - minRatio) / (maxRatio - minRatio);
                    }
                    //bar ratio < current state min ratio means that we're on a wrong state
                    else
                    {
                        ratio = 1.0f;
                    }
                }
                //if state lasts exactly 0.0 seconds
                else if( (maxRatio - minRatio) == 0.0f )
                {
                    ratio = 1.0f;
                }
            }
            //if state is infinite (return 0.0f)
        }
        return ratio;
    }

    //------------------------------------------------------------------
    f32 Ray_BlackSwarmRepellerAIComponent::getCurrentCycleRatio() const
    {
        f32 ratio = 0.0f;
        
        if( m_cycleDuration > 0.0f )
        {        
            if( getTemplate()->useSynchro() )
            {
                ratio = METRONOME_MANAGER->getBarPercentage(m_syncOffset_instance, 2.0f / m_cycleDuration, bfalse);
            }
            else
            {
                ratio = m_cycleTimer / m_cycleDuration;
            }
        }
        return ratio;
    }

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::ChangeRadiusPulseState(const RadiusPulseState _newState)
	{
		m_radiusPulseTimer = 0.0f;
		m_radiusPulseState = _newState;

		switch( _newState )
		{
		case RadiusPulseState_NONE:
			//pulse radius is null
			m_radiusPulse = 0.0f;
            m_alphaPulse = getTemplate()->getMinAlphaValue();
			break;
		case RadiusPulseState_INCREASING:
			//pulse radius starts as null
			m_radiusPulse = 0.0f;
            m_alphaPulse = getTemplate()->getMinAlphaValue();
			break;
		case RadiusPulseState_DECREASING:
			//pulse radius starts as max
			m_radiusPulse = getTemplate()->getSafeZonePulseRadius();
            m_alphaPulse = getTemplate()->getMaxAlphaValue();
			break;
		default:
			break;
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::UpdateRadiusPulse(const f32 _dt)
	{
		//update pulse state timer
		m_radiusPulseTimer += _dt;

		//get pulse transition time
		f32 radiusTransitionTime = getTemplate()->getSafeZonePulseTime();
		ITF_ASSERT_MSG( radiusTransitionTime > 0.0f, "Swarm safe zone null radius transition time." );

		//update current pulse state
		switch( m_radiusPulseState )
		{
		case RadiusPulseState_INCREASING:
			{
				//if state is not done
				if( m_radiusPulseTimer <= radiusTransitionTime )
				{
					//compute time ratio
					f32 timeRatio = Min( m_radiusPulseTimer / radiusTransitionTime, 1.0f );

					//compute new radius
					const f32 maxPulse = getTemplate()->getSafeZonePulseRadius();
					m_radiusPulse = maxPulse * timeRatio;

                    //compute new alpha
                    const f32 minAlpha = getTemplate()->getMinAlphaValue();
                    const f32 maxAlpha = getTemplate()->getMaxAlphaValue();
                    m_alphaPulse = minAlpha + ((maxAlpha - minAlpha) * timeRatio);
				}
				else
				{
					//switch to next state
					ChangeRadiusPulseState( RadiusPulseState_DECREASING );
				}
			}
			break;
		case RadiusPulseState_DECREASING:
			{
				//if state is not done
				if( m_radiusPulseTimer <= radiusTransitionTime )
				{
					//compute time ratio
					f32 timeRatio = Min( m_radiusPulseTimer / radiusTransitionTime, 1.0f );

					//compute new radius
					const f32 maxPulse = getTemplate()->getSafeZonePulseRadius();
					m_radiusPulse = maxPulse - (maxPulse * timeRatio);

                    //compute new alpha
                    const f32 minAlpha = getTemplate()->getMinAlphaValue();
                    const f32 maxAlpha = getTemplate()->getMaxAlphaValue();
                    m_alphaPulse = maxAlpha - ((maxAlpha - minAlpha) * timeRatio);
				}
				else
				{
					//switch to next state
					ChangeRadiusPulseState( RadiusPulseState_INCREASING );
				}
			}
			break;
		default:
			break;
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::computeRepelCenter()
	{
		//offset
		Vec2d offset = getTemplate()->getSafeZoneCenterOffset();
		//scaled offset
		offset *= m_actor->getScale().m_y;
		//rotated scaled offset
		offset = offset.Rotate( m_actor->getAngle() );
		//center = actor pos + rotated scaled offset
		m_repelCenter = m_actor->get2DPos() + offset;
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::createPulseMesh()
	{
		//if a previous mesh is created destroy it
		destroyPulseMesh();
		
#ifndef VBMANAGER_USE_DYNAMICRING_VB
		m_mesh.m_vtxBufferBuffered[0] = GFX_ADAPTER->createVertexBuffer(6 , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
	    ITF_ASSERT(m_mesh.m_vtxBufferBuffered[0]);
	    m_mesh.m_vtxBufferBuffered[1] = GFX_ADAPTER->createVertexBuffer(6 , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
	    ITF_ASSERT(m_mesh.m_vtxBufferBuffered[1]);
#else
    	m_mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(6 , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB


		//create mesh element
		if( m_mesh.m_ElementList.size() == 0 )
		{
			ITF_MeshElement elem;
			elem.m_startIndex = 0;
			elem.m_count = 0;
			m_mesh.m_ElementList.push_back(elem);
		}

		//create mesh element index buffer
		m_mesh.m_ElementList[0].m_indexBuffer = GFX_ADAPTER->createIndexBuffer(6, bfalse);

		//fill mesh element index buffer
		u16* indexBuffer;
		m_mesh.m_ElementList[0].m_indexBuffer->Lock((void**)&indexBuffer);
		for(u32 index = 0; index < 6; ++index)
		{
			//first triangle
			indexBuffer[0] = 0;
			indexBuffer[1] = 1;
			indexBuffer[2] = 2;
			//second triangle
			indexBuffer[3] = 2;
			indexBuffer[4] = 3;
			indexBuffer[5] = 0;
		}
		m_mesh.m_ElementList[0].m_indexBuffer->Unlock();

		//set up mesh element
		m_mesh.m_ElementList[0].m_count = 6;
		m_mesh.m_ElementList[0].m_material.m_textureDiffuse = m_pulseTextureId;
		m_mesh.m_ElementList[0].m_material.m_blendMode = getTemplate()->getTextureBlendMode();
		m_mesh.m_ElementList[0].m_material.m_TexAdressingModeU = GFXTADDRESS_MIRROR;
		m_mesh.m_ElementList[0].m_material.m_TexAdressingModeV = GFXTADDRESS_MIRROR;

		//set mesh color
		m_mesh.m_globalColor = Color::white();
		//set mesh matrix
		m_mesh.setMatrix((GMatrix44&)GMatrixIdentity);
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::destroyPulseMesh()
	{
		//free vertex buffers
#ifndef VBMANAGER_USE_DYNAMICRING_VB
		if( m_mesh.m_vtxBufferBuffered[0] != NULL )
		{
			GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBufferBuffered[0]);
			m_mesh.m_vtxBufferBuffered[0] = NULL;
		}
		if( m_mesh.m_vtxBufferBuffered[1] != NULL )
		{
			GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBufferBuffered[1]);
			m_mesh.m_vtxBufferBuffered[1] = NULL;
		}
#else
		if( m_mesh.m_vtxBuffer != NULL )
		{
			GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBuffer);
	    	m_mesh.m_vtxBuffer = NULL;	
		}		
#endif //VBMANAGER_USE_DYNAMICRING_VB

		//free index buffer
		if( m_mesh.m_ElementList.size() > 0 )
		{
			if(m_mesh.m_ElementList[0].m_indexBuffer != NULL)
			{
				GFX_ADAPTER->removeIndexBuffer(m_mesh.m_ElementList[0].m_indexBuffer);
			}
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::computePulseVertexBuffer()
	{	
		//add magic factor to deal with visual/logix difference
		const f32 currentRadius = getRepelVisualRadius() * 1.25f;
		const Vec2d &repel2DPosition = getRepelCenter();
		const Vec3d center(repel2DPosition.m_x, repel2DPosition.m_y, m_actor->getPos().m_z);

		Color col = Color::white();
        col.m_a = m_alphaPulse;
		u32 u32Color = col.getAsU32();
		
		//compute each vertex data
		m_cacheVB[0].m_pos.set(center.m_x - currentRadius, center.m_y - currentRadius, center.m_z);
		m_cacheVB[0].m_color = u32Color;
		m_cacheVB[0].m_uv = Vec2d::Zero;

		m_cacheVB[1].m_pos.set(center.m_x + currentRadius, center.m_y - currentRadius, center.m_z);
		m_cacheVB[1].m_color = u32Color;
		m_cacheVB[1].m_uv = Vec2d::XAxis * 2.0f;

		m_cacheVB[2].m_pos.set(center.m_x + currentRadius, center.m_y + currentRadius, center.m_z);
		m_cacheVB[2].m_color = u32Color;
		m_cacheVB[2].m_uv = Vec2d::One * 2.0f;

		m_cacheVB[3].m_pos.set(center.m_x - currentRadius, center.m_y + currentRadius, center.m_z);
		m_cacheVB[3].m_color = u32Color;
		m_cacheVB[3].m_uv = Vec2d::YAxis * 2.0f;
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::Draw()
	{
		Super::Draw();

		if( m_pulseTextureId.isValidResourceId() )
		{
			Texture * tex = static_cast<Texture*>(m_pulseTextureId.getResource());
			if( tex != NULL )
			{
				//get common information
				if( getRepelVisualRadius() > 0.0f )
				{
					const f32 depth = m_actor->getDepth() - 0.001f;

					//switch mesh user buffer
#ifndef VBMANAGER_USE_DYNAMICRING_VB
					m_mesh.m_userBuffer = !m_mesh.m_userBuffer;
					u32 currentBuffer = m_mesh.m_userBuffer;
					//get vertex buffer ptr
					ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBufferBuffered[currentBuffer];
					
#else
					ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBuffer;
#endif //VBMANAGER_USE_DYNAMICRING_VB
					
					//lock vertex buffer data
					VertexPCT* data = NULL;
					GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer, (void**)&data );
					ITF_ASSERT_MSG(data, "Unable to lock a vertex buffer");
					if(data != NULL)
					{
						//write in vertex buffer data
						ITF_MemcpyWriteCombined(data, &m_cacheVB, sizeof(VertexPCT)*6);
						//unlock vertex buffer
						GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);
						//add primitive to be drawn
						GFX_ADAPTER->addPrimitiveInZList((void*)&m_mesh, GFX_ITF_MESH, NULL, depth, btrue,m_actor->getRef());
					}
				}
			}
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::updateAABB()
	{
		// update AABB
		const Vec2d halfSize(m_finalRadius, m_finalRadius);
		const Vec2d offset = getRepelCenter() - m_actor->get2DPos();

		AABB aabb(halfSize);
		aabb.grow(-halfSize);
		aabb.Translate(offset);
		m_actor->growRelativeAABB(aabb);
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::onEvent(Event * _event)
	{
		Super::onEvent(_event);

		//on punch
		if ( PunchStim* stim = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            //force repeller to switch to the right state
            forceRepellerState(RepellerState_OFF_TO_ON);
            //play interaction sound
            playInteractiveSound();
        }
        else if (AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            processAnimEvent(animEvent);
        }
	}

    //------------------------------------------------------------------
    void Ray_BlackSwarmRepellerAIComponent::processAnimEvent(const AnimGameplayEvent* const _animEvent)
    {
        if (_animEvent->getName() == "MRK_ToIdle")
        {
            m_animLightComponent->setAnim("Idle");
        }
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmRepellerAIComponent::forceRepellerState(const RepellerState _state)
    {
        //recompute cycle timer using the wanted state and current radius
        m_cycleTimer = computeCycleTimerFromRadius(_state, m_radius);
		//switch to wanted state
		ChangeRepellerState( _state );
    }

    //------------------------------------------------------------------
    //compute cycle timer using a wanted state and a radius
    f32 Ray_BlackSwarmRepellerAIComponent::computeCycleTimerFromRadius(const RepellerState _state, const f32 _radius) const
    {
        ITF_ASSERT_MSG( _state == RepellerState_OFF_TO_ON, "Function not yet implemented for another state." );

        const f32 diffRadius = m_maxRadius - m_minRadius;
        if( diffRadius > 0.0f )
        {
            const f32 radiusRatio = (m_radius - m_minRadius) / diffRadius;
            const f32 startRatio = m_repellerStateData[RepellerState_OFF_TO_ON].m_startRatio;
            const f32 endRatio = m_repellerStateData[RepellerState_OFF_TO_ON].m_endRatio;
            const f32 globalCycleRatio = startRatio + (endRatio - startRatio) * radiusRatio;
            return( globalCycleRatio * m_cycleDuration );
        }
        else
        {
            return 1.0f;
        }
    }

    //------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::playLoopSound()
	{
        if( m_fxControllerComponent != NULL )
        {
            if( m_soundLoopHandle == U32_INVALID )
            {
                m_soundLoopHandle = m_fxControllerComponent->playFX( "Swarm_Idle" );
                ITF_WARNING( m_actor, m_soundLoopHandle != U32_INVALID, "Swarm Idle sound cannot be played" );
            }
        }
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmRepellerAIComponent::stopFx(u32 & _handle) const
	{
        if( m_fxControllerComponent != NULL )
        {
            if( _handle != U32_INVALID)
            {
                if( m_fxControllerComponent->isPlaying(_handle) )
                {
                    m_fxControllerComponent->stopFX( _handle );
                }
                _handle = U32_INVALID;
            }
        }
    }

    //------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::playInteractiveSound()
	{
        if( m_fxControllerComponent != NULL && m_timeSinceLastInteractionSound > MIN_TIME_BETWEEN_INTERACTIVE_SOUNDS )
        {
            const StringID & interactiveSound = getTemplate()->getInteractiveSoundName();
            m_fxControllerComponent->stopFX(interactiveSound);
            m_fxControllerComponent->playFX(interactiveSound);
            m_timeSinceLastInteractionSound = 0.0f;
        }
    }

    //------------------------------------------------------------------
	void Ray_BlackSwarmRepellerAIComponent::playPeriodicSound()
	{
        if( m_fxControllerComponent != NULL )
        {
            const StringID & periodicSound = getTemplate()->getPeriodicSoundName();
            m_fxControllerComponent->stopFX(periodicSound);
            m_fxControllerComponent->playFX(periodicSound);
        }
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmRepellerAIComponent::playFx(const StringID & _name)
    {
        if( m_fxControllerComponent != NULL )
        {
            if( m_fxHandle == U32_INVALID )
            {
                //play fx
                m_fxHandle = m_fxControllerComponent->playFX( _name );
            }
        }
    }


	//*****************************************************************************
    // Template
    //*****************************************************************************

	IMPLEMENT_OBJECT_RTTI(Ray_BlackSwarmRepellerAIComponent_Template)

	BEGIN_SERIALIZATION(Ray_BlackSwarmRepellerAIComponent_Template)
		SERIALIZE_MEMBER("interactive",	m_interactive);
		SERIALIZE_MEMBER("safeZoneToleranceDistance", m_safeZoneToleranceDistance);
		SERIALIZE_MEMBER("safeZoneRadiusMin",	m_safeZoneRadiusMin);
		SERIALIZE_MEMBER("safeZoneRadiusMax",	m_safeZoneRadiusMax);
        SERIALIZE_MEMBER("syncOffset", m_syncOffset);
        SERIALIZE_MEMBER("defaultState", m_defaultState);
        SERIALIZE_MEMBER("cycleDuration", m_cycleDuration);
        SERIALIZE_MEMBER("maxAllowedCycles", m_maxAllowedCycles);
        SERIALIZE_MEMBER("stateRatioOFF", m_stateRatioOFF);
        SERIALIZE_MEMBER("stateRatioOFFtoON", m_stateRatioOFFtoON);
        SERIALIZE_MEMBER("stateRatioON", m_stateRatioON);
        SERIALIZE_MEMBER("stateRatioONtoOFF", m_stateRatioONtoOFF);
        SERIALIZE_MEMBER("useSynchro", m_useSynchro);
		SERIALIZE_MEMBER("safeZonePulseRadius",	m_safeZonePulseRadius);
		SERIALIZE_MEMBER("safeZonePulseTime",	m_safeZonePulseTime);
        SERIALIZE_MEMBER("minAlphaValue", m_minAlphaValue);
		SERIALIZE_MEMBER("maxAlphaValue", m_maxAlphaValue);
		SERIALIZE_MEMBER("safeZoneCenterOffset",	m_safeZoneCenterOffset);
        SERIALIZE_MEMBER("pulseTexturePath",m_pulseTexturePath);
        SERIALIZE_MEMBER("particleFxName", m_particleFxName);
        SERIALIZE_MEMBER("particleFXSizeRatio", m_particleFXSizeRatio);
        SERIALIZE_MEMBER("textureBlendMode", m_textureBlendMode);
        SERIALIZE_MEMBER("interactiveSoundName", m_interactiveSoundName);
        SERIALIZE_MEMBER("periodicSoundName", m_periodicSoundName);
    END_SERIALIZATION()

	//------------------------------------------------------------------
	Ray_BlackSwarmRepellerAIComponent_Template::Ray_BlackSwarmRepellerAIComponent_Template()
	:	m_interactive(bfalse),
		m_safeZoneToleranceDistance(0.2f),
		m_safeZoneRadiusMin(3.0f),
		m_safeZoneRadiusMax(9.0f),
        m_syncOffset(0.0f),
        m_defaultState(Ray_BlackSwarmRepellerAIComponent::RepellerState_OFF),
        m_cycleDuration(2.0f),
        m_maxAllowedCycles(-1),
        m_stateRatioOFF(0.25f),
        m_stateRatioOFFtoON(0.25f),
        m_stateRatioON(0.25f),
        m_stateRatioONtoOFF(0.25f),
        m_useSynchro(bfalse),
		m_safeZonePulseRadius(0.25f),
		m_safeZonePulseTime(1.0f),
        m_minAlphaValue(0.25f),
        m_maxAlphaValue(0.5f),
		m_safeZoneCenterOffset(Vec2d::Zero),
		m_pulseTexturePath(Path::EmptyPath),
        m_particleFxName("FXGong"),
        m_particleFXSizeRatio(0.33f),
        m_textureBlendMode(2), //default blend mode alpha
        m_interactiveSoundName("Swarm_Interactive"),
        m_periodicSoundName("Swarm_Periodic")
	{
	}

	//------------------------------------------------------------------
	Ray_BlackSwarmRepellerAIComponent_Template::~Ray_BlackSwarmRepellerAIComponent_Template()
	{
	}
}
