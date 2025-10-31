#ifndef _ITF_RAY_CRYSTALKINGAICOMPONENT_H_
#define _ITF_RAY_CRYSTALKINGAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{

    class LinkComponent;
    class AnimLightComponent;
    class FXControllerComponent;

    class Ray_CrystalKingAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CrystalKingAIComponent, Ray_AIComponent,4136222629)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_CrystalKingAIComponent();
        virtual ~Ray_CrystalKingAIComponent();

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return btrue; }
        virtual bbool   needsDraw2D() const     { return bfalse; }

        virtual void    onBecomeActive();

        virtual void    Update( f32 _deltaTime );
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);
        virtual void    onActorClearComponents();

        virtual void    Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_CrystalKingAIComponent_Template*  getTemplate() const;

        void init();

        void playSound();
        void playAnim( const StringID _anim);
        void playAnimOtherAct( ActorRef _actorRef, const StringID _anim);        
        void changeState( u32 _newState );
        void updateState( f32 _dt );
        void growAABB();


        // Mesh
        void loadResource();
        void unloadResource();
        void createMesh(ITF_Mesh& _mesh, SafeArray<VertexPCT>& _vertexBufferCache);
        void destroyMesh(ITF_Mesh& _mesh);
        void buildMesh();
        void drawMesh(ITF_Mesh& _mesh, SafeArray<VertexPCT>& _vertexBufferCache, f32 _z);


    private:
     
        LinkComponent*      m_linkComponent;
        AnimLightComponent* m_animLightComponent;
        FXControllerComponent* m_fxControllerComponent;
     
        bbool     m_init;
        u32       m_state;

        // Actors
        ActorRef    m_faery;
        ActorRef    m_slotDn;
        ActorRef    m_stele;

        //Mesh
        ResourceID m_resource;

        ITF_Mesh m_meshFront;
        SafeArray<VertexPCT> m_vertexBufferCacheFront;
		ITF_Mesh m_meshFadeFront;
		SafeArray<VertexPCT> m_vertexBufferCacheFadeFront;

        ITF_Mesh m_meshBack;
        SafeArray<VertexPCT> m_vertexBufferCacheBack;
		ITF_Mesh m_meshFadeBack;
		SafeArray<VertexPCT> m_vertexBufferCacheFadeBack;

        Vec2d m_offsetScrollUV;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_CrystalKingAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CrystalKingAIComponent_Template, Ray_AIComponent_Template,591226493);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CrystalKingAIComponent);

    public:

        Ray_CrystalKingAIComponent_Template();
        ~Ray_CrystalKingAIComponent_Template();

        ITF_INLINE const StringID & getCheckMapForUnlock() const { return m_checkMapForUnlock;}

        ITF_INLINE const Path & getTextureFile() const { return m_textureFile;}
        ITF_INLINE f32 getTileLen() const { return m_tileLen;}

        ITF_INLINE Vec3d getRayOffsetCrystal() const { return m_rayOffsetCrystal;}
        ITF_INLINE Vec3d getRayOffsetStart() const { return m_rayOffsetStart;}

        ITF_INLINE f32 getWidthA() const { return m_widthA;}
        ITF_INLINE f32 getWidthB() const { return m_widthB;}
        ITF_INLINE f32 getWidthC() const { return m_widthC;}
        ITF_INLINE f32 getWidthD() const { return m_widthD;}

        ITF_INLINE f32 getSpeedScroll() const { return m_speedScroll;}

        ITF_INLINE f32 getRayZOffset_Part1() const { return m_rayZOffset_Part1;}
        ITF_INLINE f32 getRayZOffset_Part2() const { return m_rayZOffset_Part2;}


    private:
    
        StringID m_checkMapForUnlock;

        Path m_textureFile;
        f32  m_tileLen;

        Vec3d m_rayOffsetCrystal;
        Vec3d m_rayOffsetStart;

        f32  m_widthA;
        f32  m_widthB;
        f32  m_widthC;
        f32  m_widthD;

        f32  m_speedScroll;

        f32 m_rayZOffset_Part1;
        f32 m_rayZOffset_Part2;

    };

    const Ray_CrystalKingAIComponent_Template*  Ray_CrystalKingAIComponent::getTemplate() const {return static_cast<const Ray_CrystalKingAIComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_CRYSTALKINGAICOMPONENT_H_