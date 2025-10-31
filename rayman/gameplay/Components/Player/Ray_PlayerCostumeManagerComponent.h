#ifndef _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_
#define _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    struct CostumeInfo
    {
        DECLARE_SERIALIZE();

        StringID    m_id;
        u32         m_prize;
    };

    class Ray_PlayerCostumeManagerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerCostumeManagerComponent, ActorComponent,3402508516)
        DECLARE_SERIALIZE();

    public:

        Ray_PlayerCostumeManagerComponent();
        virtual ~Ray_PlayerCostumeManagerComponent();

        virtual bbool               needsUpdate()   const { return bfalse; }
        virtual bbool               needsDraw()     const { return bfalse; }
        virtual bbool               needsDraw2D()   const { return bfalse; }

        virtual void                onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void                onActorClearComponents();

        void                        registerCostume( ObjectRef _ref );
        void                        unregisterCostume( ObjectRef _ref );

        bbool                       isCostumeAvailable( const StringID& _id ) const;
        bbool                       isCostumeShown( const StringID& _id ) const;
        ObjectRef                   getNextCostumeAvailable( u32& _prize ) const;

        const SafeArray <CostumeInfo>& getCostumesInfo() const;


    private :

        const class Ray_PlayerCostumeManagerComponent_Template* getTemplate() const;

        StringID                    getIdNextCostumeAvailable( u32& _index ) const;

        ActorRefList                m_costumes;
    };

    class Ray_PlayerCostumeManagerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerCostumeManagerComponent_Template, TemplateActorComponent,3433372874);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlayerCostumeManagerComponent);

    public:

        Ray_PlayerCostumeManagerComponent_Template();
        ~Ray_PlayerCostumeManagerComponent_Template();

        const SafeArray <CostumeInfo>&      getCostumes() const { return m_costumes; }

    private :

        SafeArray <CostumeInfo>             m_costumes;
    };

    ITF_INLINE const Ray_PlayerCostumeManagerComponent_Template* Ray_PlayerCostumeManagerComponent::getTemplate() const
    {
        return static_cast<const Ray_PlayerCostumeManagerComponent_Template*>(m_template);
    }

    ITF_INLINE const SafeArray <CostumeInfo>& Ray_PlayerCostumeManagerComponent::getCostumesInfo() const
    {
        return getTemplate()->getCostumes();
    }
}

#endif //_ITF_RAY_PLAYERCOSTUMECOMPONENT_H_