
#ifndef _ITF_WAYPOINTCOMPONENT_H_
#define _ITF_WAYPOINTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

    class WaypointComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(WaypointComponent,ActorComponent,375105077);

    public:
        DECLARE_SERIALIZE()

        WaypointComponent();
        ~WaypointComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    protected:

        ITF_INLINE const class WaypointComponent_Template*  getTemplate() const;


        void                clear();

        bbool               m_registered;
    };

    //---------------------------------------------------------------------------------------------------

    class WaypointComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(WaypointComponent_Template,TemplateActorComponent,2944183250);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(WaypointComponent);

    public:

        WaypointComponent_Template();
        ~WaypointComponent_Template() {}

        const StringID&     getID() const { return m_id; }

    private:

        StringID            m_id;
    };



    ITF_INLINE const WaypointComponent_Template*  WaypointComponent::getTemplate() const {return static_cast<const WaypointComponent_Template*>(m_template);}
}

#endif // _ITF_WAYPOINTCOMPONENT_H_

