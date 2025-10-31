#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#define _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/GraphicComponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

namespace ITF
{
    class FXControllerComponent;
    class AnimLightComponent;
    class PolylineComponent;
    class Ray_EventQueryCanJumpFromCurrent;
    class HitStim;

    class Ray_GeyserPlatformAIComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GeyserPlatformAIComponent, GraphicComponent,176644665);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_GeyserPlatformAIComponent();
        virtual ~Ray_GeyserPlatformAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onDepthChanged( f32 _oldZ, f32 _newZ );
        virtual void        onEvent( Event * _event );
        virtual void        Update( f32 _dt );
        virtual void        Draw();

        bbool               isOpen() { return m_state == State_Open; }
        void                open();
        void                close();
        Transform2d         getWorldTransform2d( const bbool _useAngleOffset = btrue ) const;
        void                getShape(PhysShapePolygon & _poly);
        
        void                setTarget( const Vec3d& _worldPos );
        Vec3d               getTarget() const;
        Transform3d         getTransform() const;

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        void                drawDebugForce( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        void                validateAngle() const;

        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
        ITF_INLINE const class Ray_GeyserPlatformAIComponent_Template* editor_getTemplate() const { return getTemplate(); }
        Vec3d               editor_getTarget() const;
        void                editor_setTarget( const Vec3d& _worldPos );
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_GeyserPlatformAIComponent_Template* getTemplate() const;

        typedef SafeArray <PolyLine*> RegionInstances;

        void                clear();
        void                postLoadProperties();

        Vec2d               getScale() const;
        Transform2d         getLocalTransform2d( const bbool _useFlip = btrue ) const;
        void                getPosAndAngleLocal(Vec2d& _pos, f32& _angle) const;
        Transform2d         getLocalTransform2d_old() const;
        f32                 getForceLength() const;

        void                forceOpen();
        void                forceClose();
        void                processHit( HitStim* _hit );
        void                anticipateOpen();

        bbool               useWindTunnel() const;
        bbool               useForce() const;
        bbool               useReachPlatform() const;
        bbool               needUpdatePhysForceModifier() const { return useForce() || useWindTunnel() || useReachPlatform(); }
        void                updateForceOrWindTunnel( f32 _dt );

        void                growAABB();
        void                updateLength( f32 _dt );
        void                updatePatch( f32 _dt );
        void                updateForce( f32 _dt );
        void                updateWindTunnel( f32 _dt );
        void                updatePolyline( f32 _dt );
        void                updatePolyline_Procedural( f32 _dt );
        void                updatePolyline_Anim( f32 _dt );
        void                updateStim( f32 _dt );
        void                updateRegions( f32 _dt );
        void                updateReachPlatform( f32 _dt );

        bbool               getReachPlatformData( ObjectRef& _polyRef ) const;

        void                drawPatch();
#ifdef ITF_SUPPORT_DEBUGFEATURE
        void                drawDebugPatch( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

        void                processQueryCanJump( Ray_EventQueryCanJumpFromCurrent* _query );
        void                processQueryWindTunnelInfo( class Ray_EventQueryWindTunnelInfo* _query );

        void                registerObjects( bbool _val );
        void                createRegions();

        enum State
        {
            State_Closed,
            State_Open,
        };

        State               m_state;
        bbool               m_startOpen;

        f32                 m_angleOffset_instance; // instance-serialized
        f32                 m_length_instance;  // instance-serialized
        f32                 m_length_target;    // used for grow/shrink
        f32                 m_length_current;   // used for grow/shrink
        f32                 m_lengthRatio;      // used for alpha
        f32                 m_length;           // the actual length, with sin variation (NOT scaled, we assume m_length_instance is given already scaled)
        f32                 m_prevLength;       // used for fxOnClose

        Vec2d               m_patchStart;
        Vec2d               m_patchEnd;
        Vec2d               m_patchNormal;

        ResourceID          m_texture;
        BezierPatch         m_patch;
        f32                 m_timer;

        // procedural platform
        ProceduralPolyline  m_polyline;
        SafeArray<Vec2d>    m_polylinePoints;
        // anim platform
        u32                 m_platformBoneIndex;
        PolylineComponent*  m_polylineComponent;


        class User
        {
        public:
            User()
                : m_fxHandle(U32_INVALID)
                , m_lastFramePresent(0)
            {}

            ActorRef m_actor;
            u32 m_fxHandle;
            u32 m_lastFramePresent;
        };
        typedef SafeArray<User> UserList;

        UserList                m_users;

        void                    registerUser( ActorRef _userActor );
        void                    updateUsers( f32 _dt );


        FXControllerComponent*  m_fxController;
        bbool                   m_fxDirty;
        u32                     m_geyserFxHandle;

        Vec2d                   getFxSize() const;
        Transform3d             getFxTransform() const;
        void                    updateFX( f32 _dt );
        void                    initFxGenerators();
        void                    initFxGenerator_box( ITF_ParticleGenerator* _generator, const f32 _length, const f32 _width );
        void                    updateFxGenerators();
        void                    updateFxGenerator_ballistic( ITF_ParticleGenerator* _generator, const f32 _length );
        void                    updateUsersFX();
#ifdef ITF_SUPPORT_DEBUGFEATURE
        void                    drawDebugFX() const;
        void                    drawDebugFxGenerator_box( ITF_ParticleGenerator* _generator, const f32 _length, const f32 _width ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE

        PhysForceModifier_Instance m_physModifier;
        FixedArray<Vec2d, 4>    m_physModifierPoints;
        bbool                   m_registered;

        AnimLightComponent*     m_animComponent;
        u32                     m_attachBoneIndex;


        PhysShapePolygon m_stimShape;

        ActorRefList            m_windTunnelsActorsInside;
        Vec2d                   m_windTunnelStartPoint;
        Vec2d                   m_windTunnelExitPoint;

        RegionInstances         m_regions;


        void                    updateAutoClose( f32 _dt );
        f32                     m_triggerOnHitTimer;
        f32                     m_autoCloseTimer;
        ActorRefList            m_jumpToActors;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_GeyserPlatformAIComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GeyserPlatformAIComponent_Template, GraphicComponent_Template,2991729266);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_GeyserPlatformAIComponent);

    public:

        Ray_GeyserPlatformAIComponent_Template();
        virtual ~Ray_GeyserPlatformAIComponent_Template() {}


        struct RegionData
        {
            DECLARE_SERIALIZE()

            f32         m_widthOffset;
            f32         m_heightOffset;
            StringID    m_id;
            Path        m_gameMaterial;
        };

        typedef ITF_VECTOR <RegionData> RegionsList;


        enum GeneratorType
        {
            GeneratorType_Ballistic,
            GeneratorType_Box,
        };

        #define SERIALIZE_GENERATORTYPE(name,val) \
        SERIALIZE_ENUM_BEGIN(name,val); \
            SERIALIZE_ENUM_VAR(GeneratorType_Ballistic); \
            SERIALIZE_ENUM_VAR(GeneratorType_Box); \
        SERIALIZE_ENUM_END();


        const StringID& getAttachBone() const { return m_attachBone; }
        ITF_INLINE f32  getAngleOffset() const { return getRotOffset().ToRadians(); }
        ITF_INLINE bbool getIgnoreActorScale() const { return m_ignoreActorScale; }

        ITF_INLINE f32  getLength() const { return m_length; }
        ITF_INLINE bbool getLockLength() const { return m_lockLength; }
        ITF_INLINE bbool getLockAngle() const { return m_lockAngle; }
        ITF_INLINE f32  getGrowTargetSmoothFactor() const { return m_growTargetSmoothFactor; }
        ITF_INLINE f32  getGrowSmoothFactor() const { return m_growSmoothFactor; }
        ITF_INLINE f32  getShrinkTargetSmoothFactor() const { return m_shrinkTargetSmoothFactor; }
        ITF_INLINE f32  getShrinkSmoothFactor() const { return m_shrinkSmoothFactor; }
        ITF_INLINE f32  getLengthVariationAmplitude() const { return m_lengthVariationAmplitude; }
        ITF_INLINE f32  getLengthVariationFrequency() const { return m_lengthVariationFrequency; }
        ITF_INLINE f32  getJumpThreshold() const { return m_jumpThreshold; }

        ITF_INLINE bbool getTriggerOnHit() const { return m_triggerOnHit; }
        ITF_INLINE f32  getTriggerOnHitDelay() const { return m_triggerOnHitDelay; }
        ITF_INLINE bbool getChangeStateOnTrigger() const { return m_changeStateOnTrigger; }
        ITF_INLINE f32  getAutoCloseDelay() const { return m_autoCloseDelay; }
        ITF_INLINE bbool getStartOpen() const { return m_startOpen; }

        ITF_INLINE bbool getUsePlatform() const { return m_usePlatform; }
        const Path&     getPlatformMaterial() const { return m_platformMaterial; }
        ITF_INLINE f32  getPlatformWidth() const { return m_platformWidth; }
        ITF_INLINE const StringID& getPlatformBone() const { return m_platformBone; }
        ITF_INLINE const StringID& getPlatformPolyline() const { return m_platformPolyline; }
        ITF_INLINE f32  getPlatformLengthOffset() const { return m_platformLengthOffset; }
        ITF_INLINE f32  getPlatformLengthMultiplier() const { return m_platformLengthMultiplier; }
        ITF_INLINE f32  getPlatformHeightSink() const { return m_platformHeightSink; }

        const Path&     getTextureFile() const { return m_textureFile; }
        ITF_INLINE f32  getPatchStartOffset() const { return m_patchStartOffset; }
        ITF_INLINE f32  getPatchLengthOffset() const { return m_patchLengthOffset; }
        ITF_INLINE f32  getPatchLengthMultiplier() const { return m_patchLengthMultiplier; }
        f32             getPatchWidthBottom() const { return m_patchWidthBottom; }
        f32             getPatchWidthTop() const { return m_patchWidthTop; }
        ITF_INLINE bbool getPatchSwapXY() const { return m_patchSwapXY; }
        f32             getTileLength() const { return m_tileLength; }
        f32             getTileWidth() const { return m_tileWidth; }
        Vec2d           getUVScrollSpeed() const { return m_uvScrollSpeed; }
        BEZIER_PATCH_UVMODE getUvMode() const { return m_uvMode; }
        ITF_INLINE f32  getPatchFadeInLength() const { return m_patchFadeInLength; }
        ITF_INLINE f32  getPatchFadeOutLength() const { return m_patchFadeOutLength; }
        ITF_INLINE f32  getZOffset() const { return m_zOffset; }
        ITF_INLINE f32  getTessellationRatio() const { return m_tessellationRatio; }

        const PhysForceModifier& getPhysModifier() const { return m_physModifier; }
        ITF_INLINE const Vec2d& getForceDirection() const { return m_forceDirection; }
        ITF_INLINE f32  getForceLengthOffset() const { return m_forceLengthOffset; }
        ITF_INLINE f32  getForceLengthMultiplier() const { return m_forceLengthMultiplier; }
        f32             getForceWidthBottom() const { return m_forceWidthBottom; }
        f32             getForceWidthTop() const { return m_forceWidthTop; }
        ITF_INLINE f32  getForceFadeOutLength() const { return m_forceFadeOutLength; }

        const StringID& getFxName() const { return m_fxMain; }
        ITF_INLINE bbool getFxUseTransform() const { return m_fxUseTransform; }
        ITF_INLINE GeneratorType getFxGeneratorType() const { return m_fxGeneratorType; }
        const SafeArray<StringID>& getFxGenerators() const { return m_fxGenerators; }
        ITF_INLINE f32  getFxLengthOffset() const { return m_fxLengthOffset; }
        ITF_INLINE f32  getFxLengthMultiplier() const { return m_fxLengthMultiplier; }
        ITF_INLINE f32  getFxWidth() const { return m_fxWidth; }
        ITF_INLINE f32  getFxLifetimeOffset() const { return m_fxLifetimeOffset; }
        ITF_INLINE f32  getFxLifetimeMultiplier() const { return m_fxLifetimeMultiplier; }
        const StringID& getFxCollision() const { return m_fxCollision; }
        const StringID& getFxOnClose() const { return m_fxOnClose; }
        ITF_INLINE f32  getFxOnCloseThreshold() const { return m_fxOnCloseThreshold; }

        const StringID& getAnimOpened() const { return m_animOpened; }
        const StringID& getAnimClosed() const { return m_animClosed; }

        ITF_INLINE f32  getBiggestWidth() const { return m_biggestWidth; }
        ITF_INLINE bbool getUseStim() const { return m_useStim; }
        ITF_INLINE bbool getJumpToPlatform() const { return m_jumpToPlatform; }
        ITF_INLINE Faction getFaction() const { return m_faction; }
        ITF_INLINE bbool getRegisterToAIManager() const { return m_registerToAIManager; }

        ITF_INLINE u32  getWindTunnelLevel() const { return m_windTunnelLevel; }
        ITF_INLINE u32  getNumRegions() const { return m_regions.size(); }
        ITF_INLINE const RegionData& getRegion( u32 _index ) const { return m_regions[_index]; }
        ITF_INLINE bbool getIsGeyserBehavior() const { return m_geyserBehavior; }

#ifndef ITF_CONSOLE_FINAL
        ITF_INLINE f32  getMinAngle() const { return m_minAngle.ToRadians(); }
        ITF_INLINE f32  getMaxAngle() const { return m_maxAngle.ToRadians(); }

        ITF_INLINE bbool getDebugForce() const { return m_debugForce; }
        ITF_INLINE bbool getDebugPatch() const { return m_debugPatch; }
        ITF_INLINE bbool getDebugFx() const { return m_debugFx; }
#endif // ITF_CONSOLE_FINAL

        virtual void    onTemplateLoaded();

    private:

        StringID        m_attachBone;
        bbool           m_ignoreActorScale;

#ifndef ITF_CONSOLE_FINAL
        // validation
        Angle           m_minAngle;
        Angle           m_maxAngle;

        // debug
        bbool           m_debugForce;
        bbool           m_debugPatch;
        bbool           m_debugFx;
#endif // ITF_CONSOLE_FINAL

        f32             m_length;           // default; actual length comes from the instance
        bbool           m_lockLength;
        bbool           m_lockAngle;
        f32             m_growTargetSmoothFactor;
        f32             m_growSmoothFactor;
        f32             m_shrinkTargetSmoothFactor;
        f32             m_shrinkSmoothFactor;
        f32             m_lengthVariationAmplitude;
        f32             m_lengthVariationFrequency;

        f32             m_jumpThreshold;    // % - if player is above this height, he can jump again

        bbool           m_triggerOnHit;
        f32             m_triggerOnHitDelay;
        f32             m_autoCloseDelay;   // 0 = don't auto-close
        bbool           m_startOpen;
        bbool           m_changeStateOnTrigger;

        // platform - procedural
        bbool           m_usePlatform;
        Path            m_platformMaterial;
        f32             m_platformWidth;
        // platform - anim
        StringID        m_platformBone;
        StringID        m_platformPolyline;
        // platform - common
        f32             m_platformLengthOffset;
        f32             m_platformLengthMultiplier;
        f32             m_platformHeightSink;   // how much does the platform drop down when carrying players

        Path            m_textureFile;
        f32             m_patchStartOffset;  // anchor point offset
        f32             m_patchLengthOffset;
        f32             m_patchLengthMultiplier;
        f32             m_patchWidthBottom;
        f32             m_patchWidthTop;
        bbool           m_patchSwapXY;            // if true, X becomes Y becomes X
        f32             m_tileLength;
        f32             m_tileWidth;
        Vec2d           m_uvScrollSpeed;
        BEZIER_PATCH_UVMODE m_uvMode;
        f32             m_patchFadeInLength;    // meters
        f32             m_patchFadeOutLength;
        f32             m_zOffset;
        f32             m_tessellationLength;
        f32             m_tessellationRatio;    // onTemplateLoaded

        PhysForceModifier m_physModifier;
        Vec2d           m_forceDirection;
        f32             m_forceWidthBottom;
        f32             m_forceWidthTop;
        f32             m_forceLengthOffset;
        f32             m_forceLengthMultiplier;
        f32             m_forceFadeOutLength;


        StringID        m_fxMain;
        bbool           m_fxUseTransform;   // transform FX with pos/angleOffset?
        GeneratorType   m_fxGeneratorType;
        SafeArray<StringID> m_fxGenerators;

        // common to both generator types
        f32             m_fxLengthOffset;
        f32             m_fxLengthMultiplier;
        // box type
        f32             m_fxWidth;
        // ballistic type
        f32             m_fxLifetimeOffset;
        f32             m_fxLifetimeMultiplier;

        StringID        m_fxCollision;
        StringID        m_fxOnClose;
        f32             m_fxOnCloseThreshold;   // meters


        StringID        m_animOpened;
        StringID        m_animClosed;

        bbool           m_useStim;
        bbool           m_jumpToPlatform;
        Faction         m_faction;
        bbool           m_registerToAIManager;

        // Wind tunnel parameters
        u32             m_windTunnelLevel;

        // Regions to spawn
        RegionsList     m_regions;

        // The geyser behavior implies: no wallslide, no crush attack
        bbool           m_geyserBehavior;


        // onTemplateLoaded
        f32             m_biggestWidth;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_GeyserPlatformAIComponent_Template* Ray_GeyserPlatformAIComponent::getTemplate() const
    {
        return static_cast<const Ray_GeyserPlatformAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_