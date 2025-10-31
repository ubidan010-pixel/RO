#ifndef _ITF_RAY_PLAYERCOSTUMECOMPONENT_H_
#define _ITF_RAY_PLAYERCOSTUMECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class Ray_PlayerCostumeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerCostumeComponent, ActorComponent,3635797832)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_PlayerCostumeComponent();
        virtual ~Ray_PlayerCostumeComponent();

        virtual bbool               needsUpdate()   const { return btrue; }
        virtual bbool               needsDraw()     const { return bfalse; }
        virtual bbool               needsDraw2D()   const { return bfalse; }

        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();

        virtual void                onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void                onActorClearComponents();
        virtual void                Update( f32 _dt );
        virtual void                onEvent( Event * _event);

        StringID                    getCostumeID() const;

#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private :
        const class Ray_PlayerCostumeComponent_Template* getTemplate() const;

        enum CostumeState
        {
            STATE_LOCKED,
            STATE_SHOWN,
            STATE_AVAILABLE,
            STATE_USED,
        };

        Scene*                      getPlayerScene() const;
        void                        spawnPlayerActors();

        void                        scaleShape();
        void                        triggerActors();
        void                        applyCostume( class Player* _player, Actor* _actor );

        CostumeState                calculateState();
        void                        setState( CostumeState _state );

        Vec2d                       getShapePos() const;

        void                        updateNewIcon();

        class Ray_PlayerCostumeManagerComponent* getManager() const;

        class AnimLightComponent*   m_animComponent;
        Player::RegisteredActorList m_playerActors;
        ActorRefList                m_actorsInside;
        CostumeState                m_state;
        SpawneeGenerator            m_fxGenerator;
        PhysShape*                  m_scaledShape;
        ActorRef                    m_newIconRef;
    };

    class Ray_PlayerCostumeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerCostumeComponent_Template, TemplateActorComponent,823202235);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlayerCostumeComponent);

    public:

        Ray_PlayerCostumeComponent_Template();
        ~Ray_PlayerCostumeComponent_Template();

        const Path&             getNewIconPath() const { return m_newIconPath; }
        const Vec3d&            getNewIconOffset() const { return m_newIconOffset; }
        const StringID&         getPlayerIDInfo() const { return m_playerIDInfo; }
        const StringID&         getLockedAnim() const { return m_animLocked; }
        const StringID&         getShownAnim() const { return m_animShown; }
        const StringID&         getUsedAnim() const { return m_animUsed; }
        const StringID&         getAvailableAnim() const { return m_animAvailable; }
        const Path&             getFX() const { return m_fx; }
        const PhysShape*        getShape() const { return m_shape; }
        const Vec2d&            getShapeOffset() const { return m_shapeOffset; }

    private :

        Path                    m_newIconPath;
        Vec3d                   m_newIconOffset;
        StringID                m_playerIDInfo;
        StringID                m_animLocked;
        StringID                m_animShown;
        StringID                m_animAvailable;
        StringID                m_animUsed;
        Path                    m_fx;
        PhysShape*              m_shape;
        Vec2d                   m_shapeOffset;
    };

    ITF_INLINE const Ray_PlayerCostumeComponent_Template* Ray_PlayerCostumeComponent::getTemplate() const
    {
        return static_cast<const Ray_PlayerCostumeComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_PLAYERCOSTUMECOMPONENT_H_