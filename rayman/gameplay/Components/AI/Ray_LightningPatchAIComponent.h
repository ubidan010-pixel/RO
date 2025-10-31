#ifndef _ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_
#define _ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF   
{  

    class Ray_LightningPatchAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LightningPatchAIComponent, ActorComponent,2300483196);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        // ********************************************************************
     
    public:

        Ray_LightningPatchAIComponent();
        virtual ~Ray_LightningPatchAIComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return btrue; }
        virtual bbool needsDraw2D() const { return bfalse; }
        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void Update( f32 _dt );
        virtual void Draw();

        void open();
        void close(bbool _force);
        void charge();
#ifdef ITF_SUPPORT_EDITOR
        virtual void drawEdit( class ActorDrawEditInterface* , u32 _flags ) const;
#endif //ITF_SUPPORT_EDITOR

    protected:
        virtual BezierCurve &   getBezierCurve() { return m_bezierCurve; }
        ITF_INLINE const class Ray_LightningPatchAIComponent_Template*  getTemplate() const;
        f32 getRayCastDist(const Vec2d & _pos, const Vec2d & _dir, f32 _dist);
    private:

        void updateTail(u32 _add);
        void updateHead(u32 _add);
        void updateBody(u32 _add);

        BezierCurve m_bezierCurve;
        u32 m_boneIndex;
        u32 m_effectBoneIndex;
        class AnimLightComponent * m_animComponent;
        BezierCurveRenderer m_renderer;
        AtlasDynamicObject m_head;
        AtlasDynamicObject m_tail;
        enum State
        {
            State_None,
            State_Anticipation,
            State_Loop,
            State_End,
        };
        State m_tailState;
        State m_headState;
        State m_bodyState;

        u32 m_tailIndex;
        u32 m_headIndex;
        u32 m_bodyIndex;

        f32 m_curDist;
        f32 m_drawDist;
        f32 m_timer;

        Vec3d m_start;
        Vec3d m_stop;
        Vec3d m_effectStart;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_LightningPatchAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LightningPatchAIComponent_Template, TemplateActorComponent,302749681);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LightningPatchAIComponent);

    public:

        Ray_LightningPatchAIComponent_Template();
        ~Ray_LightningPatchAIComponent_Template();

        ITF_INLINE StringID getBoneName() const { return m_boneName; }
        ITF_INLINE StringID getEffectBoneName() const { return m_effectBoneName; }
        ITF_INLINE f32 getMaxLength() const { return m_maxLength; }
        ITF_INLINE const BezierCurveRenderer_Template & getBezierRenderer() const { return m_bezierRenderer; }
        ITF_INLINE const Path & getHeadPath() const { return m_HeadPath; }
        ITF_INLINE const Path & getBodyPath() const { return m_BodyPath; }
        ITF_INLINE const Path & getTailPath() const { return m_TailPath; }
        ITF_INLINE u32 getHeadMaxIndices() const { return m_headMaxIndices; }
        ITF_INLINE u32 getTailMaxIndices() const { return m_tailMaxIndices; }
        ITF_INLINE u32 getBodyMaxIndices() const { return m_bodyMaxIndices; }
        ITF_INLINE u32 getHeadLoopStart() const { return m_headLoopStart; }
        ITF_INLINE u32 getHeadLoopStop() const { return m_headLoopStop; }
        ITF_INLINE u32 getTailLoopStart() const { return m_tailLoopStart; }
        ITF_INLINE u32 getTailLoopStop() const { return m_tailLoopStop; }
        ITF_INLINE u32 getBodyLoopStart() const { return m_bodyLoopStart; }
        ITF_INLINE u32 getBodyLoopStop() const { return m_bodyLoopStop; }
        ITF_INLINE const Vec2d & getHeadSize() const { return m_headSize; }
        ITF_INLINE const Vec2d & getTailSize() const { return m_tailSize; }
        ITF_INLINE f32 getSpeed() const { return m_speed; }
        ITF_INLINE const Vec2d & getHeadCenter() const { return m_headCenter; }
        ITF_INLINE const Vec2d & getTailCenter() const { return m_tailCenter; }
        ITF_INLINE f32 getPlayRate() const { return m_playRate; }
        ITF_INLINE f32 getHeadZOffset() const { return m_headZOffset; }
        ITF_INLINE f32 getTailZOffset() const { return m_tailZOffset; }
        ITF_INLINE f32 getBodyZOffset() const { return m_bodyZOffset; }
        
    private:

        StringID m_boneName;
        StringID m_effectBoneName;
        f32 m_maxLength;
        BezierCurveRenderer_Template m_bezierRenderer;
        Path m_HeadPath;
        Path m_BodyPath;
        Path m_TailPath;
        u32 m_headMaxIndices;
        u32 m_tailMaxIndices;
        u32 m_bodyMaxIndices;
        u32 m_headLoopStart;
        u32 m_headLoopStop;
        u32 m_tailLoopStart;
        u32 m_tailLoopStop;
        u32 m_bodyLoopStart;
        u32 m_bodyLoopStop;
        Vec2d m_headSize;
        Vec2d m_tailSize;
        f32 m_speed;
        Vec2d m_headCenter;
        Vec2d m_tailCenter;
        f32 m_playRate;
        f32 m_headZOffset;
        f32 m_tailZOffset;
        f32 m_bodyZOffset;
    };


    const Ray_LightningPatchAIComponent_Template*  Ray_LightningPatchAIComponent::getTemplate() const {return static_cast<const Ray_LightningPatchAIComponent_Template*>(m_template);}
};

#endif //_ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_
