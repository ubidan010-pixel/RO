#ifndef _ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_
#define _ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERBASEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterBaseBehavior.h"
#endif //_ITF_RAY_AIWATERBASEBEHAVIOR_H_


namespace ITF
{
    class Ray_AILivingStoneDrowningBehavior : public Ray_AIWaterBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILivingStoneDrowningBehavior, Ray_AIWaterBaseBehavior,1712015551);
        DECLARE_SERIALIZE()

    public:

        Ray_AILivingStoneDrowningBehavior();
        virtual ~Ray_AILivingStoneDrowningBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActionFinished();
    private:

        ITF_INLINE const class Ray_AILivingStoneDrowningBehavior_Template * getTemplate() const;

        AIAction*                           m_diveAction;
        AIAction*                           m_drownAction;
    };

    class Ray_AILivingStoneDrowningBehavior_Template : public Ray_AIWaterBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILivingStoneDrowningBehavior_Template,Ray_AIWaterBaseBehavior_Template,224911302);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AILivingStoneDrowningBehavior);

    public:

        Ray_AILivingStoneDrowningBehavior_Template();
        ~Ray_AILivingStoneDrowningBehavior_Template();

        const AIAction_Template*        getDiveAction() const { return m_diveAction; }
        const AIAction_Template*        getDrownAction() const { return m_drownAction; }

    private:

        AIAction_Template*          m_diveAction;
        AIAction_Template*          m_drownAction;
    };

    ITF_INLINE const class Ray_AILivingStoneDrowningBehavior_Template * Ray_AILivingStoneDrowningBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AILivingStoneDrowningBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_