#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIReceiveCameraEjectHitAction.h"
#endif //_ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_


namespace ITF
{
    static StringID s_EjectedInput = "Eject";

    IMPLEMENT_OBJECT_RTTI(Ray_AIReceiveCameraEjectHitAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIReceiveCameraEjectHitAction_Template)
        SERIALIZE_MEMBER("minStartSpeed", m_minStartSpeed);
        SERIALIZE_MEMBER("maxStartSpeed", m_maxStartSpeed);
        SERIALIZE_MEMBER("zMinSpeed", m_zMinSpeed);
        SERIALIZE_MEMBER("zAcceleration", m_zAcceleration);
        SERIALIZE_MEMBER("gravityMultiplier", m_gravityMultiplier);
        SERIALIZE_MEMBER("ejectDuration", m_ejectDuration);
        SERIALIZE_MEMBER("zForced", m_zForced);
        SERIALIZE_MEMBER("rotationSpeed", m_rotationSpeed );
        SERIALIZE_MEMBER("fixedEjectDir", m_fixedEjectDir );
        SERIALIZE_MEMBER("fadeDuration",    m_fadeDuration );
        SERIALIZE_MEMBER("delayBeforeFade", m_delayBeforeFade );
    END_SERIALIZATION()

    Ray_AIReceiveCameraEjectHitAction_Template::Ray_AIReceiveCameraEjectHitAction_Template()
    : m_minStartSpeed(1.f, 1.f, 15.f)
    , m_maxStartSpeed(2.f, 2.f, 20.f)
    , m_zMinSpeed(7.f)
    , m_zAcceleration(20.f)
    , m_gravityMultiplier(0.1f)
    , m_ejectDuration(5.f)
    , m_zForced( 0 )
    , m_rotationSpeed( 0.0f )
    , m_fadeDuration( 0.f )
    , m_delayBeforeFade( -1.f )
    , m_fixedEjectDir(Vec2d::Zero)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(RECEIVEHIT,848873176);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIReceiveCameraEjectHitAction)

    Ray_AIReceiveCameraEjectHitAction::Ray_AIReceiveCameraEjectHitAction()
    : Super()
    , m_ejectSpeed(Vec3d::Zero)
    , m_ejectTimer(0.f)
    {
    }

    Ray_AIReceiveCameraEjectHitAction::~Ray_AIReceiveCameraEjectHitAction()
    {
    }

    void Ray_AIReceiveCameraEjectHitAction::onActivate()
    {
        Super::onActivate();


        if ( StickToPolylinePhysComponent* physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>() )
        {
            physComponent->setDisabled(btrue);
        }

        if ( GetActor()->getParentBind() )
        {
            GetActor()->getParentBind()->setRuntimeDisabled( btrue );
        }
        else if ( GetActor()->getBinding()->isBound() )
        {
            GetActor()->getBinding()->unbindFromParent();
        }

        Vec2d ejectSpeed;

        ejectSpeed.m_x = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinStartSpeed().m_x, getTemplate()->getMaxStartSpeed().m_x);
        ejectSpeed.m_y = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinStartSpeed().m_y, getTemplate()->getMaxStartSpeed().m_y);
        m_ejectSpeed.m_z = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinStartSpeed().m_z, getTemplate()->getMaxStartSpeed().m_z);

        if ( !getTemplate()->getFixedEjectDir().IsEqual( Vec2d::Zero ) )
        {
            ejectSpeed *= getTemplate()->getFixedEjectDir();
        }
        else if ( m_hitDir.m_x < 0.f )
        {
            ejectSpeed.m_x *= -1.f;
        }

        if ( getTemplate()->getZForced() == 0 )
        {
            if ( Seeder::getSharedSeeder().GetFloat() <= 0.5f )
            {
                m_ejectSpeed.m_z *= -1.f;
            }
        }
        else
        {
            m_ejectSpeed.m_z *= getTemplate()->getZForced() > 0 ? 1.f : -1.f;
        }

        f32 gravAngle = getVec2Angle(m_aiComponent->getCurrentGravityDir().getPerpendicular());
        if ( gravAngle )
        {
            ejectSpeed.Rotate(gravAngle);
        }

        m_ejectSpeed.m_x = ejectSpeed.m_x;
        m_ejectSpeed.m_y = ejectSpeed.m_y;

        m_ejectTimer = getTemplate()->getEjectDuration();

        if ( m_fxController != NULL )
        {
            u32 fxHandle = m_fxController->playFX( s_EjectedInput );
            m_fxController->setFXPosFromHandle( fxHandle, m_actor->getPos() );
        }   
    }

    void Ray_AIReceiveCameraEjectHitAction::update( f32 _dt )
    {
        Super::update(_dt);

        if ( m_ejectSpeed.m_z > 0.f )
        {
            m_ejectSpeed.m_z -= getTemplate()->getZAcceleration() * _dt;
            m_ejectSpeed.m_z = Max(m_ejectSpeed.m_z,getTemplate()->getZMinSpeed());
        }
        else
        {
            m_ejectSpeed.m_z += getTemplate()->getZAcceleration() * _dt;
            m_ejectSpeed.m_z = Min(m_ejectSpeed.m_z,-getTemplate()->getZMinSpeed());
        }

        f32 rotAngle = 0.0f;
        rotAngle = getTemplate()->getRotationSpeed() * _dt;

        Vec2d gravity = m_aiComponent->getCurrentGravity() * getTemplate()->getGravityMultiplier() * _dt;
        m_ejectSpeed.m_x += gravity.m_x;
        m_ejectSpeed.m_y += gravity.m_y;

        Vec3d pos = m_actor->getPos();
        pos += m_ejectSpeed * _dt;
        m_actor->setPos(pos);
        m_actor->setAngle( canonizeAngle( m_actor->getAngle() + rotAngle ) );
        
        f32 prevDelay = getTemplate()->getEjectDuration() - m_ejectTimer;

        m_ejectTimer -= _dt;
        if ( m_ejectTimer <= 0.f )
        {
            m_aiBehavior->onActionFinished();
        }

        // start fade
        if (    ( getTemplate()->getDelayBeforeFade() > 0.0f ) && 
                ( prevDelay < getTemplate()->getDelayBeforeFade() ) && 
                ( ( getTemplate()->getEjectDuration() - m_ejectTimer ) >= getTemplate()->getDelayBeforeFade() ) 
           )
        {
            EventShow hide( 0.f, getTemplate()->getFadeDuration() );
            m_actor->onEvent( &hide );
        }
    }
}
