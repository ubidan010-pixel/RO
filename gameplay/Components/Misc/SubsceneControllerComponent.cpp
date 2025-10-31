#include "precompiled_gameplay.h"

#ifndef _ITF_SUBSCENECONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/SubsceneControllerComponent.h"
#endif //_ITF_SUBSCENECONTROLLERCOMPONENT_H_

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

#ifndef _ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_
#include "gameplay/Components/Misc/ObjectControllerComponentUtils.h"
#endif //_ITF_OBJECTCONTROLLERCOMPONENTUTILS_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#endif



namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(SubsceneControllerComponent)

    BEGIN_SERIALIZATION_CHILD(SubsceneControllerComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(SubsceneControllerComponent)
    END_VALIDATE_COMPONENT()


    SubsceneControllerComponent::SubsceneControllerComponent()
     {
    }

    SubsceneControllerComponent::~SubsceneControllerComponent()
    {
    }


#ifdef ITF_SUPPORT_DEBUGFEATURE
    void SubsceneControllerComponent::Draw(  )
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

            DebugDraw::circle(m_actor->getPos(), 0.5f, Color::red());
            DebugDraw::circle(m_actor->getPos(), 0.45f, Color::red());
        }

    }
#endif //ITF_SUPPORT_DEBUGFEATURE

    void SubsceneControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        //m_actor->getScene()->setAlwaysActiveActor(m_actor);

    }

    void SubsceneControllerComponent::updateLinkees(bbool _immediate)
    {
        Matrix44 transform;
        Scene *scene = m_actor->getScene();

        if (scene->getSubSceneActorHolder())
        {

            const PickableList &actors = scene->getActors();
            const PickableList &friezes = scene->getFrises();
            u32 count;

            ObjectControllerComponentUtils::computeControllerMatrixFromControllerActor(transform,m_actor);

            count = actors.size();
            for (u32 i=0; i<count; i++)
            {
                Actor *act = (Actor*) actors[i];
                if (act== m_actor)
                    continue;
                ObjectControllerComponentUtils::applyMatrixToPickable(transform,act);
            }

            count = friezes.size();
            for (u32 i=0; i<count; i++)
            {
                Frise *frieze = (Frise*) friezes[i];
                if (frieze->isDynamic())
                {
                    ObjectControllerComponentUtils::applyMatrixToDynamicFrieze(transform, frieze,_immediate);
                }
            }
        }


    }

    void SubsceneControllerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        updateLinkees(bfalse);
    }


};