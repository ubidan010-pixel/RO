#ifndef _ITF_RAY_DARKARENADOORCOMPONENT_H_
#define _ITF_RAY_DARKARENADOORCOMPONENT_H_

#ifndef _ITF_ARENADOORCOMPONENT_H_
#include "gameplay/Components/Misc/ArenaDoorComponent.h"
#endif //_ITF_ARENADOORCOMPONENT_H_

namespace ITF
{
    class WindComponent;

    class Ray_DarkArenaDoorComponent : public ArenaDoorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DarkArenaDoorComponent, ArenaDoorComponent,3781071830)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_DarkArenaDoorComponent();
        virtual ~Ray_DarkArenaDoorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event );

    protected:

        virtual void        openDoor();

        WindComponent*      m_windComponent;
   
    private:
        ITF_INLINE const class Ray_DarkArenaDoorComponent_Template*  getTemplate() const;

        bbool               queryIsDead( Actor* _child );
        
        u32                 m_state;


    };

    //*****************************************************************************
    // Template
    //*****************************************************************************


    class Ray_DarkArenaDoorComponent_Template : public ArenaDoorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DarkArenaDoorComponent_Template,ArenaDoorComponent_Template,457719623);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DarkArenaDoorComponent);

    public:

        Ray_DarkArenaDoorComponent_Template();
        ~Ray_DarkArenaDoorComponent_Template() {}

        const StringID&     getHitAnim() const { return m_hitAnim; }
        const StringID&     getIdleAnim() const { return m_idleAnim; }

    protected:

        StringID            m_idleAnim;
        StringID            m_hitAnim;

    };

    const Ray_DarkArenaDoorComponent_Template*  Ray_DarkArenaDoorComponent::getTemplate() const {return static_cast<const Ray_DarkArenaDoorComponent_Template*>(m_template);}
};

#endif //_ITF_RAY_DARKARENADOORCOMPONENT_H_