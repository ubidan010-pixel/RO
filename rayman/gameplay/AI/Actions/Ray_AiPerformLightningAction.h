#ifndef _ITF_RAY_AIPERFORMLIGHTNINGACTION_H_
#define _ITF_RAY_AIPERFORMLIGHTNINGACTION_H_


#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_


#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif
namespace ITF
{
    class Ray_AiPerformLightningAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AiPerformLightningAction, AIAction,3526689695);
        DECLARE_VALIDATE_ACTION()

    public:

        Ray_AiPerformLightningAction();
        virtual ~Ray_AiPerformLightningAction();

        virtual void        update( f32 _delta );
        virtual void        onEvent( Event* _event);
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onDeactivate();

    protected:
        ITF_INLINE const class Ray_AiPerformLightningAction_Template * getTemplate() const;
        
        class Ray_LightningPatchAIComponent * m_lightningPatchComponent;
    };

    class Ray_AiPerformLightningAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AiPerformLightningAction_Template,AIAction_Template,1604504864);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AiPerformLightningAction);

    public:

        Ray_AiPerformLightningAction_Template();
        ~Ray_AiPerformLightningAction_Template(){}
    private:
    };

    ITF_INLINE const class Ray_AiPerformLightningAction_Template * Ray_AiPerformLightningAction::getTemplate() const
    {
        return static_cast<const class Ray_AiPerformLightningAction_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIPERFORMLIGHTNINGACTION_H_

