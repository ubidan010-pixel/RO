// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_CHARACTERDEBUGGER_PLUGIN_H_
#include "tools/plugins/ActorEditor/Listeners/CharacterDebugger_Plugin.h"
#endif //_ITF_CHARACTERDEBUGGER_PLUGIN_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_CHARACTERDBUGGERCOMPONENT_H_
#include "gameplay/components/Common/CharacterDebuggerComponent.h"
#endif //_ITF_CHARACTERDBUGGERCOMPONENT_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

namespace ITF
{
    CharacterDebugger_Plugin* CharacterDebugger_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* CharacterDebugger_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, CharacterDebugger_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, CharacterDebuggerComponent::GetClassCRCStatic());
        return s_instance;
    }

    void CharacterDebugger_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    void CharacterDebugger_Plugin::onDraw()
    {
        if(CHEATMANAGER->getDebugCharacters())
        {
            for(u32 iW = 0; iW < WORLD_MANAGER->getWorldCount(); ++iW)
            {
                World* world = WORLD_MANAGER->getWorldAt(iW);
                if (!world || !world->isActive())
                    continue;

                const u32 sceneCount = world->getSceneCount();
                for (u32 i = 0; i < sceneCount; i++)
                {
                    Scene* scene = world->getSceneAt(i);
                    const PickableList& actors = scene->getActors();
                    for (u32 j = 0; j < actors.size(); j++)
                    {
                        Actor* actor = (Actor*)actors[j];
                        if(actor->isVisible(CAMERA))
                        {
                            if (CharacterDebuggerComponent* rcDBG = actor->GetComponent<CharacterDebuggerComponent>())
                                rcDBG->editor_draw();
                        }
                    }
                }
            }
        }
    }

} // namespace ITF
