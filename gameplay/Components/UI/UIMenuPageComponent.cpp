#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENUPAGECOMPONENT_H_
#include "gameplay/Components/UI/UIMenuPageComponent.h"
#endif //_ITF_UIMENUPAGECOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_MENUMANAGER_H_
#include "engine/actors/managers/MenuManager.h"
#endif //_ITF_MENUMANAGER_H_

namespace ITF
{

	IMPLEMENT_OBJECT_RTTI(UIMenuPageComponent)

	BEGIN_SERIALIZATION_CHILD(UIMenuPageComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
		    SERIALIZE_MEMBER("TriggerButton", m_pageTrigger);
		    SERIALIZE_MEMBER("RelativePosFromCamera", m_relativePosFromCam);
        END_CONDITION_BLOCK()

	END_SERIALIZATION()

	UIMenuPageComponent::UIMenuPageComponent()
		: Super()
		, m_pageTrigger(0)
		, m_relativePosFromCam(Vec3d::Zero)
	{
	}

	UIMenuPageComponent::~UIMenuPageComponent()
	{
		// Register page to MenuManager
        if (MENUMANAGER && m_actor)
		    MENUMANAGER->unregisterPage(m_actor->getRef());
	}

	void UIMenuPageComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		Super::onActorLoaded(_hotReload);

		// Register page to MenuManager
		MENUMANAGER->registerPage(m_actor->getRef());
	}

	void UIMenuPageComponent::Update( f32 _dt )
	{
		Super::Update(_dt);

		ITF_WARNING(m_actor, !m_actor->getParentBind(), "This actor shouldn't be bound because it's moving by itself in the world");

		m_actor->setPos(CAMERACONTROLLERMANAGER->getCamPos() + m_relativePosFromCam);
	}

#ifdef ITF_SUPPORT_EDITOR
    void UIMenuPageComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        m_relativePosFromCam = m_actor->getPos() - CAMERACONTROLLERMANAGER->getCamPos();
        //m_relativePosFromCam.m_x = 0.0f;
        //m_relativePosFromCam.m_y = 0.0f;
    }

    void UIMenuPageComponent::onEditorCreated( Actor* _original )
    {
        Super::onEditorCreated(_original);

        m_relativePosFromCam = m_actor->getPos() - CAMERACONTROLLERMANAGER->getCamPos();
        //m_relativePosFromCam.m_x = 0.0f;
        //m_relativePosFromCam.m_y = 0.0f;
    }
#endif // ITF_SUPPORT_EDITOR

	//-------------------------------------------------------------------------------------

	IMPLEMENT_OBJECT_RTTI(UIMenuPageComponent_Template)
	BEGIN_SERIALIZATION(UIMenuPageComponent_Template)
	END_SERIALIZATION()

}

