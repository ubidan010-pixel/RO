#include "precompiled_gameplay.h"

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_OBJECTGROUP_H_
#include "engine/scene/ObjectGroup.h"
#endif //_ITF_OBJECTGROUP_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{
    //typedef TweenComponent::InstructionSet InstructionSet;
    //typedef ITF_VECTOR<InstructionSet> InstructionSetList;

    TweenComponent::InstructionSet* TweenComponent::findSet( InstructionSetList& _sets, const StringID& _name) const
    {
        if (_name.isValid())
        {
            for (u32 i = 0; i < _sets.size(); i++)
            {
                InstructionSet& set = _sets[i];

                if (set.getName() == _name)
                {
                    return &set;
                }
            }
        }

        return NULL;
    }

    const InstructionSet_Template* findSetTemplate( const InstructionSetList_Template& _sets, const StringID& _name)
    {
        if (_name.isValid())
        {
            for (u32 i = 0; i < _sets.size(); i++)
            {
                const InstructionSet_Template& set = _sets[i];

                if (set.getName() == _name)
                {
                    return &set;
                }
            }
        }

        return NULL;
    }

    TweenInstruction* TweenComponent::findInstruction( InstructionList& _instructions, const StringID& _name ) const
    {
        if (_name.isValid())
        {
            for (u32 i = 0; i < _instructions.size(); i++)
            {
                TweenInstruction* instruction = _instructions[i];

                if (instruction->getName() == _name)
                {
                    return instruction;
                }
            }
        }

        return NULL;
    }

    const TweenInstruction_Template* findInstructionTemplate( const InstructionList_Template& _instructions, const StringID& _name )
    {
        if (_name.isValid())
        {
            for (u32 i = 0; i < _instructions.size(); i++)
            {
                const TweenInstruction_Template* instruction = _instructions[i];

                if (instruction->getName() == _name)
                {
                    return instruction;
                }
            }
        }

        return NULL;
    }

    TweenInstruction* TweenComponent::findAndRemoveInstruction( InstructionSet* _set, const StringID& _name ) const
    {
        if (_set && _name.isValid())
        {
            for (u32 i = 0; i < _set->m_instructions.size(); i++)
            {
                TweenInstruction* instruction = _set->m_instructions[i];

                if (instruction->getName() == _name)
                {
                    _set->m_instructions.eraseNoOrder(i);
                    return instruction;
                }
            }
        }

        return NULL;
    }


    static const f32 referenceBarDuration = 2.f;  // bpm 120
    #define SYNCOFFSET_INVALID -999.f

    IMPLEMENT_OBJECT_RTTI(TweenComponent)

    #define ELSE } else {

    BEGIN_SERIALIZATION_CHILD(TweenComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("syncOffset", m_syncOffset_instance);
            BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit);
                if (getTemplate() != NULL)
                {
                    SERIALIZE_ENUM_BEGIN("startSet", m_startSet);
                    serializer->SerializeEnumVar(StringID::InvalidId, "- None -");
                    const InstructionSetList_Template& tplSets = getTemplate()->getInstructionSets();
                    for (u32 i = 0; i < tplSets.size(); i++)
                    {
                        const InstructionSet_Template& tplSet = tplSets[i];
                        if (tplSet.getName().isValid())
                        {
                            serializer->SerializeEnumVar(tplSet.getName(), tplSet.getName().getDebugString());
                        }
                    }
                    SERIALIZE_ENUM_END();
                }
                else
                {
                    SERIALIZE_MEMBER("startSet", m_startSet);
                }
            ELSE
                SERIALIZE_MEMBER("startSet", m_startSet);
            END_CONDITION_BLOCK()
            SERIALIZE_CONTAINER_OBJECT("instructionSets", m_instructionSets);
            SERIALIZE_FUNCTION(reinit, ESerialize_PropertyEdit_Load);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(TweenComponent, InstructionSet)
        SERIALIZE_MEMBER("name", m_name);
        SERIALIZE_CONTAINER_WITH_FACTORY("instructions", m_instructions, TweenInstructionFactory::get());
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(TweenComponent)
    #ifndef ITF_FINAL

        //VALIDATE_COMPONENT_PARAM("instructionSets", m_instructionSets.size() > 0, "you have to specify at least one instruction!");
        VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "syncOffset", 0.f <= m_syncOffset_instance && m_syncOffset_instance <= 1.f, "must be in the [0,1] range");
        VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "syncOffset", 0.f <= getTemplate()->getSyncOffset() && getTemplate()->getSyncOffset() <= 1.f, "must be in the [0,1] range");
        VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "startSet", !getTemplate()->getStartSet().isValid() || getTemplate()->getStartSetIndex() != U32_INVALID, "'%s' doesn't exist", getTemplate()->getStartSet().getDebugString());

        const InstructionSetList_Template& tplSets = getTemplate()->getInstructionSets();

        // validate template sets
        f32 dt = LOGICDT;
        for (u32 i = 0; i < tplSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = tplSets[i];
            VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "instructionSets", tplSet.getDuration() > dt, "instructionSets[%d] must have a duration!", i);
            VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "nextSet", !tplSet.getNextSet().isValid() || tplSet.getNextSetIndex() != U32_INVALID, "'%s' doesn't exist", getTemplate()->getStartSet().getDebugString());
            VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "nextSet", tplSet.getNextSetIndex() == U32_INVALID || tplSet.getIterationCount() != 0, "you can't loop if you have a nextSet");
        }

        // validate instructions
        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            const InstructionList& instructions = m_instructionSets[i].m_instructions;
            ITF_ASSERT(tplSets[i].getInstructions().size() == instructions.size());

            for (u32 j = 0; j < instructions.size(); j++)
            {
                instructions[j]->validate(m_actor, _isComponentValidated);
            }
        }

        /* validate names:
            - sets names must be unique
            - instruction names must be unique within a set
            - named instructions must be in a named set
        */
        for (u32 i = 0; i < tplSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = tplSets[i];
            const InstructionList_Template& tplInstructions = tplSet.getInstructions();

            if (tplSet.getName().isValid())
            {
                // make sure this set name is unique
                const InstructionSet_Template* otherSet = findSetTemplate(tplSets, tplSet.getName());
                VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "instructionSets", !otherSet || otherSet == &tplSet, "duplicate set name: %s", tplSet.getName().getDebugString());

                for (u32 j = 0; j < tplInstructions.size(); j++)
                {
                    // make sure this instruction name is unique
                    const TweenInstruction_Template* tplInstruction = tplInstructions[j];
                    if (tplInstruction->getName().isValid())
                    {
                        const TweenInstruction_Template* otherInstruction = findInstructionTemplate(tplInstructions, tplInstruction->getName());
                        VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "instructions", !otherInstruction || otherInstruction == tplInstruction, "duplicate instruction name: %s", tplInstruction->getName().getDebugString());
                    }
                }
            }
            else
            {
                // set is not named -> make sure instructions are not named
                for (u32 j = 0; j < tplInstructions.size(); j++)
                {
                    const TweenInstruction_Template* tplInstruction = tplInstructions[j];
                    VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "instructions", !tplInstruction->getName().isValid(), "named instruction '%s' must be contained in a named set", tplInstruction->getName().getDebugString());
                }
            }
        }

    #endif
    END_VALIDATE_COMPONENT()


    TweenComponent::TweenComponent()
        : Super()
        , m_linkComponent(NULL)
        , m_syncOffset_instance(SYNCOFFSET_INVALID)
        , m_activated(btrue)
        , m_applyPosition(btrue)
        , m_applyRotation(btrue)
        , m_applyScale(btrue)
        , m_justStarted(bfalse)
        , m_currentSetIndex(U32_INVALID)
        , m_nextSetIndex(U32_INVALID)
        , m_currentSetIteration(0)
        , m_triggableIndex(U32_INVALID)
        , m_instructionIndex(0)
        , m_instructionTime(0.f)
        , m_instructionSetSyncOffset(0.f)
        , m_instructionSetTime(0.f)
#ifdef ITF_SUPPORT_DEBUGFEATURE
        , m_debugLinearSpeed2d(0.f)
        , m_debugLinearSpeed3d(0.f)
        , m_debugAngularSpeed(0.f)
#endif // ITF_SUPPORT_DEBUGFEATURE
    {
    }

    TweenComponent::~TweenComponent()
    {
        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            m_instructionSets[i].clear();
        }
    }

    TweenComponent::InstructionSet::InstructionSet()
    {
    }

    TweenComponent::InstructionSet::~InstructionSet()
    {
    }

    void TweenComponent::InstructionSet::clear()
    {
        for ( u32 i = 0; i < m_instructions.size(); i++ )
        {
            SF_DEL(m_instructions[i]);
        }
    }

    // called from editor
    void TweenComponent::reinit()
    {
        m_currentSetIndex = U32_INVALID;
        m_nextSetIndex = U32_INVALID;
        m_currentSetIteration = 0;

        m_justStarted = btrue;
        m_instructionIndex = 0;
        m_instructionTime = 0.f;
        m_instructionSetTime = 0.f;
        m_instructionSetSyncOffset = 0.f;

        m_baseCoordinates = TweenCoordinates();
        m_setBaseCoordinates = TweenCoordinates();
        m_currentCoordinates = TweenCoordinates();

        playStartSet();
    }


    void TweenComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        if (!m_actor->isUpdateTypeFromTemplateOverridden())
        {
            m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
        }

        m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);


        m_linkComponent = m_actor->GetComponent<LinkComponent>();    // needed for TweenInstructionEvent to send delayed events..


        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),this);

        if ( getTemplate()->getTriggableSets().size() > 0 )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        }

        registerStartStopEvents();


        initInputs();

        initInstructionSets();


        if (!getTemplate()->getApplyPosition())
        {
            m_applyPosition = bfalse;
        }
        if (!getTemplate()->getApplyRotation())
        {
            m_applyRotation = bfalse;
        }
        if (!getTemplate()->getApplyScale())
        {
            m_applyScale = bfalse;
        }


        if (m_syncOffset_instance == SYNCOFFSET_INVALID)
        {
            m_syncOffset_instance = getTemplate()->getSyncOffset();
        }


        m_targetPos = m_actor->getPos();


        if (getTemplate()->getStartMode() != TweenComponent_Template::StartMode_Off)
        {
            playStartSet();
        }
    }

    void TweenComponent::onBecomeActive()
    {
        m_actor->setUpdateType(m_actor->getUpdateType(), btrue); // avoid popping issue, especially with tweened dynamic friezes inside groups
    }

    void TweenComponent::onSceneActive()
    {
        Super::onSceneActive();

        initPosition();
    }

    void TweenComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        initPosition();
    }

    void TweenComponent::registerStartStopEvents()
    {
        const InstructionSetList_Template& tplSets = getTemplate()->getInstructionSets();

        for (u32 i = 0; i < tplSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = tplSets[i];

            const Event* startEvent = tplSet.getStartEvent();
            if (startEvent)
            {
                m_actor->registerEvent(startEvent->GetObjectClassCRC(), this);
            }

            const Event* stopEvent = tplSet.getStopEvent();
            if (stopEvent)
            {
                m_actor->registerEvent(stopEvent->GetObjectClassCRC(), this);
            }
        }
    }

    void TweenComponent::initInstructionSets()
    {
        const InstructionSetList_Template& tplSets = getTemplate()->getInstructionSets();


        // copy & clear
        InstructionSetList serializedSets = m_instructionSets;
        m_instructionSets.clear();


        // rebuild all instance InstructionSets by matching existing
        // serialized instructions (using name), or create new ones
        m_instructionSets.resize(tplSets.size());
        for (u32 i = 0; i < tplSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = tplSets[i];
            InstructionSet& instructionSet = m_instructionSets[i];
            instructionSet.m_name = tplSet.getName();

            const InstructionList_Template& tplInstructions = tplSet.getInstructions();
            InstructionList& instructions = instructionSet.m_instructions;

            InstructionSet* serializedSet = findSet(serializedSets, tplSet.getName());

            for (u32 j = 0; j < tplInstructions.size(); j++)
            {
                const TweenInstruction_Template* tplInstruction = tplInstructions[j];

                TweenInstruction* instruction = findAndRemoveInstruction(serializedSet, tplInstruction->getName());
                if (instruction == NULL)
                {
                    instruction = tplInstruction->createTweenInstruction();
                }

                instructions.push_back(instruction);
                instruction->init(this, tplInstruction);
            }
        }


        // delete any serialized instruction that hasn't been used
        for (u32 i = 0; i < serializedSets.size(); i++)
        {
            serializedSets[i].clear();
        }
    }


    u32 TweenComponent::getStartSetIndex() const
    {
        // order of precedence:
        //  - instance startSet
        //  - template startSet
        //  - first set with no start event

        if ( m_startSet.isValid() )
        {
            u32 startSetIndex = getTemplate()->getSetIndex(m_startSet);
            ITF_WARNING_CATEGORY(LD, m_actor, startSetIndex != U32_INVALID, "set '%s' doesn't exist", m_startSet.getDebugString());
            if (startSetIndex != U32_INVALID)
            {
                return startSetIndex;
            }
        }

        if ( getTemplate()->getStartSetIndex() != U32_INVALID )
        {
            return getTemplate()->getStartSetIndex();
        }

        // use the first set that has no start event (we allow triggable sets because they're meant to be a sequence anyway)
        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = getTemplate()->getInstructionSets()[i];

            if ( !tplSet.getStartEvent() )
            {
                return i;
            }
        }

        return U32_INVALID;
    }

    void TweenComponent::playStartSet()
    {
        m_currentSetIndex = getStartSetIndex();

        if (m_currentSetIndex != U32_INVALID)
        {
            m_justStarted = btrue;

            // if the set we just started was triggable, offset the cursor
            m_triggableIndex = getTriggableIndex(m_currentSetIndex);
        }
    }

    void TweenComponent::playSet( const StringID& _setName )
    {
        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = getTemplate()->getInstructionSets()[i];
            if (tplSet.getName() == _setName)
            {
                queueSet(i);
                break;
            }
        }
    }

    void TweenComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventActivate* eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)) )
        {
            processActivation(eventActivate);
        }
        else if ( EventSetFloatInput* onSetInput = _event->DynamicCast<EventSetFloatInput>(ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645)) )
        {
            setInput(onSetInput->getInputName(), onSetInput->getInputValue());
        }
        else if ( EventSetUintInput* onSetInput = _event->DynamicCast<EventSetUintInput>(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903)) )
        {
            setInput(onSetInput->getInputName(), onSetInput->getInputValue());
        }

        if ( getTemplate()->getTriggableSets().size() > 0 )
        {
            if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
            {
                if (trigger->getActivated())
                {
                    playNextTriggableSet();
                }
            }
        }

        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = getTemplate()->getInstructionSets()[i];

            const Event* startEvent = tplSet.getStartEvent();
            if (startEvent && AIUtils::matchEvent(startEvent, _event))
            {
                queueSet(i);
                break;
            }

            const Event* stopEvent = tplSet.getStopEvent();
            if (stopEvent && AIUtils::matchEvent(stopEvent, _event))
            {
                // TODO: should we finish the current set first?
                m_currentSetIndex = U32_INVALID;
            }
        }
    }

    void TweenComponent::processActivation( EventActivate* _eventActivate )
    {
        if (getTemplate()->getStartMode() != TweenComponent_Template::StartMode_Auto)
        {
            return;
        }

        if (static_cast<bbool>(m_activated) == _eventActivate->getActivated())
        {
            return;
        }

        m_activated = _eventActivate->getActivated();
        if (m_activated)
        {
            computeSyncOffset();
        }
        // else: teleport to init pos for editor?
    }

    void TweenComponent::playNextTriggableSet()
    {
        if (m_currentSetIndex != U32_INVALID)
        {
            if (m_justStarted)
            {
                return;
            }

            const InstructionSet_Template& tplSet = getTemplate()->getInstructionSets()[m_currentSetIndex];
            if (!tplSet.getInterruptible())
            {
                return;
            }
        }

        const TweenComponent_Template::TriggableSetList& tplTriggableSets = getTemplate()->getTriggableSets();
        m_triggableIndex = (m_triggableIndex + 1) % tplTriggableSets.size();
        queueSet(tplTriggableSets[m_triggableIndex]);
    }

    u32 TweenComponent::getTriggableIndex( const u32 _setIndex ) const
    {
        const TweenComponent_Template::TriggableSetList& tplTriggableSets = getTemplate()->getTriggableSets();
        for (u32 i = 0; i < tplTriggableSets.size(); i++)
        {
            if (tplTriggableSets[i] == _setIndex)
            {
                return i;
            }
        }

        return U32_INVALID;
    }

    void TweenComponent::queueSet( u32 _setIndex )
    {
        if (m_currentSetIndex == U32_INVALID)
        {
            // no current set, start directly

            m_currentSetIndex = _setIndex;
            m_justStarted = btrue;

            computeSyncOffset();
        }
        else if (_setIndex != m_currentSetIndex)
        {
            // starting a different set

            const InstructionSet_Template& curSet = getTemplate()->getInstructionSets()[m_currentSetIndex];
            if (curSet.getInterruptible())
            {
                // interrupt current set with new one

                m_currentSetIndex = _setIndex;
                m_nextSetIndex = U32_INVALID;
                m_currentSetIteration = 0;

                m_justStarted = btrue;
                m_instructionIndex = 0;
                m_instructionSetTime = 0.f;
                m_instructionTime = 0.f;

                m_baseCoordinates = m_currentCoordinates;
                m_setBaseCoordinates = m_baseCoordinates;

                computeSyncOffset();

#ifdef ITF_SUPPORT_DEBUGFEATURE
                resetDebugSpeed();
#endif // ITF_SUPPORT_DEBUGFEATURE
            }
            else
            {
                // queue next set

                m_nextSetIndex = _setIndex;
            }
        }
        // TODO: maybe we'll need to restart the set (reset the counter) in some situations...
        //else
        //{
        //    m_currentSetIteration = 0;
        //}
    }

    void TweenComponent::computeSyncOffset( const bbool _accountForCurrentSetTime /*= bfalse*/ )
    {
        if ( getTemplate()->getSyncWithMetronome() &&
             m_currentSetIndex < m_instructionSets.size() )
        {
            const InstructionSet_Template& tplSet = getTemplate()->getInstructionSets()[m_currentSetIndex];
            if (_accountForCurrentSetTime)  // TODO: should this be done always?
            {
                const f32 curOffset = m_instructionSetTime / tplSet.getDuration() - m_syncOffset_instance;
                m_instructionSetSyncOffset = 1 - METRONOME_MANAGER->getBarPercentage(1 - curOffset, tplSet.getSyncRatio(), bfalse, getTemplate()->getMetronome());

                // querying the metronome just after should give us back the same time
                ITF_ASSERT(fabs(METRONOME_MANAGER->getBarPercentage(m_syncOffset_instance + m_instructionSetSyncOffset, tplSet.getSyncRatio(), bfalse, getTemplate()->getMetronome()) * tplSet.getDuration() - m_instructionSetTime) < MTH_EPSILON);
            }
            else
            {
                m_instructionSetSyncOffset = 1 - METRONOME_MANAGER->getBarPercentage(0, tplSet.getSyncRatio(), bfalse, getTemplate()->getMetronome());
            }
        }
        else
        {
            m_instructionSetSyncOffset = 0.f;
        }
    }

    void TweenComponent::resumeTweening()
    {
        if (isTweeningSuspended())
        {
            unpause();
            computeSyncOffset(btrue);
        }
    }

    void TweenComponent::initPosition()
    {
        if (!getTemplate()->getSyncWithMetronome())
        {
            return;
        }

        if (m_currentSetIndex == U32_INVALID)
        {
            return;
        }

        ITF_ASSERT(m_instructionIndex == 0);
        m_instructionIndex = 0;


        const InstructionSet_Template* tplSet = &getTemplate()->getInstructionSets()[m_currentSetIndex];
        InstructionSet* instructionSet = &m_instructionSets[m_currentSetIndex];

        if ( m_instructionIndex >= instructionSet->m_instructions.size() )
        {
            ITF_ASSERT_MSG(0, "invalid instruction index!");
            return;
        }



        const f32 syncOffset = m_syncOffset_instance;
        f32 time = tplSet->getDuration() * syncOffset;

        m_instructionSetTime = time;

        // subtract previous instructions' durations
        for (u32 i = 0; i < m_instructionIndex; i++)
        {
            time -= instructionSet->m_instructions[i]->getDuration();
        }
        m_instructionTime = time;

        //ITF_ASSERT(m_justStarted || m_instructionSetTime > 0.f);
        ITF_ASSERT_MSG(m_instructionTime >= 0.f, "if this happens during a normal run, PLEASE NOTIFY BEN!!!\nif this happens after a debugger break, this is expected (though ugly)");
        if (m_instructionTime < 0.f)
        {
            // HACK: only happens after a debugger break, this is expected (though ugly)
            m_instructionTime = 0.f;
        }



        TweenInstruction* instruction = instructionSet->m_instructions[m_instructionIndex];

        while (m_instructionTime >= instruction->getDuration())    // we validated that InstructionSets total duration is > _dt to avoid infinite loop
        {
            // instruction finished, go to next one

            //instruction->onBecomeInactive();
            m_instructionTime -= instruction->getDuration();
            instruction->executeEnd(m_baseCoordinates);

            m_instructionIndex++;

            if (m_instructionIndex == instructionSet->m_instructions.size())
            {
                break;
            }

            instruction = instructionSet->m_instructions[m_instructionIndex];
            //instruction->onBecomeActive(m_baseCoordinates);
        }



        // now we have the current instruction / time, compute the target position

        const f32 t = m_instructionTime / instruction->getDuration();
        TweenCoordinates coordinates = m_baseCoordinates;
        instruction->execute(t, coordinates);
        applyCoordinates(coordinates, tplSet->getActorAngleOffset(), 0);



        m_actor->onForceMove();
        AIUtils::ActorBindIterator iterator(m_actor);
        while ( Actor* child = iterator.getNextChild() )
        {
            child->onForceMove();
        }



        // clear all we've used
        m_instructionIndex = 0;
        m_instructionTime = 0;
        m_instructionSetTime = 0;
        m_currentCoordinates = m_baseCoordinates = m_setBaseCoordinates = TweenCoordinates();
    }

    void TweenComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

#ifdef ITF_SUPPORT_EDITOR
        if ( s_debugLock )
        {
            return;
        }
#endif // ITF_SUPPORT_EDITOR

        if ( !m_activated )
        {
            return;
        }

        if ( m_currentSetIndex >= m_instructionSets.size() )
        {
            return;
        }

        const InstructionSet_Template* tplSet = &getTemplate()->getInstructionSets()[m_currentSetIndex];
        InstructionSet* instructionSet = &m_instructionSets[m_currentSetIndex];

        if ( m_instructionIndex >= instructionSet->m_instructions.size() )
        {
            ITF_ASSERT_MSG(0, "invalid instruction index!");
            return;
        }


#ifdef DEBUG_TWEENING
        DebugCoordinates prevCoord;
        prevCoord.m_currentSetIndex = m_currentSetIndex;
        prevCoord.m_currentSetIteration = m_currentSetIteration;
        prevCoord.m_nextSetIndex = m_nextSetIndex;
        prevCoord.m_instructionIndex = m_instructionIndex;
        prevCoord.m_instructionTime = m_instructionTime;
        prevCoord.m_instructionSetTime = m_instructionSetTime;
        prevCoord.m_instructionSetSyncOffset = m_instructionSetSyncOffset;

        if (m_prevCoordinates.size() == 100)
        {
            m_prevCoordinates.eraseKeepOrder(0);
        }
        m_prevCoordinates.push_back(prevCoord);


        if (false)
        {
            // 'set next statement' here to back-debug
            m_currentSetIndex = prevCoord.m_currentSetIndex;
            m_currentSetIteration = prevCoord.m_currentSetIteration;
            m_nextSetIndex = prevCoord.m_nextSetIndex;
            m_instructionIndex = prevCoord.m_instructionIndex;
            m_instructionTime = prevCoord.m_instructionTime;
            m_instructionSetTime = prevCoord.m_instructionSetTime;
            m_instructionSetSyncOffset = prevCoord.m_instructionSetSyncOffset;

            tplSet = &getTemplate()->getInstructionSets()[m_currentSetIndex];
            instructionSet = &m_instructionSets[m_currentSetIndex];
        }
#endif // DEBUG_TWEENING


        if (tplSet->getProceduralCursor().isValid())
        {
            const f32 cursor = tplSet->getProceduralCursor().getValue(0, 0, m_inputList);  // _currentValue & _dt are only used for additive cursors; we won't use those.
            const f32 syncOffset = m_syncOffset_instance + m_instructionSetSyncOffset;
            f32 time = (cursor + syncOffset) * tplSet->getDuration();
            jumpToTime(instructionSet->m_instructions, time);
        }
        else if (getTemplate()->getSyncWithMetronome())
        {
            const f32 syncOffset = m_syncOffset_instance + m_instructionSetSyncOffset;  // no need to fmodf here, already done in METRONOME_MANAGER->getBarPercentage
            f32 time = METRONOME_MANAGER->getBarPercentage(syncOffset, tplSet->getSyncRatio(), bfalse, getTemplate()->getMetronome()) * tplSet->getDuration();
#ifdef DEBUG_TWEENING
            if (false)
            {
                // 'set next statement' here to back-debug AFTER the metronome has given us the time (which is non-deterministic during a break)
                m_currentSetIndex = prevCoord.m_currentSetIndex;
                m_currentSetIteration = prevCoord.m_currentSetIteration;
                m_nextSetIndex = prevCoord.m_nextSetIndex;
                m_instructionIndex = prevCoord.m_instructionIndex;
                m_instructionTime = prevCoord.m_instructionTime;
                m_instructionSetTime = prevCoord.m_instructionSetTime;
                m_instructionSetSyncOffset = prevCoord.m_instructionSetSyncOffset;

                tplSet = &getTemplate()->getInstructionSets()[m_currentSetIndex];
                instructionSet = &m_instructionSets[m_currentSetIndex];
            }
#endif // DEBUG_TWEENING
            jumpToTime(instructionSet->m_instructions, time);
        }
        else
        {
            m_instructionSetTime += _dt;
            m_instructionTime += _dt;
        }

        //ITF_ASSERT(m_justStarted || m_instructionSetTime > 0.f);
        ITF_ASSERT_MSG(m_instructionTime >= 0.f, "if this happens during a normal run, PLEASE NOTIFY BEN!!!\nif this happens after a debugger break, this is expected (though ugly)");
        if (m_instructionTime < 0.f)
        {
            // HACK: only happens after a debugger break, this is expected (though ugly)
            m_instructionTime = 0.f;
        }



        TweenInstruction* instruction = instructionSet->m_instructions[m_instructionIndex];

        if (m_justStarted)
        {
            m_justStarted = bfalse;
            instruction->onBecomeActive(m_baseCoordinates);
        }

        // select next instruction / instructionSet if necessary,
        // send onBecomeActive/Inactive callbacks

        while (m_instructionTime >= instruction->getDuration())    // we validated that InstructionSets total duration is > _dt to avoid infinite loop
        {
            // instruction finished, go to next one

            instruction->onBecomeInactive();
            m_instructionTime -= instruction->getDuration();
            instruction->executeEnd(m_baseCoordinates);

            m_instructionIndex++;

            if (m_instructionIndex >= instructionSet->m_instructions.size())
            {
                // we just finished the current set
                m_instructionIndex = 0;

                if (m_nextSetIndex == U32_INVALID)
                {
                    // play another iteration...
                    m_currentSetIteration++;

                    u32 iterationCount = tplSet->getIterationCount();
                    if (iterationCount != 0 && 
                        m_currentSetIteration >= iterationCount)
                    {
                        // all iterations played: set finished

                        if (tplSet->getNextSetIndex() != U32_INVALID)
                        {
                            // play next set
                            m_nextSetIndex = tplSet->getNextSetIndex();
                        }
                        else
                        {
                            // ... or stop
                            applyCoordinates(m_baseCoordinates, tplSet->getActorAngleOffset(), _dt);
                            onSetFinished(*instructionSet);

                            m_currentSetIndex = U32_INVALID;
                            m_currentSetIteration = 0;
                            m_instructionTime = 0.f;
                            m_instructionSetTime = 0.f;
                            m_instructionSetSyncOffset = 0.f;
#ifdef ITF_SUPPORT_DEBUGFEATURE
                            resetDebugSpeed();
#endif // ITF_SUPPORT_DEBUGFEATURE

                            return;
                        }
                    }
                }

                if (m_nextSetIndex != U32_INVALID)
                {
                    // play the next queued set
                    onSetFinished(*instructionSet);
                    m_currentSetIndex = m_nextSetIndex;
                    m_nextSetIndex = U32_INVALID;
                    m_currentSetIteration = 0;
                    m_instructionSetTime = m_instructionTime;
                    m_setBaseCoordinates = m_baseCoordinates;
                    computeSyncOffset();

                    tplSet = &getTemplate()->getInstructionSets()[m_currentSetIndex];
                    instructionSet = &m_instructionSets[m_currentSetIndex];
                }
            }


            instruction = instructionSet->m_instructions[m_instructionIndex];
            instruction->onBecomeActive(m_baseCoordinates);
        }



        // now we have the current instruction / time, compute the target position

        const f32 t = m_instructionTime / instruction->getDuration();
        TweenCoordinates coordinates = m_baseCoordinates;
        instruction->execute(t, coordinates);
        applyCoordinates(coordinates, tplSet->getActorAngleOffset(), _dt);
    }

    void TweenComponent::jumpToTime( const InstructionList& _instructions, f32 _time )
    {
        if (_time < m_instructionSetTime)
        {
            // we just looped

            m_instructionSetTime = _time;

            // add current & next instructions' durations
            for (u32 i = m_instructionIndex; i < _instructions.size(); i++)
            {
                _time += _instructions[i]->getDuration();
            }
            m_instructionTime = _time;
        }
        else
        {
            m_instructionSetTime = _time;

            // subtract previous instructions' durations
            for (u32 i = 0; i < m_instructionIndex; i++)
            {
                _time -= _instructions[i]->getDuration();
            }
            m_instructionTime = _time;
        }
    }

    void TweenComponent::applyCoordinates( const TweenCoordinates& _coordinates, const f32 _actorAngleOffset, const f32 _dt )
    {
        m_currentCoordinates = _coordinates;

        // transform to actor space
        const Transform3d initTransform(m_actor->getBoundLocalInitialPos(), m_actor->getBoundLocalInitialAngle());
        const Vec3d targetPos = initTransform.transformPos(_coordinates.m_pos);
        const f32 targetAngle = initTransform.transformAngle(_coordinates.m_actorAngle + _actorAngleOffset);
        const Vec2d targetScale = m_actor->getLocalInitialScale() * _coordinates.m_scaleMultiplier;

        // transform to world (for accessor & debug)
        m_targetPos = m_actor->transformToWorld(targetPos);


#ifdef ITF_SUPPORT_DEBUGFEATURE
        if (_dt)
        {
            const Vec3d posDelta = (targetPos - m_actor->getBoundLocalPos());
            m_debugLinearSpeed2d = (posDelta.truncateTo2D() / _dt).norm();
            m_debugLinearSpeed3d = (posDelta / _dt).norm();

            const f32 angleDelta = (targetAngle - m_actor->getBoundLocalAngle());
            m_debugAngularSpeed = (angleDelta / _dt);
        }
#endif // ITF_SUPPORT_DEBUGFEATURE


        if (m_applyPosition)
        {
            m_actor->setBoundLocalPos(targetPos);
        }

        if (m_applyRotation)
        {
            m_actor->setBoundLocalAngle(targetAngle);
        }

        if (m_applyScale)
        {
            m_actor->setScale(targetScale);
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TweenComponent::resetDebugSpeed()
    {
        m_debugLinearSpeed2d = 0.f;
        m_debugLinearSpeed3d = 0.f;
        m_debugAngularSpeed = 0.f;
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    bbool TweenComponent::getSetTransform( const TweenInstruction* _instruction, Transform3d& _setTransform ) const
    {
        u32 setIndex = (m_currentSetIndex != U32_INVALID) ? m_currentSetIndex : 0;

        if ( setIndex >= m_instructionSets.size() )
        {
            return bfalse;
        }


        const InstructionSet& instructionSet = m_instructionSets[setIndex];
        const InstructionList& instructions = instructionSet.m_instructions;

        TweenCoordinates coordinates = m_setBaseCoordinates;

        for (u32 i = 0; i < instructions.size(); i++)
        {
            const TweenInstruction* instruction = instructions[i];
            if (instruction == _instruction)
            {
                _setTransform = coordinates.getTransform();
                return btrue;
            }

            instruction->executeEnd(coordinates);
        }

        return bfalse;
    }

    // Get position on the trajectory at a given absolute time since trajectory start.
    Vec3d TweenComponent::getPosAtTime( f32 _t, f32* _angle /*= NULL */ )
    {
        if ( m_currentSetIndex >= m_instructionSets.size() )
        {
            //ITF_ASSERT_MSG(0, "no current instruction set!");
            return m_actor->getPos();
        }


        InstructionSet& instructionSet = m_instructionSets[m_currentSetIndex];
        InstructionList& instructions = instructionSet.m_instructions;

        TweenCoordinates coordinates = m_setBaseCoordinates;

        u32 failSafe = 0;
        while ( getPosAtTime(instructions, _t, coordinates) )
        {
            failSafe++;
            if (failSafe > 100)
            {
                ITF_ASSERT(0);
                break;
            }
        }


        const Transform3d initTransform(m_actor->getBoundWorldInitialPos(), m_actor->getBoundWorldInitialAngle());

        if (_angle)
        {
            *_angle = initTransform.transformAngle(coordinates.m_actorAngle);
        }

        return initTransform.transformPos(coordinates.m_pos);
    }

    // Get position on the trajectory at a given time relative to the current position (current instruction + m_t).
    Vec3d TweenComponent::getPosAtTimeOffset( f32 _tOffset, f32* _angle /*= NULL */ )
    {
        f32 t = Max(0.f, m_instructionSetTime + _tOffset);
        return getPosAtTime(t, _angle);
    }

    bbool TweenComponent::getPosAtTime( const InstructionList& _instructions, f32& _time, TweenCoordinates& _coordinates )
    {
        const u32 instructionCount = _instructions.size();
        for (u32 i = 0; i < instructionCount; i++)
        {
            TweenInstruction* instruction = _instructions[i];
            f32 duration = instruction->getDuration();
            if (duration < _time)
            {
                _time -= duration;
                instruction->executeEnd(_coordinates);
            }
            else
            {
                f32 t = _time / duration;
                instruction->execute(t, _coordinates);
                return bfalse;
            }
        }

        // reached the end of the list, will need to loop
        return btrue;
    }

    void TweenComponent::sendEventToChildren( Event* _event )
    {
        if (m_linkComponent)
        {
            EVENTDELAY_HANDLER->sendEventToChildren(m_linkComponent, _event, btrue);
        }
    }

    void TweenComponent::onSetFinished( InstructionSet& _set )
    {
        for (u32 i = 0; i < _set.m_instructions.size(); i++)
        {
            _set.m_instructions[i]->onSetFinished();
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    // draw given set (and next ones), then triggables (and next ones)
    void TweenComponent::drawSet( const Transform3d& _initTransform, const StringID& _setName ) const
    {
        u32 setIndex = getTemplate()->getSetIndex(_setName);
        ITF_WARNING_CATEGORY(LD, m_actor, setIndex != U32_INVALID, "unknown set '%s'", _setName.getDebugString());
        if (setIndex == U32_INVALID)
        {
            return;
        }

        TweenCoordinates coordinates;
        drawSetAndNexts(setIndex, getTriggableIndex(setIndex), _initTransform, coordinates);
    }

    // draw given set (and next ones), then triggables (and next ones)
    void TweenComponent::drawSetAndNexts( const u32 _firstSetIndex, const u32 _triggableOffset,
        const Transform3d& _initTransform, TweenCoordinates& _coordinates,
        bbool _altFirstSetColor /*= bfalse*/ ) const
    {
        // build set list
        SafeArray<u32> setList;
        pushSetAndNexts(setList, _firstSetIndex);


        // add triggable sets
        const TweenComponent_Template::TriggableSetList& tplTriggableSets = getTemplate()->getTriggableSets();
        if (_triggableOffset == U32_INVALID)
        {
            for (u32 i = 0; i < tplTriggableSets.size(); i++)
            {
                u32 setIndex = tplTriggableSets[i];
                pushSetAndNexts(setList, setIndex);
            }
        }
        else
        {
            for (u32 i = 0; i < tplTriggableSets.size(); i++)
            {
                u32 iOff = (i + _triggableOffset + 1) % tplTriggableSets.size();
                u32 setIndex = tplTriggableSets[iOff];
                if (setIndex != _firstSetIndex)
                {
                    pushSetAndNexts(setList, setIndex);
                }
            }
        }

        drawSetList(setList, _initTransform, _coordinates, _altFirstSetColor);
    }

    void TweenComponent::pushSetAndNexts( SafeArray<u32>& _setList, u32 setIndex ) const
    {
        SafeArray<u32> processedSets;

        while ( setIndex != U32_INVALID &&
            processedSets.find(setIndex) == -1)
        {
            _setList.push_back(setIndex);
            processedSets.push_back(setIndex);

            const InstructionSet_Template& tplSet = getTemplate()->getInstructionSets()[setIndex];
            setIndex = tplSet.getNextSetIndex();
        }
    }

    void TweenComponent::drawSetList( SafeArray<u32>& _setList,
        const Transform3d& _initTransform, TweenCoordinates& _coordinates,
        bbool _altFirstSetColor /*= bfalse*/ ) const
    {
        if (!_setList.size())
        {
            return;
        }


        for (u32 i = 0; i < _setList.size(); i++)
        {
            const u32 setIndex = _setList[i];
            const InstructionSet& instructionSet = m_instructionSets[setIndex];

            Color color = (_altFirstSetColor && i == 0) ? Color::pink() : Color::yellow();
            drawInstructionList(instructionSet.m_instructions, _initTransform, _coordinates, color, color);
            drawExtraIterations(setIndex, _initTransform, _coordinates);
        }


        //// TODO: should we loop the whole list?
        //const u32 lastSetIndex = _setList[_setList.size() - 1];
        //const InstructionSet_Template& lastTplSet = getTemplate()->getInstructionSets()[lastSetIndex];

        //if (lastTplSet.getNextSetIndex() != U32_INVALID)
        //{
        //}
    }

    void TweenComponent::drawExtraIterations( const u32 _setIndex, const Transform3d& _initTransform, TweenCoordinates& _coordinates ) const
    {
        const InstructionSet_Template& tplSet = getTemplate()->getInstructionSets()[_setIndex];
        const InstructionSet& instructionSet = m_instructionSets[_setIndex];

        if ( !tplSet.getIsLooping() )
        {
            Color color = Color::white();
            Color altColor = Color::white();
            //Color altColor = Color::black();

            // not looping: draw extra iterations
            u32 drawCount = tplSet.getIterationCount();
            if (drawCount == 0)
            {
                drawCount = 3; // infinite
            }
            else
            {
                drawCount = Min(drawCount, (u32)10); // cap in case user data is nuts
            }

            ITF_ASSERT(0 < drawCount && drawCount <= 10);
            for (u32 i = 0; i < drawCount - 1; i++)
            {
                drawInstructionList(instructionSet.m_instructions, _initTransform, _coordinates, color, altColor);
            }
        }
    }

    void TweenComponent::drawInstructionList( const InstructionList& _instructions,
        const Transform3d& _initTransform, TweenCoordinates& _coordinates,
        const Color& _normalColor, const Color& _alternateColor ) const
    {
        for (u32 i = 0; i < _instructions.size(); i++)
        {
            const TweenInstruction* instruction = _instructions[i];

            const Color& color = (i % 2 == 0) ? _normalColor : _alternateColor;

            String iString;iString.setTextFormat("%d", i);
            DebugDraw::circle(_initTransform.transformPos(_coordinates.m_pos), 0.1f, color, 0.f, iString);

            instruction->drawDebug(_initTransform, _coordinates, color);

            instruction->executeEnd(_coordinates);
        }
    }

    void TweenComponent::drawCurrentInfo() const
    {
        if ( m_currentSetIndex >= getTemplate()->getInstructionSets().size() )
        {
            return;
        }

        const InstructionSet_Template& tplSet = getTemplate()->getInstructionSets()[m_currentSetIndex];

        if ( m_instructionIndex >= tplSet.getInstructions().size() )
        {
            return;
        }

        const TweenInstruction_Template* tplIns = tplSet.getInstructions()[m_instructionIndex];


        f32 textY = 40;

        if (m_debugLinearSpeed2d == m_debugLinearSpeed3d)
        {
            drawDebugText(textY, "speed : %.2f m/s", m_debugLinearSpeed2d);
        }
        else
        {
            drawDebugText(textY, "speed : %.2f m/s / %.2f m/s", m_debugLinearSpeed2d, m_debugLinearSpeed3d);
        }

        drawDebugText(textY, "ang. speed : %.0f deg/s", m_debugAngularSpeed * MTH_RADTODEG);

        drawDebugText(textY, "set : %d (%s) - ins: %d (%s) - queued: %d",
            m_currentSetIndex, tplSet.getName().getDebugString(),
            m_instructionIndex, tplIns->getName().getDebugString(),
            (i32)m_nextSetIndex);

        drawDebugText(textY, "time : %.1f s / %.1f s - ins: %.1f s / %.1f s - iter: %d",
            m_instructionSetTime, tplSet.getDuration(),
            m_instructionTime, tplIns->getDuration(),
            m_currentSetIteration);

#ifndef ITF_FINAL
        DebugDraw::circle(m_targetPos, 0.10f, Color::yellow(), getTemplate()->getDebugDuration());
#endif //ITF_FINAL

    }

#define CREATE_BUFFER()\
    const int bufferSize = 1024;\
    char buffer[bufferSize];\
    va_list args;\
    va_start(args, _format);\
    _vsnprintf_s(buffer, bufferSize-1, _format, args);\
    va_end(args);\
    buffer[bufferSize-1] = 0;\

    void TweenComponent::drawDebugText( f32& textY, const char* _format, ... ) const
    {
        CREATE_BUFFER();

        DebugDraw::text(m_actor->getPos(), Vec2d(20, textY), Color::white(), DebugDraw::defaultBackgroundColor(), buffer);
#ifdef DEVELOPER_BEN
        DebugDraw::text(buffer);
#endif // DEVELOPER_BEN

        textY += f32(GFX_ADAPTER->getDebugFontSize());
    }

    // draw current/first set (and next ones), then triggables (and next ones)
    void TweenComponent::drawEditCurrent() const
    {
        if (!m_instructionSets.size())
        {
            return;
        }

        if (m_currentSetIndex != U32_INVALID)
        {
            // draw current set
            const Transform3d initTransform(m_actor->getBoundWorldInitialPos(), m_actor->getBoundWorldInitialAngle());
            TweenCoordinates coordinates = m_setBaseCoordinates;
            drawSetAndNexts(m_currentSetIndex, m_triggableIndex, initTransform, coordinates, btrue);
        }
        else
        {
            drawEditDeterministic();
        }
    }

    // draw start set (and next ones), then triggables (and next ones)
    void TweenComponent::drawEditDeterministic() const
    {
        if (!m_instructionSets.size())
        {
            return;
        }


        u32 setIndex = getStartSetIndex();
        if (setIndex == U32_INVALID)
        {
            setIndex = 0;
        }

        const Transform3d initTransform(m_actor->getBoundWorldInitialPos(), m_actor->getBoundWorldInitialAngle());
        TweenCoordinates coordinates;
        drawSetAndNexts(setIndex, getTriggableIndex(setIndex), initTransform, coordinates);
    }

    void TweenComponent::drawEdit( ActorDrawEditInterface* _drawInterface , u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        drawEditCurrent();
        //drawEditDeterministic();

        drawCurrentInfo();
    }

    bbool TweenComponent::s_debugLock(bfalse);
#endif // ITF_SUPPORT_EDITOR

    void TweenComponent::initInputs()
    {
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

        m_inputList.resize(getTemplate()->getInputs().size());

        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            Input* input = &m_inputList[i];
            const InputDesc& inputDesc = getTemplate()->getInputs()[i];

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

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(TweenComponent_Template)
    BEGIN_SERIALIZATION(TweenComponent_Template)
        SERIALIZE_FUNCTION(clear, ESerialize_PropertyEdit_Load);
        SERIALIZE_CONTAINER_OBJECT("instructionSets", m_instructionSets);
        SERIALIZE_MEMBER("sync", m_syncWithMetronome);
        SERIALIZE_METRONOMETYPE("metronome", m_metronome);
        SERIALIZE_MEMBER("syncOffset", m_syncOffset);
        SERIALIZE_STARTMODE("startMode", m_startMode);
        SERIALIZE_MEMBER("startSet", m_startSet);
        SERIALIZE_MEMBER("applyPosition", m_applyPosition);
        SERIALIZE_MEMBER("applyRotation", m_applyRotation);
        SERIALIZE_MEMBER("applyScale", m_applyScale);
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifndef ITF_FINAL
            SERIALIZE_MEMBER("debugDuration", m_debugDuration);
#endif //ITF_FINAL
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

    TweenComponent_Template::TweenComponent_Template()
        : m_syncWithMetronome(bfalse)
        , m_metronome(METRONOME_TYPE_DEFAULT)
        , m_syncOffset(0.f)
        , m_startMode(StartMode_Auto)
        , m_startSetIndex(U32_INVALID)
        , m_applyPosition(btrue)
        , m_applyRotation(btrue)
        , m_applyScale(btrue)
#ifndef ITF_FINAL
        , m_debugDuration(0.f)
#endif // ITF_FINAL
    {
    }

    void TweenComponent_Template::clear()
    {
        m_instructionSets.clear();
    }

    void TweenComponent_Template::onTemplateLoaded()
    {
        if (m_startSet.isValid())
        {
            m_startSetIndex = getSetIndex(m_startSet);
        }

        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            InstructionSet& tplSet = m_instructionSets[i];

            tplSet.onTemplateLoaded();

            if (tplSet.getTriggable())
            {
                m_triggableSets.push_back(i);
            }

            if (tplSet.getNextSet().isValid())
            {
                u32 setIndex = getSetIndex(tplSet.getNextSet());
                tplSet.setNextSetIndex(setIndex);
            }
        }
    }

    u32 TweenComponent_Template::getSetIndex( const StringID& _setName ) const
    {
        for (u32 i = 0; i < m_instructionSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = m_instructionSets[i];
            if (tplSet.getName() == _setName)
            {
                return i;
            }
        }

        return U32_INVALID;
    }

    BEGIN_SERIALIZATION_SUBCLASS(TweenComponent_Template, InstructionSet)
        SERIALIZE_MEMBER("name", m_name);
        SERIALIZE_CONTAINER_WITH_FACTORY("instructions", m_instructions, TweenInstructionFactory_Template::get());
        SERIALIZE_MEMBER("iterationCount", m_iterationCount);
        SERIALIZE_MEMBER("angleOffset", m_angleOffset);
        SERIALIZE_OBJECT_WITH_FACTORY("startEvent", m_startEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("stopEvent", m_stopEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("interruptible", m_interruptible);
        SERIALIZE_MEMBER("nextSet", m_nextSet);
        SERIALIZE_MEMBER("triggable", m_triggable);
        SERIALIZE_OBJECT("proceduralInput", m_proceduralCursor);
    END_SERIALIZATION()

    TweenComponent_Template::InstructionSet::InstructionSet()
        : m_iterationCount(0)   // default: infinite
        , m_startEvent(NULL)
        , m_stopEvent(NULL)
        , m_interruptible(bfalse)
        , m_triggable(bfalse)
        , m_nextSet()
        , m_nextSetIndex(U32_INVALID)

        , m_duration(0.f)
        , m_syncRatio(0.f)
        , m_isLooping(bfalse)
    {
    }

    TweenComponent_Template::InstructionSet::~InstructionSet()
    {
        clear();
    }

    void TweenComponent_Template::InstructionSet::clear()
    {
        SF_DEL(m_startEvent);
        SF_DEL(m_stopEvent);

        for (u32 i = 0; i < m_instructions.size(); i++)
        {
            SF_DEL(m_instructions[i]);
        }
    }

    void TweenComponent_Template::InstructionSet::onTemplateLoaded()
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

};
