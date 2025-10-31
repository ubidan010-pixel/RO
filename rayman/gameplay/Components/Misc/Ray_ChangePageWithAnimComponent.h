#ifndef _ITF_RAY_CHANGEPAGEWITHANIMCOMPONENT_H_
#define _ITF_RAY_CHANGEPAGEWITHANIMCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

    class Ray_ChangePageWithAnimComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI( Ray_ChangePageWithAnimComponent, ActorComponent ,4229378487)
        DECLARE_SERIALIZE()

    public:

		Ray_ChangePageWithAnimComponent();
		virtual ~Ray_ChangePageWithAnimComponent();

        virtual void                    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual bbool                   needsUpdate() const   { return bfalse; }
        virtual bbool                   needsDraw() const     { return bfalse; }
        virtual bbool                   needsDraw2D() const   { return bfalse; }
		
		virtual void                    onEvent( Event * _event);

    private:
		void						    processChangePageEvent( Ray_EventChangePage * _evt );
        void                            processChangePageQueryPointsEvent( Ray_EventChangePageQueryPoints * _evt );
        void                            processEventTrigger( EventTrigger * _evt );

        class Ray_ChangePageComponent*  getDestinationPageComponent() const;
        ActorRef                        getActivateCheckpoint() const;

        class LinkComponent *           m_linkComponent;
        class FXControllerComponent*    m_fxControllerComponent;
        class AnimLightComponent*       m_animLightComponent;

        ITF_INLINE const class Ray_ChangePageWithAnimComponent_Template*  getTemplate() const;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_ChangePageWithAnimComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI( Ray_ChangePageWithAnimComponent_Template, TemplateActorComponent ,2263342643);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ChangePageWithAnimComponent);

    public:

        Ray_ChangePageWithAnimComponent_Template();
        ~Ray_ChangePageWithAnimComponent_Template() {}

        const StringID&                         getAnim() const { return m_anim; }
        const StringID&                         getAspireAnim() const { return m_aspireAnim; }
        const StringID&                         getAspirePoint() const { return m_aspirePoint; }

    private:

        StringID                                m_anim;
        StringID                                m_aspireAnim;
        StringID                                m_aspirePoint;
    };


    const Ray_ChangePageWithAnimComponent_Template*     Ray_ChangePageWithAnimComponent::getTemplate() const {return static_cast<const Ray_ChangePageWithAnimComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_CHANGEPAGEWITHANIMCOMPONENT_H_

