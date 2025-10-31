#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ABYSSLIGHTCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AbyssLightComponent.h"
#endif //_ITF_RAY_ABYSSLIGHTCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_INTERSECTION_H_
#include "engine/physics/Collision/Intersection.h"
#endif //_ITF_INTERSECTION_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AbyssLightComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_AbyssLightComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_AbyssLightComponent)
    END_VALIDATE_COMPONENT()

    //------------------------------------------------------------------
    //Static members
    //------------------------------------------------------------------
    SafeArray<Ray_AbyssLightComponent *> Ray_AbyssLightComponent::ms_instances;

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::registerInstance(Ray_AbyssLightComponent *const _instance)
    {
        //assert if instance is already registered
        ITF_ASSERT(ms_instances.find(_instance) == U32_INVALID);
        //add instance
        ms_instances.push_back(_instance);
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::unregisterInstance(Ray_AbyssLightComponent *const _instance)
    {
        //try to find instance in array
        u32 index = ms_instances.find(_instance);
        //assert if instance is not registered
        ITF_ASSERT( index != U32_INVALID );
        if( index != U32_INVALID )
        {
            //remove instance from array
            ms_instances.eraseNoOrder(index);
        }
    }

    //------------------------------------------------------------------
    bbool Ray_AbyssLightComponent::segmentIntersectWithLight(const Vec2d & _position, const Vec2d & _orientation, const f32 _length)
    {
        for(u32 index = 0; index < ms_instances.size(); ++index)
        {
            const Vec2d & lightPosition = ms_instances[index]->get2DPos();
            const f32 radius = ms_instances[index]->getRadius();
            Vec2d collision, normal;
            if( IntersectionMath::intersectCircleWithSegment(lightPosition, radius, _position, _orientation, _length, collision, normal) )
            {
                return btrue;
            }
        }
        return bfalse;
    }

    //------------------------------------------------------------------
    bbool Ray_AbyssLightComponent::segmentIntersectWithLight(const Vec2d & _segmentStart, const Vec2d & _segmentStop)
    {
        const Vec2d difference = (_segmentStop - _segmentStart);
        const f32 length = difference.norm();
        ITF_ASSERT(length > 0.0f);
        const Vec2d direction = difference / length;
        return segmentIntersectWithLight(_segmentStart, direction, length);
    }

    //------------------------------------------------------------------
    bbool Ray_AbyssLightComponent::AABBIntersectWithLight(const Vec2d & _pos, const AABB &_aabb)
    {
        const Vec2d aabbCenter = _pos + _aabb.getCenter();
        const f32 aabbHalfWidth = _aabb.getWidth() * 0.5f;
        const f32 aabbHalfHeight = _aabb.getHeight() * 0.5f;
        for(u32 index = 0; index < ms_instances.size(); ++index)
        {
            const Vec2d & lightPosition = ms_instances[index]->get2DPos();
            const f32 radius = ms_instances[index]->getRadius();
            const f32 distX = f32_Abs(aabbCenter.m_x - lightPosition.m_x);
            const f32 distY = f32_Abs(aabbCenter.m_y - lightPosition.m_y);
            const f32 maxDistX = aabbHalfWidth + radius;
            const f32 maxDistY = aabbHalfHeight + radius;
            if( distX < maxDistX && distY < maxDistY )
            {
                return btrue;
            }
        }
        return bfalse;
    }

    //------------------------------------------------------------------
    //Ctors, Dtors and inherited members
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    Ray_AbyssLightComponent::Ray_AbyssLightComponent()
    : m_radius(0.0f)
    , m_offset(Vec2d::Zero)
    , m_cycleTimer(0.0f)
    , m_cycleDuration(0.0f)
    , m_alpha(1.0f)
    , m_sizeVariation(0.0f)
    , m_positionVariation(Vec2d::Zero)
    , m_animLightComponent(NULL)
    , m_instanceRegistered(bfalse)
    , m_soundComponent(NULL)
    , m_soundHandle(ITF::SoundHandle::getInvalidHandle())
    , m_stopDelay( 0.f )
    , m_stopTimer( -1.f )
    , m_stopInitRadius( 0.f )
    , m_stopDestroyAtEnd( bfalse )
    {
    }

    //------------------------------------------------------------------
    Ray_AbyssLightComponent::~Ray_AbyssLightComponent()
    {
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //init radius
        setRadius( getTemplate()->getRadius() );

        //load texture, create mesh and init VB
        if( getTemplate()->useTexture() )
        {
            loadTexture();
            createMesh();
            computeVertexBuffer();
        }

        //get anim light component
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();
        m_soundComponent = m_actor->GetComponent<SoundComponent>();
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        
        if( getTemplate() && getTemplate()->useTexture() )
        {
            //free texture resource
		    freeTexture();
		    //destroy mesh
		    destroyMesh();
        }

        //checked here in case of uncalled onBecomeInactive
        if( m_instanceRegistered )
        {
            Ray_AbyssLightComponent::unregisterInstance(this);
            m_instanceRegistered = bfalse;
        }
        stopSound();
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        Ray_AbyssLightComponent::registerInstance(this);
        m_instanceRegistered = btrue;
        startSound();
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if( m_instanceRegistered )
        {
            Ray_AbyssLightComponent::unregisterInstance(this);
            m_instanceRegistered = bfalse;
        }
        stopSound();
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::Update(f32 _dt)
    {
        //update light component data from radius
        m_near = m_radius - 1.0f;
        m_far = m_radius;

        Super::Update(_dt);

        // update stop delay (radius)
        updateStop( _dt );
       
        //update size, alpha & position
        updateVariations(_dt);
        
        if( getTemplate()->useTexture() )
        {
            //compute vertex buffer
		    computeVertexBuffer();
        }

        //update sound input
        updateSound();
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::Draw()
    {
        Super::Draw();

        if( getTemplate()->useTexture() )
        {
            if( m_textureId.isValidResourceId() )
		    {
			    Texture * tex = static_cast<Texture*>(m_textureId.getResource());
			    if( tex != NULL )
			    {
				    //get common information
				    if( m_radius > 0.0f )
				    {
                        const f32 depth = getTemplate()->getTextureRenderPrio();

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
    }


    //------------------------------------------------------------------
    //Texture, mesh & VB management
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::loadTexture()
    {
		if( !getTemplate()->getTexturePath().isEmpty() )
		{
			m_textureId = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTexturePath());
		}
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::freeTexture()
    {
        if( m_textureId.isValidResourceId() )
		{
			m_actor->removeResource(m_textureId);
            m_textureId.invalidateResourceId();
        }
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::createMesh()
    {
		//if a previous mesh is created destroy it
		destroyMesh();
		
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
		m_mesh.m_ElementList[0].m_material.m_textureDiffuse = m_textureId;
		m_mesh.m_ElementList[0].m_material.m_blendMode = GFX_BLEND_ALPHA;
		m_mesh.m_ElementList[0].m_material.m_TexAdressingModeU = GFXTADDRESS_MIRROR;
		m_mesh.m_ElementList[0].m_material.m_TexAdressingModeV = GFXTADDRESS_MIRROR;

		//set mesh color
		m_mesh.m_globalColor = Color::white();
		//set mesh matrix
		m_mesh.setMatrix((GMatrix44&)GMatrixIdentity);
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::destroyMesh()
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
    void Ray_AbyssLightComponent::computeVertexBuffer()
    {	
		//add magic factor to deal with visual/logix difference
		Vec3d center = (get2DPos() + m_positionVariation).to3d();

		Color col = Color::white();
		u32 u32Color = col.getAsU32();

        const f32 textureHeight = (m_radius * getTemplate()->getTextureRadiusRatio()) + m_sizeVariation;
        const f32 textureWidth = (m_radius * getTemplate()->getTextureRadiusRatio()) - m_sizeVariation;
		
		//compute each vertex data
		m_cacheVB[0].m_pos.set(center.m_x - textureWidth, center.m_y - textureHeight, center.m_z);
		m_cacheVB[0].m_color = u32Color;
		m_cacheVB[0].m_uv = Vec2d::Zero;

		m_cacheVB[1].m_pos.set(center.m_x + textureWidth, center.m_y - textureHeight, center.m_z);
		m_cacheVB[1].m_color = u32Color;
		m_cacheVB[1].m_uv = Vec2d::XAxis * 2.0f;

		m_cacheVB[2].m_pos.set(center.m_x + textureWidth, center.m_y + textureHeight, center.m_z);
		m_cacheVB[2].m_color = u32Color;
		m_cacheVB[2].m_uv = Vec2d::One * 2.0f;

		m_cacheVB[3].m_pos.set(center.m_x - textureWidth, center.m_y + textureHeight, center.m_z);
		m_cacheVB[3].m_color = u32Color;
		m_cacheVB[3].m_uv = Vec2d::YAxis * 2.0f;

        m_mesh.m_globalColor.m_a = m_alpha;
    }

    //------------------------------------------------------------------
    //Position
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    Vec2d Ray_AbyssLightComponent::get2DPos() const
    {
        if( m_animLightComponent )
        {
            const StringID & boneName = getTemplate()->getBoneName();
            if( boneName.isValid() )
            {
                const u32 boneIndex = m_animLightComponent->getBoneIndex(boneName);
                if( boneIndex != U32_INVALID )
                {
                    Vec2d bonePosition;
                    m_animLightComponent->getBonePos(boneIndex, bonePosition,btrue);
                    return bonePosition + m_offset;
                }
            }
        }

        return m_actor->get2DPos() + m_offset;
    }

    //------------------------------------------------------------------
    // Alpha management
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::updateVariations(const f32 _dt)
    {
        if( m_cycleTimer == 0.0f )
        {
            m_cycleDuration = getTemplate()->getVariationMinTime() + Seeder::getSharedSeeder().GetFloat(getTemplate()->getVariationMaxTime() - getTemplate()->getVariationMinTime());
        }

        ITF_ASSERT( m_cycleDuration > 0.0f );
        ITF_ASSERT( m_cycleTimer <= m_cycleDuration );
        const f32 ratio = m_cycleTimer / m_cycleDuration;


        const f32 angle = MTH_2PI * ratio;
        f32 sin, cos;
        f32_CosSinOpt(angle, &cos, &sin);
        const f32 alphaVariablePart = getTemplate()->getAlphaVariation();
        m_alpha = (1.0f - alphaVariablePart) + (alphaVariablePart * sin);
        m_sizeVariation = sin * getTemplate()->getSizeVariation();
        m_positionVariation = Vec2d(cos, sin) * getTemplate()->getPositionVariation();

        m_cycleTimer += _dt;
        if( m_cycleTimer > m_cycleDuration )
        {
            m_cycleTimer = 0.0f;
        }
    }

    //------------------------------------------------------------------
    // Sound management
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::startSound()
    {
        if( m_soundComponent )
        {
            if( m_soundHandle == ITF::SoundHandle::getInvalidHandle())
            {
                const StringID & soundName = getTemplate()->getSoundName();
                if( soundName.isValid() )
                {
                    m_soundHandle = m_soundComponent->playSound(soundName);
                }
            }
        }
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::stopSound()
    {
        if( m_soundComponent )
        {
            if(!( m_soundHandle == ITF::SoundHandle::getInvalidHandle()))
            {
                m_soundComponent->stopSound(m_soundHandle);
                m_soundHandle = ITF::SoundHandle::getInvalidHandle();
            }
        }
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::updateSound()
    {
        if( m_soundComponent )
        {
            if( !(m_soundHandle == ITF::SoundHandle::getInvalidHandle()))
            {
                const f32 maxRadius = getTemplate()->getRadius();
                const f32 ratio = f32_Min(1.0f, m_radius / maxRadius);
                m_soundComponent->setInput("Light_Volume", ratio);
            }
        }
    }

    //------------------------------------------------------------------
    void Ray_AbyssLightComponent::updateStop( f32 _dt )
    {
        if ( m_stopTimer >= 0.0f )
        {
            m_stopTimer = f32_Max( 0.0f, m_stopTimer - _dt );
            f32 factor = m_stopTimer / m_stopDelay;

            setRadius( m_stopInitRadius * factor );

            if ( factor == 0.0f )
            {
                m_stopTimer = -1.f;
                if ( m_stopDestroyAtEnd )
                {
                    if ( m_actor->isSerializable() )
                    {
                        m_actor->disable();
                    }
                    else
                    {
                        m_actor->requestDestruction();
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------
    //Template
    //------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_AbyssLightComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AbyssLightComponent_Template)
        SERIALIZE_MEMBER("radius", m_radius);
        SERIALIZE_MEMBER("useTexture", m_useTexture);
        SERIALIZE_MEMBER("texturePath", m_texturePath);
        SERIALIZE_MEMBER("textureRenderPrio", m_textureRenderPrio);
        SERIALIZE_MEMBER("textureRadiusRatio", m_textureRadiusRatio);
        SERIALIZE_MEMBER("boneName", m_boneName);
        SERIALIZE_MEMBER("variationMinTime", m_variationMinTime);
        SERIALIZE_MEMBER("variationMaxTime", m_variationMaxTime);
        SERIALIZE_MEMBER("alphaVariation", m_alphaVariation);
        SERIALIZE_MEMBER("sizeVariation", m_sizeVariation);
        SERIALIZE_MEMBER("positionVariation",m_positionVariation);
        SERIALIZE_MEMBER("soundName", m_soundName);
    END_SERIALIZATION()

	//------------------------------------------------------------------
	Ray_AbyssLightComponent_Template::Ray_AbyssLightComponent_Template()
    : m_radius(2.0f)
    , m_useTexture(btrue)
    , m_texturePath(Path::EmptyPath)
    , m_textureRenderPrio(0.0f)
    , m_textureRadiusRatio(0.5f)
    , m_boneName(StringID::Invalid)
    , m_variationMinTime(1.0f)
    , m_variationMaxTime(2.0f)
    , m_alphaVariation(0.15f)
    , m_sizeVariation(0.1f)
    , m_positionVariation(0.2f)
    , m_soundName(StringID::Invalid)
	{
	}

	//------------------------------------------------------------------
	Ray_AbyssLightComponent_Template::~Ray_AbyssLightComponent_Template()
	{
	}
}
