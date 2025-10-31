#ifndef _ITF_RAY_ABYSSLIGHTCOMPONENT_H_
#define _ITF_RAY_ABYSSLIGHTCOMPONENT_H_

#ifndef _ITF_LIGHTCOMPONENT_H_
#include "gameplay/components/misc/LightComponent.h"
#endif //_ITF_LIGHTCOMPONENT_H_

namespace ITF
{
    //forward
    class AnimLightComponent;
    class Ray_AbyssLightComponent_Template;

    class Ray_AbyssLightComponent : public LightComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AbyssLightComponent, LightComponent,3139200749)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:
        Ray_AbyssLightComponent();
        ~Ray_AbyssLightComponent();
        
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return btrue; }
        virtual bbool needsDraw2D() const { return bfalse; }

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onActorClearComponents();
        virtual void onBecomeActive();
        virtual void onBecomeInactive();
        virtual void Update(f32 _dt);
        virtual void Draw();

        ITF_INLINE f32 getRadius() const;
        ITF_INLINE void setRadius(const f32 _radius);

        ITF_INLINE void setOffset(const Vec2d & _offset);

        ITF_INLINE void stopWithDelay( f32 _delay, bbool _destroyObjAtEnd ) { ITF_ASSERT( _delay > 0.0f ); m_stopDelay = _delay; m_stopTimer = _delay; m_stopDestroyAtEnd = _destroyObjAtEnd; m_stopInitRadius = getRadius(); }

        //get actor pos
        Vec2d get2DPos() const;

        static bbool segmentIntersectWithLight(const Vec2d & _position, const Vec2d & _orientation, const f32 _length);
        static bbool segmentIntersectWithLight(const Vec2d & _segmentStart, const Vec2d & _segmentStop);
        static bbool AABBIntersectWithLight(const Vec2d & _pos, const AABB &_aabb);

    private:
        ITF_INLINE const Ray_AbyssLightComponent_Template * const getTemplate() const;

        //instance registration management
        static void registerInstance(Ray_AbyssLightComponent * const _instance);
        static void unregisterInstance(Ray_AbyssLightComponent * const _instance);

        //texture, mesh and VB management
        void loadTexture();
        void freeTexture();
        void createMesh();
        void destroyMesh();
        void computeVertexBuffer();

        //alpha, size & position variation
        void updateVariations(const f32 _dt);

        //sound management
        void startSound();
        void stopSound();
        void updateSound();
        void updateStop( f32 _dt );

        //data
    private:

        //instances static array
        static SafeArray<Ray_AbyssLightComponent *> ms_instances;

        //radius (light distance and texture size)
        f32 m_radius;

        //offset (light offset from object position)
        Vec2d m_offset;

        //cycle stuff (used for alpha and size variation)
        f32 m_cycleTimer;
        f32 m_cycleDuration;

        //light alpha
        f32 m_alpha;

        //light size variation (doesn't affect logic size only texture size)
        f32 m_sizeVariation;

        //light position variation (doesn't affect logic position only texture position)
        Vec2d m_positionVariation;

        //anim light component used to get a wanted bone position
        AnimLightComponent * m_animLightComponent;

        //sound component
        SoundComponent * m_soundComponent;
        ITF::SoundHandle m_soundHandle;

        //texture resource id
		ResourceID m_textureId;
		//mesh
		ITF_Mesh m_mesh;
		VertexPCT m_cacheVB[6];

        //bool that indicates if the instance is registered to static instances array
        bbool m_instanceRegistered;

        // timer used to stop with delay
        f32     m_stopDelay;
        f32     m_stopTimer;
        f32     m_stopInitRadius;
        bbool   m_stopDestroyAtEnd;
    };

    class Ray_AbyssLightComponent_Template : public LightComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AbyssLightComponent_Template, LightComponent_Template,1607664492);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AbyssLightComponent);

    public:
        Ray_AbyssLightComponent_Template();
        ~Ray_AbyssLightComponent_Template();

        ITF_INLINE f32 getRadius() const;
        ITF_INLINE bbool useTexture() const;
		ITF_INLINE const Path & getTexturePath() const;
        ITF_INLINE f32 getTextureRenderPrio() const;
        ITF_INLINE f32 getTextureRadiusRatio() const;
        ITF_INLINE const StringID & getBoneName() const;
        ITF_INLINE f32 getVariationMinTime() const;
        ITF_INLINE f32 getVariationMaxTime() const;
        ITF_INLINE f32 getAlphaVariation() const;
        ITF_INLINE f32 getSizeVariation() const;
        ITF_INLINE f32 getPositionVariation() const;
        ITF_INLINE const StringID & getSoundName() const;

    private:
        f32 m_radius;
        bbool m_useTexture;
        Path m_texturePath;
        f32 m_textureRenderPrio;
        f32 m_textureRadiusRatio;
        StringID m_boneName;
        f32 m_variationMinTime;
        f32 m_variationMaxTime;
        f32 m_alphaVariation;
        f32 m_sizeVariation;
        f32 m_positionVariation;
        StringID m_soundName;
        
    };

    ITF_INLINE f32 Ray_AbyssLightComponent::getRadius() const
    {
        return m_radius;
    }

    ITF_INLINE void Ray_AbyssLightComponent::setRadius(const f32 _radius)
    {
        m_radius = _radius;
    }

    ITF_INLINE void Ray_AbyssLightComponent::setOffset(const Vec2d & _offset)
    {
        m_offset = _offset;
    }

    ITF_INLINE const Ray_AbyssLightComponent_Template * const Ray_AbyssLightComponent::getTemplate() const
    {
        return static_cast<const Ray_AbyssLightComponent_Template * const>( m_template );
    }




    ITF_INLINE f32 Ray_AbyssLightComponent_Template::getRadius() const
    {
        return m_radius;
    }

    ITF_INLINE bbool Ray_AbyssLightComponent_Template::useTexture() const
    {
        return m_useTexture;
    }

    ITF_INLINE const Path & Ray_AbyssLightComponent_Template::getTexturePath() const
    {
        return m_texturePath;
    }

    ITF_INLINE f32 Ray_AbyssLightComponent_Template::getTextureRenderPrio() const
    {
        return m_textureRenderPrio;
    }

    ITF_INLINE f32 Ray_AbyssLightComponent_Template::getTextureRadiusRatio() const
    {
        return m_textureRadiusRatio;
    }

    ITF_INLINE const StringID & Ray_AbyssLightComponent_Template::getBoneName() const
    {
        return m_boneName;
    }

    ITF_INLINE f32 Ray_AbyssLightComponent_Template::getVariationMinTime() const
    {
        return m_variationMinTime;
    }

    ITF_INLINE f32 Ray_AbyssLightComponent_Template::getVariationMaxTime() const
    {
        return m_variationMaxTime;
    }

    ITF_INLINE f32 Ray_AbyssLightComponent_Template::getAlphaVariation() const
    {
        return m_alphaVariation;
    }

    ITF_INLINE f32 Ray_AbyssLightComponent_Template::getSizeVariation() const
    {
        return m_sizeVariation;
    }

    ITF_INLINE f32 Ray_AbyssLightComponent_Template::getPositionVariation() const
    {
        return m_positionVariation;
    }

    ITF_INLINE const StringID & Ray_AbyssLightComponent_Template::getSoundName() const
    {
        return m_soundName;
    }
}

#endif //_ITF_RAY_ABYSSLIGHTCOMPONENT_H_