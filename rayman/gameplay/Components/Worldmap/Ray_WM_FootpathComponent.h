#ifndef _ITF_RAY_WM_FOOTPATHCOMPONENT_H_
#define _ITF_RAY_WM_FOOTPATHCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF {

class FXControllerComponent;
class Ray_WM_FootpathComponent_Template;
class Ray_WM_SpotComponent;
class TrajectoryNodeComponent;

//------------------------------------------------------------------------------
class Ray_WM_FootpathComponent : public GraphicComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_FootpathComponent, GraphicComponent,4241412189)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_WM_FootpathComponent();
    virtual ~Ray_WM_FootpathComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return btrue; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void onBecomeActive();
    virtual void onEvent(Event* _event);
    virtual void Update(f32 _dt);
    virtual void Draw();

    // to call if no open is performed this time
    void initNoOpen();

    // regular unroll
    void forceOpen();
    void open();

    // B-worlds special case
    void forceOpenNonBWorlds();
    void openBWorlds();

    // unroll footpaths from non-completed nodes (RO-14431)
    void openFromNonCompletedToCompleted();

    // ... and keep them unrolled (RO-16406)
    void forceOpenFromNonCompletedToCompleted();

    // fog teasing
    void teaseFog(bbool _force, bbool _toBWorld = bfalse);

    // connect FX
    void playConnectFx();

    // player entry FX
    void onEnter();
    void onExit();

#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

private:
    enum BranchState_t
    {
        BranchState_Closed,
        BranchState_Opening,
        BranchState_Open,
    };

    struct Branch
    {
        Branch()
            : m_state(BranchState_Closed)
            , m_length(0.0f)
            , m_bezier()
            , m_bezierCut()
            , m_endVisual()
            , m_endFog()
            , m_endRoots()
            , m_endActor()
            , m_endTag()
            , m_toBWorld(bfalse)
            , m_toCompleted(bfalse)
            {}
        BranchState_t m_state;
        f32 m_length;
        BezierCurve m_bezier;
        BezierCurve m_bezierCut;
        BezierPatchList m_patches;
        ActorRef m_endVisual;
        ActorRef m_endFog;
        ActorRef m_endRoots;
        ActorRef m_endActor;
        StringID m_endTag;
        bbool m_toBWorld;
        bbool m_toCompleted;
    };

    typedef ITF_VECTOR<Branch> BranchList;

    ITF_INLINE const Ray_WM_FootpathComponent_Template* getTemplate() const;

    void createBranches(
        BranchList& _branches,
        bbool _spotAccessibility,
        bbool _connect);

    Actor* getVisual(Actor* _spot);
    Actor* getBoss(Actor* _spot);
    Actor* getBeaten(Actor* _spot);
    Actor* getFog(Actor* _spot);
    Actor* getFX(Actor* _spot);
    Actor* getRoots(Actor* _spot);

    void setupVisual(Actor* _actor);

    void show(Actor* _actor, bbool _force);
    void hide(Actor* _visual, bbool _force);

    void showRoots(Actor* _roots, bbool _force);
    void hideRoots(Actor* _roots, bbool _force);

    void teaseBWorlds(bbool _force);
    void teaseLandOfTheDead();

    bbool m_allOpen;
    f32 m_allOpenTimer;
    bbool m_done;
    BranchList m_branches;

    bbool m_fogShownByBuddyNode;

    BezierCurveRenderer m_bezierRenderer;
    TrajectoryNodeComponent* m_trajectoryNode;
    FXControllerComponent* m_fxController;
};

//------------------------------------------------------------------------------
class Ray_WM_FootpathComponent_Template : public GraphicComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_FootpathComponent_Template, GraphicComponent_Template,2256037681);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_FootpathComponent);

public:
    Ray_WM_FootpathComponent_Template();
    virtual ~Ray_WM_FootpathComponent_Template();

    ITF_INLINE f32 getSpeed() const { return m_speed; }
    ITF_INLINE f32 getZOffset() const { return m_zOffset; }
    ITF_INLINE f32 getTangentStretch() const { return m_tangentStretch; }
    ITF_INLINE f32 getVisualFadeInDuration() const { return m_visualFadeInDuration; }
    ITF_INLINE f32 getFxFadeInDuration() const { return m_fxFadeInDuration; }
    ITF_INLINE f32 getCameraFollowDistance() const { return m_cameraFollowDistance; }
    ITF_INLINE u32 getSamplePerEdge() const { return m_samplePerEdge; }
    ITF_INLINE const StringID& getConnectFxName() const { return m_connectFxName; }
    ITF_INLINE const BezierCurveRenderer_Template& getBezierRenderer() const { return m_bezierRenderer; }

private:
    f32 m_speed;
    f32 m_zOffset;
    f32 m_tangentStretch;
    f32 m_visualFadeInDuration;
    f32 m_fxFadeInDuration;
    f32 m_cameraFollowDistance;
    u32 m_samplePerEdge;
    StringID m_connectFxName;
    BezierCurveRenderer_Template m_bezierRenderer;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_WM_FootpathComponent_Template*
Ray_WM_FootpathComponent::getTemplate() const
{
    return static_cast<const Ray_WM_FootpathComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_WM_FOOTPATHCOMPONENT_H_
