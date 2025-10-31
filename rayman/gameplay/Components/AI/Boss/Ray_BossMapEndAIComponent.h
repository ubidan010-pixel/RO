#ifndef _ITF_RAY_BOSSMAPENDAICOMPONENT_H_
#define _ITF_RAY_BOSSMAPENDAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    class AnimGameplayEvent;
    class Ray_BossMapEndAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossMapEndAIComponent, Ray_AIComponent,1038493720)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BossMapEndAIComponent();
        virtual ~Ray_BossMapEndAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent(Event* _event);

    private:

        ActorRef            getCinematic();

        ITF_INLINE const class Ray_BossMapEndAIComponent_Template* getTemplate() const;
    };

    //-------------------------------------------------------------------------------------

    class Ray_BossMapEndAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossMapEndAIComponent_Template, Ray_AIComponent_Template,410900912)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossMapEndAIComponent)

    public:

        Ray_BossMapEndAIComponent_Template();
        ~Ray_BossMapEndAIComponent_Template() {}


    private:
 };

    //-------------------------------------------------------------------------------------

    ITF_INLINE const Ray_BossMapEndAIComponent_Template* Ray_BossMapEndAIComponent::getTemplate() const
    {
        return static_cast<const Ray_BossMapEndAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BOSSMAPENDAICOMPONENT_H_