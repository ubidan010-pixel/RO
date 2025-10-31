#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#define _ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{

    class Ray_FixedAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FixedAIComponent, Ray_AIComponent,3817174854);
        DECLARE_SERIALIZE()

    public:
        Ray_FixedAIComponent()
            : m_behavior(NULL)
        {
        }

        ~Ray_FixedAIComponent()
        {
        }


        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual  void onCheckpointLoaded()
        {
            Super::onCheckpointLoaded();
            setBehavior(m_behavior);
        }

#ifdef ITF_SUPPORT_EDITOR
        virtual void onEditorMove(bbool _modifyInitialPos = btrue)
        {
            Super::onEditorMove(_modifyInitialPos);

            if (m_behavior)
            {
                m_behavior->onEditorMove(_modifyInitialPos);
            }
            setBehavior(m_behavior);
        }
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class Ray_FixedAIComponent_Template * getTemplate() const;

        AIBehavior*         m_behavior;
    };

    class Ray_FixedAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FixedAIComponent_Template,Ray_AIComponent_Template,3524586378);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FixedAIComponent);

    public:

        Ray_FixedAIComponent_Template();
        ~Ray_FixedAIComponent_Template();

        ITF_INLINE const TemplateAIBehavior* getBehavior() const { return m_behavior; }
    private:
        TemplateAIBehavior*         m_behavior;
    };

    ITF_INLINE const Ray_FixedAIComponent_Template * Ray_FixedAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_FixedAIComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_FIXEDAICOMPONENT_H_