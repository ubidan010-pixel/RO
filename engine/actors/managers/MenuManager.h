#ifndef _ITF_MENUMANAGER_H_
#define _ITF_MENUMANAGER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

namespace ITF
{
	enum PageTrigger
	{
		PageTrigger_Default=0,
		PageTrigger_Start,
		PageTrigger_Back,

		PageTrigger_Count//RESERVED. Must be last of enum
	};
	//////////////////////////////////////////////////////////////////////////////////////////////

	class MenuManager
	{
	public:

		MenuManager();
		~MenuManager();

		void		update();
		void		registerPage(const ObjectRef& _page);
		void		unregisterPage(const ObjectRef& _page);
		void		registerInteractiveElem(const ObjectRef& _menuItem);
		void		unregisterInteractiveElem(const ObjectRef& _menuItem);

	private:

		void		checkForAction();
		void		playTransition(u32 _pageIndex);
		void		selectItem(const Vec2d& _dir);
		u32			findPageInWorkflow(u32 _index);
		u32			getPageTriggered(u32 _trigger);
		
		ObjectRefList		m_pages;
		ObjectRefList		m_menuItems;
		ITF_VECTOR<u32>		m_pageWorkflow;
		u32					m_selectedItemIndex;
		f32					m_joyTimer;
		bool				m_waitingForItem;
		bool				m_waitingForPage;
		bool				m_joystickReleased;
	};
}
#endif // _ITF_MENUMANAGER_H_
