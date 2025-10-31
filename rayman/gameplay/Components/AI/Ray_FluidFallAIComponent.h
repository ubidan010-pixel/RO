#ifndef _ITF_RAY_FLUIDFALLAICOMPONENT_H_
#define _ITF_RAY_FLUIDFALLAICOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/GraphicComponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_

namespace ITF
{
    class FXControllerComponent;
    class CurveComponent;
    class LinkComponent;
    class FXControllerComponent;

    class Ray_FluidFallAIComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FluidFallAIComponent, GraphicComponent,2143327929)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        // ********************************************************************

     public:

        class FluidFallProgress
        {

        public:

            struct SegmentCurve
            {
                SegmentCurve()
                    : m_widthStart(1.0f)
                    , m_widthEnd(1.0f)
                {
                    // none
                }

                BezierPatch     m_patch;
                f32             m_widthStart;
                f32             m_widthEnd;
            };

            FluidFallProgress();
            ~FluidFallProgress() 
            {
                m_stimShape.clear();
                m_pointsList.clear();
                m_listSegment.clear();
                //m_polyline.clear();

            }
      
            void init(CurveComponent* _curveCompo, bbool _startOpen, f32 _widthStart, f32 _widthBase);
          

            f32             m_deployCurDist;
            f32             m_deployCurDistFluid;
            f32             m_deployCurDistAnnunFX;

            f32             m_deployCurOrigin;
            u32             m_state;
            bbool           m_patchClosing;
            f32             m_currentTotalLength;
            f32             m_timer;

            // FXs
            u32 m_handleAnnunciatorFx;
            u32 m_handleAnnunciatorSound;
            f32 m_timeAnnunciationFx;

            u32 m_handleStartFx;
            u32 m_handleEndFx;

            // Sound
            u32 m_handleStartSound;


            PhysShapePolygon m_stimShape;
            PhysShapePolygon::PointsList m_pointsList;
            ITF::Vector<SegmentCurve> m_listSegment;
            ProceduralPolyline m_polyline;
        };

     public:

        Ray_FluidFallAIComponent();
        virtual ~Ray_FluidFallAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const   { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();

        virtual void        onDepthChanged( f32 _oldZ, f32 _newZ );
        virtual void        Update( f32 _dt );


        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event * _event );              
        virtual void        Draw();

        virtual void        onActorClearComponents();

                void        init(bbool _isReinit = bfalse);
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* , u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
                struct   PatchData
                {
                    u32 index;
                    f32 length;
                    BezierPatchParams params;
                };
                void        growAABB();
                void        receiveEvent(bbool _open);

                void        startAnnonce(FluidFallProgress& _ffp);
                void        open( FluidFallProgress& _ffp );
                void        close( FluidFallProgress& _ffp );
                void        forceOpen(bbool _startOpen);

                void        updateDeploy( FluidFallProgress& _ffp, f32 _dt );
                void        updateStim( FluidFallProgress& ffp );
                void        updatePolyline( FluidFallProgress& ffp, f32 _dt );

                void        DrawOpen( FluidFallProgress& _ffp );
                void        DrawDeploy( FluidFallProgress& _ffp );

                f32         getPatchAlpha(f32 _pos, f32 _totalLength, bbool _isOpening, f32 _startLength);
                void        calculateAlphaAndDraw( FluidFallProgress& _ffp, ITF_VECTOR<PatchData> &paramsArray, f32 totalLength );
                void        calculateStimShape( FluidFallProgress& _ffp, ITF_VECTOR<PatchData> &paramsArray, f32 totalLength );
                void        calculatePolyline( FluidFallProgress& _ffp, ITF_VECTOR<PatchData> &paramsArray, f32 totalLength );

                u32         playFx(StringID _name, Vec3d _pos);
                void        setFxPos(u32 _handle, Vec3d _pos);
                void        setFxAngle(u32 _handle, f32 _angle);
                void        stopFX(u32 _handle);
    
                void        getUVDatabyIndex(u32 _index, Vec2d& _uv0, Vec2d& _uv1);

                void        registerObjects();
                void        unregisterObjects();


    private:

        ITF_INLINE const class Ray_FluidFallAIComponent_Template*  getTemplate() const;

        CurveComponent*         m_curveComponent;
        FXControllerComponent*  m_fxControllerComponent;

        u32             m_stateGlobal;
        
        // Ressource texture/atlas
        ResourceID      m_texture;
        u32             m_curIndexAtlas;
        u32             m_countTrame;
        bbool           m_useAtlas;
        u32             m_countAtlasUv;


        // Members SERIALIZE
        bbool           m_startOpen;
        f32             m_widthStart;
        f32             m_widthBase;
        f32             m_speed;

        BEZIER_PATCH_UVMODE m_uvMode;

        ITF_VECTOR<FluidFallProgress*> m_listFluidProgress;

        bbool           m_registered;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_FluidFallAIComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FluidFallAIComponent_Template, GraphicComponent_Template,1727238213);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FluidFallAIComponent);

    public:

        Ray_FluidFallAIComponent_Template();
        ~Ray_FluidFallAIComponent_Template();

        ITF_INLINE const Path&      getTextureFile() const              { return m_textureFile; }
        ITF_INLINE f32              getTileLength() const               { return m_tileLength; }
        ITF_INLINE f32              getTessellationRatio() const        { return m_tessellationRatio; }
        ITF_INLINE bbool            getDrawUvFromRoot() const           { return m_drawUvFromRoot; }
        ITF_INLINE f32              getStimBezierLength() const         { return m_stimBezierLength; }
        ITF_INLINE f32              getStimBezierWidthFactor() const    { return m_stimBezierWidthFactor; }
        ITF_INLINE f32              getAlphaLength() const              { return m_alphaLength; }
        ITF_INLINE ITF::bbool       getUseStim() const                  { return m_useStim; }
        ITF_INLINE ITF::bbool       getUsePolyline() const              { return m_usePolyline; }
        ITF_INLINE ITF::Faction     getFaction() const                  { return m_faction; }
        ITF_INLINE f32              getPolylineLength() const           { return m_polylineLength; }
        ITF_INLINE const Path &     getGameMaterial() const             { return m_gameMaterial; }

        ITF_INLINE f32              getDurationAnnunciationFx() const   { return m_durationAnnunciationFx; }
        ITF_INLINE bbool            getUseAnnunciatorFx() const         { return m_useAnnunciatorFx; }

        ITF_INLINE u32              getModTileTexture() const           { return m_modTileTexture; }


        ITF_INLINE StringID         getFxNameAnnunciator() const        { return m_fxNameAnnunciator; }
        ITF_INLINE StringID         getFxNameStartFFP() const           { return m_fxNameStartFFP; }
        ITF_INLINE StringID         getFxNameEndFFP() const             { return m_fxNameEndFFP; }

        ITF_INLINE StringID         getSoundNameStartFFP() const        { return m_soundNameStartFFP; }
        ITF_INLINE StringID         getSoundNameAnnunciatorFFP() const  { return m_soundNameAnnunciatorFFP; }

        ITF_INLINE f32              getZOffset() const                  { return m_zOffset; }
        ITF_INLINE bbool            getIgnoreEventTrigger() const       { return m_ignoreEventTrigger; }

        virtual void onTemplateLoaded();

    private:

        // Patch params
        Path  m_textureFile;
        f32   m_tileLength;
        f32   m_tessellationLength; // 0 to max out
        f32   m_tessellationRatio;  // onTemplateLoaded()
        u32   m_modTileTexture;

        // Compo params
        bbool   m_drawUvFromRoot;
        bbool   m_useStim;
        bbool   m_usePolyline;
        f32     m_polylineLength;
        Path    m_gameMaterial;
        f32     m_stimBezierLength;
        f32     m_stimBezierWidthFactor;
        f32     m_alphaLength;
        Faction m_faction;

        // Annunciator FX
        bbool    m_useAnnunciatorFx;
        f32      m_durationAnnunciationFx;
        
        StringID m_fxNameAnnunciator;
        StringID m_fxNameStartFFP;
        StringID m_fxNameEndFFP;

        StringID m_soundNameStartFFP;
        StringID m_soundNameAnnunciatorFFP;

        f32      m_zOffset;
        bbool    m_ignoreEventTrigger;


    };


    const Ray_FluidFallAIComponent_Template*  Ray_FluidFallAIComponent::getTemplate() const {return static_cast<const Ray_FluidFallAIComponent_Template*>(m_template);}
};

#endif //_ITF_RAY_FLUIDFALLAICOMPONENT_H_
