#ifndef _ITF_UIMENUPAGECOMPONENT_H_
#define _ITF_UIMENUPAGECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
	class UIMenuPageComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(UIMenuPageComponent,ActorComponent,1580941451);

	public:
		DECLARE_SERIALIZE()

		UIMenuPageComponent();
		virtual ~UIMenuPageComponent();

		virtual bbool       needsUpdate() const { return btrue; }
		virtual bbool       needsDraw() const { return bfalse; }
		virtual bbool       needsDraw2D() const { return bfalse; }

		virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
#ifdef ITF_SUPPORT_EDITOR
		virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
		virtual void        onEditorCreated( Actor* _original );
#endif // ITF_SUPPORT_EDITOR

		virtual void        Update( f32 _dt );

		const u32			getTrigger() { return m_pageTrigger;}

	private:
		ITF_INLINE const class UIMenuPageComponent_Template*  getTemplate() const;

		Vec3d				m_relativePosFromCam;
		u32					m_pageTrigger;
	};

	//---------------------------------------------------------------------------------------------------

	class UIMenuPageComponent_Template : public TemplateActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(UIMenuPageComponent_Template,TemplateActorComponent,4026707220);
		DECLARE_SERIALIZE();
		DECLARE_ACTORCOMPONENT_TEMPLATE(UIMenuPageComponent);

	public:

		UIMenuPageComponent_Template() {}
		~UIMenuPageComponent_Template() {}

	private:

	};

	const UIMenuPageComponent_Template*  UIMenuPageComponent::getTemplate() const
	{return static_cast<const class UIMenuPageComponent_Template*>(m_template);}
}

#endif // _ITF_UIMENUPAGECOMPONENT_H_

