#ifndef _ITF_UIMENUITEMCOMPONENT_H_
#define _ITF_UIMENUITEMCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
	class UIMenuItemComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(UIMenuItemComponent,ActorComponent,4006055000);
		DECLARE_SERIALIZE()

	public:

		enum ItemState
		{
			ItemState_Idle=0,
			ItemState_PlayingAnimation,

			ItemState_Count
		};

		UIMenuItemComponent();
		virtual ~UIMenuItemComponent();

		virtual bbool	needsUpdate() const { return btrue; }
		virtual bbool	needsDraw() const { return bfalse; }
		virtual bbool	needsDraw2D() const { return bfalse; }

		virtual void	onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
		virtual void	onBecomeActive();
		virtual void	onBecomeInactive();
		virtual void	Update( f32 _dt );

		void			UpdateState();

		void			onSelected();	// Play select anim / sound
		void			onUnselected(); // Play unselect anim / sound
		void			onAction();		// Play anim / sound when validating item (usually when pressing A/X button)

		const u32		getState() { return m_state; }

		const ObjectRef	getNextPage();	// returns linked page to this item if there is one

	private:
		ITF_INLINE const class UIMenuItemComponent_Template*  getTemplate() const;

		u32					m_state;
	};

	//---------------------------------------------------------------------------------------------------

	class UIMenuItemComponent_Template : public TemplateActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(UIMenuItemComponent_Template,TemplateActorComponent,780160883);
		DECLARE_SERIALIZE();
		DECLARE_ACTORCOMPONENT_TEMPLATE(UIMenuItemComponent);

	public:

		UIMenuItemComponent_Template() {}
		~UIMenuItemComponent_Template() {}

	private:

	};

	const UIMenuItemComponent_Template*  UIMenuItemComponent::getTemplate() const
	{return static_cast<const class UIMenuItemComponent_Template*>(m_template);}
}

#endif // _ITF_UIMENUITEMCOMPONENT_H_

