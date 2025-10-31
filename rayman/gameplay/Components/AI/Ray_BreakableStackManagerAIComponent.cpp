#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableStackManagerAIComponent.h"
#endif //_ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableStackElementAIComponent.h" 
#endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#define CREDITS_MAX_LUMS 900

namespace ITF
{

#if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
    DECLARE_RASTER(BM_Raster_Update, RasterGroup_ComponentUpdate, Color::red());
    DECLARE_RASTER(BM_Raster_UpdateClearConnection, RasterGroup_ComponentUpdate, Color::black());
    DECLARE_RASTER(BM_Raster_UpdateConnection, RasterGroup_ComponentUpdate, Color::green());
    DECLARE_RASTER(BM_Raster_UpdateGravity, RasterGroup_ComponentUpdate, Color::yellow());
    DECLARE_RASTER(BM_Raster_UpdatePosPolyline, RasterGroup_ComponentUpdate, Color::blue());
    DECLARE_RASTER(BM_Raster_UpdatePolyline, RasterGroup_ComponentUpdate, Color::cyan());
    DECLARE_RASTER(BM_Raster_UpdateBlock, RasterGroup_ComponentUpdate, Color::white());
#endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_

    IMPLEMENT_OBJECT_RTTI(Ray_BreakableStackManagerAIComponent)
        
    BEGIN_SERIALIZATION_CHILD(Ray_BreakableStackManagerAIComponent)
        SERIALIZE_MEMBER("width",       m_width);
        SERIALIZE_MEMBER("height",      m_height);
        SERIALIZE_MEMBER("gravityFall", m_gravityFall);
        SERIALIZE_MEMBER("iceMode",     m_iceMode);
        SERIALIZE_MEMBER("syncAnim",    m_syncAnim);
        SERIALIZE_MEMBER("speedIceMode",m_speedIceMode);       
        SERIALIZE_FUNCTION(postLoadProperties, ESerialize_PropertyEdit_Load);        
    END_SERIALIZATION()

    // Info grid 
    BEGIN_SERIALIZATION(InfoElement)
        SERIALIZE_MEMBER("isOccupied",          m_isOccupied);
        SERIALIZE_MEMBER("reward",              m_reward);
        SERIALIZE_OBJECT("anim",                m_anim);
        SERIALIZE_OBJECT("animBreak",           m_animBreak);
        SERIALIZE_OBJECT("fragments",           m_fragments);
    END_SERIALIZATION()


    // Fragments / Debris 
    BEGIN_SERIALIZATION(FragmentsList)
        SERIALIZE_CONTAINER("fragments", m_fragments);
    END_SERIALIZATION()

    // Fx & Sound
    BEGIN_SERIALIZATION(FxData)
        SERIALIZE_MEMBER("fxFall",          m_fxFall);
        SERIALIZE_MEMBER("fxHit",           m_fxHit);
        SERIALIZE_MEMBER("fxSteam",         m_fxSteam);

        SERIALIZE_MEMBER("soundFall",       m_soundFall);
        SERIALIZE_MEMBER("soundHit",        m_soundHit);
        SERIALIZE_MEMBER("soundDestroy",    m_soundDestroy);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(InfoElementList)
        SERIALIZE_CONTAINER_OBJECT("elements", m_elements);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BreakableStackManagerAIComponent)
    END_VALIDATE_COMPONENT()

    //*****************************************************************************
    // Static
    //*****************************************************************************

    SafeArray<Ray_BreakableStackManagerAIComponent*> Ray_BreakableStackManagerAIComponent::m_managerList;

    const Actor* Ray_BreakableStackManagerAIComponent::getManager(Vec2d _pos)
    {

        for (u32 i = 0; i < m_managerList.size(); i++)
        {
            Ray_BreakableStackManagerAIComponent* managerCompo = m_managerList[i];
            
            if(managerCompo)
            {
                Actor* manager = managerCompo->GetActor();

                if (manager && !manager->hasDataError())
                {
                    AABB aabb = managerCompo->getMinMaxBox();

                    if (aabb.contains(_pos))
                    {
                        return manager;
                    }
                }
            }
        }

        return NULL;

    }

    //*****************************************************************************
    // 
    //*****************************************************************************


    Ray_BreakableStackManagerAIComponent::Ray_BreakableStackManagerAIComponent()
    : m_fxControllerComponent(NULL)
    , m_linkComponent(NULL)
    , m_width(4)
    , m_height(4)
    , m_gravityFall(0.1f)
    , m_timer(0.0f)
    , m_iceMode(bfalse)
    , m_syncAnim(btrue)
    , m_speedIceMode(0.5f)
    , m_atlasOffset(1.0f / 6.0f)
    , m_edgeSize(1.0f)
    , m_curGridAnimTrame(0)
    , m_offset(Vec2d::Zero)
	, m_countAtlasUv(0)
	, m_countLumCredits(0)
	, m_isCreditsMap(bfalse)
    {
        // TODO : Do this only in editor mode ?

        // Register me in manager list 
        Ray_BreakableStackManagerAIComponent::m_managerList.push_back(this);        
    }

    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::~Ray_BreakableStackManagerAIComponent()
    {        

        // Reset all connections
        clearConnection();

        // Delete all blocks
        for(u32 i = 0; i < m_listBlock.size(); i++)
        {
            if (m_listBlock[i])
            {
                SF_DEL(m_listBlock[i]);
            }
        }

        // Clean atlas
        m_atlasObject.clear(); 
        m_atlasParticles.clear();

        // Remove from static list
        for (u32 i = 0; i < m_managerList.size(); i++)
        {
            Ray_BreakableStackManagerAIComponent* managerCompo = m_managerList[i];

            if (managerCompo == this)
            {
                m_managerList.eraseNoOrder(i);
            }
        }

    }
    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::postLoadProperties()
    {

        //
        clearConnection();

        // Delete all blocks
        for(u32 i = 0; i < m_listBlock.size(); i++)
        {
            if (m_listBlock[i])
            {
                SF_DEL(m_listBlock[i]);
            }
        }
        m_listBlock.clear();
        m_listIndexFree.clear();

        // Clean atlas
        m_atlasObject.clear(); 
        m_atlasParticles.clear();

        gridInit();
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Register events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(RehitStim,1008690800), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBreakableQuery,4136568804), this);

         // Get Components
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        // Grid Init
        gridInit();

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
    {
        Super::onDepthChanged(_oldZ, _newZ);

        //AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);

        // TODO : update Z of all polyline and phantom ?
        //PhysWorld::moveBodyToLayer( PhysBody* _body, f32 _z )

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        activateAllBox(btrue);
       
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        activateAllBox(bfalse);
    }
    
    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::activateAllBox(bbool _active)
    {
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];

                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];

                    if (curBox.m_isOccupied)
                    {
                        if (_active)
                        {
                            curBox.activeBox(m_actor->getDepth());
                        }
                        else
                        {
                            curBox.desactiveBox();
                        }
                    }
                }
            }
        }
    }

    //*****************************************************************************

    AABB Ray_BreakableStackManagerAIComponent::getMinMaxBox()
    {

        Vec2d min = m_actor->get2DPos();
        Vec2d max = min + Vec2d(m_edgeSize * m_width, m_edgeSize * m_height);

        return AABB(min, max);
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::gridInit()
    {
        m_edgeSize = getTemplate()->getEdgeSize();

        // Init PhysShape
        FixedArray<Vec2d, 4> shapePoints;

        shapePoints.resize(4);
        f32 semiEdge = (m_edgeSize + OFFSET_COL_PHANTOM) * 0.5f; // Little more big than polyline
        shapePoints[0] = Vec2d(-semiEdge, -semiEdge);
        shapePoints[1] = Vec2d(-semiEdge, semiEdge);
        shapePoints[2] = Vec2d(semiEdge, semiEdge);
        shapePoints[3] = Vec2d(semiEdge, -semiEdge);
        m_physShapeBox.setPoints(shapePoints);

        // Init Atlas textures
        Path pathAtlas = getTemplate()->getAtlasPath();
        if (!pathAtlas.isEmpty() && pathAtlas.getStringID().isValid())
        {
            m_atlasObject.initialize(m_actor->getResourceGroup(), getTemplate()->getAtlasPath(), m_width * m_height);

			Texture * texture = m_atlasObject.getAtlasTexture();
			if(texture)
			{
				const UVAtlas* uvAtlas = texture->getUVAtlas();

				if (uvAtlas)
				{
					m_countAtlasUv = uvAtlas->getNumberUV();

					m_listUV.clear();
					Vec2d* listUV = &m_listUV.incrSize(m_countAtlasUv * 4);
					for (u32 i = 0; i < m_countAtlasUv; i++)
					{
						uvAtlas->get4UVAt(i, listUV+i*4);
					}
				}
			}
        }
        else
        {
            ITF_WARNING(m_actor, 0, "Atlas path is invalid !");
        }

        // Init Atlas particles textures
        Path pathParticleAtlas = getTemplate()->getAtlasParticlesPath();
        if (!pathParticleAtlas.isEmpty() && pathParticleAtlas.getStringID().isValid())
        {
            m_atlasParticles.initialize(m_actor->getResourceGroup(), getTemplate()->getAtlasParticlesPath(), (m_width * m_height) * PAR_COUNT_MAX_BY_BOX);
        }
        else
        {
            ITF_WARNING(m_actor, 0, "Atlas particles path is invalid !");
        }

		u32 sizeGrid = m_width * m_height;
        m_currentGrid.resize(sizeGrid);
        m_nextGrid.resize(sizeGrid);

        // Initialize size and init all value to U32_INVALID
		for(u32 index = 0; index < sizeGrid; index++)
		{
			m_currentGrid[index] = U32_INVALID;
			m_nextGrid[index] = U32_INVALID;
		}
	
    }

    //*****************************************************************************

    Vec2d Ray_BreakableStackManagerAIComponent::getSnapPos(Vec2d _spawnerPos) const
    {
        Vec2d managerPos = m_actor->get2DPos();

        u32 posRow = (u32((_spawnerPos.m_y+MTH_EPSILON) - managerPos.m_y)) / (u32)m_edgeSize;
        u32 posCol = (u32((_spawnerPos.m_x+MTH_EPSILON) - managerPos.m_x)) / (u32)m_edgeSize;

        Vec2d snapRelative;
        snapRelative.m_x = m_edgeSize * posCol;
        snapRelative.m_y = m_edgeSize * posRow;

        Vec2d snapPos = managerPos + snapRelative;

        return snapPos;
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::createBlocks(Actor* _spawner, u32 _checkPointRow, u32 _checkPointCol)
    {
        if (!_spawner)
        {
            return;
        }

        Vec2d originManager = getPosManager();

        if ( Ray_BreakableStackElementAIComponent* element = _spawner->GetComponent<Ray_BreakableStackElementAIComponent>() )
        {
            // Compute pos in grid
            u32 posRow = 0;
            u32 posCol = 0;
            if (_checkPointRow == U32_INVALID || _checkPointCol == U32_INVALID)
            {
                Vec2d posChild = _spawner->getBoundWorldInitialPos().truncateTo2D();
                posRow = (u32((posChild.m_y+MTH_EPSILON) - originManager.m_y)) / (u32)m_edgeSize;
                posCol = (u32((posChild.m_x+MTH_EPSILON) - originManager.m_x)) / (u32)m_edgeSize;
            }
            else
            {
                posRow = _checkPointRow;
                posCol = _checkPointCol;
            }


            // Get block's generals infos
            const InfoHeadElement& info = element->getInfoElement();

            u32 heightBlock = info.m_height;
            u32 widthBlock  = info.m_width;

            // Get fx info : sound & fx
            const FxData& fxData = element->getFxData();

            // Create & initialize the new blocks for grid
            Block* newBlock = newAlloc(mId_Gameplay, Block);
            newBlock->Init(m_actor, this, posRow, posCol, m_edgeSize, info, fxData, m_physShapeBox, Color::red());

            // Save act spawner
            newBlock->m_spawnerRef = _spawner->getRef();

            // Fill the new blocks with good data from spawner
            const InfoElementGrid& gridElement = element->getGridElement();

            ITF_ASSERT(heightBlock == gridElement.size());

            for(u32 indexRow = 0; indexRow < gridElement.size(); indexRow++)
            {
                const  ITF_VECTOR<InfoElement>& rowInfo = gridElement[indexRow].m_elements;

                ITF_ASSERT(widthBlock == rowInfo.size());

                for(u32 indexCol = 0; indexCol < rowInfo.size(); indexCol++)
                {
                    const InfoElement& infoElem = rowInfo[indexCol];

                    Box& curBox = newBlock->m_listBox[indexRow][indexCol];

                    curBox.m_isOccupied = infoElem.m_isOccupied;
                    curBox.m_reward     = infoElem.m_reward;
                    
                    if (curBox.m_isOccupied)
                    {
                        if (newBlock->m_hitPoints > 0)
                        {
                            // Phantom
                            PhysPhantomInitCommonInfo initPhantom;

                            SET_OWNER(initPhantom, m_actor->getRef());
                            initPhantom.m_shape = &m_physShapeBox;
                            initPhantom.m_pos = m_actor->get2DPos() + curBox.m_drawRelativePos;
                            initPhantom.m_angle = 0.0f; //m_actor->getAngle();
                            initPhantom.m_userData = m_actor->getRef();
                            initPhantom.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;

                            curBox.m_phantom = PHYSWORLD->allocPhantom(initPhantom);
                            curBox.m_phantom->updateAABB();
                            PHYSWORLD->insertPhantom(curBox.m_phantom,m_actor->getDepth());
                            curBox.m_physWorldRegistred = btrue;
                        }

                        // Animation
                        curBox.m_animation      = infoElem.m_anim;
                        curBox.m_animationBreak = infoElem.m_animBreak;
                        curBox.m_animPlayer.init(&curBox.m_animation);

                        // Fragments / Shards for particles generator
                        curBox.m_fragments = infoElem.m_fragments;
                    }
                }
            }

            // TODO : check space in grid before create block, for not make a NEW and DEL 

            // Try to add in grid
            bbool isAdded = addBlockInGrid(newBlock);

            if (isAdded)
            {
                // Check if this block have one or more empty row
                newBlock->checkEmptyRow();
            }
            else
            {
                ITF_WARNING_CATEGORY(LD, _spawner, 0, "This breakable bloc can't spawn in grid because there no place !");
                SF_DEL(newBlock);
            }

        }

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::createCreditsBlocks(InfoHeadElement& _info, FxData& _fxData, InfoElementGrid& _gridElement, u32 _row, u32 _col)
    {
        Vec2d originManager = getPosManager();

        u32 heightBlock = _info.m_height;
        u32 widthBlock  = _info.m_width;

        // Create & initialize the new blocks for grid
        Block* newBlock = newAlloc(mId_Gameplay, Block);
        newBlock->Init(m_actor, this, _row, _col, m_edgeSize, _info, _fxData, m_physShapeBox, Color::red());

        ITF_ASSERT(heightBlock == _gridElement.size());

        for(u32 indexRow = 0; indexRow < _gridElement.size(); indexRow++)
        {
            const  ITF_VECTOR<InfoElement>& rowInfo = _gridElement[indexRow].m_elements;

            ITF_ASSERT(widthBlock == rowInfo.size());

            for(u32 indexCol = 0; indexCol < rowInfo.size(); indexCol++)
            {
                const InfoElement& infoElem = rowInfo[indexCol];

                Box& curBox = newBlock->m_listBox[indexRow][indexCol];

                curBox.m_isOccupied = infoElem.m_isOccupied;
                curBox.m_reward     = infoElem.m_reward;
                
                if (curBox.m_isOccupied)
                {
                    if (newBlock->m_hitPoints > 0)
                    {
                        // Phantom
                        PhysPhantomInitCommonInfo initPhantom;

                        SET_OWNER(initPhantom, m_actor->getRef());
                        initPhantom.m_shape = &m_physShapeBox;
                        initPhantom.m_pos = m_actor->get2DPos() + curBox.m_drawRelativePos;
                        initPhantom.m_angle = 0.0f; //m_actor->getAngle();
                        initPhantom.m_userData = m_actor->getRef();
                        initPhantom.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;

                        curBox.m_phantom = PHYSWORLD->allocPhantom(initPhantom);
                        curBox.m_phantom->updateAABB();
                        PHYSWORLD->insertPhantom(curBox.m_phantom,m_actor->getDepth());
                        curBox.m_physWorldRegistred = btrue;
                    }

                    // Animation
                    curBox.m_animation      = infoElem.m_anim;
                    curBox.m_animationBreak = infoElem.m_animBreak;
                    curBox.m_animPlayer.init(&curBox.m_animation);

                    // Fragments / Shards for particles generator
                    curBox.m_fragments = infoElem.m_fragments;
                }
            }
        }

        // TODO : check space in grid before create block, for not make a NEW and DEL 

        // Try to add in grid
        bbool isAdded = addBlockInGrid(newBlock);

        if (isAdded)
        {
            // Check if this block have one or more empty row
            newBlock->checkEmptyRow();
        }
        else
        {
            SF_DEL(newBlock);
        }

      

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        #if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
                TIMERASTER_SCOPE(BM_Raster_Update);
        #endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_

        #ifdef _ITF_BSE_DEBUG_
                DebugDraw::text("Size m_listBlock : %d", m_listBlock.size());
                DebugDraw::text("Size m_listIndexFree : %d", m_listIndexFree.size());
        #endif

        // Global timer
        m_timer += _dt;

        // Sync all animation frame with the grid
        if (m_syncAnim)
        {
            m_curGridAnimTrame++;
        }

        // It melts the grid
        if (m_iceMode)
        {
            updateOffset(_dt);
        }
       
        // Block can fall ?
        //updateGravity();   // Double pass (not all case)
        updateGravityTree(); // Tree

        // Update with state
        updateBlock(_dt);

        // Switch between currentGrid and NextGrid
        switchGrid();

        // Reset all polyline's connection
        clearConnection();  

        // Free Memory to recycle item's vector
        removeAllDisabledBlock();

        // Compute new polyline position
        updatePosPolyline();

        // Compute polyline's connection (prev/next)
        updateConnection(); 

        // Simply update polylines
        updatePolyline(_dt);

        // update position & rotation/fade of particles
        updateParticles(_dt);

        growAABB();

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::updateOffset(f32 _dt)
    {

        Vec2d originManager = getPosManager();

        m_offset += -Vec2d::Up * (m_speedIceMode * _dt);

        u32 rowForDelete = 0; // Down iceMode
        if (m_speedIceMode < 0.0f)
        {
            rowForDelete = m_height; // Up iceMode
        }


        if(m_offset.m_y <= -m_edgeSize || m_offset.m_y >= m_edgeSize)
        {   
            m_offset = Vec2d::Zero;
            
            // Send event generic "newLine" to child
            if (m_linkComponent)
            {
                EventGeneric eventGeneric;
                eventGeneric.setSender( m_actor->getRef() );
                eventGeneric.setId(ITF_GET_STRINGID_CRC(breakableNewLine,1910942963));

                m_linkComponent->sendEventToChildren(&eventGeneric);
            }


            // ### DELETE ALL BOX IN THE FIRST ROW

            if (m_height > 0)
            {
                for(u32 indexCol = 0; indexCol < m_width; indexCol++)
                {
					u32 indexBlock = m_currentGrid[indexCol];

                    if (indexBlock != U32_INVALID)
                    {
                        ITF_ASSERT(indexBlock < m_listBlock.size());

                        Box* curBox = getBox(rowForDelete, indexCol);

                        if (curBox)
                        {
                            curBox->desactiveBox();  
                            curBox->m_isOccupied = bfalse;
                            Block* curBlock = getBlock(rowForDelete, indexCol);
                            setGridIdBlock(U32_INVALID, rowForDelete, indexCol);
                            setNextGridIdBlock(U32_INVALID, rowForDelete, indexCol);

                            if (curBlock)
                            {
                                // Play FX Steam
                                Vec3d fxPos((originManager + curBox->m_drawRelativePos).to3d());
                                playFx(curBlock->m_fxData.m_fxSteam, fxPos);

                                curBlock->checkEmptyRow();
                            }
                             
                        }

                    }

                }

            }

            // ### Move all to -1 row (Fall immediate)
            for (u32 i = 0; i < m_listBlock.size(); i++)
            {
                Block* curBlock = m_listBlock[i];

                if (!curBlock || curBlock->isDesactivate())
                {
                    continue;
                }

                startFallBlock(*curBlock,btrue); 
            }
       
            switchGrid();
        
        }

#ifndef ITF_FINAL
        //DebugDraw::text("offset = %f / %f", m_offset.m_x, m_offset.m_y );
#endif // ITF_FINAL
    }

    //*****************************************************************************

    Vec2d Ray_BreakableStackManagerAIComponent::getShake(f32 _intensX, f32 _freqX, f32 _intensY, f32 _freqY, bbool _dephase /*= bfalse*/)
    {
        // Random shake
        f32 percent = 0.1f;

        _intensX += Seeder::getSharedSeeder().GetFloat(0.0f, _intensX * percent);
        _intensY += Seeder::getSharedSeeder().GetFloat(0.0f, _intensY * percent);


        f32 shakeX = 0.0f;
        f32 shakeY = 0.0f;
            
        shakeX = _intensX * f32_Sin(m_timer * _freqX);

        if (_dephase)
        {
            _intensY = _intensY * 0.5f;
            shakeY = _intensY + (_intensY * f32_Sin(m_timer * _freqY));
        }
        else
        {
            shakeY = _intensY * f32_Sin(m_timer * _freqY);
        }

        return Vec2d(shakeX, shakeY);

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::updatePosBallistics(Block* _block, f32 _dt)
    {
        f32 m_gravity = getTemplate()->getGravityBallistics();
        f32 rotate = MTH_PI * 0.05f; // piBy20

        // Set ballistics pos
        if ((_block->m_timeExpulse += _dt) < getTemplate()->getTimeExpulse())
        {
            f32 progress = _block->m_timeExpulse /  getTemplate()->getTimeExpulse();

            // update fade alpha
            _block->m_alpha = 1.0f - progress;
            if (_block->m_alpha < 0.0f)
            {
                 _block->m_alpha = 0.0f;
            }

            for (u32 indexRow = 0 + _block->m_heightOffset; indexRow < _block->m_listBox.size(); indexRow++)
            {
                ITF_VECTOR<Box>& curRow = _block->m_listBox[indexRow];

                for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
                {
                    Box& curBox = curRow[indexCol];

                    if (curBox.m_isOccupied)
                    {                        
                        // Trajectory
                        curBox.m_drawRelativePos.m_x = curBox.m_startAbsoluteExpulsePos.m_x + curBox.m_impulse.m_x * _block->m_timeExpulse;
                        curBox.m_drawRelativePos.m_y = curBox.m_startAbsoluteExpulsePos.m_y + curBox.m_impulse.m_y * _block->m_timeExpulse + 0.5f * m_gravity * _block->m_timeExpulse * _block->m_timeExpulse;

                        // Rotate
                        if (_block->m_explodeToRight)
                        {
                            curBox.m_angle -= rotate;
                        }
                        else
                        {
                            curBox.m_angle += rotate;
                        }

                        // Z
                        curBox.m_currentExpulseZ = curBox.m_targetExpulseZ * progress;

                        // FX 
                        if (curBox.m_handleFX != U32_INVALID)
                        {
                            setFxPos(curBox.m_handleFX, Vec3d(curBox.m_drawRelativePos.m_x, curBox.m_drawRelativePos.m_y, curBox.m_currentExpulseZ));
#ifndef ITF_FINAL
                            //DebugDraw::circle(Vec3d(curBox.m_drawRelativePos.m_x, curBox.m_drawRelativePos.m_y, curBox.m_currentExpulseZ), 0.1f, Color::red(), 1.0f);
#endif // ITF_FINAL
                        }
                    }
                }
            }

        }
        else
        {
            _block->m_state = BLOCK_STATE_DISABLED;
        }

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::removeAllDisabledBlock()
    {

        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (curBlock && curBlock->m_state == BLOCK_STATE_DISABLED)
            {
                SF_DEL(curBlock);     
                m_listBlock[i] = NULL;
                m_listIndexFree.push_back(i);
                break;
            }

        }
    }


    //*****************************************************************************

    bbool Ray_BreakableStackManagerAIComponent::addBlockInGrid(Block* _block)
    {
        ITF_ASSERT(_block);

        if (!gridZoneIsFree(_block))
        {
            return bfalse;
        }

        // Recycled
        bbool recycled = bfalse;
        if (m_listIndexFree.size() > 0)
        {
            u32 index = m_listIndexFree.back();

            Block* ptrBlock = m_listBlock[index];
            ITF_ASSERT(!ptrBlock);

            if (!ptrBlock)
            {
                m_listBlock[index] = _block;
                m_listIndexFree.eraseNoOrder(m_listIndexFree.size() - 1);
                recycled = btrue; 
            }
        }

        if (!recycled)
        {        
            m_listBlock.push_back(_block);
        }


        for (u32 indexRow = 0; indexRow < _block->m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Box>& curRow = _block->m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
                    u32 indexBlock = getIndexBlock(*_block);

                    setGridIdBlock(indexBlock, _block->m_posRow + indexRow, _block->m_posCol + indexCol);
                    setNextGridIdBlock(indexBlock, _block->m_posRow + indexRow, _block->m_posCol + indexCol);
                }
            }
        }

        return btrue;
    } 
    //*****************************************************************************

    bbool Ray_BreakableStackManagerAIComponent::gridZoneIsFree(Block* _block)
    {
        for (u32 indexRow = 0; indexRow < _block->m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Box>& curRow = _block->m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
                    u32 iRow = _block->m_posRow + indexRow;
                    u32 iCol = _block->m_posCol + indexCol;

                    if (iRow >= m_height || iCol >= m_width)
                    {
                        // Out of grid
                        return bfalse;
                    }


                    if (m_currentGrid[iRow * m_width + iCol] != U32_INVALID)
                    {
                        //ITF_WARNING(m_actor, 0, "You can't put this block here in the grid. This zone is not free !");
                        return bfalse;
                    }
                }
            }
        }

        return btrue;

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::growAABB()
    {
      
        AABB aabb(m_actor->getPos());

        Vec3d actorPos = m_actor->getPos();

        Vec3d aabbMax = actorPos + Vec3d(m_edgeSize * m_width, m_edgeSize * m_height, 0.0f);

        aabb.grow(aabbMax);

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    //*****************************************************************************

    u32 Ray_BreakableStackManagerAIComponent::playFx(StringID _name, Vec3d _pos)
    {
        if (m_fxControllerComponent)
        {
            u32 fxHandle = m_fxControllerComponent->playFX(_name);

            m_fxControllerComponent->setFXPos(fxHandle,_pos); 
            return fxHandle; 
        }

        return U32_INVALID; 
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::setFxPos(u32 _handle, Vec3d _pos)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->setFXPos(_handle,_pos);
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::setFxAngle(u32 _handle, f32 _angle)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->setFXAngle(_handle, _angle);
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::stopFX(u32 _handle)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->stopFX(_handle);
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::setGridIdBlock(u32 _indexBlock, u32 _row, u32 _col)
    {
		if (_row < m_height && _col < m_width)
		{
			m_currentGrid[_row * m_width + _col] = _indexBlock;
			return;
		}

        ITF_ASSERT_MSG(0, "Try to set indexBlock in invalid cell.");
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::setNextGridIdBlock(u32 _indexBlock, u32 _row, u32 _col)
    {
		if (_row < m_height && _col < m_width)
		{
			m_nextGrid[_row * m_width + _col] = _indexBlock;
			return;
		}

        ITF_ASSERT_MSG(0, "Try to set indexBlock in invalid cell (NextGrid).");
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::sendEventToPolyline(Event* _event, ObjectRef _polylineRef)
    {
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];
                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];

                    for (u32 edge = 0; edge < curBox.m_procPolyList.size(); edge++)
                    {
                        ProceduralPolyline& procPoly = curBox.m_procPolyList[edge].m_procPoly;

                        if (procPoly.isActive())
                        {
                            ObjectRef polyRef = procPoly.getPolyline().getRef();
                            if (_polylineRef == polyRef)
                            {
                                procPoly.onEvent(_event);
                            }
                        }
                    }
                }
            }
        }
    }

    void Ray_BreakableStackManagerAIComponent::receiveQueryBreakable( Ray_EventBreakableQuery* _query )
    {
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];
                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];

                    for (u32 edge = 0; edge < curBox.m_procPolyList.size(); edge++)
                    {
                        ProceduralPolyline& procPoly = curBox.m_procPolyList[edge].m_procPoly;

                        if (procPoly.isActive())
                        {
                            ObjectRef polyRef = procPoly.getPolyline().getRef();
                            if (_query->getPolyline() == polyRef)
                            {
                                if ( curBox.m_phantom )
                                {
                                    _query->setIsBreakable(btrue);
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //*****************************************************************************

    // TODO : merger avec searchDestroy !!!

    Ray_BreakableStackManagerAIComponent::Block* Ray_BreakableStackManagerAIComponent::searchBlockbyPhysCollidable(ObjectRef _collidable)
    {
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];
                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];

                    if (curBox.m_isOccupied)
                    {
                        // Check phantom
                        if (curBox.m_phantom)
                        {  

                            if ( BaseObject * baseObj = _collidable.getObject() )
                            {
                                if ( PhysCollidable * physCol = baseObj->DynamicCast<PhysCollidable>(ITF_GET_STRINGID_CRC(PhysCollidable,1764848679)) )
                                {
                                    if ( PolyLine* poly = AIUtils::getPolyLine( physCol->getUserData() ) )
                                    {
                                        for (u32 edge = 0; edge < curBox.m_procPolyList.size(); edge++)
                                        {
                                            if ( poly == &(curBox.m_procPolyList[edge].m_procPoly.getPolyline()) )
                                            {
                                                return curBlock;
                                            }
                                        }
                                    }
                                }
                            }

                            ObjectRef phantomRef = curBox.m_phantom->getRef();
                            if (_collidable == phantomRef)
                            {                               
                                return curBlock;
                            }
                        }
                    }
                }
            }
        }

        return NULL;
    }

    //*****************************************************************************

    bbool Ray_BreakableStackManagerAIComponent::searchDestroyBlock(ObjectRef _phantom, Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender, ObjectRef _receiverReward)
    {

        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];
                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];

                    if (curBox.m_isOccupied)
                    {
                        // Check phantom
                        if (curBox.m_phantom)
                        {
                            // Special shooter 
                            //
                            if ( BaseObject * baseObj = _phantom.getObject() )
                            {
                                if ( PhysCollidable * physCol = baseObj->DynamicCast<PhysCollidable>(ITF_GET_STRINGID_CRC(PhysCollidable,1764848679)) )
                                {
                                    if ( PolyLine* poly = AIUtils::getPolyLine( physCol->getUserData() ) )
                                    {
                                        for (u32 edge = 0; edge < curBox.m_procPolyList.size(); edge++)
                                        {
                                            if ( poly == &(curBox.m_procPolyList[edge].m_procPoly.getPolyline()) )
                                            {
                                                hitBlock(*curBlock, _dirHit, _hitLevel, _sender, _receiverReward);
                                                return btrue;
                                            }
                                        }
                                    }
                                }
                            }

                            ObjectRef phantomRef = curBox.m_phantom->getRef();
                            if (_phantom == phantomRef)
                            {
                                hitBlock(*curBlock, _dirHit, _hitLevel, _sender, _receiverReward);
                                return btrue;
                            }
                        }
                    }
                }
            }
        }
        return bfalse;
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::hitBlock(Block& _block, Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender, ObjectRef _receiverReward)
    {

        // Unbreakable block !
        // TODO :  mettre un bool pour gerer �a
        if (_block.m_hitPoints <= 0)
        {
            return;
        }

        Vec2d originManager = getPosManager();

        // Explode stim
        if (_block.m_explosive)
        {
            if (_block.m_mustExplode)
            {
                return;
            }
            else
            {
                if(_sender == m_actor->getRef())
                {
                    _block.m_receiverReward = _receiverReward;
                    _block.m_timeExplode  = TIME_FOR_CHAINE_EXPLODE;
                }
                else
                {
                    _block.m_receiverReward = _receiverReward;
                    _block.m_timeExplode  = _block.m_timeBeforeExplode;
                }

                _block.m_mustExplode  = btrue;
                _block.m_dirExplode   = _dirHit;
                _block.m_levelExplode = _hitLevel;
            }

        }


        if (_block.m_countDownHit <= 0.0f)
        {
            _block.m_countDownHit = getTemplate()->getCountDownHit();

            if (!_block.m_mustExplode)
            {
                switch(_hitLevel)
                {
                case 0:
                    _block.m_hitPoints -= 1;
                    break;
                case 1:
                    _block.m_hitPoints -= 2;
                    break;
                case 2:
                    _block.m_hitPoints -= 4;
                    break;
                default:
                    _block.m_hitPoints -= 1;
                    break;
                }
            }

 
            for (u32 indexRow = 0 + _block.m_heightOffset; indexRow < _block.m_listBox.size(); indexRow++)
            {
                ITF_VECTOR<Box>& curRow = _block.m_listBox[indexRow];

                for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
                {
                    Box& curBox = curRow[indexCol];

					if (!_block.m_explosive)
					{
						curBox.m_animPlayer.init(&curBox.m_animationBreak);
					}

                    if (curBox.m_isOccupied)
                    {
                        // Play FX
                        //Vec3d fxPos(m_actor->getPos() + curBox.m_drawRelativePos.to3d());
                        //curBox.m_handleFX = playFx(_block.m_fxData.m_fxHit, fxPos);
                        //setFxAngle(curBox.m_handleFX, _dirHit.getAngle());


                        // Particles
                        if (_block.m_hitPoints > 0)
                        {
                            FragmentsList& fragList = curBox.m_fragments;
                            u32 nbParticles = Min((u32)fragList.m_fragments.size(), (u32)PAR_COUNT_MAX_BY_BOX);

                            f32 angle = 0.0f;
                            if (nbParticles > 0)
                            {
                                angle = MTH_2PI / (f32)nbParticles;
                            }
                             
                            f32 scale = PAR_SCALE_LITTLE_FRAG;
                            
                            for (u32 i = 0; i < nbParticles; i++)
                            {
                               // Circle dispos
                               Vec2d offsetRadius = Vec2d::Right * (m_edgeSize * 0.5f);
                               offsetRadius = offsetRadius.Rotate((f32)i * angle);                            

                               curBox.m_generator.addParticles(fragList.m_fragments[i], _dirHit, originManager + curBox.m_drawRelativePos + offsetRadius, scale);
                            }  
                        }
                        

                    } 
                }
            }

            // Block is dead
            if (_block.m_hitPoints <= 0)
            {
                destroyBlock(_block, _dirHit, _hitLevel, _sender, _receiverReward);
            }
            else
            {
                // Play sound
                playSound(_block.m_fxData.m_soundHit, _block.getRelativeCenterPos());

                // reset static if block catch hit
                if(_block.m_blockStatic)
                {
                    _block.m_blockStatic = bfalse;
                }

                _block.m_shaking = btrue;
                _block.m_timeShake = TIME_SHAKE_ON_HIT;
            }
        }
    }

   
    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::destroyBlock(Block& _block, Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender, ObjectRef _receiverReward)
    {

        Vec2d originManager = getPosManager();

        _block.m_timeExpulse = 0.0f;
        _block.m_state = BLOCK_STATE_EXPULSE;

        f32 dotImpulse = _dirHit.dot(Vec2d::Right);
        if (dotImpulse > 0.0f && dotImpulse <= 1.0f)
        {
            _block.m_explodeToRight = btrue;
        }

        if (_block.m_spawnerRef.isValid())
        {
            if (Actor* spawnerAct = _block.m_spawnerRef.getActor())
            {
                if ( Ray_BreakableStackElementAIComponent* element = spawnerAct->GetComponent<Ray_BreakableStackElementAIComponent>() )
                {
                    element->flagDestroy();
                }
            }
        }


        for (u32 indexRow = 0 + _block.m_heightOffset; indexRow < _block.m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Box>& curRow = _block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Box& curBox = curRow[indexCol];

                // Clean grid
                if (curBox.m_isOccupied) 
                {
                    if (curBox.m_reward > 0)
                    {
						if (!m_isCreditsMap || (m_isCreditsMap && m_countLumCredits < CREDITS_MAX_LUMS))
						{
							_block.spawnReward(m_actor, _receiverReward, curBox.m_drawRelativePos.to3d(), curBox.m_reward);
							m_countLumCredits++;
						}

                    }

                    // Fx & particles
                    FragmentsList& fragList = curBox.m_fragments;
                    u32 nbParticles = Min((u32)fragList.m_fragments.size(), (u32)PAR_COUNT_MAX_BY_BOX);

                    f32 angle = 0.0f;
                    if (nbParticles > 0)
                    {
                        angle = MTH_2PI / (f32)nbParticles;
                    }

                    for (u32 i = 0; i < nbParticles; i++)
                    {
                        // Circle dispos
                        Vec2d offsetRadius = Vec2d::Right * (m_edgeSize * 0.5f);
                        offsetRadius = offsetRadius.Rotate((f32)i * angle);                            

                        curBox.m_generator.addParticles(fragList.m_fragments[i], _dirHit, originManager + curBox.m_drawRelativePos + offsetRadius, 1.0f);
                    }  
                    ///// End FX

                    // Set expluse param
                    curBox.m_startAbsoluteExpulsePos = originManager + curBox.m_drawRelativePos;

                    // Set dir expulse
                    curBox.m_impulse.m_x = _dirHit.m_x;
                    curBox.m_impulse.m_y = _dirHit.m_y;

                    // random target Z
                    curBox.m_targetExpulseZ = Seeder::getSharedSeeder().GetFloat(EXPULSE_MIN_RANDOM_Z, EXPULSE_MAX_RANDOM_Z);

                    // random angle
                    f32 randomAngle = Seeder::getSharedSeeder().GetFloat(-EXPULSE_RANDOM_ANGLE, EXPULSE_RANDOM_ANGLE);
                    curBox.m_impulse = curBox.m_impulse.Rotate(randomAngle);

                    // random power
                    curBox.m_impulse *= Seeder::getSharedSeeder().GetFloat(0.0f, EXPULSE_RANDOM_POWER);

                    // Reset grid
                    setGridIdBlock(U32_INVALID, _block.m_posRow + indexRow - _block.m_heightOffset, _block.m_posCol + indexCol);
                    setNextGridIdBlock(U32_INVALID, _block.m_posRow + indexRow - _block.m_heightOffset, _block.m_posCol + indexCol);


                    ///// Explode
                    if (_block.m_explosive)
                    {

						// Set animation break, just for explode block
						curBox.m_animPlayer.init(&curBox.m_animationBreak);
	

                        // Send stim
                        PunchStim explodeStim;
                        explodeStim.setReceivedHitType(RECEIVEDHITTYPE_EJECTXY);
                        explodeStim.setIsRadial(btrue);
                        explodeStim.setLevel(1); // TODO : exposer level punch
                        explodeStim.setSender(m_actor->getRef());
                        explodeStim.setOriginalSender(_receiverReward);
                        explodeStim.setAngle(0.0f);
                        explodeStim.setFaction(getFaction());

                        const f32 z = m_actor->getDepth();

                        ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> > actorCollisions;

                        PhysShapeCircle physCircle(_block.m_radiusExplosive);
                        const Vec2d& stimPos = originManager + curBox.m_drawRelativePos;

                        PhysContactsContainer contacts;

                        PHYSWORLD->collidePhantoms(stimPos, stimPos, 0.0f,
                            &physCircle, m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS, contacts);

                        const u32 contactCount = contacts.size();

                        for ( u32 i = 0; i < contactCount; i++ )
                        {
                            ActorRef contactActorRef = contacts[i].m_collidableUserData;

                            Actor* contactActor = contactActorRef.getActor();
                            if (contactActor)
                            {
                                actorCollisions[contactActorRef].push_back(&contacts[i]);
                            }                      

                        }

                        for (ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> >::iterator it = actorCollisions.begin(); it != actorCollisions.end(); ++it)
                        {

                            ITF_VECTOR<SCollidableContact*> & collidables = it->second;
                            SCollidableContact* firstContact = collidables[0];

                            explodeStim.setDirection(firstContact->m_normal);
                            explodeStim.setFxPos(firstContact->m_contactPoint.to3d(z));
                            explodeStim.setPos(firstContact->m_contactPoint);

                            for (ITF_VECTOR<SCollidableContact*>::iterator it2 = collidables.begin(); it2 != collidables.end(); ++it2)
                            {
                                if (explodeStim.getContacts().size() < explodeStim.getContacts().capacity())
                                    explodeStim.addContact(*(*it2));
                            }

                            Actor * actor = it->first.getActor();
                            if (actor)
                                actor->onEvent(&explodeStim);
                            explodeStim.resetContacts();
                        }

#ifndef ITF_FINAL
                       //DebugDraw::shape(stimPos, m_actor->getDepth(), 0.0f, &physCircle);
#endif // ITF_FINAL
                    }

                    ///// Explode



                }

                curBox.desactiveBox();

            }
        }


        //FX explode
        if (_block.m_explosive)
        {          
			Vec3d fxPos(_block.getRelativeCenterPos().to3d());
            playFx(ITF_GET_STRINGID_CRC(Tetris_Explosion,3151088620), originManager.to3d() + fxPos); 

			playSound(ITF_GET_STRINGID_CRC(Tetris_OcSnd_Explosion,676057525), fxPos.truncateTo2D());
        }
		else
		{
			// Play sound
			playSound(_block.m_fxData.m_soundDestroy, _block.getRelativeCenterPos());
		}

      
        // Send event to child
        if (m_linkComponent)
        {
            EventTrigger eventTrig;
            eventTrig.setSender( m_actor->getRef() );
            eventTrig.setActivated(btrue);
            eventTrig.setActivator(_sender);
            m_linkComponent->sendEventToChildren(&eventTrig);
        }

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (RehitStim* evtOnHit = _event->DynamicCast<RehitStim>(ITF_GET_STRINGID_CRC(RehitStim,1008690800)))
        {
            if (canReceiveHit(evtOnHit))
            {
                receiveHit(evtOnHit->getContacts(), evtOnHit->getDirection(), evtOnHit->getLevel(), evtOnHit->getSender(), evtOnHit->getRewardReceiver());
            }
        }
        else if (HitStim* evtOnHit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
        {
            if (canReceiveHit(evtOnHit))
            {
                receiveHit(evtOnHit->getContacts(), evtOnHit->getDirection(), evtOnHit->getLevel(), evtOnHit->getSender(), evtOnHit->getOriginalSender());
            }
        }
        else if ( EventStickOnPolyline* eventStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            sendEventToPolyline(eventStick, eventStick->getPolyline());
        }
        else if ( EventHanging* eventHang = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
        {
            sendEventToPolyline(eventHang, eventHang->getPolyline());
        }
        else if (EventQueryBlocksHits* onQueryBlockHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
        {
            bbool canBlockHit = btrue;

             if (onQueryBlockHits->getCurrentPunchHitLevel() >= 2)
             {
                canBlockHit = getCanBlockHits(*onQueryBlockHits->getContacts());
             }

            onQueryBlockHits->setCanBlockHits(canBlockHit);
        }
        else if (Ray_EventBreakableQuery* onBreakableQuery = _event->DynamicCast<Ray_EventBreakableQuery>(ITF_GET_STRINGID_CRC(Ray_EventBreakableQuery,4136568804)))
        {
            receiveQueryBreakable(onBreakableQuery);
        }
    }

    //*****************************************************************************

    bbool Ray_BreakableStackManagerAIComponent::getCanBlockHits( const PhysContactsContainer& _contacts )
    {
        const u32 contactCount = _contacts.size();

        for (u32 i = 0; i < contactCount; i++)
        {
            const SCollidableContact& contact = _contacts[i];

            if (contact.m_collidable.isValid())
            {               
               Block* blockFound = searchBlockbyPhysCollidable(contact.m_collidable);

                if (blockFound)
                {

                    if (blockFound->m_hitPoints <= 0) // TODO : mettre un boolean unbreakable
                    {
                        return btrue;
                    }
                }
            }
        }

        return bfalse;
    }

    //*****************************************************************************

    bbool Ray_BreakableStackManagerAIComponent::canReceiveHit( HitStim* _hit )
    {
        if ( _hit->getSender() == m_actor->getRef() )
        {
            return btrue;
        }

        Faction faction = AIUtils::getFaction(m_actor);
        if (!AIUtils::isEnemyHit(_hit, faction) )
        {
            return bfalse;
        }

        return btrue;
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::receiveHit( const PhysContactsContainer& _contacts, Vec2d _direction,  u32 _level, ObjectRef _sender, ObjectRef _receiverReward)
    {

        if (!_receiverReward.isValid())
        {
            _receiverReward = _sender;
        }

        const u32 contactCount = _contacts.size();
        bbool hitValid = bfalse;

        for (u32 i = 0; i < contactCount; i++)
        {
            const SCollidableContact& contact = _contacts[i];

#ifndef ITF_FINAL
            // Debug phantom
            //PhysPhantom* phan = IRTTIObject::SafeDynamicCast<PhysPhantom>(contact.m_collidable.getObject(),ITF_GET_STRINGID_CRC(PhysPhantom,126559815));
            //DebugDraw::shape(phan->getPos(), m_actor->getDepth(), phan->getAngle(), phan->getShape(), Color::red(), 5.0f);  
#endif // ITF_FINAL

            if (contact.m_collidable.isValid())
            {               
                bbool found = searchDestroyBlock(contact.m_collidable, _direction, _level, _sender, _receiverReward);
                hitValid = hitValid || found;
            }
        }
        if ( hitValid ) 
        {
            if ( Actor * sender = AIUtils::getActor( _sender ) )
            {
                EventHitSuccessful hitSuccess;
                hitSuccess.setSender( m_actor->getRef() );
                sender->onEvent( &hitSuccess );
            }
        }


    }

    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::Block* Ray_BreakableStackManagerAIComponent::getBlock( u32 _row, u32 _col )
    {
		if (_row < m_height && _col < m_width)
		{
			u32 indexBlock = m_currentGrid[_row * m_width + _col];
			if (indexBlock != U32_INVALID)
			{
				ITF_ASSERT(indexBlock < m_listBlock.size());
				return m_listBlock[indexBlock];
			}
		}

        return NULL;
    }

    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::Box* Ray_BreakableStackManagerAIComponent::getBox( u32 _row, u32 _col )
    {

		if (_row < m_height && _col < m_width)
		{
			u32 indexBlock = m_currentGrid[_row * m_width + _col];
			if (indexBlock != U32_INVALID)
			{
				if (Block* curBlock = m_listBlock[indexBlock])
				{
					if (curBlock->m_state != BLOCK_STATE_DISABLED && curBlock->m_state != BLOCK_STATE_EXPULSE)
					{
						return curBlock->getGlobalBox(_row, _col);
					}
				}
			}
		}

        return NULL;
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::drawAtlas() 
    {
        const f32 offset = m_edgeSize * m_atlasOffset; //0.1666666666f;
        const f32 semiEdge = m_edgeSize * 0.5f;
        const Vec2d originManager = getPosManager();

		m_atlasObject.startFillVertexBuffer();
		ITF_VertexBuffer * vtxBuff = m_atlasObject.getCurrentVertexBuffer();
		u32 countQuadObject = 0;

        // Draw Box atlas in grid
        for(u32 indexRow = 0; indexRow < m_height; indexRow++)
        {
            for(i32 indexCol = m_width - 1; indexCol >= 0; indexCol--)
            {
                u32 indexBlock = m_currentGrid[indexRow * m_width + indexCol];

                if (indexBlock != U32_INVALID)
                {
                    ITF_ASSERT(indexBlock < m_listBlock.size());
                    Block* curBlock = m_listBlock[indexBlock];

                    if (!curBlock || curBlock->m_state == BLOCK_STATE_DISABLED)
                    {
                        continue;
                    }

                    Box* curBox = curBlock->getGlobalBox(indexRow, indexCol);

                    if (curBox && curBox->m_isOccupied)
                    {  
                        // Draw Blocks

                        Vec2d p1 = curBox->m_drawRelativePos;
                        p1.m_x += -semiEdge;
                        p1.m_y += semiEdge;
                        p1.m_x += -offset;
                        p1.m_y += offset;

                        Vec2d p2 = curBox->m_drawRelativePos;
                        p2.m_x += semiEdge;
                        p2.m_y += semiEdge;
                        p2.m_x += offset;
                        p2.m_y += offset;

                        Vec2d p3 = curBox->m_drawRelativePos;
                        p3.m_x += semiEdge;
                        p3.m_y += -semiEdge;
                        p3.m_x += offset;
                        p3.m_y += -offset;

                        Vec2d p4 = curBox->m_drawRelativePos;
                        p4.m_x += -semiEdge;
                        p4.m_y += -semiEdge;
                        p4.m_x += -offset;
                        p4.m_y += -offset;

                        if (curBlock->m_state == BLOCK_STATE_SHAKING_BEFORE_FALL || curBlock->m_shaking)
                        {
                            // Shake visual

                            Vec2d vecShake;
                            if (curBlock->m_shaking)
                            { 
                                vecShake = getShake(0.01f, 40.0f, 0.06f, 58.0f, curBlock->m_shaking);
                            }
                            else
                            {
                                vecShake = getShake(0.01f, 40.0f, 0.03f, 58.0f);
                            }


                            p1 += vecShake;
                            p2 += vecShake;
                            p3 += vecShake;
                            p4 += vecShake;  
                        }

                        // Explode blink
                        Color explodeColor(1.0f, 1.0f, 1.0f, 1.0f);
                        if (curBlock->m_mustExplode)
                        {                           
                            
                            if (curBlock->m_stateBlink == HL_STATE_ON)
                            {
                                if(curBlock->m_timeBlink <= 0.0f)
                                {
                                    // Switch state ON -> OFF
                                    curBlock->m_timeBlink  = HL_DURATION_OFF;
                                    curBlock->m_stateBlink = HL_STATE_OFF;
                                }
                            }
                            else
                            {
                                if(curBlock->m_timeBlink <= 0.0f)
                                {
                                    // Switch state OFF -> ON
                                    f32 newDurationON = HL_DURATION_ON -  ((HL_DURATION_ON * HL_DURATION_COEF) * curBlock->m_countBlink);
                                    newDurationON = Clamp(newDurationON, HL_DURATION_ON * HL_DURATION_COEF, HL_DURATION_ON);
        
                                    curBlock->m_timeBlink  = newDurationON;
                                    curBlock->m_stateBlink = HL_STATE_ON;
                                    curBlock->m_countBlink++;
                                }
                            }

                            if (curBlock->m_stateBlink == HL_STATE_ON)
                            {
                                explodeColor = Color(1.0f, 1.0f, 0.5f, 0.5f);  
                            }

                        }

						ITF_ASSERT(curBox->m_animPlayer.getCurAtlas() < m_countAtlasUv);
						if (curBox->m_animPlayer.getCurAtlas() < m_countAtlasUv)
						{
							vtxBuff->wgp_write(explodeColor.getAsU32(), 0.0f, originManager + p1, m_listUV[curBox->m_animPlayer.getCurAtlas() * 4]);
							vtxBuff->wgp_write(explodeColor.getAsU32(), 0.0f, originManager + p4, m_listUV[curBox->m_animPlayer.getCurAtlas() * 4 +1]);
							vtxBuff->wgp_write(explodeColor.getAsU32(), 0.0f, originManager + p3, m_listUV[curBox->m_animPlayer.getCurAtlas() * 4 +2]);
							vtxBuff->wgp_write(explodeColor.getAsU32(), 0.0f, originManager + p2, m_listUV[curBox->m_animPlayer.getCurAtlas() * 4 +3]);
							countQuadObject++;
						}


#ifndef ITF_FINAL
                        //DebugDraw::text(p1, 0.0f, Color::white(), "Id : %d", indexBlock);
                        //DebugDraw::circle(p1, 0.0f, 0.1f, Color::white());
                        //DebugDraw::circle(p2, 0.0f, 0.1f, Color::red());
                        //DebugDraw::circle(p3, 0.0f, 0.1f, Color::cyan());
                        //DebugDraw::circle(p4, 0.0f, 0.1f, Color::yellow());
#endif // ITF_FINAL

                        drawParticles(curBox);
                    }
                }
            }
        }


        // Draw atlas in expulse state
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->m_state != BLOCK_STATE_EXPULSE)
            {
                continue;
            }

            for (u32 indexRow = 0; indexRow < curBlock->m_listBox.size(); indexRow++)
            {
                ITF_VECTOR<Box>& curRow = curBlock->m_listBox[indexRow];

                for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
                {
                    Box& curBox = curRow[indexCol];

                    if (curBox.m_isOccupied)
                    {       

                        Vec2d p1 = curBox.m_drawRelativePos; // Absolute version for expulse
                        p1.m_x += -(m_edgeSize * 0.5f);
                        p1.m_y += (m_edgeSize * 0.5f);
                        p1.m_x += -offset;
                        p1.m_y += offset;

                        Vec2d p2 = curBox.m_drawRelativePos;
                        p2.m_x += (m_edgeSize * 0.5f);
                        p2.m_y += (m_edgeSize * 0.5f);
                        p2.m_x += offset;
                        p2.m_y += offset;

                        Vec2d p3 = curBox.m_drawRelativePos;
                        p3.m_x += (m_edgeSize * 0.5f);
                        p3.m_y += (-m_edgeSize * 0.5f);
                        p3.m_x += offset;
                        p3.m_y += -offset;

                        Vec2d p4 = curBox.m_drawRelativePos;
                        p4.m_x += (-m_edgeSize * 0.5f);
                        p4.m_y += (-m_edgeSize * 0.5f);
                        p4.m_x += -offset;
                        p4.m_y += -offset;

                        // Expulse rotation
                        p1 = p1.RotateAround(curBox.m_drawRelativePos, curBox.m_angle );
                        p2 = p2.RotateAround(curBox.m_drawRelativePos, curBox.m_angle );
                        p3 = p3.RotateAround(curBox.m_drawRelativePos, curBox.m_angle );
                        p4 = p4.RotateAround(curBox.m_drawRelativePos, curBox.m_angle );

                        Color alphaColor(curBlock->m_alpha, 1.0f, 1.0f, 1.0f);                        

						ITF_ASSERT(curBox.m_animPlayer.getCurAtlas() < m_countAtlasUv);
						if (curBox.m_animPlayer.getCurAtlas() < m_countAtlasUv)
						{
							vtxBuff->wgp_write(alphaColor.getAsU32(), curBox.m_currentExpulseZ, p1, m_listUV[curBox.m_animPlayer.getCurAtlas() * 4]);
							vtxBuff->wgp_write(alphaColor.getAsU32(), curBox.m_currentExpulseZ, p4, m_listUV[curBox.m_animPlayer.getCurAtlas() * 4 +1]);
							vtxBuff->wgp_write(alphaColor.getAsU32(), curBox.m_currentExpulseZ, p3, m_listUV[curBox.m_animPlayer.getCurAtlas() * 4 +2]);
							vtxBuff->wgp_write(alphaColor.getAsU32(), curBox.m_currentExpulseZ, p2, m_listUV[curBox.m_animPlayer.getCurAtlas() * 4 +3]);
							countQuadObject++;
						}

                        drawParticles(&curBox);
                    }

                }

            }

        }
     
		m_atlasObject.endFillVertexBuffer();
        //m_atlasObject.draw(m_actor->getDepth( ));
		m_atlasObject.drawVertexBuffer(m_actor->getDepth(), countQuadObject);

        m_atlasParticles.draw(m_actor->getDepth( ) + 0.1f);

    }

    //***************************************************************************** 


    void Ray_BreakableStackManagerAIComponent::updateBlock(f32 _dt)
    {
#if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
        TIMERASTER_SCOPE(BM_Raster_UpdateBlock);
#endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_

        // Get grid position 
        Vec2d originManager = getPosManager();

		u32 sizeGrid = m_width * m_height;
        for(u32 index = 0; index < sizeGrid; index++)
        {
                u32 indexBlock = m_currentGrid[index];

                if (indexBlock != U32_INVALID)
                {
                    ITF_ASSERT(indexBlock < m_listBlock.size());
                    Block* curBlock = m_listBlock[indexBlock];

                    if (!curBlock || curBlock->m_state == BLOCK_STATE_DISABLED)
                    {
                        continue;
                    }

                    if (curBlock->m_updated)
                    {
                        continue;
                    }
                    else
                    {
                        curBlock->m_updated = btrue;
                    }


                    // Update Countdown Hit
                    if (curBlock->m_countDownHit > 0.0f)
                    {
                        curBlock->m_countDownHit -= _dt;
                    }

                    // Update shaking hit
                    if(curBlock->m_shaking)
                    {
                        curBlock->m_timeShake -= _dt;

                        if (curBlock->m_timeShake <= 0.0f )
                        {
                            curBlock->m_shaking = bfalse;
                        }
                    }

                    // Update for explode
                    if(curBlock->m_mustExplode)
                    {
                        curBlock->m_timeExplode -= _dt;
                        curBlock->m_timeBlink -= _dt;

                        if (curBlock->m_timeExplode <= 0.0f )
                        {
                            // TODO : faire une fonction explode !
                            destroyBlock(*curBlock, curBlock->m_dirExplode, curBlock->m_levelExplode, m_actor->getRef(), curBlock->m_receiverReward);

                            break;
                        }
                    }

                    if (curBlock->m_state == BLOCK_STATE_FALLING)
                    {
                        f32 forceY = m_gravityFall;
                        curBlock->m_originRelativePos.m_y -= forceY;

                        f32 minPosY = m_edgeSize * curBlock->m_posRow;

                        if (curBlock->m_originRelativePos.m_y + /*MTH_EPSILON*/ 0.01f <= minPosY)
                        {
                            curBlock->m_originRelativePos.m_y = minPosY;
                            curBlock->m_state = BLOCK_STATE_STATIC;

                            if (curBlock->m_canFall)
                            {
                                // Continue falling
                                startFallBlock(*curBlock);
                            }
                            else
                            {
                                // Stop
                                //stopFX(curBlock->m_handleFX);
                                curBlock->m_handleFX = U32_INVALID;

                                // Play sound
                                playSound(curBlock->m_fxData.m_soundFall, curBlock->getRelativeCenterPos());

                                // Shaking
                                curBlock->m_shaking = btrue;
                                curBlock->m_timeShake = 0.1f;

                                // Save pos for checkpoint
                                if (curBlock->m_spawnerRef.isValid())
                                {
                                    if (Actor* spawnerAct = curBlock->m_spawnerRef.getActor())
                                    {
                                        if ( Ray_BreakableStackElementAIComponent* element = spawnerAct->GetComponent<Ray_BreakableStackElementAIComponent>() )
                                        {
                                            element->saveCheckpointPos(curBlock->m_posRow, curBlock->m_posCol);
                                        }
                                    }
                                }


                            }
                        }

                    }
                    else if(curBlock->m_state == BLOCK_STATE_SHAKING_BEFORE_FALL)
                    {
                        curBlock->m_timeShake -= _dt;

                        if (curBlock->m_timeShake <= 0.0f )
                        {
                            curBlock->m_timeShake = 0.0f;
                            startFallBlock(*curBlock);
                        }
                    }

                }           

        }


        // reset updated block + update expulse
        // clean updated flag
        // clean block disabled
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->m_state == BLOCK_STATE_DISABLED)
            {
                continue;
            }

            // update expulse
            if (curBlock->m_state == BLOCK_STATE_EXPULSE)
            {
                updatePosBallistics(curBlock, _dt);
            }

            curBlock->m_updated = bfalse;

        }

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::updateGravity()
    {

        #if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
                TIMERASTER_SCOPE(BM_Raster_UpdateGravity);
        #endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_
        
        // clean all canFall flag 
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            curBlock->m_canFall = btrue;
            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];

                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];
                    curBox.m_canFall = btrue;
                }
            }
        }


        // First pass : order by grid, check all "box" one at one (by bottom/left)

		for(u32 indexRow = 0; indexRow < m_height; indexRow++)
		{
			for(u32 indexCol = 0; indexCol < m_width; indexCol++)
			{
                u32 indexBlock = m_currentGrid[indexRow * m_width + indexCol];

                if (indexBlock != U32_INVALID)
                {
                    ITF_ASSERT(indexBlock < m_listBlock.size());

                    Block* curBlock = m_listBlock[indexBlock];
                  
                    // This block is already invalidate
                    if (!curBlock || !curBlock->m_canFall)
                    {
                        continue;
                    }
                    // One box can't fall, so block can't fall
                    else if ((curBlock->m_blockStatic /*&& !m_iceMode*/) || !boxCanFall(indexRow, indexCol))
                    {
                        curBlock->invalidFall();
                    }

                }
			}          
        }


        // Second pass : order by grid, check all "box" one at one (by top/right)
        for(i32 indexRow = m_height - 1; indexRow >= 0; indexRow--)
        {
            for(i32 indexCol = m_width - 1; indexCol >= 0; indexCol--)
            {
                u32 indexBlock = m_currentGrid[indexRow * m_width + indexCol];

                if (indexBlock != U32_INVALID)
                {
                    ITF_ASSERT(indexBlock < m_listBlock.size());

                    Block* curBlock = m_listBlock[indexBlock];

                    // This block is already invalidate
                    if (!curBlock->m_canFall)
                    {
                        continue;
                    }

                    // One box can't fall, so block can't fall
                    if (!boxCanFall(indexRow, indexCol))
                    {
                        curBlock->invalidFall();
                    }
                }
            }
        }


        // Request fall of block
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }
            
            if (curBlock->m_state == BLOCK_STATE_STATIC && curBlock->m_canFall)
            {
                requestFallBlock(*curBlock);
            }
        }


    }
  
    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::updateGravityTree()
    {

#if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
        TIMERASTER_SCOPE(BM_Raster_UpdateGravity);
#endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_


        // clean all canFall flag 
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }
            curBlock->m_canFall = btrue;
        }



        // Propagation by seed at bottom
        if (m_currentGrid.size() > 0)
        {
            for(u32 indexCol = 0; indexCol < m_width; indexCol++)
            {
                u32 indexBlock = m_currentGrid[indexCol];

                if (indexBlock != U32_INVALID)
                {
                    ITF_ASSERT(indexBlock < m_listBlock.size());

                    Block* curBlock = m_listBlock[indexBlock];

                    // This block is already invalidate
                    if (!curBlock || !curBlock->m_canFall)
                    {
                        continue;
                    }

					// This block can fall more
					curBlock->m_canFall = bfalse;
					checkContacts(curBlock);
                }
            }
        }
   

        // Propagation from shaking state blocks
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->m_state != BLOCK_STATE_SHAKING_BEFORE_FALL)
            {
                continue;
            }

			// This block can fall more
			curBlock->m_canFall = bfalse;
			checkContacts(curBlock);
        }


        // Check for static block
        //if (!m_iceMode)
        {
            for (u32 i = 0; i < m_listBlock.size(); i++)
            {
                Block* curBlock = m_listBlock[i];

                if (!curBlock || curBlock->isDesactivate() || !curBlock->m_blockStatic)
                {
                    continue;
                }

				// This block can fall more
				curBlock->m_canFall = bfalse;
				checkContacts(curBlock);
            }
        }



        // Request fall of block
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            if (curBlock->m_state == BLOCK_STATE_STATIC && curBlock->m_canFall)
            {
                requestFallBlock(*curBlock);
            }
        }



    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::checkContacts(Block* _block)
    {
        for(u32 indexRow = 0 + _block->m_heightOffset; indexRow < _block->m_height; indexRow++)
        {
            ITF_VECTOR<Box>& row = _block->m_listBox[indexRow];

            for(u32 indexCol = 0; indexCol < _block->m_width; indexCol++)
            { 

                Box& myBox = row[indexCol];

                if (myBox.m_isOccupied)
                {
                    u32 globalIndexRow = _block->m_posRow + indexRow - _block->m_heightOffset;
                    u32 globalIndexCol = _block->m_posCol + indexCol;

                    Block* topBlock = getBlock(globalIndexRow + 1, globalIndexCol);

                    if (topBlock && topBlock != _block && topBlock->m_canFall)
                    {
                        topBlock->m_canFall = bfalse;
                        checkContacts(topBlock);
                    }
                }
            }
        }

    }

    //*****************************************************************************


    void Ray_BreakableStackManagerAIComponent::clearConnection()
    {

        #if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
                TIMERASTER_SCOPE(BM_Raster_UpdateClearConnection);
        #endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_

        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock)
            {
                continue;
            }
 
            for(u32 indexRow = 0; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];
                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];

                    for (u32 edge = 0; edge < curBox.m_procPolyList.size(); edge++)
                    { 
                        curBox.m_procPolyList[edge].m_procPoly.getPolyline().m_connection.reset();
                    }
                }
            }
        }
    }


    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::updatePosPolyline()
    { 

        #if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
                TIMERASTER_SCOPE(BM_Raster_UpdatePosPolyline);
        #endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_

        Vec2d originManager = getPosManager();

        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            ITF_ASSERT(curBlock->m_state != BLOCK_STATE_DISABLED);
            ITF_ASSERT(curBlock->m_state != BLOCK_STATE_EXPULSE);

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];

                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];

                    if (curBox.m_isOccupied)
                    {

                        // ## Update polylines positions
                        if (m_iceMode || (!m_iceMode && curBlock->m_state == BLOCK_STATE_FALLING))
                        {
                     
                            Vec2d posA, posB, posC, posD;

                            posA.m_x = originManager.m_x + curBlock->m_originRelativePos.m_x + m_edgeSize * indexCol;
                            posA.m_y = originManager.m_y + curBlock->m_originRelativePos.m_y + m_edgeSize * (indexRow - curBlock->m_heightOffset);

                            posB.m_x = posA.m_x;
                            posB.m_y = posA.m_y + m_edgeSize;

                            posC.m_x = posA.m_x + m_edgeSize;
                            posC.m_y = posA.m_y + m_edgeSize;

                            posD.m_x = posA.m_x + m_edgeSize;
                            posD.m_y = posA.m_y;

                            curBox.m_drawRelativePos.m_x = (posA.m_x - originManager.m_x) + m_edgeSize * 0.5f;
                            curBox.m_drawRelativePos.m_y = (posA.m_y - originManager.m_y) + m_edgeSize * 0.5f;

                            for (u32 i = 0; i < curBox.m_procPolyList.size(); i++)
                            {
                                PolylineBox& polyBox = curBox.m_procPolyList[i];

                                switch(i)
                                {
                                case 0:
                                    polyBox.m_points[0] = posA;
                                    polyBox.m_points[1] = posB;
                                    break;

                                case 1:
                                    polyBox.m_points[0] = posB;
                                    polyBox.m_points[1] = posC;
                                    break;

                                case 2:
                                    polyBox.m_points[0] = posC;
                                    polyBox.m_points[1] = posD;
                                    break;

                                case 3:
                                    polyBox.m_points[0] = posD;
                                    polyBox.m_points[1] = posA;
                                    break;
                                }

                                polyBox.m_procPoly.setPoints(polyBox.m_points);

                            }

                            // Update phantom
                            if (curBox.m_phantom)
                            {
                                curBox.m_phantom->setPos(originManager + curBox.m_drawRelativePos);
                                curBox.m_phantom->updateAABB();
                            }

                        }

                    } // end if(curBox.m_isOccupied)
                }
            }
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::updateConnection()
    { 

        #if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
                TIMERASTER_SCOPE(BM_Raster_UpdateConnection);
        #endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_

		f32 depthActor = m_actor->getDepth();

        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            ITF_ASSERT(curBlock->m_state != BLOCK_STATE_DISABLED);
            ITF_ASSERT(curBlock->m_state != BLOCK_STATE_EXPULSE);

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];

                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 

                    Box& myBox = row[indexCol];

                    if (myBox.m_isOccupied)
                    {
                        u32 globalIndexRow = curBlock->m_posRow + indexRow - curBlock->m_heightOffset;
                        u32 globalIndexCol = curBlock->m_posCol + indexCol;

                        //ITF_ASSERT(m_currentGrid[globalIndexRow][globalIndexCol] != U32_INVALID); // Big problem ??
                        if(m_currentGrid[globalIndexRow * m_width + globalIndexCol] == U32_INVALID)
                        {
                            curBlock->m_state = BLOCK_STATE_DISABLED;
                            break;
                        }

                        NeighborBox neighborBox;

                        if (curBlock->m_state != BLOCK_STATE_STATIC)
                        {
                            curBlock->getLocalBoxNeighbor(globalIndexRow, globalIndexCol, neighborBox);
                        }
                        else
                        {
                            getGlobalBoxNeighbor(globalIndexRow, globalIndexCol, neighborBox);
                        }

                        // Update activate / deactivate polylines
                        myBox.updateOnOffPolylineBox(neighborBox, depthActor);

                        // Update connections
                        myBox.updateConnectionBox(neighborBox);

                    }
                }
            }
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::requestFallBlock(Block& _block)
    {
        // Start FX
        //Vec2d originManager = m_actor->getBoundWorldInitialPos().truncateTo2D();
        //Vec3d fxPos(m_actor->getPos() + _block.getRelativeCenterPos().to3d());
        //playFx(_block.m_fxData.m_fxFall, fxPos);
#ifndef ITF_FINAL
        //DebugDraw::circle(fxPos, 0.1f, Color::red(), 1.0f);
#endif // ITF_FINAL

        _block.m_timeShake = getTemplate()->getTimeShakeBeforeFall();
        _block.m_state     = BLOCK_STATE_SHAKING_BEFORE_FALL; 
     

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::startFallBlock(Block& _block, bbool _immediateFall /*= bfalse*/)
    {
        u32 myIndex = getIndexBlock(_block);

        if (_block.m_posRow <= 0)
        {
            return;
        }

        // Erase IdBloc in old pos in grid
        for (u32 indexRow = 0 + _block.m_heightOffset; indexRow < _block.m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Box>& curRow = _block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
					u32 row = _block.m_posRow + indexRow - _block.m_heightOffset;
					u32 col = _block.m_posCol + indexCol;
                    if (m_nextGrid[row * m_width + col] == myIndex)
                    {
                        setNextGridIdBlock(U32_INVALID, row, col);
                    }
                }
            }
        }

        // Move Row
        if (m_iceMode)
        {
            if (m_speedIceMode >= 0.0f)
            {
                _block.m_posRow -= 1;
            }
            else
            {
                _block.m_posRow += 1;
            }
        }
        else
        {
            _block.m_posRow -= 1;
        }


        if (!_immediateFall)
        {
            _block.m_state = BLOCK_STATE_FALLING;
            _block.m_timeShake = 0.0f;

            _block.m_originRelativePos.m_y -= m_gravityFall;
        }
        else
        {
            if (m_speedIceMode >= 0.0f)
            {
                _block.m_originRelativePos.m_y -= m_edgeSize;
            }
            else
            {
                _block.m_originRelativePos.m_y += m_edgeSize;
            }
        }

     
        // Warning : ne gere pas les blocks de plus de 2 lignes.
        if (_block.m_posRow >= m_height - 1 && _immediateFall)
        {
            return;
        }


        // Set IdBloc in new pos in grid
        for (u32 indexRow = 0 + _block.m_heightOffset; indexRow < _block.m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Box>& curRow = _block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
                    setNextGridIdBlock(getIndexBlock(_block), _block.m_posRow + indexRow - _block.m_heightOffset, _block.m_posCol + indexCol);
                }
            }
        }
    }

    //*****************************************************************************
    
    u32 Ray_BreakableStackManagerAIComponent::getIndexBlock(Block& _block)
    {
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            if (m_listBlock[i] == &_block)
            {
                return i;
            }
        }

        return U32_INVALID;
    }

    //*****************************************************************************

    bbool Ray_BreakableStackManagerAIComponent::boxCanFall(u32 _row, u32 _col)
    {

        if(_row == 0)
        {
            return bfalse;
        }

        Box* bottomBox = getBox(_row - 1, _col);

        Block* bottomBlock = getBlock(_row - 1, _col);
        if (bottomBlock && bottomBlock->m_state == BLOCK_STATE_SHAKING_BEFORE_FALL)
        {
            return bfalse;
        }

        if (bottomBox && bottomBox->m_isOccupied && !bottomBox->m_canFall)
        {
            return bfalse;
        }

        return btrue;
    }

    //*****************************************************************************

     void Ray_BreakableStackManagerAIComponent::getGlobalBoxNeighbor(u32 _globalIndexRow, u32 _globalIndexCol, NeighborBox& _neighborBox)
     {
         // Neighbor Row index 
         u32 rowTop    = _globalIndexRow + 1;
         u32 rowCur    = _globalIndexRow;
         u32 rowBottom = _globalIndexRow - 1;

         // Neighbor Col index 
         u32 colRight  = _globalIndexCol + 1;
         u32 colCur    = _globalIndexCol;
         u32 colLeft   = _globalIndexCol - 1;

		 // Fill NeighborBox
		 Block* blocA = getBlock(rowTop, colLeft);
		 if (blocA && blocA->m_state == BLOCK_STATE_STATIC)
		 {
			 _neighborBox.boxA = blocA->getGlobalBox(rowTop, colLeft);
		 }

		 Block* blocB = getBlock(rowTop, colCur);
		 if (blocB && blocB->m_state == BLOCK_STATE_STATIC)
		 {
			 _neighborBox.boxB = blocB->getGlobalBox(rowTop, colCur);  
		 }

		 Block* blocC = getBlock(rowTop, colRight);
		 if (blocC && blocC->m_state == BLOCK_STATE_STATIC)
		 {
			 _neighborBox.boxC = blocC->getGlobalBox(rowTop, colRight);                    
		 }

		 Block* blocD = getBlock(rowCur, colRight);
		 if (blocD && blocD->m_state == BLOCK_STATE_STATIC)
		 {
			 _neighborBox.boxD = blocD->getGlobalBox(rowCur, colRight);
		 }

		 Block* blocE = getBlock(rowBottom, colRight);
		 if (blocE && blocE->m_state == BLOCK_STATE_STATIC)
		 {
			 _neighborBox.boxE = blocE->getGlobalBox(rowBottom, colRight);
		 }

		 Block* blocF = getBlock(rowBottom, colCur);
		 if (blocF && blocF->m_state == BLOCK_STATE_STATIC)
		 {
			 _neighborBox.boxF = blocF->getGlobalBox(rowBottom, colCur);
		 }

		 Block* blocG = getBlock(rowBottom, colLeft);
		 if (blocG && blocG->m_state == BLOCK_STATE_STATIC)
		 {
			 _neighborBox.boxG = blocG->getGlobalBox(rowBottom, colLeft);
		 }

		 Block* blocH = getBlock(rowCur, colLeft);
		 if (blocH && blocH->m_state == BLOCK_STATE_STATIC)
		 {
			 _neighborBox.boxH = blocH->getGlobalBox(rowCur, colLeft);
		 }

     }

     //*****************************************************************************

     void Ray_BreakableStackManagerAIComponent::playSound(const StringID& _sound, Vec2d _pos)
     {
         if (m_fxControllerComponent && _sound.isValid())
         {  
             Vec3d soundPos(m_actor->getPos());
             soundPos += _pos.to3d();
             
             u32 handleFX = m_fxControllerComponent->playFX(_sound);
             m_fxControllerComponent->setFXPos(handleFX, soundPos);
         }
     }

     //*****************************************************************************

     void Ray_BreakableStackManagerAIComponent::updateParticles( f32 _dt )
     {
         for (u32 i = 0; i < m_listBlock.size(); i++)
         {
             Block* curBlock = m_listBlock[i];

             if (!curBlock || curBlock->m_state == BLOCK_STATE_DISABLED)
             {
                 continue;
             }

             for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
             {
                 ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];

                 for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                 { 
                     Box& curBox = row[indexCol];

                     if (curBox.m_isOccupied)
                     {
                         curBox.m_generator.update(_dt);
                     }
                 }
             }
         }
     }

     //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::updatePolyline( f32 _dt )
    {

        #if defined(_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_)
                TIMERASTER_SCOPE(BM_Raster_UpdatePolyline);
        #endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_RASTER_

        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];
                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    Box& curBox = row[indexCol];
                        
                    if (curBox.m_isOccupied)
                    {
                        for (u32 edge = 0; edge < curBox.m_procPolyList.size(); edge++)
                        { 
							if (curBox.m_procPolyList[edge].m_procPoly.isActive())
							{
								curBox.m_procPolyList[edge].m_procPoly.getPolyline().update(_dt);
							}
                        }

                        // update animation
                        if (m_syncAnim)
                        {
                            curBox.m_animPlayer.setFrame(m_curGridAnimTrame);
                        }
                        else
                        {
                            curBox.m_animPlayer.update();
                        }
                    }
                }
            }
        }
    }
      
    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Draw()
    {
        Super::Draw();
        drawAtlas();
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::drawParticles(Box* _box)
    {
        

        for(u32 i = 0; i < _box->m_generator.m_particles.size(); i++)
        {
            const ParticlesGenerator::Particle& par = _box->m_generator.m_particles[i];

            f32 semiSize = (m_edgeSize * 0.75f) * 0.5f;  
            semiSize *= par.m_scale;

            // Pos
            Vec2d p1 = par.m_curPos;
            p1.m_x += -semiSize;
            p1.m_y += semiSize;

            Vec2d p2 = par.m_curPos;
            p2.m_x += semiSize;
            p2.m_y += semiSize;   

            Vec2d p3 = par.m_curPos;
            p3.m_x += semiSize;
            p3.m_y += -semiSize;   

            Vec2d p4 = par.m_curPos;
            p4.m_x += -semiSize;
            p4.m_y += -semiSize;    

            // Rotation
            p1 = p1.RotateAround(par.m_curPos, par.m_angle );
            p2 = p2.RotateAround(par.m_curPos, par.m_angle );
            p3 = p3.RotateAround(par.m_curPos, par.m_angle );
            p4 = p4.RotateAround(par.m_curPos, par.m_angle );

            // Alpha
            Color alphaColor(par.m_alpha, 1.0f, 1.0f, 1.0f);                        
            m_atlasParticles.addQuad(par.m_atlasIndex, p1, p4, p3, p2, 0.0f, alphaColor.getAsU32());    

        }        

    }

#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_BreakableStackManagerAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (!(_flags & ActorComponent::DrawEditFlag_Collisions))
            return;

        Vec2d originPos = getPosManager(); 

        for(u32 indexRow = 0; indexRow < m_height; indexRow++)
        {

            for(u32 indexCol = 0; indexCol < m_width; indexCol++)
            {
#ifdef _ITF_BSE_DEBUG_
                u32 idBlock = m_currentGrid[indexRow * m_width + indexCol];
                u32 idBlocknext = m_nextGrid[indexRow * m_width + indexCol];
#endif

                Vec2d posA, posB, posC, posD;

                posA.m_x = originPos.m_x + m_edgeSize * indexCol;
                posA.m_y = originPos.m_y + m_edgeSize * indexRow;

                posB.m_x = posA.m_x;
                posB.m_y = posA.m_y + m_edgeSize;

                posC.m_x = posA.m_x + m_edgeSize;
                posC.m_y = posA.m_y + m_edgeSize;

                posD.m_x = posA.m_x + m_edgeSize;
                posD.m_y = posA.m_y;

                Vec2d posCenter;
                posCenter.m_x = posA.m_x + m_edgeSize * 0.5f;
                posCenter.m_y = posA.m_y + m_edgeSize * 0.5f;

                DebugDraw::line2D(posA,  posB, m_actor->getDepth(), Color::red(), 1.0f); 
                DebugDraw::line2D(posB,  posC, m_actor->getDepth(), Color::red(), 1.0f); 
                DebugDraw::line2D(posC,  posD, m_actor->getDepth(), Color::red(), 1.0f); 
                DebugDraw::line2D(posD,  posA, m_actor->getDepth(), Color::red(), 1.0f);

#ifdef _ITF_BSE_DEBUG_
                if (idBlock != U32_INVALID || idBlocknext != U32_INVALID)
                {
                    DebugDraw::text(posCenter, 0.0f, Color::green(), "%d / %d", idBlock, idBlocknext);
                }              
#endif

            }
        }

        // DrawDebug Polyline
        for (u32 i = 0; i < m_listBlock.size(); i++)
        {
            const Block* curBlock = m_listBlock[i];

            if (!curBlock || curBlock->isDesactivate())
            {
                continue;
            }

            //DebugDraw::text(originPos + curBlock->getRelativeCenterPos(), 0.0f, Color::blue(), "\nId :%d State : %d\nPos : %d, %d", i, curBlock->m_state, curBlock->m_posRow, curBlock->m_posCol);

            for(u32 indexRow = 0 + curBlock->m_heightOffset; indexRow < curBlock->m_height; indexRow++)
            {
                const ITF_VECTOR<Box>& row = curBlock->m_listBox[indexRow];
                for(u32 indexCol = 0; indexCol < curBlock->m_width; indexCol++)
                { 
                    const Box& curBox = row[indexCol];

                    for (u32 edge = 0; edge < curBox.m_procPolyList.size(); edge++)
                    { 
                        const PolylineBox& curPolylineBox = curBox.m_procPolyList[edge];

                        Color colorLine = curBlock->m_colorDebug;
                        if (curBox.m_isOccupied)
                        {
                            curPolylineBox.m_procPoly.drawEdit(_drawInterface, _flags);
                            //DebugDraw::line2D(curPolylineBox.m_points[0],  curPolylineBox.m_points[1], m_actor->getDepth(), colorLine, 4.0f);                            
                        }

                        // Phnatom
                        //if (curBox.m_phantom)
                        //DebugDraw::shape(curBox.m_phantom->getPos(), m_actor->getDepth(), curBox.m_phantom->getAngle(), curBox.m_phantom->getShape());  

                    } 
                }
            }
        }
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // PolylineBox
    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::PolylineBox::PolylineBox()
    {
        // none
    }

    Ray_BreakableStackManagerAIComponent::PolylineBox::~PolylineBox()
    {
        // none
    }

    //*****************************************************************************
    // Box
    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::Box::Box()
    : m_isOccupied(btrue)
    , m_canFall(btrue)
    , m_angle(0.0f)
    , m_targetExpulseZ(0.0f)
    , m_currentExpulseZ(0.0f)
    , m_handleFX(U32_INVALID)
    , m_phantom(NULL)
    , m_physWorldRegistred(bfalse)
    , m_reward(0)
    {
        m_procPolyList.resize(4);
    }

    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::Box::~Box()
    {
        if (m_phantom)
        {
            if (m_physWorldRegistred)
            {
                PHYSWORLD->removePhantom(m_phantom);
            }

            PHYSWORLD->deallocPhantom(m_phantom);
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Box::updateOnOffPolylineBox(NeighborBox& _neighborBox, f32 _depth)
    {

        ITF_ASSERT(m_procPolyList.size() == 4);
        ProceduralPolyline* proceduralPoly = NULL;

		u32 countActivate = 0;

        // LEFT
        proceduralPoly = &m_procPolyList[POLY_LEFT].m_procPoly;
        if(_neighborBox.boxH && _neighborBox.boxH->m_isOccupied)
        {                        
            proceduralPoly->deactivate();
        }
        else 
        {
            proceduralPoly->activate();
			countActivate++;
        }

        // TOP
        proceduralPoly = &m_procPolyList[POLY_TOP].m_procPoly;
        if(_neighborBox.boxB && _neighborBox.boxB->m_isOccupied)
        {                        
            proceduralPoly->deactivate();
        }
        else
        {
            proceduralPoly->activate();
			countActivate++;
        }

        // RIGHT
        proceduralPoly = &m_procPolyList[POLY_RIGHT].m_procPoly;
        if(_neighborBox.boxD && _neighborBox.boxD->m_isOccupied)
        {                        
            proceduralPoly->deactivate();
        }
        else
        {
            proceduralPoly->activate();
			countActivate++;
        }

        // BOTTOM
        proceduralPoly = &m_procPolyList[POLY_BOTTOM].m_procPoly;
        if(_neighborBox.boxF && _neighborBox.boxF->m_isOccupied)
        {                        
            proceduralPoly->deactivate();
        }
        else
        {
            proceduralPoly->activate();
			countActivate++;
        }

		if(m_phantom)
		{
			if (countActivate > 0 && !m_physWorldRegistred)
			{
				PHYSWORLD->insertPhantom(m_phantom, _depth);
				m_physWorldRegistred = btrue;
			}
			else if (countActivate == 0 && m_physWorldRegistred)
			{
				PHYSWORLD->removePhantom(m_phantom);
				m_physWorldRegistred = bfalse;		
			}
		}

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Box::updateConnectionBox(NeighborBox& _neighborBox)
    {

        ITF_ASSERT(m_procPolyList.size() == 4);

        // ### Left Polyline
        {
            ProceduralPolyline& leftProcPoly = m_procPolyList[POLY_LEFT].m_procPoly;

            if (leftProcPoly.isActive())
            {
                PolyLine& leftPoly = leftProcPoly.getPolyline();
                Vec2d&  posConnection = m_procPolyList[POLY_LEFT].m_points[1];

                if((_neighborBox.boxA && _neighborBox.boxB))
                {
                    PolyLine& nextPoly = _neighborBox.boxA->m_procPolyList[POLY_BOTTOM].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&leftPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_LEFT], _neighborBox.boxA->m_procPolyList[POLY_BOTTOM]);
                    #endif


                }
                else if (!_neighborBox.boxA && _neighborBox.boxB)
                {
                    PolyLine& nextPoly = _neighborBox.boxB->m_procPolyList[POLY_LEFT].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&leftPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_LEFT], _neighborBox.boxB->m_procPolyList[POLY_LEFT]);
                    #endif

                }
                else
                {
                    PolyLine& nextPoly = m_procPolyList[POLY_TOP].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&leftPoly, posConnection);
    
                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_LEFT], m_procPolyList[POLY_TOP]);
                    #endif

                }
            }
        }


        // ### Top Polyline
        {
            ProceduralPolyline& topProcPoly = m_procPolyList[POLY_TOP].m_procPoly;

            if (topProcPoly.isActive())
            {
                PolyLine& topPoly = topProcPoly.getPolyline();
                Vec2d&  posConnection = m_procPolyList[POLY_TOP].m_points[1];

                if((_neighborBox.boxC && _neighborBox.boxD))
                {
                    PolyLine& nextPoly = _neighborBox.boxC->m_procPolyList[POLY_LEFT].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&topPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_TOP], _neighborBox.boxC->m_procPolyList[POLY_LEFT]);
                    #endif

                }
                else if (!_neighborBox.boxC && _neighborBox.boxD)
                {
                    PolyLine& nextPoly = _neighborBox.boxD->m_procPolyList[POLY_TOP].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&topPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_TOP], _neighborBox.boxD->m_procPolyList[POLY_TOP]);
                    #endif

                }
                else
                {
                    PolyLine& nextPoly = m_procPolyList[POLY_RIGHT].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&topPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_TOP], m_procPolyList[POLY_RIGHT]);
                    #endif

                }
            }

        }




        // ### Right Polyline
        {
            ProceduralPolyline& rightProcPoly = m_procPolyList[POLY_RIGHT].m_procPoly;

            if (rightProcPoly.isActive())
            {
                PolyLine& rightPoly = rightProcPoly.getPolyline();
                Vec2d&  posConnection = m_procPolyList[POLY_RIGHT].m_points[1];

                if((_neighborBox.boxF && _neighborBox.boxE))
                {
                    PolyLine& nextPoly = _neighborBox.boxE->m_procPolyList[POLY_TOP].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&rightPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_RIGHT], _neighborBox.boxE->m_procPolyList[POLY_TOP]);
                    #endif

                }
                else if (!_neighborBox.boxE && _neighborBox.boxF)
                {
                    PolyLine& nextPoly = _neighborBox.boxF->m_procPolyList[POLY_RIGHT].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&rightPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_RIGHT], _neighborBox.boxF->m_procPolyList[POLY_RIGHT]);
                    #endif

                }
                else
                {
                    PolyLine& nextPoly = m_procPolyList[POLY_BOTTOM].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&rightPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_RIGHT], m_procPolyList[POLY_BOTTOM]);
                    #endif

                }
            }

        }



        // ### Bottom Polyline
        {
            ProceduralPolyline& bottomProcPoly = m_procPolyList[POLY_BOTTOM].m_procPoly;

            if (bottomProcPoly.isActive())
            {
                PolyLine& bottomPoly = bottomProcPoly.getPolyline();
                Vec2d&  posConnection = m_procPolyList[POLY_BOTTOM].m_points[1];

                if((_neighborBox.boxG && _neighborBox.boxH))
                {
                    PolyLine& nextPoly = _neighborBox.boxG->m_procPolyList[POLY_RIGHT].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&bottomPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_BOTTOM], _neighborBox.boxG->m_procPolyList[POLY_RIGHT]);
                    #endif


                }
                else if (!_neighborBox.boxG && _neighborBox.boxH)
                {
                    PolyLine& nextPoly = _neighborBox.boxH->m_procPolyList[POLY_BOTTOM].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&bottomPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_BOTTOM], _neighborBox.boxH->m_procPolyList[POLY_BOTTOM]);
                    #endif

                }
                else
                {
                    PolyLine& nextPoly = m_procPolyList[POLY_LEFT].m_procPoly.getPolyline();
                    nextPoly.m_connection.setPrevious(&bottomPoly, posConnection);

                    #ifdef _ITF_BSE_DEBUG_
                    debugDisplayConnection(m_procPolyList[POLY_BOTTOM], m_procPolyList[POLY_LEFT]);
                    #endif

                }
            }

        }

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Box::desactiveBox()
    {
        // Phantom
        if (m_physWorldRegistred)
        {
            if(m_phantom)
            {
                PHYSWORLD->removePhantom(m_phantom);
                m_physWorldRegistred = bfalse;
            }
        }

        // Polyline
        for (u32 i = 0; i < m_procPolyList.size(); i++) 
        {
            m_procPolyList[i].m_procPoly.deactivate(); 
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Box::activeBox(f32 _depth)
    {
        // Phantom
        if (!m_physWorldRegistred)
        {
            if(m_phantom)
            {
                PHYSWORLD->insertPhantom(m_phantom, _depth);
                m_physWorldRegistred = btrue;
            }
        }

        // Polyline
        for (u32 i = 0; i < m_procPolyList.size(); i++) 
        {
            m_procPolyList[i].m_procPoly.activate(); 
        }
    }

    //*****************************************************************************
    // Block
    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::Block::Block()
    : m_width(0)
    , m_height(0)
    , m_posRow(0)
    , m_posCol(0)
    , m_state(BLOCK_STATE_STATIC)
    , m_handleFX(U32_INVALID)
    , m_hitPoints(BLOCK_HIT_POINTS)
    , m_countDownHit(0.0f)
    , m_timeShake(0.0f)
    , m_timeExpulse(0.0f)
    , m_updated(bfalse)
    , m_heightOffset(0)
    , m_canFall(btrue)
    , m_alpha(1.0f)
    , m_explodeToRight(bfalse)
    , m_shaking(bfalse)
    , m_blockStatic(bfalse)
    , m_mustExplode(bfalse)
    , m_timeExplode(0.0f)
    , m_levelExplode(0)
    , m_explosive(bfalse)
    , m_radiusExplosive(1.0f)
    , m_timeBeforeExplode(0.0f)
    , m_timeBlink(0.0f)
    , m_stateBlink(HL_STATE_ON)
    , m_countBlink(0)
    , m_edgeSize(1.0f)
    {            
        // none
    }

    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::Block::~Block()
    {
        // Reset polyline connections
//        resetConnection();        
    }

    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent::Box* Ray_BreakableStackManagerAIComponent::Block::getGlobalBox(u32 _rowGlobal, u32 _colGlobal)
    {
        u32 localRow = (_rowGlobal - m_posRow) + m_heightOffset;
        u32 localCol = (_colGlobal - m_posCol);

        if (localRow < m_height && localCol < m_width)
        {
            return &(m_listBox[localRow][localCol]);
        }

        return NULL;
    }
    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Block::spawnReward(Actor* _sender, ObjectRef _receiver, Vec3d _offset, u32 _type)
    {
        Ray_EventSpawnRewardLum lum;

        Ray_EventSpawnReward* reward = NULL;

        switch(_type)
        {
        case 1:
        default:
            reward = &lum;
            break;
        }

        reward->clear();
        reward->setup(_sender, _offset);
        reward->setNumRewards(1);
        reward->setRewardReceiver(_receiver);
        reward->setEjectionDir(Vec2d::Up);
        GAMEMANAGER->onEvent(reward);   

    }

    //*****************************************************************************
    
    const Vec2d Ray_BreakableStackManagerAIComponent::Block::getRelativeCenterPos() const
    {
        Vec2d blockCenterPos;

        blockCenterPos.m_x = m_originRelativePos.m_x + (m_edgeSize * m_width) * 0.5f;
        blockCenterPos.m_y = m_originRelativePos.m_y + (m_edgeSize * m_height) * 0.5f;

        return blockCenterPos;
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Block::getLocalBoxNeighbor(u32 _globalIndexRow, u32 _globalIndexCol, NeighborBox& _neighborBox)
    {
        // Neighbor Row index 
        u32 rowTop    = _globalIndexRow + 1;
        u32 rowCur    = _globalIndexRow;
        u32 rowBottom = _globalIndexRow - 1;

        // Neighbor Col index 
        u32 colRight  = _globalIndexCol + 1;
        u32 colCur    = _globalIndexCol;
        u32 colLeft   = _globalIndexCol - 1;

        // Fill NeighborBox
        _neighborBox.boxA = getGlobalBox(rowTop, colLeft);
        if (_neighborBox.boxA && !_neighborBox.boxA->m_isOccupied) {_neighborBox.boxA = NULL;}

        _neighborBox.boxB = getGlobalBox(rowTop, colCur);  
        if (_neighborBox.boxB && !_neighborBox.boxB->m_isOccupied) {_neighborBox.boxB = NULL;}

        _neighborBox.boxC = getGlobalBox(rowTop, colRight);                    
        if (_neighborBox.boxC && !_neighborBox.boxC->m_isOccupied) {_neighborBox.boxC = NULL;}

        _neighborBox.boxD = getGlobalBox(rowCur, colRight);
        if (_neighborBox.boxD && !_neighborBox.boxD->m_isOccupied) {_neighborBox.boxD = NULL;}

        _neighborBox.boxE = getGlobalBox(rowBottom, colRight);
        if (_neighborBox.boxE && !_neighborBox.boxE->m_isOccupied) {_neighborBox.boxE = NULL;}

        _neighborBox.boxF = getGlobalBox(rowBottom, colCur);
        if (_neighborBox.boxF && !_neighborBox.boxF->m_isOccupied) {_neighborBox.boxF = NULL;}

        _neighborBox.boxG = getGlobalBox(rowBottom, colLeft);
        if (_neighborBox.boxG && !_neighborBox.boxG->m_isOccupied) {_neighborBox.boxG = NULL;}

        _neighborBox.boxH = getGlobalBox(rowCur, colLeft);
        if (_neighborBox.boxH && !_neighborBox.boxH->m_isOccupied) {_neighborBox.boxH = NULL;}

    }

    //*****************************************************************************

    // TODO : replace all parameters by struct ?
    void Ray_BreakableStackManagerAIComponent::Block::Init(Actor* _owner, IEventListener* _eventListener, u32 _posRow, u32 _posCol, f32 _edgeSize, const InfoHeadElement& _info, const FxData& _fxData, PhysShapePolygon& _physShape, Color _color)
    {
        // Init block members
        m_width         = _info.m_width;
        m_height        = _info.m_height;
        m_hitPoints     = _info.m_hitPoint;
        m_blockStatic   = _info.m_blockStatic;
        m_explosive     = _info.m_explosive;
        m_radiusExplosive    = _info.m_radiusExplosive;
        m_timeBeforeExplode  = _info.m_timeBeforeExplode;

        m_edgeSize      = _edgeSize;
        m_posRow        = _posRow;
        m_posCol        = _posCol;

        m_colorDebug    = _color;

        // FxData
        m_fxData = _fxData;

        // Get grid position 
        //Vec2d originManager = _owner->getBoundWorldInitialPos().truncateTo2D();
		Vec2d originManager = _owner->getWorldInitialPos().truncateTo2D();


        // Relative
        m_originRelativePos.m_x = m_edgeSize * m_posCol;
        m_originRelativePos.m_y = m_edgeSize * (m_posRow - m_heightOffset);

        // Absolute
        Vec2d originAbsolutePos = originManager + m_originRelativePos;

        // Init block sub grid

        m_listBox.resize(m_height); 
        for (u32 indexRow = 0; indexRow < m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Box>& curRow = m_listBox[indexRow];
            
            curRow.resize(m_width);
            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Box& curBox = curRow[indexCol];

                // Set box points
                //  --------
                // | B    C |
                // |        |
                // | A    D |
                //  --------
                Vec2d posA, posB, posC, posD;

                posA.m_x = originAbsolutePos.m_x + m_edgeSize * indexCol;
                posA.m_y = originAbsolutePos.m_y + m_edgeSize * (indexRow - m_heightOffset);

                posB.m_x = posA.m_x;
                posB.m_y = posA.m_y + m_edgeSize;

                posC.m_x = posA.m_x + m_edgeSize;
                posC.m_y = posA.m_y + m_edgeSize;

                posD.m_x = posA.m_x + m_edgeSize;
                posD.m_y = posA.m_y;

                // Init box draw pos
                curBox.m_drawRelativePos.m_x = (posA.m_x - originManager.m_x) + m_edgeSize * 0.5f;
                curBox.m_drawRelativePos.m_y = (posA.m_y - originManager.m_y) + m_edgeSize * 0.5f;

                // Set polyline points
                for (u32 i = 0; i < curBox.m_procPolyList.size(); i++)
                {
                    PolylineBox& polyBox = curBox.m_procPolyList[i];
                    polyBox.m_points.resize(2);

                    switch(i)
                    {
                    case 0:
                        polyBox.m_points[0] = posA;
                        polyBox.m_points[1] = posB;
                        break;

                    case 1:
                        polyBox.m_points[0] = posB;
                        polyBox.m_points[1] = posC;
                        break;

                    case 2:
                        polyBox.m_points[0] = posC;
                        polyBox.m_points[1] = posD;
                        break;

                    case 3:
                        polyBox.m_points[0] = posD;
                        polyBox.m_points[1] = posA;
                        break;
                    }

                    polyBox.m_procPoly.init(_owner, _eventListener);
                    polyBox.m_procPoly.setPoints(polyBox.m_points);
                    if ( _info.m_gmat.getStringID() != GameMaterial_Template::nullMatName.getStringID() )
                    {
                        polyBox.m_procPoly.setGameMaterial(_info.m_gmat);
                    }
                    polyBox.m_procPoly.deactivate();
                }


            }
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Block::resetConnection()
    {
        for(u32 indexRow = 0; indexRow < m_height; indexRow++)
        {
            ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& row =  m_listBox[indexRow];
            for(u32 indexCol = 0; indexCol <  m_width; indexCol++)
            { 
                Ray_BreakableStackManagerAIComponent::Box& curBox = row[indexCol];

                for (u32 edge = 0; edge < curBox.m_procPolyList.size(); edge++)
                { 
                    curBox.m_procPolyList[edge].m_procPoly.getPolyline().m_connection.reset();
                }
            }
        }
    }

    //*****************************************************************************

    bbool Ray_BreakableStackManagerAIComponent::Block::checkEmptyRow()
    {
        bbool rowRemoved = bfalse;

        for (u32 indexRow = 0 + m_heightOffset; indexRow < m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Box>& curRow = m_listBox[indexRow];

            bbool rowIsEmpty = btrue;

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
                    rowIsEmpty = bfalse;
                    break;
                }
            }

            if (rowIsEmpty)
            {
                rowRemoved = btrue;

                m_posRow += 1;
                m_heightOffset += 1;
                m_originRelativePos += Vec2d(0.0f, m_edgeSize);

                if (m_listBox.size() == m_heightOffset)
                {
                    m_state = BLOCK_STATE_DISABLED;
                }

            }
            else
            {
                break;
            }

        }

        return rowRemoved;
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::Block::invalidFall()
    {
        m_canFall = bfalse;

        for (u32 indexRow = 0 + m_heightOffset; indexRow < m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Box>& curRow = m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Box& curBox = curRow[indexCol];
                curBox.m_canFall = bfalse;
            }
        }

    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::ParticlesGenerator::update(f32 _dt )
    {
        f32 addAngle = PAR_ROTATION;
        f32 gravity = PAR_GRAVITY;

        for(u32 i = 0; i < m_particles.size(); i++)
        {
            Particle& par = m_particles[i];

            // Check life time
            par.m_time += _dt;
            if (par.m_time > par.m_timeMax)
            {
                m_particles.removeAt(i);
                continue;
            }

            // Progression
            f32 progress = par.m_time / par.m_timeMax;
            f32 progressAlpha = (progress - PAR_PERCENT_START_ALPHA) / (1.0f - PAR_PERCENT_START_ALPHA);
            progressAlpha = Max(0.0f, progressAlpha);

            // update fade alpha
            par.m_alpha = 1.0f - progressAlpha;
            if (par.m_alpha < 0.0f)
            {
                par.m_alpha = 0.0f;
            }

            // Ballistics
            par.m_curPos.m_x = par.m_startPos.m_x + par.m_dir.m_x * par.m_time;
            par.m_curPos.m_y = par.m_startPos.m_y + par.m_dir.m_y * par.m_time + 0.5f * gravity * par.m_time * par.m_time;

            if (par.m_explodeToRight)
            {
                par.m_angle += addAngle;
            }
            else
            {
                par.m_angle -= addAngle;
            }

        }
    }

    //*****************************************************************************

    void Ray_BreakableStackManagerAIComponent::ParticlesGenerator::addParticles( u32 _indexAtlas, Vec2d _dir, Vec2d _pos, f32 _scale /*= 1.0f*/ )
    {
             
        Particle newParticle;

        // Rotation dir
        f32 dotDir = _dir.dot(Vec2d::Right);
        if (dotDir > 0.5f && dotDir <= 1.0f)
        {
            newParticle.m_explodeToRight = btrue;
        }

        dotDir = _dir.dot(-Vec2d::Right);
        if (dotDir > 0.5f && dotDir <= 1.0f)
        {
            newParticle.m_explodeToLeft = btrue;
        }

        // Scale
        newParticle.m_scale = _scale;

        // Atlas
        newParticle.m_atlasIndex = _indexAtlas;

        // Set pos
        newParticle.m_startPos = _pos;
        newParticle.m_curPos   = _pos;

        // Set time life
        newParticle.m_time = 0.0f;
        newParticle.m_timeMax = PAR_LIFE_TIME;

        // Set Dir
        _dir.normalize();

        f32 randomPower = Seeder::getSharedSeeder().GetFloat(PAR_EXPULSE_POWER_MIN, PAR_EXPULSE_POWER_MAX);
        
        if (newParticle.m_explodeToRight)
        {
            _dir = _dir.Rotate(PAR_EXPULSE_Y_OFFSET);
        }
        else if (newParticle.m_explodeToLeft)
        {
            _dir = _dir.Rotate(-PAR_EXPULSE_Y_OFFSET);
        }

        f32 randomAngle = Seeder::getSharedSeeder().GetFloat(-PAR_EXPULSE_ANGLE, PAR_EXPULSE_ANGLE);
        _dir = _dir.Rotate(randomAngle);
        _dir *= randomPower;

        newParticle.m_dir = _dir;

        // Add particle in generator
        m_particles.push_back(newParticle);

    }

#ifdef _ITF_BSE_DEBUG_
    //*****************************************************************************
    void Ray_BreakableStackManagerAIComponent::Box::debugDisplayConnection( PolylineBox& _prev, PolylineBox& _next )
    {
        Vec2d centerPrev = _prev.m_points[0] + ((_prev.m_points[1] - _prev.m_points[0]) * 0.5f);
        Vec2d centerNext = _next.m_points[0] + ((_next.m_points[1] - _next.m_points[0]) * 0.5f);
        //DebugDraw::line2D(centerPrev, centerNext, 0.0f, Color::white(), 4.0f, 0.0f);

        DebugDraw::arrow2D(centerPrev, centerNext, 0.0f, Color::white(), 4.0f, 15.0f, 15.0f);
    } 
#endif // _ITF_BSE_DEBUG_

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_BreakableStackManagerAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_BreakableStackManagerAIComponent_Template)
        
        SERIALIZE_MEMBER("timeShakeBeforeFall",  m_timeShakeBeforeFall);
        SERIALIZE_MEMBER("countDownHit",         m_countDownHit);
        SERIALIZE_MEMBER("gravityBallistics",    m_gravityBallistics);
        SERIALIZE_MEMBER("timeExpulse",          m_timeExpulse);
        SERIALIZE_MEMBER("atlasPath",            m_atlasPath);
        SERIALIZE_MEMBER("atlasParticlesPath",   m_atlasParticlesPath);
       
        SERIALIZE_MEMBER("edgeSize",             m_edgeSize);
        SERIALIZE_MEMBER("faction",              m_faction);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent_Template::Ray_BreakableStackManagerAIComponent_Template()
    : m_timeShakeBeforeFall(0.3f)
    , m_countDownHit(0.5f)
    , m_gravityBallistics(-15.0)
    , m_timeExpulse(3.0)
    , m_edgeSize(1.0f)
    , m_faction(0)
    {
        // none
    }

    //*****************************************************************************

    Ray_BreakableStackManagerAIComponent_Template::~Ray_BreakableStackManagerAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************


};
