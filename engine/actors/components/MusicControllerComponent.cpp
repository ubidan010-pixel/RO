#include "precompiled_engine.h"

#ifndef _ITF_MUSICCONTROLLERCOMPONENT_H_
#include "engine/actors/components/MusicControllerComponent.h"
#endif //_ITF_MUSICCONTROLLERCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(MusicControllerComponent_Template)
    BEGIN_SERIALIZATION(MusicControllerComponent_Template)
        SERIALIZE_OBJECT("musicVolume",m_musicVolume);
        SERIALIZE_MEMBER("metronomeType", m_metronomeType);
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    END_SERIALIZATION()

    MusicControllerComponent_Template::MusicControllerComponent_Template()
        : m_metronomeType(U32_INVALID)
    {
    }

    MusicControllerComponent_Template::~MusicControllerComponent_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(MusicControllerComponent)
    BEGIN_SERIALIZATION_CHILD(MusicControllerComponent)
    END_SERIALIZATION()
    
    BEGIN_VALIDATE_COMPONENT(MusicControllerComponent)
    END_VALIDATE_COMPONENT()

    MusicControllerComponent::MusicControllerComponent()
        : Super()
        , m_metronomeType(U32_INVALID)
        , m_inputUpdated(bfalse)
    {
    }

    MusicControllerComponent::~MusicControllerComponent()
    {
    }



    void MusicControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_metronomeType = getTemplate()->getMetronomeType();
        m_musicVolume   = getTemplate()->getMusicVolume();

        m_inputList.resize(getTemplate()->getInputList().size());

        for ( u32 i = 0; i < m_inputList.size(); i++ )
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

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903),this);
    }

    void MusicControllerComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        m_inputUpdated = bfalse;

        if (EventSetFloatInput * onSetInput = _event->DynamicCast<EventSetFloatInput>(ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645)))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
            m_inputUpdated = btrue;
        }
        else if (EventSetUintInput * onSetInput = _event->DynamicCast<EventSetUintInput>(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903)))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
            m_inputUpdated = btrue;
        }
    }

    void MusicControllerComponent::Update( f32 _deltaTime )
    {
        // VOLUME
        if (m_inputUpdated && m_musicVolume.isValid())
        {
            Volume volume = Volume(m_musicVolume.getValue(0.0f,_deltaTime,m_inputList),btrue);
            MUSICMANAGER->setLinearVolume(m_metronomeType,volume.ratio());

            m_inputUpdated = bfalse;
        }
    }
}

