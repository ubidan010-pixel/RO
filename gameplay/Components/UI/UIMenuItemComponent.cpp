#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENUITEMCOMPONENT_H_
#include "gameplay/Components/UI/UIMenuItemComponent.h"
#endif //_ITF_UIMENUITEMCOMPONENT_H_

#ifndef _ITF_MENUMANAGER_H_
#include "engine/actors/managers/MenuManager.h"
#endif //_ITF_MENUMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

// TODO : remove this later, when playing animations
#ifndef _ITF_TODOCOMPONENT_H_
#include "gameplay/Components/Misc/TODOComponent.h"
#endif //_ITF_TODOCOMPONENT_H_
// END TODO

namespace ITF
{

	IMPLEMENT_OBJECT_RTTI(UIMenuItemComponent)
	BEGIN_SERIALIZATION_CHILD(UIMenuItemComponent)
	END_SERIALIZATION()

	UIMenuItemComponent::UIMenuItemComponent()
		: Super()
		, m_state(ItemState_Idle)
	{
	}


	UIMenuItemComponent::~UIMenuItemComponent()
	{
		// Unregister page to MenuManager
        if (MENUMANAGER && m_actor)
		    MENUMANAGER->unregisterInteractiveElem(m_actor->getRef());
	}

	void UIMenuItemComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		Super::onActorLoaded(_hotReload);
	}

	void UIMenuItemComponent::onBecomeActive()
	{
		Super::onBecomeActive();

		// Register page to MenuManager
		MENUMANAGER->registerInteractiveElem(m_actor->getRef());
	}

	void UIMenuItemComponent::onBecomeInactive()
	{
		Super::onBecomeInactive();

		// Unregister page to MenuManager
		MENUMANAGER->unregisterInteractiveElem(m_actor->getRef());
	}

	void UIMenuItemComponent::UpdateState()
	{
	}

	void UIMenuItemComponent::Update( f32 _dt )
	{
		Super::Update(_dt);

		UpdateState();
	}

	void UIMenuItemComponent::onSelected()
	{
		// TODO : play animation instead of changing color
		// TEMP
		TODOComponent* TODOCmp = m_actor->GetComponent<TODOComponent>();
		if ( TODOCmp )
		{
			// Switch to selected color
			TODOCmp->setTextColor(1.0f, 0.0f, 1.0f, 1.0f);
		}
		// END TEMP
	}

	void UIMenuItemComponent::onUnselected()
	{
		// TODO : play animation instead of changing color
		// TEMP
		TODOComponent* TODOCmp = m_actor->GetComponent<TODOComponent>();
		if ( TODOCmp )
		{
			// Switch to selected color
			TODOCmp->setTextColor(1.0f, 1.0f, 1.0f, 1.0f);
		}
		// END TEMP
	}

	void UIMenuItemComponent::onAction()
	{
		 // Play animation ?

		// Currently no animation
	}

	const ObjectRef UIMenuItemComponent::getNextPage()
	{
		// Is there a link component ?
		LinkComponent* linkCmps = m_actor->GetComponent<LinkComponent>();
		if ( linkCmps )
		{
			const LinkComponent::ChildrenList& children = linkCmps->getChildren();
			if( children.size() > 0 )
			{
				if ( Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[0].getPath()) )
				{
					if ( Actor* child = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
					{
						if (!child->hasDataError())
						{
							return child->getRef();
						}
					}
				}
			}
		}

		return NULL;
	}

	//-------------------------------------------------------------------------------------

	IMPLEMENT_OBJECT_RTTI(UIMenuItemComponent_Template)
	BEGIN_SERIALIZATION(UIMenuItemComponent_Template)
	END_SERIALIZATION()

}

