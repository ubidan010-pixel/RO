
#ifndef _ITF_SOUNDBUSCONTROLLERCOMPONENT_H_
#define _ITF_SOUNDBUSCONTROLLERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_
#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H


namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    class SoundBusControllerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SoundBusControllerComponent,ActorComponent,2952737971);
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        SoundBusControllerComponent();
        ~SoundBusControllerComponent();
        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);
        virtual void    Update( f32 _deltaTime );
        template <typename T>
        void                        setInput(StringID _inputName, T _value );

        struct Bus
        {
            DECLARE_SERIALIZE();
            StringID m_busName;
            ProceduralInputData m_busFilterFrequency;
            ProceduralInputData m_busFilterOneOverQ;
            ProceduralInputData m_busVolume;

        };

    protected:
        SafeArray<Bus> m_busData;
        InputContainer              m_inputList;
    };
    template <typename T>
    ITF_INLINE void SoundBusControllerComponent::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if (input.getId() == _inputName)
            {
                input.setValue(_value);
                break;
            }
        }
    }

    class SoundBusControllerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SoundBusControllerComponent_Template,TemplateActorComponent,2216764263);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SoundBusControllerComponent);

    public:

        SoundBusControllerComponent_Template();
        ~SoundBusControllerComponent_Template();

        ITF_INLINE const SafeArray<SoundBusControllerComponent::Bus> & getBusData() const { return m_busData; }
        ITF_INLINE const InputContainer & getInputs() const { return m_inputList; }

    private:
        SafeArray<SoundBusControllerComponent::Bus> m_busData;
        InputContainer              m_inputList;
    };

}

#endif // _ITF_SOUNDBUSCONTROLLERCOMPONENT_H_

