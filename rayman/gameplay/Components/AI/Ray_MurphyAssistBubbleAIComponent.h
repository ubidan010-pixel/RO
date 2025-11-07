#ifndef _ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_
#define _ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

namespace ITF
{
    class Ray_MurphyAssistAIComponent;

    class Ray_MurphyAssistBubbleAIComponent : public Ray_FixedAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MurphyAssistBubbleAIComponent, Ray_FixedAIComponent, 2616088894);
        DECLARE_SERIALIZE()

    public:

        Ray_MurphyAssistBubbleAIComponent();
        virtual ~Ray_MurphyAssistBubbleAIComponent();

        virtual bbool                   needsUpdate()   const { return btrue; }
        virtual bbool                   needsDraw()     const { return bfalse; }
        virtual bbool                   needsDraw2D()   const { return bfalse; }

        void                            Update(f32 _dt);

        void                            setOwner(Ray_MurphyAssistAIComponent* _owner) { m_owner = _owner; }

    protected:

        void                            onActorLoaded(Pickable::HotReloadType _hotReload);
        void                            onEvent(Event* _event);
        void                            processPunchEvent(const PunchStim* const _event);

        Ray_MurphyAssistAIComponent*    m_owner;
        void                            broadcastHitEventToOwner();

        ITF_INLINE const class Ray_MurphyAssistBubbleAIComponent_Template* getTemplate() const;
    };

    class Ray_MurphyAssistBubbleAIComponent_Template : public Ray_FixedAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MurphyAssistBubbleAIComponent_Template, Ray_FixedAIComponent_Template, 1710378469);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MurphyAssistBubbleAIComponent);

    public:

        Ray_MurphyAssistBubbleAIComponent_Template();
        ~Ray_MurphyAssistBubbleAIComponent_Template();

    private:

    };

    const Ray_MurphyAssistBubbleAIComponent_Template* Ray_MurphyAssistBubbleAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_MurphyAssistBubbleAIComponent_Template*>(m_template);
    }

}

#endif //_ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_
