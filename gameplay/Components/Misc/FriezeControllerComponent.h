#ifndef _ITF_FRIEZECONTROLLERCOMPONENT_H_
#define _ITF_FRIEZECONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    class FriezeControllerComponent : public LinkComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FriezeControllerComponent, LinkComponent,455692797);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        FriezeControllerComponent();
        virtual ~FriezeControllerComponent();

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

    //-------------------------------------------------------------------------------------
    class FriezeControllerComponent_Template : public LinkComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(FriezeControllerComponent_Template,LinkComponent_Template,1837670405);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(FriezeControllerComponent);

    public:

        FriezeControllerComponent_Template();
        ~FriezeControllerComponent_Template() {}

    private:

    };
};

#endif //_ITF_FRIEZECONTROLLERCOMPONENT_H_