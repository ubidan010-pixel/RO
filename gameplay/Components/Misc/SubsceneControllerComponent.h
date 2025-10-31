#ifndef _ITF_SUBSCENECONTROLLERCOMPONENT_H_
#define _ITF_SUBSCENECONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    class SubsceneControllerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SubsceneControllerComponent, ActorComponent,1035544071);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        SubsceneControllerComponent();
        virtual ~SubsceneControllerComponent();

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
    };
};

#endif //_ITF_SUBSCENECONTROLLERCOMPONENT_H_