#ifndef _ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_
#define _ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{

    class Ray_LumMusicManagerAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LumMusicManagerAIComponent, Ray_AIComponent,2185561851)
        DECLARE_SERIALIZE()

    public:
        Ray_LumMusicManagerAIComponent() : m_lumKingMusicHandle (ITF::SoundHandle::getInvalidHandle())
        {
        }

        ~Ray_LumMusicManagerAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActorRemovedFromScene(Scene* /*_pScene*/, bbool _forDeletion);
        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual void        Update( f32 _dt );
        //music manager
        static Actor *findMusicManager(const ObjectRef &_worldRef);
        static void registerMusicManager(const ObjectRef &_worldRef, const ActorRef &_actorRef);
        static void unregisterMusicManager(const ActorRef &_actorRef);
        static void destroyAllExisting();

        void startKingLumMusic();

    protected:
        struct ManagerPerWorld
        {
            ObjectRef m_worldRef;
            ActorRef m_manager;
        };

        ITF::SoundHandle m_lumKingMusicHandle ;
        static FixedArray<ManagerPerWorld,32> m_musicManagers;

        ITF_INLINE const class Ray_LumMusicManagerAIComponent_Template * getTemplate() const;

    };

    class Ray_LumMusicManagerAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LumMusicManagerAIComponent_Template,Ray_AIComponent_Template,2203457311);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LumMusicManagerAIComponent);

    public:

        Ray_LumMusicManagerAIComponent_Template();
        ~Ray_LumMusicManagerAIComponent_Template();
    };

    ITF_INLINE const Ray_LumMusicManagerAIComponent_Template * Ray_LumMusicManagerAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_LumMusicManagerAIComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_
