#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MAMAEYESAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_MamaEyesAIComponent.h"
#endif //_ITF_RAY_MAMAEYESAICOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_MamaEyesAIComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_MamaEyesAIComponent)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_MamaEyesAIComponent)
        END_VALIDATE_COMPONENT()

    Ray_MamaEyesAIComponent::Ray_MamaEyesAIComponent()
        : Super()
        , m_speed(Vec3d::Zero)
        , m_rotation(2.f)
    {
    }

    Ray_MamaEyesAIComponent::~Ray_MamaEyesAIComponent()
    {
    }

    void Ray_MamaEyesAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        setInitSpeed();
        Scene* scene=m_actor->getScene();
        if(scene->getAlwaysActiveActors().find(m_actor)<0) scene->setAlwaysActiveActor(m_actor);
    }

    void Ray_MamaEyesAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);
    }

    void Ray_MamaEyesAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        move(_dt);
        checkDeath();
    }

    void Ray_MamaEyesAIComponent::setInitSpeed()
    {
        m_speed = getTemplate()->getSpeed();
        m_speed.m_x = Seeder::getSharedSeeder().GetFloat(-m_speed.m_x, m_speed.m_x);
        m_speed.m_y = Seeder::getSharedSeeder().GetFloat(0.f, m_speed.m_y);
        m_speed.m_z = Seeder::getSharedSeeder().GetFloat(0.f, m_speed.m_z);

        f32 rotation = getTemplate()->getRotation();
        m_rotation = Seeder::getSharedSeeder().GetFloat(-rotation, rotation);

        if(Seeder::getSharedSeeder().GetU32(0, 100) < 50)
        {
            m_actor->setFlipped(btrue);
        }

        f32 scale = Seeder::getSharedSeeder().GetFloat(
            getTemplate()->getScaleMin(),
            getTemplate()->getScaleMax());
        
        Vec2d vScale = Vec2d(scale, scale);
        m_actor->setScale(vScale);
    }

    void Ray_MamaEyesAIComponent::move(f32 _dt)
    {
        m_speed.m_y += _dt * getTemplate()->getGravity();

        Vec3d pos = m_actor->getPos();
        pos += _dt * m_speed;
        m_actor->setPos(pos);

        f32 angle = m_actor->getAngle();
        angle += _dt * m_rotation;
        m_actor->setAngle(angle);
    }

    void Ray_MamaEyesAIComponent::checkDeath()
    {
        if(m_animatedComponent && m_animatedComponent->isSubAnimFinished())
        {
            m_actor->disable();
            m_actor->requestDestruction();   
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_MamaEyesAIComponent_Template);
    BEGIN_SERIALIZATION_CHILD(Ray_MamaEyesAIComponent_Template);
        SERIALIZE_MEMBER("initSpeed", m_initSpeed);
        SERIALIZE_MEMBER("gravity", m_gravity);
        SERIALIZE_MEMBER("rotation", m_rotation);
        SERIALIZE_MEMBER("scaleMin", m_scaleMin);
        SERIALIZE_MEMBER("scaleMax", m_scaleMax);
    END_SERIALIZATION();

    Ray_MamaEyesAIComponent_Template::Ray_MamaEyesAIComponent_Template()
        : Super()
        , m_initSpeed(Vec3d(2.f, 2.f, 0.5f))
        , m_gravity(-10.f)
        , m_rotation(5.f)
        , m_scaleMin(0.5f)
        , m_scaleMax(1.2f)
    {
    }
};