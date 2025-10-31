#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#define _ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INGAMECAMERAPARAMETERS_H_
#include "engine/actors/components/Camera/InGameCameraParameters.h"
#endif // _ITF_INGAMECAMERAPARAMETERS_H_

#ifndef _ITF_CURVE_H_
#include "engine/gameplay/Curve.h"
#endif // _ITF_CURVE_H_


namespace ITF
{
    class CameraModifierComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CameraModifierComponent,ActorComponent,1821234798);

    public:
        DECLARE_SERIALIZE()

        CameraModifierComponent();
        ~CameraModifierComponent();

        
        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent(Event* _event);
        void                updateModifiersList();
        void                init();        
        virtual void        Update( f32 _deltaTime );
        void                updateAABB();

        const CamModifier& getCM() const { return m_CM; }
        const CamModifier_Template& getCMTemplate() const;

        const CamModifierUpdate& getUpdateData() const { return m_Update; }

        bbool               isConstraint() const { return m_isConstraint; }

        bbool               isConstraintExtendedLeft() const { return m_CM.m_constraintExtendedLeft.m_offset != 0.f; }
        bbool               isConstraintExtendedRight() const { return m_CM.m_constraintExtendedRight.m_offset != 0.f; }
        bbool               isConstraintExtendedTop() const { return m_CM.m_constraintExtendedTop.m_offset != 0.f; }
        bbool               isConstraintExtendedBottom() const { return m_CM.m_constraintExtendedBottom.m_offset != 0.f; }
        bbool               isConstraintExtended() const { return isConstraintExtendedLeft() || isConstraintExtendedRight() || isConstraintExtendedTop() || isConstraintExtendedBottom(); }

        void                setConstraintExtendedLeft(f32 _delta);
        void                setConstraintExtendedRight(f32 _delta);
        void                setConstraintExtendedTop(f32 _delta);
        void                setConstraintExtendedBottom(f32 _delta);

        void                setConstraintExtendedTopToDest();
        void                setConstraintExtendedBottomToDest();
        void                setConstraintExtendedRightToDest();
        void                setConstraintExtendedLeftToDest();

        AABB                getModifierAABBDest() const;
        AABB                getModifierAABBCur()  const;
        AABB                getModifierAABBMax()  const;

        f32                 getBorderBlending( const Vec2d& _pos, AABB& _borderAABB, bbool _setAABB) const;
        bbool               isBorderBlending() const { return m_isBorderBlending; }

        const Vec2d&        getExtendedOffsetMin() const { return m_extendedOffsetMinCur; }
        const Vec2d&        getExtendedOffsetMax() const { return m_extendedOffsetMaxCur; }

        f32                 getExtendedOffsetLeftDest() const { return m_CM.m_constraintExtendedLeft.m_offset; }
        f32                 getExtendedOffsetRightDest() const { return m_CM.m_constraintExtendedRight.m_offset; }
        f32                 getExtendedOffsetTopDest() const { return m_CM.m_constraintExtendedTop.m_offset; }
        f32                 getExtendedOffsetBottomDest() const { return m_CM.m_constraintExtendedBottom.m_offset; }

        void                resetExtension();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        void                drawDebug();
#endif //ITF_SUPPORT_DEBUGFEATURE

    private:

        enum ExtendedState
        {
            Extended_None = 0,
            Extended_Increase,
            Extended_Decrease,
            Extended_WaitToDecrease,
            Extended_Count
        };

        struct ExtendedParamsDecrease
        {
            ExtendedParamsDecrease()
                : m_time(0.f)
                , m_timeCoeff(0.f)
            {
            }

            f32 m_time;
            f32 m_timeCoeff;

            void reset() { m_time = 0.f; }
        };

        struct ExtendedParams
        {
            ExtendedParams()
                : m_time(0.f)
                , m_timeCoeff(0.f)
                , m_state(Extended_None)
            {
            }

            f32 m_time;
            f32 m_timeCoeff;
            ExtendedState m_state;
            ExtendedParamsDecrease m_decrease;

            void reset() { m_time = 0.f; m_state = Extended_None; m_decrease.reset(); }
        };

        struct BorderBlending
        {
            BorderBlending()
                : m_stop(0.f)
                , m_ratio(0.f)
            { 
            }

            f32 m_stop;
            f32 m_ratio;

            void init() { m_ratio =0.f; }
        };

        const class CameraModifierComponent_Template* getTemplate() const;
        void                updateConstraintExtended( f32 _deltaTime );
        void                setConstraintExtendedTimeCoeff( f32& _timeCoeff, f32 _time ) const;
        void                initConstraintExtended( ConstraintExtended& _constraint );
        void                initConstraintsExtended();
        void                setConstraintExtentedTimeMin( ConstraintExtended& _constraint, ExtendedParams& _params, f32 _delta) const;
        void                setConstraintExtended( ConstraintExtended& _constraint, ExtendedParams& _params, f32& _dest, f32 _delta) const;
        void                updateExtendedParams( ConstraintExtended& _constraintExtended, ExtendedParams& _extendedParams, f32& _cur, f32& _dest, f32 _deltaTime);
        f32                 getExtendedDecreaseOffset( ConstraintExtended& _constraintExtended, ExtendedParamsDecrease& _decrease, f32 _deltaTime);
        void                initBorderBlendings();
        void                limitBlendingZone( bbool _isVertical);
        void                tryToSetAABBToRatio16By9( AABB &_aabb );

        bbool                       m_isConstraint;
        CamModifierUpdate           m_Update;
        CamModifier                 m_CM;
        CamModifierOverride         m_CM_override;

        ExtendedParams              m_extendedLeft;
        ExtendedParams              m_extendedRight;
        ExtendedParams              m_extendedTop;
        ExtendedParams              m_extendedBottom;

        Vec2d                       m_extendedOffsetMinCur;
        Vec2d                       m_extendedOffsetMaxCur;
        Vec2d                       m_extendedOffsetMinDest;
        Vec2d                       m_extendedOffsetMaxDest;

        LogisticParams              m_curveLogistic;

        bbool               m_isBorderBlending;
        BorderBlending      m_borderBlendingLeft;
        BorderBlending      m_borderBlendingRight;
        BorderBlending      m_borderBlendingTop;
        BorderBlending      m_borderBlendingBottom;
    };

    class CameraModifierComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CameraModifierComponent_Template,TemplateActorComponent,3701615444);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(CameraModifierComponent);

    public:

        CameraModifierComponent_Template();
        ~CameraModifierComponent_Template();

        const CamModifier_Template&             getCM() const { return m_CM; }
        const CamModifierOverride_Template&     getCMOverride() const { return m_CM_override; }

    private:

        CamModifier_Template                    m_CM;
        CamModifierOverride_Template            m_CM_override;       
    };

    ITF_INLINE const CameraModifierComponent_Template* CameraModifierComponent::getTemplate() const
    {
        return static_cast<const CameraModifierComponent_Template*>(m_template);
    }

    ITF_INLINE const CamModifier_Template& CameraModifierComponent::getCMTemplate() const { return getTemplate()->getCM(); }
}

#endif // _ITF_CAMERAMODIFIERCOMPONENT_H_