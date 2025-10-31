#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SNAKEBODYPART_DESTRUCTIBLE_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Destructible.h"
#endif //_ITF_RAY_SNAKEBODYPART_DESTRUCTIBLE_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

namespace ITF
{
    //-----------------------------------------------------------------------------------------------------------------------------
    // DESTRUCTIBLE BODY PART
    //-----------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI( DestructibleBodyPart )

    DestructibleBodyPart::DestructibleBodyPart()
        :Super()
        , m_ejectInitialized( bfalse )
        , m_ejectSpeed(Vec3d::Zero)
        , m_ejectTimer( 0.0f )
        , m_reward( NULL )
    {
    }
    //-----------------------------------------------------------------------------------------------------------------------------
    DestructibleBodyPart::~DestructibleBodyPart()
    {
        SF_DEL(m_reward);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    Actor* DestructibleBodyPart::spawnBodyPart()
    {
        return Super::spawnBodyPart();
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    void DestructibleBodyPart::onInit()
    {
        Super::onInit();

        // Init reward
        //
        if ( ( m_reward == NULL ) && ( getTemplate()->getReward() != NULL ) )
        {
            m_reward = getTemplate()->getReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));;
            BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getReward()), m_reward );
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    bbool DestructibleBodyPart::removeAndKeepAliveOnDeath() 
    { 
        return ( getTemplate() ? getTemplate()->ejectOnDeath() : bfalse ); 
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    bbool DestructibleBodyPart::receiveHit( HitStim * _hitStim, bbool _leftSide )
    {
        i32 prevHealth = m_curHealth;
        bbool result = Super::receiveHit( _hitStim, _leftSide );

        // Send reward
        //
        if ( ( m_reward != NULL ) && ( prevHealth > 0 ) && ( m_curHealth == 0 ) && _hitStim->getSender().isValid() )
        {
            m_reward->clear();
            m_reward->setScene( getParent()->getScene() );
            m_reward->setPos( getPos() );
            m_reward->setRewardReceiver( _hitStim->getOriginalSender().isValid() ? _hitStim->getOriginalSender() : _hitStim->getSender() );
            m_reward->setEjectionDir( -Vec2d::Right );
            RAY_GAMEMANAGER->onEvent(m_reward);
        }
        
        // eject ?
        //
        if ( ( m_curHealth == 0 ) && !m_ejectInitialized && getTemplate()->ejectOnDeath() )
        {
            m_ejectInitialized = btrue;
            Vec2d ejectSpeed;

            ejectSpeed.m_x = Seeder::getSharedSeeder().GetFloat(getTemplate()->getEjectMinStartSpeed().m_x, getTemplate()->getEjectMaxStartSpeed().m_x);
            ejectSpeed.m_y = Seeder::getSharedSeeder().GetFloat(getTemplate()->getEjectMinStartSpeed().m_y, getTemplate()->getEjectMaxStartSpeed().m_y);
            m_ejectSpeed.m_z = Seeder::getSharedSeeder().GetFloat(getTemplate()->getEjectMinStartSpeed().m_z, getTemplate()->getEjectMaxStartSpeed().m_z);

            if ( !getTemplate()->getEjectFixedEjectDir().IsEqual( Vec2d::Zero ) )
            {
                ejectSpeed *= getTemplate()->getEjectFixedEjectDir();
            }
           /* else if ( m_hitDir.m_x < 0.f )
            {
                ejectSpeed.m_x *= -1.f;
            }*/

            if ( getTemplate()->getEjectZForced() == 0 )
            {
                if ( Seeder::getSharedSeeder().GetFloat() <= 0.5f )
                {
                    m_ejectSpeed.m_z *= -1.f;
                }
            }
            else
            {
                m_ejectSpeed.m_z *= getTemplate()->getEjectZForced() > 0 ? 1.f : -1.f;
            }

            m_ejectSpeed.m_x = ejectSpeed.m_x;
            m_ejectSpeed.m_y = ejectSpeed.m_y;

            m_ejectTimer = getTemplate()->getEjectDuration();
        }

        return result;
    }
    //-----------------------------------------------------------------------------------------------------------------------------
    bbool DestructibleBodyPart::updateKeepAlive( f32 _dt )
    {
        m_renderer->update(_dt);

        bbool destroy = btrue;

        if ( m_ejectInitialized )
        {
            if ( m_ejectSpeed.m_z > 0.f )
            {
                m_ejectSpeed.m_z -= getTemplate()->getEjectZAcceleration() * _dt;
                m_ejectSpeed.m_z = Max(m_ejectSpeed.m_z,getTemplate()->getEjectZMinSpeed());
            }
            else
            {
                m_ejectSpeed.m_z += getTemplate()->getEjectZAcceleration() * _dt;
                m_ejectSpeed.m_z = Min(m_ejectSpeed.m_z,-getTemplate()->getEjectZMinSpeed());
            }

            f32 rotAngle = 0.0f;
            rotAngle = getTemplate()->getEjectRotationSpeed() * _dt;

            Vec2d gravity = Vec2d( 0.0f, -1.0f ) * getTemplate()->getEjectGravityMultiplier() * _dt;
            m_ejectSpeed.m_x += gravity.m_x;
            m_ejectSpeed.m_y += gravity.m_y;

            setPos( getPos() + m_ejectSpeed * _dt );
            setAngle( canonizeAngle( getAngle() + rotAngle ) );

            const f32 prevDelay = getTemplate()->getEjectDuration() - m_ejectTimer;
            m_ejectTimer -= _dt;
            const f32 newDelay = getTemplate()->getEjectDuration() - m_ejectTimer;

            if ( getTemplate()->getEjectDelayBeforeFade() > 0.0f )
            {
                // we want to fade
                switch (m_renderMode)
                {
                case RenderMode_Actor:
                    {
                        if ( prevDelay < getTemplate()->getEjectDelayBeforeFade() && 
                             newDelay >= getTemplate()->getEjectDelayBeforeFade() )
                        {
                            // we just went over the limit
                            EventShow hide( 0.f, getTemplate()->getEjectFadeDuration() );
                            getActor()->onEvent( &hide );
                        }
                    }
                    break;

                case RenderMode_Sprite:
                    {
                        if (newDelay > getTemplate()->getEjectDelayBeforeFade() )
                        {
                            Color color = m_spriteRenderer.getTemplate()->getColor();
                            color.m_a = Max(0.f, 1 - (newDelay - getTemplate()->getEjectDelayBeforeFade()) / getTemplate()->getEjectFadeDuration());
                            m_spriteRenderer.setColor(color);
                        }
                    }
                    break;
                }
            }


            // destroy the part as soon as it gets offscreen
            destroy = !CAMERA->isRectVisible(getAABB(), getPos().m_z);
        }

        return destroy;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(DestructibleBodyPart_Template)

    BEGIN_SERIALIZATION_CHILD(DestructibleBodyPart_Template)
        SERIALIZE_MEMBER("ejectOnDeath",            m_ejectOnDeath );

        SERIALIZE_MEMBER("ejectMinStartSpeed",      m_ejectMinStartSpeed);
        SERIALIZE_MEMBER("ejectMaxStartSpeed",      m_ejectMaxStartSpeed);
        SERIALIZE_MEMBER("ejectzMinSpeed",          m_ejectzMinSpeed);
        SERIALIZE_MEMBER("ejectzAcceleration",      m_ejectzAcceleration);
        SERIALIZE_MEMBER("ejectGravityMultiplier",  m_ejectGravityMultiplier);
        SERIALIZE_MEMBER("ejectDuration",           m_ejectDuration);
        SERIALIZE_MEMBER("ejectzForced",            m_ejectzForced);
        SERIALIZE_MEMBER("ejectRotationSpeed",      m_ejectRotationSpeed );
        SERIALIZE_MEMBER("ejectFixedEjectDir",      m_ejectFixedEjectDir );
        SERIALIZE_MEMBER("ejectFadeDuration",       m_ejectFadeDuration );
        SERIALIZE_MEMBER("ejectDelayBeforeFade",    m_ejectDelayBeforeFade );

        SERIALIZE_OBJECT_WITH_FACTORY("reward",     m_reward, ACTORSMANAGER->getEventFactory() );
    END_SERIALIZATION()

    DestructibleBodyPart_Template::DestructibleBodyPart_Template()
        :Super()
        , m_ejectOnDeath( bfalse )
        , m_ejectMinStartSpeed(1.f, 1.f, 15.f)
        , m_ejectMaxStartSpeed(2.f, 2.f, 20.f)
        , m_ejectzMinSpeed(7.f)
        , m_ejectzAcceleration(20.f)
        , m_ejectGravityMultiplier(0.1f)
        , m_ejectDuration(5.f)
        , m_ejectzForced( 0 )
        , m_ejectRotationSpeed( 0.0f )
        , m_ejectFixedEjectDir( 0.0f, 0.0f )
        , m_ejectFadeDuration( 0.f )
        , m_ejectDelayBeforeFade( -1.f )
        , m_reward( NULL )
    {
    }

    DestructibleBodyPart_Template::~DestructibleBodyPart_Template()
    {
        SF_DEL( m_reward );
    }

};
