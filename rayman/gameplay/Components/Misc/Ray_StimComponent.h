#ifndef _ITF_RAY_STIMCOMPONENT_H_
#define _ITF_RAY_STIMCOMPONENT_H_

#ifndef _ITF_SHAPECOMPONENT_H_
#include "gameplay/Components/Misc/ShapeComponent.h"
#endif //_ITF_SHAPECOMPONENT_H_

namespace ITF
{
    class Ray_StimComponent : public ShapeComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StimComponent, ShapeComponent,4139393211)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_StimComponent();
        virtual ~Ray_StimComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onBecomeActive();

        virtual void        onBecomeInactive();

        virtual void        onDepthChanged( f32 _oldZ, f32 _newZ );

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

        ITF_INLINE bbool    getDisabled() const { return m_disabled; }
        void                setDisabled( bbool _value );

        ITF_INLINE bbool    getUseFixedAngle() const;

        ITF_INLINE f32      getFixedAngle() const { return m_fixedAngle.ToRadians(); }
        ITF_INLINE void     setFixedAngle( f32 _value ) { m_fixedAngle.SetRadians(_value); }

        ITF_INLINE EReceivedHitType      getHitType() const;

        ITF_INLINE Faction  getFaction() const;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug() const;
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:
        void registerObjects();
        void unregisterObjects();
        ITF_INLINE const class Ray_StimComponent_Template * getTemplate() const;

        bbool               m_disabled;
        Angle               m_fixedAngle;           // force the angle/direction of the hit
        Vec2d               m_previousShapePos;
        bbool               m_registered;
        AABB                m_shapeAABB;            // update each frame to manage out of screen optims 
    };

    //-------------------------------------------------------------------------------------
    class Ray_StimComponent_Template : public ShapeComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StimComponent_Template,ShapeComponent_Template,4173636078);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_StimComponent);

    public:

        Ray_StimComponent_Template();
        ~Ray_StimComponent_Template() {}

        ITF_INLINE Faction             getFaction()             const { return m_faction; }
        ITF_INLINE bbool               getUseFixedAngle()       const { return m_useFixedAngle; }
        ITF_INLINE const Angle&        getFixedAngle()          const { return m_fixedAngle; }
        ITF_INLINE const Angle&        getLocalAngleOffset()    const { return m_localAngleOffset; }
        ITF_INLINE EReceivedHitType    getHitType()             const { return m_hitType; }
        ITF_INLINE u32                 getHitLevel()            const { return m_hitLevel; }
        ITF_INLINE bbool               getHitRadial()           const { return m_hitRadial; }
        ITF_INLINE bbool               getUseNormal()           const { return m_useNormal; }
        ITF_INLINE bbool               getRegisterToAIManager() const { return m_registerToAIManager; }
        ITF_INLINE bbool               getUseOutOfScreenOptim() const { return m_useOutOfScreenOptim; }
        ITF_INLINE bbool               getHitEnemiesOnce()      const { return m_hitEnemiesOnce; }

    private:

        Faction             m_faction;
        bbool               m_useFixedAngle;
        Angle               m_fixedAngle;           // force the angle/direction of the hit
        Angle               m_localAngleOffset;     // add an offset to the angle/direction of the hit
        EReceivedHitType    m_hitType;
        u32                 m_hitLevel;
        bbool               m_hitRadial;
        bbool               m_useNormal;
        bbool               m_registerToAIManager;
        bbool               m_useOutOfScreenOptim;
        bbool               m_hitEnemiesOnce;
    };

    ITF_INLINE const class Ray_StimComponent_Template * Ray_StimComponent::getTemplate() const
    {
        return static_cast<const class Ray_StimComponent_Template *>(m_template);
    }

};

#endif //_ITF_RAY_STIMCOMPONENT_H_