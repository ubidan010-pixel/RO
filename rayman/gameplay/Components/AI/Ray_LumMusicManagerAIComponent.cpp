#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LumMusicManagerAIComponent.h"
#endif //_ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#endif

#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AIScoreLumBehavior.h"
#endif //_ITF_RAY_AISCORELUMSBEHAVIOR_H_


namespace ITF
{
    FixedArray<Ray_LumMusicManagerAIComponent::ManagerPerWorld,32> Ray_LumMusicManagerAIComponent::m_musicManagers;


    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_LumMusicManagerAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_LumMusicManagerAIComponent_Template)
    END_SERIALIZATION()

    Ray_LumMusicManagerAIComponent_Template::Ray_LumMusicManagerAIComponent_Template()
    {
    }

    Ray_LumMusicManagerAIComponent_Template::~Ray_LumMusicManagerAIComponent_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_LumMusicManagerAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_LumMusicManagerAIComponent)
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    void  Ray_LumMusicManagerAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload) 
    {
        Super::onActorLoaded(_hotReload);

        Scene *scene = m_actor->getScene();
        ITF_ASSERT_CRASH(scene, "loaded into unknown scene");

        if (scene->getAlwaysActiveActors().find(m_actor)<0)
        {
            scene->setAlwaysActiveActor(m_actor);
        }
        ITF_WARNING(m_actor, findMusicManager(m_actor->getScene()->getWorld()->getRef())==NULL, "there is already a music manager for current world");
        registerMusicManager(m_actor->getScene()->getWorld()->getRef(), m_actor->getRef());

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_LumMusicManagerAIComponent::~Ray_LumMusicManagerAIComponent()
    {        
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_LumMusicManagerAIComponent::registerMusicManager(const ObjectRef &_worldRef, const ActorRef &_actorRef)
    {
        ITF_WARNING(_actorRef.getActor(), m_musicManagers.size()<8, "too many music managers - contact gameplay programmers");
        if (!_worldRef.getObject() || !_actorRef.getObject())
        {
            ITF_ASSERT_MSG(bfalse, "incorrect parameters");
        }
        ManagerPerWorld item;
        item.m_manager = _actorRef;
        item.m_worldRef = _worldRef;
        m_musicManagers.push_back(item);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_LumMusicManagerAIComponent::unregisterMusicManager(const ActorRef &_actorRef)
    {
        for (u32 i=0; i<m_musicManagers.size(); i++)
        {
            const ManagerPerWorld &manager = m_musicManagers[i];
            if (manager.m_manager == _actorRef)
            {
                m_musicManagers.eraseNoOrder(i);
                return;
            }
    }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Actor *Ray_LumMusicManagerAIComponent::findMusicManager(const ObjectRef &_worldRef)
    {
        for (u32 i=0; i<m_musicManagers.size(); i++)
        {
            const ManagerPerWorld &manager = m_musicManagers[i];
            if (manager.m_worldRef == _worldRef)
            {
                Actor *act = manager.m_manager.getActor();
                ITF_WARNING(NULL, act!=NULL, "lum music manager objectref is no longer valid!");
                return act;
            }
        }
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_LumMusicManagerAIComponent::destroyAllExisting()
    {
        for (u32 i=0; i<m_musicManagers.size(); i++)
        {
            const ManagerPerWorld &manager = m_musicManagers[i];
            Actor *act = manager.m_manager.getActor();
            if (act)
            {
                act->requestDestruction();
            }
        }
        m_musicManagers.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_LumMusicManagerAIComponent::onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion)
    {
        Super::onActorRemovedFromScene(_pScene, _forDeletion);
        if (_pScene->getAlwaysActiveActors().find(m_actor)>=0)
        {
            _pScene->setAlwaysActiveActor(m_actor, bfalse);
        }
        unregisterMusicManager(m_actor->getRef());        
    }

    void Ray_LumMusicManagerAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        Ray_EventQueryLumStage query;
        Actor *actor = (Actor*)Ray_AIScoreLumBehavior::getInstance().getObject();
        if (actor)
        {
            actor->onEvent(&query);
            if (query.getComboState() == Ray_AIScoreLumBehavior::ComboState_None)
            {
                if (m_lumKingMusicHandle != ITF::SoundHandle::getInvalidHandle())
                    m_soundComponent->stopSound(m_lumKingMusicHandle);
                m_lumKingMusicHandle = ITF::SoundHandle::getInvalidHandle();
            }
        }
    }

    void Ray_LumMusicManagerAIComponent::startKingLumMusic()
    {
        // Launch its music associated on its specific metronome
        if (APPLOOPFPS==50)
        {
            m_lumKingMusicHandle = m_soundComponent->playSound(ITF_GET_STRINGID_CRC(LumKing_Picked_Jingle_50FPS,2213022608));
        }
        else
        {
            m_lumKingMusicHandle = m_soundComponent->playSound(ITF_GET_STRINGID_CRC(LumKing_Picked_Jingle,3804337898));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

} // namespace ITF

