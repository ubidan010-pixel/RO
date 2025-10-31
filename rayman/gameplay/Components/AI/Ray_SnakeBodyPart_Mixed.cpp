#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SNAKEBODYPART_MIXED_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Mixed.h"
#endif //_ITF_RAY_SNAKEBODYPART_MIXED_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPart_Template
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    IMPLEMENT_OBJECT_RTTI(BodyPart_Template)
    
    BEGIN_SERIALIZATION_CHILD(BodyPart_Template)
        SERIALIZE_OBJECT("actorRenderer", m_actorRenderer);
        SERIALIZE_OBJECT("spriteRenderer", m_spriteRenderer);
        SERIALIZE_MEMBER("health", m_health);
        SERIALIZE_MEMBER("destroyOnDeath", m_destroyOnDeath);
        SERIALIZE_CONTAINER("damageLevels", m_damageLevels);
        SERIALIZE_MEMBER("leftHitAnim", m_leftHitAnim);
        SERIALIZE_MEMBER("rightHitAnim", m_rightHitAnim);
        SERIALIZE_MEMBER("deathAnim", m_deathAnim);
    END_SERIALIZATION()

    BodyPart_Template::BodyPart_Template() 
        : m_health(-1)
        , m_destroyOnDeath(btrue)
    {
        m_damageLevels.resize(AIComponent::s_maxHitLevels);
        m_damageLevels[0] = 5;
        m_damageLevels[1] = 25;
        m_damageLevels[2] = 50;
        m_damageLevels[3] = 100;
    }



    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPart
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    IMPLEMENT_OBJECT_RTTI(BodyPart)

    //-----------------------------------------------------------------------------------------------------------------------------
    void BodyPart::onInit()
    {
        Super::onInit();

        m_renderMode = getTemplate()->getRenderMode();

        switch (m_renderMode)
        {
        case RenderMode_Actor:
            {
                m_renderer = &m_actorRenderer;
                m_actorRenderer.onInit(&getTemplate()->getActorRenderer(), this);
            }
            break;

        case RenderMode_Sprite:
            {
                m_renderer = &m_spriteRenderer;
                m_spriteRenderer.onInit(&getTemplate()->getSpriteRenderer());
            }
            break;
        }

        m_curHealth = getTemplate()->getHealth();
    }

    void BodyPart::onBecomeActive()
    {
        switch (m_renderMode)
        {
        case RenderMode_Actor:
            {
                spawnBodyPart();
            }
            break;
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------------
    void BodyPart::playAnim( const StringID & _anim )
    {
        if ( m_renderer->playAnim(_anim) )
        {
            m_curPlayingAnim = _anim;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    void BodyPart::update( f32 _dt )
    {
        Super::update(_dt);

        if ( m_curPlayingAnim.isValid() &&
             !m_renderer->isAnimLooped() &&
             m_renderer->isAnimFinished() )
        {
            onAnimFinished();
        }

        m_renderer->update(_dt);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    // the broadcastEventToPart event flag must be set on snake to have this function called 
    void BodyPart::onEvent( Event* _event )
    {
        switch (m_renderMode)
        {
        case RenderMode_Actor:
            {
                if ( getAnimComponent() &&
                     _event->getSender() == getActorRef() &&
                     getTemplate()->getActorRenderer().getEndAnimMRK().isValid() )
                {
                    if ( AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
                    {
                        if ( animEvent->getName() == getTemplate()->getActorRenderer().getEndAnimMRK() )
                        {
                            onAnimFinished();
                        }
                    }
                }
            }
            break;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    void BodyPart::onAnimFinished()
    {
        ITF_ASSERT_MSG(m_renderMode == RenderMode_Actor, "RenderMode_Sprite doesn't support this function yet.");

        if ( getAnimComponent() == NULL )
        {
            return;
        }

        if ( m_curPlayingAnim.isValid() )
        {
            bbool isDeathAnim = ( m_curPlayingAnim == getTemplate()->getDeathAnim() ) ? btrue : bfalse;
            if ( isDeathAnim && ( m_curHealth == 0 ) )
            {
                m_isDead = btrue;
            }

            // no destruction and death anim finished, don't restart the default anim
            if ( !( isDeathAnim && !getTemplate()->destroyOnDeath() ) )
            {
                getAnimComponent()->setAnim( getAnimComponent()->getDefaultAnimation() );
            }
            m_curPlayingAnim.invalidate();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    bbool BodyPart::receiveHit( HitStim * _hitStim, bbool _leftSide )
    {
        // invincible ?
        if ( m_curHealth < 0 )
            return bfalse;

        if ( m_curHealth == 0 )
        {
            return btrue;
        }

        ITF_ASSERT( _hitStim->getLevel() < AIComponent::s_maxHitLevels );
        if ( _hitStim->getLevel() < AIComponent::s_maxHitLevels )
        {
            m_curHealth = Max( m_curHealth - i32( getTemplate()->getDamageLevels()[ _hitStim->getLevel() ] ), 0 );

            // death
            if ( m_curHealth == 0 )
            {
                if ( getTemplate()->getDeathAnim().isValid() )
                {
                    playAnim( getTemplate()->getDeathAnim() );
                }
                else
                {
                    // if the dead animation is valid, wait the end of the animation to change the flag
                    m_isDead = btrue;
                }
            }
            else
            {
                if ( _leftSide )
                {
                    playAnim( getTemplate()->getLeftHitAnim() );
                }
                else
                {
                    playAnim( getTemplate()->getRightHitAnim() );
                }
            }
            return btrue;
        }
        return bfalse;
    }





    //-----------------------------------------------------------------------------------------------------------------------------
    Actor* BodyPart::createBodyPart()
    {
        ITF_ASSERT(m_renderMode == RenderMode_Actor);

        return m_actorRenderer.m_spawner.getSpawnee(m_parent->getScene(), m_parent->getPos());
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    Actor* BodyPart::spawnBodyPart()
    {
        ITF_ASSERT(m_renderMode == RenderMode_Actor);
        ITF_ASSERT_CRASH(m_parent, "did you call initialize?");
        ITF_ASSERT( getActorRef().getActor() == NULL );    // not already created


        Actor* bodyPartActor = createBodyPart();

        if (!bodyPartActor)
        {
            String8 error;error.setTextFormat("unable to spawn body part");
            ITF_WARNING(m_parent, 0, error.cStr());
            m_parent->setDataError(error);
            return NULL;
        }


        AIUtils::setAlwaysActive(bodyPartActor, btrue);
        bodyPartActor->setAngle(m_parent->getAngle());
        bodyPartActor->setScale(m_parent->getScale());

        m_actorRenderer.m_actorRef = bodyPartActor->getRef();
        m_isDead = bfalse;

        // register anim gameplay event 
        if ( getTemplate()->getActorRenderer().getEndAnimMRK().isValid() )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(bodyPartActor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_parentComponent);
        }

        return bodyPartActor;
    }

#ifdef ITF_SUPPORT_EDITOR
    void BodyPart::onEditorMove()
    {
        Super::onEditorMove();

        switch (m_renderMode)
        {
        case RenderMode_Actor:
            {
                if ( Actor* actor = getActorRef().getActor() )
                {
                    actor->onEditorMove();
                }
            }
            break;
        }
    }
#endif // ITF_SUPPORT_EDITOR


    void BodyPart::addPolylinePoints_sprite( PolyLine& _poly )
    {
        //Super::addPolylinePoints_sprite(_poly);
        ITF_ASSERT(m_renderMode == RenderMode_Sprite);

        m_rightFirstEdgeIdx = _poly.getPosCount();
        m_rightLastEdgeIdx  = m_spriteRenderer.addPolylinePoints(_poly, this);
    }

    void BodyPart::addOtherPolylinePoints_sprite( PolyLine& _poly )
    {
        //Super::addOtherPolylinePoints_sprite(_poly);
        ITF_ASSERT(m_renderMode == RenderMode_Sprite);

        m_leftFirstEdgeIdx = _poly.getPosCount() - 1;
        m_leftLastEdgeIdx = m_spriteRenderer.addOtherPolylinePoints(_poly, this);

        // because of the last body part
        if ( ( getIndex() == ( getBodyPartCount() - 1 ) ) && ( getIndex() != 0 ) )
        {
            m_leftFirstEdgeIdx = m_rightFirstEdgeIdx;
            m_leftLastEdgeIdx = m_leftLastEdgeIdx - 1;
        }        
    }

};