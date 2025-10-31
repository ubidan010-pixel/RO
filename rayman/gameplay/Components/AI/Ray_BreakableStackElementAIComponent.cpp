#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableStackElementAIComponent.h"
#endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BreakableStackElementAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BreakableStackElementAIComponent)
     
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_MEMBER("managerPath", m_managerPath);
        END_CONDITION_BLOCK()

        SERIALIZE_MEMBER("countSpawnMax", m_countSpawnMax);
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_MEMBER("blockIsDestroy",m_blockIsDestroy);
            SERIALIZE_MEMBER("checkPointRow", m_checkPointRow);
            SERIALIZE_MEMBER("checkPointCol", m_checkPointCol);
            SERIALIZE_MEMBER("blockState",m_blockState);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BreakableStackElementAIComponent)
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_BreakableStackElementAIComponent::Ray_BreakableStackElementAIComponent()
    : m_fxControllerComponent(NULL)
    , m_width(0)
    , m_height(0)
    , m_countSpawnMax(0)
    , m_countSpawn(0)
    , m_atlasOffset(1.0f / 6.0f)
    , m_edgeSize(1)
    , m_timer(0.0f)
    , m_instantSpawned(bfalse)
    , m_blockState(0)
    , m_blockIsDestroy(bfalse)
    , m_checkPointRow(U32_INVALID)
    , m_checkPointCol(U32_INVALID)
#ifdef ITF_SUPPORT_EDITOR 
	, m_countLums(0)
#endif
    {
        // none
    }

    //*****************************************************************************

    Ray_BreakableStackElementAIComponent::~Ray_BreakableStackElementAIComponent()
    {
        m_block.resetConnection();

        // Clean atlas
        m_atlasObject.clear(); 
        m_atlasParticles.clear();
    }
    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Register events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796), this);

        m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);

        // Get Components
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        // Get manager
        if (m_managerPath.isValid())
        {
            Pickable* pick = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, m_managerPath);
            if (pick)
            {
                m_managerRef = pick->getRef();
            }            
        }

        m_edgeSize = getTemplate()->getEdgeSize();

        // Init "Grid"
        m_width = getTemplate()->getWidth();
        m_height = getTemplate()->getHeight();
        m_offsetCenter.m_x = (m_edgeSize * m_width) * 0.5f;
        m_offsetCenter.m_y = (m_edgeSize * m_height) * 0.5f;

        // Init PhysShape
        FixedArray<Vec2d, 4> shapePoints;
        shapePoints.resize(4);
        f32 semiEdge = (m_edgeSize + OFFSET_COL_PHANTOM) * 0.5f; // Little more big than polyline
        shapePoints[0] = Vec2d(-semiEdge, -semiEdge);
        shapePoints[1] = Vec2d(-semiEdge, semiEdge);
        shapePoints[2] = Vec2d(semiEdge, semiEdge);
        shapePoints[3] = Vec2d(semiEdge, -semiEdge);
        m_physShapeBox.setPoints(shapePoints);


        // Init Atlas
        initAtlas();

        // Init Block
        initBlock();

    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::onBecomeActive()
     {
        Super::onBecomeActive();

        if (m_managerRef.isValid())
        {
            // In grid -> desactivate
            activateAllBox(bfalse);
        }
        else
        {
            // Out grid -> activate
            activateAllBox(btrue);
        }
 
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if (!m_managerRef.isValid())
        {
            // Out grid -> desactivate
            activateAllBox(bfalse);
        }
    }

    //*****************************************************************************


    u32 Ray_BreakableStackElementAIComponent::playFx(StringID _name, Vec3d _pos)
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

    void Ray_BreakableStackElementAIComponent::setFxPos(u32 _handle, Vec3d _pos)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->setFXPos(_handle,_pos);
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::setFxAngle(u32 _handle, f32 _angle)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->setFXAngle(_handle, _angle);
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::stopFX(u32 _handle)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->stopFX(_handle);
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::playSound(const StringID& _sound)
    {
        if (m_fxControllerComponent && _sound.isValid())
        {  
            m_fxControllerComponent->playFX(_sound);
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::initAtlas()
    {
        // Init Atlas textures
        Path pathAtlas = getTemplate()->getAtlasPath();
        if (!pathAtlas.isEmpty() && pathAtlas.getStringID().isValid())
        {
            m_atlasObject.initialize(m_actor->getResourceGroup(), getTemplate()->getAtlasPath(), m_width * m_height);
        }
        else
        {
            ITF_WARNING_CATEGORY(LD,m_actor, 0, "Atlas path is invalid !");
        }

        // Init Atlas particles textures
        Path pathParticleAtlas = getTemplate()->getAtlasParticlesPath();
        if (!pathParticleAtlas.isEmpty() && pathParticleAtlas.getStringID().isValid())
        {
            m_atlasParticles.initialize(m_actor->getResourceGroup(), getTemplate()->getAtlasParticlesPath(), (m_width * m_height) * PAR_COUNT_MAX_BY_BOX);
        }
        else
        {
            ITF_WARNING_CATEGORY(LD,m_actor, 0, "Atlas particles path is invalid !");
        }



    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);


        // Only one spawn (without trigger)
        if (getTemplate()->getInstantSpawn() && !m_instantSpawned && !m_blockIsDestroy)
        {
            printBlock();  
            m_instantSpawned = btrue;
            if (m_managerRef.isValid())
            {
                m_actor->disable();
            }
        }   


        // Global timer
        m_timer += _dt;

        // If this block is not in a grid
        if (!m_managerRef.isValid())
        {
            // Update Countdown Hit
            if (m_block.m_countDownHit > 0.0f)
            {
                m_block.m_countDownHit -= _dt;
            }

            // Update shaking hit
            if(m_block.m_shaking)
            {
                m_block.m_timeShake -= _dt;

                if (m_block.m_timeShake <= 0.0f )
                {
                    m_block.m_shaking = bfalse;
                }
            }

            if (getBlockState() == BLOCK_STATE_STATIC)
            {
                updatePolyline(_dt);
                updateParticles(_dt);
            }
            else if (getBlockState() == BLOCK_STATE_EXPULSE)
            {
                updatePosBallistics(_dt);
                updateParticles(_dt);
            }
            else if (getBlockState() == BLOCK_STATE_DISABLED)
            {
                m_actor->disable();
            }

            growAABB();
        }

    }


    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::updatePosBallistics( f32 _dt )
    {
        f32 m_gravity = getTemplate()->getGravityBallistics();
        f32 rotate = MTH_PI * 0.05f; // piBy20

        if ((m_block.m_timeExpulse += _dt) < getTemplate()->getTimeExpulse())
        {

            f32 progress = m_block.m_timeExpulse /  getTemplate()->getTimeExpulse();

            // update fade alpha
            m_block.m_alpha = 1.0f - progress;
            if (m_block.m_alpha < 0.0f)
            {
                m_block.m_alpha = 0.0f;
            }

            for (u32 indexRow = 0 + m_block.m_heightOffset; indexRow < m_block.m_listBox.size(); indexRow++)
            {
                ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

                for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
                {
                    Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                    if (curBox.m_isOccupied)
                    {                        
                        // Trajectory
                        curBox.m_drawRelativePos.m_x = curBox.m_startAbsoluteExpulsePos.m_x + curBox.m_impulse.m_x * m_block.m_timeExpulse;
                        curBox.m_drawRelativePos.m_y = curBox.m_startAbsoluteExpulsePos.m_y + curBox.m_impulse.m_y * m_block.m_timeExpulse + 0.5f * m_gravity * m_block.m_timeExpulse * m_block.m_timeExpulse;                    

                        // Rotate
                        if (m_block.m_explodeToRight)
                        {
                            curBox.m_angle -= rotate;
                        }
                        else
                        {
                            curBox.m_angle += rotate;
                        }

                        // Z
                        curBox.m_currentExpulseZ = curBox.m_targetExpulseZ * progress;

                    }
                }
            }
        }
        else
        {
            setBlockState(BLOCK_STATE_DISABLED);
        }

    }
   
    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::initBlock()
    {

		// Load GMat		
		if ( getTemplate()->getGmatPath().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
		{
			World::addGameMaterialFromFile(getTemplate()->getGmatPath(), m_actor->getResourceGroup());
		}

        // Init block
        m_block.Init(m_actor, this, 0, 0, m_edgeSize, getInfoElement(), getTemplate()->getFxData(), m_physShapeBox);

        // Fill block
        const InfoElementGrid& grid = getTemplate()->getGrid();

        for (u32 indexRow = 0; indexRow < m_block.m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                const  ITF_VECTOR<InfoElement>& rowGrid = grid[indexRow].m_elements;

                curBox.m_isOccupied = rowGrid[indexCol].m_isOccupied;
                curBox.m_reward     = rowGrid[indexCol].m_reward;

#ifdef ITF_SUPPORT_EDITOR 
				// Count Lums
				if (curBox.m_reward == 1)
				{
					m_countLums++;
				}
#endif

                // Phantom
                if (curBox.m_isOccupied)
                {
                    PhysPhantomInitCommonInfo initPhantom;

                    SET_OWNER(initPhantom, m_actor->getRef());
                    initPhantom.m_shape = &m_physShapeBox;
                    initPhantom.m_pos = m_actor->get2DPos() + curBox.m_drawRelativePos;
                    initPhantom.m_angle = m_actor->getAngle();
                    initPhantom.m_userData = m_actor->getRef();
                    initPhantom.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;

                    curBox.m_phantom = PHYSWORLD->allocPhantom(initPhantom);
                    curBox.m_phantom->updateAABB();
                    //PHYSWORLD->insertPhantom(curBox.m_phantom,m_actor->getDepth());
                    curBox.m_physWorldRegistred = bfalse;

                    // Animation
                    curBox.m_animation = rowGrid[indexCol].m_anim;
                    curBox.m_animationBreak = rowGrid[indexCol].m_animBreak;
                    curBox.m_animPlayer.init(&curBox.m_animation);

                    // Fragments / debris
                    curBox.m_fragments = rowGrid[indexCol].m_fragments;

                }
                else
                {
                    for (u32 edge = 0; edge < 4; edge++)
                    { 
                        curBox.m_procPolyList[edge].m_procPoly.deactivate();
                    }
                }
            }
        }      
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (EventTrigger* evtTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (!getTemplate()->getInstantSpawn())
            {
                printBlock();  
            }      
        }
        else if (HitStim* evtOnHit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
        {
            const PhysContactsContainer& contacts = evtOnHit->getContacts();
            const u32 contactCount = contacts.size();

            if (contactCount > 0)
            {
                const SCollidableContact& contact = contacts[0];

                if (contact.m_collidable.isValid())
                {                    
                    hitBlock(evtOnHit->getDirection(), evtOnHit->getLevel(), evtOnHit->getSender());
                }
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
#ifdef ITF_SUPPORT_EDITOR 
		else if ( Ray_EventQueryLumCount * eventCountLums = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)) )
		{        
			eventCountLums->addNormalLums(m_countLums);
		}
#endif


    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::flagDestroy()
    {
        m_blockIsDestroy = btrue;
    }
    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::saveCheckpointPos(u32 _row, u32 _col)
    {
        m_checkPointRow = _row;
        m_checkPointCol = _col;
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::sendEventToPolyline(Event* _event, ObjectRef _polylineRef)
    {
        for (u32 indexRow = 0; indexRow < m_block.m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
                    for (u32 edge = 0; edge < 4; edge++)
                    { 
                        ObjectRef polyRef = curBox.m_procPolyList[edge].m_procPoly.getPolyline().getRef();
                        if (_polylineRef == polyRef)
                        {
                            curBox.m_procPolyList[edge].m_procPoly.onEvent(_event);
                        }
                    }
                }
            }
        }
    }

    //*****************************************************************************

    const InfoHeadElement Ray_BreakableStackElementAIComponent::getInfoElement() const
    {
        InfoHeadElement info;

        info.m_width                = m_width;
        info.m_height               = m_height;
        info.m_hitPoint             = getTemplate()->getHitPoint();
        info.m_gmat                 = getTemplate()->getGmatPath();
        info.m_blockStatic          = getTemplate()->getBlockStatic();
        info.m_explosive            = getTemplate()->getExplosive();
        info.m_radiusExplosive      = getTemplate()->getRadiusExplosive();
        info.m_timeBeforeExplode    = getTemplate()->getTimeBeforeExplode();

        return info;
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::growAABB()
    {
        Vec2d vecGrow(m_edgeSize * m_width * 0.5f, m_edgeSize * m_height * 0.5f);

        AABB aabb(vecGrow);
        aabb.grow(-vecGrow);
        aabb.Rotate(m_actor->getAngle(), btrue);
        m_actor->growRelativeAABB(aabb);
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::updatePolyline( f32 _dt )
    {
		f32 depthActor = m_actor->getDepth();

        for (u32 indexRow = 0; indexRow < m_block.m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
        
                    // SetPos
                    Vec2d posA, posB, posC, posD;

                    posA.m_x = m_edgeSize * indexCol;
                    posA.m_y = m_edgeSize * indexRow;

                    posB.m_x = posA.m_x;
                    posB.m_y = posA.m_y + m_edgeSize;

                    posC.m_x = posA.m_x + m_edgeSize;
                    posC.m_y = posA.m_y + m_edgeSize;

                    posD.m_x = posA.m_x + m_edgeSize;
                    posD.m_y = posA.m_y;

                     // Center offset
                    posA -= m_offsetCenter;
                    posB -= m_offsetCenter;
                    posC -= m_offsetCenter;
                    posD -= m_offsetCenter;

                    // Rotation
                    Transform2d tranform(m_actor->get2DPos(), m_actor->getAngle());

                    posA = tranform.transformPos(posA);
                    posB = tranform.transformPos(posB);
                    posC = tranform.transformPos(posC);
                    posD = tranform.transformPos(posD);
                    
                    for (u32 edge = 0; edge < 4; edge++)
                    { 
                        Ray_BreakableStackManagerAIComponent::PolylineBox& polyBox = curBox.m_procPolyList[edge];

                        switch(edge)
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

                        // Update
                        polyBox.m_procPoly.getPolyline().update(_dt);
                    }

                    // Update phantom
                    if (curBox.m_phantom)
                    {
                        Vec2d posPhantom = tranform.transformPos(curBox.m_drawRelativePos - m_offsetCenter);
                        curBox.m_phantom->setPos(posPhantom);
                        curBox.m_phantom->setAngle(m_actor->getAngle());
                        curBox.m_phantom->updateAABB();
                    }
                }
            }
        }


        // Set connections and activate/desativate polylines
        for (u32 indexRow = 0; indexRow < m_block.m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
                    Ray_BreakableStackManagerAIComponent::NeighborBox neighborBox;

                    m_block.getLocalBoxNeighbor(indexRow, indexCol, neighborBox);

                    // Update activate / deactivate polylines
                    curBox.updateOnOffPolylineBox(neighborBox, depthActor);
                }
            }
        }


        // Update connection
        for (u32 indexRow = 0; indexRow < m_block.m_listBox.size(); indexRow++)
        {
            ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
                    Ray_BreakableStackManagerAIComponent::NeighborBox neighborBox;

                    m_block.getLocalBoxNeighbor(indexRow, indexCol, neighborBox);

                    // Update connections
                    curBox.updateConnectionBox(neighborBox);
                }
            }
        }



    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::printBlock()
    {
        if (m_countSpawnMax > 0 && m_countSpawn >= m_countSpawnMax)
        {
            return;
        }

        if (m_managerRef.isValid())
        {

            Actor* manager = m_managerRef.getActor();

            if (manager)
            {
                if (Ray_BreakableStackManagerAIComponent* managerCompo = manager->GetComponent<Ray_BreakableStackManagerAIComponent>())
                {
                    managerCompo->createBlocks(m_actor, m_checkPointRow, m_checkPointCol);
                    m_countSpawn++;
                }
            }
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::hitBlock(Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender)
    {

        // Unbreakable block !
        if (m_block.m_hitPoints <= 0)
        {
            return;
        }

        if (m_block.m_countDownHit <= 0.0f)
        {

            m_block.m_countDownHit = getTemplate()->getCountDownHit();

            switch(_hitLevel)
            {
            case 0:
                m_block.m_hitPoints -= 1;
                break;
            case 1:
                m_block.m_hitPoints -= 2;
                break;
            case 2:
                m_block.m_hitPoints -= 4;
                break;
            default:
                m_block.m_hitPoints -= 1;
                break;
            }

            for (u32 indexRow = 0 + m_block.m_heightOffset; indexRow < m_block.m_listBox.size(); indexRow++)
            {
                ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

                for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
                {
                    Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];
                    
                    curBox.m_animPlayer.init(&curBox.m_animationBreak);

                    if (curBox.m_isOccupied)
                    {

                        // Play FX
                        //Vec3d fxPos(m_actor->getPos() + curBox.m_drawRelativePos.to3d() - m_offsetCenter.to3d());
                        //curBox.m_handleFX = playFx(m_block.m_fxData.m_fxHit, fxPos);
                        //setFxAngle(curBox.m_handleFX, _dirHit.getAngle());


                        // Particles
                        FragmentsList& fragList = curBox.m_fragments;
                        u32 nbParticles = Min((u32)fragList.m_fragments.size(), (u32)PAR_COUNT_MAX_BY_BOX);

                        f32 angle = 0.0f;
                        if (nbParticles > 0)
                        {
                            angle = MTH_2PI / (f32)nbParticles;
                        }

                        f32 scale = 1.0f;
                        if (m_block.m_hitPoints > 0)
                        {
                            scale = PAR_SCALE_LITTLE_FRAG;
                        }

                        for (u32 i = 0; i < nbParticles; i++)
                        {

                            // Circle dispos
                            Vec2d offsetRadius = Vec2d::Right * (m_edgeSize * 0.5f);
                            offsetRadius = offsetRadius.Rotate((f32)i * angle);                            

                            curBox.m_generator.addParticles(fragList.m_fragments[i], _dirHit, m_actor->get2DPos() + curBox.m_drawRelativePos + offsetRadius, scale);
                        }  

                    }

                }
            }

            // Block is dead
            if (m_block.m_hitPoints <= 0)
            {
                destroyBlock(_dirHit, _hitLevel, _sender);
            }
            else
            {
                // Play sound
                playSound(m_block.m_fxData.m_soundHit);

                m_block.m_shaking = btrue;
                m_block.m_timeShake = TIME_SHAKE_ON_HIT;
            }
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::destroyBlock(Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender)
    {

        if (m_managerRef.isValid())
        {
            return;
        }

        f32 dotImpulse = _dirHit.dot(Vec2d::Right);
        if (dotImpulse > 0.0f && dotImpulse <= 1.0f)
        {
            m_block.m_explodeToRight = btrue;
        }

        for (u32 indexRow = 0; indexRow < m_block.m_listBox.size(); indexRow++)
        {
         ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

             for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
             {
                 Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                 if (curBox.m_isOccupied)
                 {
                    Vec2d expulsePos = curBox.m_drawRelativePos - m_offsetCenter;

                    if (curBox.m_reward > 0)
                    {
                        m_block.spawnReward(m_actor, _sender, curBox.m_drawRelativePos.to3d(), curBox.m_reward);
                    }

                    // Rotation
                    Transform2d tranform(m_actor->getBoundWorldInitialPos().truncateTo2D(), m_actor->getAngle());
                    expulsePos = tranform.transformPos(expulsePos) ;
                    curBox.m_startAbsoluteExpulsePos = expulsePos;

                    // Force impulse
                    curBox.m_impulse.m_x = _dirHit.m_x;
                    curBox.m_impulse.m_y = _dirHit.m_y;

                    // random target Z
                    curBox.m_targetExpulseZ = Seeder::getSharedSeeder().GetFloat(EXPULSE_MIN_RANDOM_Z, EXPULSE_MAX_RANDOM_Z);

                    // random angle
                    f32 randomAngle = Seeder::getSharedSeeder().GetFloat(-EXPULSE_RANDOM_ANGLE, EXPULSE_RANDOM_ANGLE);
                    curBox.m_impulse = curBox.m_impulse.Rotate(randomAngle);

                    // random power
                    curBox.m_impulse *= Seeder::getSharedSeeder().GetFloat(0.0f, EXPULSE_RANDOM_POWER);

                    curBox.desactiveBox();

                 }
             }
        }

        // Play sound
        playSound(m_block.m_fxData.m_soundDestroy);

        setBlockState(BLOCK_STATE_EXPULSE);
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::drawAtlas() const
    {
        f32 offset = m_edgeSize * m_atlasOffset; //0.1666666666f;
        f32 semiEdge = m_edgeSize * 0.5f;
        Vec2d actPos = m_actor->getBoundWorldInitialPos().truncateTo2D();

        // Draw Box atlas
        for (u32 indexRow = 0; indexRow < m_block.m_listBox.size(); indexRow++)
        {
            const ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

            for(i32 indexCol = m_width - 1; indexCol >= 0; indexCol--)
            {
                const Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {
                    Vec2d pos = curBox.m_drawRelativePos;
                    
                    Vec2d p1 = pos;
                    p1.m_x += -semiEdge;
                    p1.m_y += semiEdge;
                    p1.m_x += -offset;
                    p1.m_y += offset;

                    Vec2d p2 = pos;
                    p2.m_x += semiEdge;
                    p2.m_y += semiEdge;
                    p2.m_x += offset;
                    p2.m_y += offset;

                    Vec2d p3 = pos;
                    p3.m_x += semiEdge;
                    p3.m_y += -semiEdge;
                    p3.m_x += offset;
                    p3.m_y += -offset;

                    Vec2d p4 = pos;
                    p4.m_x += -semiEdge;
                    p4.m_y += -semiEdge;
                    p4.m_x += -offset;
                    p4.m_y += -offset;


                    if (m_block.m_shaking)
                    {
                        // Shake visual
                        Vec2d vecShake = getShake(0.01f, 40.0f, 0.06f, 58.0f, btrue);
                        p1 += vecShake;
                        p2 += vecShake;
                        p3 += vecShake;
                        p4 += vecShake;  
                    }


                    if (m_managerRef.isValid())
                    {
                        m_atlasObject.addQuad(curBox.m_animPlayer.getCurAtlas(), actPos + p1, actPos + p4, actPos + p3, actPos + p2); 
                    }
                    else
                    {
                        if (getBlockState() == BLOCK_STATE_EXPULSE)
                        {
                            // Expulse rotation
                            p1 = p1.RotateAround(curBox.m_drawRelativePos, curBox.m_angle );
                            p2 = p2.RotateAround(curBox.m_drawRelativePos, curBox.m_angle );
                            p3 = p3.RotateAround(curBox.m_drawRelativePos, curBox.m_angle );
                            p4 = p4.RotateAround(curBox.m_drawRelativePos, curBox.m_angle );

                            Color alphaColor(m_block.m_alpha, 1.0f, 1.0f, 1.0f);                        
                            m_atlasObject.addQuad(curBox.m_animPlayer.getCurAtlas(), p1, p4, p3, p2, curBox.m_currentExpulseZ , alphaColor.getAsU32());  
                        }
                        else
                        {
                            // Center offset
                            p1 -= m_offsetCenter;
                            p2 -= m_offsetCenter;
                            p3 -= m_offsetCenter;
                            p4 -= m_offsetCenter;

                            // Rotation
                            Transform2d tranform(m_actor->get2DPos(), m_actor->getAngle());
                            m_atlasObject.addQuad(curBox.m_animPlayer.getCurAtlas(), tranform.transformPos(p1) , tranform.transformPos(p4) ,tranform.transformPos(p3) , tranform.transformPos(p2) );  
                        }

                        drawParticles(&curBox);
                    }   
                    
                    curBox.m_animPlayer.update();
                }
            }
        }

        m_atlasObject.draw(m_actor->getDepth());
        m_atlasParticles.draw(m_actor->getDepth( ) + 0.1f);
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::updateParticles( f32 _dt )
    {

        for(u32 indexRow = 0 + m_block.m_heightOffset; indexRow < m_block.m_height; indexRow++)
        {
            ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& row = m_block.m_listBox[indexRow];

            for(u32 indexCol = 0; indexCol < m_block.m_width; indexCol++)
            { 
                Ray_BreakableStackManagerAIComponent::Box& curBox = row[indexCol];

                if (curBox.m_isOccupied)
                {
                    curBox.m_generator.update(_dt);
                }
            }
        }
        
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::drawParticles(const Ray_BreakableStackManagerAIComponent::Box* _box) const
    {

        for(u32 i = 0; i < _box->m_generator.m_particles.size(); i++)
        {
            const Ray_BreakableStackManagerAIComponent::ParticlesGenerator::Particle& par = _box->m_generator.m_particles[i];

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

            // Center offset
            p1 -= m_offsetCenter;
            p2 -= m_offsetCenter;
            p3 -= m_offsetCenter;
            p4 -= m_offsetCenter;

            // Alpha
            Color alphaColor(par.m_alpha, 1.0f, 1.0f, 1.0f);  

            Transform2d tranform(m_actor->get2DPos(), m_actor->getAngle());
            //m_atlasParticles.addQuad(par.m_atlasIndex, tranform.transformPos(p1), tranform.transformPos(p4), tranform.transformPos(p3), tranform.transformPos(p2), 0.0f, alphaColor.getAsU32());    
            m_atlasParticles.addQuad(par.m_atlasIndex, p1, p4, p3, p2, 0.0f, alphaColor.getAsU32());    

        }        

    }

    //*****************************************************************************

    Vec2d Ray_BreakableStackElementAIComponent::getShake(f32 _intensX, f32 _freqX, f32 _intensY, f32 _freqY, bbool _dephase /*= bfalse*/) const
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

    void Ray_BreakableStackElementAIComponent::Draw()
    {
        Super::Draw();

        // We are not in a valid grid
        if (!m_managerRef.isValid())
        {
            drawAtlas();
        }
    }

    //*****************************************************************************

    void Ray_BreakableStackElementAIComponent::activateAllBox(bbool _active)
    {
		f32 depthActor = m_actor->getDepth();

        for(u32 indexRow = 0 + m_block.m_heightOffset; indexRow < m_block.m_height; indexRow++)
        {
            ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& row = m_block.m_listBox[indexRow];

            for(u32 indexCol = 0; indexCol < m_block.m_width; indexCol++)
            { 
                Ray_BreakableStackManagerAIComponent::Box& curBox = row[indexCol];

                if (curBox.m_isOccupied)
                {
                    if (_active)
                    {
                        curBox.activeBox(m_actor->getDepth());

                        // Update activate / deactivate polylines
                        Ray_BreakableStackManagerAIComponent::NeighborBox neighborBox;
                        m_block.getLocalBoxNeighbor(indexRow, indexCol, neighborBox);
                        curBox.updateOnOffPolylineBox(neighborBox, depthActor);

                    }
                    else
                    {
                        curBox.desactiveBox();
                    }
                }
            }
        }
    }

    //*****************************************************************************

    const InfoElementGrid& Ray_BreakableStackElementAIComponent::getGridElement() const
    {
        return getTemplate()->getGrid();
    }

    //*****************************************************************************

    const FxData& Ray_BreakableStackElementAIComponent::getFxData() const
    {
        return getTemplate()->getFxData();
    }

    void Ray_BreakableStackElementAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
        if (m_blockState == BLOCK_STATE_DISABLED)
            m_block.m_state = BLOCK_STATE_DISABLED;
    }

    void Ray_BreakableStackElementAIComponent::setBlockState( u32 _state )
    {
        m_block.m_state = _state;
        m_blockState = _state;
    }



#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_BreakableStackElementAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (!(_flags & ActorComponent::DrawEditFlag_Collisions))
            return;

        if (m_managerRef.isValid())
        {
            drawAtlas();
        }

        // Draw debugLine + polyline
        Vec2d originPos = m_actor->getBoundWorldInitialPos().truncateTo2D();

        for (u32 indexRow = 0; indexRow < m_block.m_listBox.size(); indexRow++)
        {
            const ITF_VECTOR<Ray_BreakableStackManagerAIComponent::Box>& curRow = m_block.m_listBox[indexRow];

            for (u32 indexCol = 0; indexCol < curRow.size(); indexCol++)
            {
                const Ray_BreakableStackManagerAIComponent::Box& curBox = curRow[indexCol];

                if (curBox.m_isOccupied)
                {

                    if (!m_managerRef.isValid())
                    {
                        for (u32 edge = 0; edge < 4; edge++)
                        { 
                            const Ray_BreakableStackManagerAIComponent::PolylineBox& curPolylineBox = curBox.m_procPolyList[edge];
                            curPolylineBox.m_procPoly.drawEdit(_drawInterface, _flags);
                        }
                    }

                    Vec2d posA, posB, posC, posD;

                    posA.m_x = m_edgeSize * indexCol;
                    posA.m_y = m_edgeSize * indexRow;

                    posB.m_x = posA.m_x;
                    posB.m_y = posA.m_y + m_edgeSize;

                    posC.m_x = posA.m_x + m_edgeSize;
                    posC.m_y = posA.m_y + m_edgeSize;

                    posD.m_x = posA.m_x + m_edgeSize;
                    posD.m_y = posA.m_y;

                    if (m_managerRef.isValid())
                    {
                        posA += originPos;
                        posB += originPos;
                        posC += originPos;
                        posD += originPos;
                    }
                    else
                    {
                        // Center offset
                        posA -= m_offsetCenter;
                        posB -= m_offsetCenter;
                        posC -= m_offsetCenter;
                        posD -= m_offsetCenter;

                        // Rotation
                        Transform2d tranform(m_actor->get2DPos(), m_actor->getAngle());

                        posA = tranform.transformPos(posA);
                        posB = tranform.transformPos(posB);
                        posC = tranform.transformPos(posC);
                        posD = tranform.transformPos(posD);

                    }

                    Color colorLine;
                    if (m_managerRef.isValid())
                    {
                        colorLine = Color::green();
                    }
                    else
                    {
                        colorLine = Color::red();
                    }

                    DebugDraw::line2D(posA,  posB, m_actor->getDepth(), colorLine, 4.0f); 
                    DebugDraw::line2D(posB,  posC, m_actor->getDepth(), colorLine, 4.0f); 
                    DebugDraw::line2D(posC,  posD, m_actor->getDepth(), colorLine, 4.0f); 
                    DebugDraw::line2D(posD,  posA, m_actor->getDepth(), colorLine, 4.0f);

                }

                // Phantom
                //if (curBox.m_phantom)
                //DebugDraw::shape(curBox.m_phantom->getPos(), m_actor->getDepth(), curBox.m_phantom->getAngle(), curBox.m_phantom->getShape());  
            }
        }
    }

    //*****************************************************************************
    void Ray_BreakableStackElementAIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        // Get inner manager
        const Actor* manager = Ray_BreakableStackManagerAIComponent::getManager(m_actor->get2DPos());

        if (manager)
        {
            m_managerRef = manager->getRef();
            SceneObjectPathUtils::getRelativePathFromObject(m_actor, manager, m_managerPath);

            // Reset aabb
            AABB aabb(Vec2d::Zero, Vec2d::Zero);
            m_actor->changeRelativeAABB(aabb);

            // Desactivate polyline + phantoms
            activateAllBox(bfalse);

            // Snap on grid
            Vec2d snapPos;
            if (Ray_BreakableStackManagerAIComponent* managerCompo = manager->GetComponent<Ray_BreakableStackManagerAIComponent>())
            {
                snapPos = managerCompo->getSnapPos(m_actor->get2DPos());
                m_actor->set2DPos(snapPos);
                m_actor->setWorldInitialPos(snapPos.to3d());
            }


        }
        else
        {
            m_managerRef.invalidate();
            m_managerPath.invalidate();
            activateAllBox(btrue);
        }
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_BreakableStackElementAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_BreakableStackElementAIComponent_Template)

        SERIALIZE_CONTAINER_OBJECT("grid", m_grid);
        SERIALIZE_MEMBER("width",  m_width);
        SERIALIZE_MEMBER("height", m_height);
        SERIALIZE_OBJECT("fxData", m_fxData);

        SERIALIZE_MEMBER("gmatPath",  m_gmatPath);
        SERIALIZE_MEMBER("hitPoint",  m_hitPoint);
        SERIALIZE_MEMBER("atlasPath", m_atlasPath);
        SERIALIZE_MEMBER("atlasParticlesPath", m_atlasParticlesPath);

        // Standalone Block
        SERIALIZE_MEMBER("countDownHit",         m_countDownHit);
        SERIALIZE_MEMBER("gravityBallistics",    m_gravityBallistics);
        SERIALIZE_MEMBER("timeExpulse",          m_timeExpulse);
        SERIALIZE_MEMBER("edgeSize",             m_edgeSize);

        SERIALIZE_MEMBER("blockStatic",          m_blockStatic);
        SERIALIZE_MEMBER("instantSpawn",         m_instantSpawn);

        // Explosive
        SERIALIZE_MEMBER("explosive",            m_explosive);
        SERIALIZE_MEMBER("radiusExplosive",      m_radiusExplosive);
        SERIALIZE_MEMBER("timeBeforeExplode",    m_timeBeforeExplode);


    END_SERIALIZATION()

    //*****************************************************************************

    Ray_BreakableStackElementAIComponent_Template::Ray_BreakableStackElementAIComponent_Template()
    : m_hitPoint(2)
    , m_countDownHit(0.5f)
    , m_gravityBallistics(-15.0)
    , m_timeExpulse(3.0)
    , m_edgeSize(1)
    , m_width(0)
    , m_height(0)
    , m_blockStatic(bfalse)
    , m_explosive(bfalse)
    , m_radiusExplosive(1.0f)
    , m_timeBeforeExplode(0.0f)
    , m_instantSpawn(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_BreakableStackElementAIComponent_Template::~Ray_BreakableStackElementAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************


};
