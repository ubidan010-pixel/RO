#include "precompiled_gameplay.h"

#ifndef _ITF_TWEENINTERPRETER_H_
#include "gameplay/Components/Common/TweenInterpreter.h"
#endif //_ITF_TWEENINTERPRETER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    static const f32 referenceBarDuration = 2.f;  // bpm 120

    //---------------------------------------------------------------------------------------------------

    BEGIN_SERIALIZATION(TweenInstructionSet_Template)
        SERIALIZE_MEMBER("name", m_name);
        SERIALIZE_CONTAINER_WITH_FACTORY("instructions", m_instructions, TweenInstructionFactory_Template::get());
        SERIALIZE_MEMBER("iterationCount", m_iterationCount);
    END_SERIALIZATION()

    TweenInstructionSet_Template::TweenInstructionSet_Template()
        : m_iterationCount(0)   // default: infinite

        , m_duration(0.f)
        , m_syncRatio(0.f)
        , m_isLooping(bfalse)
    {
    }

    TweenInstructionSet_Template::~TweenInstructionSet_Template()
    {
        clear();
    }

    void TweenInstructionSet_Template::clear()
    {
        for (u32 i = 0; i < m_instructions.size(); i++)
        {
            SF_DEL(m_instructions[i]);
        }
    }

    void TweenInstructionSet_Template::onTemplateLoaded()
    {
        TweenCoordinates coordinates;

        for (u32 i = 0; i < m_instructions.size(); i++)
        {
            TweenInstruction_Template* tplInstruction = m_instructions[i];
            tplInstruction->onTemplateLoaded();

            m_duration += tplInstruction->getDuration();

            tplInstruction->executeEnd(coordinates);
        }

        m_syncRatio = referenceBarDuration / m_duration;
        m_isLooping = coordinates.m_pos.IsEqual(Vec3d::Zero, MTH_EPSILON);
    }



    //---------------------------------------------------------------------------------------------------

    BEGIN_SERIALIZATION(TweenInterpreter_Template)
        SERIALIZE_FUNCTION(clear, ESerialize_PropertyEdit_Load);
        SERIALIZE_CONTAINER_OBJECT("instructionSets", m_instructionSets);
    END_SERIALIZATION()

    void TweenInterpreter_Template::clear()
    {
        m_instructionSets.clear();
    }

    void TweenInterpreter_Template::onTemplateLoaded()
    {
        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            TweenInstructionSet_Template& tplSet = m_instructionSets[i];
            tplSet.onTemplateLoaded();
        }
    }

    u32 TweenInterpreter_Template::getSetIndex( const StringID& _setName ) const
    {
        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            const TweenInstructionSet_Template& tplSet = m_instructionSets[i];
            if (tplSet.getName() == _setName)
            {
                return i;
            }
        }

        return U32_INVALID;
    }



    //---------------------------------------------------------------------------------------------------

    void TweenInterpreter::update( const f32 _dt )
    {
        f32 t;
        if (m_sync)
        {
            t = METRONOME_MANAGER->getBarPercentage(m_syncOffset, m_instructionSet->getSyncRatio(), bfalse) * m_instructionSet->getDuration();
        }
        else
        {
            m_timer += _dt;
            t = f32_Modulo(m_timer, m_instructionSet->getDuration());
        }

        TweenCoordinates coordinates;

        for (u32 i = 0; i < m_instructionSet->getInstructions().size(); i++)
        {
            const TweenInstruction_Template* tplInstruction = m_instructionSet->getInstructions()[i];

            if (t > tplInstruction->getDuration())
            {
                tplInstruction->executeEnd(coordinates);
                t -= tplInstruction->getDuration();
            }
            else
            {
                tplInstruction->execute(t / tplInstruction->getDuration(), coordinates);
                break;
            }
        }

        m_currentCoordinates = coordinates;
    }


};