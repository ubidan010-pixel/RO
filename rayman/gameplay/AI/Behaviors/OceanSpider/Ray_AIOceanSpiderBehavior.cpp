#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/OceanSpider/Ray_AIOceanSpiderBehavior.h"
#endif //_ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIOceanSpiderBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIOceanSpiderBehavior_Template)
    SERIALIZE_CONTAINER_OBJECT("Actions",m_actions);
    SERIALIZE_CONTAINER_OBJECT("Instructions",m_instructions);
    SERIALIZE_CONTAINER_OBJECT("InactiveInstructions",m_inactiveInstructions);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_AIOceanSpiderBehavior_Template, ActionTemplate)
        SERIALIZE_MEMBER("name", m_name);
        SERIALIZE_OBJECT_WITH_FACTORY("action", m_action, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_AIOceanSpiderBehavior_Template, InstructionTemplate)
        SERIALIZE_MEMBER("actionName", m_actionName);
        SERIALIZE_MEMBER("playCount", m_playCount);
    END_SERIALIZATION()
        
    Ray_AIOceanSpiderBehavior_Template::Ray_AIOceanSpiderBehavior_Template()
    {
        
    }

    Ray_AIOceanSpiderBehavior_Template::~Ray_AIOceanSpiderBehavior_Template()
    {
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIOceanSpiderBehavior)

    Ray_AIOceanSpiderBehavior::Ray_AIOceanSpiderBehavior()
    :m_currentInstruction(0)
    , m_active(btrue)
    , m_requestActivation(btrue)
    {
    }

    Ray_AIOceanSpiderBehavior::~Ray_AIOceanSpiderBehavior()
    {
        for (u32 i = 0; i < m_actions.size(); i++)
            m_actions[i].m_action = NULL;
    }

    void Ray_AIOceanSpiderBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_actions.resize(getTemplate()->getActions().size());
        for (u32 i = 0; i < m_actions.size(); i++)
        {
            m_actions[i].m_name = getTemplate()->getActions()[i].m_name;
            m_actions[i].m_action = static_cast<AIAction*>(createAiAction(getTemplate()->getActions()[i].m_action));
        }
        m_instructions.resize(getTemplate()->getInstructions().size());
        for (u32 i = 0; i < m_instructions.size(); i++)
        {
            m_instructions[i].m_actionName = getTemplate()->getInstructions()[i].m_actionName;
            m_instructions[i].m_playCount = 0;
        }
        m_inactiveInstructions.resize(getTemplate()->getInactiveInstructions().size());
        for (u32 i = 0; i < m_inactiveInstructions.size(); i++)
        {
            m_inactiveInstructions[i].m_actionName = getTemplate()->getInactiveInstructions()[i].m_actionName;
            m_inactiveInstructions[i].m_playCount = 0;
        }
    }

    void Ray_AIOceanSpiderBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
    }

    void Ray_AIOceanSpiderBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
    }

    void Ray_AIOceanSpiderBehavior::onActivate()
    {
        playAction(m_instructions[m_currentInstruction].m_actionName);
    }

    void Ray_AIOceanSpiderBehavior::onDeactivate()
    {
        Super::onDeactivate();
    }

    void Ray_AIOceanSpiderBehavior::onActionFinished()
    {
        Super::onActionFinished();

        bbool justChanged = bfalse;

        if (m_requestActivation != m_active)
        {
            m_active = m_requestActivation;
            justChanged = btrue;
            if (m_requestActivation)
            {
                for (u32 i = 0; i < m_instructions.size(); i++)
                {
                    m_instructions[i].m_playCount = 0;
                }
                m_currentInstruction = 0;
                playAction(m_instructions[m_currentInstruction].m_actionName);
            }
            else
            {
                for (u32 i = 0; i < m_inactiveInstructions.size(); i++)
                {
                    m_inactiveInstructions[i].m_playCount = 0;
                    m_currentInstruction = 0;
                    playAction(m_instructions[m_currentInstruction].m_actionName);
                }
            }
        }


        if ( !justChanged )
        {
            const ITF_VECTOR<Ray_AIOceanSpiderBehavior_Template::InstructionTemplate> & instructionsT = m_active?getTemplate()->getInstructions():getTemplate()->getInactiveInstructions();
            ITF_VECTOR<Instruction> & instructions = m_active?m_instructions:m_inactiveInstructions;
            Instruction & instruction = m_active?m_instructions[m_currentInstruction]:m_inactiveInstructions[m_currentInstruction];

            if (instruction.m_playCount+1 >= instructionsT[m_currentInstruction].m_playCount)
            {
                instruction.m_playCount = 0 ; //reset counter
                if (m_currentInstruction + 1  >= instructions.size())
                {
                    m_currentInstruction = 0;
                    playAction(instructions[0].m_actionName);
                }
                else
                {
                    playAction(instructions[m_currentInstruction+1].m_actionName);
                    m_currentInstruction++;
                }
            }
            else
            {
                instruction.m_playCount++;
                playAction(instructions[m_currentInstruction].m_actionName);
            }
        }
    }

    void Ray_AIOceanSpiderBehavior::playAction( StringID name )
    {
        for (u32 i = 0; i < m_actions.size(); ++i)
        {
            if (m_actions[i].m_name == name)
            {
                m_animComponent->resetCurTime();
                setAction(m_actions[i].m_action,btrue);
            }
        }
    }

    void Ray_AIOceanSpiderBehavior::activate( bbool _activate )
    {
        m_requestActivation = _activate;
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIOceanSpiderBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
    }
#endif // ITF_SUPPORT_EDITOR
}

