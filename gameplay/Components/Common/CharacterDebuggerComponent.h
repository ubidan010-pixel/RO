#ifndef _ITF_CHARACTERDBUGGERCOMPONENT_H_
#define _ITF_CHARACTERDBUGGERCOMPONENT_H_

namespace ITF
{
    class CharacterDebuggerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CharacterDebuggerComponent,ActorComponent,3709650243);

    public:
        DECLARE_SERIALIZE()

        CharacterDebuggerComponent();
        virtual ~CharacterDebuggerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        Update( f32 _deltaTime );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        editor_draw();
#endif // ITF_SUPPORT_EDITOR

    protected:

        ITF_INLINE const class CharacterDebuggerComponent_Template*  getTemplate() const;


        struct STrajectoryPoint
        {
            bbool m_sticked;
            Vec3d m_pos;
            Vec2d m_axis;
        };

        typedef ITF_LIST <STrajectoryPoint> TrajectoryList;
        typedef ITF_LIST <Vec2d> PointsList;


        // Read from LUA
        TrajectoryList      m_trajectory;
        PointsList          m_contacts;
        ObjectRef           m_stickedPolyline;
        u32                 m_stickedEdge;
        Vec2d               m_currentForce;
        Vec2d               m_currentGroundDir;
        Vec2d               m_currentSpeed;
        Vec3d               m_physPos;
        Vec3d               m_physPrevPos;
        Vec2d               m_speedAverage;
        f32                 m_torqueAverage;
        Vec2d               m_currentGravityDir;
        SafeArray <Angle>   m_stickyRanges;
        f32                 m_currentFrictionCoef;
        f32                 m_radius;
        bbool               m_active;
        
        static const u32    s_maxPointsTrajectory;
        static const f32    s_trajectoryPointSize;
        static const f32    s_contactsPointSize;
        static const f32    s_minForceSize;
        static const f32    s_maxForceSize;
        static const f32    s_maxForce;
    };

    //---------------------------------------------------------------------------------------------------

    class CharacterDebuggerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CharacterDebuggerComponent_Template,TemplateActorComponent,2200057270);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(CharacterDebuggerComponent);

    public:

        CharacterDebuggerComponent_Template();
        ~CharacterDebuggerComponent_Template() {}

        bbool               getShowCollider() const { return m_showCollider; }
        bbool               getShowTrajectory() const { return m_showTrajectory; }
        bbool               getDebugController() const { return m_debugController; }
        bbool               getDebugAnim() const { return m_debugAnim; }
        bbool               getDebugAnimInputs() const { return m_debugAnimInputs; }
        bbool               getDebugAI() const { return m_debugAI; }
        u32                 getMaxTrajectory() const { return m_maxTrajectory; }
        bbool               getDebugSoundInputs() const { return m_debugSoundInputs; }

    private:

        bbool               m_showCollider;
        bbool               m_showTrajectory;
        bbool               m_debugController;
        bbool               m_debugAnim;
        bbool               m_debugAnimInputs;
        bbool               m_debugAI;
        u32                 m_maxTrajectory;
        bbool               m_debugSoundInputs;
    };

    ITF_INLINE const CharacterDebuggerComponent_Template*  CharacterDebuggerComponent::getTemplate() const {return static_cast<const CharacterDebuggerComponent_Template*>(m_template);}

}

#endif // _ITF_CHARACTERDBUGGERCOMPONENT_H_

