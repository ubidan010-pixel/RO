#ifndef _ITF_ICAMERACONTROLLER_H_
#define _ITF_ICAMERACONTROLLER_H_

namespace ITF
{
    class CameraControllerData;
    class CameraModifierComponent;

    class ICameraController
    {
    public:
        ICameraController()           
        {}

        virtual Vec3d       getCameraPosition() const = 0;
        virtual f32         getCameraFocale() const { return MTH_PIBY4; }
        virtual f32         getDepth() const = 0;
        virtual void        teleport(const Vec3d& /*_newPos*/) {}        
        virtual ObjectRef   getActorRef( ) {return ITF_INVALID_OBJREF;}
        virtual void        onCameraEvent( Event* _event ) {}
        virtual void        onManagerUpdate(f32 /*_dt*/) {}
        virtual bbool       isAlwaysActive() { return bfalse; }
        virtual CameraModifierComponent *getCameraModifier() const {return NULL;}                

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const CameraControllerData& /*_data*/ ) {}
#endif //ITF_SUPPORT_DEBUGFEATURE
    };
}

#endif // _ITF_ICAMERACONTROLLER_H_
