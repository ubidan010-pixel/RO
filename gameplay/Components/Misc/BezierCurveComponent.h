#ifndef _ITF_BEZIERCURVECOMPONENT_H_
#define _ITF_BEZIERCURVECOMPONENT_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

namespace ITF
{

    

    class BezierCurveComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(BezierCurveComponent, ActorComponent,1974194984);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

     public:

        BezierCurveComponent();
        virtual ~BezierCurveComponent();

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

               //  f32        getDistCurveTotal()     { return m_distCurveTotal;}
               //  u32        getCountNode()          { return m_bezierCurve.getNumPoints();/*return m_nodeArray.size();*/ }

               //Vec3d        getPosAtDist( f32 _queryDist);
               //  u32        getIndexSegmentAtDist( f32 _queryDist);
               // void        getPosAndIndexSegAtDist( f32 _queryDist, Vec3d& _posOut, Vec3d& _tangOut, u32& _indexSegOut, f32& _tOut);

               //Vec3d        getExitTangent() { return getTangent(1.f, m_bezierCurve.getNumPoints()- 2); }

               // void        getClosestPos( const Vec3d& _refPos, Vec3d& _closetPosOut, Vec3d& _tangOut, f32& _distOnCurveOut, u32& _indexSegOut, f32& _tOut );

                Vec3d       actorTransform(const Vec3d& _pos, bbool _translate = btrue);
                Vec3d       actorUntransform(const Vec3d& _pos, bbool _translate = btrue);
               // BezierCurve::Edge * getEdgeAt(u32 _index) { return m_bezierCurve.getEdgeAt(_index); }
               // 
                BezierCurve & getBezierCurve()  { return m_bezierCurve; }

                bbool       getLockFirstTangent() const;
                u32         getDefaultNodeCount() const;
    private:

                Vec3d       getPos( f32 _t, u32 _nodeIndex );
                Vec3d       getTangent( f32 _t, u32 _nodeIndex );

    private:

        ITF_INLINE const class BezierCurveComponent_Template*  getTemplate() const;

        f32              m_distCurveTotal;
        BezierCurve      m_bezierCurve;
    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class BezierCurveComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(BezierCurveComponent_Template,TemplateActorComponent,87532024);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(BezierCurveComponent);

    public:

        BezierCurveComponent_Template();
        ~BezierCurveComponent_Template();

        ITF_INLINE bbool getLockFirstTangent() const { return m_lockFirstTangent; }
        ITF_INLINE u32 getDefaultNodeCount() const { return m_defaultNodeCount; }

    private:
        bbool m_lockFirstTangent;
        u32 m_defaultNodeCount;
    };

    const BezierCurveComponent_Template*  BezierCurveComponent::getTemplate() const {return static_cast<const BezierCurveComponent_Template*>(m_template);}
};

#endif //_ITF_BEZIERCURVECOMPONENT_H_
