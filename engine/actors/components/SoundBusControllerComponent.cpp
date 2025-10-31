#include "precompiled_engine.h"

#ifndef _ITF_SOUNDBUSCONTROLLERCOMPONENT_H_
#include "engine/actors/components/SoundBusControllerComponent.h"
#endif //_ITF_SOUNDBUSCONTROLLERCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(SoundBusControllerComponent_Template)
    BEGIN_SERIALIZATION(SoundBusControllerComponent_Template)
    SERIALIZE_CONTAINER_OBJECT("busData",m_busData);
    SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    END_SERIALIZATION()

    SoundBusControllerComponent_Template::SoundBusControllerComponent_Template()
    {
    }

    SoundBusControllerComponent_Template::~SoundBusControllerComponent_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(SoundBusControllerComponent)
        BEGIN_SERIALIZATION_CHILD(SoundBusControllerComponent)
        SERIALIZE_CONTAINER_OBJECT("busData",m_busData);
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    END_SERIALIZATION()
    
    BEGIN_SERIALIZATION_SUBCLASS(SoundBusControllerComponent,Bus)
        SERIALIZE_MEMBER("busName",m_busName);
        SERIALIZE_OBJECT("busFilterFrequency",m_busFilterFrequency);
        SERIALIZE_OBJECT("busFilterOneOverQ",m_busFilterOneOverQ);
        SERIALIZE_OBJECT("busVolume",m_busVolume);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(SoundBusControllerComponent)
    END_VALIDATE_COMPONENT()

    SoundBusControllerComponent::SoundBusControllerComponent()
    : Super()

    {
    }

    SoundBusControllerComponent::~SoundBusControllerComponent()
    {
    }



    void SoundBusControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            Input* input = &m_inputList[i];

            if ( input->getType() == InputType_F32 )
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

    void SoundBusControllerComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (EventSetFloatInput * onSetInput = _event->DynamicCast<EventSetFloatInput>(ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645)))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
        else if (EventSetUintInput * onSetInput = _event->DynamicCast<EventSetUintInput>(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903)))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
    }

    void SoundBusControllerComponent::Update( f32 _deltaTime )
    {

        u32 size = m_busData.size();
        for (u32 i = 0; i < size; ++i)
        {
            Bus & bus = m_busData[i];
            if (bus.m_busName.isValid())
            {
                // VOLUME
                if (bus.m_busVolume.isValid())
                {
#ifdef ITF_SUPPORT_RAKI
                    SOUND_ADAPTER->setBusVolume(bus.m_busName,Volume(bus.m_busVolume.getValue(0.0f,_deltaTime,m_inputList),btrue),0.0f);
#else
                    AUDIO_ADAPTER->setBusVolume(bus.m_busName, Volume(bus.m_busVolume.getValue(0.0f, _deltaTime, m_inputList), btrue), 0.0f);

#endif
                }

                //FILTER
#ifdef ITF_SUPPORT_RAKI

                FilterType type = FilterType_None;
                f32 filterFrequency = 0.f;
                f32 filterQ= 0.f;


                SOUND_ADAPTER->getBusFilter(bus.m_busName,type, filterFrequency, filterQ);


                bbool applyFilter = bfalse;

                if (bus.m_busFilterFrequency.isValid())
                {
                    filterFrequency = bus.m_busFilterFrequency.getValue(0.0f,_deltaTime,m_inputList);
                    applyFilter = btrue;
                }

                if (bus.m_busFilterOneOverQ.isValid())
                {
                    filterQ = bus.m_busFilterOneOverQ.getValue(0.0f,_deltaTime,m_inputList);
                    applyFilter = btrue;
                }

                if (applyFilter)
                    SOUND_ADAPTER->setBusFilter(bus.m_busName,type, filterFrequency, filterQ);
#else
                LOG("TO DO Should get bus filter on bus  [%s] ", bus.m_busName.getDebugString());

#endif


            }
        }

    }

}

