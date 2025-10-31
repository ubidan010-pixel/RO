#ifndef _ITF_RAY_PLAYERDEADSOUL2DCOMPONENT_H_
#define _ITF_RAY_PLAYERDEADSOUL2DCOMPONENT_H_

namespace ITF
{
    //forward
    class Ray_Player;
    class FXControllerComponent;

    /***********************************************************/
    //Ray_PlayerDeadSoul2DComponent
    //
    //  Manage a graphic omponent on top of a dead PC to deal with GPE's and Frontgrounds hiding this actor.
    //  This component can be a fx component or an anim light component (drawn in 2D by this component)
    //  Deal with the total number of dead players to avoid drawing the component during the fade to blackscreen (when all active players are dead)
    /***********************************************************/
    class Ray_PlayerDeadSoul2DComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerDeadSoul2DComponent,ActorComponent,1239346062)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT();

    public:

        Ray_PlayerDeadSoul2DComponent();
        ~Ray_PlayerDeadSoul2DComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload) ;
        virtual void Update(f32 _deltaTime);
        virtual void onEvent( Event* _event );

    private:
        const class Ray_PlayerDeadSoul2DComponent_Template* getTemplate() const;

        //toggle fx launch
        void toggleFX(const bbool _active);
        //update actor 3D position according to player position
        void updateActor3DPosition( const Vec3d& _pos );
        //update FX color
        void updateFXColor(const Player * const _player) const;

        //particles generator
        FXControllerComponent * m_fxControllerComponent;

        //visibility state
        bbool m_soulBubbleVisible;
        //associated player index
        u32 m_playerIndex;
        //fx handle
        u32 m_fxHandle;
    };


    //--------------------------------------------------
    //  Template

    class Ray_PlayerDeadSoul2DComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerDeadSoul2DComponent_Template,TemplateActorComponent,373239380);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlayerDeadSoul2DComponent);

    public:

        Ray_PlayerDeadSoul2DComponent_Template();
        ~Ray_PlayerDeadSoul2DComponent_Template();

        ITF_INLINE const StringID&  getFxName() const;

    private:

        StringID m_fxName;          //name of the Fx to launch with fx controller (only used for fx controller component)
    };
    
    ITF_INLINE const Ray_PlayerDeadSoul2DComponent_Template* Ray_PlayerDeadSoul2DComponent::getTemplate() const
    {
        return static_cast<const Ray_PlayerDeadSoul2DComponent_Template*>(m_template);
    }

    ITF_INLINE const StringID& Ray_PlayerDeadSoul2DComponent_Template::getFxName() const
    {
        return m_fxName; 
    }
};


#endif //_ITF_RAY_PLAYERDEADSOUL2DCOMPONENT_H_