#ifndef _ITF_BASECAMERACOMPONENT_H_
#define _ITF_BASECAMERACOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_ICAMERACONTROLLER_H_
#include "engine/display/ICameraController.h"
#endif //_ITF_ICAMERACONTROLLER_H_

namespace ITF
{
    class BaseCameraComponent : public ActorComponent, public ICameraController
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BaseCameraComponent, ActorComponent,1900041299)
        DECLARE_SERIALIZE()

    public:

        BaseCameraComponent();
        virtual ~BaseCameraComponent();


        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        // ICameraController
        virtual f32         getCameraFocale() const { return MTH_PIBY4; }
        virtual void        teleport(const Vec3d& /*_newPos*/) {}    
#ifndef ITF_FINAL
        virtual void        drawDebug( const CameraControllerData& _data ) {}
#endif
        // end ICameraController

        virtual ObjectRef   getActorRef( ) {return m_actor->getRef();}
        void                setAsMainCamera(bbool _value);

    protected:

        const class BaseCameraComponent_Template* getTemplate() const;
    };

    class BaseCameraComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BaseCameraComponent_Template,TemplateActorComponent,1745559923)
        DECLARE_SERIALIZE();

    public:

        BaseCameraComponent_Template();
        ~BaseCameraComponent_Template();

        bbool               getStartAsMainCamera() const { return m_startAsMainCamera; }
        f32                 getRampUpDestinationCoeff() const { return m_rampUpDestinationCoeff; }
        f32                 getRampUpCoeff() const { return m_rampUpCoeff; }

    private:
        bbool               m_startAsMainCamera;
        f32                 m_rampUpDestinationCoeff;
        f32                 m_rampUpCoeff;
    };

    ITF_INLINE const BaseCameraComponent_Template* BaseCameraComponent::getTemplate() const
    {
        return static_cast<const BaseCameraComponent_Template*>(m_template);
    }
}

#endif // _ITF_BASECAMERACOMPONENT_H_

