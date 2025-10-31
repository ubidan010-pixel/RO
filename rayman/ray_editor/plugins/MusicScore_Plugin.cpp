#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_MUSICSCORE_PLUGIN_H_
#include "rayman/ray_editor/plugins/MusicScore_Plugin.h"
#endif //_ITF_MUSICSCORE_PLUGIN_H_

#ifndef _ITF_SNAPONMUSICSCORE_PLUGIN_H_
#include "rayman/ray_editor/plugins/SnapOnMusicScore_Plugin.h"
#endif //_ITF_SNAPONMUSICSCORE_PLUGIN_H_

#ifndef _ITF_RAY_MUSICSCOREAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreAIComponent.h"
#endif //_ITF_RAY_MUSICSCOREAICOMPONENT_H_

#ifndef _ITF_RAY_MUSICSCORESNAPCOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreSnapComponent.h"
#endif //_ITF_RAY_MUSICSCORESNAPCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    MusicScore_Plugin* MusicScore_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* MusicScore_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, MusicScore_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, Ray_MusicScoreAIComponent::GetClassCRCStatic());
        return s_instance;
    }

    // ********************************************************************

    void MusicScore_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    // ********************************************************************

    MusicScore_Plugin::MusicScore_Plugin()
    {
        //none
    }

    // ********************************************************************

    MusicScore_Plugin::~MusicScore_Plugin()
    {
        // none
    }

    // ********************************************************************

    void MusicScore_Plugin::onObjectChanged(Actor* _actor, ActorComponent* _component)
    {
        Ray_MusicScoreAIComponent* snapCompo = static_cast<Ray_MusicScoreAIComponent*>(_component);
        snapCompo->init(btrue);

        AIUtils::ActorBindIterator iterator(_actor);
        while ( Actor* child = iterator.getNextChild() )
        {
            Ray_MusicScoreSnapComponent* snapCompo = child->GetComponent<Ray_MusicScoreSnapComponent>();

            if (snapCompo)
            {
                SnapOnMusicScore_Plugin::snap( child, snapCompo, btrue );
            }

        }
    }

    // ********************************************************************

    void MusicScore_Plugin::onShapePosChanged( Actor* _actor, ActorComponent* _component, PickingShape* _shape, const Vec3d& /*_deltaPos*/ )
    {
        Ray_MusicScoreAIComponent* snapCompo = static_cast<Ray_MusicScoreAIComponent*>(_component);
        snapCompo->init(btrue);

        AIUtils::ActorBindIterator iterator(_actor);
        while ( Actor* child = iterator.getNextChild() )
        {
            Ray_MusicScoreSnapComponent* snapCompo = child->GetComponent<Ray_MusicScoreSnapComponent>();

            if (snapCompo)
            {
                SnapOnMusicScore_Plugin::snap( child, snapCompo, btrue );
            }

        }
    }

    // ********************************************************************


} // namespace ITF
