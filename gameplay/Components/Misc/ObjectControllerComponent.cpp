#include "precompiled_gameplay.h"

#ifndef _ITF_OBJECTCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/ObjectControllerComponent.h"
#endif //_ITF_OBJECTCONTROLLERCOMPONENT_H_

#ifndef _ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_
#include "gameplay/Components/Misc/ObjectControllerComponentUtils.h"
#endif //_ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#endif



namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ObjectControllerComponent)

    BEGIN_SERIALIZATION_CHILD(ObjectControllerComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(ObjectControllerComponent)
        for (u32 i=0; i<getChildren().size(); i++)
        {
            const ObjectPath &objpath=getChildren()[i].getPath();
            Pickable * obj = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objpath);
            ITF_WARNING(m_actor, obj!=NULL, "linked object not found");
        }
    END_VALIDATE_COMPONENT()


    ObjectControllerComponent::ObjectControllerComponent()
    {
    }

    ObjectControllerComponent::~ObjectControllerComponent()
    {
        //m_actor->getScene()->setAlwaysActiveActor(m_actor, bfalse);
    }


#ifdef ITF_SUPPORT_DEBUGFEATURE
    void ObjectControllerComponent::Draw(  )
    {
        Float4 upVec, upVec0;
        Float4 rightVec, rightVec0;
        Matrix44 localToWorld;

        Super::Draw();

        if (EDITOR && EDITOR->getState()==Editor::EditorState_EditMode)
        {
            setFloat4(upVec0, 0,1,0,0);
            setFloat4(rightVec0,1,0,0,0);

            m_actor->computePlacementMatrix(localToWorld, btrue);

            M44_matrixFloat4Transform(&rightVec, &localToWorld, &rightVec0);
            M44_matrixFloat4Transform(&upVec, &localToWorld, &upVec0);

            DebugDraw::circle(m_actor->getPos(), 0.5f, Color::green());
            DebugDraw::circle(m_actor->getPos(), 0.45f, Color::green());
            DebugDraw::line3D(m_actor->getPos(), m_actor->getPos()+(Vec3d&)upVec*0.6f, Color::green());
            DebugDraw::line3D(m_actor->getPos(), m_actor->getPos()+(Vec3d&)rightVec*0.6f, Color::red());
        }
    }
#endif //ITF_SUPPORT_DEBUGFEATURE

    void ObjectControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        //m_actor->getScene()->setAlwaysActiveActor(m_actor);

        if (Pickable::HotReloadType_None == _hotReload)
        {
            ObjBinding *binding = m_actor->getBinding();
            for (u32 i=0; i<getChildren().size(); i++)
            {
                const ObjectPath &objpath=getChildren()[i].getPath();
                Pickable * obj = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objpath);
                if (obj)
                {
                    BaseObject::ObjectType type = obj->getObjectType();
                    if (type==BaseObject::eActor)
                    {
                        binding->bindChild(obj->getRef());
                    }
                    else if (type == BaseObject::eFrise)
                    {
                        bindChildFrieze((Frise*)obj, btrue);
                    }
                }
            }
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventOnLink,1558255516),this);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void ObjectControllerComponent::bindChildFrieze(Frise *_frieze, bbool _linkON)
    {
        if (_linkON)
        {
            _frieze->addEventListener(m_actor->getRef());
        }
        else
        {
            _frieze->removeEventListener(m_actor->getRef());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void ObjectControllerComponent::updateLinkees(bbool _immediate)
    {
        Matrix44 transform;

		ObjectControllerComponentUtils::computeControllerMatrixFromControllerActor(transform,m_actor);
        for (u32 i=0; i<getChildren().size(); i++)
        {
            const ObjectPath &objpath=getChildren()[i].getPath();
            Pickable * obj = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objpath);
            if (obj==NULL)
            {
                continue;
            }

            if (obj->getObjectType()==BaseObject::eFrise)
            {
                ObjectControllerComponentUtils::applyMatrixToDynamicFrieze(transform, (Frise*)obj, _immediate);
            }
            else if (obj->getObjectType()==BaseObject::eActor)
            {
                ObjectControllerComponentUtils::applyMatrixToPickable(transform, obj);
            }
            else
            {
                ITF_WARNING(m_actor, bfalse, "linked object not found and neither a frieze or an actor");
            }
        }
    }

    void ObjectControllerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        updateLinkees(bfalse);
    }

    void ObjectControllerComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);
        if ( EventOnLink* linkEvent = _event->DynamicCast<EventOnLink>(ITF_GET_STRINGID_CRC(EventOnLink,1558255516)) )
        {
            BaseObject *object = linkEvent->getChild().getObject();
            if (!object)
                return;

            if (linkEvent->getLinked())
            {
                BaseObject::ObjectType type = object->getObjectType();
                if (type == BaseObject::eFrise)
                {
                    bindChildFrieze((Frise*)object, btrue);
                }
                else if (type == BaseObject::eActor)
                    m_actor->getBinding()->bindChild(linkEvent->getChild());
            }
            else
            {
                BaseObject::ObjectType type = object->getObjectType();
                if (type == BaseObject::eFrise)
                {
                    bindChildFrieze((Frise*)object, bfalse);
                }
                else if (type == BaseObject::eActor)
                    m_actor->getBinding()->unbindChild(linkEvent->getChild());
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(ObjectControllerComponent_Template)
    BEGIN_SERIALIZATION_CHILD(ObjectControllerComponent_Template)
    END_SERIALIZATION()

    ObjectControllerComponent_Template::ObjectControllerComponent_Template()
    {
    }
};