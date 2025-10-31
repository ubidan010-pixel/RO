
#ifndef _ITF_MUSICCONTROLLERCOMPONENT_H_
#define _ITF_MUSICCONTROLLERCOMPONENT_H_

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
    class MusicControllerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(MusicControllerComponent,ActorComponent,2032749090);
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        MusicControllerComponent();
        ~MusicControllerComponent();
        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);
        virtual void    Update( f32 _deltaTime );
        template <typename T>
        void                        setInput(StringID _inputName, T _value );

    protected:
        ProceduralInputData         m_musicVolume;
        u32                         m_metronomeType;
        InputContainer              m_inputList;
        bbool                       m_inputUpdated;

    private:
        ITF_INLINE const class MusicControllerComponent_Template* getTemplate() const;
    };
    template <typename T>
    ITF_INLINE void MusicControllerComponent::setInput(StringID _inputName, T _value )
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

    class MusicControllerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(MusicControllerComponent_Template,TemplateActorComponent,4226307952);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(MusicControllerComponent);

    public:

        MusicControllerComponent_Template();
        ~MusicControllerComponent_Template();

        ITF_INLINE const ProceduralInputData & getMusicVolume() const { return m_musicVolume; }
        ITF_INLINE const u32 getMetronomeType() const { return m_metronomeType; }
        ITF_INLINE const SafeArray<InputDesc> & getInputList() const { return m_inputList; }

    private:
        ProceduralInputData         m_musicVolume;
        u32                         m_metronomeType;
        SafeArray <InputDesc>       m_inputList;
    };

    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const MusicControllerComponent_Template* MusicControllerComponent::getTemplate() const
    {
        return static_cast<const MusicControllerComponent_Template*>(m_template);
    }
}

#endif // _ITF_MUSICCONTROLLERCOMPONENT_H_

