#ifndef _ITF_RAY_HUNTERBOSSNODECOMPONENT_H_
#define _ITF_RAY_HUNTERBOSSNODECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    class Ray_HunterBossNodeComponent : public LinkComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_HunterBossNodeComponent, LinkComponent,3283634905)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_HunterBossNodeComponent();
        virtual ~Ray_HunterBossNodeComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags )const;
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_HunterBossNodeComponent_Template* getTemplate() const;

        Ray_EventQueryHunterNode m_nodeData;
    };

    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_HunterBossNodeComponent_Template : public LinkComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_HunterBossNodeComponent_Template, LinkComponent_Template,2218509339);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_HunterBossNodeComponent);

    public:

        Ray_HunterBossNodeComponent_Template(){}
        ~Ray_HunterBossNodeComponent_Template() {}


    private:
    };

    const Ray_HunterBossNodeComponent_Template* Ray_HunterBossNodeComponent::getTemplate() const
    {
        return static_cast<const Ray_HunterBossNodeComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_HUNTERBOSSNODECOMPONENT_H_