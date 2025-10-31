#ifndef _ITF_RAY_HANDSAICOMPONENT_H_
#define _ITF_RAY_HANDSAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{

    class PlayerDetectorComponent;

    class Ray_HandsAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_HandsAIComponent, Ray_AIComponent,184634122)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    private:
      

    public:

        Ray_HandsAIComponent();
        virtual ~Ray_HandsAIComponent();

        virtual bbool       needsUpdate()   const { return btrue; }
        virtual bbool       needsDraw()     const { return bfalse; }
        virtual bbool       needsDraw2D()   const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/) ;
        virtual void        onEvent( Event * _event );

                Actor*      getPlayerDetected() const;

    private:
 
            
    private:

        ITF_INLINE const class Ray_HandsAIComponent_Template * getTemplate() const;

        PlayerDetectorComponent*    m_playerDetectorComponent;


    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_HandsAIComponent_Template : public Ray_AIComponent_Template
    { 
        DECLARE_OBJECT_CHILD_RTTI(Ray_HandsAIComponent_Template, Ray_AIComponent_Template,1264202302);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_HandsAIComponent);

    public:

        Ray_HandsAIComponent_Template();
        ~Ray_HandsAIComponent_Template();

    private:
  

    };


    ITF_INLINE const class Ray_HandsAIComponent_Template * Ray_HandsAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_HandsAIComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_HANDSAICOMPONENT_H_
