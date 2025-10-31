#ifndef _ITF_PATCHCURVECOMPONENT_H_
#define _ITF_PATCHCURVECOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_PATCHCURVE_H_
#include "engine/display/PatchCurve.h"
#endif //_ITF_PATCHCURVE_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

namespace ITF
{
    /*
        Base component to draw a PatchCurve between 2 points (eg an actor and its children).

        Usage:
        - draw a patch oriented on X (left-to-right)
        - uvMin is left, uvMax is right
        - if tileLength == 0, uvMin = 0, uvMax = 1
            - otherwise uvMin = 0, uvMax = length / tileLength
        - if attachToChild == true:
            - uvMin = -length / tileLength, uvMax = 0
        - uvScrollSpeed: positive value will give you left-to-right movement
    */
    class PatchCurveComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PatchCurveComponent, GraphicComponent,3824839181);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        PatchCurveComponent();
        virtual ~PatchCurveComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

        Vec2d               getParentAttachPosition();
        Vec2d               getParentAttachNormal();
        Vec2d               getChildAttachPosition( Actor* _child );
        Vec2d               getChildAttachNormal( Actor* _child, const Vec2d& _parentPos, const Vec2d& _childPos );

        void                drawLink( const Vec2d& _parentPos, const Vec2d& _parentNormal, const Vec2d& _childPos, const Vec2d& _childNormal, PatchCurve& _patchCurve, const f32 _alpha = 1.f );
        void                drawLink( const Vec2d& _parentPos, const Vec2d& _parentNormal, const Vec2d& _childPos, const Vec2d& _childNormal, BezierPatch& _bezierPatch, const f32 _alpha = 1.f );

    protected:

        AnimLightComponent* m_animComponent;

        ResourceID      m_texture;

    private:
        ITF_INLINE const class PatchCurveComponent_Template*  getTemplate() const;

        void            clear();
        void            computeCommonData( const Vec2d& _p2, const Vec2d& _p1, f32& patchLength, f32& uvLength, Vec2d& startUV, Vec2d& endUV );

        f32             m_timer;
    };

    //-------------------------------------------------------------------------------------
    class PatchCurveComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(PatchCurveComponent_Template, GraphicComponent_Template,2155725626);
        DECLARE_ACTORCOMPONENT_TEMPLATE(PatchCurveComponent);
        DECLARE_SERIALIZE();

    public:

        PatchCurveComponent_Template();
        ~PatchCurveComponent_Template() {}

        const Path&     getTextureFile() const { return m_textureFile; }
        f32             getWidth() const { return m_width; }
        f32             getTileLength() const { return m_tileLength; }
        f32             getUVScrollSpeed() const { return m_uvScrollSpeed; }
        f32             getZOffset() const { return m_zOffset; }
        ITF_INLINE f32  getTessellationRatio() const { return m_tessellationRatio; }
        const StringID& getParentBone() const { return m_parentBone; }
        const StringID& getChildBone() const { return m_childBone; }
        f32             getChildOrientationInfluence() const { return m_childOrientationInfluence; }
        bbool           getAttachToChild() const { return m_attachToChild; }

        virtual void    onTemplateLoaded();

    private:

        Path            m_textureFile;
        f32             m_width;
        f32             m_tileLength;
        f32             m_uvScrollSpeed;
        f32             m_zOffset;
        f32             m_tessellationLength;
        f32             m_tessellationRatio;    // onTemplateLoaded
        StringID        m_parentBone;
        StringID        m_childBone;
        f32             m_childOrientationInfluence;    // 1 = use child down axis, 0 = use parent-to-child direction
        bbool           m_attachToChild;
    };



    const PatchCurveComponent_Template*  PatchCurveComponent::getTemplate() const {return static_cast<const PatchCurveComponent_Template*>(m_template);}

};

#endif //_ITF_PATCHCURVECOMPONENT_H_