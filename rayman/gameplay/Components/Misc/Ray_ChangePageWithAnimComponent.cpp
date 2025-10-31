#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHANGEPAGEWITHANIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageWithAnimComponent.h"
#endif //_ITF_RAY_CHANGEPAGEWITHANIMCOMPONENT_H_

#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageComponent.h"
#endif //_ITF_RAY_CHANGEPAGECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ChangePageWithAnimComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_ChangePageWithAnimComponent)
	
    END_SERIALIZATION()

	//--------------------------------------------------------------------------------------------------------
    Ray_ChangePageWithAnimComponent::Ray_ChangePageWithAnimComponent()
    : m_linkComponent(NULL)
    , m_fxControllerComponent( NULL )
    , m_animLightComponent(NULL)
	{
    }

	Ray_ChangePageWithAnimComponent::~Ray_ChangePageWithAnimComponent()
	{
	}

	void Ray_ChangePageWithAnimComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();
        		
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventChangePage,1004490573), this);
	}

	//--------------------------------------------------------------------------------------------------------
	void Ray_ChangePageWithAnimComponent::onEvent( Event * _event)
	{
        Super::onEvent(_event);

		// Event from trigger
		if ( Ray_EventChangePage * eventChangePage = _event->DynamicCast<Ray_EventChangePage>(ITF_GET_STRINGID_CRC(Ray_EventChangePage,1004490573)) )
		{
            processChangePageEvent( eventChangePage );
		}
	}

    //--------------------------------------------------------------------------------------------------------
    Ray_ChangePageComponent * Ray_ChangePageWithAnimComponent::getDestinationPageComponent() const
    {
        Ray_ChangePageComponent * destPageComp = NULL;

        if ( m_linkComponent != NULL )
        {
            if ( ObjectRefList* children = LINKMANAGER->getChildren( m_actor->getRef() ) )
            {
                for (u32 i = 0; i < children->size(); i++)
                {
                    BaseObject * baseObj = ((*children)[i]).getObject();
                    
                    if ( baseObj != NULL )
                    {
                        if ( Actor* child = baseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                        {
                            destPageComp = child->GetComponent<Ray_ChangePageComponent>();

                            if ( destPageComp != NULL )
                            {
                                break;
                            }
                        }
                    }                    
                }
            }
        }
        return destPageComp;
    }

	//--------------------------------------------------------------------------------------------------------
	void Ray_ChangePageWithAnimComponent::processChangePageEvent( Ray_EventChangePage * _evt )
	{
        Actor* activator = AIUtils::getActor(_evt->getActivator());

        if ( !activator )
        {
            return;
        }

        Ray_ChangePageComponent* destPageComponent = getDestinationPageComponent();

        if ( !destPageComponent )
        {
            return;
        }

        Vec3d targetPos = m_actor->getPos();

        if ( m_animLightComponent && getTemplate()->getAspirePoint().isValid() )
        {
            AnimPolyline* pPolyline = NULL;
            u32 pointIndex;
            const Vec2d* pointsBuffer = m_animLightComponent->getCurrentFirstPolylinePoint(getTemplate()->getAspirePoint(), &pPolyline, &pointIndex);
            if (pointsBuffer)
            {
                targetPos = pointsBuffer[pointIndex].to3d(m_actor->getPos().m_z);
            }
        }

        ActorRef destPageRef = destPageComponent->GetActor()->getRef();
        ActorRef checkpointRef = getActivateCheckpoint();

        RAY_GAMEMANAGER->triggerChangePageWithAnim( m_actor->getRef(),
                                                    destPageRef,
                                                    checkpointRef,
                                                    getTemplate()->getAnim(),
                                                    targetPos);

        if ( m_fxControllerComponent != NULL )
        {
            u32 fxHandle = m_fxControllerComponent->playFX( ITF_GET_STRINGID_CRC(Enter,2000132787) );
            m_fxControllerComponent->setFXPos( fxHandle, m_actor->getPos() );
        }

        if ( m_animLightComponent && getTemplate()->getAspireAnim().isValid() )
        {
            m_animLightComponent->setAnim(getTemplate()->getAspireAnim());
        }
	}

    //--------------------------------------------------------------------------------------------------------
    ActorRef Ray_ChangePageWithAnimComponent::getActivateCheckpoint() const
    {
        if ( m_linkComponent != NULL )
        {
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            u32 numChildren = children.size();

            for ( u32 i = 0; i < numChildren; i++ )
            {
                const ChildEntry& child = children[i];

                if ( child.hasTag(ITF_GET_STRINGID_CRC(activateCheckpoint,1465678949)) )
                {
                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if ( path.getIsAbsolute() )
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);
                    }

                    if ( target )
                    {
                        return target->getRef();
                    }

                    break;
                }
            }
        }

		return ObjectRef::InvalidRef;
    }

	//--------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ChangePageWithAnimComponent_Template)
	BEGIN_SERIALIZATION(Ray_ChangePageWithAnimComponent_Template)

        SERIALIZE_MEMBER( "animPlayer",          m_anim );
        SERIALIZE_MEMBER( "aspireAnim",          m_aspireAnim );
        SERIALIZE_MEMBER( "aspirePoint",         m_aspirePoint );

    END_SERIALIZATION()

    Ray_ChangePageWithAnimComponent_Template::Ray_ChangePageWithAnimComponent_Template()
    {
    }	

} // namespace ITF
