#ifndef _ITF_CAMERADETECTORCOMPONENT_H_
#define _ITF_CAMERADETECTORCOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

namespace ITF
{
    class CameraDetectorComponent : public ShapeDetectorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CameraDetectorComponent, ShapeDetectorComponent,1974096445);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        CameraDetectorComponent();
        virtual ~CameraDetectorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual void        Update( f32 _dt );
    private:
        ITF_INLINE const class CameraDetectorComponent_Template * getTemplate() const;
    };

    //-------------------------------------------------------------------------------------
    class CameraDetectorComponent_Template : public ShapeDetectorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(CameraDetectorComponent_Template,ShapeDetectorComponent_Template,1965871294);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(CameraDetectorComponent);

    public:

        CameraDetectorComponent_Template();
        ~CameraDetectorComponent_Template() {}

        ITF_INLINE bbool    getTriggEachCameraController() const { return m_triggEachCameraController; }

    private:

        bbool       m_triggEachCameraController;                // by default, each camera controller is triggered. It means that every camera controller (cam Shooter/ plateformer) will trig

    };

    ITF_INLINE const CameraDetectorComponent_Template * CameraDetectorComponent::getTemplate() const
    {
        return static_cast<const CameraDetectorComponent_Template*>( m_template );
    }
};

#endif //_ITF_CAMERADETECTORCOMPONENT_H_