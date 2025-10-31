#include "precompiled_gameplay.h"

#ifndef _ITF_FRIEZECONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/FriezeControllerComponent.h"
#endif //_ITF_FRIEZECONTROLLERCOMPONENT_H_

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


#ifndef _ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_
#include "gameplay/Components/Misc/ObjectControllerComponentUtils.h"
#endif //_ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(FriezeControllerComponent)

    BEGIN_SERIALIZATION_CHILD(FriezeControllerComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(FriezeControllerComponent)
        for (u32 i=0; i<getChildren().size(); i++)
        {
            const ObjectPath &objpath=getChildren()[i].getPath();
            Pickable * obj = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objpath);
            ITF_WARNING(m_actor, obj!=NULL && obj->getObjectType()==BaseObject::eFrise, "linked object not found or not a frieze");
        }
    END_VALIDATE_COMPONENT()


    FriezeControllerComponent::FriezeControllerComponent()
    {
    }

    FriezeControllerComponent::~FriezeControllerComponent()
    {
        //m_actor->getScene()->setAlwaysActiveActor(m_actor, bfalse);
    }


#ifdef ITF_SUPPORT_DEBUGFEATURE
    void FriezeControllerComponent::Draw(  )
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

    void FriezeControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        //m_actor->getScene()->setAlwaysActiveActor(m_actor);

    }

    void FriezeControllerComponent::updateLinkees(bbool _immediate)
    {
        Matrix44 rotationMatrix;
        Matrix44 initPosToZeroMatrix, invInitTransfo;
        Matrix44 transform;


        m_actor->computePlacementMatrix(rotationMatrix, btrue);

		M44_setMatrixRotationZ(&initPosToZeroMatrix, m_actor->getWorldInitialRot());
        const Vec3d &pos = m_actor->getWorldInitialPos();
		initPosToZeroMatrix.setLineAt(3, pos.m_x, pos.m_y, pos.m_z, 1.f);

		M44_matrixInverse((f32*)invInitTransfo.fa16, (f32*)initPosToZeroMatrix.fa16);

        M44_mul4x4Out(&transform, &invInitTransfo, &rotationMatrix);

        for (u32 i=0; i<getChildren().size(); i++)
        {
            const ObjectPath &objpath=getChildren()[i].getPath();
            Pickable * obj = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objpath);
            if (obj!=NULL && obj->getObjectType()==BaseObject::eFrise)
            {
                Frise *frieze = (Frise*)obj;
                if (frieze->isDynamic())
                {
                    frieze->setDynamicTransform((GMatrix44&)transform);
                    if (_immediate)
                    {
                        frieze->applyDynamicTransform();
                    }
                }
                else
                {
                    ITF_WARNING(m_actor, frieze->isDynamic(),"destination frieze is not dynamic");
                }
            }
            else
            {
                ITF_WARNING(m_actor, obj!=NULL && obj->getObjectType()==BaseObject::eFrise, "linked object not found or not a frieze");
            }
        }
    }

    void FriezeControllerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        updateLinkees(bfalse);
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(FriezeControllerComponent_Template)
    BEGIN_SERIALIZATION_CHILD(FriezeControllerComponent_Template)
    END_SERIALIZATION()

    FriezeControllerComponent_Template::FriezeControllerComponent_Template()
    {
    }
};