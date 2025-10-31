#include "precompiled_engine.h"

#ifndef _ITF_PARTICLEGENERATORCOMPONENT_H_
#include "engine/actors/components/ParticleGeneratorComponent.h"
#endif //_ITF_PARTICLEGENERATORCOMPONENT_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(ParticleGeneratorComponent)

BEGIN_SERIALIZATION_CHILD(ParticleGeneratorComponent)
END_SERIALIZATION()

ParticleGeneratorComponent::ParticleGeneratorComponent()
: m_velocityNorm(1.0f)
, m_texture()
, m_increment(0.0f)
, m_isPause(bfalse)
, m_maxFrequency(0.0f)
, m_isFlipped(bfalse)
{
}

ParticleGeneratorComponent::~ParticleGeneratorComponent()
{
    clear();
}

void ParticleGeneratorComponent::clear()
{
    if ( m_actor && m_texture.isValidResourceId() )
    {
        m_actor->removeResource(m_texture);
        m_texture.invalidateResourceId();
    }
}

void ParticleGeneratorComponent::start()
{
    //m_increment = m_maxFrequency * TARGETDT / m_startTime;  // force constant dt
    pause(bfalse);
}
void ParticleGeneratorComponent::stop()
{
    //m_increment = - m_maxFrequency * TARGETDT / m_startTime;  // force constant dt
    pause(btrue);
}

void ParticleGeneratorComponent::onBecomeActive()
{
    m_particleGenerator.getParameters()->setDepth(GetActor()->getDepth());
}

void ParticleGeneratorComponent::Draw()
{
    if (m_particleGenerator.getParameters()->getFrequency() > 0.0f)
    {
        m_particleGenerator.setColorFactor(getColorFactor());
        m_particleGenerator.setAlpha(getAlpha());
        m_particleGenerator.setFog(getColorFog());
        m_particleGenerator.setRenderInTarget(m_renderInTarget);
        m_particleGenerator.render();
    }
}

void ParticleGeneratorComponent::updateAABB()
{
    //const Layer* layer = LAYER_MANAGER->findLayer(GetActor()->getLayerID());
    //m_particleGenerator.applyAmbiantColor(layer->getAmbientLight().m_color);
    if (m_particleGenerator.getTrueAABB())
    {
        GetActor()->growRelativeAABBFromAbsolute(m_particleGenerator.getParameters()->getBoundingBox());
        return;
    }

    Vec2d minAABB = m_particleGenerator.getParameters()->getBoundingBox().getMin();
    Vec2d maxAABB = m_particleGenerator.getParameters()->getBoundingBox().getMax();
    minAABB *= GetActor()->getScale(); 
    maxAABB *= GetActor()->getScale();
    AABB aabb(minAABB);
    aabb.grow(maxAABB);
    GetActor()->growRelativeAABB(aabb);
}

bbool ParticleGeneratorComponent::keepAlive()
{
    return m_particleGenerator.getActive();
}

void ParticleGeneratorComponent::addLight(const LightInfo &_light)
{
    m_particleGenerator.addlight(_light);
}

void ParticleGeneratorComponent::Update(f32 _dt)
{
    ITF_ASSERT(!m_pause);
    ITF_WARNING(m_actor, m_particleGenerator.checkGeneratorValidity(), "generator: '%s'", StringToUTF8(m_particleGenerator.getValidityMessage()).get());

    m_particleGenerator.getParameters()->setFlip(m_actor->isFlipped());

    f32 freq = f32_Inv(m_particleGenerator.getParameters()->getFrequency());

    if (!getTemplate()->getFrequencyInput().isValid())
    {
        freq += m_increment;
        m_particleGenerator.getParameters()->setFrequency(1.0f / std::min(freq,m_maxFrequency));
    }
    else
    {
        freq = getTemplate()->getFrequencyInput().getValue(freq, _dt, m_inputList);
        f32 invFreq = freq ? f32_Inv(freq) : 0.f;
        m_particleGenerator.getParameters()->setFrequency(invFreq);
        //freq = 1.f; // just to pass the if (freq > 0.0f)
    }

    if (getTemplate()->getEmitCountInput().isValid())
    {
        f32 emitCount = (f32)m_particleGenerator.getParameters()->getNumToEmit();
        emitCount = getTemplate()->getEmitCountInput().getValue(emitCount, _dt, m_inputList);
        m_particleGenerator.getParameters()->setNumToEmit((u32)emitCount);
    }

    if (freq > 0.0f)
    {
        m_particleGenerator.getParameters()->setPosition(m_actor->getPos());
        m_particleGenerator.getParameters()->setAngle(Angle( bfalse, m_actor->getAngle()));

        Vec2d direction = Vec2d::Right.Rotate(m_actor->getAngle());
        if ( m_isFlipped )
            direction *= -1.f;
        m_particleGenerator.getParameters()->setVelocity( direction.to3d() );
        //GFX_ADAPTER->drawArrow(m_actor->getPos(), m_actor->getPos() + direction.to3d());

        m_particleGenerator.update(_dt, m_isPause);
    }

    updateAABB();
}

void ParticleGeneratorComponent::setTexture(const Path & _path)
{
    if ( m_texture.isValidResourceId())
    {
        m_actor->removeResource(m_texture);
        m_texture.invalidateResourceId();
    }

    m_texture = m_actor->addResource(Resource::ResourceType_Texture, _path);
    
    m_particleGenerator.getParameters()->setTexture(m_texture);
}

void ParticleGeneratorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    Vec3d velocity;
    f32_CosSin(m_actor->getAngle(), &velocity.m_x, &velocity.m_y);
    velocity.m_z = 0.f;

    m_particleGenerator.init(&getTemplate()->getParticuleGenerator());
    m_maxFrequency      = f32_Inv(m_particleGenerator.getParameters()->getFrequency());
    m_particleGenerator.getParameters()->setVelocity(velocity);
    m_particleGenerator.setMaxParticles(m_particleGenerator.getParameters()->getMaxParticles());

    setTexture(getTemplate()->getTextureFile());

    if (getTemplate()->getBeginStart())
    {
        start();
    }
    else
    {
        stop();
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDie,1997554881),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903),this);

    m_inputList.resize(getTemplate()->getInputList().size());

    const u32 s = m_inputList.size();
    for ( u32 i = 0; i < s; i++ )
    {
        Input* input = &m_inputList[i];
        const InputDesc& inputDesc = getTemplate()->getInputList()[i];

        input->setId(inputDesc.getId());
        input->setType(inputDesc.getType());

        if ( inputDesc.getType() == InputType_F32 )
        {
            input->setValue(0.f);
        }
        else
        {
            input->setValue(static_cast<u32>(0));
        }
    }
}

void ParticleGeneratorComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if (EventTrigger * evt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if (evt->getActivated())
            start();
        else
            stop();
    }
    else if (_event->IsClassCRC(EventDie::GetClassCRCStatic()))
    {
        stop();
    }
    else if (_event->IsClassCRC(EventSequenceActorActivate::GetClassCRCStatic()))
    {
        m_particleGenerator.reInit();
    }
    else if (EventSetFloatInput * onSetInput = _event->DynamicCast<EventSetFloatInput>(ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645)))
    {
        setInput(onSetInput->getInputName(),onSetInput->getInputValue());
    }
    else if (EventSetUintInput * onSetInput = _event->DynamicCast<EventSetUintInput>(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903)))
    {
        setInput(onSetInput->getInputName(),onSetInput->getInputValue());
    }
}

#ifdef ITF_SUPPORT_EDITOR
void ParticleGeneratorComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    if (!GetActor()->isUpdateAllowed() && GetActor()->getLastUpdatedFrame() < CURRENTFRAME) // avoid double job pushing --> crash
        Update(LOGICDT);
}
#endif // ITF_SUPPORT_EDITOR


/////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(ParticleGeneratorComponent_Template)

BEGIN_SERIALIZATION(ParticleGeneratorComponent_Template)

    SERIALIZE_OBJECT("ParticleGeneratorParams",m_particleGenerator);
    SERIALIZE_MEMBER("startTime",m_startTime);
    SERIALIZE_MEMBER("stopTime",m_stopTime);
    SERIALIZE_MEMBER("texture",m_textureFile);
    SERIALIZE_MEMBER("beginStart",m_beginStart);
    SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    SERIALIZE_OBJECT("frequencyInput",m_frequencyInput);
    SERIALIZE_OBJECT("emitCountInput",m_emitCountInput);

END_SERIALIZATION()

ParticleGeneratorComponent_Template::ParticleGeneratorComponent_Template()
: m_startTime(1.0f)
, m_stopTime(1.0f)
, m_beginStart(btrue)
{
}

} // namespace ITF

