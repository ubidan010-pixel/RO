
#ifndef _ITF_GRAPHICCOMPONENT_H_
#define _ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "../actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

namespace ITF
{

struct ShadowObj
{
	//shadow mesh
	ITF_Mesh				m_shadowMesh;
	//TODO: create only if shadow ou alors structure shadow...
	VertexPCT				m_cacheVB[10];

	ResourceID				m_shadowTexture;
	Vec3d					m_shadowPos[5];
	u32						m_sS;
	u32						m_sE;
	bbool					m_renderShadow;
	f32						m_shadowAlpha;
	f32						m_shadowMul;
};

class GraphicComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(GraphicComponent,ActorComponent,2152159230);

public:
    DECLARE_SERIALIZE()

    GraphicComponent();
    ~GraphicComponent();

    virtual bbool           needsUpdate() const { return btrue; }
    virtual bbool           needsDraw() const { return bfalse; }
    virtual bbool           needsDraw2D() const { return bfalse; }
    virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void            onCheckpointLoaded();

    virtual void            Update( f32 _deltaTime );
    virtual void            onEvent( Event * _event);

    virtual void            addLight(const LightInfo& ){};
    virtual AABB            getVisualAABB() const;

	void                    clear();
    void                    processAlpha(f32 _deltaTime); // process alpha when override Update
    void                    processShadow(f32 _deltaTime, f32 _angle);
	void					drawShadow();
	const bbool             useShadow() const {  return m_shadowObj ? btrue : bfalse;  }
	void					setShadowMul(f32 _val) {m_shadowObj->m_shadowMul = _val;}

	void					createShadowMesh();
	void					destroyShadowMesh();
	void					computeVertexBufferCache();

    Vec2d                   getShadowSize();

    static ITF_IndexBuffer*     getStaticIndexBuffer(u32 _hdiv, u32 _vdiv);
    ITF_IndexBuffer*            getMyStaticIndexBuffer();

	ShadowObj*				getShadowObject() {return m_shadowObj;}
    
	f32						getAlpha() const { return m_alpha; }
	void					setAlpha(f32 _alpha) { m_alpha = _alpha; }
    void                    setColorFog(   const Color& _color );
    const Color&            getColorFog(   );

    void                    setColorFactor(   const Color& _color );
    const Color&            getColorFactor( );

    ITF_INLINE u32          getHDiv() const { return m_hDiv; }
    ITF_INLINE u32          getVDiv() const { return m_vDiv; }

    void                    setTagId(u32 _tagid) {  m_colorComputerTagId = _tagid;}
    const u32               getTagId() const {  return m_colorComputerTagId;    }
	
    bbool                   getDisableLight() const { return m_disableLight; }
	void					setDisableLight(	bbool _v	) {	m_disableLight = _v;	};

    ITF_INLINE const class GraphicComponent_Template*  getTemplate() const;

protected:

    u32                     m_hDiv;
    u32                     m_vDiv;
    f32                     m_alpha;
    f32                     m_alphaSrc;
    f32                     m_alphaDst;
    f32                     m_faidingTime;
    f32                     m_faidingValue;
    bbool                   m_pauseOnFade;
    bbool                   m_destroyOnFade;
    bbool                   m_renderInTarget;

    //deprecate (start)
    f32                     m_colorFog_red;
    f32                     m_colorFog_green;
    f32                     m_colorFog_blu;
    f32                     m_colorFog_alpha;
    //deprecate (end)

    Color                   m_colorFog;
    Color                   m_colorFactor;

    // Material params. TODO: use a material instead.
    u32                     m_matFlags;

    // WARNING don't serialize in Final :
    u32                     m_colorComputerTagId;
	
	bbool                   m_disableLight;

	// Shadow.
	ShadowObj*				m_shadowObj;

private:
    void                    reset();
};

class GraphicComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(GraphicComponent_Template,TemplateActorComponent,1900486610);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(GraphicComponent);

public:

    GraphicComponent_Template();
    ~GraphicComponent_Template();

    u32                     getPatchLevel() const { return m_patchLevel; }
    u32                     getHDiv() const { return m_hDiv; }
    u32                     getVDiv() const { return m_vDiv; }
    const AABB&             getVisualAABB() const { return m_visualAABB; }
    const Vec2d&            getPosOffset() const { return m_posOffset; }
    const Angle&            getRotOffset() const { return m_angleOffset; }
    GFX_BLENDMODE           getBlendMode() const { return m_blendMode; }
    GFX_MATERIAL_TYPE       getMaterialType() const { return m_matType; }
    Color                   getSelfIllumColor() const { return m_selfIllumColor; }
    ITF_INLINE bbool        getDisableLight() const { return m_disableLight; }
    ITF_INLINE bbool        forceDisableLight() const { return m_forceDisableLight; }

	bbool					getIsUseShadow() const {return m_useShadow;}
	bbool					getUseRootBone() const {return m_useRootBone;}
	
	const Path&				getShadowTextureFile() const { return m_shadowTextureFile; }
	const Vec2d&            getShadowSize() const { return m_shadowSize; }
	const f32	            getShadowAttenuation() const { return m_shadowAttenuation; }
	const Vec3d&            getShadowOffsetPos() const {return m_shadowOffsetPos; }
	const f32	            getShadowDist() const { return m_shadowDist; }
	const f32	            getAngleLimit() const { return m_angleLimit; }

private:

    u32                     m_patchLevel;
    u32                     m_hDiv;
    u32                     m_vDiv;
    AABB                    m_visualAABB;
    bbool                   m_renderInTarget; //map to m_matflags.
    Vec2d                   m_posOffset;
    Angle                   m_angleOffset;
    GFX_BLENDMODE           m_blendMode;
    GFX_MATERIAL_TYPE       m_matType;
    Color                   m_selfIllumColor;
    bbool                   m_disableLight; // default value, overridable per instance
    bbool                   m_forceDisableLight;    //force disable light value...

    // shadow Params.
	bbool					m_useShadow;
	bbool					m_useRootBone;
	Vec2d					m_shadowSize;
	Path					m_shadowTextureFile;
	f32						m_shadowAttenuation;
	f32						m_shadowDist;
	Vec3d					m_shadowOffsetPos;
	f32						m_angleLimit;
};

    //ITF_INLINE const class GraphicComponent_Template* GraphicComponent::getTemplate() const { return static_cast<const GraphicComponent_Template*>(m_template); }
	const GraphicComponent_Template*  GraphicComponent::getTemplate() const {return static_cast<const GraphicComponent_Template*>(m_template);}
}

#endif // _ITF_GRAPHICCOMPONENT_H_

