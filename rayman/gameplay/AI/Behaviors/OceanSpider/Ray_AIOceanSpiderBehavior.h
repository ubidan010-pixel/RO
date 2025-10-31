#ifndef _ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_
#define _ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AIOceanSpiderBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIOceanSpiderBehavior, AIBehavior,1190991647)

    public:

        Ray_AIOceanSpiderBehavior();
        virtual ~Ray_AIOceanSpiderBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActionFinished();
        virtual void                        onEvent(Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        void                                activate(bbool _activate);

    protected:
        ITF_INLINE const class Ray_AIOceanSpiderBehavior_Template * getTemplate() const;

    private:
        void playAction( StringID name );

        struct Action
        {
            Action() : m_action(NULL){}
            StringID m_name;
            AIAction * m_action;
        };
        struct Instruction
        {
            Instruction() : m_playCount(0) {}
            StringID m_actionName;
            u32 m_playCount;
        };
        ITF_VECTOR<Action> m_actions;
        ITF_VECTOR<Instruction> m_instructions;
        ITF_VECTOR<Instruction> m_inactiveInstructions;
        u32 m_currentInstruction;
        bbool m_active;
        bbool m_requestActivation;
    };

    class Ray_AIOceanSpiderBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIOceanSpiderBehavior_Template,TemplateAIBehavior,1883528063);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIOceanSpiderBehavior);

    public:

        Ray_AIOceanSpiderBehavior_Template();
        ~Ray_AIOceanSpiderBehavior_Template();

        struct ActionTemplate
        {
            DECLARE_SERIALIZE()
            ActionTemplate() : m_action(NULL){}
            ~ActionTemplate(){ SF_DEL(m_action); }
            StringID m_name;
            AIAction_Template * m_action;
        };

        struct InstructionTemplate
        {
            DECLARE_SERIALIZE()
            InstructionTemplate() : m_playCount(1) {}
            StringID m_actionName;
            u32 m_playCount;
        };

        ITF_INLINE const ITF_VECTOR<ActionTemplate> & getActions() const { return m_actions; }
        ITF_INLINE const ITF_VECTOR<InstructionTemplate> & getInstructions() const { return m_instructions; }
        ITF_INLINE const ITF_VECTOR<InstructionTemplate> & getInactiveInstructions() const { return m_inactiveInstructions; }
    private:
        ITF_VECTOR<ActionTemplate> m_actions;
        ITF_VECTOR<InstructionTemplate> m_instructions;
        ITF_VECTOR<InstructionTemplate> m_inactiveInstructions;
    };
    ITF_INLINE const Ray_AIOceanSpiderBehavior_Template * Ray_AIOceanSpiderBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIOceanSpiderBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_
