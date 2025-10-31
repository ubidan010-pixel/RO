#ifndef _ITF_RAY_FIREPATCHAICOMPONENT_H_
#define _ITF_RAY_FIREPATCHAICOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/GraphicComponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERCURVECOMPONENT_H_
#include "gameplay/Components/Misc/BezierCurveComponent.h"
#endif //_ITF_BEZIERCURVECOMPONENT_H_

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_


#define STATE_CLOSED    0
#define STATE_OPENING   1
#define STATE_OPEN      2
#define STATE_CLOSING   3

#define PATCH_OFF       0
#define PATCH_ON        1
#define PATCH_OPENING   2
#define PATCH_CLOSING   3

#define MODE_BOTH_SIDE  0
#define MODE_LEFT_SIDE  1
#define MODE_RIGHT_SIDE 2

namespace ITF
{  

    //*****************************************************************************
    // Ray_FirePatchAIComponent
    //*****************************************************************************

    class FXControllerComponent;

    class Ray_FirePatchAIComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FirePatchAIComponent, GraphicComponent,838986340)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        // ********************************************************************
     
    private:

        struct TextureAnim
        {

            TextureAnim()
                : m_useAtlas(bfalse)
                , m_countAtlasUv(0)
            {
                // none
            }

            ResourceID m_textureID;
            bbool m_useAtlas;
            u32 m_countAtlasUv;

        };

        struct SubPatch
        {
            SubPatch()
                : m_distStart(0.0f)
                , m_length(1.0f)
                , m_width(1.0f)
                , m_offset(0.0f)
                , m_zOffset(0.0f)
                , m_curIndexAtlas(0)
                , m_flipped(bfalse)
                , m_isSidePatch(bfalse)
                , m_state(PATCH_OFF)
                , m_modTile(2)
                , m_texAnim(NULL)
            {
                // none
            }

            // Patch
            BezierPatch m_patch;

            // Pos
            f32 m_distStart;
            f32 m_length;
            f32 m_width;
            f32 m_offset;
            f32 m_zOffset;
            bbool m_flipped;
            bbool m_isSidePatch;



            // Texture Annim
            TextureAnim* m_texAnim;
            u32 m_curIndexAtlas;
            u32 m_modTile;

            // State
            u32 m_state;

			// AABB$
			AABB m_aabb;

        };


     public:

        Ray_FirePatchAIComponent();
        virtual ~Ray_FirePatchAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const   { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        Update( f32 _dt );

        virtual void        onEvent( Event * _event );

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        
        virtual void        Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* , u32 _flags ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
 
    private :

            void            initTexture();
            void            initPathesList();

            void            receiveEvent(bbool _open);
            void            updatePatchSide();
            void            updatePatchSmoke();
            void            updateStim();
            void            calculateStimShape( const Transform3d* _transform = NULL );

            void            startSound();
            void            stopSound();
            void            moveSound(Vec3d _pos);

            void            drawSubPatch(const BezierCurve& _curve, SubPatch& _subPatch, f32 _zOffset, bbool _flipped = bfalse, const Transform3d* _transform = NULL );
            void            getUVDatabyIndex(Texture* _texture, u32 _index, Vec2d& _uv0, Vec2d& _uv1);

            void            growAABB( const Transform3d* _transform = NULL);

    protected:
            virtual BezierCurve &   getBezierCurve();
            u32 m_state;

    private:

        ITF_INLINE const class Ray_FirePatchAIComponent_Template*  getTemplate() const;

        bbool          m_init;
        ResourceID     m_texture;
        u32            m_countTrame;

        FXControllerComponent*  m_fxControllerComponent;
        BezierCurveComponent*  m_bezierCurveComponent;

        // Texture Animation
        TextureAnim m_textureAnimSide;
        TextureAnim m_textureAnimSmoke;

        ITF_VECTOR<SubPatch> m_patchesSide;
        ITF_VECTOR<SubPatch> m_patchesSmoke;

        Color m_color;
        Color m_fogColor;
        BEZIER_PATCH_UVMODE m_uvMode;
        f32 m_tessellationRatio;

        f32 m_distOpening;
        f32 m_distClosing;

        // Stim
        PhysShapePolygon m_stimShape;
        PhysShapePolygon::PointsList m_pointsList;

        // Fx / Sound
        u32 m_handleSound;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_FirePatchAIComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FirePatchAIComponent_Template, GraphicComponent_Template,1625357308);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FirePatchAIComponent);

    public:

        Ray_FirePatchAIComponent_Template();
        ~Ray_FirePatchAIComponent_Template();


        ITF_INLINE f32 getSidePatchLength() const { return m_sidePatchLength; }
        ITF_INLINE f32 getSidePatchWidth() const { return m_sidePatchWidth; }

        ITF_INLINE f32 getSpaceMinBetween2Start() const { return m_spaceMinBetween2Start; }
        ITF_INLINE f32 getSpaceMaxBetween2Start() const { return m_spaceMaxBetween2Start; }

        ITF_INLINE f32 getOffsetMin() const { return m_offsetMin; }
        ITF_INLINE f32 getOffsetMax() const { return m_offsetMax; }

        ITF_INLINE f32 getScaleMin() const { return m_scaleMin; }
        ITF_INLINE f32 getScaleMax() const { return m_scaleMax; }
        
        ITF_INLINE u32 getModTileMin() const { return m_modTileMin; }
        ITF_INLINE u32 getModTileMax() const { return m_modTileMax; }

        ITF_INLINE u32 getStartOpen() const { return m_startOpen; }
        ITF_INLINE u32 getEndOpen() const { return m_endOpen; }
        ITF_INLINE u32 getStartLoop() const { return m_startLoop; }
        ITF_INLINE u32 getEndLoop() const { return m_endLoop; }
        ITF_INLINE u32 getStartClose() const { return m_startClose; }
        ITF_INLINE u32 getEndClose() const { return m_endClose; }


        ITF_INLINE u32 getSmokeStartOpen() const { return m_smokeStartOpen; }
        ITF_INLINE u32 getSmokeEndOpen() const { return m_smokeEndOpen; }
        ITF_INLINE u32 getSmokeStartLoop() const { return m_smokeStartLoop; }
        ITF_INLINE u32 getSmokeEndLoop() const { return m_smokeEndLoop; }
        ITF_INLINE u32 getSmokeStartClose() const { return m_smokeStartClose; }
        ITF_INLINE u32 getSmokeEndClose() const { return m_smokeEndClose; }


        ITF_INLINE const Path& getTextureSideFile() const { return m_textureSideFile; }
        ITF_INLINE const Path& getTextureSmokeFile() const { return m_textureSmokeFile; }

        ITF_INLINE f32 getTessellationLength() const { return m_tessellationLength; }
        ITF_INLINE f32 getOpenSpeed() const { return m_openSpeed; }
        ITF_INLINE ITF::Faction     getFaction() const  { return m_faction; }
        ITF_INLINE f32 getStimBezierLength() const { return m_stimBezierLength; }
        ITF_INLINE f32 getStimBezierWidth() const { return m_stimBezierWidth; }
        ITF_INLINE f32 getDistSafeHead() const { return m_distSafeHead; }
        ITF_INLINE f32 getStimOffsetTail() const { return m_stimOffsetTail; }

        ITF_INLINE u32 getSideMode() const { return m_sideMode; }

        ITF_INLINE f32 getZOffset() const { return m_zOffset; }

        ITF_INLINE f32 getSmokeForwardOffset() const { return m_smokeForwardOffset; }
        ITF_INLINE f32 getSmokeBackwardOffset() const { return m_smokeBackwardOffset; }

        ITF_INLINE bbool getUseSmoke() const { return m_useSmoke; }
        ITF_INLINE StringID getStartMarker() const { return m_startMarker; }
        ITF_INLINE StringID getStopMarker() const { return m_stopMarker; }
        ITF_INLINE bbool getStopOnHitSuccessful() const { return m_stopOnHitSuccessful; }
        ITF_INLINE bbool getStopDeployOnStop() const { return m_stopDeployOnStop; }

		ITF_INLINE bbool getUseOptimAABB() const { return m_useOptimAABB; }


    private:


        f32 m_sidePatchLength;
        f32 m_sidePatchWidth;

        f32 m_spaceMinBetween2Start;
        f32 m_spaceMaxBetween2Start;

        f32 m_offsetMin;
        f32 m_offsetMax;

        f32 m_scaleMin;
        f32 m_scaleMax;

        u32 m_modTileMin;
        u32 m_modTileMax;

        Path m_textureSideFile;
        Path m_textureSmokeFile;

        f32 m_tessellationLength;

        // Animation Fire
        u32 m_startOpen;
        u32 m_endOpen;
        u32 m_startLoop;
        u32 m_endLoop;
        u32 m_startClose;
        u32 m_endClose;
 
        // Animation Smoke
        u32 m_smokeStartOpen;
        u32 m_smokeEndOpen;
        u32 m_smokeStartLoop;
        u32 m_smokeEndLoop;
        u32 m_smokeStartClose;
        u32 m_smokeEndClose;

        bbool m_useSmoke;
        f32 m_smokeForwardOffset;
        f32 m_smokeBackwardOffset;

        f32 m_openSpeed;

        Faction m_faction;

        // Stim
        f32 m_stimBezierLength;
        f32 m_stimBezierWidth;
        f32 m_distSafeHead;
        f32 m_stimOffsetTail;

        u32 m_sideMode;

        f32 m_zOffset;

        StringID m_startMarker;
        StringID m_stopMarker;
        bbool m_stopOnHitSuccessful;
        bbool m_stopDeployOnStop;

		bbool m_useOptimAABB;
    };


    const Ray_FirePatchAIComponent_Template*  Ray_FirePatchAIComponent::getTemplate() const {return static_cast<const Ray_FirePatchAIComponent_Template*>(m_template);}
};

#endif //_ITF_RAY_FIREPATCHAICOMPONENT_H_
