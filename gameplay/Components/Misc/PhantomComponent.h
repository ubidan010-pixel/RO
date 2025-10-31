#ifndef _ITF_PHANTOMCOMPONENT_H_
#define _ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_SHAPECOMPONENT_H_
#include "gameplay/Components/Misc/ShapeComponent.h"
#endif //_ITF_SHAPECOMPONENT_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{
    class PhantomComponent : public ShapeComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PhantomComponent,ShapeComponent,726931488);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        PhantomComponent();
        virtual ~PhantomComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onEvent( Event* _event );

        virtual void        Update( f32 _dt );

        ITF_INLINE class PhysPhantom* getPhantom() const { return m_phantom; }

        ITF_INLINE bbool    getDisabled() const { return m_disabled; }
        ITF_INLINE void     setDisabled( bbool _value ) { m_disabled = _value; }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug() const;
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:


        ITF_INLINE const class PhantomComponent_Template*  getTemplate() const;

        void                clear();
        void                postLoadProperties();

        void                createPhantom();
        void                registerPhantom();
        void                unregisterPhantom();

        bbool               m_disabled;

        class PhysPhantom*  m_phantom;
        bbool               m_registered;
    };

    //-------------------------------------------------------------------------------------
    class PhantomComponent_Template : public ShapeComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(PhantomComponent_Template,ShapeComponent_Template,1750004224);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PhantomComponent);

    public:

        PhantomComponent_Template();
        ~PhantomComponent_Template() {}

        ECollisionGroup         getCollisionGroup() const { return m_collisionGroup; }

    private:

        ECollisionGroup         m_collisionGroup;
    };

    ITF_INLINE const PhantomComponent_Template*  PhantomComponent::getTemplate() const {return static_cast<const PhantomComponent_Template*>(m_template);}
}

#endif // _ITF_PHANTOMCOMPONENT_H_