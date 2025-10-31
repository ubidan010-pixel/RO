#ifndef _ITF_GENERICAICOMPONENT_H_
#define _ITF_GENERICAICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

namespace ITF
{
    class GenericAIComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(GenericAIComponent, AIComponent,3282978206);
        DECLARE_SERIALIZE()

    public:

        GenericAIComponent();
        virtual ~GenericAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    protected:
        ITF_INLINE const class GenericAIComponent_Template * getTemplate() const;

        AIBehavior*         m_behavior;
    };

    class GenericAIComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(GenericAIComponent_Template,AIComponent_Template,3427293913);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(GenericAIComponent);

    public:

        GenericAIComponent_Template();
        ~GenericAIComponent_Template();

        ITF_INLINE const class TemplateAIBehavior* getBehavior() const { return m_behavior; }

    private:
        class TemplateAIBehavior*         m_behavior;
    };

    ITF_INLINE const class GenericAIComponent_Template * GenericAIComponent::getTemplate() const
    {
        return static_cast<const class GenericAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_GENERICAICOMPONENT_H_