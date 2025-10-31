#include "precompiled_engine.h"

#ifndef _ITF_MENUMANAGER_H_
#include "engine/actors/managers/MenuManager.h"
#endif //_ITF_MENUMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_UIMENUPAGECOMPONENT_H_
#include "gameplay/Components/UI/UIMenuPageComponent.h"
#endif //_ITF_UIMENUPAGECOMPONENT_H_

#ifndef _ITF_UIMENUITEMCOMPONENT_H_
#include "gameplay/Components/UI/UIMenuItemComponent.h"
#endif //_ITF_UIMENUITEMCOMPONENT_H_

namespace ITF
{
	///////////////////////////////////////////////////////////////////////////////////////////
	MenuManager::MenuManager ()
		: m_pages(8)
		, m_menuItems(16)
		, m_selectedItemIndex(0)
		, m_joyTimer(0.0f)
		, m_waitingForItem(false)
		, m_waitingForPage(false)
		, m_joystickReleased(true)
	{ 
        m_pageWorkflow.reserve(8);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	MenuManager::~MenuManager ()
	{  
		m_pages.clear();
		m_menuItems.clear();
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MenuManager::registerPage(const ObjectRef& _page)
	{
		if ( _page.isValid() && m_pages.find(_page) == -1 )
		{
			const Actor* pPage = (const Actor*)GETOBJECT(_page);
			if (pPage)
			{
				UIMenuPageComponent* MenuPageComponent = pPage->GetComponent<UIMenuPageComponent>();
				if ( MenuPageComponent )
				{
					// Add this page
					m_pages.push_back(_page);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MenuManager::unregisterPage(const ObjectRef& _page)
	{
		i32 index = m_pages.find(_page);
		if ( index != -1 )
		{
			m_pages.eraseNoOrder(index);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MenuManager::registerInteractiveElem(const ObjectRef& _menuItem)
	{
		if ( _menuItem.isValid() && m_menuItems.find(_menuItem) == -1 )
		{
			const Actor* pItem = (const Actor*)GETOBJECT(_menuItem);
			if (pItem)
			{
				UIMenuItemComponent* menuItemComponent = pItem->GetComponent<UIMenuItemComponent>();
				if ( menuItemComponent )
				{
					// Add this page
					m_menuItems.push_back(_menuItem);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MenuManager::unregisterInteractiveElem(const ObjectRef& _menuItem)
	{
		i32 index = m_menuItems.find(_menuItem);
		if ( index != -1)
		{
			m_menuItems.eraseNoOrder(index);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MenuManager::playTransition(u32 _pageIndex)
	{
		const Actor* pPage = (const Actor*)GETOBJECT(m_pages[_pageIndex]);
		if (pPage)
		{
			SequencePlayerComponent* sequencePlayer = pPage->GetComponent<SequencePlayerComponent>();
			if ( sequencePlayer )
			{
				// Resume sequence
				sequencePlayer->start();
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	u32 MenuManager::findPageInWorkflow(u32 _index)
	{
		for ( u32 i = 0 ; i < m_pageWorkflow.size() ; i++ )
		{
			if ( m_pageWorkflow[i] == _index )
			{
				return i;
			}
		}

		return U32_INVALID;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	u32 MenuManager::getPageTriggered(u32 _trigger)
	{
		for ( u32 i = 0 ; i < m_pages.size() ; i++ )
		{
			const Actor* pPage = (const Actor*)GETOBJECT(m_pages[i]);
			if (pPage)
			{
				UIMenuPageComponent* MenuPageComponent = pPage->GetComponent<UIMenuPageComponent>();
				if ( MenuPageComponent
					&& MenuPageComponent->getTrigger() == _trigger)
				{
					return i;
				}
			}
		}

		return U32_INVALID;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MenuManager::selectItem(const Vec2d& _dir)
	{
		// Get 2D pos of current selected Item
		Vec2d	startPos2D		= Vec2d::Zero;
		f32		bestScore		= -1.0f;
		u32		bestIndex		= U32_INVALID;
		const Actor* pCurrentItem = (const Actor*)GETOBJECT(m_menuItems[m_selectedItemIndex]);
		// There should be an item selected !
		ITF_ASSERT_MSG(pCurrentItem, "No item selected in the menu !");
		if (pCurrentItem == NULL)
			return;
			
		startPos2D = pCurrentItem->get2DPos();

		// Find closest item to select
		f32		currentAngleWithdir = 0.0f;
		f32		currentScore		= 0.0f;
		f32		currentDist			= 0.0f;
		Vec2d	vStartToItem		= Vec2d::Zero;
		for ( u32 i = 0 ; i < m_menuItems.size() ; i++ )
		{
			if ( i != m_selectedItemIndex )
			{
				const Actor* pItem = (const Actor*)GETOBJECT(m_menuItems[i]);
				if (pItem)
				{
					vStartToItem		= pItem->get2DPos() - startPos2D;
					currentDist			= vStartToItem.norm();
					currentAngleWithdir = _dir.getShortestAngle( vStartToItem );

					currentScore		= currentDist + currentAngleWithdir;

					if ( currentAngleWithdir < MTH_PIBY2
						&& (bestScore == -1.0f || currentScore < bestScore) )
					{
						bestScore = currentScore;
						bestIndex = i;
					}
				}
			}
		}

		// Found another item to select ?
		if ( bestIndex != U32_INVALID )
		{
			// Unselect current item
			UIMenuItemComponent* menuItemComponent = pCurrentItem->GetComponent<UIMenuItemComponent>();
			if ( menuItemComponent )
			{
				menuItemComponent->onUnselected();
			}

			// Select next Item
			const Actor* pNextItem = (const Actor*)GETOBJECT(m_menuItems[bestIndex]);
			menuItemComponent = pNextItem->GetComponent<UIMenuItemComponent>();
			if ( menuItemComponent )
			{
				menuItemComponent->onSelected();
			}

			// update selected Item
			m_selectedItemIndex	= bestIndex;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MenuManager::checkForAction()
	{
		m_joyTimer += LOGICDT;

		if ( m_pages.size() > 0 )
		{
			// Check for pads
			for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
			{
				/////////////////////////////////////////////////////////////
				//		ACTIONS ON MENU
				/////////////////////////////////////////////////////////////

				// Buttons
				InputAdapter::PressStatus buts[JOY_MAX_BUT];
				INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,i, buts, JOY_MAX_BUT);

				// Menu is displayed ?
				if ( m_pageWorkflow.size() > 0 )
				{
					// Back
					if (buts[m_joyButton_B] == InputAdapter::JustPressed)
					{
						m_waitingForPage = true;

						// play transition on current page and previous page
						if (m_pageWorkflow.size() > 1)
						{
							u32	currentPageIndex	= m_pageWorkflow.back();
							m_pageWorkflow.pop_back();
							u32 previousPageIndex	= m_pageWorkflow.back();

							// Play transition of current page
							playTransition(currentPageIndex);

							// Play transition of previous page
							playTransition(previousPageIndex);
						}
						// This is the first page -> going to close menu then
						else
						{
							playTransition(m_pageWorkflow.back());
							//m_pageWorkflow.pop_back();
						}
					}
					// Action
					else if ( buts[m_joyButton_A] == InputAdapter::JustPressed )
					{
						const Actor* pItem = (const Actor*)GETOBJECT(m_menuItems[m_selectedItemIndex]);
						if (pItem)
						{
							UIMenuItemComponent* pItemCmp = pItem->GetComponent<UIMenuItemComponent>();
							if ( pItemCmp )
							{
								pItemCmp->onAction();
								m_waitingForItem = true;
							}
						}
					}
				}
				else
				{
					u32 pageIndex = U32_INVALID;

					// Display Menu ?
					if (buts[m_joyButton_Start] == InputAdapter::JustPressed)
					{
						// TODO : Find a way to determine which page must be display at first (start ? pause ? select ?)

						pageIndex = getPageTriggered(PageTrigger_Start);
					}
					else if (buts[m_joyButton_Back] == InputAdapter::JustPressed)
					{
						// TODO : Find a way to determine which page must be display at first (start ? pause ? select ?)

						pageIndex = getPageTriggered(PageTrigger_Back);
					}

					if ( pageIndex != U32_INVALID )
					{
						playTransition(pageIndex);
						m_pageWorkflow.push_back(pageIndex);

						m_waitingForPage = true;
					}
				}

				/////////////////////////////////////////////////////////////
				//		MOVING INSIDE MENU
				/////////////////////////////////////////////////////////////

				// Joysticks
				float pos[JOY_MAX_AXES];
				INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentLua,i, pos, JOY_MAX_AXES);

				if ( m_menuItems.size() > 0 )
				{
					if ( pos[0] < -0.7f || pos[0] > 0.7f 
						|| pos[1] < -0.7f || pos[1] > 0.7f )
					{
						if ( m_joystickReleased )
						{
							m_joystickReleased	= false;
							m_joyTimer			= 0.0f;

							Vec2d dir = Vec2d::Zero;
							dir.m_x	= pos[0];
							dir.m_y = pos[1];

							// Normalize vector
							dir.normalize();

							selectItem(dir);
						}
					}
					else if ( m_joyTimer > 0.5f )
					{
						m_joystickReleased = true;
					}
				}
			}

		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MenuManager::update ()
	{
		// Check if a sequence is being played or an item is playing an animation (wait until the end before doing something else)
		bool isReady = true;
		if ( m_pageWorkflow.size() > 0 )
		{
			if ( m_waitingForPage )
			{
				isReady = false;

				const Actor* pPage = (const Actor*)GETOBJECT(m_pages[m_pageWorkflow.back()]);
				if (pPage)
				{
					SequencePlayerComponent* sequencePlayer = pPage->GetComponent<SequencePlayerComponent>();
					if ( sequencePlayer )
					{
						u32 sequenceState = sequencePlayer->getState();
						// Reached the end ? i.e. is hidden ?
						if ( sequenceState == SequencePlayerComponent::State_Stopped )
						{
							// Remove first page as none of them is displayed now
							m_pageWorkflow.pop_back();
							isReady = true;
							m_waitingForPage = false;
						}
						// Animation of the current page is over ?
						else if ( sequencePlayer->getState() == SequencePlayerComponent::State_Paused )
						{
							isReady = true;
							m_waitingForPage = false;

							// Select first item by default
							m_selectedItemIndex = 0;

							if ( m_menuItems.size() > 0 )
							{
								const Actor* pItem = (const Actor*)GETOBJECT(m_menuItems[m_selectedItemIndex]);
								if (pItem)
								{
									UIMenuItemComponent* pItemCmp = pItem->GetComponent<UIMenuItemComponent>();
									if ( pItemCmp )
									{
										pItemCmp->onSelected();
									}
								}
							}
						}
					}
				}
			}

			// Checking current item state and maybe change page if needed
			if ( isReady && m_menuItems.size() > 0 )
			{
				const Actor* pItem = (const Actor*)GETOBJECT(m_menuItems[m_selectedItemIndex]);
				if (pItem)
				{
					UIMenuItemComponent* pItemCmp = pItem->GetComponent<UIMenuItemComponent>();
					if ( pItemCmp
						&& m_waitingForItem
						&& pItemCmp->getState() == UIMenuItemComponent::ItemState_Idle )
					{
						// This item is linked to another page ?
						const ObjectRef nextPage = pItemCmp->getNextPage();
						if ( nextPage != NULL
							&& nextPage.isValid() )
						{
							i32 nextIndex = m_pages.find(nextPage);
							if ( nextIndex != -1 )
							{
								u32	currentPageIndex = m_pageWorkflow.back();

								// Play transition of current page
								playTransition(currentPageIndex);

								// Play transition of next page
								playTransition(nextIndex);

								u32 pageWorkflowIndex = findPageInWorkflow(nextIndex);
								// Going back to a page we previously visited ?
								if ( pageWorkflowIndex != U32_INVALID )
								{
									// Erase all history after this page then
									u32 nbElemToDelete = (m_pageWorkflow.size() - 1) - pageWorkflowIndex;
									while ( nbElemToDelete > 0 )
									{
										m_pageWorkflow.pop_back();
										nbElemToDelete--;
									}
								}
								else
								{
									// Keep history of visited pages
									m_pageWorkflow.push_back(nextIndex);
								}

								m_waitingForPage = true;
							}
						}

						m_waitingForItem = false;
					}
				}
			}
		}

		if ( isReady )
		{
			// Check pad state and launch action if necessary
			checkForAction();
		}
	}
}
