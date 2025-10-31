#ifndef _ITF_CURVECOMPONENT_H_
#define _ITF_CURVECOMPONENT_H_

namespace ITF
{

    class CurveComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CurveComponent, ActorComponent,1270663238);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        // ********************************************************************

        struct TrajectoryNode
        {
            DECLARE_SERIALIZE()

            TrajectoryNode()
            : m_lengthSeg(0.0f)
            {
                // none
            }

            Vec3d m_pos;
            Vec3d m_tangA;
            Vec3d m_tangB;

            f32   m_lengthSeg;
            SafeArray<f32> trajectorySamples;
        };

        typedef ITF_VECTOR<TrajectoryNode> TrajectoryArray;

        // ********************************************************************

     public:

        CurveComponent();
        virtual ~CurveComponent();

        virtual bbool       needsUpdate() const     { return btrue; }
        virtual bbool       needsDraw() const       { return bfalse; }
        virtual bbool       needsDraw2D() const     { return bfalse; }

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorCreated( Actor* _original );
#endif // ITF_SUPPORT_EDITOR
        virtual void        onBecomeActive();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

                void        init();

        TrajectoryArray&    getNodeArrayRef()       { return m_nodeArray; }
                 f32        getDistCurveTotal()     { return m_distCurveTotal;}
                 u32        getCountNode()          { return m_nodeArray.size(); }

               Vec3d        getPosAtDist( f32 _queryDist);
                 u32        getIndexSegmentAtDist( f32 _queryDist);
                void        getPosAndIndexSegAtDist( f32 _queryDist, Vec3d& _posOut, Vec3d& _tangOut, u32& _indexSegOut, f32& _tOut);

               Vec3d        getExitTangent() { return getTangent(1.f, m_nodeArray.size() - 2); }

                void        getClosestPos( const Vec3d& _refPos, Vec3d& _closetPosOut, Vec3d& _tangOut, f32& _distOnCurveOut, u32& _indexSegOut, f32& _tOut );

                Vec3d       actorTransform(const Vec3d& _pos, bbool _translate = btrue);
                Vec3d       actorUntransform(const Vec3d& _pos, bbool _translate = btrue);
    private:

                Vec3d       getPos( f32 _t, u32 _nodeIndex );
                Vec3d       getTangent( f32 _t, u32 _nodeIndex );

    private:

        ITF_INLINE const class CurveComponent_Template*  getTemplate() const;

        TrajectoryArray  m_nodeArray;
        f32              m_distCurveTotal;
    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class CurveComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CurveComponent_Template,TemplateActorComponent,449299892);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(CurveComponent);

    public:

        CurveComponent_Template();
        ~CurveComponent_Template();

    private:

    };

    const CurveComponent_Template*  CurveComponent::getTemplate() const {return static_cast<const CurveComponent_Template*>(m_template);}
};

#endif //_ITF_CURVECOMPONENT_H_
