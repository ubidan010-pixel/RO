#ifndef _ITF_TWEENINSTRUCTIONS_H_
#define _ITF_TWEENINSTRUCTIONS_H_

#ifndef _ITF_TRANSFORM2D3D_H_
#include "engine/gameplay/Transform.h"
#endif //_ITF_TRANSFORM2D3D_H_

#ifndef ITF_FINAL

    #define MY_DECLARE_VALIDATE() virtual void validate( BaseObject* _baseObject, bbool& _isObjectValidated ) const;

    #define MY_BEGIN_VALIDATE(_classname_) void _classname_::validate( BaseObject* _baseObject, bbool& _isObjectValidated ) const {
    #define MY_BEGIN_VALIDATE_CHILD(_classname_) void _classname_::validate( BaseObject* _baseObject, bbool& _isObjectValidated ) const { Super::validate(_baseObject, _isObjectValidated);
    #define MY_END_VALIDATE() }

    #define MY_VALIDATE_PARAM(_parameter_, _assertion_, ...)\
        do { \
            ITF_DISABLE_WARNING_PUSH\
            ITF_DISABLE_WARNING_ID(4127)\
            if (!(_assertion_))\
            {\
                _isObjectValidated = bfalse;\
                ITF_ASSERT_MSG_FILTER(_assertion_, ITF::AssertManager::m_FlagWarningAndError, _parameter_ ":" __VA_ARGS__ );\
                WarningManager::addWarning(WarningCategory_LD,_baseObject, _parameter_, __VA_ARGS__ );\
            }\
            ITF_DISABLE_WARNING_POP\
        } while (WarningManager::returnFalse())

    #define MY_VALIDATE_OBJECT(_parameter_, _object_) _object_->validate(m_actor, _isObjectValidated)

#else
    #define MY_DECLARE_VALIDATE()
    #define MY_BEGIN_VALIDATE(_classname_)
    #define MY_BEGIN_VALIDATE_CHILD(_classname_)
    #define MY_END_VALIDATE()
    #define MY_VALIDATE_PARAM(_parameter_, _assertion_, ...)
    #define MY_VALIDATE_OBJECT(_parameter_, _object_)
#endif



#define DECLARE_SERIALIZE_INSTANCE() DECLARE_SERIALIZE()

#define BEGIN_SERIALIZATION_INSTANCE(object) BEGIN_SERIALIZATION_CHILD(object)


#define DECLARE_SERIALIZE_DUMMY(object) \
    public: \
    void Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(#object, this) ) \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<object>();  \
        } \
    }


namespace ITF
{
    /*
        initTransform = actor space, relative to init pos/angle
        setTransform = InstructionSet space
    */

    class BaseObject;
    class TweenComponent;
    class TweenInstruction_Template;
    class LinkComponent;

    class TweenCoordinates
    {
    public:
        TweenCoordinates()
            : m_pos(Vec3d::Zero)
            , m_trajectoryAngle(0.f)
            , m_actorAngle(0.f)
            , m_scaleMultiplier(Vec2d::One)
            , m_tangentAngle(0.f)
        {
        }

        // these transform the trajectory
        Vec3d   m_pos;
        f32     m_trajectoryAngle;
        Transform3d getTransform() const { return Transform3d(m_pos, m_trajectoryAngle); }

        f32     m_actorAngle;       // same as tangentAngle in auto-rotate
        Vec2d   m_scaleMultiplier;
        f32     m_tangentAngle;     // trajectory direction on exit
    };


    /**************** TweenInstruction ****************/

    class TweenInstruction : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(TweenInstruction, IRTTIObject,715430016);
        DECLARE_SERIALIZE_VIRTUAL()
        MY_DECLARE_VALIDATE()

    public:
        TweenInstruction()
            : m_tweenComponent(NULL)
            , m_actor(NULL)
            , m_template(NULL)
        {
        }
        virtual ~TweenInstruction() {}

                bbool   isSerializable() const { return m_name.isValid(); }
        const StringID  getName() const { return m_name; }

                void    init( TweenComponent* _owner, const TweenInstruction_Template* _template );
        virtual void    onActorLoaded( Pickable::HotReloadType _hotReload ) {}
        virtual void    onBecomeActive( const TweenCoordinates& _coordinates ) {}
        virtual void    onBecomeInactive() {}
        virtual void    onSetFinished() {}

        // template indirections (overridable)
        virtual f32     getDuration() const;
        virtual void    execute( const f32 _t, TweenCoordinates& _coordinates ) const;
        virtual void    executeEnd( TweenCoordinates& _coordinates ) const;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void    drawDebug( const Transform3d& _initTransform,
                                   const TweenCoordinates& _coordinates,
                                   const Color& _color ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

    protected:

        TweenComponent*                     m_tweenComponent;
        Actor*                              m_actor;
        const TweenInstruction_Template*    m_template;
        StringID    m_name;
    };

    #define DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenInstructionClass) \
    public: \
        virtual TweenInstruction* createTweenInstruction() const { return newAlloc(mId_System, TweenInstructionClass()); } \
        virtual StringID::StringIdValueType getInstructionClassCRC() const { return TweenInstructionClass::GetClassCRCStatic(); } \

    class TweenInstruction_Template : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(TweenInstruction_Template, IRTTIObject,4048037358);
        DECLARE_SERIALIZE_VIRTUAL()
        MY_DECLARE_VALIDATE()

    public:

        virtual TweenInstruction* createTweenInstruction() const = 0;
        virtual StringID::StringIdValueType getInstructionClassCRC() const = 0;

        TweenInstruction_Template()
            : m_duration(1.f)
        {
        }
        ~TweenInstruction_Template() {}

        virtual void    onTemplateLoaded() {}

        f32             getDuration() const { return m_duration; }
        const StringID& getName() const { return m_name; }

        virtual void    execute( const f32 _t, TweenCoordinates& _coordinates ) const {}
        virtual void    executeEnd( TweenCoordinates& _coordinates ) const {}
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void    drawDebug( const Transform3d& _initTransform, const TweenCoordinates& _coordinates, const Color& _color ) const { drawDebug(_initTransform.transform(_coordinates.getTransform()), _color); }
        virtual void    drawDebug( const Transform3d& _worldTransform, const Color& _color ) const {}
#endif // ITF_SUPPORT_DEBUGFEATURE

    protected:

        StringID    m_name;
        f32         m_duration;
    };

    ITF_INLINE f32  TweenInstruction::getDuration() const { return m_template->getDuration(); }
    ITF_INLINE void TweenInstruction::execute( const f32 _t, TweenCoordinates& _coordinates ) const { m_template->execute(_t, _coordinates); }
    ITF_INLINE void TweenInstruction::executeEnd( TweenCoordinates& _coordinates ) const { m_template->executeEnd(_coordinates); }
#ifdef ITF_SUPPORT_DEBUGFEATURE
    ITF_INLINE void TweenInstruction::drawDebug( const Transform3d& _initTransform, const TweenCoordinates& _coordinates, const Color& _color ) const { m_template->drawDebug(_initTransform, _coordinates, _color); }
#endif //ITF_SUPPORT_DEBUGFEATURE



    class TimeSmoother
    {
    public:
        TimeSmoother()
            : m_dist(-1.f)
            , m_duration(-1.f)
            , ts(0.f)
            , te(0.f)
            , vs(0.f)
            , ve(0.f)
            , vm(-1.f)
            , ds(-1.f)
            , dm(-1.f)
            , as(-1.f)
            , ae(-1.f)
        {
        }

        void        init( const f32 _duration, const f32 _dist );
        f32         getSmoothedT( f32 _t ) const;

        // serialized
        f32         ts; //m_startDuration;
        f32         te; //m_endDuration;
        f32         vs; //m_startSpeed;
        f32         ve; //m_endSpeed;

    private:
        // passed on init
        f32         m_duration;
        f32         m_dist;

        // computed on init
        f32         vm; // mid speed (constant portion)
        f32         ds; // start distance
        f32         dm; // mid distance
        f32         as; // start acceleration
        f32         ae; // end acceleration
    };

    /**************** TweenTranslation ****************/

    class TweenTranslation : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(TweenTranslation, TweenInstruction,2067506116);

    public:
        TweenTranslation() {}
        virtual ~TweenTranslation() {}

    private:
        ITF_INLINE const class TweenTranslation_Template* getTemplate() const;
    };

    class TweenTranslation_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(TweenTranslation_Template, TweenInstruction_Template,2291864289);
        DECLARE_SERIALIZE()
        MY_DECLARE_VALIDATE()

    public:

        TweenTranslation_Template()
            : m_dist(-1.f)
            , m_movement(Vec3d::Zero)
            , m_trajectoryRotation(0.f)
            , m_actorRotation(0.f)
            , m_endTangentAngle(0.f)

            , m_rotateTrajectory(bfalse)
            , m_autoRotate(bfalse)
            , m_scaleMultiplier(Vec2d::Zero)
        {
        }
        ~TweenTranslation_Template() {}

        virtual void    onTemplateLoaded();

        virtual void    execute( const f32 _t, TweenCoordinates& _coordinates ) const;
        virtual void    executeEnd( TweenCoordinates& _coordinates ) const;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void    drawDebug( const Transform3d& _worldTransform, const Color& _color ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

        const Vec3d&    getMovement() const { return m_movement; }
        const TimeSmoother& getTimeSmoother() const { return m_timeSmoother; }

        // internal stuff
        virtual void    execute( const f32 _t, TweenCoordinates& _coordinates, const Vec3d& _posInternal, const f32 _angleInternal ) const;
        virtual void    executeEnd( TweenCoordinates& _coordinates, const Vec3d& _posInternal, const f32 _angleInternal, const f32 _actorRotation, const f32 _tangentAngle ) const;

    protected:

        virtual Vec3d   getPosInternal( f32 _t ) const = 0;
        virtual f32     getAngleInternal( f32 _t ) const = 0;

        // computed onTemplateLoaded by each overload
        f32         m_dist;             // distance to travel (can be linear or angular, used to compute the smoothedT)
        Vec3d       m_movement;         // linear movement  / end position
        f32         m_trajectoryRotation; // end trajectory angle
        f32         m_actorRotation;    // end actor angle (the amount of rotation added to the actor in autoRotate mode)
        f32         m_endTangentAngle;

        ITF_INLINE f32 getAngle() const { return m_angle.ToRadians(); }

    //private:

        TimeSmoother m_timeSmoother;

        bbool       m_rotateTrajectory;
        bbool       m_autoRotate;  // auto-rotate along trajectory normal
        AngleAmount m_angle;        // forced rotation lerped on duration
        AngleAmount m_angleOffset;
        Vec2d       m_scaleMultiplier;
    };

    ITF_INLINE const TweenTranslation_Template* TweenTranslation::getTemplate() const
    {
        return static_cast<const TweenTranslation_Template*>(m_template);
    }



    /**************** TweenTranslationLine ****************/

    class TweenTranslationLine : public TweenTranslation
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenTranslationLine, TweenTranslation,327013100);
        DECLARE_SERIALIZE_DUMMY(TweenTranslationLine)

    public:
        TweenTranslationLine() {}
        virtual ~TweenTranslationLine() {}

    private:
        ITF_INLINE const class TweenTranslationLine_Template* getTemplate() const;
    };

    class TweenTranslationLine_Template : public TweenTranslation_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenTranslationLine_Template, TweenTranslation_Template,3862585322);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenTranslationLine)
        DECLARE_SERIALIZE()

    public:

        TweenTranslationLine_Template()
        {
        }
        ~TweenTranslationLine_Template() {}

        void                onTemplateLoaded();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const Transform3d& _worldTransform, const Color& _color ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

    protected:

        virtual Vec3d       getPosInternal( f32 _t ) const { return m_movement * _t; }
        virtual f32         getAngleInternal( f32 _t ) const { return m_actorRotation; }

    };

    ITF_INLINE const TweenTranslationLine_Template* TweenTranslationLine::getTemplate() const
    {
        return static_cast<const TweenTranslationLine_Template*>(m_template);
    }



    /**************** TweenTranslationSine ****************/

    class TweenTranslationSine : public TweenTranslation
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenTranslationSine, TweenTranslation,1348589277);
        DECLARE_SERIALIZE_DUMMY(TweenTranslationSine)

    public:
        TweenTranslationSine() {}
        virtual ~TweenTranslationSine() {}

    private:
        const class TweenTranslationSine_Template* getTemplate() const;
    };

    class TweenTranslationSine_Template : public TweenTranslation_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenTranslationSine_Template, TweenTranslation_Template,2798634649);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenTranslationSine)
        DECLARE_SERIALIZE()
        MY_DECLARE_VALIDATE()

    public:

        TweenTranslationSine_Template()
            : m_amplitude(1.f)
            , m_cycleCount(1.f)
            , m_cycleOffset(0.f)
            , m_startAngle(0.f)
        {
        }
        ~TweenTranslationSine_Template() {}

        virtual void        onTemplateLoaded();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const Transform3d& _worldTransform, const Color& _color ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

    protected:

        virtual Vec3d       getPosInternal( f32 _t ) const;
        virtual f32         getAngleInternal( f32 _t ) const;

    private:

        Vec2d               getSinePos( f32 _t ) const;
        f32                 getSineAngle( f32 _t ) const;

        f32                 m_amplitude;
        f32                 m_cycleCount;
        f32                 m_cycleOffset;

        Vec2d               m_startPos;     // due to cycleOffset, we may not start at local (0,0) --> compensate
        f32                 m_startAngle;   // sine is a bitch for angles! whatever the angle we start at, compensate (only add to whatever we had)
    };

    ITF_INLINE const TweenTranslationSine_Template* TweenTranslationSine::getTemplate() const
    {
        return static_cast<const TweenTranslationSine_Template*>(m_template);
    }



    /**************** TweenTranslationCircle ****************/

    class TweenTranslationCircle : public TweenTranslation
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenTranslationCircle, TweenTranslation,2879495710);
        DECLARE_SERIALIZE_DUMMY(TweenTranslationCircle)

    public:
        TweenTranslationCircle() {}
        virtual ~TweenTranslationCircle() {}

    private:
        ITF_INLINE const class TweenTranslationCircle_Template* getTemplate() const;
    };

    class TweenTranslationCircle_Template : public TweenTranslation_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenTranslationCircle_Template, TweenTranslation_Template,410144229);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenTranslationCircle)
        DECLARE_SERIALIZE()
        MY_DECLARE_VALIDATE()

    public:

        TweenTranslationCircle_Template()
            : m_cycleCount(1.f)
            , m_radius(-1.f)
            , m_pivotAngle(0.f)
        {
        }
        ~TweenTranslationCircle_Template() {}

        virtual void        onTemplateLoaded();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const Transform3d& _worldTransform, const Color& _color ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

    protected:

        virtual Vec3d       getPosInternal( f32 _t ) const;
        virtual f32         getAngleInternal( f32 _t ) const;

    private:

        Vec3d               m_pivot;        // pivot is 3D: we do the circle on xy, and simply interpolate on z (stretched, not tilted)
        f32                 m_cycleCount;   // positive: counter-clockwise, negative: clockwise

        f32                 m_radius;
        f32                 m_pivotAngle;
    };

    ITF_INLINE const TweenTranslationCircle_Template* TweenTranslationCircle::getTemplate() const
    {
        return static_cast<const TweenTranslationCircle_Template*>(m_template);
    }



    /**************** TweenBallistic ****************/

    class TweenBallistic : public TweenTranslation
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenBallistic, TweenTranslation,1641159219);
        DECLARE_SERIALIZE_INSTANCE()
        friend class Tweening_Plugin;

    public:
        TweenBallistic()
            : m_movement(UNINIT_VEC3D)
            , m_startTangent(UNINIT_VEC3D)
            , m_linkComponent(NULL)
            , m_duration(1.f)
            , m_startAngle(0.f)
            , m_trajectoryRotation(0.f)
            , m_endTangentAngle(0.f)
            , m_actorRotation(0.f)
        {}
        virtual ~TweenBallistic() {}

        virtual f32         getDuration() const { return m_duration; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onBecomeActive( const TweenCoordinates& _coordinates );
        virtual void        onBecomeInactive();

        virtual void        execute( const f32 _t, TweenCoordinates& _coordinates ) const;
        virtual void        executeEnd( TweenCoordinates& _coordinates ) const;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const Transform3d& _initTransform,
                                       const TweenCoordinates& _coordinates,
                                       const Color& _color ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:
        const class TweenBallistic_Template* getTemplate() const;

        // serialized
        Vec3d               m_movement;
        Vec3d               m_startTangent;

        // onBecomeActive
        Bezier4<Vec3d>      m_bezier;
        f32                 m_startAngle;

        Bezier4<Vec3d>      getBezier( const TweenCoordinates& _coordinates ) const;
        void                initBezier( const TweenCoordinates& _coordinates );
        f32                 getAngleInternal( f32 _t ) const;
        Vec3d               getPosInternal( f32 _t ) const;

        // everything that's usually done in template
        f32                 m_duration;
        TimeSmoother        m_timeSmoother;
        f32                 m_trajectoryRotation;
        f32                 m_endTangentAngle;
        f32                 m_actorRotation;


        LinkComponent*      m_linkComponent;
        void                disableCollisions( const bbool _disable );
    };

    class TweenBallistic_Template : public TweenTranslation_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenBallistic_Template, TweenTranslation_Template,1898920804);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenBallistic)
        DECLARE_SERIALIZE()
        MY_DECLARE_VALIDATE()

    public:

        enum TangentMode
        {
            TangentMode_Auto,
            TangentMode_Custom,
            TangentMode_Jump,
            ENUM_FORCE_SIZE_32(TangentMode)
        };

        #define SERIALIZE_TANGENTMODE(name,val) \
        SERIALIZE_ENUM_BEGIN(name,val); \
            SERIALIZE_ENUM_VAR(TangentMode_Auto); \
            SERIALIZE_ENUM_VAR(TangentMode_Custom); \
            SERIALIZE_ENUM_VAR(TangentMode_Jump); \
        SERIALIZE_ENUM_END();


        TweenBallistic_Template()
            : m_tangentMode(TangentMode_Auto)
            , m_startTangent(Vec3d::Right)
            , m_disableCollisions(btrue)
            , m_speed(0.f)
        {
        }
        ~TweenBallistic_Template() {}

        ITF_INLINE TweenBallistic_Template::TangentMode getTangentMode() const { return m_tangentMode; }
        ITF_INLINE const Vec3d& getStartTangent() const { return m_startTangent; }
        ITF_INLINE bbool    getDisableCollisions() const { return m_disableCollisions; }
        ITF_INLINE f32      getSpeed() const { return m_speed; }

    protected:

        // everything is done in instance
        virtual void        onTemplateLoaded() {}   // bypass Super
        virtual Vec3d       getPosInternal( f32 _t ) const { ITF_ASSERT(0); return Vec3d::Zero; }
        virtual f32         getAngleInternal( f32 _t ) const { ITF_ASSERT(0); return 0; }

    private:

        TangentMode         m_tangentMode;
        Vec3d               m_startTangent;
        bbool               m_disableCollisions;
        f32                 m_speed;
    };

    ITF_INLINE const TweenBallistic_Template* TweenBallistic::getTemplate() const
    {
        return static_cast<const TweenBallistic_Template*>(m_template);
    }



    /**************** TweenInstructionWait ****************/

    class TweenInstructionWait : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionWait, TweenInstruction,2978193431);
        DECLARE_SERIALIZE_DUMMY(TweenInstructionWait)

    public:
        TweenInstructionWait() {}
        virtual ~TweenInstructionWait() {}
    };

    class TweenInstructionWait_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionWait_Template,TweenInstruction_Template,71414445);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenInstructionWait)
        DECLARE_SERIALIZE()

    public:

        TweenInstructionWait_Template() {}
        ~TweenInstructionWait_Template() {}
    };



    /**************** TweenInstructionEvent ****************/

    class TweenInstructionEvent : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionEvent, TweenInstruction,2636892017);
        DECLARE_SERIALIZE_DUMMY(TweenInstructionEvent)

    public:
        TweenInstructionEvent()
            : m_event(NULL)
        {
        }
        virtual ~TweenInstructionEvent();

        virtual void onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void onBecomeActive( const TweenCoordinates& _coordinates );

    private:
        ITF_INLINE const class TweenInstructionEvent_Template* getTemplate() const;

        Event*                  m_event;
    };

    class TweenInstructionEvent_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionEvent_Template,TweenInstruction_Template,372486786);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenInstructionEvent)
        DECLARE_SERIALIZE()

    public:

        TweenInstructionEvent_Template()
            : m_event(NULL)
            , m_triggerSelf(btrue)
            , m_triggerChildren(bfalse)
            , m_triggerBoundChildren(bfalse)
            , m_triggerGameManager(bfalse)
            , m_triggerBroadcast(bfalse)
        {
        }
        ~TweenInstructionEvent_Template();

        const Event*        getEvent() const { return m_event; }
        bbool               getTriggerSelf() const { return m_triggerSelf; }
        bbool               getTriggerChildren() const { return m_triggerChildren; }
        bbool               getTriggerBoundChildren() const { return m_triggerBoundChildren; }
        bbool               getTriggerGameManager() const { return m_triggerGameManager; }
        bbool               getTriggerBroadcast() const { return m_triggerBroadcast; }

    private:

        Event*              m_event;
        bbool               m_triggerSelf;
        bbool               m_triggerChildren;
        bbool               m_triggerBoundChildren;
        bbool               m_triggerGameManager;
        bbool               m_triggerBroadcast;             // broadcast to anyone who registered via EventManager
    };

    ITF_INLINE const TweenInstructionEvent_Template* TweenInstructionEvent::getTemplate() const
    {
        return static_cast<const TweenInstructionEvent_Template*>(m_template);
    }



    /**************** TweenInstructionFX ****************/

    class TweenInstructionFX : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionFX, TweenInstruction,3479894667);
        DECLARE_SERIALIZE_INSTANCE()
        friend class Tweening_Plugin;

    public:
        TweenInstructionFX()
            : m_fxController(NULL)
            , m_fxPlayed(bfalse)
        {
        }
        virtual ~TweenInstructionFX() {}

        virtual void onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void onBecomeActive( const TweenCoordinates& _coordinates );
        virtual void onSetFinished();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void drawDebug( const Transform3d& _initTransform,
                                const TweenCoordinates& _coordinates,
                                const Color& _color ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:
        ITF_INLINE const class TweenInstructionFX_Template* getTemplate() const;

        ObjectPath m_target;
        Pickable* getTarget() const;

        class FXControllerComponent* m_fxController;
        bbool m_fxPlayed;
    };

    class TweenInstructionFX_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionFX_Template,TweenInstruction_Template,1721356376);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenInstructionFX)
        DECLARE_SERIALIZE()

    public:

        TweenInstructionFX_Template()
            : m_stop(bfalse)
        {
            m_duration = 0.f;
        }
        ~TweenInstructionFX_Template() {}

        const StringID&     getFXControlID() const { return m_fxControlID; }
        bbool               getStop() const { return m_stop; }

    private:

        StringID            m_fxControlID;
        bbool               m_stop;
    };

    ITF_INLINE const TweenInstructionFX_Template* TweenInstructionFX::getTemplate() const
    {
        return static_cast<const TweenInstructionFX_Template*>(m_template);
    }



    /**************** TweenInstructionInput ****************/

    class TweenInstructionInput : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionInput, TweenInstruction,1422489750);
        DECLARE_SERIALIZE_DUMMY(TweenInstructionInput)

    public:
        TweenInstructionInput() {}
        virtual ~TweenInstructionInput() {}

        virtual void onBecomeActive( const TweenCoordinates& _coordinates );

    private:
        ITF_INLINE const class TweenInstructionInput_Template* getTemplate() const;
    };

    class TweenInstructionInput_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionInput_Template,TweenInstruction_Template,2854985971);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenInstructionInput)
        DECLARE_SERIALIZE()

    public:

        TweenInstructionInput_Template()
            : m_inputValue_f32(UNINIT_F32)
            , m_inputValue_u32(U32_INVALID)
        {
        }
        ~TweenInstructionInput_Template() {}

        const StringID&     getInputName() const { return m_inputName; }
        ITF_INLINE f32      getInputValue_f32() const { return m_inputValue_f32; }
        ITF_INLINE u32      getInputValue_u32() const { return m_inputValue_u32; }

    private:

        StringID            m_inputName;
        f32                 m_inputValue_f32;
        u32                 m_inputValue_u32;
    };

    ITF_INLINE const TweenInstructionInput_Template* TweenInstructionInput::getTemplate() const
    {
        return static_cast<const TweenInstructionInput_Template*>(m_template);
    }



    /**************** TweenInstructionAnim ****************/

    class TweenInstructionAnim : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionAnim, TweenInstruction,1652757797);
        DECLARE_SERIALIZE_DUMMY(TweenInstructionAnim)

    public:
        TweenInstructionAnim()
            : m_animComponent(NULL)
        {
        }
        virtual ~TweenInstructionAnim() {}

        virtual void onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void onBecomeActive( const TweenCoordinates& _coordinates );

    private:
        ITF_INLINE const class TweenInstructionAnim_Template* getTemplate() const;

        class AnimLightComponent* m_animComponent;
    };

    class TweenInstructionAnim_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionAnim_Template,TweenInstruction_Template,2809340194);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenInstructionAnim)
        DECLARE_SERIALIZE()

    public:

        TweenInstructionAnim_Template()
        {
        }
        ~TweenInstructionAnim_Template() {}

        const StringID&     getAnim() const { return m_anim; }

    private:

        StringID            m_anim;
    };

    ITF_INLINE const TweenInstructionAnim_Template* TweenInstructionAnim::getTemplate() const
    {
        return static_cast<const TweenInstructionAnim_Template*>(m_template);
    }



    /**************** TweenInstructionFlip ****************/

    class TweenInstructionFlip : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionFlip, TweenInstruction,2996166264);
        DECLARE_SERIALIZE_DUMMY(TweenInstructionFlip)

    public:
        TweenInstructionFlip() {}
        virtual ~TweenInstructionFlip() {}

        virtual void onBecomeActive( const TweenCoordinates& _coordinates );

    private:
        ITF_INLINE const class TweenInstructionFlip_Template* getTemplate() const;
    };

    class TweenInstructionFlip_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionFlip_Template,TweenInstruction_Template,1619460815);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenInstructionFlip)
        DECLARE_SERIALIZE()

    public:

        TweenInstructionFlip_Template()
        {
            m_duration = 0.f;
        }
        ~TweenInstructionFlip_Template() {}

    private:
    };

    ITF_INLINE const TweenInstructionFlip_Template* TweenInstructionFlip::getTemplate() const
    {
        return static_cast<const TweenInstructionFlip_Template*>(m_template);
    }



    /**************** TweenInstructionPlaySet ****************/

    class TweenInstructionPlaySet : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionPlaySet, TweenInstruction,915643568);
        DECLARE_SERIALIZE_DUMMY(TweenInstructionPlaySet)

    public:
        TweenInstructionPlaySet() {}
        virtual ~TweenInstructionPlaySet() {}

        virtual void onBecomeActive( const TweenCoordinates& _coordinates );

    private:
        ITF_INLINE const class TweenInstructionPlaySet_Template* getTemplate() const;

    };

    class TweenInstructionPlaySet_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenInstructionPlaySet_Template,TweenInstruction_Template,39621327);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenInstructionPlaySet)
        DECLARE_SERIALIZE()
        MY_DECLARE_VALIDATE()

    public:

        TweenInstructionPlaySet_Template()
        {
            m_duration = 0.f;
        }
        ~TweenInstructionPlaySet_Template() {}

        const StringID&     getInstructionSetName() const { return m_instructionSetName; }

    private:

        StringID            m_instructionSetName;
    };

    ITF_INLINE const TweenInstructionPlaySet_Template* TweenInstructionPlaySet::getTemplate() const
    {
        return static_cast<const TweenInstructionPlaySet_Template*>(m_template);
    }



    /**************** TweenTeleport ****************/

    class TweenTeleport : public TweenInstruction
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenTeleport, TweenInstruction,240238977);
        DECLARE_SERIALIZE_DUMMY(TweenTeleport)

    public:
        TweenTeleport() {}
        virtual ~TweenTeleport() {}

    private:
        ITF_INLINE const class TweenTeleport_Template* getTemplate() const;
    };

    class TweenTeleport_Template : public TweenInstruction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenTeleport_Template,TweenInstruction_Template,1685727569);
        DECLARE_TWEENINSTRUCTION_TEMPLATE(TweenTeleport)
        DECLARE_SERIALIZE()
        MY_DECLARE_VALIDATE()

    public:

        TweenTeleport_Template()
            : m_resetAngle(bfalse)
        {
            m_duration = 0.f;
        }
        ~TweenTeleport_Template() {}

        virtual void    execute( const f32 _t, TweenCoordinates& _coordinates ) const;
        virtual void    executeEnd( TweenCoordinates& _coordinates ) const;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void    drawDebug( const Transform3d& _initTransform,
                                   const TweenCoordinates& _coordinates,
                                   const Color& _color ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

        const Vec3d& getPosition() const { return m_position; }

    private:

        Vec3d m_position;
        bbool m_resetAngle;
    };

    ITF_INLINE const TweenTeleport_Template* TweenTeleport::getTemplate() const
    {
        return static_cast<const TweenTeleport_Template*>(m_template);
    }






    /**************** typedefs & Factory ****************/


    typedef SafeArray<TweenInstruction*> InstructionList;
    typedef SafeArray<TweenInstruction_Template*> InstructionList_Template;

    class TweenInstructionFactory_Template : public ObjectFactory
    {
    public:
        ITF_INLINE static void create() { get()->init(); }
        ITF_INLINE static TweenInstructionFactory_Template* get()
        {
            static TweenInstructionFactory_Template s_instance;
            return &s_instance;
        }

    private:
        void init();
    };

    class TweenInstructionFactory : public ObjectFactory
    {
    public:
        ITF_INLINE static void create() { get()->init(); }
        ITF_INLINE static TweenInstructionFactory* get()
        {
            static TweenInstructionFactory s_instance;
            return &s_instance;
        }

    private:
        void init();
    };
};

#endif //_ITF_TWEENINSTRUCTIONS_H_
