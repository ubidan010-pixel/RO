
#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#define _ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_SUBANIMLIGHTSET_H_
#include    "engine/animation/SubAnimSet.h"
#endif //_ITF_SUBANIMLIGHTSET_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_SUBANIMFRAMEINFO_H_
#include "engine/animation/SubAnimFrameInfo.h"
#endif //_ITF_SUBANIMFRAMEINFO_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#define MAX_EVENTPROCESSCONTAINER_EVENTS    10

namespace ITF
{
    class AnimScene;
    class AnimBoneDyn;
    class AnimMeshScene;

	struct BoneMapping
	{
		DECLARE_SERIALIZE()
		StringID m_inputBone;
		StringID m_outputBone;
	};
    VECTAGSET(BoneMapping, ITF::MemoryId::mId_VectorAnimation)

    struct AnimLightFrameInfo
    {
        DECLARE_SERIALIZE()

        AnimLightFrameInfo() : m_weight(1.f), m_usePatches(btrue) {}
        AnimLightFrameInfo( const SubAnimFrameInfo& _subAnimFrameInfo, f32 _weight, bbool _usePatches ) : m_subAnimFrameInfo(_subAnimFrameInfo), m_weight(_weight), m_usePatches(_usePatches) {}

        SubAnimFrameInfo        m_subAnimFrameInfo;
        f32                     m_weight;
        bbool                   m_usePatches;
    };

    typedef SafeArray<AnimLightFrameInfo> AnimLightFrameInfoArray;

    class AnimLightComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimLightComponent,GraphicComponent,2800021434);

    public:
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        AnimLightComponent();
        ~AnimLightComponent();

        ITF_INLINE const class AnimLightComponent_Template* getTemplate() const;

        virtual bbool                   needsUpdate() const { return btrue; }
        ITF_INLINE virtual bbool        needsDraw() const;
        ITF_INLINE virtual bbool        needsDraw2D() const;

        virtual void                    Update( f32 _deltaTime );
        void                            forceSynchronousFrame();
        virtual void                    Draw();
        virtual void                    Draw2D();
        virtual void                    onEvent( Event* _event );

#ifdef ITF_SUPPORT_EDITOR
        virtual void                    onEditorMove(bbool _modifyInitialPos = btrue);
        ITF_INLINE f32                  editor_getSyncOffset() const { return m_syncOffset; }
        ITF_INLINE void                 editor_setSyncOffset( const f32 _value ) { m_syncOffset = _value; m_subAnimSet.setGlobalSyncOffset(m_syncOffset); }
#endif // ITF_SUPPORT_EDITOR
        
        virtual void                    onUnloadResources();
        bbool                           isLoaded() const;
        virtual void                    onResourceLoaded();


        ITF_INLINE AnimMeshScene*       getAnimMeshScene() const { return m_AnimMeshScene; }
        void                            deleteAnimMesh() { SF_DEL(m_AnimMeshScene); }

        void                            setPlayRate( f32 _rate );

        virtual void                    onActorLoaded(Pickable::HotReloadType /*_hotReload*/) ;
        virtual void                    onCheckpointLoaded();

        i32                             getBoneIndex (const StringID& _boneName);
        bbool                           getBonePos (u32 _boneIndex, Vec2d& _dest, bbool _getBoneEnd = bfalse) const;
        bbool                           getBonePos (u32 _boneIndex, Vec3d& _dest, bbool _getBoneEnd = bfalse) const;
        bbool                           getBoneOrientation (u32 _boneIndex, Vec2d& _orientation) const;
        bbool                           getBoneScale (u32 boneIndex, Vec2d& _dest) const;
        AnimBoneDyn*                    getBone(u32 boneIndex) const;
        u32                             getBoneCount() const;

        AnimPolyline *                  getCurrentPolyline(const StringID & _polyId, bbool _useWorking = bfalse) const;
        Vec2d *                         getCurrentPolylinePointBuffer(const StringID & _polyId, u32 * _size = NULL, AnimPolyline::PolylineType * _type = NULL) const;
        Vec2d *                         getCurrentPolylinePoint(const StringID & _polyId, const StringID & _polyPointName, u32 * _index = NULL) const;
        Vec2d *                         getCurrentFirstPolylinePoint(const StringID & _polyPointName, class AnimPolyline ** _polyline = NULL, u32 * _index = NULL) const;

        void                            addLight(const LightInfo &_light);
        void                            clearLights();
        virtual AABB                    getVisualAABB() const;
        void                            fillAnimsAABBLocal(AABB &animAABB) const;
        AABB                            getAnimsAABB() const;

        bbool                           isDrawEnabled() const { return m_drawEnabled;}
        void                            enableDraw(bbool _enable) { m_drawEnabled = _enable;}
        ITF_INLINE bbool                getDraw2D() const;
        virtual void                    updateAABB();
        AABB                            getPatchAABB() const;
        virtual void                    updateAnimTime( f32 _dt );
        virtual void                    updatePatches();
        virtual bool                    updateData();
        virtual bool                    postUpdateData();
        void                            updateEvents();

        bbool                           isProcedural() const { return m_isProcedural; }
        ITF_INLINE virtual bbool        isSynchronous() const;
        void                            setIsProcedural( bbool _val ) { m_isProcedural = _val; }

        void                            setUpdateAnimTime( bbool _val ) { m_updateAnimTime = _val; }
        bbool                           getUpdateAnimTime() const { return m_updateAnimTime; }

        //////// SubAnimSet indirections
        virtual void                    setAnim(const StringID & _subAnim, u32 _blendFrames = U32_INVALID);
        u32                             getSubAnimCount() const                                              { return m_subAnimSet.getSubAnimCount(); }
        SubAnim*                        getSubAnim(int _index) const                                        { return m_subAnimSet.getSubAnim(_index); }
        SubAnim*                        getSubAnim(const StringID &_subAnimName) const                      { return m_subAnimSet.getSubAnim(_subAnimName); }

        void                            setCurFrame(f32 _frame, AnimEventContainer* _eventList);
        void                            setCurFrameFromStart(f32 _frame, AnimEventContainer* _evLst);
        virtual void                    resetCurTime();
        void                            setProceduralCursor(f32 _cursor);
        f32                             getCurTime( u32 _index = 0 ) const;
        void                            setCurTime( f32 _time, u32 _index = 0 );

        virtual bbool                   isAnimToPlayFinished() const { return !m_playAnimName.isValid() && isSubAnimFinished(); }
        bbool                           isSubAnimFinished() const;
        bbool                           isAnimLooped() const;
        void                            setAnimLoop(bbool _loop);
        void                            computeVisiblePrediction();
        ITF_INLINE bbool                isVisiblePrediction() const { return m_isVisiblePrediction; }
        bbool                           isBoneDataOk(i32 _boneIndex) const;
        void                            setDisableVisiblePrediction( bbool _val ) { m_disableVisiblePrediction = _val; }
        ITF_INLINE bbool                getDisableVisiblePrediction() const;
        void                            updatePatchAndAABB();

        u32                             getNumPlayingSubAnims() const                                        { return m_currentFrameSubAnims.size(); }
        ITF_INLINE const SubAnim*       getCurSubAnim( u32 _index ) const                                    { return m_currentFrameSubAnims[_index].m_subAnimFrameInfo.getSubAnim(); }

        void                            setColor(Color _color)  { m_currentColor = _color;}
        ITF_INLINE void                 resetColor();
        ITF_INLINE  const Color&        getColor() const {return m_currentColor;}
        void                            processColor();

        SubAnimSet *                    getSubAnimSet()                                                     { return &m_subAnimSet; }

        ITF_INLINE   i32                getVertexFormat() {return m_vertexFormat;}

        Vec3d                           getOffsettedPos() const;
        f32                             getOffsettedAngle() const;

        bbool                            getIsLooped() const { return m_isLooped; }
        bbool                            getUseRootRotation() const { return m_useRootRotation; }
        bbool                            getDefaultAnimFlip() const { return m_animDefaultFlip; }
        const ITF_VECTOR<AnimFrameInfo>& getAnimFrameInfo() const { return m_animFrameInfo; }

        u32                             getNumAnimsPlaying() const { return m_currentFrameSubAnims.size(); }
        void                            dbgGetCurAnims( u32 _index, String8 & _text);

        void                            setForceDirectUpdate(bbool _val, bbool _semiForced = bfalse) { if (_val) m_forceDirectUpdate++; else m_forceDirectUpdate--; ITF_ASSERT(m_forceDirectUpdate >= 0); m_semiForcedDirectUpdate = _semiForced; }
        void                            resetSubAnimsToPlay();
        i32                             addSubAnimToPlay( const StringID & _subAnim, f32 _weight, bbool _usePatches);
        void                            commitSubAnimsToPlay();
        void                            changeSubAnimToPlay(u32 _subAnimIndex, const StringID & _subAnim, f32 _weight, bbool _usePatches);
        void                            setFrameForSubAnim(u32 _subAnimIndex, f32 _time);


        void                            setBankState(u32 _state);
        bbool                           canGetRootDelta() const;
        Vec2d                           getRootDelta();
        Vec2d                           getRootDeltaCumulated( bbool _useFloatingFrame );

        ITF_INLINE bbool                getCanonizeTransitions() const;
        ITF_INLINE const Path&          getAnimPath() const;
        ITF_INLINE const StringID&      getDefaultAnimation() const;
        ITF_INLINE const Color&         getDefaultColor() const;

        ITF_INLINE virtual bbool        getUseBase() const;
        ITF_INLINE void                 setAnimInstance( const Path& _animInstance ) { m_animInstance = _animInstance; }
        void                            addAnimByPath(const String & path, const String & name, i32 beginMarker, i32 endMarker);

		const StringID&					getBoneFromGeneric(const StringID& _bone) const {ITF_MAP<StringID,StringID>::const_iterator it = m_boneMap.find(_bone); return it != m_boneMap.end() ? it->second : _bone;}

        void                            forceUpdateFlip();
        ITF_INLINE f32 getScaleMultiplier() const { return m_scaleMultiplier; }
        ITF_INLINE void setScaleMultiplier(f32 val) { m_scaleMultiplier = val; }

        ITF_INLINE void                 setRank2D(u32 rank) { m_rank2D = rank; }
        ITF_INLINE u32                  getRank2D() const { return m_rank2D; }

        ITF_INLINE const Vec2d&         getInstancePositionOffset() const {return m_instancePositionOffset;} //instance position offset is an offset which is added to the one in getTemplate()
        ITF_INLINE void                 setInstancePositionOffset(const Vec2d &_offset) {m_instancePositionOffset = _offset;}

        Vec2d                           getOffset() const;

    protected:
        void                            createAnimMeshScene();
        i32                             addSubAnimToPlay( const SubAnimFrameInfo& _subAnim, f32 _weight, bbool _usePatches );
        void                            resetSubAnimFrameInfoFlags();
        void                            updateSubAnimFrameInfoFlag(const SubAnimFrameInfo& _subAnimFrame);
        void                            setCurrentBlendFrames( u32 _blendFrames );

        virtual void                    setFirstAnim();
        virtual void                    processAnim();
        void                            processEventReset( class EventReset* _resetEvent );
        void                            processEventShow( class EventShow* _eventShow );
        void                            processEventActivate( class EventActivate* _eventActivate );

        void                            commitPlayAnim();

        bbool                           isLookingRight() const;
        Vec2d                           getScale()const{ return m_actor->getScale() * m_scaleMultiplier; }

        Path                            m_animInstance;
        AnimEventContainer              m_frameEvents;
        AnimLightFrameInfoArray         m_currentFrameSubAnims;
        ITF_VECTOR<AnimFrameInfo>       m_animFrameInfo;
        SafeArray<SubAnim*>             m_newSubAnims;
        SubAnimSet                      m_subAnimSet;
        int                             m_vertexFormat;
        bbool                           m_loadingOK;
        bbool                           m_ignoreEvents;
        AnimMeshScene *                 m_AnimMeshScene;
        ResourceID                      m_animID;
        f32                             m_currentDt;
        AABB                            m_visualAABB;
#ifndef ANIMATION_FIX_ABB
        AABB                            m_drawPredictionAABB;
#endif
        Vec2d                           m_instancePositionOffset; //This offset is added to template offset
        StringID                        m_playAnimName;
        u32                             m_playAnimNumFrames;

        bbool                           m_previousLookRight;
        u32                             m_rootIndex;
        Color                           m_currentColor;
        Color                           m_colorSrc;
        Color                           m_colorDst;
        f32                             m_currentBlendFrames;

        bbool                           m_drawEnabled;
        bbool                           m_isProcedural;
        bbool                           m_updateAnimTime;
        bbool                           m_isLooped;
        bbool                           m_useRootRotation;
        bbool                           m_animDefaultFlip;
        bbool                           m_useScissor;
        f32                             m_syncOffset;   // instance-serialized

        class JobUpdateVisual*          m_jobUpdateVisual;

        i32                             m_forceDirectUpdate; // force the use of m_currentFrameSubAnims in animLight
        bbool                           m_semiForcedDirectUpdate; // if true, animTree will not be updated, but current animation will still be updated
        bbool                           m_isVisiblePrediction;
        bbool                           m_disableVisiblePrediction;

		ITF_VECTOR<BoneMapping>			m_boneList;
		ITF_MAP<StringID,StringID>		m_boneMap;
        f32                             m_scaleMultiplier;

        u32                             m_rank2D;
    private:
        void                        clear();
        void                        postChangeProperties();
        bbool                       fillAnimInfo();
		void						fillBoneMap();
        void                        updateScissor();
    };

    ITF_INLINE void AnimLightComponent::updatePatches()
    {
        m_AnimMeshScene->m_pAnimPatches.processPatchs(m_AnimMeshScene);
    }

    class AnimLightComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimLightComponent_Template,GraphicComponent_Template,2740286289);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimLightComponent);

    public:

        AnimLightComponent_Template();
        ~AnimLightComponent_Template() {}
        
        const Path&                     getAnimPath() const { return m_animPath; }
        bbool                           getUseBase() const { return m_useBase; }
        bbool                           getSmoothAnim() const { return m_smoothAnim; }
        bbool                           getCanonizeTransitions() const { return m_canonizeTransitions; }
        u32                             getDefaultBlendFrames() const { return m_defaultBlendFrames; }
        bbool                           getDraw2d() const { return m_draw2D; }
        bbool                           getDraw2dSubScreen() const { return (m_draw2D == 2) ? btrue : bfalse; }
        const StringID&                 getVisualAABBanimID() const { return m_visualAABBanimID; }
        const SubAnimSet_Template*      getSubAnimTemplate() const { return &m_subAnimSet; }
        const StringID&                 getDefaultAnim() const { return m_defaultAnimation; }
        const Color&                    getDefaultColor() const { return m_defaultColor; }
        bbool                           getStartFlip() const { return m_startFlip; }
        f32                             getBackZOffset() const { return m_backZOffset; }
        f32                             getFrontZOffset() const { return m_frontZOffset; }
        bbool                           getIsSynchronous() const { return m_isSynchronous; }
        bbool                           getRestoreStateAtCheckpoint() const { return m_restoreStateAtCheckpoint; }
        bbool                           getDisableVisiblePrediction() const { return m_disableVisiblePrediction; }
		const ITF_VECTOR<BoneMapping>&	getBoneList() const { return m_boneList; }
        void                            postSerialize();

    protected:

        Path                            m_animPath;
        bbool                           m_useBase;
        bbool                           m_smoothAnim;
        bbool                           m_canonizeTransitions;
        u32                             m_defaultBlendFrames;
        u32                             m_draw2D;
        StringID                        m_visualAABBanimID;
        SubAnimSet_Template             m_subAnimSet;
        StringID                        m_defaultAnimation;
        Color                           m_defaultColor;
        bbool                           m_startFlip;
        f32                             m_backZOffset;
        f32                             m_frontZOffset;
        bbool                           m_isSynchronous;
        bbool                           m_restoreStateAtCheckpoint;
        bbool                           m_disableVisiblePrediction;

		ITF_VECTOR<BoneMapping>			m_boneList;
    };

    ITF_INLINE Vec3d AnimLightComponent::getOffsettedPos() const
    {
        return m_actor->getPos() + getOffset().to3d();
    }

    ITF_INLINE f32 AnimLightComponent::getOffsettedAngle() const
    {
        return m_actor->getAngle() + getTemplate()->getRotOffset().ToRadians();
    }

    ITF_INLINE const class AnimLightComponent_Template* AnimLightComponent::getTemplate() const { return static_cast<const AnimLightComponent_Template*>(m_template); }
    ITF_INLINE bbool AnimLightComponent::needsDraw() const { return getTemplate() && !getTemplate()->getDraw2d() && !m_isProcedural; }
    ITF_INLINE bbool AnimLightComponent::needsDraw2D() const{ return getTemplate() && getTemplate()->getDraw2d() && !m_isProcedural; }
    ITF_INLINE bbool AnimLightComponent::isSynchronous() const { return getTemplate()->getIsSynchronous() || isProcedural() || m_frameEvents.size(); }
    ITF_INLINE void AnimLightComponent::resetColor() { m_currentColor = getTemplate()->getDefaultColor(); }
    ITF_INLINE const Color& AnimLightComponent::getDefaultColor() const { return getTemplate()->getDefaultColor(); }
    ITF_INLINE bbool AnimLightComponent::getUseBase() const { return getTemplate()->getUseBase(); }
    ITF_INLINE bbool AnimLightComponent::getCanonizeTransitions() const { return getTemplate()->getCanonizeTransitions(); }
    ITF_INLINE const Path& AnimLightComponent::getAnimPath() const { return getTemplate()->getAnimPath(); }
    ITF_INLINE const StringID& AnimLightComponent::getDefaultAnimation() const { return getTemplate()->getDefaultAnim(); }
    ITF_INLINE bbool AnimLightComponent::getDraw2D() const {return getTemplate()->getDraw2d();}
    ITF_INLINE bbool AnimLightComponent::getDisableVisiblePrediction() const { return ( m_disableVisiblePrediction || getTemplate()->getDisableVisiblePrediction() ); }
}

#endif // _ITF_ANIMLIGHTCOMPONENT_H_

