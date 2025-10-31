#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DANCEEYECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceEyeComponent.h"
#endif //_ITF_RAY_DANCEEYECOMPONENT_H_

#ifndef _ITF_RAY_DANCEDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceDoorComponent.h"
#endif //_ITF_RAY_DANCEDOORCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#define STATE_LOCK                      0
#define STATE_OCCUPIED                  1
#define STATE_UNLOCK                    2
#define STATE_LOCK_to_OCCUPIED          3
#define STATE_OCCUPIED_to_UNLOCK        4
#define STATE_UNLOCK_to_OCCUPIED        5
#define STATE_OCCUPIED_to_LOCK          6
#define STATE_UNLOCK_to_OFF             7
#define STATE_DOOR_OPEN                 8

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_DanceEyeComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_DanceEyeComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_DanceEyeComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "Ray_DanceEyeComponent requires an AnimLightComponent/AnimatedComponent.");
    END_VALIDATE_COMPONENT()

    // ****************************************************************************

    Ray_DanceEyeComponent::Ray_DanceEyeComponent()
    : m_animComponent(NULL)
    , m_fxControllerComponent(NULL)
    , m_alreadyRegistred(bfalse)
    , m_occupiedByNPC(bfalse)
    , m_occupiedByNPC_Temp(bfalse)
    , m_occupiedByPlayer(bfalse)
    , m_occupiedByPlayer_Temp(bfalse)
    , m_unlock(bfalse)
    , m_unlock_Temp(bfalse)
    , m_state(STATE_LOCK)
    , m_refStone(ITF_INVALID_OBJREF)
    , m_doorIsReadyForOpen(bfalse)
    , m_isReadyForDoorOpen(bfalse)
    , m_angle(0.0f)
    , m_dist(0.0f)
    , m_handleFX(U32_INVALID)
    {
        // none
    }

    // ****************************************************************************

    Ray_DanceEyeComponent::~Ray_DanceEyeComponent()
    {
        // none
    }

    // ****************************************************************************

    void Ray_DanceEyeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get Components
        m_animComponent = m_actor->GetComponent<AnimatedComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        // Init state
        changeState(STATE_LOCK);

    }

    // ****************************************************************************

    void Ray_DanceEyeComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    // ****************************************************************************

    void Ray_DanceEyeComponent::onResourceReady()
    {
        m_parent = m_actor->getParent();

        if (!m_alreadyRegistred)
        {
            if (m_parent.isValid())
            {
                Actor* parentAct = m_parent.getActor();
                if (parentAct)
                {
                    Ray_DanceDoorComponent* danceDoorComponent = parentAct->GetComponent<Ray_DanceDoorComponent>();
                    if (danceDoorComponent)
                    {
                        // Register on parent (door)
                        danceDoorComponent->registerEye(m_actor->getRef());
                        m_alreadyRegistred = btrue;
                    }
                }
            }
        }
    }

    //*****************************************************************************

    void Ray_DanceEyeComponent::startFX()
    {
        const StringID fxName = ITF_GET_STRINGID_CRC(Prtcl_Oukdoor,2441837687);

        if (m_fxControllerComponent)
        {
            if (fxName.isValid() && m_handleFX == U32_INVALID)
            {
                m_handleFX = m_fxControllerComponent->playFX(fxName);
                m_fxControllerComponent->setFXPos(m_handleFX,m_actor->getPos()); 
            }
        }
    }

    //*****************************************************************************

    void Ray_DanceEyeComponent::stopFX()
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->stopFX(m_handleFX);
            m_handleFX = U32_INVALID;
            m_curDistPartciles = 0.0f;
        }
    }

    //*****************************************************************************

    void Ray_DanceEyeComponent::moveFX(Vec3d _pos)
    {
        if (m_fxControllerComponent && m_handleFX != U32_INVALID)
        {
            m_fxControllerComponent->setFXPos(m_handleFX, _pos); 
        }
    }

    // ****************************************************************************

    void Ray_DanceEyeComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_state == STATE_DOOR_OPEN)
        {
            return;
        }

        m_parent = m_actor->getParent();
        ITF_WARNING(m_actor, m_parent.isValid(), "This eye is not bind on a Oukoum door eye !");
        if (m_parent.isValid())
        {
            Actor* parentAct = m_parent.getActor();
            if (parentAct)
            {
                Ray_DanceDoorComponent* danceDoorComponent = parentAct->GetComponent<Ray_DanceDoorComponent>();
                if (danceDoorComponent)
                {
                    // Register on parent (door)
                    if (!m_alreadyRegistred)
                    {
                        danceDoorComponent->registerEye(m_actor->getRef());
                        m_alreadyRegistred = btrue;
                    }

                    // Check if door is ready for opening
                    m_doorIsReadyForOpen = danceDoorComponent->isReadyForOpen();

                }
            }
        }
       
        // Replicate data 
        m_posStone          = m_posStone_Temp;
        m_occupiedByNPC     = m_occupiedByNPC_Temp;
        m_occupiedByPlayer  = m_occupiedByPlayer_Temp;
        m_unlock            = m_unlock_Temp;

        // Change state if is needed
        if (m_doorIsReadyForOpen)
        {
            if (m_state == STATE_UNLOCK_to_OFF)
            {
                if (m_animComponent->isMainNodeFinished())
                {
                    changeState(STATE_DOOR_OPEN);
                }
            }
            else
            {
                changeState(STATE_UNLOCK_to_OFF);
            }         
        }
        else
        {
            switch (m_state)
            {
            // Main states
            case STATE_LOCK:
                if (m_occupiedByNPC || m_occupiedByPlayer)
                {
                    changeState(STATE_LOCK_to_OCCUPIED);
                }                
                break;

            case STATE_OCCUPIED:
                if (m_unlock)
                {
                    changeState(STATE_OCCUPIED_to_UNLOCK);
                }
                else if (!m_occupiedByNPC && !m_occupiedByPlayer)
                {
                    changeState(STATE_OCCUPIED_to_LOCK);
                }
                break;

            case STATE_UNLOCK:
                if (!m_unlock)
                {
                    changeState(STATE_UNLOCK_to_OCCUPIED);
                }
                break;

                // Transition states
            case STATE_LOCK_to_OCCUPIED:
                if (m_animComponent->isMainNodeFinished())
                {
                    changeState(STATE_OCCUPIED);
                }
                break;

            case STATE_OCCUPIED_to_UNLOCK:
                if (m_animComponent->isMainNodeFinished())
                {
                    changeState(STATE_UNLOCK);
                }
                break;

            case STATE_UNLOCK_to_OCCUPIED:
                if (m_animComponent->isMainNodeFinished())
                {
                    changeState(STATE_OCCUPIED);
                }
                break;

            case STATE_OCCUPIED_to_LOCK:
                if (m_animComponent->isMainNodeFinished())
                {
                    changeState(STATE_LOCK);
                }
                break;
            }
        }
       
        getAngleAndDist(m_posStone);

        updateGodrayParticles(_dt);

        // Update input animation
        const StringID  inputScale = ITF_GET_STRINGID_CRC(GodrayScaleCursor,4131582659);
        const StringID  inputRotation = ITF_GET_STRINGID_CRC(GodrayRotationCursor,573469756);
        m_animComponent->setInput(inputRotation,  Angle::Radians(m_angle).ToDegrees360());
        m_animComponent->setInput(inputScale, m_dist);
        

    }

    // ****************************************************************************

    void Ray_DanceEyeComponent::updateGodrayParticles( f32 _dt )
    {
        Vec3d vecToStone = (m_posStone - m_actor->getPos());
        vecToStone.normalize();

        Vec3d newParticlesPos = m_actor->getPos();

        if (m_state != STATE_LOCK)
        {
            m_curDistPartciles += getTemplate()->getSpeedMoveParticles() * _dt;
            if (m_curDistPartciles > m_dist)
            {
                m_curDistPartciles = 0.0f;
            }
        
            newParticlesPos = m_actor->getPos() + vecToStone * m_curDistPartciles;
            moveFX(newParticlesPos);
        }

#ifndef ITF_FINAL
        //DebugDraw::circle(newParticlesPos, 0.30f, Color::green(), 0.0f);       
#endif // ITF_FINAL
    }

    // ****************************************************************************

    void Ray_DanceEyeComponent::getAngleAndDist(Vec3d _target)
    {

        Vec2d posTarget = _target.truncateTo2D();
        Vec2d vecToTarget = (posTarget - m_actor->get2DPos());

        // Compute distance
        f32 distance = vecToTarget.norm();

        // Actor orientation
        Vec2d dirActor = Vec2d::Right.Rotate(m_actor->getAngle());

        vecToTarget.normalize();
        dirActor.normalize();

        // Compute angle
        f32 angle = vecToTarget.getOrientedAngle(dirActor); 
        if (m_actor->isFlipped()) 
        {
            angle = MTH_PI - angle;
        }

        // Set input
        m_angle = angle;
        m_dist  = distance;

    }


    // ****************************************************************************

    void Ray_DanceEyeComponent::changeState( int _newState )
    {
        switch(_newState)
        {
            // Change to Main State
            case STATE_LOCK:
                playAnim(getTemplate()->getLockAnim());
                break;

            case STATE_OCCUPIED:
                playAnim(getTemplate()->getOccupiedAnim());
                break;

            case STATE_UNLOCK:
                playAnim(getTemplate()->getUnlockAnim());
                break;

            // Change to Transition State
            case STATE_LOCK_to_OCCUPIED:
                playAnim(getTemplate()->getLock_to_occupied_Anim());
                break;

            case STATE_OCCUPIED_to_UNLOCK:
                playAnim(getTemplate()->getOccupied_to_unlock_Anim());
                startFX();
                break;

            case STATE_UNLOCK_to_OCCUPIED:
                playAnim(getTemplate()->getUnlock_to_occupied_Anim());
                stopFX();
                break;

            case STATE_OCCUPIED_to_LOCK:
                playAnim(getTemplate()->getOccupied_to_lock_Anim());
                break;

            case STATE_UNLOCK_to_OFF:
                playAnim(getTemplate()->getUnlock_to_off_Anim());
                stopFX();
                break;

            case STATE_DOOR_OPEN:
                m_isReadyForDoorOpen = btrue;
                break;

        }
    
        m_state = _newState;
    }

    // ****************************************************************************

    void Ray_DanceEyeComponent::updateStone( ObjectRef _stone, Vec3d _stonePos, bbool _occupiedByNPC, bbool _occupiedByPlayer, bbool _unlocked)
    {
        m_refStone                  = _stone;
        m_posStone_Temp             = _stonePos;;
        m_occupiedByNPC_Temp        = _occupiedByNPC;
        m_occupiedByPlayer_Temp     = _occupiedByPlayer;
        m_unlock_Temp               = _unlocked;
    }

    // ****************************************************************************

    bbool Ray_DanceEyeComponent::isOccupiedByNPC() const
    {
        return m_occupiedByNPC;
    }

    // ****************************************************************************

    bbool Ray_DanceEyeComponent::isOccupiedByPlayer() const
    {
        return m_occupiedByPlayer;
    }

    // ****************************************************************************

    bbool Ray_DanceEyeComponent::isOccupied() const
    {
        return (m_occupiedByNPC || m_occupiedByPlayer);
    }

    // ****************************************************************************

    bbool Ray_DanceEyeComponent::isUnlock() const
    {
        return m_unlock;
    }

    // ****************************************************************************

    bbool Ray_DanceEyeComponent::isReadyForDoorOpen() const
    {
        return m_isReadyForDoorOpen;
    }

    // ****************************************************************************

    bbool Ray_DanceEyeComponent::playAnim( const StringID& _anim )
    {
        ITF_WARNING(m_actor, _anim.isValid(), "I wan't play an invalid anim !");

        if (_anim.isValid())
        {
            m_animComponent->setAnim(_anim);
            return btrue;
        }

        return bfalse;
    }


    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_DanceEyeComponent_Template)

    BEGIN_SERIALIZATION(Ray_DanceEyeComponent_Template)
        SERIALIZE_MEMBER("lockAnim",     m_lockAnim);
        SERIALIZE_MEMBER("occupiedAnim", m_occupiedAnim);
        SERIALIZE_MEMBER("unlockAnim",   m_unlockAnim);

        SERIALIZE_MEMBER("lock_to_occupied_Anim",   m_lock_to_occupied_Anim);
        SERIALIZE_MEMBER("occupied_to_unlock_Anim", m_occupied_to_unlock_Anim);
        SERIALIZE_MEMBER("unlock_to_occupied_Anim", m_unlock_to_occupied_Anim);
        SERIALIZE_MEMBER("occupied_to_lock_Anim",   m_occupied_to_lock_Anim);
        SERIALIZE_MEMBER("unlock_to_off_Anim",      m_unlock_to_off_Anim);

        SERIALIZE_MEMBER("speedMoveParticles",      m_speedMoveParticles);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_DanceEyeComponent_Template::Ray_DanceEyeComponent_Template()
    : m_speedMoveParticles(4.0f)
    {
        // none
    }
    
    //*****************************************************************************
    
    Ray_DanceEyeComponent_Template::~Ray_DanceEyeComponent_Template()
    {
        // none
    }

    //*****************************************************************************

};
