#ifndef _ITF_RAY_MUSICSCOREAICOMPONENT_H_
#define _ITF_RAY_MUSICSCOREAICOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/GraphicComponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

namespace ITF
{
    class CurveComponent;
    class LinkComponent;
    class FXControllerComponent;

    class Ray_MusicScoreAIComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MusicScoreAIComponent, GraphicComponent,1123749061)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        // ********************************************************************

     public:

        Ray_MusicScoreAIComponent();
        virtual ~Ray_MusicScoreAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const   { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onBecomeActive();
        virtual void        Update( f32 _dt );
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event * _event );              
        virtual void        Draw();

                f32         getNoteInterval(u32 _indexSegment, Vec3d& _tangent3D, const Vec3d& _closetPosOut, bbool& _isBadSnap, bbool& _reverse, f32 t);
                void        getPosAtDist(Vec3d& _posOut, f32 _distOnCurve, u32& _selectedNote, f32& _intervalNote, bbool& _isBadSnap, bbool& _reverse);
                void        getClosestPos(Vec3d _refPos, Vec3d& _closetPosOut, f32& _distOnCurveOut, u32& _selectedNote, f32& _intervalNote, bbool& _isBadSnap, bbool& _reverse);
                
                void        init(bbool _isReinit = bfalse);
                void        forceOpen();

    private:

                void        growAABB();
                void        receiveEvent();
                void        open();
                void        close();
                void        updateDeploy( f32 _dt );

                void        sendMessageToChilds( u32 _messageType );
                void        registerOnChilds();
                void        updateOpeningForChild(bbool _forced);
                void        updateClosingForChild();
                void        updatePosForChild();

                void        playSound(const StringID& _sound);

    private:

        ITF_INLINE const class Ray_MusicScoreAIComponent_Template*  getTemplate() const;

        CurveComponent* m_curveComponent;
        FXControllerComponent* m_fxControllerComponent;

        f32             m_deployCurDist;
        u32             m_state;
    
        ResourceID      m_texture;


        struct SegmentMusic
        {
            SegmentMusic()
            : m_widthStart(1.0f)
            , m_widthEnd(1.0f)
            {
                // none
            }

            BezierPatch     m_patch;
            f32             m_widthStart;
            f32             m_widthEnd;
        };

        ITF::Vector<SegmentMusic> m_listSegment;

        f32             m_timer;
        
        bbool           m_startOpen;
        bbool           m_registered;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_MusicScoreAIComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MusicScoreAIComponent_Template, GraphicComponent_Template,3374490094);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MusicScoreAIComponent);

    public:

        Ray_MusicScoreAIComponent_Template();
        ~Ray_MusicScoreAIComponent_Template();

        ITF_INLINE const Path&     getTextureFile() const           { return m_textureFile; }
        ITF_INLINE f32             getTileLength() const            { return m_tileLength; }

        ITF_INLINE f32             getWidthStart() const            { return m_widthStart; }
        ITF_INLINE f32             getWidthBase() const             { return m_widthBase; }
        ITF_INLINE f32             getAmplitudeSin() const          { return m_amplitudeSin; }
        ITF_INLINE f32             getFrequencySin() const          { return m_frequencySin; }

        ITF_INLINE f32             getOpeningSpeed() const          { return m_openingSpeed; }
        ITF_INLINE bbool           getDrawUvFromRoot() const        { return m_drawUvFromRoot; }

        ITF_INLINE f32             getIntervalNotePNG() const       { return m_intervalNotePNG; }

        // Sound
        ITF_INLINE const StringID&  getOpenSoundID() const          { return m_openSound; }
        ITF_INLINE const StringID&  getCloseSoundID() const         { return m_closeSound; }


    private:

        // patch params
        Path  m_textureFile;
        f32   m_tileLength;

        // Variation
        f32   m_widthStart;
        f32   m_widthBase;
        f32   m_amplitudeSin;
        f32   m_frequencySin;

        // compo params
        f32   m_openingSpeed;
        bbool m_drawUvFromRoot;

        f32   m_intervalNotePNG;

        //Sound
        StringID  m_openSound;
        StringID  m_closeSound;


    };


    const Ray_MusicScoreAIComponent_Template*  Ray_MusicScoreAIComponent::getTemplate() const {return static_cast<const Ray_MusicScoreAIComponent_Template*>(m_template);}
};

#endif //_ITF_RAY_MUSICSCOREAICOMPONENT_H_
