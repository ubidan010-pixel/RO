#include "precompiled_gameplay.h"

#ifndef _ITF_TRIGGERONBUTTONPRESSEDCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerOnButtonPressedComponent.h"
#endif //_ITF_TRIGGERONBUTTONPRESSEDCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(TriggerOnButtonPressedComponent)

    BEGIN_SERIALIZATION_CHILD(TriggerOnButtonPressedComponent)
    END_SERIALIZATION()

    TriggerOnButtonPressedComponent::TriggerOnButtonPressedComponent()
    {
    }

    TriggerOnButtonPressedComponent::~TriggerOnButtonPressedComponent()
    {
        clear();
    }



    void TriggerOnButtonPressedComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        clear();

        const ITF_VECTOR<u32> & inputEvents = getTemplate()->getInputs();
        for (u32 i = 0; i < inputEvents.size(); ++i)
        {
            m_inputs.push_back(inputEvents[i]);
        }
    }


    void TriggerOnButtonPressedComponent::checkSendEvent( ActorRefList& actorsInsideThisFrame )
    {
        // Look for button modifications
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            if ( GAMEMANAGER->getPlayer(i)->getActive()
                && GAMEMANAGER->getPlayer(i)->getActor()
                && actorsInsideThisFrame.find(GAMEMANAGER->getPlayer(i)->getActor()->getRef()) != -1 )
            {
                // Buttons
                InputAdapter::PressStatus buts[JOY_MAX_BUT];
                INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,i, buts, JOY_MAX_BUT);

                //updateWiiInput(i, buts, pos);

                u32 size        = m_inputs.size();
                u32 buttonCount = 0;
                for ( u32 i = 0 ; i < size ; ++i )
                {
                    if ( buts[m_inputs[i]] == InputAdapter::JustPressed
                        || buts[m_inputs[i]] == InputAdapter::Pressed )
                        buttonCount++;
                }

                // all need buttons are pressed, we can go to next step
                if ( buttonCount == size )
                {
                    if ( canTrigger() )
                    {
                        m_actorsInsideLastFrame.clear();
                        Super::checkSendEvent(actorsInsideThisFrame);
                    }
                }
            }
        }
    }

    void TriggerOnButtonPressedComponent::clear()
    {
        m_inputs.clear();
    }



    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(TriggerOnButtonPressedComponent_Template)
    BEGIN_SERIALIZATION_CHILD(TriggerOnButtonPressedComponent_Template)
    SERIALIZE_CONTAINER("inputs",m_inputs);
    END_SERIALIZATION()

    TriggerOnButtonPressedComponent_Template::TriggerOnButtonPressedComponent_Template()
    {
    }

    TriggerOnButtonPressedComponent_Template::~TriggerOnButtonPressedComponent_Template()
    {
    }

} // namespace ITF
