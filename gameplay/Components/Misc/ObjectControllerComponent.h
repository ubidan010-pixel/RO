#ifndef _ITF_OBJECTCONTROLLERCOMPONENT_H_
#define _ITF_OBJECTCONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    class Frise;

    class ObjectControllerComponent : public LinkComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ObjectControllerComponent, LinkComponent,1155118512);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        ObjectControllerComponent();
        virtual ~ObjectControllerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual bbool       needsDraw() const { return btrue; }
        virtual void        Draw();
#else
        virtual bbool       needsDraw() const { return bfalse; }
#endif //ITF_SUPPORT_DEBUGFEATURE

        virtual bbool       needsDraw2D() const { return bfalse; }

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue) {updateLinkees(btrue);}
#endif // ITF_SUPPORT_EDITOR

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

    private:
        void                updateLinkees(bbool _immediate);
        void                onEvent( Event * _event);
        void                bindChildFrieze(Frise *_frieze, bbool _linkON);
    };

    //-------------------------------------------------------------------------------------
    class ObjectControllerComponent_Template : public LinkComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(ObjectControllerComponent_Template,LinkComponent_Template,24176894);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ObjectControllerComponent);

    public:

        ObjectControllerComponent_Template();
        ~ObjectControllerComponent_Template() {}

    private:

    };
};

#endif //_ITF_OBJECTCONTROLLERCOMPONENT_H_