#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AILIGHTNINGACTION_H_
#include "rayman/gameplay/ai/Actions/Shooter/Ray_AILightningAction.h"
#endif //_ITF_RAY_AILIGHTNINGACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_TIMEDSPAWNERAICOMPONENT_H_
#include "gameplay/Components/AI/TimedSpawnerAIComponent.h"
#endif //_ITF_TIMEDSPAWNERAICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AILightningAction)

        Ray_AILightningAction::Ray_AILightningAction()
        : Super()
        , m_timedSpawnerAIComponent( NULL )
        , m_startBoneIdx( U32_INVALID )
        , m_destBoneIdx( U32_INVALID )
    {
    }

    Ray_AILightningAction::~Ray_AILightningAction()
    {
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AILightningAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_timedSpawnerAIComponent = m_aiComponent->DynamicCast<TimedSpawnerAIComponent>(ITF_GET_STRINGID_CRC(TimedSpawnerAIComponent,4093386385));
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AILightningAction::onActivate()
    {
        Super::onActivate();

        if ( m_timedSpawnerAIComponent == NULL )
        {
            m_aiBehavior->onActionFinished();
            return;
        }

        // Get binded actor
        if ( m_actor->getChildrenBindHandler().getChildren().size() > 0 )
        {
            m_bindedActor = m_actor->getChildrenBindHandler().getChildren()[0];
        }

        // get index
        if ( ( m_animComponent != NULL ) && getTemplate()->getStartBoneName().isValid() )
        {
            m_startBoneIdx = m_animComponent->getBoneIndex(getTemplate()->getStartBoneName());
        }

        Actor * bindedActor = AIUtils::getActor( m_bindedActor );
        if ( ( bindedActor != NULL ) && getTemplate()->getDestBoneName().isValid() )
        {
            AnimLightComponent * animComp = bindedActor->GetComponent<AnimLightComponent>();
            if ( animComp != NULL )
            {
                m_destBoneIdx = animComp->getBoneIndex(getTemplate()->getDestBoneName());
            }
        }

        // get spawnee
        Actor * spawnee  = m_timedSpawnerAIComponent->getJustSpawnedActor();
        if ( spawnee != NULL )
        {
            m_lightning = spawnee->getRef();

            updatePos();
        }
    }
    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AILightningAction::onDeactivate()
    {
        Super::onDeactivate();
    }	

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AILightningAction::update( f32 _dt )
    {
        Super::update(_dt);

        if ( m_timedSpawnerAIComponent == NULL )
        {
            m_aiBehavior->onActionFinished();
            return;
        }

        updatePos();
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AILightningAction::updatePos()
    {
        Actor * lightning = AIUtils::getActor( m_lightning );
        Actor * bindedActor = AIUtils::getActor( m_bindedActor );

        // get start / dest position
        //
        Vec3d startPos = m_actor->getPos();
        Vec3d destPos;

        if ( ( m_animComponent != NULL )&& ( m_startBoneIdx != U32_INVALID ) )
        {
            m_animComponent->getBonePos(m_startBoneIdx,startPos);
        }
        if ( ( bindedActor != NULL )  )
        {
            AnimLightComponent * animComp = bindedActor->GetComponent<AnimLightComponent>();
            if ( animComp != NULL && m_destBoneIdx != U32_INVALID)
            {
                animComp->getBonePos(m_destBoneIdx,destPos);
            }
            else
            {
                destPos = bindedActor->getPos();
            }
        }        

        if ( lightning != NULL )
        {
            // Use actor Z 
            startPos.m_z += lightning->getTemplate()->getForcedZ();

            // no dest
            if ( destPos.IsEqual( Vec3d::Zero ) || ( getTemplate()->getDefaultSize() == 0.f ) )
            {
                lightning->updateComponentPosAngle( startPos, m_actor->getAngle(), NULL );
            }
            else 
            {
                Vec3d bonesVec = destPos - startPos;
                Vec2d bonesVec2d;
                bonesVec.truncateTo2D( bonesVec2d );
                f32 scale = bonesVec2d.norm() / getTemplate()->getDefaultSize();
                bonesVec2d.normalize();
                f32 angle = bonesVec2d.getAngle() + getTemplate()->getAngleOffset().ToRadians();
                angle = canonizeAngle( angle );


                lightning->updateComponentPosAngle( startPos, angle, NULL );
                lightning->setScale( Vec2d( scale, scale ) );              

                if (getTemplate()->getUseStartStim())
                {
                    const Vec2d startStimPos = startPos.truncateTo2D() + getTemplate()->getStartStimOffset();
                    PhysShapeCircle circle;
                    circle.setRadius(getTemplate()->getStartStimRadius());
                    PunchStim* punchStim = STIMSMANAGER->requestStim<PunchStim>(&circle);
                    if (punchStim)
                    {
                        punchStim->setLevel(getTemplate()->getHitLevel());
                        punchStim->setReceivedHitType(EReceivedHitType (getTemplate()->getHitType()));
                        punchStim->setIsRadial(EReceivedHitType (getTemplate()->getHitType()) == RECEIVEDHITTYPE_HURTBOUNCE);
                        punchStim->setFaction(m_aiComponent->getFaction());
                        punchStim->setPos(startStimPos);
                        punchStim->setPrevPos( startStimPos);
                        punchStim->setSender(m_actor->getRef());
                        punchStim->setDepth(m_actor->getDepth());
                        punchStim->setAngle(0.0f);

                        STIMSMANAGER->sendStim(punchStim);
                    }
                }
                if (getTemplate()->getUseDestStim())
                {
                    const Vec2d destStimPos = destPos.truncateTo2D() + getTemplate()->getStartStimOffset();
                    PhysShapeCircle circle;
                    circle.setRadius(getTemplate()->getDestStimRadius());
                    PunchStim* punchStim = STIMSMANAGER->requestStim<PunchStim>(&circle);
                    if (punchStim)
                    {
                        punchStim->setLevel(getTemplate()->getHitLevel());
                        punchStim->setReceivedHitType(EReceivedHitType (getTemplate()->getHitType()));
                        punchStim->setIsRadial(EReceivedHitType (getTemplate()->getHitType()) == RECEIVEDHITTYPE_HURTBOUNCE);
                        punchStim->setFaction(m_aiComponent->getFaction());
                        punchStim->setPos(destStimPos);
                        punchStim->setPrevPos( destStimPos);
                        punchStim->setSender(m_actor->getRef());
                        punchStim->setDepth(m_actor->getDepth());
                        punchStim->setAngle(0.0f);

                        STIMSMANAGER->sendStim(punchStim);
                    }
                }
            }            
        }    
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AILightningAction::onEvent( Event * _event )
    {
        Super::onEvent( _event );
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AILightningAction::updateAnimInputs()
    {
        Super::updateAnimInputs();
    }


    //--------------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_AILightningAction_Template)

        BEGIN_SERIALIZATION_CHILD(Ray_AILightningAction_Template)
        SERIALIZE_MEMBER( "startBoneName", m_startBone );
    SERIALIZE_MEMBER( "destBoneName", m_destBoneName );
    SERIALIZE_MEMBER( "defaultSize", m_defaultSize );
    SERIALIZE_MEMBER( "angleOffset", m_angleOffset );
    SERIALIZE_MEMBER( "hitLevel", m_hitLevel);
    SERIALIZE_MEMBER( "hitType", m_hitType);
    SERIALIZE_MEMBER( "useStartStim", m_useStartStim);
    SERIALIZE_MEMBER( "useDestStim", m_useDestStim);
    SERIALIZE_MEMBER( "startStimRadius", m_startStimRadius);
    SERIALIZE_MEMBER( "destStimRadius", m_destStimRadius);
    SERIALIZE_MEMBER( "startStimOffset", m_startStimOffset);
    SERIALIZE_MEMBER( "destStimOffset", m_destStimOffset);

    END_SERIALIZATION()

        Ray_AILightningAction_Template::Ray_AILightningAction_Template()
        :Super()
        , m_defaultSize( 0.f )
        , m_hitLevel(1)
        , m_hitType(u32(RECEIVEDHITTYPE_HURTBOUNCE))
        , m_useStartStim(bfalse)
        , m_useDestStim(bfalse)
        , m_startStimRadius(1.5f)
        , m_destStimRadius(0.5f)
        , m_startStimOffset(0.0f,0.0f)
        , m_destStimOffset(0.0f,0.0f)
    {
    }


}