#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CRYSTALKINGAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_CrystalKingAIComponent.h"
#endif //_ITF_RAY_CRYSTALKINGAICOMPONENT_H_

#ifndef _ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceVisualComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#define  STATE_CRYSTAL_HIDE         0
#define  STATE_CRYSTAL_OPENING      1
#define  STATE_CRYSTAL_OPEN         2
#define  STATE_CRYSTAL_OPEN_LIGHT   3

#define COUNT_QUAD                  1

    namespace ITF
    {
    
    IMPLEMENT_OBJECT_RTTI(Ray_CrystalKingAIComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_CrystalKingAIComponent)
       //BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
       //     SERIALIZE_MEMBER("flagWorld", m_flagWorld);
       // END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_CrystalKingAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "This component requires an LinkComponent.");
    END_VALIDATE_COMPONENT()

    // Anims Crystal
    #define animStandClosed ITF_GET_STRINGID_CRC(StandClosed,2906609031)
    #define animOpening ITF_GET_STRINGID_CRC(Opening,133555280)
    #define animStandOpen ITF_GET_STRINGID_CRC(StandOpen,2278229236)
    #define animActivation ITF_GET_STRINGID_CRC(Activation,2155859560)

    // Anims Faery
    #define animFaeryStand ITF_GET_STRINGID_CRC(Stand,2617849515)
    #define animFaeryMeditation ITF_GET_STRINGID_CRC(Meditation,1098147100)

    // Anims SlotDn
    #define animSlotClosed ITF_GET_STRINGID_CRC(StandClosed,2906609031)
    #define animSlotOpen ITF_GET_STRINGID_CRC(StandOpen,2278229236)

    // Link tag
    #define nameFaeryTag ITF_GET_STRINGID_CRC(Faery,2706872614)
    #define nameSlotDnTag ITF_GET_STRINGID_CRC(SlotDn,431178683)
    #define nameSteleTag ITF_GET_STRINGID_CRC(Stele,2350840939)

    // Sound / FX
    #define soundLaser ITF_GET_STRINGID_CRC(Laser,1098011849)
    
    //*****************************************************************************

    Ray_CrystalKingAIComponent::Ray_CrystalKingAIComponent()
    : m_fxControllerComponent(NULL)
    , m_linkComponent(NULL)
    , m_animLightComponent(NULL)
    , m_init(bfalse)
    , m_state(STATE_CRYSTAL_HIDE)
    , m_resource(ResourceID::Invalid)
    , m_offsetScrollUV(Vec2d::Zero)
    , m_faery(ITF_INVALID_OBJREF)
    , m_slotDn(ITF_INVALID_OBJREF)
    , m_stele(ITF_INVALID_OBJREF)
    {
        // none
    }

    //*****************************************************************************

    Ray_CrystalKingAIComponent::~Ray_CrystalKingAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get components
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        // Event
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

        // Mesh
        loadResource();
        createMesh(m_meshFront, m_vertexBufferCacheFront);
		createMesh(m_meshFadeFront, m_vertexBufferCacheFadeFront);

        createMesh(m_meshBack, m_vertexBufferCacheBack);
		createMesh(m_meshFadeBack, m_vertexBufferCacheFadeBack);

    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        destroyMesh(m_meshFront);
		destroyMesh(m_meshFadeFront);

		destroyMesh(m_meshBack);
		destroyMesh(m_meshFadeBack);

        unloadResource();
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::loadResource()
    {
        m_resource = m_actor->addResource(Resource::ResourceType_Texture,getTemplate()->getTextureFile());
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::unloadResource()
    {
        if( m_resource.isValidResourceId() )
        {
            m_actor->removeResource(m_resource);
            m_resource.invalidateResourceId();
        }
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::growAABB()
    {

        AABB aabb(m_actor->getPos());

        if (Actor* actSlot = m_slotDn.getActor())
        {
            aabb.grow(actSlot->getAABB());
        }

        aabb.grow(m_actor->getPos() + getTemplate()->getRayOffsetStart());


        m_actor->growRelativeAABBFromAbsolute(aabb);

    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::createMesh(ITF_Mesh& _mesh, SafeArray<VertexPCT>& _vertexBufferCache)
    {
        const u32 nbQuads    = COUNT_QUAD;
        const u32 nbVertices = nbQuads * 4;
        const u32 nbIndices  = nbQuads * 6;

#ifndef VBMANAGER_USE_DYNAMICRING_VB
        _mesh.m_vtxBufferBuffered[0] = GFX_ADAPTER->createVertexBuffer(nbVertices, VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
        ITF_ASSERT(_mesh.m_vtxBufferBuffered[0]);
        _mesh.m_vtxBufferBuffered[1] = GFX_ADAPTER->createVertexBuffer(nbVertices, VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
        ITF_ASSERT(_mesh.m_vtxBufferBuffered[1]);
#else
        _mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(nbVertices, VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB

        //create mesh element
        if( _mesh.m_ElementList.size() == 0 )
        {
            ITF_MeshElement elem;
            elem.m_startIndex = 0;
            elem.m_count = 0;
            _mesh.m_ElementList.push_back(elem);
        }

        //create mesh element index buffer
        _mesh.m_ElementList[0].m_indexBuffer = GFX_ADAPTER->createIndexBuffer(nbIndices, bfalse);

        //fill mesh element index buffer
        u16* indexBuffer;
        _mesh.m_ElementList[0].m_indexBuffer->Lock((void**)&indexBuffer);
        for(u16 index = 0; index < nbQuads; ++index)
        {
            const u16 vertexStartIndex = index * 4;
            const u16 indiceStartIndex = index * 6;

            //first triangle
            indexBuffer[indiceStartIndex] = vertexStartIndex;
            indexBuffer[indiceStartIndex + 1] = vertexStartIndex + 1;
            indexBuffer[indiceStartIndex + 2] = vertexStartIndex + 2;
            //second triangle
            indexBuffer[indiceStartIndex + 3] = vertexStartIndex + 2;
            indexBuffer[indiceStartIndex + 4] = vertexStartIndex + 3;
            indexBuffer[indiceStartIndex + 5] = vertexStartIndex;
        }
        _mesh.m_ElementList[0].m_indexBuffer->Unlock();

        //set up mesh element
        _mesh.m_ElementList[0].m_count = nbIndices;
        _mesh.m_ElementList[0].m_material.m_textureDiffuse    = m_resource;
        _mesh.m_ElementList[0].m_material.m_blendMode         = GFX_BLEND_ADDALPHA;
        _mesh.m_ElementList[0].m_material.m_TexAdressingModeU = GFXTADDRESS_WRAP;
        _mesh.m_ElementList[0].m_material.m_TexAdressingModeV = GFXTADDRESS_WRAP;

        //set mesh color
        _mesh.m_globalColor = Color::white();
        //set mesh matrix
        _mesh.setMatrix((GMatrix44&)GMatrixIdentity);

        _vertexBufferCache.resize( nbQuads * 4 );
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::destroyMesh(ITF_Mesh& _mesh)
    {
        //free vertex buffers
#ifndef VBMANAGER_USE_DYNAMICRING_VB
        if( _mesh.m_vtxBufferBuffered[0] != NULL )
        {
            GFX_ADAPTER->removeVertexBuffer(_mesh.m_vtxBufferBuffered[0]);
            _mesh.m_vtxBufferBuffered[0] = NULL;
        }
        if( _mesh.m_vtxBufferBuffered[1] != NULL )
        {
            GFX_ADAPTER->removeVertexBuffer(_mesh.m_vtxBufferBuffered[1]);
            _mesh.m_vtxBufferBuffered[1] = NULL;
        }
#else
        if( _mesh.m_vtxBuffer != NULL )
        {
            GFX_ADAPTER->removeVertexBuffer(_mesh.m_vtxBuffer);
            _mesh.m_vtxBuffer = NULL;	
        }		
#endif //VBMANAGER_USE_DYNAMICRING_VB

        //free index buffer
        if( _mesh.m_ElementList.size() > 0 )
        {
            if(_mesh.m_ElementList[0].m_indexBuffer != NULL)
            {
                GFX_ADAPTER->removeIndexBuffer(_mesh.m_ElementList[0].m_indexBuffer);
            }
        }
    }

    //*****************************************************************************


    void Ray_CrystalKingAIComponent::buildMesh()
    {
		// main ray
		const Color startColor_Part1    = Color(0.0f, 1.0f, 1.0f, 1.0f);
        const Color endColor_Part1      = Color::white();

        const Color startColor_Part2    = Color::white();
        const Color endColor_Part2      = Color::white();

		// fade ray in crystal
		const Color startFadeColor_Part1    = Color::white();
		const Color endFadeColor_Part1      = Color(0.0f, 1.0f, 1.0f, 1.0f);


        //compute particle 3d position
        Vec3d crystalPosition = m_actor->getPos() + getTemplate()->getRayOffsetCrystal();

        u32 index = 0;
        u32 vertexStartIndex = index * 4;

        f32 tileLen = getTemplate()->getTileLen();
        ITF_ASSERT(tileLen > 0.0f);
		if (tileLen <= 0.0f)
		{
			tileLen = 1.0f;
		}

		f32 lenFade = 1.0f;// TODO : expose len fade

        // First Ray

        Vec3d posA = crystalPosition + getTemplate()->getRayOffsetStart();
        Vec3d posB = crystalPosition + getTemplate()->getRayOffsetStart().normalize() * lenFade; 

        f32 lenMesh = (posA - posB).norm();
        f32 tileUV = lenMesh / tileLen;

        m_vertexBufferCacheFront[vertexStartIndex].m_pos = posA + (Vec3d::Up * (getTemplate()->getWidthA() * 0.5f));
        m_vertexBufferCacheFront[vertexStartIndex].m_color = startColor_Part1.getAsU32();
        m_vertexBufferCacheFront[vertexStartIndex].m_uv = Vec2d::Zero + m_offsetScrollUV;
   
        m_vertexBufferCacheFront[vertexStartIndex + 1].m_pos = posB + (Vec3d::Up * (getTemplate()->getWidthB() * 0.5f));
        m_vertexBufferCacheFront[vertexStartIndex + 1].m_color = endColor_Part1.getAsU32();
        m_vertexBufferCacheFront[vertexStartIndex + 1].m_uv = Vec2d(tileUV, 0.0f) + m_offsetScrollUV;

        m_vertexBufferCacheFront[vertexStartIndex + 2].m_pos = posB - (Vec3d::Up * (getTemplate()->getWidthB() * 0.5f));
        m_vertexBufferCacheFront[vertexStartIndex + 2].m_color = endColor_Part1.getAsU32();
        m_vertexBufferCacheFront[vertexStartIndex + 2].m_uv = Vec2d(tileUV, 1.0f)  + m_offsetScrollUV;

        m_vertexBufferCacheFront[vertexStartIndex + 3].m_pos = posA - (Vec3d::Up * (getTemplate()->getWidthA() * 0.5f));
        m_vertexBufferCacheFront[vertexStartIndex + 3].m_color = startColor_Part1.getAsU32();
        m_vertexBufferCacheFront[vertexStartIndex + 3].m_uv = Vec2d::YAxis + m_offsetScrollUV;

		// Fade
		Vec3d posAf = posB;
		Vec3d posBf = crystalPosition; 

		tileUV = lenFade / tileLen;

		m_vertexBufferCacheFadeFront[vertexStartIndex].m_pos = posAf + (Vec3d::Up * (getTemplate()->getWidthA() * 0.5f));
		m_vertexBufferCacheFadeFront[vertexStartIndex].m_color = startFadeColor_Part1.getAsU32();
		m_vertexBufferCacheFadeFront[vertexStartIndex].m_uv = Vec2d::Zero + m_offsetScrollUV;

		m_vertexBufferCacheFadeFront[vertexStartIndex + 1].m_pos = posBf + (Vec3d::Up * (getTemplate()->getWidthB() * 0.5f));
		m_vertexBufferCacheFadeFront[vertexStartIndex + 1].m_color = endFadeColor_Part1.getAsU32();
		m_vertexBufferCacheFadeFront[vertexStartIndex + 1].m_uv = Vec2d(tileUV, 0.0f) + m_offsetScrollUV;

		m_vertexBufferCacheFadeFront[vertexStartIndex + 2].m_pos = posBf - (Vec3d::Up * (getTemplate()->getWidthB() * 0.5f));
		m_vertexBufferCacheFadeFront[vertexStartIndex + 2].m_color = endFadeColor_Part1.getAsU32();
		m_vertexBufferCacheFadeFront[vertexStartIndex + 2].m_uv = Vec2d(tileUV, 1.0f)  + m_offsetScrollUV;

		m_vertexBufferCacheFadeFront[vertexStartIndex + 3].m_pos = posAf - (Vec3d::Up * (getTemplate()->getWidthA() * 0.5f));
		m_vertexBufferCacheFadeFront[vertexStartIndex + 3].m_color = startFadeColor_Part1.getAsU32();
		m_vertexBufferCacheFadeFront[vertexStartIndex + 3].m_uv = Vec2d::YAxis + m_offsetScrollUV;

        // Second Ray

        Vec3d posC = crystalPosition;
        Vec3d posD = crystalPosition;
		Vec3d posDf = crystalPosition; 

        if (m_slotDn.isValid())
        {
            if (Actor* actSlot = m_slotDn.getActor())
            {
                posD = actSlot->getPos();
				posDf = actSlot->getPos();
				posD = posD + ((posC - posD).normalize() * lenFade);
            }
        }

		Vec3d posCf = posD;


        lenMesh = (posC - posD).norm();
        tileUV = lenMesh / tileLen;

        m_vertexBufferCacheBack[vertexStartIndex].m_pos = posC + (Vec3d::Up * (getTemplate()->getWidthC() * 0.5f));
        m_vertexBufferCacheBack[vertexStartIndex].m_color = startColor_Part2.getAsU32();
        m_vertexBufferCacheBack[vertexStartIndex].m_uv = Vec2d::Zero + m_offsetScrollUV;

        m_vertexBufferCacheBack[vertexStartIndex + 1].m_pos = posD + (Vec3d::Up * (getTemplate()->getWidthD() * 0.5f));
        m_vertexBufferCacheBack[vertexStartIndex + 1].m_color = endColor_Part2.getAsU32();
        m_vertexBufferCacheBack[vertexStartIndex + 1].m_uv = Vec2d(tileUV, 0.0f) + m_offsetScrollUV;

        m_vertexBufferCacheBack[vertexStartIndex + 2].m_pos = posD - (Vec3d::Up * (getTemplate()->getWidthD() * 0.5f));
        m_vertexBufferCacheBack[vertexStartIndex + 2].m_color = endColor_Part2.getAsU32();
        m_vertexBufferCacheBack[vertexStartIndex + 2].m_uv = Vec2d(tileUV, 1.0f) + m_offsetScrollUV;

        m_vertexBufferCacheBack[vertexStartIndex + 3].m_pos = posC - (Vec3d::Up * (getTemplate()->getWidthC() * 0.5f));
        m_vertexBufferCacheBack[vertexStartIndex + 3].m_color = startColor_Part2.getAsU32();
        m_vertexBufferCacheBack[vertexStartIndex + 3].m_uv = Vec2d::YAxis + m_offsetScrollUV;


		// Fade
		tileUV = lenFade / tileLen;

		m_vertexBufferCacheFadeBack[vertexStartIndex].m_pos = posCf + (Vec3d::Up * (getTemplate()->getWidthD() * 0.5f));
		m_vertexBufferCacheFadeBack[vertexStartIndex].m_color = startFadeColor_Part1.getAsU32();
		m_vertexBufferCacheFadeBack[vertexStartIndex].m_uv = Vec2d::Zero + m_offsetScrollUV;

		m_vertexBufferCacheFadeBack[vertexStartIndex + 1].m_pos = posDf + (Vec3d::Up * (getTemplate()->getWidthD() * 0.5f));
		m_vertexBufferCacheFadeBack[vertexStartIndex + 1].m_color = endFadeColor_Part1.getAsU32();
		m_vertexBufferCacheFadeBack[vertexStartIndex + 1].m_uv = Vec2d(tileUV, 0.0f) + m_offsetScrollUV;

		m_vertexBufferCacheFadeBack[vertexStartIndex + 2].m_pos = posDf - (Vec3d::Up * (getTemplate()->getWidthD() * 0.5f));
		m_vertexBufferCacheFadeBack[vertexStartIndex + 2].m_color = endFadeColor_Part1.getAsU32();
		m_vertexBufferCacheFadeBack[vertexStartIndex + 2].m_uv = Vec2d(tileUV, 1.0f) + m_offsetScrollUV;

		m_vertexBufferCacheFadeBack[vertexStartIndex + 3].m_pos = posCf - (Vec3d::Up * (getTemplate()->getWidthD() * 0.5f));
		m_vertexBufferCacheFadeBack[vertexStartIndex + 3].m_color = startFadeColor_Part1.getAsU32();
		m_vertexBufferCacheFadeBack[vertexStartIndex + 3].m_uv = Vec2d::YAxis + m_offsetScrollUV;

    }


    //*****************************************************************************

    void Ray_CrystalKingAIComponent::onBecomeActive( )
    {
        Super::onBecomeActive();

    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::changeState( u32 _newState )
    {

        switch (_newState)
        {
        case STATE_CRYSTAL_HIDE:
            playAnim(animStandClosed);
            playAnimOtherAct(m_faery, animFaeryStand);
            playAnimOtherAct(m_slotDn, animSlotClosed);
            break;
        case STATE_CRYSTAL_OPENING:
            playAnim(animOpening);
            //playAnimOtherAct(m_faery, animFaeryStandToSit);
            playAnimOtherAct(m_slotDn, animSlotClosed);
            break;
        case STATE_CRYSTAL_OPEN:
            playAnim(animStandOpen);
			playAnimOtherAct(m_faery, animFaeryStand);
            //playAnimOtherAct(m_faery, animFaerySit);
            playAnimOtherAct(m_slotDn, animSlotClosed);
            break;
        case STATE_CRYSTAL_OPEN_LIGHT:
            {
                playAnim(animActivation);
                playAnimOtherAct(m_faery, animFaeryMeditation);
                playAnimOtherAct(m_slotDn, animSlotOpen);
                playSound();

                if (m_stele.isValid())
                {
                    if (Actor* actStele = m_stele.getActor())
                    {
                        Ray_ActivationSequenceVisualComponent* activationVisual = actStele->GetComponent<Ray_ActivationSequenceVisualComponent>();

                        if ( activationVisual )
                        {
                            activationVisual->setDisabled(bfalse);
                        }
                    }
                }
                break;
            }
        }

        m_state = _newState;
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::playSound()
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->playFX(soundLaser);
        }
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::playAnim( const StringID _anim)
    {
        if (m_animLightComponent && _anim.isValid())
        {
            m_animLightComponent->setAnim(_anim);
        }
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::playAnimOtherAct( ActorRef _actorRef, const StringID _anim )
    {
        if (_actorRef.isValid())
        {
            if (Actor* act = _actorRef.getActor())
            {
                if (AnimLightComponent* animCompo = act->GetComponent<AnimLightComponent>())
                {
                    animCompo->setAnim(_anim);
                }  
            }
        }
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if ( EventGeneric* eventGen = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            if (eventGen->getId() == ITF_GET_STRINGID_CRC(CrystalKing_Open,411560556))
            {
                changeState(STATE_CRYSTAL_OPENING);
            }
        }
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::init()
    {
        // Init other actor ref

        StringID valueTag; 

        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
            for (u32 i = 0; i < children.size(); i++)
            {
                Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());

                if ( pickable )
                {
                    Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if (childActor)
                    {
                        if (children[i].getTagValue(nameFaeryTag, valueTag) && !m_faery.isValid())
                        {
                            m_faery = childActor->getRef();                            
                        }    
                        else if (children[i].getTagValue(nameSlotDnTag, valueTag) && !m_slotDn.isValid())
                        {
                            m_slotDn = childActor->getRef();                            
                        }    
                        else if (children[i].getTagValue(nameSteleTag, valueTag) && !m_stele.isValid())
                        {
                            m_stele = childActor->getRef();                            
                        }    

                        
                    }
                }
            }
        }


        // Check if the world is completed
        ESpot_State stateWorld = RAY_GAMEMANAGER->getWMSpotState(getTemplate()->getCheckMapForUnlock());
        ESpot_State stateWorld_B = RAY_GAMEMANAGER->getWMSpotState(ITF_GET_STRINGID_CRC(JU_B,2419180662));

        if ( stateWorld == SPOT_STATE_COMPLETED )
        {
            changeState(STATE_CRYSTAL_OPEN_LIGHT);
        } 
        else if (stateWorld_B != SPOT_STATE_CLOSED)
        {
            changeState(STATE_CRYSTAL_OPEN);
        }
        else
        {
            changeState(STATE_CRYSTAL_HIDE);
        }
 

        m_init = btrue;
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

         if (!m_init)
        {
            init();
        }
        else
        {
            updateState(_dt);
        }

    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::updateState( f32 _dt )
    {
        switch (m_state)
        {
        case STATE_CRYSTAL_OPEN_LIGHT:

            m_offsetScrollUV.m_x += getTemplate()->getSpeedScroll() * _dt;
            buildMesh();
            growAABB();
            break;
        }
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::drawMesh(ITF_Mesh& _mesh, SafeArray<VertexPCT>& _vertexBufferCache, f32 _z)
    {

        if (m_state != STATE_CRYSTAL_OPEN_LIGHT)
        {
            return;
        }

        if( m_resource.isValidResourceId() )
        {
            Texture * tex = static_cast<Texture*>(m_resource.getResource());
            if( tex != NULL )
            {
                //get common information

#ifndef VBMANAGER_USE_DYNAMICRING_VB
                //switch mesh user buffer
                _mesh.m_userBuffer = !_mesh.m_userBuffer;
                u32 currentBuffer = _mesh.m_userBuffer;
                //get vertex buffer ptr
                ITF_VertexBuffer * vertexBuffer = _mesh.m_vtxBufferBuffered[currentBuffer];
#else
                ITF_VertexBuffer * vertexBuffer = _mesh.m_vtxBuffer;
#endif //VBMANAGER_USE_DYNAMICRING_VB

                //lock vertex buffer data
                VertexPCT* data = NULL;
                GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer, (void**)&data );
                ITF_ASSERT_MSG(data, "Unable to lock a vertex buffer");
                if(data != NULL)
                {
                    //write in vertex buffer data
                    ITF_MemcpyWriteCombined(data, _vertexBufferCache.getDataBuffer(), sizeof(VertexPCT)*_vertexBufferCache.size());
                    //unlock vertex buffer
                    GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);
                    //add primitive to be drawn
                    GFX_ADAPTER->addPrimitiveInZList((void*)&_mesh, GFX_ITF_MESH, NULL, _z, btrue,m_actor->getRef());
                }
            }
        }
    }

    //*****************************************************************************

    void Ray_CrystalKingAIComponent::Draw()
    {
        Super::Draw();

        drawMesh(m_meshFront, m_vertexBufferCacheFront, m_actor->getDepth() + getTemplate()->getRayZOffset_Part1());
		drawMesh(m_meshFadeFront, m_vertexBufferCacheFadeFront, m_actor->getDepth() + getTemplate()->getRayZOffset_Part1());

        drawMesh(m_meshBack, m_vertexBufferCacheBack,   m_actor->getDepth() + getTemplate()->getRayZOffset_Part2());
		drawMesh(m_meshFadeBack, m_vertexBufferCacheFadeBack,   m_actor->getDepth() + getTemplate()->getRayZOffset_Part2());

    }

    //*****************************************************************************

#ifdef ITF_SUPPORT_EDITOR

    void Ray_CrystalKingAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

    }

#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_CrystalKingAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_CrystalKingAIComponent_Template)

        SERIALIZE_MEMBER("checkMapForUnlock", m_checkMapForUnlock);

        SERIALIZE_MEMBER("textureFile", m_textureFile);
        SERIALIZE_MEMBER("tileLen", m_tileLen);

        SERIALIZE_MEMBER("rayOffsetCrystal", m_rayOffsetCrystal);
        SERIALIZE_MEMBER("rayOffsetStart", m_rayOffsetStart);

        SERIALIZE_MEMBER("widthA",      m_widthA);
        SERIALIZE_MEMBER("widthB",      m_widthB);
        SERIALIZE_MEMBER("widthC",      m_widthC);
        SERIALIZE_MEMBER("widthD",      m_widthD);

        SERIALIZE_MEMBER("speedScroll", m_speedScroll);

        SERIALIZE_MEMBER("rayZOffset_Part1", m_rayZOffset_Part1);
        SERIALIZE_MEMBER("rayZOffset_Part2", m_rayZOffset_Part2);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_CrystalKingAIComponent_Template::Ray_CrystalKingAIComponent_Template()  
    : m_tileLen(1.0f)
    , m_widthA(1.0f)
    , m_widthB(1.0f)
    , m_widthC(1.0f)
    , m_widthD(1.0f)
    , m_speedScroll(1.0f)
    , m_rayOffsetCrystal(Vec3d::Zero)
    , m_rayOffsetStart(Vec3d::Zero)
    , m_rayZOffset_Part1(0.0f)
    , m_rayZOffset_Part2(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_CrystalKingAIComponent_Template::~Ray_CrystalKingAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************



}
