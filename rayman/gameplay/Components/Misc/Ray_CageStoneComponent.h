#ifndef _ITF_RAYCAGESTONECOMPONENT_H_
#define _ITF_RAYCAGESTONECOMPONENT_H_


#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_


namespace ITF
{
    class Ray_CageStoneComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CageStoneComponent, ActorComponent,190727526)
    public:
        DECLARE_SERIALIZE()

        Ray_CageStoneComponent()
            : m_nbActorToSpawn(0)
            , m_posXStart(0.0f)
            , m_posXEnd(0.0f)
            , m_interval(0.0f)
        {};
        virtual ~Ray_CageStoneComponent();

        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual void    onBecomeActive();
        virtual void    onBecomeInactive();

    private:
        ITF_INLINE const class Ray_CageStoneComponent_Template * getTemplate() const;

        SpawneeGenerator            m_spawneeGen;
        ITF_VECTOR<ActorRef>        m_spawnedActors;
        u32                         m_nbActorToSpawn;
        f32                         m_posXStart;
        f32                         m_posXEnd;
        f32                         m_interval;

    };

    //-------------------------------------------------------------------------------------
    class Ray_CageStoneComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CageStoneComponent_Template, TemplateActorComponent,2185410257);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CageStoneComponent);

    public:

        Ray_CageStoneComponent_Template();
        ~Ray_CageStoneComponent_Template();

        ITF_INLINE const u32 getCageID() const { return m_cageID; }
        ITF_INLINE const f32 getdeltaPosX() const { return m_deltaPosX; }
        ITF_INLINE const Path& getActorToSpawn() const { return m_actorToSpawn; }

    private:
        u32     m_cageID;
        f32     m_deltaPosX;
        Path    m_actorToSpawn;
    };

    ITF_INLINE const class Ray_CageStoneComponent_Template * Ray_CageStoneComponent::getTemplate() const
    {
        return static_cast<const class Ray_CageStoneComponent_Template *>(m_template);
    }

}

#endif // _ITF_RAYCAGESTONECOMPONENT_H_
