#ifndef _ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_
#define _ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERACOMPONENT_H_

namespace ITF
{
    class Ray_ShooterCameraModifierComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterCameraModifierComponent, ActorComponent,1893778059)

    public:
        DECLARE_SERIALIZE()

        Ray_ShooterCameraModifierComponent();
        virtual ~Ray_ShooterCameraModifierComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );

        ITF_INLINE const ShooterCameraModifier &   getModifier()    const { return m_camModifier; }
    
    private:
        ITF_INLINE const class Ray_ShooterCameraModifierComponent_Template * getTemplate() const;

        // Editable params
        ShooterCameraModifier               m_camModifier;
        ShooterCameraModifier_Transition    m_transitionIN;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ShooterCameraModifierComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterCameraModifierComponent_Template,TemplateActorComponent,1908345317);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterCameraModifierComponent);

    public:

        Ray_ShooterCameraModifierComponent_Template();
        ~Ray_ShooterCameraModifierComponent_Template() {}

    private:

    };

    ITF_INLINE const class Ray_ShooterCameraModifierComponent_Template * Ray_ShooterCameraModifierComponent::getTemplate() const
    {
        return static_cast<const class Ray_ShooterCameraModifierComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_