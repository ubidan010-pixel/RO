#include "precompiled_gameplay.h"

#ifndef _ITF_FLYINGCOMPONENT_H_
#include "FlyingComponent.h"
#endif //_ITF_FLYINGCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_WAYPOINTSMANAGER_H_
#include "gameplay/managers/WaypointsManager.h"
#endif //_ITF_WAYPOINTSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_


namespace ITF
{

IMPLEMENT_OBJECT_RTTI(FlyingComponent)
	BEGIN_SERIALIZATION_CHILD(FlyingComponent)	
	//SERIALIZE_FUNCTION(reset,ESerialize_Template_Load|ESerialize_PropertyEdit_Load);
END_SERIALIZATION()

static f32 InterpolateAngle(f32 _fA, f32 _fB, f32 _fFactor)
{
    f32 ca,sa,cb,sb;
    f32_CosSin(_fA, &ca, &sa);
    f32_CosSin(_fB, &cb, &sb);
	return atan2f(	Interpolate(sa, sb, _fFactor),
					Interpolate(ca, cb, _fFactor) );
}

// This function average an angle
static float AverageAngleUpdate( Vec2d & _vStorageVector, const f32 & _fWantedAngle, const f32 & _fLag, const f32 & _fFrameTime )
{
	f32 fOldXValue	= _vStorageVector.m_x;
	f32 fTargetValue	= InterpolateAngle( _vStorageVector.m_y, _fWantedAngle , 0.5f );
	// Adjust angles between 0..3*180� so that blending between them is cool
	// assume both are normalized between 0..360�
	f32 fDelta = fOldXValue - fTargetValue;
	if (fDelta > MTH_PI )
		fTargetValue += MTH_2PI;
	else if (fDelta < -MTH_PI )
		fOldXValue += MTH_2PI;

	_vStorageVector.m_x	= fOldXValue + ( ( fTargetValue - fOldXValue ) * ( _fFrameTime / Max( _fFrameTime, _fLag ) ) );
	NormalizeAngle2PI( _vStorageVector.m_x );
	_vStorageVector.m_y	= _fWantedAngle;
	return _vStorageVector.m_x;
}

// Init the angle average
static void AverageAngleInit( Vec2d & _vStorageVector, const f32 & _fInitValue )
{
	f32 angleInit = _fInitValue;
	NormalizeAngle2PI( angleInit );
	_vStorageVector = Vec2d( angleInit, angleInit );
}


//-----------------------------------------------------------------------------------------------------

FlyingComponent::FlyingComponent()
: Super()
,m_physComponent(NULL)
,m_MoveVecAngle(0.0)
,m_FlyingMode(true)
,m_EnvContactValid(false)
,m_EnvContactNextEdge(false)
{
	AverageAngleInit( m_MoveAvgAngle, 0 );
	AverageAngleInit( m_DirAvgAngle, 0.0 );
}

FlyingComponent::~FlyingComponent()
{
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
	Super::onActorLoaded(_hotReload);

	// register events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCollide,3122529699),this);

	//misc
	m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
	if ( m_physComponent != NULL )
	{
		//m_physComponent->setProcessContacts( false );
		m_physComponent->setStickingDisabled( true );
		m_physComponent->setAirFrictionMultiplier( 1.0 );
	}

	MoveModeChange(true);
}
//-----------------------------------------------------------------------------------------------------
void FlyingComponent::Update( f32 _deltaTime )
{
	if ( getTemplate()->getDebugData().isValid() )
	{
		if ( getTemplate()->getDebugData().m_OnPlayer )
		{
			Actor* player = GAMEMANAGER->getActivePlayer(0);
			MoveTo( player->get2DPos() ); 
		}
		else
		{
			Vec2d wayPointPos;
			Actor* waypoint = WAYPOINTS_MANAGER->getWaypoint( getTemplate()->getDebugData().m_SnapToWaypointId );
			if ( waypoint != NULL )
			{
				MoveTo( waypoint->get2DPos() ); 
			}
		}
	}	

	EnvironmentEvaluation();
	MoveUpdate(_deltaTime);
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::onEvent( Event * _event)
{
	Super::onEvent( _event);

	if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventCollide,3122529699)) )
	{
		if ( m_physComponent != NULL )
		{
			EventCollide * evCollide = _event->DynamicCast<EventCollide>(ITF_GET_STRINGID_CRC(EventCollide,3122529699));
			Vec2d moveDir = m_actor->get2DPos() - evCollide->getPos();
			moveDir.normalize();
			f32 addedForce = 10.0;
			if ( getTemplate()->getDebugData().isValid() )
			{
				addedForce = getTemplate()->getDebugData().m_CollideInvForce;
			}
			m_physComponent->addForce( moveDir * addedForce );
		}		
	}

}

//*****************************************************************************************************************************
//
//													EVALUATIONS
//	
//*****************************************************************************************************************************
//-----------------------------------------------------------------------------------------------------
Vec2d FlyingComponent::getNearestEdgeExtremityFromPos( const PolyLineEdge& _edge, Vec2d _pos, f32 &_res_sqrNorm )
{
	f32 curEdgeFirstPosNorm = ( _edge.getPos() - _pos ).sqrnorm();
	f32 curEdgeSecondPosNorm = ( _edge.getPos() + _edge.m_vector - _pos ).sqrnorm();

	if ( curEdgeFirstPosNorm > curEdgeSecondPosNorm )
	{
		_res_sqrNorm = curEdgeSecondPosNorm;
		return ( _edge.getPos() + _edge.m_vector );
	}
	
	_res_sqrNorm = curEdgeFirstPosNorm;
	return _edge.getPos();
}


//-----------------------------------------------------------------------------------------------------
void FlyingComponent::EnvironmentEvaluation()
{
	// Environement Evaluation is managed only if a physic component exist on the actor for moment...
	if ( m_physComponent == NULL )
	{
		return;
	}

	// Check Polylines contacts
	const PhysContactsContainer & physContacts = m_physComponent->getContacts();
	m_EnvContactValid = false;
	
	// Use First contact only for moment
	if ( physContacts.size() > 0 )
	{
		m_EnvContactPos = physContacts[0].m_contactPoint;
		m_EnvContactNormal = physContacts[0].m_normal;
		m_EnvContactEdgeIdx = physContacts[0].m_edgeIndex;
		
		m_EnvContactPolyline = static_cast<PolyLine*>(GETOBJECT(physContacts[0].m_collidableUserData));

#ifndef ITF_FINAL
		/*	if ( m_EnvContactPolyline )
		{
			const PolyLineEdge& curEdge = m_EnvContactPolyline->getEdgeAt( physContacts[0].m_edgeIndex );
			DebugDraw::edge( &curEdge, 0.f, COLOR_RED );
			
						
			u32 curEdgeId =  (physContacts[0].m_edgeIndex + 1)%m_EnvContactPolyline->getPosCount();
			const PolyLineEdge& nextEdge = m_EnvContactPolyline->getEdgeAt( curEdgeId );
			u32 nearestEdgeId = curEdgeId;
			f32 nearestEdgeSqrNorm = Min( ( nextEdge.getPos() - m_EnvContactPos ).sqrnorm(), ( nextEdge.getPos() + nextEdge.m_vector - m_EnvContactPos ).sqrnorm() );
			
			curEdgeId =  (physContacts[0].m_edgeIndex - 1) < 0 ? m_EnvContactPolyline->getPosCount() - 1 : physContacts[0].m_edgeIndex - 1;
			const PolyLineEdge& prevEdge = m_EnvContactPolyline->getEdgeAt( curEdgeId );
			if ( nearestEdgeSqrNorm > Min( ( prevEdge.getPos() - m_EnvContactPos ).sqrnorm(), ( prevEdge.getPos() + prevEdge.m_vector - m_EnvContactPos ).sqrnorm() ) )
			{
				nearestEdgeId = curEdgeId;
			}

			const PolyLineEdge& nextFinalEdge = m_EnvContactPolyline->getEdgeAt( nearestEdgeId );
			DebugDraw::edge( &nextFinalEdge, 0, COLOR_GREEN );
		}*/
#endif // ITF_FINAL

		m_EnvContactValid = true;
	}


	
	// LAND mode 
	if ( !m_FlyingMode )
	{
		// No more contacts
		if ( ( physContacts.size() == 0 ) && !m_EnvContactNextEdge )
		{
			Vec2d destVec = m_DestinationPos - m_actor->get2DPos();
			//const PolyLine* nextPolyline;
			//destVec.normalize();
			// Try to stay on polyline
			if ( ( destVec.dot( m_MoveDir ) < 0.0 ) && ( m_EnvContactPolyline != NULL ) )
			{
				f32 firstEdgeSqrNorm, secEdgeSqrNorm;
				u32 firstEdgeId				=  (m_EnvContactEdgeIdx + 1)%m_EnvContactPolyline->getPosCount();
				Vec2d firstEdgeNearestPos	= getNearestEdgeExtremityFromPos( m_EnvContactPolyline->getEdgeAt( firstEdgeId ), m_EnvContactPos, firstEdgeSqrNorm );
				u32 secEdgeId				=  (m_EnvContactEdgeIdx - 1) < 0 ? m_EnvContactPolyline->getPosCount() - 1 : m_EnvContactEdgeIdx - 1;
				Vec2d secEdgeNearestPos		= getNearestEdgeExtremityFromPos( m_EnvContactPolyline->getEdgeAt( secEdgeId ), m_EnvContactPos, secEdgeSqrNorm );

				u32 nearestEdgeId = firstEdgeId;
				m_EnvContactNextEdgePos = firstEdgeNearestPos;
				if ( firstEdgeSqrNorm > secEdgeSqrNorm )
				{
					nearestEdgeId = secEdgeId;
					m_EnvContactNextEdgePos = secEdgeNearestPos;
				}

				const PolyLineEdge& nextFinalEdge = m_EnvContactPolyline->getEdgeAt( nearestEdgeId );
#ifndef ITF_FINAL
				/*DebugDraw::edge( &nextFinalEdge, 0, COLOR_GREEN );*/
#endif // ITF_FINAL

				//m_EnvContactPos = physContacts[0].m_contactPoint;
				m_EnvContactNormal = -nextFinalEdge.m_normalizedVector.getPerpendicular();
				m_EnvContactNextEdge = true;

				LandModeInitialize();
			}
			else		
			{
				MoveModeChange(true);
			}
		}
		else if ( ( physContacts.size() > 0 ) && m_EnvContactNextEdge )
		{
			m_EnvContactNextEdge = false;
		}
	}

	if ( m_FlyingMode && ( physContacts.size() != 0 ) )
	{
		MoveModeChange(false);
	}	
}

//*****************************************************************************************************************************
//
//													GLOBAL CASES
//	
//*****************************************************************************************************************************
//-----------------------------------------------------------------------------------------------------
void FlyingComponent::MoveTo( Vec2d _DestinationPos )
{
	m_DestinationPos = _DestinationPos;
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::MoveDirCompute( f32 _CurAngle, f32 _deltaTime )
{
    m_MoveVecAngle = AverageAngleUpdate( m_MoveAvgAngle, _CurAngle, getTemplate()->getMoveVecBlendFactor(), _deltaTime );

	m_MoveDir = Vec2d::Right; 
	m_MoveDir = m_MoveDir.Rotate( m_MoveVecAngle );
}
//-----------------------------------------------------------------------------------------------------
void FlyingComponent::MovePosCompute( Vec2d _MovePos )
{
	m_MoveCurPos = _MovePos;
	m_MoveCurPosDist = ( m_MoveCurPos - m_actor->get2DPos() ).norm();
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::MoveUpdate(f32 _deltaTime)
{
	// Reset force
	m_MoveAddForce.clear();

	// By the air
	if ( m_FlyingMode )
	{
		FlyModeCompute( _deltaTime );
	}
	else
	{
		LandModeCompute( _deltaTime );
	}

#ifndef ITF_FINAL
	//DebugDraw::arrow2D( m_actor->get2DPos(), m_actor->get2DPos() + m_MoveDir, 0.0 );
#endif // ITF_FINAL

	// Check component type
	//
	if ( m_physComponent != NULL )
	{
		MoveUpdate_WithPhysic( _deltaTime );	
	}
	else
	{
		MoveUpdate_Normal( _deltaTime );
	}
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::MoveUpdate_WithPhysic(f32 _deltaTime )
{	
	// Limit the force to wanted speed 
	f32 currentSpeed = m_physComponent->getSpeed().norm();
	f32 finalForce = getTemplate()->getMoveSpeed();
	finalForce *= 1.0f - Clamp( currentSpeed/getTemplate()->getMoveSpeed(), 0.0f, 1.0f );
	
	m_MoveVector = m_MoveDir * finalForce * Min( m_MoveCurPosDist, 1.f );
	m_MoveVector += m_MoveAddForce;

	//m_actor->setAngle( m_MoveVecAngle );
	m_physComponent->addForce(m_MoveVector);
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::MoveUpdate_Normal(f32 _deltaTime)
{
	m_actor->set2DPos( m_actor->get2DPos() + m_MoveVector );
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::MoveModeChange( bbool _Flying )
{
	m_FlyingMode = _Flying;
	m_MoveAddForce.clear();
	
	if (_Flying )
	{
		FlyModeInitialize();
	}
	else
	{
		LandModeInitialize();
	}
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::DirectionCompute( Vec2d _moveDir, Vec2d _contactNormal, f32 _deltaTime )
{
	bbool flipped = ( getSign( _moveDir.dot( Vec2d::Right ) ) * getSign( _contactNormal.dot( Vec2d::Up ) ) ) > 0.f ? false : true;

	if ( ( !flipped && m_actor->isFlipped() ) || ( flipped && !m_actor->isFlipped() ) )
	{
		m_actor->setStartFlipped( flipped );
		m_actor->setFlipped( flipped );
	}

	f32 curAngle = _contactNormal.getAngle() - MTH_PIBY2;
    f32 newAngle = AverageAngleUpdate( m_DirAvgAngle, curAngle, getTemplate()->getMoveVecBlendFactor(), _deltaTime );
	m_actor->setAngle( newAngle );
}

//*****************************************************************************************************************************
//
//													FLYING CASES
//	
//*****************************************************************************************************************************
//-----------------------------------------------------------------------------------------------------
void FlyingComponent::FlyModeInitialize()
{
	m_FlyInterPosValid = false;
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::FlyModeCompute( f32 _deltaTime )
{
	// Get the real wanted move dir angle
	//
	Vec2d moveVec = m_DestinationPos - m_actor->get2DPos();
	moveVec.normalize();
	f32 curAngle = moveVec.getAngle();
	f32 initCurAngle = curAngle;
	f32 curAngleSign = getSign( curAngle );

	// Clamp the wanted angle with limits
	//
	if ( f32_Abs( curAngle ) > MTH_PIBY2 )
	{
		curAngle = Clamp( f32_Abs( curAngle ), MTH_PI - getTemplate()->getMoveDirMaxAngle(), MTH_PI - getTemplate()->getMoveDirMinAngle() ) * curAngleSign;
	}
	else
	{
		curAngle = Clamp( f32_Abs( curAngle ), getTemplate()->getMoveDirMinAngle(), getTemplate()->getMoveDirMaxAngle() ) * curAngleSign;
	}
	
	// Check if the inter position is reached
	if ( m_FlyInterPosValid )
	{
		if ( (m_FlyInterPos - m_actor->get2DPos()).sqrnorm() < 0.2 )
		{
			m_FlyInterPosValid = false;
		}
	}
	
	// The Real wanted angle can't be process, compute inter pos
	if ( curAngle != initCurAngle )
	{
		if ( !m_FlyInterPosValid && ( getTemplate()->getMoveDirAmplitude() > 0.f ) )
		{
			Vec2d nextDir = Vec2d::Right;
			nextDir = nextDir.Rotate( curAngle );
			m_FlyInterPos = m_actor->get2DPos() + ( nextDir * getTemplate()->getMoveDirAmplitude() ); 
			m_FlyInterPosValid = true;
		}
	}
	else
	{
		m_FlyInterPosValid = false;
	}
	
	if ( m_FlyInterPosValid )
	{
		// Compute the final angle to apply
		Vec2d nextMoveVec = m_FlyInterPos - m_actor->get2DPos();
		nextMoveVec.normalize();
		curAngle = nextMoveVec.getAngle();
	}
	
	MoveDirCompute( curAngle, _deltaTime );
	MovePosCompute( m_FlyInterPosValid ? m_FlyInterPos : m_DestinationPos );
	// Direction Compute
	DirectionCompute( m_MoveDir, Vec2d::Up, _deltaTime );
}

//*****************************************************************************************************************************
//
//													LAND CASES
//	
//*****************************************************************************************************************************
//-----------------------------------------------------------------------------------------------------
void FlyingComponent::LandModeInitialize()
{
	Vec2d globalMoveDir = m_DestinationPos - m_actor->get2DPos();

	m_LandMoveDir = -m_EnvContactNormal.getPerpendicular();
	m_LandMoveDir *= getSign( m_LandMoveDir.dot( globalMoveDir ) );

	m_MoveDir = m_LandMoveDir;
}

//-----------------------------------------------------------------------------------------------------
void FlyingComponent::LandModeCompute( f32 _deltaTime )
{
	// Move compute
	Vec2d moveDir = -m_EnvContactNormal.getPerpendicular();
	moveDir *= getSign( moveDir.dot( m_LandMoveDir ) );

	// Contact no more valid, try to stick
	m_MoveAddForce.clear();
	if ( m_EnvContactNextEdge )
	{
		m_MoveAddForce = ( m_EnvContactNextEdgePos - m_actor->get2DPos() ).normalize() * getTemplate()->getMoveSpeed()*2.0;
	}

	/*if ( !m_EnvContactValid )
	{
		
	}*/
	
	MoveDirCompute( moveDir.getAngle(), _deltaTime );

#ifndef ITF_FINAL
	//DebugDraw::arrow2D( m_actor->get2DPos(), m_actor->get2DPos() + m_EnvContactNormal, 0.0 ); 
#endif // ITF_FINAL

	// Direction Compute
	DirectionCompute( moveDir, -m_EnvContactNormal, _deltaTime );
}

//-----------------------------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void FlyingComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);
    MoveModeChange( true );
    m_EnvContactNextEdge = false;
}
#endif // ITF_SUPPORT_EDITOR
//-----------------------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(FlyingComponent_Template)

BEGIN_SERIALIZATION(FlyingComponent_Template)
    SERIALIZE_OBJECT("DebugData",			m_DebugData);
    SERIALIZE_MEMBER("MoveVecBlendFactor",	m_MoveVecBlendFactor);
    SERIALIZE_MEMBER("MoveSpeed",			m_MoveSpeed);
    SERIALIZE_MEMBER("MoveDirMaxAngle",		m_MoveDirMaxAngle);
    SERIALIZE_MEMBER("MoveDirMinAngle",		m_MoveDirMinAngle);
    SERIALIZE_MEMBER("MoveDirAmplitude",	m_MoveDirAmplitude);
END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(FlyingComponent_Template,FlyingComponentDebugData)
    SERIALIZE_MEMBER("Used",				m_Used);
    SERIALIZE_MEMBER("SnapToWaypointId",	m_SnapToWaypointId);
    SERIALIZE_MEMBER("OnPlayer",			m_OnPlayer);
    SERIALIZE_MEMBER("CollideInvForce",		m_CollideInvForce);
END_SERIALIZATION()

FlyingComponent_Template::FlyingComponent_Template()
: m_MoveVecBlendFactor(1.0)
, m_MoveSpeed(0.0)
, m_MoveDirMaxAngle(MTH_PIBY2)
, m_MoveDirMinAngle(0.0)
, m_MoveDirAmplitude(0.0)
{
}

}