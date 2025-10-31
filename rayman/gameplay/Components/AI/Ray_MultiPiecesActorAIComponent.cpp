#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MultiPiecesActorAIComponent.h"
#endif //_ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_



namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_MultiPiecesActorAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_MultiPiecesActorAIComponent)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_MultiPiecesActorAIComponent::Ray_MultiPiecesActorAIComponent()
    : Super()
    , m_curDetachOrder( 0 )
    , m_initPieceCount( 0 )
    {
        m_piecesList.clear();
    }

    //*****************************************************************************

    Ray_MultiPiecesActorAIComponent::~Ray_MultiPiecesActorAIComponent()
    {
        destroyPieces();
    }

    //*****************************************************************************

    void Ray_MultiPiecesActorAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventCanBeVacuum,774659105),this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventIsInfluencedByVacuum,1958637106),this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this );

        m_initPieceCount = 0;
    }

    //*****************************************************************************

    void Ray_MultiPiecesActorAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        // register pieces
        //ObjectRefList removeParentList;
        ActorBindHandler & bindHandler = m_actor->getChildrenBindHandler();
        const ObjectRefList&  children = bindHandler.getChildren();

        for ( u32 i = 0; i < children.size(); ++i )
        {
            BaseObject * obj = children[i].getObject();
            if ( obj != NULL )
            {
                Actor * actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                if ( actor != NULL )
                {   
                    const PieceData * pieceData = getTemplate()->getPieceData( actor->getLua() );
                    if ( pieceData != NULL )
                    {
                        Piece piece; 
                        piece.m_actor = children[i];
                        piece.m_pieceData = pieceData;

                        m_piecesList.push_back( piece );

                        // Register hit event on me for the piece
                        ACTOR_REGISTER_EVENT_COMPONENT(actor, ITF_GET_STRINGID_CRC(HitStim,2121727796), this );

                        if ( actor->getParentBind() != NULL )
                        {
                            actor->getParentBind()->setRuntimeDisabled(bfalse);
                        }

                        // !!! temp !!! waiting for engine update
                        //if ( actor->getParentBind() != NULL )
                        //{
                        //   /* Vec3d pos = actor->getParentBind()->getPosOffset();
                        //    f32 angle = actor->getParentBind()->getAngleOffset();*/

                        //    removeParentList.push_back( piece.m_actor );
                        //}
                    }
                }            
            }
        }

        /*for ( u32 i = 0; i < removeParentList.size(); ++i )
        {
            BaseObject * obj = removeParentList[i].getObject();
            if ( obj != NULL )
            {
                Actor * actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                if ( actor != NULL )
                {
                    actor->setParentBind( NULL );
                    m_actor->getBinding()->bindChild( removeParentList[i] );
                }
            }
        }
        removeParentList.clear();*/

        m_initPieceCount = m_piecesList.size();
        m_curDetachOrder = 0;
        
    }
    
    //*****************************************************************************

    void Ray_MultiPiecesActorAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        // check pieces coherency
        //
        checkPiecesCoherency();
    }    

    //*****************************************************************************

    void Ray_MultiPiecesActorAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();
    }

    //*****************************************************************************

    void Ray_MultiPiecesActorAIComponent::onEvent( Event* _event )
    {
        if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
        {
            Actor * hitPieceOwner = NULL;

            if ( BaseObject * phantom = hit->getPhantomRef().getObject() )
            {
                if ( BaseObject * userdata = ( phantom->DynamicCast<PhysPhantom>(ITF_GET_STRINGID_CRC(PhysPhantom,126559815)) )->getUserData().getObject() )
                {
                    if ( PolyLine * poly = userdata->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)) )
                    {
                        if ( ( poly->getOwnerActor() != m_actor ) && isMyPiece( poly->getOwnerActor()->getRef() ) )
                        {
                            hitPieceOwner = poly->getOwnerActor();
                        }
                    }
                    else if ( Actor * actor = userdata->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                    {
                        if ( isMyPiece( actor->getRef() ) )
                        {
                            hitPieceOwner = actor;
                        }
                    }
                }
            }
            if ( hitPieceOwner != NULL )
            {
                onPieceHit( hitPieceOwner );
            }
            else
            {
                processHit( hit );
            }
        }
        // piece can be vacuum ?
        else if ( Ray_EventCanBeVacuum * canBeVacuum = _event->DynamicCast<Ray_EventCanBeVacuum>(ITF_GET_STRINGID_CRC(Ray_EventCanBeVacuum,774659105)) )
        {
            if ( canBeVacuum->getSender() != m_actor->getRef() )
            {
                canBeVacuum->setCanBeVaccum( isPieceCanBeVacuum( canBeVacuum->getSender() ) );
            }
        }
        // piece can be vacuum ?
        else if ( Ray_EventIsInfluencedByVacuum * influencedByVacuum = _event->DynamicCast<Ray_EventIsInfluencedByVacuum>(ITF_GET_STRINGID_CRC(Ray_EventIsInfluencedByVacuum,1958637106)) )
        {
            if ( influencedByVacuum->getSender() != m_actor->getRef() )
            {
                influencedByVacuum->setInfluenced( isPieceCanBeVacuum( influencedByVacuum->getSender() ) );
            }
        }

        Super::onEvent(_event);
    }

    //*****************************************************************************
    void Ray_MultiPiecesActorAIComponent::onPieceHit( Actor * _piece )
    {

    }

    //*****************************************************************************
    bbool Ray_MultiPiecesActorAIComponent::isPieceCanBeVacuum( const ActorRef & _pieceRef )
    {
        Piece * piece = getPieceData( _pieceRef );
        if ( piece == NULL )
        {
            return btrue;
        }
        else if ( ( piece->m_pieceData->m_detachOrder >= 0 ) && 
                  ( u32( piece->m_pieceData->m_detachOrder ) <= m_curDetachOrder ) )
        {
            return btrue;
        }
        return bfalse;
    }
    
    //*****************************************************************************
    bbool Ray_MultiPiecesActorAIComponent::isMyPiece( const ObjectRef & _objRef )
    {
        for ( u32 i = 0; i < m_piecesList.size(); ++i )
        {
            if ( m_piecesList[i].m_actor == _objRef )
                return btrue;        
        }
        return bfalse;
    }

    //*****************************************************************************
    Piece * Ray_MultiPiecesActorAIComponent::getPieceData( const ObjectRef & _objRef )
    {
        for ( u32 i = 0; i < m_piecesList.size(); ++i )
        {
            if ( m_piecesList[i].m_actor == _objRef )
                return &(m_piecesList[i]);
        }
        return NULL;
    }

    //*****************************************************************************
    void Ray_MultiPiecesActorAIComponent::checkPiecesCoherency()
    {
        // register pieces
        /*ActorBindHandler & bindHandler = m_actor->getChildrenBindHandler();
        const ObjectRefList&  children = bindHandler.getChildren();*/
        m_actor->getBinding()->forceRemovePending();

        const StringID * startBhvName = NULL;

        for ( u32 i = 0; i < m_piecesList.size(); ++i )
        {
            Piece & piece = m_piecesList[i];

            //i32 idx = children.find( piece.m_actor );
            // object no more binded
            //------------------------
            //if ( idx < 0 )
            Actor * actor = piece.m_actor.getActor();
            if ( ( actor == NULL ) || ( actor->getParentBind() && actor->getParentBind()->isRuntimeDisabled() ) )
            {
                if ( piece.m_pieceData->m_behaviorName.isValid() )
                {
                    startBhvName = &piece.m_pieceData->m_behaviorName;
                }

                if ( actor != NULL )
                {
                    actor->unregisterEvent( ITF_GET_STRINGID_CRC(HitStim,2121727796),this );
                }
                m_piecesList.eraseKeepOrder(i);
                m_curDetachOrder++;
                i--;
            }
            // check damages
            else
            {
                if ( !piece.m_bhvStarted && piece.m_pieceData->m_behaviorName.isValid() /*&& piece.m_pieceData->*/ )
                {
                    if ( AIComponent * pieceAI = actor->GetComponent<AIComponent>() )
                    {
                        if ( pieceAI->getHealth() <= piece.m_pieceData->m_startBhvUnderDamage )
                        {
                            piece.m_bhvStarted = btrue;
                            startBhvName = &piece.m_pieceData->m_behaviorName;
                        }
                    }
                    else if ( piece.m_pieceData->m_startBhvUnderDamage == 0.0f )
                    {
                        EventQueryIsDead deadQuery;
                        actor->onEvent( &deadQuery );
                        if ( deadQuery.getIsDead() )
                        {
                            piece.m_bhvStarted = btrue;
                            startBhvName = &piece.m_pieceData->m_behaviorName;
                        }
                    }
                }
            }
            // update piece position
            /*else
            {
                Actor * actor = piece.m_actor.getActor();
                if ( ( actor != NULL ) && piece.m_pieceData->m_boneName.isValid() )
                {
                    i32 boneidx = m_animComponent->getBoneIndex( piece.m_pieceData->m_boneName );
                    if ( boneidx >= 0 )
                    {
                        AnimBoneDyn* bone = m_animComponent->getBone(boneidx);
                        Vec2d bonePos; 
                        m_animComponent->getBonePos( boneidx, bonePos );
                        bonePos += piece.m_pieceData->m_localPos.Rotate( -piece.m_pieceData->m_localAngle );

                        actor->updateComponentPosAngle( bonePos.to3d((piece.m_pieceData->m_depth)), bone->getAngle() + piece.m_pieceData->m_localAngle, NULL );
                    }
                }
            }*/
        }

        // change behavior
        //--------------------
        if ( ( startBhvName != NULL ) && ( getBehaviorFromName( *startBhvName ) != NULL ) )
        {
            setBehavior( *startBhvName );
        }
    }

     //*****************************************************************************
    void Ray_MultiPiecesActorAIComponent::destroyPieces()
    {
        for ( u32 i = 0; i < m_piecesList.size(); ++i )
        {
            Actor * actor = m_piecesList[i].m_actor.getActor();
            if ( actor != NULL )
            {
                actor->unregisterEvent( ITF_GET_STRINGID_CRC(HitStim,2121727796),this );
            }

            m_piecesList.eraseNoOrder(i);
            i--;
        }
        m_piecesList.clear();
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_MultiPiecesActorAIComponent_Template)
    
    BEGIN_SERIALIZATION_CHILD(Ray_MultiPiecesActorAIComponent_Template)
        SERIALIZE_CONTAINER_OBJECT( "piecesDataList", m_piecesDataList );
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(PieceData)
        SERIALIZE_MEMBER( "actorPath"           , m_actorPath );
        SERIALIZE_MEMBER( "name"                , m_name );
        SERIALIZE_MEMBER( "detachOrder"         , m_detachOrder );
        SERIALIZE_MEMBER( "startBhvUnderDamage" , m_startBhvUnderDamage );
        SERIALIZE_MEMBER( "behaviorName"        , m_behaviorName );
    END_SERIALIZATION()

    //*****************************************************************************
    //
    Ray_MultiPiecesActorAIComponent_Template::Ray_MultiPiecesActorAIComponent_Template()
    : Super()
    {
        // none
    }

    //*****************************************************************************
    //
    Ray_MultiPiecesActorAIComponent_Template::~Ray_MultiPiecesActorAIComponent_Template()
    {
    }

    //*****************************************************************************
    const PieceData * Ray_MultiPiecesActorAIComponent_Template::getPieceData( const Path & _actorPath ) const
    {
        for ( u32 i = 0; i < m_piecesDataList.size(); ++i )
        {
            if ( m_piecesDataList[i].m_actorPath.getStringID() == _actorPath.getStringID() )
            {
                return &(m_piecesDataList[i]);
            }
        }
        return NULL;
    }

}