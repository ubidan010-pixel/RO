#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERVACUUMSKILL_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterVacuumSkill.h"
#endif //_ITF_RAY_SHOOTERVACUUMSKILL_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_
#include "rayman\gameplay\Components\Shooter\Ray_ShooterActorParameterComponent.h"
#endif //_ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_


namespace ITF
{
    
	//------------------------------------------------------------------------------------------------------------------------
	VacuumSkill::VacuumSkill()
		: m_actor(NULL)
		, m_animComponent(NULL)
		, m_vacuumActive(bfalse)
		, m_instantVacuumActive(bfalse)
        , m_template(NULL)
		, m_stackFillValue( 0 )
		, m_totalVaccumedSizeFromStart( 0 )
		, m_fillValueHasChanged( bfalse )
        , m_actorsBeingVacuumedFillValue(0)
        , m_powerFactor( 1.0f )
        , m_vacuumWidth( 0.0f )
	{
        m_specificProjectiles.resize( 10 );
	}

	//------------------------------------------------------------------------------------------------------------------------
	VacuumSkill::~VacuumSkill()
	{
        if ( m_actor != NULL )
        {
            PHYSWORLD->removeWindForce(m_actor->getRef(),&m_vacuumForce);
        }	
    }

	//------------------------------------------------------------------------------------------------------------------------
	void VacuumSkill::initialize( Actor * _parentActor, AnimatedComponent* _animComponent, const VacuumSkill_Template* _template )
	{
		m_actor = _parentActor;
		m_animComponent = _animComponent;
        m_template = _template;

		m_vacuumForce.setTemplate( m_template->getVacuumForcePtr() );

		// stack size must be greater than stack full size
		//ITF_ASSERT( m_template->getStackFullSize() <= m_template->getStackSize() );
    }

    //------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::reset( bbool _destroyObj /*= bfalse*/ )
    {
        if ( _destroyObj )
        {
            for ( u32 i = 0; i < m_stackedObjects.size(); ++i )
            {
                stackRemoveAt( i , btrue );
				i--;
            }
        }

        m_actorsBeingVacuumed.clear();
        m_actorsBeingVacuumedFillValue = 0;
        m_stackedObjects.clear();
        m_specificProjectiles.fill( ObjectRef::InvalidRef );  // should destroy ??
        m_stackFillValue = 0;
        m_totalVaccumedSizeFromStart = 0;
        m_fillValueHasChanged = bfalse;
        m_vacuumActive = bfalse;
        m_instantVacuumActive = bfalse;
    }
	//------------------------------------------------------------------------------------------------------------------------
	// return true if the given actor re is an actor currently managed by the skill
	//
	bbool VacuumSkill::isVacuumedActor( ActorRef const & _actor )
	{
		if ( m_specificProjectiles.find( _actor ) >= 0 )
			return btrue;
		
		for (u32 i = 0; i < m_actorsBeingVacuumed.size(); ++i)
		{
			if ( m_actorsBeingVacuumed[i].m_actor == _actor )
			{
				return btrue;
			}
		}
		return bfalse;
	}

	//------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::sendVacuumStim()
    {
        if ( m_vacuumForce.getPolyline().m_polygon.isValid() )
        {
            VacuumStim* vacuumStim = STIMSMANAGER->requestStim<VacuumStim>(&m_vacuumForce.getPolyline().m_polygon);

            if ( vacuumStim )
            {
                vacuumStim->setPos(m_actor->get2DPos());
                vacuumStim->setPrevPos(m_actor->get2DPos());
                vacuumStim->setSender(m_actor->getRef());
                vacuumStim->setAngle(m_actor->getAngle());
                vacuumStim->setDepth(m_actor->getDepth());
                vacuumStim->setDir((-Vec2d::Right).Rotate(m_actor->getAngle()));
				
				vacuumStim->setVacuumDist( m_vacuumWidth );
				vacuumStim->setVacuumSpeedFactor( m_template->getVaccumSpeedFactor() * m_powerFactor );
				if ( m_instantVacuumActive )
				{
					vacuumStim->setVacuumSpeedFactor( 10.0f );
				}

                STIMSMANAGER->sendStim(vacuumStim);
            }
        }
    }

	//------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::addVacuum()
    {
        if (!m_vacuumActive)
        {
            m_vacuumActive = btrue;
			m_totalVaccumedSizeFromStart = 0;
            PHYSWORLD->addWindForce(m_actor->getRef(),&m_vacuumForce,m_actor->getDepth());
        }
    }

	//------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::removeVacuum()
    {
        if (m_vacuumActive)
        {
            m_vacuumActive = bfalse;
            PHYSWORLD->removeWindForce(m_actor->getRef(),&m_vacuumForce);
        }

		setInstantVacuum( bfalse );
    }

	//------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::updateVacuumShape()
    {
        m_vacuumWidth = 0.0f;

        if ( m_template->getVacuumForce().getPolyline().m_animId.isValid() )
        {
            u32 numPoints;
            const Vec2d* points = m_animComponent->getCurrentPolylinePointBuffer(m_template->getVacuumForce().getPolyline().m_animId,&numPoints);
            PhysForceModifier_Instance::PolylineData& polyData = m_vacuumForce.getPolyline();
            
           if ( points )
           {
                FixedArray <Vec2d,15> polyPoints;
                Vec2d p;
                const Vec2d& pos2d = m_actor->get2DPos();
                f32 angle = m_actor->getAngle();
                bbool flipped = m_actor->isFlipped();

                polyPoints.resize(numPoints);

                bbool firstPoint = btrue;
                i32 increm = flipped ? -1 : 1;
                i32 startVal = flipped ? static_cast<i32>(numPoints) - 1 : 0;

                for ( i32 i = startVal; i >= 0 && i < static_cast<i32>(numPoints); i += increm )
                {
                    p = ( points[i] - pos2d ).Rotate(-angle);
                    p.m_x *= m_powerFactor;
                    
                    polyPoints[i] = p;
                    if (firstPoint)
                    {
                        polyData.m_aabb.setMin(p);
                        polyData.m_aabb.setMax(p);
                        firstPoint = bfalse;
                    }
                    else
                    {
                        polyData.m_aabb.grow(p);
                    }
                }
                polyData.m_polygon.setPoints(polyPoints);

                m_vacuumWidth = polyData.m_aabb.getWidth();
#ifndef ITF_FINAL
				//m_vacuumForce.drawDebug( m_actor->get2DPos(), m_actor->getAngle(), m_actor->getDepth() );
#endif // ITF_FINAL
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------------
    Vec2d	VacuumSkill::getVacuumDirectionAndPos( Vec2d & _res_pos ) const
    { 
        const PhysForceModifier_Instance::PolylineData& polyData = m_vacuumForce.getPolyline();
        const PhysShapePolygon::PointsList& ptlist = polyData.m_polygon.getVertex();
        
        _res_pos = m_actor->get2DPos();

        // concider 2 first points as the start of the vacuum box
        //
        Vec2d avgDir;
        if ( ptlist.size() == 4 )
        {
            _res_pos += ( ptlist[0] + ( ( ptlist[3] - ptlist[0] ) * 0.5f ) ).Rotate( m_actor->getAngle() );
            avgDir = ( ( ptlist[1] - ptlist[0] ) + ( ptlist[2] - ptlist[3] ) ) * 0.5f;
            return (avgDir.normalize()).Rotate( m_actor->getAngle() );
        }        
        return Vec2d::Zero;
    }

	//------------------------------------------------------------------------------------------------------------------------
	// Free space wanted size in the stack from back to front by default.
	// In lifoMode the space is free from front to back
	void VacuumSkill::stackFreeSpace( const u32 & _sizeToReserve )
	{
		const u32 stackSize = m_template->getStackSize();

		i32 neededSpace = i32( _sizeToReserve );
		neededSpace -= ( stackSize - m_stackFillValue );
		neededSpace = Max( 0, neededSpace );

		if ( neededSpace > 0 )
		{
			i32 startIdx = m_stackedObjects.size() - 1;
			i32 incremVal = -1;
			if ( m_template->isStackInLifoMode() )
			{
				startIdx = 0;
				incremVal = 1;
			}						

			for ( i32 i = startIdx; ( i >= 0 && i < i32( m_stackedObjects.size() ) ) ; i += incremVal )
			{
				StackedObject & arrayStackedObj = m_stackedObjects[i];

				m_stackFillValue -= arrayStackedObj.m_stackSize;
				neededSpace -= arrayStackedObj.m_stackSize;
				neededSpace = Max( 0, neededSpace );

				// Crash previous object
				//
				if ( !arrayStackedObj.m_isGenerator )
				{
					Actor * actor = m_specificProjectiles[arrayStackedObj.m_arrayIndex].getActor();
					if ( actor )
					{
						if (actor->isSerializable())
						{
							actor->disable();
						}
						else
						{
							actor->requestDestruction();
						}
					}						
                    removeSpecificProjectileAt( arrayStackedObj.m_arrayIndex );
				}
				m_stackedObjects.eraseKeepOrder(i);
				if ( incremVal > 0 )
				{
					i -= incremVal;
				}

				// no more needed space
				if ( neededSpace == 0 )
				{
					break;
				}
			}
			ITF_ASSERT_MSG( neededSpace == 0, "## VacuumSkill::stackFreeSpace ## Can't free enought space in stack..\n Obj reserved size too big ?" );
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::stackObject( StackedObject & _stackedObj )
    {
		// free space
		stackFreeSpace( _stackedObj.m_stackSize );
		
		// Put object in the stack 
		m_stackedObjects.push_back( _stackedObj );
		m_stackFillValue += _stackedObj.m_stackSize;
		m_totalVaccumedSizeFromStart += _stackedObj.m_stackSize;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void VacuumSkill::stackPop( StackedObject & _popedObj )
	{
		if ( m_stackedObjects.size() > 0 )
		{
			_popedObj = m_stackedObjects.back();
		    m_stackFillValue -= _popedObj.m_stackSize;
		    m_stackedObjects.pop_back();
		}
	}
	//------------------------------------------------------------------------------------------------------------------------
	void VacuumSkill::stackRemoveAt( const u32 & _idx, bbool _destroyObj /*=bfalse*/ )
	{
		ITF_ASSERT( _idx < m_stackedObjects.size() );
		if ( _idx < m_stackedObjects.size() )
		{
			m_stackFillValue -= m_stackedObjects[_idx].m_stackSize;

            if ( _destroyObj  )
            {
                const StackedObject & stackedObj = m_stackedObjects[_idx];
                if ( /*!stackedObj.m_isGenerator && */( stackedObj.m_arrayIndex < m_specificProjectiles.size() ) )
                {
                    Actor * actor = m_specificProjectiles[stackedObj.m_arrayIndex].getActor();
                    if ( actor )
                    {
                        if (actor->isSerializable())
                        {
                            actor->disable();
                        }
                        else
                        {
                            actor->requestDestruction();
                        }
                    }						
                    removeSpecificProjectileAt( stackedObj.m_arrayIndex );
                }
            }

			m_stackedObjects.eraseKeepOrder( _idx );
		}
	}
	//------------------------------------------------------------------------------------------------------------------------
	i32 VacuumSkill::stackFindObjIdx( const u32 & _arrayIdx, bbool _isSpecProjectile )
	{
		for ( u32 i = 0; i < m_stackedObjects.size(); ++i )
		{
			const StackedObject & stackedObj = m_stackedObjects[i];
			if ( _arrayIdx == stackedObj.m_arrayIndex )
			{
				if ( ( _isSpecProjectile && !stackedObj.m_isGenerator ) || ( !_isSpecProjectile && stackedObj.m_isGenerator ) )
				{
					return i;
				}
			}			
		}
		return -1;
	}
    //------------------------------------------------------------------------------------------------------------------------
    i32 VacuumSkill::stackFindObjTypeIndex( const StringID & _stackData_Type )
    {
        for ( u32 i = 0; i < m_stackedObjects.size(); ++i )
        {
            const StackedObject & stackedObj = m_stackedObjects[i];
            if ( stackedObj.m_type == _stackData_Type )
            {
                return i;
            }			
        }
        return -1;
    }

	//------------------------------------------------------------------------------------------------------------------------
	u32 VacuumSkill::getBiggestObjSize() const
	{
		u32 biggestSize = 0;  
		for( u32 i = 0; i < m_stackedObjects.size(); ++i )
		{
			const StackedObject & arrayStackedObj = m_stackedObjects[i];
			if ( arrayStackedObj.m_stackSize > biggestSize )
			{
				biggestSize = arrayStackedObj.m_stackSize;
			}
		}
		return biggestSize;
	}

	//------------------------------------------------------------------------------------------------------------------------
	u32 VacuumSkill::addSpecificProjectile( Actor * _actor )
	{
		if ( _actor == NULL )
		{
			return U32_INVALID;
		}
        i32 freeIdx = m_specificProjectiles.find( ObjectRef::InvalidRef );
        if ( freeIdx >= 0 )
        {
            m_specificProjectiles[freeIdx] = _actor->getRef();
        }
        else
        {
            ITF_ASSERT( 0 ); // no more space ?
            return U32_INVALID;
        }
        
		//actor->disable();
		PhysComponent * physComp = _actor->GetComponent<PhysComponent>();
		if ( physComp != NULL )
		{
			physComp->resetForces();
			physComp->setSpeed( Vec2d::Zero );
		}

		return freeIdx;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void VacuumSkill::removeSpecificProjectileAt( const u32 & _idx )
	{
		ITF_ASSERT( _idx < m_specificProjectiles.size() );

		if ( _idx < m_specificProjectiles.size() )
		{
            m_specificProjectiles[_idx] = ITF_INVALID_OBJREF;
			//m_specificProjectiles.eraseKeepOrder( _idx );
		}
	}
	//------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::processSwallowed( VacuumedActor & _vacuumedActor, bbool _finished /*=bfalse*/ )
    {
        if ( _vacuumedActor.m_hasBeenSwallowed )
            return;

		//check if there is a projectile for this actor
        Actor * actor = _vacuumedActor.m_actor.getActor();
        if (actor)
        {
            bbool swallowedEffective = btrue;
			m_fillValueHasChanged = btrue;
            const Ray_ShooterActorParameter_StackData * stackData = getActorStackData( actor );
            
			StackedObject stackObj;
            bbool destroyObj	= bfalse;
            bbool disableObj	= btrue;

            // object not found : is a specific projectile
			//----------------------------------------------
            if ( ( stackData == NULL ) || ( stackData->m_inStackSize == U32_INVALID ) )
            {
                if ( !RAY_GAMEMANAGER->isPlayerActor( _vacuumedActor.m_actor ) )
                {
				    stackObj.m_isGenerator = bfalse;
				    stackObj.m_arrayIndex = addSpecificProjectile( actor );
				    stackObject( stackObj );
                }
                else
                {
                    EventShow show( 1.0f, m_template->getStickDuration() );
                    actor->onEvent(&show);
                    // don not send swallowed to player if the finished is forced 
                    if ( _finished && ( ( m_template->getStickDuration() - _vacuumedActor.m_timer ) > ( m_template->getStickDuration() * 0.5f ) ) )
                    {
                        swallowedEffective = bfalse;
                    }
                    disableObj = bfalse;
                }
            }
			// object found
            else
            {
				u32 biggestObjSize	= getBiggestObjSize();
				u32 freeSpace		= Max( i32( m_template->getStackSize() - m_stackFillValue ), 0 );
				
				// save stacked obj data
				stackObj.m_isGenerator	    = stackData->needProjectileSpawnee();
				stackObj.m_stackSize	    = stackData->m_inStackSize;
                stackObj.m_type             = stackData->m_type;
				if ( !stackObj.m_isGenerator )
				{
                    if ( stackData->m_max > 0 )  // only one for moment 
                    {
                        i32 sameDataTypeIdx = stackFindObjTypeIndex( stackData->m_type );
                        while( sameDataTypeIdx >= 0 )
                        {
                            stackRemoveAt( (u32)sameDataTypeIdx, btrue );
                            sameDataTypeIdx = stackFindObjTypeIndex( stackData->m_type );
                        }
                    }

					// enough space or obj bigger than bigger one
					if ( !stackData->m_notStackable && 
                         ( ( freeSpace >= stackObj.m_stackSize ) || ( stackObj.m_stackSize >= biggestObjSize ) || m_template->isStackInLifoMode() )
                       )
					{
						stackObj.m_arrayIndex = addSpecificProjectile( actor );
						stackObject( stackObj );
					}
					else if ( !stackData->m_notStackableKeepAlive )
					{
						destroyObj = btrue;
					}
				}
				else
				{
					destroyObj = btrue;
					u32 addProjNb = stackData->m_numProjectiles;
										
					if ( ( freeSpace >= stackObj.m_stackSize ) || ( stackObj.m_stackSize >= biggestObjSize ) || m_template->isStackInLifoMode() )
					{
                        if ( stackData->m_max > 0 )  // only one for moment 
                        {
                            i32 sameDataTypeIdx = stackFindObjTypeIndex( stackData->m_type );
                            while( sameDataTypeIdx >= 0 )
                            {
                                stackRemoveAt( (u32)sameDataTypeIdx, btrue );
                                sameDataTypeIdx = stackFindObjTypeIndex( stackData->m_type );
                            }
                        }

						if ( !m_template->isStackInLifoMode() )
						{
							addProjNb = Min( stackData->m_numProjectiles, freeSpace / stackObj.m_stackSize );
						}

                        // Add the vacuumed obj in the array anyway, to allow projectile generation when shoot requested
                        stackObj.m_arrayIndex = addSpecificProjectile( actor );
                        destroyObj = bfalse;

						// object generates multi projectiles
						stackFreeSpace( addProjNb * stackObj.m_stackSize );
						for ( u32 i = 0; i < addProjNb; ++i )
						{
                            stackObject( stackObj );
						}
					}
				}				
            }

            if ( swallowedEffective )
            {
                _vacuumedActor.m_hasBeenSwallowed = btrue;

                if ( destroyObj )
                {
                    if (actor->isSerializable())
                        actor->disable();
                    else
                    {
                        actor->requestDestruction();
                    }
                }
                // disable the actor anyway to avoid playing bhv
                else if ( disableObj )
                {
                    actor->disable();
                }

                Ray_EventShooterSwallow eventSwallow;
                eventSwallow.setSender( m_actor->getRef() );
                actor->onEvent( &eventSwallow );                
            }
            else
            {
                Ray_EventShooterVacuumReleased released;
                released.setSender( m_actor->getRef() );
                actor->onEvent( &released );
            }
        }
    }

	//------------------------------------------------------------------------------------------------------------------------
	f32	VacuumSkill::getFillFactor() const
	{  
		if ( m_template->getStackFullSize() > 0 )
		{
			return Min( f32( m_stackFillValue ) / f32( m_template->getStackFullSize() ), 1.0f );
		}
		return 0.0f;
	}
	//------------------------------------------------------------------------------------------------------------------------
	bbool VacuumSkill::isFull( bbool _realStackSizeCheck /*= bfalse*/ )
	{
		if ( _realStackSizeCheck )
		{
			return ( m_stackFillValue == m_template->getStackSize() );
		}
		return ( m_stackFillValue >= m_template->getStackFullSize() );
	}

	//------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::processVacuumed( Ray_EventShooterVacuumed * onVacuumed )
    {
		bool found = bfalse;
       
		for (u32 i = 0; i < m_actorsBeingVacuumed.size(); ++i)
		{
			if (m_actorsBeingVacuumed[i].m_actor == onVacuumed->getSender())
			{
				found = btrue;
				break;
			}
		}
        if ( m_specificProjectiles.find( onVacuumed->getSender() ) >= 0 )
        {
            found = btrue;
        }

		if (found)
		{
			return;
		}

        Actor * actor = ActorRef(onVacuumed->getSender()).getActor();
        if (!actor)
            return;

		bbool processActor = btrue;//!isFull( /*btrue */);

		if ( m_instantVacuumActive && ( m_actorsBeingVacuumed.size() > 0 ) )
		{
			processActor = bfalse;
		}
		
        if (processActor)
        {
            if ( addVacuumedActor( actor ) )
            {
			    // return effective
			    onVacuumed->setVaccumEffective();

			    EventShow show( 0.0f, m_template->getStickDuration() );
                actor->onEvent(&show);
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------------
    const Ray_ShooterActorParameter_StackData * VacuumSkill::getActorStackData( Actor * _actor )
    {
        ITF_ASSERT( _actor != NULL );
        if ( _actor == NULL )
        {
            return NULL;
        }
        Ray_ShooterActorParameterComponent * actorParamComponent = getActorShooterActorParamComponent( _actor );
        if ( actorParamComponent != NULL )
        {
            return &(actorParamComponent->getStackData() );
        }   
        return NULL;
    }
    
    //------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterActorParameterComponent * VacuumSkill::getActorShooterActorParamComponent( Actor * _actor )
    {
        ITF_ASSERT( _actor != NULL );
        if ( _actor == NULL )
        {
            return NULL;
        }
        return _actor->GetComponent<Ray_ShooterActorParameterComponent>();
    }

	//------------------------------------------------------------------------------------------------------------------------
	void VacuumSkill::updateSpecificProjectiles( Vec3d const & _pos )
	{
		for ( u32 i = 0; i < m_specificProjectiles.size(); ++i )
		{
            if ( !m_specificProjectiles[i].isValid() )
                continue;

			Actor * actor = m_specificProjectiles[i].getActor();
			//ITF_ASSERT( actor != NULL );
			if ( actor )
			{
				actor->setPos( _pos );
			}			
			else
			{
				// Actor destroyed, remove it
				i32 stackIdx = stackFindObjIdx( i, btrue );
				ITF_ASSERT( stackIdx != -1 );				// projectile in the array must be in the stack ! else there is an issue somewhere
				if ( stackIdx != -1 )
				{
					stackRemoveAt( stackIdx );				
				}
				removeSpecificProjectileAt( i );
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	Actor * VacuumSkill::Shoot ( Vec3d const &_startPos, Vec2d const &_dir )
	{
		ITF_ASSERT(hasAmmo());
        if ( !hasAmmo() )
            return NULL;

		Actor * ejectedActor = NULL;

		StackedObject popedObj;
		stackPop( popedObj );

		if ( popedObj.m_isGenerator )
		{
            ITF_ASSERT( popedObj.m_arrayIndex < m_specificProjectiles.size() );

            Actor * spawnedActor = NULL;

            if ( popedObj.m_arrayIndex < m_specificProjectiles.size() ) 
            {
                Actor * parentActor = m_specificProjectiles[popedObj.m_arrayIndex].getActor();
                if ( parentActor != NULL )
                {
                    Ray_ShooterActorParameterComponent * actorShooterParamComponent = getActorShooterActorParamComponent( parentActor );
                    if ( actorShooterParamComponent != NULL )
                    {
                        Scene * spawnScene = CURRENTWORLD->getRootScene();
                        spawnedActor = actorShooterParamComponent->getGeneratedProjectile( spawnScene, _startPos, 0.0f );
                    }

                    // stack can still contain the parent actor index if more than one projectile are needed
                    // check if there is still a ref, else destroy the parent
                    if ( stackFindObjIdx( popedObj.m_arrayIndex, btrue ) < 0 )
                    {
                        removeSpecificProjectileAt( popedObj.m_arrayIndex );
                        if (parentActor->isSerializable())
                        {
                            parentActor->disable();
                        }
                        else
                        {
                            parentActor->requestDestruction();
                        }
                    }
                }
            }
			
            if ( spawnedActor != NULL )
            {
                EventSetDirection dirEv;
                dirEv.setSender( m_actor->getRef() );
                dirEv.setDirection( _dir );
                spawnedActor->onEvent(&dirEv);
                spawnedActor->enable();
            }
		}
		else
		{
            ITF_ASSERT( popedObj.m_arrayIndex < m_specificProjectiles.size() );
            if ( popedObj.m_arrayIndex < m_specificProjectiles.size() ) 
            {
			    ejectedActor = m_specificProjectiles[popedObj.m_arrayIndex].getActor();
			    removeSpecificProjectileAt( popedObj.m_arrayIndex );
            }
		}		
		return ejectedActor;
	}

    //------------------------------------------------------------------------------------------------------------------------
    bbool VacuumSkill::addVacuumedActor( Actor * _actor )
    {
        if ( _actor == NULL )
        {
            return bfalse;
        }

        VacuumedActor v;
        v.m_actor = _actor->getRef();
        v.m_timer = 0.0f;
        v.m_stackSize = 1;
        v.m_hasBeenSwallowed = bfalse;

        const Ray_ShooterActorParameter_StackData * stackData = getActorStackData( _actor );
        if ( stackData != NULL )
        {
            v.m_stackSize = stackData->m_inStackSize;
            ITF_ASSERT( v.m_stackSize < 999 ); //data not initialized ?? 
        }        

        // Add it ?
        u32 biggestObjSize	= getBiggestObjSize();
        u32 freeSpace		= Max( i32( m_template->getStackSize() - m_stackFillValue - m_actorsBeingVacuumedFillValue ), 0 );
        if ( ( freeSpace >= v.m_stackSize ) || ( ( ( v.m_stackSize >= biggestObjSize ) /*|| m_template->isStackInLifoMode()*/ ) && ( freeSpace > 0 ) ) )
        {
            m_actorsBeingVacuumedFillValue += v.m_stackSize;
            m_actorsBeingVacuumed.push_back(v);
            return btrue;
        }

        return bfalse;
    }

    //------------------------------------------------------------------------------------------------------------------------
    void VacuumSkill::removeVacuumedActor( u32 _idx )
    {
        ITF_ASSERT( _idx < m_actorsBeingVacuumed.size() );
        if ( _idx >= m_actorsBeingVacuumed.size() )
        {
            return;
        }

        m_actorsBeingVacuumedFillValue -= m_actorsBeingVacuumed[_idx].m_stackSize;
        ITF_ASSERT( m_actorsBeingVacuumedFillValue < 999 );
       
        m_actorsBeingVacuumed.eraseNoOrder( _idx );
    }

    //------------------------------------------------------------------------------------------------------------------------
    // Actors vaccumed are swallowed 
    //
    void VacuumSkill::aspirationFinished()
    {
        for (u32 i = 0; i < m_actorsBeingVacuumed.size(); ++i)
        {
            VacuumedActor & v = m_actorsBeingVacuumed[i];
            processSwallowed( v, btrue );
            removeVacuumedActor( i );
            i--;
        }
        m_actorsBeingVacuumed.clear();
        m_actorsBeingVacuumedFillValue = 0;
    }    
	//------------------------------------------------------------------------------------------------------------------------
	// Process vacuum shape, and process stim
	//
	void VacuumSkill::aspirate()
	{
		updateVacuumShape();
		sendVacuumStim();
	}

	//------------------------------------------------------------------------------------------------------------------------
	// Update vaccumed actors
	//
	void VacuumSkill::aspirationUpdate( f32 _dt, Vec3d const &_startPos )
	{
		m_fillValueHasChanged = bfalse;

		Vec3d newPos = _startPos;
		newPos.m_z = m_actor->getDepth();

		for (u32 i = 0; i < m_actorsBeingVacuumed.size(); ++i)
		{
			VacuumedActor & v = m_actorsBeingVacuumed[i];
			Actor * target = v.m_actor.getActor();
			if (target)
			{
				if (v.m_timer < m_template->getStickDuration())
				{
					newPos = (target->getPos() * (m_template->getStickDuration() - v.m_timer) + newPos * v.m_timer)*(1.f/m_template->getStickDuration());
					v.m_timer += _dt;
					target->updateComponentPosAngle( newPos, target->getAngle(), NULL);
				}
				else
				{
					processSwallowed( v );
                    if ( i < m_actorsBeingVacuumed.size() ) // could arrives if the swallowed process has cleaned the array
                    {
                        removeVacuumedActor( i );
                        i--;
                    }
				}
			}
			// actor destroyed before vacuum finished
			else
			{
				removeVacuumedActor( i );
				i--;			
			}
		}
	}

#ifdef ITF_SUPPORT_DEBUGFEATURE
	//------------------------------------------------------------------------------------------------------------------------
	void VacuumSkill::debugDraw() const 
	{
		Vec2d offsetPos = m_actor->get2DPos();
		offsetPos -= Vec2d( 0.0f, 1.0f );
		Vec2d indexSize = Vec2d( 0.5f, 0.5f );
		Vec2d stackSize = Vec2d( indexSize.m_x * m_template->getStackSize(), indexSize.m_y );
		stackSize *= 0.5f;
		Vec2d stackStartPos = offsetPos - Vec2d( stackSize.m_x, stackSize.m_y );
		
		//DebugDraw::line2D( offsetPos, offsetPos + ( Vec2d( 1.0, 1.0 ) * 2.0f ), m_actor->getDepth() );
		GFX_ADAPTER->drawDBGBox( offsetPos, m_actor->getAngle(), stackSize );
		Vec2d prevIndexPos = stackStartPos;

		for ( u32 i = 0; i < m_stackedObjects.size() ; ++i )
		{
			f32 boxWidth = indexSize.m_x * f32( m_stackedObjects[i].m_stackSize );
			Vec2d drawBoxSize = Vec2d( boxWidth, indexSize.m_y ) * 0.5f;
			Vec2d boxPos = prevIndexPos + drawBoxSize;
			prevIndexPos += Vec2d::Right * boxWidth;
			//Color boxColor = Color( 1.0f, , 1.0f, 1.0f );
			f32 colorFactor = f32( i ) * ( 1.0f / f32( m_template->getStackSize() - 1 ) );
			
			GFX_ADAPTER->drawDBGBox( boxPos, m_actor->getAngle(), drawBoxSize, colorFactor, 0.5f + ( 0.5f * colorFactor ), 1.0f - colorFactor, 0.0, m_actor->getDepth(), btrue );
			
		}

		m_vacuumForce.drawDebug( m_actor->get2DPos(), m_actor->getAngle(), m_actor->getDepth() );
	}
#endif // ITF_SUPPORT_DEBUGFEATURE

    //-----------------------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION( VacuumSkill_Template )
        SERIALIZE_OBJECT("vacuumForce",			m_vacuumForce);
        SERIALIZE_MEMBER("stickDuration",		m_stickDuration);
		SERIALIZE_MEMBER("stackSize",			m_stackSize	);
		SERIALIZE_MEMBER("stackFullSize",		m_stackFullSize	);
		SERIALIZE_MEMBER("stackLifoMode",		m_stackLifoMode );
		SERIALIZE_MEMBER("vaccumSpeedFactor",	m_vaccumSpeedFactor );
		
        SERIALIZE_MEMBER("stateStartDuration",	m_stateStartDuration);
        SERIALIZE_MEMBER("stateStopDuration",	m_stateStopDuration);
		SERIALIZE_MEMBER("stateLoopDuration",	m_stateLoopDuration);
		SERIALIZE_MEMBER("stateBeforeRestartDuration",m_stateBeforeRestartDuration);
		
    END_SERIALIZATION()

    VacuumSkill_Template::VacuumSkill_Template()
    : m_stickDuration( 0.2f )
	, m_stackSize( 5 )
	, m_vaccumSpeedFactor( 1.0f )
	, m_stateStartDuration( 0.5f )
    , m_stateStopDuration( 0.5f )
	, m_stateLoopDuration( 3.0f )
	, m_stateBeforeRestartDuration( 2.0f )
	, m_stackLifoMode( bfalse )
    {
		m_stackFullSize = m_stackSize;
    }
};
