#ifndef _ITF_RAY_STARGATEMANAGERAICOMPONENT_H_
#define _ITF_RAY_STARGATEMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{

    class LinkComponent;

    class Ray_StargateManagerAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StargateManagerAIComponent, Ray_AIComponent,2484063737)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_StargateManagerAIComponent();
        virtual ~Ray_StargateManagerAIComponent();

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return bfalse; }
        virtual bbool   needsDraw2D() const     { return bfalse; }

        virtual void    onBecomeActive();

        virtual void    Update( f32 _deltaTime );
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);


#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_StargateManagerAIComponent_Template*  getTemplate() const;

        void    changeState( u32 _newState );
        bbool   startSequence( i32 _numSequence );
        void    processTrigger( );

    private:
     
        LinkComponent*     m_linkComponent;
        bbool              m_worldChecked;
    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_StargateManagerAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StargateManagerAIComponent_Template, Ray_AIComponent_Template,816597063);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_StargateManagerAIComponent);

    public:

        Ray_StargateManagerAIComponent_Template();
        ~Ray_StargateManagerAIComponent_Template();

    private:
    
        


    };

    const Ray_StargateManagerAIComponent_Template*  Ray_StargateManagerAIComponent::getTemplate() const {return static_cast<const Ray_StargateManagerAIComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_STARGATEMANAGERAICOMPONENT_H_