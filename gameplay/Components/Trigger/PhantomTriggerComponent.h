#ifndef _ITF_PHANTOMTRIGGERCOMPONENT_H_
#define _ITF_PHANTOMTRIGGERCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

namespace ITF
{
    /*
        Just a trigger that only uses PhantomDetectors.
        Workaround to allow having 2 detectors & 2 triggers on one actor,
        one reacting on players and the other on NPCs...
    */
    class PhantomTriggerComponent : public TriggerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PhantomTriggerComponent, TriggerComponent,2366929498);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        PhantomTriggerComponent();
        virtual ~PhantomTriggerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );

    private:
        ITF_INLINE const class PhantomTriggerComponent_Template* getTemplate() const;

    };


    //---------------------------------------------------------------------------------------------------

    class PhantomTriggerComponent_Template : public TriggerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(PhantomTriggerComponent_Template, TriggerComponent_Template,3505476799);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PhantomTriggerComponent);

    public:

        PhantomTriggerComponent_Template()
        {
        }
        virtual ~PhantomTriggerComponent_Template() {}

    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const PhantomTriggerComponent_Template* PhantomTriggerComponent::getTemplate() const
    {
        return static_cast<const PhantomTriggerComponent_Template*>(m_template);
    }
};

#endif //_ITF_PHANTOMTRIGGERCOMPONENT_H_