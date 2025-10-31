#ifndef _ITF_RAY_PLAYEROFFSCREENICONCOMPONENT_H_
#define _ITF_RAY_PLAYEROFFSCREENICONCOMPONENT_H_

namespace ITF
{
    class Ray_Player;
    class Ray_PlayerOffScreenIconComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerOffScreenIconComponent,ActorComponent,892597551)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT();

    public:

        Ray_PlayerOffScreenIconComponent();
        ~Ray_PlayerOffScreenIconComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/) ;
        virtual void                onResourceReady();
        virtual void                Update( f32 _deltaTime );
        virtual void                onEvent( Event* _event );

    private:

        const class Ray_PlayerOffScreenIconComponent_Template* getTemplate() const;

        Ray_Player* m_player;
        Actor* m_playerActor;
        void initPlayer();

        AnimLightComponent* m_animComponent;
        u32 m_arrowBoneIndex;

        bbool m_showBubble;
        f32 m_timer;
        Vec2d m_lastAnchor;

        u32 m_playerIndex;

        void positionBubble();
    };

    class Ray_PlayerOffScreenIconComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerOffScreenIconComponent_Template,TemplateActorComponent,2699213758);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlayerOffScreenIconComponent);

    public:

        Ray_PlayerOffScreenIconComponent_Template();
        ~Ray_PlayerOffScreenIconComponent_Template() {}

        const StringID&         getArrowBoneName() const { return m_arrowBoneName; }
        const Vec2d&            getScreenBorder() const { return m_screenBorder; }
        f32                     getFadeInDelay() const { return m_fadeInDelay; }
        f32                     getFadeInDuration() const { return m_fadeInDuration; }
        f32                     getFadeOutDelay() const { return m_fadeOutDelay; }
        f32                     getFadeOutDuration() const { return m_fadeOutDuration; }

    private:

        StringID m_arrowBoneName;
        Vec2d m_screenBorder;
        f32 m_fadeInDelay;
        f32 m_fadeInDuration;
        f32 m_fadeOutDelay;
        f32 m_fadeOutDuration;
    };
    
    ITF_INLINE const Ray_PlayerOffScreenIconComponent_Template* Ray_PlayerOffScreenIconComponent::getTemplate() const
    {
        return static_cast<const Ray_PlayerOffScreenIconComponent_Template*>(m_template);
    }
};


#endif //_ITF_RAY_PLAYEROFFSCREENICONCOMPONENT_H_